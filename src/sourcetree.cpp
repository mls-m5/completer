/*
 * sourcetree.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "sourcetree.h"
#include "rootsourcetree.h"
#include "tokenizer.h"
#include <fstream>
#include <map>
#include <sstream>

#define PRINT_DEBUG false
#include "common.h"

#include "binaryoperators.h"
#include "conversionstrings.h"
#include "groupingrule.h"
using namespace std;

list<SourceTree*> basicTypes;
SourceTree *autoTypePointer; //Used to identify auto pointer

static const vector<string> basicTypeNames = { "void", "int", "auto", "float",
		"double", "long", "unsigned", "signed", "long int", //Todo: Extend with all more basicTypes
		"short", "long", "char", };
static std::vector<SourceTree::DataType> typesToRegister = {
		SourceTree::ClassDeclaration, SourceTree::VariableDeclaration,
		SourceTree::FunctionDeclaration, SourceTree::FunctionDefinition,
		SourceTree::Namespace, };

//For initializing stuff
static class InitializerClass {
public:
	InitializerClass() {
		if (!basicTypes.size()) {
			for (auto &it : basicTypeNames) {
				auto typeAst = new SourceTree;
				typeAst->type(SourceTree::Type);
				typeAst->_name = it;
				basicTypes.push_back(typeAst);
				if (it == "auto") {
					autoTypePointer = basicTypes.back();
				}
			}
		}
	}
} _initializer;




template<class T>
bool findInContainer(T container, std::string str) {
	for (auto it : container) {
		if (it == str) {
			return true;
		}
	}

	return false;
}




SourceTree::SourceTree() {
}



SourceTree::~SourceTree() {
}




FilePosition SourceTree::parse(std::istream& stream,
		FilePosition fileIterator) {
	FilePosition filePosition = fileIterator;

	auto token = Tokenizer::GetNextToken(stream);
	bool templateBrackets;
	filePosition += token;
	while (token.type != Token::None) {
//		cout << "(" << filePosition.line << ":" << filePosition.column << ") " << token << endl;
		if (token.type == Token::Space) {
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			continue;
		} else if (token == ";") {
			push_back(SourceTree(token, Semicolon, this));
		} else if (token == ",") {
			push_back(SourceTree(token, ComaOperator, this));
		} else if (token == "{") {
			push_back(SourceTree(token, BraceBlock, this));
			DEBUG cout << "new braceblock" << endl;
			filePosition = back().parse(stream, filePosition);
		} else if (token == "(") {
			DEBUG cout << "new paranthesis block" << endl;
			push_back(SourceTree(token, ParanthesisBlock, this));
			filePosition = back().parse(stream, filePosition);
		} else if (token == "[") {
			DEBUG cout << "new bracket block" << endl;
			push_back(SourceTree(token, BracketBlock, this));
			filePosition = back().parse(stream, filePosition);
		} else if (token == "->" or token == "::" or token == ".") {
			back()._name += token;
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			if (token.type == Token::Word) {
				back()._name += token;
				token = Tokenizer::GetNextToken(stream);
				filePosition += token;
			}
			DEBUG cout << "new token: " << back()._name << endl;
			continue;

		} else if (token.type == Token::KeyWord and token == "template") {
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			if (token.type == Token::Space) {
				token = Tokenizer::GetNextToken(stream);
				filePosition += token;
			}
			if (token == "<") {
				DEBUG cout << "new template block" << endl;
				push_back(SourceTree(token, TemplateBlock, this));
				filePosition = back().parse(stream, filePosition);
			} else {
				cout << "expected '<'" << endl;
				break;
			}
		} else if (_type == BraceBlock and token == "}") {
			DEBUG cout << "end of scope" << endl;
			break;
		} else if (_type == ParanthesisBlock and token == ")") {
			DEBUG cout << "end of scope" << endl;
			break;
		} else if (_type == BracketBlock and token == "]") {
			DEBUG cout << "end of brackets" << endl;
			break;
		} else if (_type == TemplateBlock and token == ">") {
			DEBUG cout << "end of template" << endl;
			break;
		} else if (_type == TemplateBlock and token == ">>") {
			stream.unget();
			DEBUG cout << "end of template double >>" << endl;
			break;
		} else {
			push_back(
					SourceTree(token,
							(token.type == Token::OperatorOrPunctuator) ?
									Operator : Raw, this));
		}

		token = Tokenizer::GetNextToken(stream);
		filePosition += token;
	}

	return filePosition;
}




void FilePosition::operator +=(std::string& str) {
	for (auto c : str) {
		if (c == '\n') {
			line += 1;
			column = 1;
		} else {
			column += 1;
		}
	}
}




void intent(ostream & stream, int level) {
	for (int i = 0; i < level; ++i) {
		stream << "    ";
	}
}




void SourceTree::print(std::ostream& stream, int level) {
	intent(stream, level);
	if (_name.empty()) {
		stream << "-";
	} else {
		stream << _name;
	}
	if (isSymbol()) {
		auto fullName = getFullName();
		if (!fullName.empty()) {
			stream << " (" << fullName << ")";
		}
	}

	if (size()) {
		stream << " group ";
	}

	if (auto f = typeNameStrings.find(_type) != typeNameStrings.end()) {
		stream << " : " << typeNameStrings.at(_type);
	}

	if (pointerDepth) {
		stream << ", pointerdepth: " << pointerDepth;
	}

	if (auto tmpDataType = getType()) {
		stream << ", datatype: " << tmpDataType->getFullName();
	}

	stream << endl;
	for (auto &it : *this) {
		it.print(stream, level + 1);
	}
	if (size()) {
		intent(stream, level);
		stream << "end group" << endl << endl;
	}
}





template<class T, class U>
bool comparePattern(const T &st, const U &pattern) {
	if (pattern.size() != st.size()) {
		return false;
	}
	for (int i = 0; i < pattern.size(); ++i) {
		if (st[i]->type() != pattern[i]) {
			return false;
		}
	}
	return true;
}





void SourceTree::checkFieldForNames(iterator &it) {
	if (auto tmpType = findDataType(it->_name)) {
		DEBUG cout << "datatype " << it->_name << endl;
		it->_type = Type;
		it->dataType = tmpType;

		//Start chunking together data types that consists of several words
		auto jt = it;
		++jt;
		if (FindBasicType(it->_name)) {
			while (jt != end()) {
				if (auto tmpType2 = FindBasicType(jt->_name)) {

					it->_name += (" " + jt->_name);
					cout << "multi word data type " << it->_name << endl;
					it->dataType = tmpType2;

					jt = erase(jt);
				} else {
					break;
				}
			}
		}

		//Count pointer depth
		while (jt != end() and jt->_name == "*") {
			++it->pointerDepth;
			jt = erase(jt);
		}
	} else if (auto variable = findVariable(&it->_name)) {
		it->_type = Variable;
		it->dataType = variable->getType();
	}
}





void SourceTree::groupExpressionsWithOperators(iterator &begin, iterator &end) {
	auto start = begin;
	++start;
	if (start == end) {
		return;
	}
//	cout << "start " << start->name << endl;
//	cout << "stop " << stop->name << endl;
//	cout << "first " << start->name << endl;
//
//	cout << "check if some of these are binary operators ";
//	for (auto it = start; it != stop; ++it) {
//		cout << "'" << it->name << "' ";
//	}
//	cout << endl;
	for (const auto &it : orderedBinaryOperators) {
		for (auto op = start; op != end; ++op) {
//			cout << "checking " << op->name << " to " << it << endl;
			if (op->_name == it) {
				auto first = op;
				auto second = op;
				--first;
				++second;
				cout << "grouping '" << first->_name << "' '" << op->_name
						<< "' '" << second->_name << "'" << endl;

				if (op == start) {
					op = groupExpressions(first, second);
					op->type(BinaryOperation);
					start = op;
					++start; //The first expression will not be a operator
					begin = op; //To not mess with the sent operators
				} else {
					op = groupExpressions(first, second);
					op->type(BinaryOperation);
				}
//				print(cout, 0);
				if (second == end) {
					end = op;
					++end;
					return;
				}

//				cout << "continuing with ";
//				for (auto it = start; it != stop; ++it) {
//					cout << "'" << it->name << "' " << endl;
//				}
//				cout << endl;
			}
		}
	}
}






void SourceTree::secondPass() {
	iterator statementBeginning = begin();
#define endOfStatement() statementBeginning = it;
#define eraseFromStatement() if (statementBeginning == it) {statementBeginning = it = erase(it); } else { it = erase(it); }

	auto f = nameInterpretations.begin();

	for (auto it = begin(); it != end();) {
		if (it->size()) {
			it->secondPass();
		}
		if ((f = nameInterpretations.find(it->_name))
				!= nameInterpretations.end()) {
			//eg interprets "=" to type Equals and so on
			it->type(f->second);
		} else if (it->_name.type == Token::Digit) {
			it->type(Digit);
		} else if (it->_name.type == Token::PreprocessorCommand) {
			DEBUG cout << "skipping preprocessor command" << endl;
			eraseFromStatement()
			continue;
		} else if (it->_name == "static") {
			DEBUG cout << "do not handle static keyword.. skipping" << endl;
			eraseFromStatement()
			continue;
		}
		else if (it->type() == Raw) {
			checkFieldForNames(it);
		}

		//Load in templates in beginning of statement
//		if (unprocessedExpressions.empty() and it->type == TemplateBlock){
//			cout << "template block... do something with this" << endl;
//		}

		if (it->type() == Semicolon or it->type() == ComaOperator) {
			bool containsOperators = false;
			auto statementEnd = it;
			++statementEnd;
			for (auto it2 = statementBeginning; it2 != statementEnd; ++it2){
				if (it2->type() == Operator or it2->type() == Equals) {
					containsOperators = true;
					break;
				}
			}
			if (containsOperators) {
				groupExpressionsWithOperators(statementBeginning, it);
			}
			it = erase(it); //Skip semicolon
			endOfStatement()
			DEBUG cout << "end of statement" << endl;
			continue;
		} else {
			++it;
			for (auto &pattern : patternInterpretations) {
				if (pattern.tryGroupExpressions(this, statementBeginning, it)) {
					DEBUG cout << "grouped pattern " << pattern._groupName << endl;
					if (pattern.needSemicolon == false) {
						//Some expressions that do not need semicolon
						endOfStatement();
					}
					break;
				}
			}
			continue; //To avoid doing ++it twice
		}
		++it;
	}
}




SourceTree* SourceTree::findDataType(std::string &name) {
	if (name.empty()) {
		return 0;
	}
	SourceTree* st = 0;
	if ((st = FindBasicType(name))) {
		return st;
	}
	for (auto &it : *this) {
		if (it.type() == Type or it.type() == ClassDeclaration) {
			if (it.getLocalName() == name or it.getFullName() == name) {
				return &it;
			}
		}
	}
	if (_parent) {
		st = _parent->findDataType(name);
	}
	if (this->_name == name) {
		return this;
	}
	return st;
}




SourceTree::iterator SourceTree::groupExpressions(SourceTree::iterator first,
		SourceTree::iterator last) {
	SourceTree st;
	++last; //last is not included by default

	auto ret = insert(first, st);

	ret->splice(ret->begin(), *this, first, last);

	ret->setParent(this);

	return ret;
}




std::string SourceTree::getFullName() const {
	if (not _parent) {
		return getLocalName();
	}
	auto parentName = _parent->getFullName();
	if (parentName.empty()) {
		return getLocalName();
	} else {
		auto localName = getLocalName();
		if (localName.empty()) {
			return parentName;
		} else {
			return parentName + "::" + getLocalName();
		}
	}
}





std::string SourceTree::getLocalName() const {
	for (auto &it : *this) {
		if (it.type() == DeclarationName or it.type() == DefinitionName) {
			return it._name;
		}
	}
	if (_type == Type) {
		return _name;
	}
	return "";
}





void SourceTree::setParent(SourceTree* parent) {
	this->_parent = parent;
	if (parent) {
		this->_root = parent->_root;
	}
	for (auto &it : *this) {
		it.setParent(this);
	}
}






SourceTree* SourceTree::FindBasicType(std::string& name) {
	if (name.empty()) {
		return 0;
	}
	for (auto &it : basicTypes) {
		if (it->_name == name) {
			return it;
		}
	}
	return 0;
}





SourceTree* SourceTree::findVariable(const std::string* name) {
	for (auto &it : *this) {
		if (it.type() == VariableDeclaration) {
			//Todo return the latest definition
			if (it.getLocalName() == *name) {
				return &it;
			}
		} else if (it.type() == BinaryOperation) {
//			cout << "searching " << it.front().getLocalName() << endl;
			if (it.size() > 0) {
				if (auto variable = it.findVariable(name)) {
					return variable;
				}
			}
			//Todo find global variables
		}
	}
	return 0;
}





SourceTree* SourceTree::findNameSpace(std::string& name) {
	for (auto &it : *this) {
		if (it.type() == Namespace) {
			if (it.getLocalName() == name) { // or it.getFullName() == name){ //Todo: maybe getFullName is unnessecary? It takes a lots of resources anyway
				return &it;
			}
		}
	}
	return 0;
}





SourceTree* SourceTree::findBranchByType(DataType type) {
	for (auto &it : *this) {
		if (it.type() == type) {
			return &it;
		}
	}
	return 0;
}





SourceTree* SourceTree::getType() {
	auto returnDataType = dataType;
	if (!returnDataType) {
		if (auto typeBranch = findBranchByType(Type)) {
			if (typeBranch->dataType == autoTypePointer){
				if (_parent) {
					if (_parent->type() == BinaryOperation) {
						auto assignmentValue = &_parent->back();
						if (assignmentValue) {
							return assignmentValue->dataType;
						}
					}
					else{
						cout << "auto is used, but does not refer to a variable with a type" << endl;
						return 0;
					}
				}
			}
			else {
				return typeBranch->dataType;
			}
		}
	}
	if (returnDataType == autoTypePointer) {
//		cout << "reference to auto " << endl;
	}
	return returnDataType;
}




std::string SourceTree::getTypeName() const {
	auto typeName = typeNameStrings.find(type());
	if (typeName == typeNameStrings.end()) {
		return "unnamed type";
	} else {
		return typeName->second;
	}
}




void SourceTree::type(DataType t) {
	_type = t;

	if (_root) {
		for (auto it : typesToRegister) {
			if (it == t) {
				if (_isSymbol) {
					return; //symbol is already registered
				} else {
					_isSymbol = true;
					_root->insertSymbol(this);
					break;
				}
			}
		}
	}
}

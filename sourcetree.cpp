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

#define PRINT_DEBUG true
#include "common.h"

#include "binaryoperators.h"
#include "conversionstrings.h"
#include "replacementrule.h"
using namespace std;

list<SourceTree*> basicTypes;
SourceTree *autoTypePointer; //Used to identify auto pointer

static const vector<string> basicTypeNames = {
		"void",
		"int",
		"auto",
		"float",
		"double",
		"long",
		"unsigned",
		"signed",
		"long int", //Todo: Extend with all more basicTypes
		"short",
		"long",
		"char",
};


//For initializing stuff
static class InitializerClass{
public:
	InitializerClass(){
		if (!basicTypes.size()){
			for (auto &it: basicTypeNames){
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

template <class T>
bool findInContainer(T container, std::string str){
	for (auto it: container){
		if (it == str){
			return true;
		}
	}

	return false;
}





SourceTree::SourceTree() {
}





SourceTree::~SourceTree() {
}





FilePosition SourceTree::parse(std::istream& stream, FilePosition fileIterator) {
	FilePosition filePosition = fileIterator;

	auto token = Tokenizer::GetNextToken(stream);
	bool templateBrackets;
	filePosition += token;
	while (token.type != Token::None){
//		cout << "(" << filePosition.line << ":" << filePosition.column << ") " << token << endl;
		if (token.type == Token::Space){
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			continue;
		}
		else if (token == ";"){
			push_back(SourceTree(token, Semicolon, this));
		}
		else if (token == ","){
			push_back(SourceTree(token, ComaOperator, this));
		}
		else if (token == "{"){
			push_back(SourceTree(token, BraceBlock, this));
			cout << "new braceblock" << endl;
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "("){
			cout << "new paranthesis block" << endl;
			push_back(SourceTree(token, ParanthesisBlock, this));
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "["){
			cout << "new bracket block" << endl;
			push_back(SourceTree(token, BracketBlock, this));
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "->" or token == "::" or token == "."){
			back()._name += token;
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			if (token.type == Token::Word) {
				back()._name += token;
				token = Tokenizer::GetNextToken(stream);
				filePosition += token;
			}
			cout << "new token: " << back()._name << endl;
			continue;

		}
		else if (token.type == Token::KeyWord and token == "template"){
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			if (token.type == Token::Space){
				token = Tokenizer::GetNextToken(stream);
				filePosition += token;
			}
			if (token == "<"){
				cout << "new template block" << endl;
				push_back(SourceTree(token, TemplateBlock, this));
				filePosition = back().parse(stream, filePosition);
			}
			else {
				cout << "expected '<'" << endl;
				break;
			}
		}
		else if (_type == BraceBlock and token == "}"){
			cout << "end of scope" << endl;
			break;
		}
		else if (_type == ParanthesisBlock and token == ")"){
			cout << "end of scope" << endl;
			break;
		}
		else if (_type == BracketBlock and token == "]"){
			cout << "end of brackets" << endl;
			break;
		}
		else if (_type == TemplateBlock and token == ">"){
			cout << "end of template" << endl;
			break;
		}
		else if (_type == TemplateBlock and token == ">>"){
			stream.unget();
			cout << "end of template double >>" << endl;
			break;
		}
		else{
			push_back(SourceTree(token,
					 (token.type == Token::OperatorOrPunctuator)? Operator: Raw, this));
		}

		token = Tokenizer::GetNextToken(stream);
		filePosition += token;
	}

	return filePosition;
}





void FilePosition::operator +=(std::string& str) {
	for (auto c: str){
		if (c == '\n'){
			line += 1;
			column = 1;
		}
		else {
			column += 1;
		}
	}
}





void intent(ostream & stream, int level){
	for (int i = 0; i < level; ++i){
		stream << "    ";
	}
}





void SourceTree::print(std::ostream& stream, int level) {

	intent(stream, level);
	if (_name.empty()){
		stream << "-";
	}
	else {
		stream << _name;
	}
	auto fullName = getFullName();
	if (!fullName.empty()){
		stream << " (" << fullName << ")";
	}


	if (auto f = typeNameStrings.find(_type) != typeNameStrings.end()){
		stream << " : " << typeNameStrings.at(_type);
	}

	if (pointerDepth) {
		stream << ", pointerdepth: " << pointerDepth;
	}

	if (auto tmpDataType = getType()) {
		if (tmpDataType) {
			stream << ", datatype: " << tmpDataType->getFullName();
		}
	}

	if (size()){
//		intent(stream, level);
		stream << " : start group ";

		switch (_type){
		case ParanthesisBlock:
			stream << "()";
			break;
		case BraceBlock:
			stream << "{}";
			break;
		case BracketBlock:
			stream << "[]";
			break;
		case VariableDeclaration:
			stream << "variable definition";
			break;
		case ControlStatement:
			stream << front()._name;
			break;
		case AssignmentStatement:
			stream << "assignment";
			break;
		case ClassDeclaration:
			stream << "class";
			break;
		case StructDeclaration:
			stream << "struct";
			break;
		case TemplateBlock:
			stream << "template";
			break;
		case FunctionDeclaration:
			stream << "function declaration";
			break;
		case FunctionDefinition:
			stream << "function definition";
			break;
		case LambdaFunction:
			stream << "lambda function";
			break;
		default:
			stream << "type " << _type;
			break;
		}
	}
	stream << endl;
	for (auto &it: *this){
		it.print(stream, level + 1);
	}
	if (size()){
		intent(stream, level);
		stream << "end group" << endl << endl;
	}
}





template <class T, class U>
bool comparePattern(const T &st, const U &pattern){
	if (pattern.size() != st.size()){
		return false;
	}
	for (int i = 0; i < pattern.size(); ++i){
		if (st[i]->type() != pattern[i]){
			return false;
		}
	}
	return true;
}






void SourceTree::checkFieldForNames(iterator &it) {
	if (auto tmpType = findDataType(it->_name)){
		cout << "datatype " << it->_name << endl;
		it->_type = Type;
		it->dataType = tmpType;

		//Start chunking together data types that consists of several words
		auto jt = it;
		++jt;
		if (FindBasicType(it->_name)){
			while (jt != end()){
				if (auto tmpType2 = FindBasicType(jt->_name)){

					it->_name += (" " + jt->_name);
					cout << "multi word data type " << it->_name << endl;
					it->dataType = tmpType2;

					jt = erase(jt);
				}
				else {
					break;
				}
			}
		}

		//Count pointer depth
		while (jt != end() and jt->_name == "*"){
			++it->pointerDepth;
			jt = erase(jt);
		}
	}
	else if (auto variable = findVariable(&it->_name)){
		it->_type = Variable;
		it->dataType = variable->getType();
	}
}






void SourceTree::groupExpressionsWithOperators(iterator to, int count) {
	if (count < 3) {
		return;
	}
	auto stop = to;
	auto start = stop;
	for (int i = 0; i < count -2; ++i) {
		--start;
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
	for (const auto &it: orderedBinaryOperators) {
		for (auto op = start; op != stop; ++op) {
//			cout << "checking " << op->name << " to " << it << endl;
			if (op->_name == it) {
				auto first = op;
				auto last = op;
				--first;
				++last;
				cout << "grouping '" << first->_name << "' '" << op->_name << "' '" << last->_name << "'" << endl;

				if (op == start) {
					op = groupExpressions(first, last);
					op->type(BinaryOperation);
					start = op;
					++start; //The first expression will not be a operator
				}
				else {
					op = groupExpressions(first, last);
					op->type(BinaryOperation);
				}
//				print(cout, 0);
				if (last == stop){
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
	vector<SourceTree*> unprocessedExpressions; //A list of expressions that is not yet used
	unprocessedExpressions.reserve(5);
	auto f = nameInterpretations.begin();

	for (auto it = begin(); it != end();){
		if (it->size()){
			it->secondPass();
		}
		if ((f = nameInterpretations.find(it->_name)) != nameInterpretations.end()){
			//eg interprets "=" to type Equals and so on
			it->type(f->second);
		}
		else if (it->_name.type == Token::Digit){
			it->type(Digit);
		}
		else if (it->_name.type == Token::PreprocessorCommand){
			cout << "skipping preprocessor command" << endl;
			it = erase(it);
			continue;
		}
		else if (it->_name == "static") {
			cout << "do not handle static keyword.. skipping" << endl;
			it = erase(it);
			continue;
		}
//		else if (it->type == BraceBlock) {
//			//Do nothing this was to avoid braces to be interpreted as variable declarations
//		}
		else if (it->type() == Raw) {
			checkFieldForNames(it);
		}

		//Load in templates in beginning of statement
//		if (unprocessedExpressions.empty() and it->type == TemplateBlock){
//			cout << "template block... do something with this" << endl;
//		}
//		else{
			unprocessedExpressions.push_back(&*it);
//		}


		//Some expressions that do not need semicolon
		if (it->type() == Namespace) {
			unprocessedExpressions.clear();
		}
		if (it->type() == Semicolon or it->type() == ComaOperator){
			//Todo: Handle this
//			cout << "size : " << unprocessedExpressions.size() << endl;
//			for (auto &it: unprocessedExpressions) {
//				cout << "'" << it->name << "' ";
//			}
//			cout << endl;
			bool containsOperators = false;
			for (auto it2: unprocessedExpressions){
				if (it2->type() == Operator or it2->type() == Equals) {
					containsOperators = true;
				}
			}
			if (containsOperators) {
				auto tmpit = it;
				--tmpit;
				groupExpressionsWithOperators(tmpit, unprocessedExpressions.size() - 1);
			}
			unprocessedExpressions.clear();
			cout << "end of statement" << endl;
		}
		else {
			for (auto &pattern: patternInterpretations){
				if (tryGroupExpressions(it, unprocessedExpressions,
						pattern.first, pattern.second, pattern.replacementPattern)){
					cout << "grouped pattern " << pattern.second << endl;
					break;
				}
			}
		}

		++it;
	}
}





SourceTree* SourceTree::findDataType(std::string &name) {
	if (name.empty()){
		return 0;
	}
	SourceTree* st = 0;
	if ((st = FindBasicType(name))){
		return st;
	}
	for (auto &it: *this){
		if (it.type() == Type or it.type() == ClassDeclaration){
			if (it.getLocalName() == name or it.getFullName() == name){
				return &it;
			}
		}
	}
	if (_parent) {
		st = _parent->findDataType(name);
	}
	if (this->_name == name){
		return this;
	}
	return st;
}





SourceTree::iterator SourceTree::groupExpressions(SourceTree::iterator first, SourceTree::iterator last) {
	SourceTree st;
	++last; //last is not included by default

	auto ret = insert(first, st);

	ret->splice(ret->begin(), *this, first, last);

	ret->setParent(this);

	return ret;
}





SourceTree::iterator SourceTree::groupExpressions(SourceTree::iterator last,
		int count) {
	auto first = last;
	for (int i = 1; i < count; ++i){
		--first;
	}
	return groupExpressions(first, last);
}





bool SourceTree::tryGroupExpressions(iterator &it, std::vector<SourceTree*> &unprocessedExpressions,
		const std::vector<DataType> &pattern, DataType resultingType, const std::vector<DataType> &replacementPattern) {
	if (comparePattern(unprocessedExpressions, pattern)){
		it = groupExpressions(it, pattern.size());
		if (pattern.size() == replacementPattern.size()){
			//Replace the identifier pattern with a new one
			//For example interprets which fields are name fields etc
			auto replacementIterator = replacementPattern.begin();
			for (auto jt = it->begin(); jt != it->end(); ++jt, ++replacementIterator){
				jt->type(*replacementIterator);
			}
		}
		it->type(resultingType);
		unprocessedExpressions.clear();
		unprocessedExpressions.push_back(&*it);
		return true;
	}
	return false;
}





std::string SourceTree::getFullName() const {
	if (not _parent) {
		return getLocalName();
	}
	auto parentName = _parent->getFullName();
	if (parentName.empty()){
		return getLocalName();
	}
	else{
		auto localName = getLocalName();
		if (localName.empty()){
			return parentName;
		}
		else {
			return parentName + "::" + getLocalName();
		}
	}
}





std::string SourceTree::getLocalName() const {
	for (auto &it: *this){
		if (it.type() == DeclarationName or it.type() == DefinitionName){
			return it._name;
		}
	}
	return "";
}




void SourceTree::setParent(SourceTree* parent) {
	this->_parent = parent;
	if (parent) {
		this->_root = parent->_root;
	}
	for (auto &it: *this){
		it.setParent(this);
	}
}





SourceTree* SourceTree::FindBasicType(std::string& name) {
	if (name.empty()){
		return 0;
	}
	for (auto &it: basicTypes){
		if (it->_name == name){
			return it;
		}
	}
	return 0;
}





std::list<SourceTree *> SourceTree::findExpressions(std::list<Token> tokens) {
	std::list<SourceTree *> ret;
	if (tokens.size() >= 2) {
		auto name = tokens.front();
		tokens.pop_front();
		auto operatorName = tokens.back(); //Todo check that it is :: -> or something
		tokens.pop_front();

		auto datatypeFound = findDataType(name);
		if (datatypeFound) {
//			ret.push_back(datatypeFound);
			auto codeBlock = datatypeFound->findBranchByType(BraceBlock);
			if (codeBlock) {
				ret.push_back(codeBlock);
			}
			if (!tokens.empty()){
				auto found = codeBlock->findExpressions(tokens);
				ret.splice(ret.end(), found);
			}
		}

//		auto namespace = findNameSpace(name);
		auto namespaceFound = findNameSpace(name);
		if (namespaceFound) {
			auto codeblock = namespaceFound->findBranchByType(BraceBlock);
			ret.push_back(codeblock);
//			codeblock->print(cout, 0);
			if (!tokens.empty()){
				auto found = codeblock->findExpressions(tokens);
				ret.splice(ret.end(), found);
			}
		}

		auto variableFound = findVariable(&name);
		if (variableFound) {
//			ret.push_back(variableFound);
			if (auto datatypeFound = variableFound->getType()){
				if (auto codeBlock = datatypeFound->findBranchByType(BraceBlock)){
					ret.push_back(codeBlock);
					if (!tokens.empty()){
						auto found = variableFound->findExpressions(tokens);
						ret.splice(ret.end(), found);
					}
				}
			}
		}
	}
//	else if (tokens.size() == 2){
//		//this should not happend
//		cerr << "trange error __FILE__ << ":" << __LINE__ << endl;
//	}
	else if (tokens.size() == 0){
		//Let the calling function do the rest
	}
	return ret;
}





std::list<SourceTree*> SourceTree::completeExpression(std::string name) {
	std::list<SourceTree *> ret;

	auto isBeginningSame = [] (std::string word, std::string beginning) {
		if (beginning.size() > word.size()){
			return false;
		}
		if (beginning.size() == word.size()) {
			return word == beginning;
		}
		for (int i = 0; i < beginning.size(); ++i){
			if (beginning[i] != word[i]) {
				return false;
			}
		}

		return true;
	};

	istringstream ss(name);
	std::list<Token> tokens;
	while (ss){
		tokens.push_back(Tokenizer::GetNextToken(ss));
	}
	DEBUG cout << "tokens to complete with:" << endl;
	for (auto &it: tokens){
		DEBUG cout << "(" << it << "), ";
	}
	DEBUG cout << endl;

	string completeName = tokens.back();
	tokens.pop_back();

	list<SourceTree*> possibleContainers = {this};
	if (tokens.size() >= 2) {
		auto baseExpressions = findExpressions(tokens);
		possibleContainers.splice(possibleContainers.end(), baseExpressions);
	}

	for (auto &match: possibleContainers){
		for (auto &it: *match){
//			auto branchName = it.getFullName();
			auto branchName = it.getLocalName();
			DEBUG cout << branchName << "(" << it.getFullName() <<
					") compared to " << completeName << " (" << name << ")" << endl;
			if (isBeginningSame(branchName, completeName)){
				DEBUG cout << branchName << " match to '" << completeName << "'" << endl;
				ret.push_back(&it);
			}
		}
	}

	return ret;
}





SourceTree* SourceTree::findVariable(const std::string* name) {
	for (auto &it: *this) {
		if (it.type() == VariableDeclaration) {
			 //Todo return the latest definition
			if (it.getLocalName() == *name) { // or it.getFullName() == *name) {
				return &it;
			}
		}
		else if (it.type() == BinaryOperation) {
//			cout << "searching " << it.front().getLocalName() << endl;
			if (it.size() > 0) {
				if (auto variable = it.findVariable(name)) {
					return variable;
				}
			}
			//Todo find global variables
		}
	}
//	if (parent) { //Was never used
//		if (auto variable = parent->findVariable(name)){
//			return variable;
//		}
//	}
	return 0;
}





SourceTree* SourceTree::findNameSpace(std::string& name) {
	for (auto &it: *this) {
		if (it.type() == Namespace) {
			if (it.getLocalName() == name) { // or it.getFullName() == name){ //Todo: maybe getFullName is unnessecary? It takes a lots of resources anyway
				return &it;
			}
		}
	}
	return 0;
}





SourceTree* SourceTree::findBranchByType(DataType type) {
	for (auto &it: *this) {
		if (it.type() == type) {
			return &it;
		}
	}
	return 0;
}





SourceTree* SourceTree::getType(){
	auto returnDataType = dataType;
	if (!returnDataType) {
		if (auto typeBranch = findBranchByType(Type)){
			return typeBranch->dataType;
		}
	}
	if (returnDataType == autoTypePointer) {
//		cout << "reference to auto " << endl;
	}
	return returnDataType;
}




std::string SourceTree::getTypeName() const {
	auto typeName = typeNameStrings.find(type());
	if (typeName == typeNameStrings.end()){
		return "unnamed type";
	}
	else {
		return typeName->second;
	}
}

void SourceTree::type(DataType t) {
	_type = t;

	if (_root) {
		for (auto it: typesToRegister) {
			if (it == t) {
				if (_isSymbol) {
					return; //symbol is alreadyregistered
				}
				else {
					_root->insertSymbol(this);
				}
			}
		}
	}
}

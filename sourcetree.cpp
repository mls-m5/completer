/*
 * sourcetree.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "sourcetree.h"
#include "tokenizer.h"
#include <fstream>
#include <map>
#include <sstream>

#define PRINT_DEBUG true
#define DEBUG if(PRINT_DEBUG)

#include "binaryoperators.h"
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

vector<string> controlStatements = {
		"if",
		"while",
		"for",
		"switch",
};

//For converting string to type
map<string, SourceTree::DataType> nameInterpretations = {
		{"if", SourceTree::ControlStatementKeyword},
		{"while", SourceTree::ControlStatementKeyword},
		{"for", SourceTree::ControlStatementKeyword},
		{"switch", SourceTree::ControlStatementKeyword},
		{"class", SourceTree::ClassKeyword},
		{"template", SourceTree::TemplateKeyword},
		{"struct", SourceTree::StructKeyword},
		{"return", SourceTree::ReturnKeyword},
		{"new", SourceTree::NewKeyword},
		{"delete", SourceTree::DeleteKeyword},
		{"namespace", SourceTree::NamespaceKeyWord},

		{"+", SourceTree::Operator},
		{"++", SourceTree::Operator},
		{"-", SourceTree::Operator},
		{"--", SourceTree::Operator},
		{",", SourceTree::ComaOperator},
		{";", SourceTree::Semicolon},
		{"=", SourceTree::Equals},
		{"::", SourceTree::ScopeResolution},
		{"->", SourceTree::ElementSelectionThroughPointer},
		{".", SourceTree::ElementSelectionThroughPointer},

};

//For converting type to string
const map<SourceTree::DataType, string> typeNameStrings = {
		{SourceTree::Equals, "equals"},
		{SourceTree::Raw, "raw"},
		{SourceTree::ClassKeyword, "class keyword"},
		{SourceTree::Type, "type"},
		{SourceTree::ControlStatementKeyword, "control statement keyword"},
		{SourceTree::ReturnKeyword, "return keyword"},
		{SourceTree::Operator, "operator"},
		{SourceTree::DeclarationName, "declaration name"},
		{SourceTree::DefinitionName, "name"},
		{SourceTree::NamespaceKeyWord, "namespace keyword"},
		{SourceTree::Namespace, "namespace"},
		{SourceTree::Variable, "variable"},
		{SourceTree::Digit, "digit"},
		{SourceTree::BinaryOperation, "binary operation"},
};

typedef const vector<SourceTree::DataType> patternType;

patternType controlStatementPattern = {
		SourceTree::ControlStatementKeyword,
		SourceTree::ParanthesisBlock,
		SourceTree::BraceBlock
};

patternType anonymousClassDeclarationPattern = {
		SourceTree::ClassKeyword,
		SourceTree::BraceBlock
};

patternType variableDeclarationAndAssignmentPattern = {
		SourceTree::VariableDeclaration,
		SourceTree::Equals,
};

struct replacementRule {
	patternType first;
	SourceTree::DataType second;
	patternType replacementPattern;
};

std::vector<replacementRule> patternInterpretations = {
		{{
				SourceTree::Type,
				SourceTree::Raw,
		},
				SourceTree::VariableDeclaration,
				{
						SourceTree::Type,
						SourceTree::DefinitionName
				}
		},

		{anonymousClassDeclarationPattern, SourceTree::ClassDeclaration},
		{{

				SourceTree::ClassKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::ClassDeclaration,
				{
						SourceTree::ClassKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},
		//Templated class
		{{
				SourceTree::TemplateBlock,
				SourceTree::ClassKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::ClassDeclaration,
				{
//						SourceTree::TemplateBlock,
//						SourceTree::ClassKeyword,
//						SourceTree::DeclarationName,
//						SourceTree::BraceBlock,
						SourceTree::TemplateBlock,
						SourceTree::ClassKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock,
				}
		},

		{{

				SourceTree::StructKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::StructDeclaration,
				{
						SourceTree::StructKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},

		//Templated struct
		{{
				SourceTree::TemplateBlock,
				SourceTree::StructKeyword,
				SourceTree::Raw,
				SourceTree::BraceBlock
		},
				SourceTree::StructDeclaration,
				{
						SourceTree::TemplateBlock,
						SourceTree::StructKeyword,
						SourceTree::DeclarationName,
						SourceTree::BraceBlock
				}
		},

		{controlStatementPattern, SourceTree::ControlStatement},

		//Namespace
		{{
				SourceTree::NamespaceKeyWord,
				SourceTree::Raw,
				SourceTree::BraceBlock,
		},
				SourceTree::Namespace,
				{
						SourceTree::NamespaceKeyWord,
						SourceTree::DefinitionName,
						SourceTree::BraceBlock,
				}
		},

//		{{
//				SourceTree::VariableDeclaration,
//				SourceTree::Equals
//		}, //Probably another pattern in the future
//				SourceTree::AssignmentStatement
//		},

		{{
			SourceTree::VariableDeclaration,
			SourceTree::ParanthesisBlock,
		},
			SourceTree::FunctionDeclaration
		},

		{{
			SourceTree::FunctionDeclaration,
			SourceTree::BraceBlock,
		},
			SourceTree::FunctionDefinition,
		},


		{{
			SourceTree::BracketBlock, //Todo make sure this is identified even when it is not at an end of a line
			SourceTree::ParanthesisBlock,
			SourceTree::BraceBlock,
		},
			SourceTree::LambdaFunction,
		},
};

//For initializing stuff
static class InitializerClass{
public:
	InitializerClass(){
		if (!basicTypes.size()){
			for (auto &it: basicTypeNames){
				auto typeAst = new SourceTree;
				typeAst->type = SourceTree::Type;
				typeAst->name = it;
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
			push_back(SourceTree(token, Semicolon));
		}
		else if (token == ","){
			push_back(SourceTree(token, ComaOperator));
		}
		else if (token == "{"){
			push_back(SourceTree(token, BraceBlock));
			cout << "new braceblock" << endl;
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "("){
			cout << "new paranthesis block" << endl;
			push_back(SourceTree(token, ParanthesisBlock));
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "["){
			cout << "new bracket block" << endl;
			push_back(SourceTree(token, BracketBlock));
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "->" or token == "::" or token == "."){
			back().name += token;
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			if (token.type == Token::Word) {
				back().name += token;
				token = Tokenizer::GetNextToken(stream);
				filePosition += token;
			}
			cout << "new token: " << back().name << endl;
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
				push_back(SourceTree(token, TemplateBlock));
				filePosition = back().parse(stream, filePosition);
			}
			else {
				cout << "expected '<'" << endl;
				break;
			}
		}
		else if (type == BraceBlock and token == "}"){
			cout << "end of scope" << endl;
			break;
		}
		else if (type == ParanthesisBlock and token == ")"){
			cout << "end of scope" << endl;
			break;
		}
		else if (type == BracketBlock and token == "]"){
			cout << "end of brackets" << endl;
			break;
		}
		else if (type == TemplateBlock and token == ">"){
			cout << "end of template" << endl;
			break;
		}
		else if (type == TemplateBlock and token == ">>"){
			stream.unget();
			cout << "end of template double >>" << endl;
			break;
		}
		else{
			push_back(SourceTree(token,
					 (token.type == Token::OperatorOrPunctuator)? Operator: Raw));
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
	if (name.empty()){
		stream << "-";
	}
	else {
		stream << name;
	}
	auto fullName = getFullName();
	if (!fullName.empty()){
		stream << " (" << fullName << ")";
	}


	if (auto f = typeNameStrings.find(type) != typeNameStrings.end()){
		stream << " : " << typeNameStrings.at(type);
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

		switch (type){
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
			stream << front().name;
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
			stream << "type " << type;
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
		if (st[i]->type != pattern[i]){
			return false;
		}
	}
	return true;
}


void SourceTree::checkFieldForNames(iterator &it) {
	if (auto tmpType = findDataType(it->name)){
		cout << "datatype " << it->name << endl;
		it->type = Type;
		it->dataType = tmpType;

		//Start chunking together data types that consists of several words
		auto jt = it;
		++jt;
		if (FindBasicType(it->name)){
			while (jt != end()){
				if (auto tmpType2 = FindBasicType(jt->name)){

					it->name += (" " + jt->name);
					cout << "multi word data type " << it->name << endl;
					it->dataType = tmpType2;

					jt = erase(jt);
				}
				else {
					break;
				}
			}
		}

		//Count pointer depth
		while (jt != end() and jt->name == "*"){
			++it->pointerDepth;
			jt = erase(jt);
		}
	}
	else if (auto variable = findVariable(&it->name)){
		it->type = Variable;
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
			if (op->name == it) {
				auto first = op;
				auto last = op;
				--first;
				++last;
				cout << "grouping '" << first->name << "' '" << op->name << "' '" << last->name << "'" << endl;

				if (op == start) {
					op = groupExpressions(first, last);
					op->type = BinaryOperation;
					start = op;
					++start; //The first expression will not be a operator
				}
				else {
					op = groupExpressions(first, last);
					op->type = BinaryOperation;
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
		if ((f = nameInterpretations.find(it->name)) != nameInterpretations.end()){
			//eg interprets "=" to type Equals and so on
			it->type = f->second;
		}
		else if (it->name.type == Token::Digit){
			it->type = Digit;
		}
		else if (it->name.type == Token::PreprocessorCommand){
			cout << "skipping preprocessor command" << endl;
			it = erase(it);
			continue;
		}
		else if (it->name == "static") {
			cout << "do not handle static keyword.. skipping" << endl;
			it = erase(it);
			continue;
		}
//		else if (it->type == BraceBlock) {
//			//Do nothing this was to avoid braces to be interpreted as variable declarations
//		}
		else if (it->type == Raw) {
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
		if (it->type == Namespace) {
			unprocessedExpressions.clear();
		}
		if (it->type == Semicolon or it->type == ComaOperator){
			//Todo: Handle this
//			cout << "size : " << unprocessedExpressions.size() << endl;
//			for (auto &it: unprocessedExpressions) {
//				cout << "'" << it->name << "' ";
//			}
//			cout << endl;
			bool containsOperators = false;
			for (auto it2: unprocessedExpressions){
				if (it2->type == Operator or it2->type == Equals) {
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
		if (it.type == Type or it.type == ClassDeclaration){
			if (it.getLocalName() == name or it.getFullName() == name){
				return &it;
			}
		}
	}
	if (parent) {
		st = parent->findDataType(name);
	}
	if (this->name == name){
		return this;
	}
	return st;
}

SourceTree::iterator SourceTree::groupExpressions(SourceTree::iterator first, SourceTree::iterator last) {
	SourceTree st;
	++last; //last is not included by default

	auto ret = insert(first, st);
//	auto ret = first;
//	--ret;
	//splice moves the elements enstead of recreating and deleting
	//this way it is possible to keep references (hopefully)
	ret->splice(ret->begin(), *this, first, last);

//	st.insert(st.begin(), first, last);
//
//	insert(first, st);
//	auto ret = first;
//	--ret;
//	erase(first, last);

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
				jt->type = *replacementIterator;
			}
		}
		it->type = resultingType;
		unprocessedExpressions.clear();
		unprocessedExpressions.push_back(&*it);
		return true;
	}
	return false;
}

std::string SourceTree::getFullName() const {
	if (not parent) {
		return getLocalName();
	}
	auto parentName = parent->getFullName();
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
		if (it.type == DeclarationName or it.type == DefinitionName){
			return it.name;
		}
	}
	return "";
}

void SourceTree::setParent(SourceTree* parent) {
	this->parent = parent;
	for (auto &it: *this){
		it.setParent(this);
	}
}

SourceTree* SourceTree::FindBasicType(std::string& name) {
	if (name.empty()){
		return 0;
	}
	for (auto &it: basicTypes){
		if (it->name == name){
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
		if (it.type == VariableDeclaration) {
			 //Todo return the latest definition
			if (it.getLocalName() == *name) { // or it.getFullName() == *name) {
				return &it;
			}
		}
		else if (it.type == BinaryOperation) {
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
		if (it.type == Namespace) {
			if (it.getLocalName() == name) { // or it.getFullName() == name){ //Todo: maybe getFullName is unnessecary? It takes a lots of resources anyway
				return &it;
			}
		}
	}
	return 0;
}

SourceTree* SourceTree::findBranchByType(DataType type) {
	for (auto &it: *this) {
		if (it.type == type) {
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

SourceTree SourceTree::CreateFromString(std::string source) {
	SourceTree sourceTree;
	istringstream ss(source);
	sourceTree.parse(ss);
	sourceTree.secondPass();
	DEBUG sourceTree.print(cout, 0);
	return sourceTree; //This would be a waste if there was no move constructor :)
}

std::string SourceTree::getTypeName() const {
	return typeNameStrings.at(type);
}

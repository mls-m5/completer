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

using namespace std;

list<SourceTree*> basicTypes;

vector<string> basicTypeNames = {
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

		{"+", SourceTree::Operator},
		{"++", SourceTree::Operator},
		{"-", SourceTree::Operator},
		{"--", SourceTree::Operator},
		{",", SourceTree::ComaOperator},
		{";", SourceTree::Semicolon},
		{"=", SourceTree::Equals},

};

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

		{{
				SourceTree::VariableDeclaration,
				SourceTree::Equals
		}, //Probably another pattern in the future
				SourceTree::AssignmentStatement},

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
			}
		}
	}
} _initializer;

bool isDataType(string &name){
	for (auto &it: basicTypes){
		if (it->name == name){
			return true;
		}
	}
	return false;
}

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
			push_back(SourceTree());
			back().type = (token.type == Token::OperatorOrPunctuator)? Operator: Raw;
			back().name = token;
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

	if (auto f = typeNameStrings.find(type) != typeNameStrings.end()){
		stream << " : " << typeNameStrings.at(type);
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
		else if (auto tmpType = findDataType(it->name)){
			cout << "datatype " << it->name << endl;
			it->type = Type;
			it->dataType = tmpType;

			//Start chunking togther datatypes that consists of several words
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
//			it->dataType = findDataType(it->name);

			//Count pointer depth
			while (jt != end() and jt->name == "*"){
				++it->pointerDepth;
				jt = erase(jt);
			}
		}

		//Load in templates in beginning of statement
//		if (unprocessedExpressions.empty() and it->type == TemplateBlock){
//			cout << "template block... do something with this" << endl;
//		}
//		else{
			unprocessedExpressions.push_back(&*it);
//		}

		if (it->type == Semicolon or it->type == ComaOperator){
			//Todo: Handle this
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
	SourceTree* st;
	if ((st = FindBasicType(name))){
		return st;
	}
	for (auto &it: *this){
		if (it.type == Type){
			if (it.name == name){
				return &it;
			}
		}
	}
	if (this->name == name){
		return this;
	}
	return 0;
}

SourceTree::iterator SourceTree::groupExpressions(SourceTree::iterator first, SourceTree::iterator last) {
	SourceTree st;
	++last; //last is not included by default
	st.insert(st.begin(), first, last);
	insert(first, st);
	auto ret = first;
	--ret;
	erase(first, last);
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

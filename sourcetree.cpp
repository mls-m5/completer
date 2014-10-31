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


		{"+", SourceTree::Operator},
		{"++", SourceTree::Operator},
		{"-", SourceTree::Operator},
		{"--", SourceTree::Operator},
		{",", SourceTree::Coma},
		{";", SourceTree::Semicolon},
		{"=", SourceTree::Equals},

};

typedef const vector<SourceTree::DataType> patternType;

patternType variableDeclarationPattern = {
		SourceTree::Type,
		SourceTree::Raw,
//		SourceTree::Semicolon
};

patternType controlStatementPattern = {
		SourceTree::ControlStatementKeyword,
		SourceTree::ParanthesisBlock,
		SourceTree::BraceBlock
};

patternType classDeclarationPattern = {
		SourceTree::ClassKeyword,
		SourceTree::Raw,
		SourceTree::BraceBlock
};

patternType structDeclarationPattern = {
		SourceTree::StructKeyword,
		SourceTree::Raw,
		SourceTree::BraceBlock
};

patternType anonymousClassDeclarationPattern = {
		SourceTree::ClassKeyword,
		SourceTree::BraceBlock
};


std::map<patternType, SourceTree::DataType> patternInterpretations = {
		{anonymousClassDeclarationPattern, SourceTree::ClassDeclaration},
		{classDeclarationPattern, SourceTree::ClassDeclaration},
		{structDeclarationPattern, SourceTree::StructDeclaration},
		{controlStatementPattern, SourceTree::ControlStatement},
		{variableDeclarationPattern, SourceTree::VariableDeclaration},

};

//For initializing stuff
static class InitializerClass{
public:
	InitializerClass(){
		if (!basicTypes.size()){
			for (auto &it: basicTypeNames){
				auto typeAst = new SourceTree;
				typeAst->type = SourceTree::BasicType;
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
			push_back(SourceTree(token, Coma));
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
		else if (type == BraceBlock and token == "}"){
			cout << "end of scope" << endl;
			break;
		}
		else if (type == ParanthesisBlock and token == ")"){
			cout << "end of scope" << endl;
			break;
		}
		else{
			push_back(SourceTree());
			back().type = Raw;
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
	if (size()){
		intent(stream, level);
		stream << "start group ";

		switch (type){
		case ParanthesisBlock:
			stream << "()";
			break;
		case BraceBlock:
			stream << "{}";
			break;
		case VariableDeclaration:
			stream << "variable declaration";
			break;
		case ControlStatement:
			stream << front().name;
			break;
//		case VariableDeclarationAndInitialization:
//			stream << "declaration with initialization";
//			break;
		case ClassDeclaration:
			stream << "class";
			break;
		}

		stream << endl;
	}
	intent(stream, level);
	stream << name << endl;
	for (auto &it: *this){
		it.print(stream, level + 1);
	}
	if (size()){
		intent(stream, level);
		stream << "end group" << endl;
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
			it->type = f->second;
		}
		else if (it->name.type == Token::Digit){
			it->type = Digit;
		}
		else if (findDataType(it->name)){
			cout << "datatype " << it->name << endl;
			it->type = Type;

			//Start chunking togther datatypes that consists of several words
			auto jt = it;
			++jt;
			if (FindBasicType(it->name)){
				while (jt != end()){
					if (FindBasicType(jt->name)){

						it->name += (" " + jt->name);
						cout << "multi word data type " << it->name << endl;

						jt = erase(jt);
					}
					else {
						break;
					}
				}
			}
			it->dataType = findDataType(it->name);

			//Count pointer depth
			while (jt != end() and jt->name == "*"){
				++it->pointerDepth;
				jt = erase(jt);
			}
		}

		unprocessedExpressions.push_back(&*it);

		if (it->type == Semicolon or it->type == Coma){
			//Todo: Handle this
			unprocessedExpressions.clear();
			cout << "end of statement" << endl;
		}
		else {
			for (auto &pattern: patternInterpretations){
				if (tryGroupExpressions(it, unprocessedExpressions,
						pattern.first, pattern.second)){
					cout << "grouped pattern " << pattern.second << endl;
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

bool SourceTree::tryGroupExpressions(iterator &it, std::vector<SourceTree*> unprocessedExpressions,
		const std::vector<DataType> &pattern, DataType resultingType) {
	if (comparePattern(unprocessedExpressions, pattern)){
		it = groupExpressions(it, pattern.size());
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

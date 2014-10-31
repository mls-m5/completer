/*
 * sourcetree.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "sourcetree.h"
#include "tokenizer.h"
#include <fstream>

using namespace std;

std::list<SourceTree*> basicTypes;

std::vector<string> basicTypeNames = {
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

SourceTree::SourceTree() {
}

SourceTree::~SourceTree() {
}

FilePosition SourceTree::parse(std::istream& stream, FilePosition fileIterator) {
	FilePosition filePosition = fileIterator;

	auto token = Tokenizer::GetNextToken(stream);
	filePosition += token;
	while (token.type != Token::None){
		cout << "(" << filePosition.line << ":" << filePosition.column << ") " << token << endl;
		if (token.type == Token::Space){
			token = Tokenizer::GetNextToken(stream);
			filePosition += token;
			continue;
		}
		else if (token == "{"){
			push_back(SourceTree());
			cout << "new braceblock" << endl;
			back().type = BraceBlock;
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "("){
			cout << "new paranthesis block" << endl;
			push_back(SourceTree());
			back().type = ParanthesisBlock;
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
	for (int i = 1; i < level; ++i){
		stream << "    ";
	}
}

void SourceTree::print(std::ostream& stream, int level) {
	if (size()){
		intent(stream, level);
		stream << "start block ";

		switch (type){
		case ParanthesisBlock:
			stream << "()";
			break;
		case BraceBlock:
			stream << "{}";
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
		stream << "end block" << endl;
	}
}

void SourceTree::secondPass() {
	list<SourceTree*> ambigousExpressions; //A list of expressions that is not yet used

	for (auto it = begin(); it != end(); ++it){
		if (it->name == "template"){
			cout << "template ... not handled yet" << endl;
		}
		else if (findDataType(it->name)){
			cout << "datatype " << it->name << endl;
			it->type = Type;
			auto jt = it;
			jt ++;
//			if (findDataType(jt->name)){
//				cout << "multi word data type" << endl;
//			}
		}

		//Todo:: handle ambigious expressions
	}
}

SourceTree* SourceTree::findDataType(std::string &name) {
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

SourceTree* SourceTree::FindBasicType(std::string& name) {
	for (auto &it: basicTypes){
		if (it->name == name){
			return it;
		}
	}
	return 0;
}

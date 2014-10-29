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

std::vector<string> dataTypes = {
		//Todo:: Fill this
		"int",
		"float",
		"short",
};

bool isDataType(string &name){
	for (auto &it: dataTypes){
		if (it == name){
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
	Tokenizer tokenizer;
	FilePosition filePosition = fileIterator;

	auto token = tokenizer.getNextToken(stream);
	while (token.type != Token::None){
		filePosition += token;
		token = tokenizer.getNextToken(stream);
		cout << "(" << filePosition.line << ":" << filePosition.column << ") " << token << endl;
		if (token == "{"){
			push_back(SourceTree());
			cout << "new scope" << endl;
			filePosition = back().parse(stream, filePosition);
		}
		else if (token == "}"){
			cout << "end of scope" << endl;
		}
		else if (token.type == Token::KeyWord){
			if (isDataType(token)){
				cout << "datatype";
				token = tokenizer.getNextToken(stream);
				auto newContent = new DataTypeContent;
				newContent->name = token;
				content = std::shared_ptr<SourceContent>(newContent);
			}
			cout << "keyword" << endl;
		}
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

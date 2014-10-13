/*
 * sourcetree.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "sourcetree.h"
#include "tokenizer.h"

using namespace std;

SourceTree::SourceTree() {
}

SourceTree::~SourceTree() {
}

void SourceTree::parse(std::istream& stream) {
	Tokenizer tokenizer;

	auto token = tokenizer.getNextToken(stream);
	while (token.type != Token::None){
		cout << token << endl;
	}
}

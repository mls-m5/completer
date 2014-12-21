/*
 * tokenizer.h
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once
#include <iostream>
#include <list>
#include <string>

class Token: public std::string{
public:
	enum TokenType {
		Space,
		Word,
		KeyWord,
		Digit,
		String,
		Char,
		StartScope,
		SpacedOutCharacter,
		OperatorOrPunctuator,
		PreprocessorCommand,
		None
	};
	Token (std::string str): std::string(str) {}
	Token (std::string str, TokenType type):
		type(type),
		std::string(str){
	}

	std::string getStringContent(){
		if (size() > 2){
			return substr(1, size() - 2);
		}
		else{
			return std::string();
		}
	}

	TokenType type = None;

};

namespace Tokenizer {
	Token GetNextToken(std::istream& stream);
	std::list<Token> splitStringToToken(std::string);
};


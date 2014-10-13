/*
 * tokenizer.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "tokenizer.h"

#include <sstream>
using namespace std;

string specialCharacters = "_{}[]#()<>%:;.?*+-/^&|∼!=,\'";

Tokenizer::Tokenizer() {
}

Tokenizer::~Tokenizer() {
}

Token Tokenizer::getNextToken(std::istream& stream) {
	stringstream ss;

	Token::TokenType mode = Token::None;

	while (!stream.eof()){
		char c = stream.get();
		switch (mode){
		case Token::None:
		{
			if (isspace(c)){
				ss.put(c);
				mode = Token::Space;
				continue;
			}
			else if (isalpha(c)){
				ss.put(c);
				mode = Token::Word;
				continue;
			}
			else if (isdigit(c) || c == '.'){
				ss.put(c);
				mode = Token::Digit;
				continue;
			}
			else if(c == '"'){
				mode = Token::String;
				continue;
			}
			else if(specialCharacters.find(c) != string::npos){
				mode = Token::SpacedOutCharacter;
				ss.put(c);
				return Token(ss.str(), Token::SpacedOutCharacter);
			}
		}
		break;
		case Token::Space:
			while (isspace(c) && !stream.eof()){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			return Token(ss.str(), Token::Space);

		case Token::Word:
			while (isalnum(c) && !stream.eof()){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			return Token(ss.str(), Token::Word);

		case Token::Digit:
			while ((isdigit(c) || c == '.') && !stream.eof()){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			return Token(ss.str(), Token::Digit);

		case Token::String:
			while (c != '"' && !stream.eof()){
				ss.put(c);
				c = stream.get();
				if (c == '\\'){
					ss.put(c);
					c = stream.get();
					ss.put(c);
					c = stream.get();
				}
			}

			//No unget here, we dont want to keep trailing '"'
			return Token(ss.str(), Token::String);
		}
	}

	return Token(ss.str(), Token::None);
}

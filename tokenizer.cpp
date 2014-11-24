/*
 * tokenizer.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#include "tokenizer.h"
#include "preprocessing_op_or_punc.h"

#include <sstream>
#include <vector>
#include <fstream>
using namespace std;

string specialCharacters = "_{}[]#()<>%:;.?*+-/^&|∼!=,\'";


static class KeyWords: public vector<string>{
public:
	KeyWords(){
		ifstream file("keywords.txt");
		string word;
		while (file >> word){
			push_back(word);
		}
	}

	bool isKeyWord(const string &c){
		for (auto &it: *this){
			if (it == c){
				return true;
			}
		}
		return false;
	}
} keyWords;

//Return 0 if no match is found
//-1 if several matches is found or one with a length larger than the expression
//index of operator if only one is found with the same length
int getPreprocessOperorOrSubString(std::string &beginning){
	int foundstring = 0;
	int count = 0;
	for (int s = 1; s < preprocessingOpOrPunc.size(); ++s){
		auto it = preprocessingOpOrPunc[s];
		bool same = true;
		if (beginning.size() > it.size()){
			//not the same
			continue;
		}
		for (int i = 0; i < beginning.size(); ++i){
			if (beginning[i] != it[i]){
				same = false;
				break;
			}
		}
		if (same){
			if (foundstring){
				return -1; //Several matches
			}
			else{
				foundstring = s;
			}
		}
	}

	return foundstring;
}

Token Tokenizer::GetNextToken(std::istream& stream) {
	stringstream ss;

	Token::TokenType mode = Token::None;

	while (stream){
		char c = stream.get();
		switch (mode){
		case Token::None:
		{
			if (isspace(c)){
				ss.put(c);
				mode = Token::Space;
				continue;
			}
			else if (isalpha(c) or c == '_'){
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
				ss.put(c);
				continue;
			}
			else if(c == '#'){
				mode = Token::PreprocessorCommand;
				ss.put(c);

				char line[500];
				stream.getline(line, 500);
				ss << line;
				return Token(ss.str(), Token::PreprocessorCommand);
			}
			if (c == '/'){
				//possible comment
				if (stream.peek() == '/' or stream.peek() == '*'){
					mode = Token::Space;
					stream.unget();
					continue;
				}
			}
			if (c == '\''){
				c = stream.get();
				if (c == '\''){
					return Token("", Token::Char);
				}
				if (stream.get() == '\''){
					char tmp[2] = {c, 0};
					return Token(tmp, Token::Char);
				}
			}
			else if(specialCharacters.find(c) != string::npos && stream){
				mode = Token::SpacedOutCharacter;
				ss.put(c);
				continue;
//				return Token(ss.str(), Token::SpacedOutCharacter);
			}
		}
		break;
		case Token::Space:
			while ((isspace(c) or c == '/') && stream){
				if (c == '/'){ //Ignore comments
					if (stream.peek() == '/'){
						while (c != '\n' and stream){
							ss.put(c);
							c = stream.get();
						}
					}
					else if (stream.peek() == '*'){ //c-style comment
						while (!(c == '*' and stream.peek() == '/') and stream){
							ss.put(c);
							c = stream.get();
						}
						ss.put(c);
						c = stream.get();
					}
					else{
						return Token("/", Token::SpacedOutCharacter);
					}
				}

				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			return Token(ss.str(), Token::Space);

		case Token::Word:
		{
			while ((isalnum(c) or c == '_') && stream){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();
			bool isKeyword = keyWords.isKeyWord(ss.str());
			return Token(ss.str(), isKeyword? Token::KeyWord : Token::Word);
		}

		case Token::Digit:
		{
			while ((isdigit(c) || c == '.') && stream){
				ss.put(c);
				c = stream.get();
			}
			stream.unget();

			auto token = Token(ss.str(), Token::Digit);
			if (token == ".") {
				token.type = Token::OperatorOrPunctuator;
			}
			return token;
		}

		case Token::String:
			while (c != '"' && stream){
				ss.put(c);
				c = stream.get();
				if (c == '\\'){
					ss.put(c);
					c = stream.get();
					ss.put(c);
					c = stream.get();
				}
			}
			ss.put('"');

			return Token(ss.str(), Token::String);
		case Token::SpacedOutCharacter:
			std::string tmp = ss.str();
			bool isOperatorIsh = getPreprocessOperorOrSubString(tmp) != 0;
			ss.put(c);
			tmp = ss.str(); //Todo: Optimize this
			while ((getPreprocessOperorOrSubString(tmp)) && stream){
				c = stream.get();
				ss.put(c);
				tmp = ss.str();
				isOperatorIsh = true;
			}
			stream.unget();
			return Token(tmp.substr(0, tmp.size() -1),
					isOperatorIsh? Token::OperatorOrPunctuator: Token::SpacedOutCharacter);

			break;
		}
	}

	return Token(ss.str(), Token::None);
}

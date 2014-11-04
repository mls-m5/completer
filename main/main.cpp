/*
 * main.cpp
 *
 *  Created on: 4 nov 2014
 *      Author: Mattias Larsson Sköld
 */
#include <fstream>
#include <iostream>
#include "../sourcetree.h"

using namespace std;

int tokenize(istream &stream){
	while (stream){
		auto token = Tokenizer::GetNextToken(stream);
		if (token.type != Token::Space){
			cout << token << endl;
		}
	}
	return 0;
}

int printAst(istream &stream){
	SourceTree st;
	st.parse(stream, FilePosition());
	st.secondPass();
	st.print(cout, 0);
	return 0;
}

int main(int argc, char **argv) {
	enum {
		Default,
		Tokenize,
		AstOutput,
	} mode = Default;

	istream *input = 0;
	for (int i = 0; i < argc; ++i){
		string arg(argv[i]);
		if (arg == "--ast"){
			mode = AstOutput;
		}
		else if (arg == "--tokenize"){
			mode = Tokenize;
		}
		if (arg == "-f" or arg == "--file"){
			i ++;
			if (!input){
				input = new ifstream(argv[i]);
			}
			else {
				cerr << "can only select one file" << endl;
			}
		}
		if (arg == "-" or arg == "--stdin"){
			input = &cin;
		}
	}

	if (!input){
		cerr << "no input file define file with '-f <filename>' or use stdin with '-'" << endl;
		return 1;
	}

	switch (mode){
	case Tokenize:
		tokenize(*input);
		break;
	case AstOutput:
		printAst(*input);
		break;
	case Default:
		cerr << "no mode selected --tokenize or --ast" << endl;
	}
}


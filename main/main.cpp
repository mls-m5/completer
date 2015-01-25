/*
 * main.cpp
 *
 *  Created on: 4 nov 2014
 *      Author: Mattias Larsson Sköld
 */
#include <fstream>
#include <iostream>
#include "../rootsourcetree.h"

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

int completeSymbol(istream &stream, std::string expression){
	RootSourceTree st;
	st.parse(stream, FilePosition());
	st.secondPass();
	auto completionList = st.completeSymbol(expression);

	for (auto it: completionList) {
//		cout << "completion : " << it->getFullName() << endl;
//		cout << "info: " << it->getTypeName() << ", " <<
//				(it->getType() ? it->getType()->getFullName() : std::string(""))  << endl;
		cout << it->getLocalName() << endl;
	}

	return 0;
}

int dumpSymbols(istream &stream) {
	RootSourceTree st;
	st.parse(stream, FilePosition());
	st.secondPass();

	st.printSymbols(&cout, false);
	return 0;
}

int main(int argc, char **argv) {
	enum {
		Default,
		Tokenize,
		AstOutput,
		GlobalComplete,
		PrintSymbols,
	} mode = Default;
	std::string argument;

	istream *input = 0;
	for (int i = 0; i < argc; ++i){
		string arg(argv[i]);
		if (arg == "--ast"){
			mode = AstOutput;
		}
		else if (arg == "--tokenize"){
			mode = Tokenize;
		}
		else if (arg == "--complete") {
			mode = GlobalComplete;
			++i;
			if (i >= argc) {
				cerr << "expression needed" << endl;
				return 1;
			}
			argument = argv[i];
		}
		else if (arg == "--symbols") {
			mode = PrintSymbols;
		}
		else if (arg == "-f" or arg == "--file"){
			++i;
			if (!input){
				input = new ifstream(argv[i]);
				if (!input) {
					cerr << "file note opened" << endl;
					return 1;
				}
			}
			else {
				cerr << "can only select one file" << endl;
			}
		}
		else if (arg == "-" or arg == "--stdin"){
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
	case GlobalComplete:
		completeSymbol(*input, argument);
		break;
	case PrintSymbols:
		dumpSymbols(*input);
		break;
	case Default:
		cerr << "no mode selected --tokenize, --ast, --symbols, or --complete" << endl;
	}
}


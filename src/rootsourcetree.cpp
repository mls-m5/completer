/*
 * sourcetreeroot.cpp
 *
 *  Created on: 20 dec 2014
 *      Author: Mattias Larsson Sköld
 */

#include "rootsourcetree.h"
#include <sstream>
#include <iostream>
#define PRINT_DEBUG true
#include "common.h"

using namespace::std;



RootSourceTree::~RootSourceTree() {
}




RootSourceTree RootSourceTree::CreateFromString(std::string source) {
	RootSourceTree sourceTree;
	istringstream ss(source);
	sourceTree.parse(ss);
	sourceTree.secondPass();
	DEBUG sourceTree.print(cout, 0);
	return sourceTree; //This would be a waste if there was no move constructor :)
}




RootSourceTree::RootSourceTree() {
	_root = this;
}




void RootSourceTree::insertSymbol(SourceTree* sourceTree) {
	_symbols.insert(pair<std::string, SourceTree*>(sourceTree->getLocalName(), sourceTree));
//	DEBUG cout << "inserting symbol " <<  sourceTree->getFullName() << " -> size " << _symbols.size() << endl;
}



std::list<SourceTree*> RootSourceTree::completeSymbol(std::string name,
		std::string context, SourceTree* location) {

	auto tokens = Tokenizer::splitStringToToken(name);
	auto localNameToken = tokens.back();

	std::list<SourceTree*> found;
	for (auto it: _symbols) {
		if (it.first.compare(0, localNameToken.size(), localNameToken) == 0) {
			found.push_back(it.second);
		}
	}
	return found;
}




std::list<SourceTree*> RootSourceTree::findSymbol(std::string name,
		std::string context, SourceTree* location, SourceTree::DataType type) {
	auto tokens = Tokenizer::splitStringToToken(name);
	cout << "tokens to search" << endl;
	for (auto &it: tokens) {
		cout << it << endl;
	}
	auto foundRange = _symbols.equal_range(tokens.back());
	std::list<SourceTree*> retList;
	for (auto it = foundRange.first; it != foundRange.second; ++it) {
		if (type != SourceTree::None and type != it->second->type()) {
			continue; //Wrong type
		}
		retList.push_back(it->second);
	}
	return retList;
}

void RootSourceTree::printSymbols(std::ostream* stream, bool printInfo) {
	if (stream == 0) {
		stream = &cout;
	}

	if (printInfo) {
		*stream << "numbers of symbols: " << _symbols.size() << endl;
		for (auto it: _symbols) {
			*stream << it.first << " : " << it.second->getFullName() << " : " << it.second->getTypeName() << endl;
		}
	}
	else {
		for (auto it: _symbols) {
			*stream << it.second->getLocalName() << endl;
		}
	}
}

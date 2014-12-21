/*
 * sourcetreeroot.h
 *
 *  Created on: 20 dec 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include "sourcetree.h"
#include <list>
#include <map>
#include <ostream>


class RootSourceTree: public SourceTree {
public:
	RootSourceTree();
	~RootSourceTree();

	std::list<SourceTree*> findSymbol(std::string name, std::string context,
			SourceTree *location = 0, SourceTree::DataType = SourceTree::None);

	std::list<SourceTree*> completeSymbol(std::string name, std::string context = "",
			SourceTree *location = 0);


	void printSymbols(std::ostream *stream = 0);


	//For testing mainly
	static RootSourceTree CreateFromString(std::string);

protected:
	friend SourceTree;

	void insertSymbol(SourceTree*);

	std::multimap<std::string, SourceTree*> _symbols;
};


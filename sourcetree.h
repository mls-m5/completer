/*
 * sourcetree.h
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <vector>
#include <iostream>

class SourceTree: public std::vector<SourceTree> {
public:
	SourceTree();
	virtual ~SourceTree();

	void parse(std::istream &stream);
};


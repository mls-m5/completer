/*
 * sourcetree.h
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <vector>
#include <iostream>

struct FilePosition {
	FilePosition() {
		line = 1;
		column = 1;
	}
	void operator += (std::string &str);

	int line, column;
};
class SourceTree: public std::vector<SourceTree> {
public:
	enum Type{
		BasicType,
		UnqualifiedId,
		Scope,

		ArrayList
	};
	SourceTree();
	virtual ~SourceTree();

	FilePosition parse(std::istream &stream, FilePosition startPos = FilePosition());

	Type type;
};


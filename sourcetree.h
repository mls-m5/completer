/*
 * sourcetree.h
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include <vector>
#include <iostream>
#include <memory>

struct FilePosition {
	FilePosition() {
		line = 1;
		column = 1;
	}
	void operator += (std::string &str);

	int line, column;
};

class SourceContent {
public:

	enum ContentType{
		None,
		DataType,
	};

	ContentType type = None;

};

class DataTypeContent: public SourceContent {
public:
	DataTypeContent(){
		type = DataType;
	}
	std::string name;
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

	std::shared_ptr<SourceContent> content = 0;
	Type type = BasicType;
};


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
#include <list>

#include "tokenizer.h"
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

class SourceTree: public std::list<SourceTree> {
public:
	enum DataType{
		BasicType,
		Type,
		Digit,
		UnqualifiedId,
		Scope,
		ArrayList,
		Raw,

		VariableDeclaration,
		VariableDeclarationAndInitialization,
		ControlStatementKeyword,
		ControlStatement,

		Semicolon,
		Coma,
		Equals,
		Operator,

		ArrayBlock,

		ClassKeyword,
		ClassDeclaration,

		TemplateKeyword,

		BraceBlock,
		BracketBlock,
		ParanthesisBlock,
	};
	SourceTree();
	SourceTree(Token name, DataType type): name(name), type(type) {}
	virtual ~SourceTree();

	FilePosition parse(std::istream &stream, FilePosition startPos = FilePosition());
	void secondPass();
	SourceTree::iterator groupExpressions(SourceTree::iterator last, int count);
	SourceTree::iterator groupExpressions(SourceTree::iterator first, SourceTree::iterator last);

	bool tryGroupExpressions(iterator &it, std::vector<SourceTree*> unprocessedExpressions,
			const std::vector<DataType> &pattern, DataType resultingType);

	void print(std::ostream &stream, int level);

	SourceTree *findDataType(std::string &name);
	static SourceTree *FindBasicType(std::string &name);

	Token name = Token("", Token::None);
	std::shared_ptr<SourceContent> content = 0;
	DataType type = BasicType;
	SourceTree* dataType = 0;
	int pointerDepth = 0;
};


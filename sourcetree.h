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
		None,
		Type,
		Digit,
		UnqualifiedId,
		Scope,
		ArrayList,
		Raw,

		VariableDeclaration,
		DefinitionName,
		DeclarationName,
//		VariableDeclarationAndInitialization,
		FunctionDeclaration,
		FunctionDefinition,
		LambdaFunction,
		ControlStatementKeyword,
		ControlStatement,
		AssignmentStatement,
		NamespaceKeyWord,
		Namespace,
		NamespaceScope,

		Semicolon,
		ComaOperator,
		Equals,
		ScopeResolution,
		ElementSelectionThroughPointer,
		ElementSelectionByReference,
		Operator,

		ArrayBlock,

		ClassKeyword,
		ClassDeclaration,
		StructKeyword,
		StructDeclaration,

		TemplateKeyword,
		ReturnKeyword,
		NewKeyword,
		DeleteKeyword,

		BraceBlock,
		BracketBlock,
		ParanthesisBlock,
		TemplateBlock,
	};
	SourceTree();
	SourceTree(Token name, DataType type): name(name), type(type) {}
	virtual ~SourceTree();

	FilePosition parse(std::istream &stream, FilePosition startPos = FilePosition());
	void secondPass();
	SourceTree::iterator groupExpressions(SourceTree::iterator last, int count);
	SourceTree::iterator groupExpressions(SourceTree::iterator first, SourceTree::iterator last);

	bool tryGroupExpressions(iterator &it, std::vector<SourceTree*> &unprocessedExpressions,
			const std::vector<DataType> &pattern, DataType resultingType, const std::vector<DataType> &replacementPattern);

	void print(std::ostream &stream, int level) const;
	std::string getFullName() const;
	std::string getLocalName() const;
	std::string getTypeName() const;
	void setParent(SourceTree *parent);

	std::list<SourceTree *> completeExpression(std::string name);
	std::list<SourceTree *> findExpressions(std::list<Token> tokens);
	SourceTree *findDataType(std::string &name);
	SourceTree *findBranchByType(DataType type);
	SourceTree *findVariable(std::string &name);
	SourceTree *findNameSpace(std::string &name);
	static SourceTree *FindBasicType(std::string &name);
	SourceTree *getType();

	//For testing mainly
	static SourceTree CreateFromString(std::string);

	Token name = Token("", Token::None);
	std::shared_ptr<SourceContent> content = 0;
	DataType type = Type;
	SourceTree* dataType = 0;
	int pointerDepth = 0;
	bool hidden = false;
	SourceTree *parent = 0;
};


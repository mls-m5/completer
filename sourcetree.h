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
	void operator +=(std::string &str);

	int line, column;
};

class SourceContent {
public:

	enum ContentType {
		None, DataType,
	};

	ContentType type = None;

};

class DataTypeContent: public SourceContent {
public:
	DataTypeContent() {
		type = DataType;
	}
	std::string name;
};

class SourceTree: public std::list<SourceTree> {
public:
	enum DataType {
		None,
		Type,
		Variable,
		Digit,
		UnqualifiedId,
		Scope,
		ArrayList,
		Raw,

		VariableDeclaration,
		DefinitionName,
		DeclarationName,
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
		BinaryOperation,

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
	SourceTree(Token name, DataType type, SourceTree *parent) :
			_name(name), _type(type), _parent(parent) {
		if (_parent) {
			_root = _parent->_root;
		}
	}
	virtual ~SourceTree();

	FilePosition parse(std::istream &stream, FilePosition startPos =
			FilePosition());
	void secondPass();
	void checkFieldForNames(iterator &it);
	void groupExpressionsWithOperators(iterator &begin, iterator &end);
	SourceTree::iterator groupExpressions(SourceTree::iterator first,
			SourceTree::iterator last);

	void print(std::ostream &stream, int level);
	std::string getFullName() const;
	std::string getLocalName() const;
	std::string getTypeName() const;
	void setParent(SourceTree *parent);

	std::list<SourceTree *> findExpressions(std::list<Token> tokens);
	SourceTree *findDataType(std::string &name);
	SourceTree *findBranchByType(DataType type);
	SourceTree *findVariable(const std::string *name);
	SourceTree *findVariable(std::string name) {
		return findVariable(&name);
	}
	SourceTree *findNameSpace(std::string &name);
	static SourceTree *FindBasicType(std::string &name);
	SourceTree *getType();

	//properties
	DataType type() const {
		return _type;
	}
	void type (DataType t); //Handles registration to symbol list at root

	inline const Token &name() {
		return _name;
	}

	//public variables
	Token _name = Token("", Token::None);
	std::shared_ptr<SourceContent> content = 0;
	SourceTree* dataType = 0;
	int pointerDepth = 0;
	bool hidden = false;


protected:
	SourceTree *_parent = 0;
	class RootSourceTree *_root = 0;
	DataType _type = Type;
	bool _isSymbol = false; //if the class should be added to the symbol map
};


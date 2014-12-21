/*
 * replacementpatterns.h
 *
 *  Created on: 20 dec 2014
 *      Author: Mattias Larsson Sköld
 */


#pragma once


#include "sourcetree.h"
#include <vector>
typedef const std::vector<SourceTree::DataType> patternType;

struct ReplacementRule {
	patternType first;
	SourceTree::DataType second;
	patternType replacementPattern;

	void tryGroupExpressions();
};

patternType controlStatementPattern = {
		SourceTree::ControlStatementKeyword,
		SourceTree::ParanthesisBlock,
		SourceTree::BraceBlock
};

patternType anonymousClassDeclarationPattern = {
		SourceTree::ClassKeyword,
		SourceTree::BraceBlock
};

patternType variableDeclarationAndAssignmentPattern = {
		SourceTree::VariableDeclaration,
		SourceTree::Equals,
};

extern std::vector<ReplacementRule> patternInterpretations;

static std::vector<SourceTree::DataType> typesToRegister = {
	SourceTree::ClassDeclaration,
	SourceTree::VariableDeclaration,
	SourceTree::FunctionDeclaration,
	SourceTree::FunctionDefinition,
	SourceTree::Namespace,
};



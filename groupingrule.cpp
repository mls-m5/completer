/*
 * replacementrule.cpp
 *
 *  Created on: 21 dec 2014
 *      Author: Mattias Larsson Sköld
 */

#include "groupingrule.h"

static patternType controlStatementPattern = {
		SourceTree::ControlStatementKeyword, SourceTree::ParanthesisBlock,
		SourceTree::BraceBlock };

static patternType anonymousClassDeclarationPattern = {
		SourceTree::ClassKeyword, SourceTree::BraceBlock };

std::vector<GroupingRule> patternInterpretations =
		{ { { SourceTree::Type, SourceTree::Raw, },
				SourceTree::VariableDeclaration, { SourceTree::Type,
						SourceTree::DefinitionName } },

		{ anonymousClassDeclarationPattern, SourceTree::ClassDeclaration }, { {

		SourceTree::ClassKeyword, SourceTree::Raw, SourceTree::BraceBlock },
				SourceTree::ClassDeclaration, { SourceTree::ClassKeyword,
						SourceTree::DeclarationName, SourceTree::BraceBlock } },
		//Templated class
				{ { SourceTree::TemplateBlock, SourceTree::ClassKeyword,
						SourceTree::Raw, SourceTree::BraceBlock },
						SourceTree::ClassDeclaration, {
//						SourceTree::TemplateBlock,
//						SourceTree::ClassKeyword,
//						SourceTree::DeclarationName,
//						SourceTree::BraceBlock,
								SourceTree::TemplateBlock,
								SourceTree::ClassKeyword,
								SourceTree::DeclarationName,
								SourceTree::BraceBlock, } },

				{ {

				SourceTree::StructKeyword, SourceTree::Raw,
						SourceTree::BraceBlock }, SourceTree::StructDeclaration,
						{ SourceTree::StructKeyword,
								SourceTree::DeclarationName,
								SourceTree::BraceBlock } },

				//Templated struct
				{ { SourceTree::TemplateBlock, SourceTree::StructKeyword,
						SourceTree::Raw, SourceTree::BraceBlock },
						SourceTree::StructDeclaration, {
								SourceTree::TemplateBlock,
								SourceTree::StructKeyword,
								SourceTree::DeclarationName,
								SourceTree::BraceBlock } },

				{ controlStatementPattern, SourceTree::ControlStatement },

				//Namespace
				{ { SourceTree::NamespaceKeyWord, SourceTree::Raw,
						SourceTree::BraceBlock, }, SourceTree::Namespace, {
						SourceTree::NamespaceKeyWord,
						SourceTree::DefinitionName, SourceTree::BraceBlock, }, true, false },

				{ { SourceTree::VariableDeclaration,
						SourceTree::ParanthesisBlock, },
						SourceTree::FunctionDeclaration },

				{ { SourceTree::FunctionDeclaration, SourceTree::BraceBlock, },
						SourceTree::FunctionDefinition, },

				{ { SourceTree::BracketBlock, //Todo make sure this is identified even when it is not at an end of a line
						SourceTree::ParanthesisBlock, SourceTree::BraceBlock, },
						SourceTree::LambdaFunction, }, };

bool GroupingRule::comparePattern(SourceTree::iterator begin,
		SourceTree::iterator end, Range *range) {
	auto it = begin;
//	if (pattern.size() != _first.size()) {
//		return false;
//	}
	for (int i = 0; i < _first.size(); ++i) {
		if (it->type() != _first[i]) {
			return false;
		}
		++it;
	}

	if (it != end) {
		return false; //The whole patternWas not matched
	}
	range->begin = begin;
	range->end = end;
	return true;
}

bool GroupingRule::tryGroupExpressions(SourceTree* sourceTree,
		SourceTree::iterator& begin, SourceTree::iterator& end) {
	Range range; //Return from comparePattern
	if (comparePattern(begin, end, &range)) {
		auto it = groupExpressions(sourceTree, begin, end, range.begin, range.end);
		if (_first.size() == replacementPattern.size()) {
			//Replace the identifier pattern with a new one
			//For example interprets which fields are name fields etc
			auto replacementIterator = replacementPattern.begin();
			for (auto jt = it->begin(); jt != it->end();
					++jt, ++replacementIterator) {
				jt->type(*replacementIterator);
			}
		}
		it->type(_groupName);
//		unprocessedExpressions.clear();
//		unprocessedExpressions.push_back(&*it);
		return true;
	}
	return false;
}

SourceTree::iterator GroupingRule::groupExpressions(SourceTree* owner,
		SourceTree::iterator &statementBeginning,
		SourceTree::iterator &statementEnd, SourceTree::iterator &begin,
		SourceTree::iterator &end) {

	SourceTree st;
	auto ret = owner->insert(begin, st);

	ret->splice(ret->begin(), *owner, begin, end);

	ret->setParent(owner);

	if (begin == statementBeginning) {
		statementBeginning = ret;
	}
	begin = ret;
	if (end == statementEnd) {
		end = begin;
		++end;
		statementEnd = end;
	}
	else {
		end = begin;
		++end;
	}

	return ret;
}

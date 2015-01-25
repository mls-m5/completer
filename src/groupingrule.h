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

struct GroupingRule {
	typedef SourceTree::DataType DataType;
	GroupingRule(patternType pattern, SourceTree::DataType groupName,
			patternType replacement = patternType(), bool beginningOnly = true,
			bool needSemicolon = true) :
			_first(pattern), _groupName(groupName), replacementPattern(
					replacement), beginningOnly(beginningOnly), needSemicolon(
					needSemicolon) {
	}
	virtual ~GroupingRule() {
	}
	;

	patternType _first;
	SourceTree::DataType _groupName;
	patternType replacementPattern;
	bool beginningOnly; //If the pattern has to be in the beginning of a statement
	bool needSemicolon; //If the pattern will end the statement my itself

	struct Range {
		SourceTree::iterator begin, end;
	};

	virtual bool comparePattern(SourceTree::iterator begin,
			SourceTree::iterator end, Range *range);

	virtual bool tryGroupExpressions(SourceTree *sourceTree,
			SourceTree::iterator &statementBeginning,
			SourceTree::iterator &last);

	SourceTree::iterator groupExpressions(SourceTree *owner,
			SourceTree::iterator &statementBeginning,
			SourceTree::iterator &statementEnd, SourceTree::iterator &begin,
			SourceTree::iterator &end);

};

extern std::vector<GroupingRule> patternInterpretations;


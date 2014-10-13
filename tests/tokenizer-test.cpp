/*
 * tokenizer-test.cpp
 *
 *  Created on: 13 okt 2014
 *      Author: Mattias Larsson Sköld
 */


#include "unittest.h"
#include "tokenizer.h"
#include <sstream>

using namespace std;

TEST_SUIT_BEGIN

TEST_CASE("parse spaces"){
	Tokenizer tokenizer;

	stringstream ss("     \tapa ");

	auto ret = tokenizer.getNextToken(ss);

	ASSERT(ret.compare("     \t") == 0, "not expected token");

	return 0;
}

TEST_CASE("parse word"){
	Tokenizer tokenizer;

	stringstream ss;
	ss << "apa bepa";

	auto ret = tokenizer.getNextToken(ss);

	ASSERT_EQ(ret, "apa");
}

TEST_CASE("parse digit"){
	Tokenizer tokenizer;
	stringstream ss;

	ss << ".99 2.4";

	auto ret1 = tokenizer.getNextToken(ss);
	tokenizer.getNextToken(ss); //space
	auto ret2 = tokenizer.getNextToken(ss);

	ASSERT_EQ(ret1, ".99");
	ASSERT_EQ(ret1.type, Token::Digit);
}

TEST_CASE("parse string"){
	Tokenizer tokenizer;
	string testString("apan is \\\"sneel\\\"");
	stringstream ss("\"" + testString +  "\" bepan is dum");
	auto ret = tokenizer.getNextToken(ss);


	ASSERT_EQ(ret, testString);

	return 0;
}

TEST_CASE("parse special character"){
	Tokenizer tokenizer;
	stringstream ss("&%#");
	auto ret = tokenizer.getNextToken(ss);

	for (int i = 0; i < 3; ++i){
		ASSERT_EQ(ret.size(), 1);
		ASSERT_EQ(ret.type, Token::SpacedOutCharacter);
	}

	return 0;
}

TEST_SUIT_END


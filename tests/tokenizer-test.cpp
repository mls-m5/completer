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
	stringstream ss("     \tapa ");

	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret, "     \t");
}

TEST_CASE("multicharacter operator"){
	stringstream ss("*= <<= ++ ");

	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret.type, Token::OperatorOrPunctuator) //fill in here
	ASSERT_EQ(ret, "*=");
	ret = Tokenizer::GetNextToken(ss); //Space
	ret = Tokenizer::GetNextToken(ss); //Space
	ASSERT_EQ(ret.type, Token::OperatorOrPunctuator) //fill in here
	ASSERT_EQ(ret, "<<=");

	ret = Tokenizer::GetNextToken(ss); //Space
	ret = Tokenizer::GetNextToken(ss); //Space
	ASSERT_EQ(ret.type, Token::OperatorOrPunctuator) //fill in here
	ASSERT_EQ(ret, "++");
}

TEST_CASE("parse word"){

	stringstream ss;
	ss << "apa bepa";

	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret, "apa");
	ASSERT_EQ(ret.type, Token::Word);
}

TEST_CASE("parse keyword"){
	stringstream ss;
	ss << "int bepa";

	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret, "int");
	ASSERT_EQ(ret.type, Token::KeyWord);
}

TEST_CASE("parse digit"){
	stringstream ss;

	ss << ".99 2.4";

	auto ret1 = Tokenizer::GetNextToken(ss);
	Tokenizer::GetNextToken(ss); //space
	auto ret2 = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret1, ".99");
	ASSERT_EQ(ret1.type, Token::Digit);
}

TEST_CASE("parse string"){
	string testString("\"apan is \\\"sneel\\\"\"");
	stringstream ss(testString +  " bepan is dum");
	auto ret = Tokenizer::GetNextToken(ss);


	ASSERT_EQ(ret, testString);
}

TEST_CASE("parse special character"){
	stringstream ss("&%#");
	auto ret = Tokenizer::GetNextToken(ss);

	for (int i = 0; i < 3; ++i){
		ASSERT_EQ(ret.size(), 1);
		ASSERT_EQ(ret.type, Token::OperatorOrPunctuator);
	}
}

TEST_CASE("preprocessor command"){
	stringstream ss("#define apa bepa\n cepa");
	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret.type, Token::PreprocessorCommand);
	ASSERT_EQ(ret, "#define apa bepa");
}

TEST_CASE("paranthesis"){
	stringstream ss("(int)");
	auto ret = Tokenizer::GetNextToken(ss);

	ASSERT_EQ(ret, "(");

	ret = Tokenizer::GetNextToken(ss);
	ASSERT_EQ(ret, "int");

	ret = Tokenizer::GetNextToken(ss);
	ASSERT_EQ(ret, ")");
}

TEST_SUIT_END


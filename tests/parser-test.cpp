/*
 * parser-test.cpp
 *
 *  Created on: 29 okt 2014
 *      Author: Mattias Larsson Sköld
 */


#include "unittest.h"
#include <sstream>

#include "sourcetree.h"
using namespace std;

TEST_SUIT_BEGIN

TEST_CASE("blocks"){
	istringstream ss("int apa() { int bepa; }");

	SourceTree ast;
	ast.parse(ss);
}

TEST_SUIT_END

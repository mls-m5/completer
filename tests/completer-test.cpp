/*
 * completer-test.cpp
 *
 *  Created on: 20 dec 2014
 *      Author: Mattias Larsson Sköld
 */


#include "unittest.h"
#include "sourcetree.h"
#include <iostream>

#include "rootsourcetree.h"
using namespace std;

TEST_SUIT_BEGIN


TEST_CASE("complete auto expressions") {
	auto st = RootSourceTree::CreateFromString(
			"int apa;"
			"auto bepa = apa;"
	);

	auto ret = st.findVariable("bepa");

	cout << "ret är " << endl;
	ret->print(cout, 0);

	ASSERT(ret, "variable bepa not found");
	ASSERT(ret->getType(), "bepa does not have a datatype");
	ASSERT_EQ(ret->getType()->_name, "int");
}




TEST_CASE("find symbol with 'using' statement") {
	auto st = RootSourceTree::CreateFromString(
			"namespace Apa {"
			"	int apa;"
			"}"
			"namespace Bepa {"
			"	int bepa;" //this should not be found
			"}"
			"using namespace Apa;"
			);
//	auto ret = st.findVariable("apa");
	{
		auto ret = st.findSymbol("apa", "", 0, SourceTree::VariableDeclaration);

		cout << "found symbols when searching 'apa'" << endl;
		for (auto it: ret) {
			cout << it->getFullName() << endl;
		}
		cout << endl;

		ASSERT(ret.size() == 1, "variable not found");
	}

	{
		auto ret = st.findSymbol("Apa::apa", "", 0, SourceTree::VariableDeclaration);

		cout << "found symbols when searching 'Apa::apa'" << endl;
		for (auto it: ret) {
			cout << it->getFullName() << endl;
		}
		cout << endl;

		ASSERT(ret.size() == 1, "variable not found");
	}


	{
		auto ret = st.findSymbol("bepa", "", 0, SourceTree::VariableDeclaration);

		cout << "found symbols when searching 'bepa'" << endl;
		for (auto it: ret) {
			cout << it->getFullName() << endl;
		}
		cout << endl;
		ASSERT(ret.size() == 0, "variable bepa is found but should not be in public namespace");
	}


	ERROR_NOT_IMPLEMENTED();
}




TEST_CASE("find symbol") {
	auto st = RootSourceTree::CreateFromString(
			"namespace Apa {"
			"	class Bepa {"
			"		public:"
			"		int apa;"
			"	};"
			"	int cepa;"
			"}"
			"Apa::Bepa bepa; \n"
			"int bepa;"
	);

	st.printSymbols();
	cout << endl;

	auto ret = st.findSymbol("apa", "", 0, SourceTree::VariableDeclaration);

	cout << "found symbols" << endl;
	for (auto it: ret) {
		cout << it->getFullName() << endl;
	}
	cout << endl;

	ASSERT_EQ(ret.size(), 1);
	ASSERT_EQ(ret.front()->getFullName(), "Apa::Bepa::apa");
}





TEST_CASE("complete complex cases") {
	auto st = RootSourceTree::CreateFromString(
			"namespace Apa {"
			"	class Apa {"
			"		static int apa;"
			"		int bepa;"
			"		class Bepa {"
			"			int cepa;"
			"		};"
			"	};"
			"}"
			"Apa::Apa apa;"
			"Apa::Apa::Bepa bepa;"
	);

	st.printSymbols();

	auto ret = st.completeSymbol("Apa::Apa::ap");
	ASSERT_GT(ret.size(), 0);
	ASSERT_EQ(ret.front()->getFullName(), "Apa::Apa::apa");

	ret = st.completeSymbol("bepa.ce");
	ASSERT_GT(ret.size(), 0);
	ASSERT_EQ(ret.front()->getFullName(), "Apa::Apa::Bepa::cepa");
}





TEST_CASE("complete classes") {
	{
		auto st = RootSourceTree::CreateFromString(
				"class Apa {"
				"	static int bepa;"
				"};"
				);

		auto ret = st.completeSymbol("Apa::be");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "bepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::bepa");
	}

	{
		auto st = RootSourceTree::CreateFromString(
				"class Apa {"
				"	int bepa;"
				"};"
				"Apa apa;"
				);

		auto ret = st.completeSymbol("apa.be");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "bepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::bepa");
	}
}




TEST_SUIT_END



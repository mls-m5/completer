
#include "sourcetree.h"
#include "unittest.h"

#include <sstream>

using namespace std;

TEST_SUIT_BEGIN

TEST_CASE("create source tree"){
	stringstream ss("int main(int argc, char **argv)\n"
			"{ cout << \"hello world\" << endl;\n"
			" return 0;\n"
			" }\n");

	SourceTree sourceTree;
	sourceTree.parse(ss);

	sourceTree.print(cout, 0);

	ASSERT_EQ(sourceTree.size(), 4);
//	ASSERT_EQ(sourceTree[0].size(), 2); //check if this is supposed to be the case
}

TEST_CASE("second pass"){
	stringstream ss("int main(int argc, char **argv)\n"
			"{ \n"
			" int x;\n"
			" cout << \"hello world\" << endl;\n"
			" return 0;\n"
			" }\n");

	SourceTree sourceTree;
	sourceTree.parse(ss);

	sourceTree.secondPass();
//	sourceTree.print(cout, 0);
}

TEST_CASE("basic data type"){
	stringstream ss("int apa;");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.secondPass();
//	sourceTree.print(cout, 0);

	ASSERT(sourceTree.front().front().dataType, "ingen content (innehåll)");

	ASSERT_EQ(sourceTree.front().size(), 2);
	ASSERT_EQ(sourceTree.front().front().type, SourceTree::Type);
	ASSERT_EQ(sourceTree.front().back().type, SourceTree::DefinitionName);
}

TEST_CASE("user defined datatype"){
	stringstream ss("class Apa { int x }; Apa apa;");
	SourceTree st;
	st.parse(ss);
	st.secondPass();
	st.print(cout, 0);

	ASSERT(0, "not implemented");
}

TEST_CASE("preprocessor command"){
	stringstream ss("#pragma once \n int apa; ");
	SourceTree st;
	st.parse(ss);
	st.secondPass();
	st.print(cout, 0);
}

TEST_CASE("lambda functions"){
	{
		stringstream ss("[] (int x) {cout << \"hej\" << endl; } ");
		SourceTree st;
		st.parse(ss);

		st.secondPass();
		st.print(cout, 0);
		ASSERT_EQ(st.front().type, SourceTree::LambdaFunction);
	}
	{
		stringstream ss("auto f = [] (int x) {cout << \"hej\" << endl; }; ");
		SourceTree st;
		st.parse(ss);

		st.secondPass();
		st.print(cout, 0);
	}
}

TEST_CASE("multiple word datatypes"){
	stringstream ss("long int **apa;");

	SourceTree sourceTree;
	sourceTree.parse(ss);

//	sourceTree.print(cout, 0);
	sourceTree.secondPass();
	sourceTree.print(cout, 0);

	ASSERT_EQ(sourceTree.front().pointerDepth, 0);
	ASSERT_EQ(sourceTree.front().type, SourceTree::VariableDeclaration);
	ASSERT_EQ(sourceTree.front().front().pointerDepth, 2);
	ASSERT_EQ(sourceTree.front().front().name, "long int");
//	ASSERT_EQ(sourceTree.size(), 1); //Should in the end only be one statement
}


TEST_CASE("for"){
	stringstream ss("for (int i = 0; i < 10; ++i) { int x = i; }");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.type = SourceTree::BraceBlock;
	sourceTree.secondPass();

	sourceTree.print(cout, 0);
}

TEST_CASE("class declaration"){
	stringstream ss("template <class T> class Apa { int x; };");

	SourceTree st;
	st.parse(ss);
	st.secondPass();
	st.print(cout, 0);

	ASSERT_EQ(st.front().type, SourceTree::ClassDeclaration);
}


TEST_CASE("struct declaration"){
	stringstream ss("template <class T> struct apa { int x; };");

	SourceTree st;
	st.parse(ss);
	st.secondPass();
	st.print(cout, 0);

	ASSERT_EQ(st.front().type, SourceTree::StructDeclaration);
}


TEST_CASE("paranthesis"){
	stringstream ss("(int x)");
	SourceTree st;
	st.parse(ss);

	st.print(cout, 0);
	ASSERT_EQ(st.front().front().name, "int");

}

TEST_CASE("functions"){
	stringstream ss("int main(int x) {return x;}");
	SourceTree st;
	st.parse(ss);
	st.secondPass();

	st.print(cout, 0);
}

TEST_CASE("assignment"){
	stringstream ss("int i = 0;");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.secondPass();
	sourceTree.print(cout, 0);
}

TEST_CASE("templates"){
	stringstream ss("template <class T> int apa(T bepa);");

	SourceTree st;
	st.parse(ss);

	st.secondPass();
	st.print(cout, 0);
}

TEST_CASE("multiple character operator"){
	stringstream ss("+= ");
	SourceTree sourceTree;
	sourceTree.parse(ss);

	ASSERT_EQ(sourceTree.front().name, "+=");
	ASSERT_EQ(sourceTree.front().type, SourceTree::Operator);
}

TEST_SUIT_END

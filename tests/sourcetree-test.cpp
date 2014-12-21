
#include "rootsourcetree.h"
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
}




TEST_CASE("second pass"){
	stringstream ss("int main(int argc, char **argv)\n"
			"{ \n"
			" int x;\n"
			" cout << \"hello world\" << \"tjoho\" << endl;\n"
			" return 0;\n"
			" }\n");

	SourceTree sourceTree;
	sourceTree.parse(ss);

	sourceTree.secondPass();
}




TEST_CASE("basic data type"){
	auto sourceTree = RootSourceTree::CreateFromString("int apa;");

	ASSERT(sourceTree.front().front().dataType, "ingen content (innehåll)");

	ASSERT_EQ(sourceTree.front().size(), 2);
	ASSERT_EQ(sourceTree.front().front().type(), SourceTree::Type);
	ASSERT_EQ(sourceTree.front().back().type(), SourceTree::DefinitionName);
}



TEST_CASE("variable declaration"){
	auto st = RootSourceTree::CreateFromString(""
			"int apa;"
			"apa = apa + 1;"
			"int bepa = 0;");

	auto variable = st.findVariable("apa");
	ASSERT(variable, "apa not found");

	variable = st.findVariable("bepa");
	ASSERT(variable, "bepa not found");

	//Check that the second expression necognises the variable
}




TEST_CASE("user defined datatype"){
	auto st = RootSourceTree::CreateFromString("class Apa { int x }; Apa apa;");

	string name("Apa");

	ASSERT(st.findDataType(name), "data type not found");
}



TEST_CASE("preprocessor command"){
	auto st = RootSourceTree::CreateFromString(
			"#pragma once \n"
			"int apa; ");
}




TEST_CASE("lambda functions"){
	ERROR_NOT_IMPLEMENTED();
	{
		auto st = RootSourceTree::CreateFromString("[] (int x) {cout << \"hej\" << endl; } ");
		ASSERT_EQ(st.front().type(), SourceTree::LambdaFunction);
	}
	{
		auto st = RootSourceTree::CreateFromString("auto f = [] (int x) {cout << \"hej\" << endl; }; ");
	}
}




TEST_CASE("multiple word datatypes"){
	auto sourceTree = RootSourceTree::CreateFromString("long int **apa;");

	ASSERT_EQ(sourceTree.front().pointerDepth, 0);
	ASSERT_EQ(sourceTree.front().type(), SourceTree::VariableDeclaration);
	ASSERT_EQ(sourceTree.front().front().pointerDepth, 2);
	ASSERT_EQ(sourceTree.front().front()._name, "long int");
}




TEST_CASE("for"){
	auto sourceTree = RootSourceTree::CreateFromString("for (int i = 0; i < 10; ++i) { int x = i; }");
	ERROR_NOT_IMPLEMENTED();
}



TEST_CASE("class declaration"){
	auto st = RootSourceTree::CreateFromString("template <class T> class Apa { int x; };");

	ASSERT_EQ(st.front().type(), SourceTree::ClassDeclaration);
}




TEST_CASE("struct declaration"){
	auto st = RootSourceTree::CreateFromString("template <class T> struct apa { int x; };");

	ASSERT_EQ(st.front().type(), SourceTree::StructDeclaration);
}




TEST_CASE("paranthesis"){
	stringstream ss("(int x)");
	SourceTree st;
	st.parse(ss);

	st.print(cout, 0);
	ASSERT_EQ(st.front().front()._name, "int");
}



TEST_CASE("functions"){
	auto st = RootSourceTree::CreateFromString("int main(int x) {return x;}");
}




TEST_CASE("assignment"){
	auto st = RootSourceTree::CreateFromString("int i = 0;");
}




TEST_CASE("ambiguous statements") {
	auto st = RootSourceTree::CreateFromString(
			"int *x = 0;"
			"int y, z;"
			"y * z;"
			"class Apa {};"
			"Apa *apa;"
			);
	string className = "Apa";
	ASSERT(st.findDataType(className), "cannot find class Apa");
}



TEST_CASE("templates"){
	auto st = RootSourceTree::CreateFromString("template <class T> int apa(T bepa);");
}




TEST_CASE("multiple character operator"){
	stringstream ss("+= ");
	SourceTree sourceTree;
	sourceTree.parse(ss);

	ASSERT_EQ(sourceTree.front()._name, "+=");
	ASSERT_EQ(sourceTree.front().type(), SourceTree::Operator);
}


TEST_CASE("group expressions by operators") {
	auto st = RootSourceTree::CreateFromString(
			"int x = 2;"
			"int y = 3;"
			"int z = 3;"
			"int apa = x + y + (z + x) * (z - x) ^ 2;"
			);
	ERROR_NOT_IMPLEMENTED();
}


TEST_CASE("complete expression namespaces"){
	{
		auto st = RootSourceTree::CreateFromString("int Apa");
		auto ret = st.completeExpression("Ap");
		ASSERT_GT(ret.size(), 0);
		for (auto it: ret) {
			cout << it->getFullName() << endl;
		}
		ASSERT_EQ(ret.front()->getFullName(), "Apa");
	}

	{ //Namespaces
		auto st = RootSourceTree::CreateFromString(
				"namespace Apa {"
				"	int bepa;"
				"	namespace Bepa {"
				"		int cepa;"
				"	}"
				"}");

		auto ret = st.completeExpression("Apa::be");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "bepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::bepa");

		ret = st.completeExpression("Apa::Bepa::ce");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "cepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::Bepa::cepa");
	}

}




TEST_CASE("complete expression classes") {
	{
		auto st = RootSourceTree::CreateFromString(
				"class Apa {"
				"static int bepa;"
				"};"
				);

		auto ret = st.completeExpression("Apa::be");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "bepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::bepa");
	}

	{
		auto st = RootSourceTree::CreateFromString(
				"class Apa {"
				"int bepa;"
				"};"
				"Apa apa;"
				);

		auto ret = st.completeExpression("apa.be");
		ASSERT_GT(ret.size(), 0);
		ASSERT_EQ(ret.front()->getLocalName(), "bepa");
		ASSERT_EQ(ret.front()->getFullName(), "Apa::bepa");
	}
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

	auto ret = st.completeExpression("Apa::Apa::ap");
	ASSERT_GT(ret.size(), 0);
	ASSERT_EQ(ret.front()->getFullName(), "Apa::Apa::apa");

	ret = st.completeExpression("bepa.ce");
	ASSERT_GT(ret.size(), 0);
	ASSERT_EQ(ret.front()->getFullName(), "Apa::Apa::Bepa::cepa");
}




TEST_CASE("namespaced datatype") {
	auto st = RootSourceTree::CreateFromString(
			"namespace Apa {"
			"	class Apa {"
			"	};"
			"}"
			"Apa::Apa apa;"
	);

	string name = "Apa::Apa";
	auto ret = st.findDataType(name);
	ASSERT(ret, "datatype not found");
	ASSERT_EQ(ret->getFullName(), "Apa::Apa");

	name = "apa";
	ret = st.findVariable(name);
	ASSERT(ret, "variable not found");
}



TEST_SUIT_END

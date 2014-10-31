
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
	sourceTree.print(cout, 0);
}

TEST_CASE("basic data type"){
	stringstream ss("int apa");

	SourceTree sourceTree;
	sourceTree.parse(ss);

	ASSERT(sourceTree.content, "ingen content (innehåll)");

	ASSERT_EQ(sourceTree.content->type, SourceContent::DataType);

	return 1;
}

TEST_CASE("multiple word datatypes"){
	stringstream ss("long int **apa;");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.type = SourceTree::BraceBlock;

	sourceTree.print(cout, 0);
	sourceTree.secondPass();
	sourceTree.print(cout, 0);

	ASSERT_EQ(sourceTree.front().pointerDepth, 0);
	ASSERT_EQ(sourceTree.front().type, SourceTree::VariableDeclaration);
	ASSERT_EQ(sourceTree.front().front().pointerDepth, 2);
	ASSERT_EQ(sourceTree.front().front().name, "long int");
	ASSERT_EQ(sourceTree.size(), 1); //Should in the end only be one statement
}


TEST_CASE("multiple word datatypes"){
	stringstream ss("for (int i = 0; i < 10; ++i) { int x = i; }");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.type = SourceTree::BraceBlock;
	sourceTree.secondPass();

	sourceTree.print(cout, 0);
}

TEST_CASE("class declaration"){
	stringstream ss("class apa { int x; };");

	SourceTree sourceTree;
	sourceTree.parse(ss);
	sourceTree.type = SourceTree::BraceBlock;
	sourceTree.secondPass();
	sourceTree.print(cout, 0);
}
TEST_SUIT_END


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

	return 0;
}

TEST_CASE("basic data type"){
	stringstream ss("int apa");

	SourceTree sourceTree;
	sourceTree.parse(ss);

	ASSERT(sourceTree.content, "ingen content (innehåll)");

	ASSERT_EQ(sourceTree.content->type, SourceContent::DataType);

	return 1;
}

TEST_SUIT_END

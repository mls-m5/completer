SRC=$(wildcard *.cpp)
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(wildcard tests/*.cpp)
TEST_BIN=$(TEST_SRC:.cpp=)
CFLAGS+=--std=c++11 -g

all: $(OBJECTS) $(TEST_BIN)
	@echo Klart
	
%.o: %.cpp *.cpp *.h
	g++ $< -c -o $@ $(CFLAGS)
	
tests/%: tests/%.cpp *.cpp *.h
	g++ $< $(OBJECTS) -o $@ -I. $(CFLAGS)



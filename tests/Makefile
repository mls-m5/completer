SRC=$(wildcard ../src/*.cpp)
CXX=g++
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(wildcard *.cpp)
TEST_BIN=$(TEST_SRC:.cpp=)
CFLAGS+=--std=c++11 -g -O0  -Iinclude/ -I../include/
LIB=../completer.a

#first: tests/completer-test #to only build one test
#first: tests/sourcetree-test

all: $(OBJECTS) $(TEST_BIN)
	@echo Klart
	
%.o: %.cpp *.h
	$(CXX) $< -c -o $@ $(CFLAGS)
	
%-test: %-test.cpp *.cpp *.h $(OBJECTS)
	$(CXX) $< $(LIB) -o $@ -I. $(CFLAGS) $(LIB)
	
clean:
	rm -f $(OBJECTS) $(BIN) $(TEST_BIN) main/main.o



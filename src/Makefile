SRC=$(wildcard *.cpp)
CXX=g++
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(wildcard tests/*.cpp)
TEST_BIN=$(TEST_SRC:.cpp=)
CFLAGS+=--std=c++11 -g -O0  -Iinclude/ -I../include/
BIN=../completer
LIB=../completer.a

#first: tests/completer-test #to only build one test
#first: tests/sourcetree-test

all: $(OBJECTS) $(TEST_BIN) $(BIN) $(LIB)
	@echo Klart
	
%.o: %.cpp *.h
	$(CXX) $< -c -o $@ $(CFLAGS)
	
../main/main/%.o: ../main/main/%.cpp *.h $(OBJECTS)
	$(CXX) $< -c -o $@ $(CFLAGS)

$(BIN): ../main/main.o
	$(CXX) $< $(LIB) -o $@ -I. $(CFLAGS)
	
$(LIB): $(OBJECS)
	ar rsv $(LIB) $(OBJECTS)

tests/%: tests/%.cpp *.cpp *.h $(OBJECTS)
	$(CXX) $< $(OBJECTS) -o $@ -I. $(CFLAGS)
	
clean:
	rm -f $(OBJECTS) $(BIN) $(TEST_BIN) main/main.o



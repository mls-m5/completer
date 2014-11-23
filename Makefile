SRC=$(wildcard *.cpp)
CXX=clang++
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(wildcard tests/*.cpp)
TEST_BIN=$(TEST_SRC:.cpp=)
CFLAGS+=--std=c++11 -g -O0
BIN=completer

all: $(OBJECTS) $(TEST_BIN) $(BIN)
	@echo Klart
	
%.o: %.cpp *.h
	$(CXX) $< -c -o $@ $(CFLAGS)
	
main/%.o: main/%.cpp *.h $(OBJECTS)
	$(CXX) $< -c -o $@ $(CFLAGS)

$(BIN): main/main.o
	$(CXX) $< $(OBJECTS) -o $@ -I. $(CFLAGS)

tests/%: tests/%.cpp *.cpp *.h
	$(CXX) $< $(OBJECTS) -o $@ -I. $(CFLAGS)
	
clean:
	rm -f $(OBJECTS) $(BIN) $(TEST_BIN) main/main.o



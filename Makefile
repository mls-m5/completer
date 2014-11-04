SRC=$(wildcard *.cpp)
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(wildcard tests/*.cpp)
TEST_BIN=$(TEST_SRC:.cpp=)
CFLAGS+=--std=c++11 -g
BIN=completer

all: $(OBJECTS) $(TEST_BIN) $(BIN)
	@echo Klart
	
%.o: %.cpp *.h
	g++ $< -c -o $@ $(CFLAGS)
	
main/%.o: main/%.cpp *.h $(OBJECTS)
	g++ $< -c -o $@ $(CFLAGS)

$(BIN): main/main.o
	g++ $< $(OBJECTS) -o $@ -I. -I../ $(CFLAGS)

tests/%: tests/%.cpp *.cpp *.h
	g++ $< $(OBJECTS) -o $@ -I. $(CFLAGS)



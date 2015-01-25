BIN=completer
LIB=completer.a

#first: tests/completer-test #to only build one test
#first: tests/sourcetree-test

all: ${LIB} ${BIN}
	@echo Klart
	
${BIN}:
	make -C src/ ../${BIN}
	
clean:
	make -C src/
	make -C tests/



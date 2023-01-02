#all:
	#@echo "no default target"

build: sfr-lisp build_test

build_test: nassert_test Tokenizer_test

nassert_test: nassert_test.c nassert.h
	gcc -o nassert_test nassert_test.c

#=== { Objects { ===
Tokenizer.o: Tokenizer.c Tokenizer.h
	gcc -c Tokenizer.c
String.o: String.h String.c
	gcc -c String.c
#=== } Objects } ===

Tokenizer_test: Tokenizer_test.c Tokenizer.o nassert.h
	gcc -O3 -o Tokenizer_test Tokenizer_test.c Tokenizer.o

Tokenizer_test_run: Tokenizer_test
	./Tokenizer_test

test: build_test
	./nassert_test
	./Tokenizer_test

sfr-lisp: sfr-lisp.c Tokenizer.o String.o
	gcc -o sfr-lisp sfr-lisp.c Tokenizer.o String.o

clean:
	rm -f *.o
	rm -f sfr-lisp

.PHONY: test 

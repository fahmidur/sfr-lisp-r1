#CFLAGS := -g -fsanitize=address
CFLAGS := -g 

build: sfr-lisp build_test

build_test: nassert_test Tokenizer_test Atom_test

nassert_test: nassert_test.c nassert.h
	$(CC) -o nassert_test nassert_test.c

Tokenizer.o: Tokenizer.c Tokenizer.h
	$(CC) $(CFLAGS) -c Tokenizer.c

String.o: String.h String.c
	$(CC) $(CFLAGS) -c String.c

Atom.o: Atom.c Atom.h

Tokenizer_test: Tokenizer_test.c Tokenizer.o nassert.h
	$(CC) $(CFLAGS) -o Tokenizer_test Tokenizer_test.c Tokenizer.o

Atom_test: Atom.o Atom_test.c
	$(CC) $(CFLAGS) -o Atom_test Atom_test.c Atom.o

test: build_test
	./nassert_test
	./Tokenizer_test

sfr-lisp: sfr-lisp.c Tokenizer.o String.o
	$(CC) $(CFLAGS) -o sfr-lisp sfr-lisp.c Tokenizer.o String.o

clean:
	rm -f *.o
	rm -f sfr-lisp
	rm -f Atom_test
	rm -f Tokenizer_test
	rm -f nassert_test

.PHONY: test 

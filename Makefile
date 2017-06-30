CC = gcc

allagain: clean all

all: main.o  obrobka.o
	$(CC) -pedantic -Wall main.o obrobka.o -lm -o test

main.o: main.c obrobka.h
	$(CC) -pedantic -Wall -c main.c -o main.o

obrobka.o: obrobka.c obrobka.h
	$(CC) -pedantic -Wall -c obrobka.c -o obrobka.o

clean:
	rm -f *.o
	rm -f test
	rm -f tmp.pgm

tar:
	tar -cvf 235291Obrazy2.tar Makefile *.c *.h

uu: tar
	uuencode 235291Obrazy2.tar 235291Obrazy2.tar > 235291Obrazy2.tar.uu

zip:
	mkdir myziptmp
	cp *.c myziptmp
	cp Makefile myziptmp
	zip -r 235291Obrazy2.zip myziptmp
	rm myziptmp/*
	rmdir myziptmp

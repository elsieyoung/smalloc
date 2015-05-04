all : simpletest

simpletest : simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o

mytest : mytest.o smalloc.o testhelpers.o
	gcc -Wall -g -o mytest smalloc.o testhelpers.o mytest.o

tests : mytest.o simpletest.o smalloc.o testhelpers.o
	gcc -Wall -g -o mytest smalloc.o testhelpers.o mytest.o
	gcc -Wall -g -o simpletest simpletest.o smalloc.o testhelpers.o
	./mytest
	./simpletest

%.o : %.c smalloc.h
	gcc -Wall -g -c $<

clean : 
	rm simpletest mytest *.o

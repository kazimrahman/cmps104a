# $Id: Makefile,v 1.5 2014-10-03 19:01:09-07 - - $

GPP   = g++ -g -O0 -Wall -Wextra -std=gnu++11
GRIND = valgrind --leak-check=full --show-reachable=yes


all : teststring

test : teststring
	teststring test.oc

teststring : main.o stringset.o
	${GPP} main.o stringset.o -o teststring

%.o : %.cpp
	${GPP} -c $<

ci :
	git add *.h *.cpp Makefile && \
	git commit && \
	git push https://github.com/smorad/cmps104a master

spotless : clean
	- rm teststring Listing.ps Listing.pdf test.str\

clean :
	-rm stringset.o main.o

test.out : teststring
	${GRIND} teststring * * * >test.out 2>&1

lis : test.out
	mkpspdf Listing.ps stringset.h stringset.cpp main.cpp \
	        Makefile test.out

# Depencencies.
main.o: main.cpp stringset.h
stringset.o: stringset.cpp stringset.h


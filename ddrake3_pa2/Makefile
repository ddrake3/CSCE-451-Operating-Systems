CXX = g++
CXXFLAGS = -Wall -g -O3 -std=c++11 -pedantic

TESTDIR = testscripts
T6 = 6.singlePipe.txt
T7 = 7.moreLogical.txt
T8 = 8.morePipes.txt
T9 = 9.simplePipeAndLogical.txt

A6 = ea6.txt
A7 = ea7.txt
A8 = ea8.txt
A9 = ea9.txt

.PHONY: all
all: osh

osh: main.o parser.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

main.o: main.cpp parser.hpp command.hpp
parser.o: parser.cpp parser.hpp

.PHONY: clean
clean:
	rm -rf osh *.o

test6:
	./osh -t < $(TESTDIR)/$(T6) > tmp 2>61; diff tmp $(TESTDIR)/$(A6); rm tmp

test7:
	./osh -t < $(TESTDIR)/$(T7) > tmp 2>61; diff tmp $(TESTDIR)/$(A7); rm tmp

test8:
	./osh -t < $(TESTDIR)/$(T8) > tmp 2>61; diff tmp $(TESTDIR)/$(A8); rm tmp

test9:
	./osh -t < $(TESTDIR)/$(T9) > tmp 2>61; diff tmp $(TESTDIR)/$(A9); rm tmp

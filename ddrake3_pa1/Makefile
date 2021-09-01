CXX = g++
CXXFLAGS = -Wall -g -O3 -std=c++11 -pedantic

TESTDIR = testscripts
T1 = 1.singleCommand.txt
T2 = 2.simpleRedir.txt
T3 = 3.moreRedir.txt
T4 = 4.logicalConditional.txt
T5 = 5.malformed.txt

A1 = ea1.txt
A2 = ea2.txt
A3 = ea3.txt
A4 = ea4.txt
A5 = ea5.txt

.PHONY: all
all: osh

osh: main.o parser.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

main.o: main.cpp parser.hpp command.hpp
parser.o: parser.cpp parser.hpp

.PHONY: clean
clean:
	rm -rf osh *.o

test1:
	./osh -t < $(TESTDIR)/$(T1) > tmp 2>61; diff tmp $(TESTDIR)/$(A1); rm tmp

test2:
	./osh -t < $(TESTDIR)/$(T2) > tmp 2>61; diff tmp $(TESTDIR)/$(A2); rm tmp

test3:
	./osh -t < $(TESTDIR)/$(T3) > tmp 2>61; diff tmp $(TESTDIR)/$(A3); rm tmp

test4:
	./osh -t < $(TESTDIR)/$(T4) > tmp 2>61; diff tmp $(TESTDIR)/$(A4); rm tmp

test5:
	./osh -t < $(TESTDIR)/$(T5) > tmp 2>61; diff tmp $(TESTDIR)/$(A5); rm tmp

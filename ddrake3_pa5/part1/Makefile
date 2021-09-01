CXX = g++
CXXFLAGS = -std=c++11 -I.

TESTDIR = test
backing_store = BACKING_STORE.bin
addresses = addresses.txt

correct = correct.txt
givenCorrect = givenCorrect.txt

.PHONY: all
all: part1

part1: part1.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

part1.o: part1.cpp

.PHONY: clean
clean:
	rm -rf part1 *.o

testcase:
	./part1 $(TESTDIR)/$(backing_store) $(TESTDIR)/$(addresses); diff $(correct) $(TESTDIR)/$(givenCorrect);

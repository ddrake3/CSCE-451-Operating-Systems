CXX = g++
CXXFLAGS = -std=c++11 -I.

TESTDIR = test

backing_store = BACKING_STORE.bin
addresses1 = addresses1.txt
addresses2 = addresses2.txt

fifo = fifo
lru = lru

correct = correct.txt

givenCorrect1_fifo =	givenCorrect1_fifo.txt
givenCorrect2_fifo = givencorrect2_fifo.txt
givenCorrect1_lru = givenCorrect1_lru.txt
givenCorrect2_lru = givenCorrect2_lru.txt

.PHONY: all
all: part2

part2: part2.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^

part2.o: part2.cpp

.PHONY: clean
clean:
	rm -rf part2 *.o

testcase1:
	./part2 $(TESTDIR)/$(backing_store) $(TESTDIR)/$(addresses1) $(fifo); diff $(correct) $(TESTDIR)/$(givenCorrect1_fifo);
testcase2:
	./part2 $(TESTDIR)/$(backing_store) $(TESTDIR)/$(addresses1) $(lru); diff $(correct) $(TESTDIR)/$(givenCorrect1_lru);
testcase3:
	./part2 $(TESTDIR)/$(backing_store) $(TESTDIR)/$(addresses2) $(fifo); diff $(correct) $(TESTDIR)/$(givenCorrect2_fifo);
testcase4:
	./part2 $(TESTDIR)/$(backing_store) $(TESTDIR)/$(addresses2) $(lru); diff $(correct) $(TESTDIR)/$(givenCorrect2_lru);

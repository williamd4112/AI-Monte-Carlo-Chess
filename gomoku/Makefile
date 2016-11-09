CC=g++
CFLAGS=-O3 -std=c++11

all:
	g++ -O3 main.cpp -o mcts-gomoku -std=c++11

test_tss: test_tss.cpp tss.h pattern.h
	$(CC) $(CFLAGS) test_tss.cpp -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

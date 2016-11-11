CC=g++
CFLAGS= -Wall -O3 -std=c++11 -D_UNIX_SLEEP -D_UNIX

all:
	g++ -O3 main.cpp -o mcts-gomoku -std=c++11

test_policy: test_policy.cpp fast_tss.h pattern.h policy.h
	$(CC) $(CFLAGS) -D_DEBUG_POLICY  test_policy.cpp -o $@

test_fast_tss: test_fast_tss.cpp fast_tss.h pattern.h
	$(CC) $(CFLAGS) -D_LOG_FAST_TSS -D_DEBUG_FAST_TSS test_fast_tss.cpp -o $@

test_tss: test_tss.cpp tss.h pattern.h
	$(CC) $(CFLAGS) -DDFS_TSS test_tss.cpp -o $@

test_tss2: test_tss2.cpp tss.h pattern.h
	$(CC) $(CFLAGS)  test_tss2.cpp -o $@

test_board: test_board.cpp board.h
	$(CC) $(CFLAGS)  test_board.cpp -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

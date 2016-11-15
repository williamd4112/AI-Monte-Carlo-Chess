CC = g++
CFLAGS = -Wall -std=c++11 -D_UNIX_SLEEP -D_UNIX -D_LOG_MCTS

OBJS = state.o policy.o fast_tss.o pattern.o board.o util.o simulation.o simulation_usual.o sim.o

all: $(OBJS)
	g++ $(CFLAGS) -O3  main.cpp $(OBJS) -o mcts-gomoku -std=c++11

debug: $(OBJS)
	g++ -g $(CFLAGS) main.cpp $(OBJS) -o mcts-gomoku-dbg -std=c++11

test_util: test_util.cpp util.h state.h
	$(CC) $(CFLAGS) -O3 -D_DEBUG_UTIL test_util.cpp -o $@

test_sim:
	$(CC) $(CFLAGS) -O3 test_sim.cpp util.cpp board.cpp sim.cpp policy.cpp state.cpp pattern.cpp fast_tss.cpp simulation.cpp simulation_usual.cpp $< -o $@

test_policy: fast_tss.o pattern.o policy.o util.o
	$(CC) $(CFLAGS) -O3 -D_DEBUG_POLICY test_policy.cpp $< -o $@

test_fast_tss: $(OBJS)
	$(CC) $(CFLAGS)-O3 -D_LOG_FAST_TSS test_fast_tss.cpp $(OBJS) -o $@

test_tss: test_tss.cpp tss.h pattern.h
	$(CC) $(CFLAGS) -O3 -DDFS_TSS test_tss.cpp -o $@

test_tss2: test_tss2.cpp tss.h pattern.h
	$(CC) $(CFLAGS) -O3 test_tss2.cpp -o $@

test_board: test_board.cpp board.h
	$(CC) $(CFLAGS)-O3  test_board.cpp -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o

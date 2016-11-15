CC=g++
CFLAGS = -Wall -O3 -std=c++11 -D_UNIX_SLEEP -D_UNIX -D_DEBUG_MCTS -D_LOG_FAST_TSS
#CFLAGS += -fopenmp

OBJS = state.o policy.o fast_tss.o pattern.o board.o util.o simulation.o simulation_usual.o sim.o

all: $(OBJS)
	g++ $(CFLAGS) main.cpp $(OBJS) -o mcts-gomoku -std=c++11

test_util: test_util.cpp util.h state.h
	$(CC) $(CFLAGS) -D_DEBUG_UTIL test_util.cpp -o $@

test_sim:
	$(CC) $(CFLAGS) test_sim.cpp util.cpp board.cpp sim.cpp policy.cpp state.cpp pattern.cpp fast_tss.cpp simulation.cpp simulation_usual.cpp $< -o $@

test_policy: fast_tss.o pattern.o policy.o util.o
	$(CC) $(CFLAGS)  -D_DEBUG_POLICY test_policy.cpp $< -o $@

test_fast_tss: test_fast_tss.cpp fast_tss.h pattern.h
	$(CC) $(CFLAGS) -D_LOG_FAST_TSS test_fast_tss.cpp -o $@

test_tss: test_tss.cpp tss.h pattern.h
	$(CC) $(CFLAGS) -DDFS_TSS test_tss.cpp -o $@

test_tss2: test_tss2.cpp tss.h pattern.h
	$(CC) $(CFLAGS)  test_tss2.cpp -o $@

test_board: test_board.cpp board.h
	$(CC) $(CFLAGS)  test_board.cpp -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o

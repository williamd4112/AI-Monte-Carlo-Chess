CC = g++
CFLAGS = -Wall -std=c++11 -D_UNIX_SLEEP -D_UNIX
CFLAGS += -O3
#CFLAGS += -g
#CFLAGS += -D_LOG_FAST_TSS -D_DEBUG_FAST_TSS
#CFLAGS += -D_LOG_POLICY -D_DEBUG_POLICY
OBJS = state.o policy.o fast_tss.o pattern.o board.o util.o simulation.o simulation_usual.o sim.o

OPT :=

all: $(OBJS)
	echo $(OPT)
	g++ $(CFLAGS) main.cpp $(OBJS) -o mcts-gomoku -std=c++11

test_mcts_case: $(OBJS)
	g++ $(CFLAGS) test/test_mcts_cases.cpp $(OBJS) -o test_mcts_cases -std=c++11

debug: $(OBJS)
	g++ $(DBG) $(CFLAGS) main.cpp $(OBJS) -o mcts-gomoku-dbg -std=c++11

test_util: test_util.cpp util.h state.h
	$(CC) $(CFLAGS) -O3 -D_DEBUG_UTIL test_util.cpp -o $@

test_sim:
	$(CC) $(CFLAGS) -O3 test_sim.cpp util.cpp board.cpp sim.cpp policy.cpp state.cpp pattern.cpp fast_tss.cpp simulation.cpp simulation_usual.cpp $< -o $@

test_policy: $(OBJS)
	$(CC) $(CFLAGS) test_policy.cpp $(OBJS) -o $@

test_fast_tss: $(OBJS) test_fast_tss.cpp
	$(CC) $(CFLAGS) -O3 test_fast_tss.cpp $(OBJS) -o $@

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

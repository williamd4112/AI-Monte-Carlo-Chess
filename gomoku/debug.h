#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cassert>

#ifdef _DEBUG
#define DEBUG(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG(args...)
#endif

#ifdef _DEBUG_UTIL
#define DEBUG_UTIL(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG_UTIL(args...)
#endif

#ifdef _DEBUG_TSS
#define DEBUG_TSS(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG_TSS(args...)
#endif

#ifdef _DEBUG_FAST_TSS
#define DEBUG_FAST_TSS(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#define DEBUG_FAST_TSS_POSITION(pos) print_position(std::cout, (pos));
#else
#define DEBUG_FAST_TSS(args...)
#define DEBUG_FAST_TSS_POSITION(pos)
#endif

#ifdef _LOG_FAST_TSS
#define LOG_FAST_TSS(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#define LOG_FAST_TSS_POSITION(pos) print_position(std::cout, (pos));
#else
#define LOG_FAST_TSS(args...)
#define LOG_FAST_TSS_POSITION(pos)
#endif

#ifdef _DEBUG_BOARD
#define DEBUG_BOARD(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG_BOARD(args...)
#endif

#ifdef _DEBUG_PATTERN
#define DEBUG_PATTERN(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG_PATTERN(args...)
#endif

#ifdef _DEBUG_POLICY
#define DEBUG_POLICY(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG_POLICY(args...)
#endif

#ifdef _LOG_POLICY
#define LOG_POLICY(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define LOG_POLICY(args...)
#endif

#ifdef _LOG_MCTS
#define LOG_MCTS(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#define LOG_MCTS_EXPAND(states) for (auto & s : (states)) std::cout << "Expansion" << endl << s << std::endl
#ifdef _LOG_MCTS_SIM
#define LOG_MCTS_SIM_STATE(state) std::cout << "Simulate on state:\n" << (state) << std::endl
#define LOG_MCTS_SIM_PAYOFF(payoffs) { for (int i = 0; i < (int)(payoffs).size(); i++) printf("payoffs[%d] = %f\n", i, payoffs[i]); }
#else
#define LOG_MCTS_SIM_STATE(state)
#define LOG_MCTS_SIM_PAYOFF(payoffs)
#endif
#define LOG_MCTS_BP_STATE(state, payoff, simulation_count) std::cout << "Backpropgate on state(" << payoff << ", " << simulation_count << ")\n" << (state)
#else
#define LOG_MCTS(args...)
#define LOG_MCTS_EXPAND(states)
#define LOG_MCTS_BP_STATE(state, payoff, simulation_count)
#endif

#ifdef _DEBUG_SIM
#define DEBUG_SIM(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#define DEBUG_SIM_STATE(s) (std::cout << (s) << std::endl)
#else
#define DEBUG_SIM(args...)
#define DEBUG_SIM_STATE(s)
#endif

#endif

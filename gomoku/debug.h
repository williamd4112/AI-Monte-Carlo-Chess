#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cstdio>
#include <cassert>
#include "util.h"

#ifdef _DEBUG
#define DEBUG(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#else
#define DEBUG(args...)
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

#ifdef _DEBUG_SIM
#define DEBUG_SIM(format, args...) printf( "[%s:%d] " format, __FILE__, __LINE__, ##args)
#define DEBUG_SIM_STATE(s) (std::cout << (s) << std::endl)
#else
#define DEBUG_SIM(args...)
#define DEBUG_SIM_STATE(s)
#endif

#endif

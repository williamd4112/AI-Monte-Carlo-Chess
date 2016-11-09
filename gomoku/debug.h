#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <cstdio>

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


#endif

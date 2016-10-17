//自定義的 調試用的 宏
#ifndef KING_LIB_HEADER_DEBUG
#define KING_LIB_HEADER_DEBUG

#include <iostream>
#include <cstdio>

#ifdef KING_DEBUG
//調試段 定義
#define KING_DEBUG_SECTION(XXX) XXX

//puts and printf
#define KING_DEBUG_PUTS(XXX)    puts(XXX);
#define KING_DEBUG_PRINTF(format,...)    printf(format,__VA_ARGS__);

//std::cout
#define KING_DEBUG_COUT(XXX) std::cout<<XXX;
//std::cout and add \n at end
#define KING_DEBUG_COUT_LINE(XXX) std::cout<<XXX<<"\n";
#else
#define KING_DEBUG_SECTION(XXX)

#define KING_DEBUG_PUTS(XXX)
#define KING_DEBUG_PRINTF(format,...)

#define KING_DEBUG_COUT(XXX)
#define KING_DEBUG_COUT_LINE(XXX)
#endif // KING_DEBUG

#endif

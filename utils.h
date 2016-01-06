#pragma once
#include <stdint.h>

#define JS_DEBUG

#ifdef JS_DEBUG
#define dbgprint(format,args...) \
	fprintf(stderr, format, ##args)
#else
	#define dbgprint(format,args...)
#endif

// TODO:use clean up function instead of exit
#define FATAL_ERROR(format,args...) \
	fprintf(stderr, format, ##args),\
	exit(-1)

#define CHAR_BIT 8

typedef u64 uint64_t
typedef u32 uint32_t
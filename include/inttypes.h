#ifndef __IBSENOS_INTEGER_TYPES_H__
#define __IBSENOS_INTEGER_TYPES_H__


/*
 * Some integer limits.
 */
#define INT8_MAX 127
#define INT8_MIN (-INT8_MAX-1)
#define UINT8_MAX 255
#define INT16_MAX 32767
#define INT16_MIN (-INT16_MAX-1)
#define UINT16_MAX 65535
#define INT32_MAX 2147483647L
#define INT32_MIN (-INT32_MAX-1L)
#define UINT32_MAX 4294967295L
#define INT64_MAX 9223372036854775807LL
#define INT64_MIN (-INT64_MAX-1LL)
#define UINT64_MAX 18446744073709551615ULL
#define SIZE_MAX INT32_MAX
#define SSIZE_MAX UINT32_MAX


typedef signed long int ssize_t;

typedef unsigned long int size_t;


typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int int16_t;
_Static_assert(sizeof(int16_t) == 2, "int16_t must be 2 bytes");
typedef unsigned short int uint16_t;
_Static_assert(sizeof(uint16_t) == 2, "uint16_t must be 2 bytes");

typedef signed long int int32_t;
_Static_assert(sizeof(int32_t) == 4, "int32_t must be 4 bytes");
typedef unsigned long int uint32_t;
_Static_assert(sizeof(uint32_t) == 4, "uint32_t must be 4 bytes");

typedef signed long long int int64_t;
_Static_assert(sizeof(int64_t) == 8, "int64_t must be 8 bytes");
typedef unsigned long long int uint64_t;
_Static_assert(sizeof(uint64_t) == 8, "uint64_t must be 8 bytes");

/* TODO: int_least, int_fast, and intmax_t and uintmax_t intptr_t, uintptr_t */

#endif

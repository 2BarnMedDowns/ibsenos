/*
 * String manipulation convenience functions.
 */
#ifndef __IBSENOS_STRING_H__
#define __IBSENOS_STRING_H__

#include <inttypes.h>


size_t strlen(const char *str);


int strcmp(const char *str1, const char *str2);


int strncmp(const char *str1, const char *str2, size_t n);


size_t strcpy(char *restrict dst, const char *src);


size_t strncpy(char *restrict dst, const char *src, size_t n);


/*
 * Converts the unsigned 64-bit integer to its string representation
 * in the specified base.
 *
 * If base is 0, decimal (base 10) is assumed.
 *
 * The buffer must be long enough to store the number and the trailing
 * null-terminator in the specified base:
 *  - Binary (base 2): 65 bytes for
 *    "1111111111111111111111111111111111111111111111111111111111111111"
 *  - Octal (base 8): 23 bytes for "1777777777777777777777"
 *  - Decimal (base 10): 21 bytes for "18446744073709551615".
 *  - Hexadecimal (base 16): 17 bytes for "ffffffffffffffff"
 */
int u64tostr(uint64_t value, char *str, int base);



#endif

/*
 * String manipulation convenience functions.
 */
#ifndef __IBSENOS_STRING_H__
#define __IBSENOS_STRING_H__

#include <inttypes.h>



/*
 * Fill memory with with constant byte.
 *
 * Fill the first n bytes of the memory pointed to by s 
 * with the constant byte c.
 *
 * Returns a pointer to the memory area s.
 */
void * memset(void *s, int c, size_t n);


/*
 * Copy memory area.
 *
 * Copy n bytes from memory area src to memory area dst.
 * These memory areas must not overlap.
 *
 * If memory areas overlap, use memmove instead.
 *
 * Returns a pointer to dst.
 */
void * memcpy(void *restrict dst, const void *restrict src, size_t n);


/*
 * Copy memory area.
 * Identical to memcpy, but returns returns a pointer to dst + n
 */
void * mempcpy(void *restrict dst, const void *restrict src, size_t n);


/*
 * Compare memory areas.
 *
 * Compares the first n bytes (each interpreted as unsigned char) of the 
 * memory areas s1 and s2.
 *
 * For a nonzero return value, the sign is determined by the sign of the 
 * difference between the first pair of bytes (interpreted as unsigned char)
 * that differ in s1 and s2.
 *
 * If n is zero, the return value is zero.
 */
int memcmp(const void *s1, const void *s2, size_t n);


/*
 * Determine the length of a fixed-size string.
 *
 * Returns the number of characters in the string pointed to by str, 
 * excluding the terminating null byte, but at most n characters.  
 * In other words, only look at the first size characters in the string 
 * pointed to by str and never beyond str[n-1].
 */
size_t strnlen(const char *str, size_t n);



/*
 * Calculate the length of a null-terminated string.
 * Returns the number of characters in the string, excluding the 
 * terminating null-character.
 */
size_t strlen(const char *str);


/*
 * Compare two strings.
 *
 * Returns zero if the strings are equal.
 * Returns a negative value if str1 < str2
 * Returns a positive value if str1 > str2
 */
int strcmp(const char *str1, const char *str2);


/*
 * Compare two strings.
 *
 * Returns zero if the strings are equal.
 * Returns a negative value if str1 < str2
 * Returns a positive value if str1 > str2
 */
int strncmp(const char *str1, const char *str2, size_t n);


/*
 * Copy a string.
 *
 * Copies the string src into the buffer dst including the terminating
 * null character. The destination buffer dst must be large enough,
 * that is strlen(src) + 1.
 *
 * The two strings must not overlap.
 *
 * Returns a pointer to the terminating null-character in dst.
 */ 
char * stpcpy(char *restrict dst, const char *restrict src);


/*
 * Copy a string.
 *
 * Copies the string src into the buffer dst including the terminating
 * null character. The destination buffer dst must be large enough,
 * that is strlen(src) + 1.
 *
 * The two strings must not overlap.
 *
 * Returns a pointer to dst.
 */ 
char * strcpy(char *restrict dst, const char *restrict src);


/*
 * Copy a string to a fixed-size buffer.
 * 
 * Fill a fixed-size buffer with non-null bytes from a string, 
 * padding with null bytes as needed
 *
 * Copy characters from the string pointed to by src into the array pointed to by dst.
 * If the source has too few non-null bytes to fill the destination, the functions pad 
 * the destination with trailing null bytes. If the destination buffer isn't large 
 * enough to hold the copy, the resulting character sequence is truncated.
 *
 * Returns a pointer to one after the last character in dst.
 */ 
char * stpncpy(char *restrict dst, const char *restrict src, size_t n);


/*
 * Copy a string to a fixed-size buffer.
 * 
 * Fill a fixed-size buffer with non-null bytes from a string, 
 * padding with null bytes as needed
 *
 * Copy characters from the string pointed to by src into the array pointed to by dst.
 * If the source has too few non-null bytes to fill the destination, the functions pad 
 * the destination with trailing null bytes. If the destination buffer isn't large 
 * enough to hold the copy, the resulting character sequence is truncated.
 *
 * Returns a pointer to dst.
 */ 
char * strncpy(char *restrict dst, const char *restrict src, size_t n);


/*
 * In-place reverse a null-terminated string.
 * Returns a pointer to str.
 */
char * strrev(char *str);


/*
 * Reverse a fixed-sized string.
 */
char * strnrev(char *str, size_t n);


/*
 * Concatenate the string pointed to by str after the string pointed to dst
 * (overwriting the terminating null-character).
 *
 * The destination buffer dst must be large enough, that is 
 * strlen(dst) + strlen(src) + 1.
 *
 * Returns a pointer to dst.
 */
char * strcat(char *restrict dst, const char *restrict src);



// TODO strlcat strlcpy


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
 *  - Base36: 14 bytes for "3w5e11264sgse"
 *
 * Returns the number of digits/characters in the string representation,
 * or zero if converting the number was not possible.
 */
size_t u64tostr(uint64_t value, char *str, int base);

#endif

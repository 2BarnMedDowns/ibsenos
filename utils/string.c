#include <cdefs.h>
#include <string.h>
#include <inttypes.h>


// TODO: optimize the sh*t out of this
void * memset(void *s, int c, size_t n)
{
    unsigned char v = c;
    unsigned char *p = (unsigned char*) s;

    while (n-- > 0) {
        *p++ = v;
    }

    return s;
}


// TODO: optimize the sh*t out of this
void * mempcpy(void *restrict dst, const void *restrict src, size_t n)
{
    unsigned char *pdst = (unsigned char*) dst;
    unsigned char *psrc = (unsigned char*) src;

    while (n-- > 0) {
        *pdst++ = *psrc++;
    }

    return (void*) pdst;
}


void * memcpy(void *restrict dst, const void *restrict src, size_t n)
{
    mempcpy(dst, src, n);
    return dst;
}



// TODO: optimize the sh*t out of this
int memcmp(const void *s1, const void *s2, size_t n)
{
    unsigned char *s1p = (unsigned char*) s1;
    unsigned char *s2p = (unsigned char*) s2;

    while (n-- > 0) {
        unsigned char c1 = *s1p++;
        unsigned char c2 = *s2p++;
        int c = ((int) c1) - ((int) c2);
        if (c != 0) {
            return c;
        }
    }

    return 0;
}


size_t strnlen(const char *str, size_t n)
{
    size_t len = 0;

    while (n-- > 0) {
        if (*str++ == '\0') {
            return len;
        }
        ++len;
    }

    return len;
}


size_t strlen(const char *str)
{
    return strnlen(str, INT32_MAX);
}


char * stpncpy(char *restrict dst, const char *restrict src, size_t size)
{
    size_t len = strnlen(src, size);
    return memset(mempcpy(dst, src, len), 0, size - len);
}


char * strncpy(char *restrict dst, const char *restrict src, size_t size)
{
    stpncpy(dst, src, size);
    return dst;
}


char *stpcpy(char *restrict dst, const char *restrict src)
{
    char *p = mempcpy(dst, src, strlen(src));
    *p = '\0';
    return p;
}


char * strcpy(char *restrict dst, const char *restrict src)
{
    stpcpy(dst, src);
    return dst;
}


int strncmp(const char *str1, const char *str2, size_t size)
{
    size_t i = 0;
    char c1 = '\0';
    char c2 = '\0';

    while (i < size) {
        c1 = str1[i];
        c2 = str2[i];

        if (c1 != c2) {
            break;
        } else if (c1 == '\0') {
            break;
        } else if (c2 == '\0') {
            break;
        }
        ++i;
    }

    return ((int) c1) - ((int) c2);
}


int strcmp(const char *str1, const char *str2)
{
    return strncmp(str1, str2, INT32_MAX);
}


char * strnrev(char *str, size_t n)
{
    size_t i = 0;

    while (i < n) {
        char tmp = str[i];
        str[i] = str[n - 1];
        str[n - 1] = tmp;
        ++i;
        --n;
    }

    return str;
}


char * strrev(char *str)
{
    return strnrev(str, strlen(str));
}


char * strcat(char *restrict dst, const char *restrict src)
{
    stpcpy(dst + strlen(dst), src);
    return dst;
}


/*
 * Do a naive conversion to string with division and modulo.
 *
 * Note that this is extremely inefficient for large values,
 * so probably only useful for high bases in order to limit the 
 * number of iterations (13 for UINT64_MAX in base-36).
 */
static int naive_convert(uint64_t value, char *buffer, int base)
{
    int digits = 0;
    char reverse[64];

    do {
        int digit = value % base;
        value /= base;
        if (digit > 9) {
            digit += 'a' - '0' - 10;
        }
        reverse[digits++] = '0' + digit;
    } while (value > 0);

    for (int digit = 0; digit < digits; ++digit) {
        buffer[digit] = reverse[digits - 1 - digit];
    }

    buffer[digits] = '\0';
    return digits;
}


static int bitwise_convert(uint64_t value, char *buffer, int bits)
{
    int digits = 0;
    int mask = (1 << bits) - 1;
    int pos = 63 / bits * bits;
    
    do {
        int digit = (value >> pos) & mask;
        value -= (uint64_t) digit << pos;
        pos -= bits;
        
        if (digits != 0 || digit != 0 || pos < 0) {
            if (digit > 9) {
                digit += 'a' - '0' - 10;
            }
            buffer[digits++] = '0' + digit;
        }
    } while (pos >= 0);

    buffer[digits] = '\0';
    return digits;
}


static int to_decimal(uint64_t value, char *buffer)
{
    int digits = 0;
    int pos = 19;

    do {
        int i;
        uint64_t lim;

        for (i = 0, lim = 1; i < pos; ++i) {
            lim *= 10;
        }

        if (digits != 0 || value >= lim || pos == 0) {
            int digit;
            for (digit = 0; value >= lim; ++digit) {
                value -= lim;
            }
            buffer[digits++] = '0' + digit;
        }
    } while (pos--);

    buffer[digits] = '\0';
    return digits;
}


size_t u64tostr(uint64_t value, char *str, int base)
{
    switch (base) {
        case 0:
            return to_decimal(value, str);
        case 2:
            return bitwise_convert(value, str, 1);
        case 8:
            return bitwise_convert(value, str, 3);
        case 10:
            return to_decimal(value, str);
        case 16:
            return bitwise_convert(value, str, 4);
        case 36:
            return naive_convert(value, str, 36);
            
        default:
            // Unsupported base
            return 0;
    }
}

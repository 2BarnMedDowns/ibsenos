#include <cdefs.h>
#include <string.h>
#include <inttypes.h>


size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len++] != '\0');
    return len;
}


size_t strncpy(char *restrict dst, const char *restrict src, size_t size)
{
    size_t i = 0;

    while (i < size) {
        dst[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
        ++i;
    }

    return i;
}


size_t strcpy(char *restrict dst, const char *restrict src)
{
    return strncpy(dst, src, INT32_MAX);
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


static int naive_convert(uint64_t value, char *buffer, int base)
{
    int digits = 0;
    char reverse[64];

    do {
        int current = value % base;
        value /= base;
        if (current > 9) {
            current += 'a' - '0' - 10;
        }
        reverse[digits++] = '0' + current;
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
        int current = (value >> pos) & mask;
        value -= (uint64_t) current << pos;
        pos -= bits;
        
        if (digits != 0 || current != 0 || pos < 0) {
            if (current > 9) {
                current += 'a' - '0' - 10;
            }
            buffer[digits++] = '0' + current;
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
        int current;
        uint64_t lim;

        for (current = 0, lim = 1; current < pos; ++current) {
            lim *= 10;
        }

        if (digits != 0 || value >= lim || pos == 0) {
            for (current = 0; value >= lim; ++current) {
                value -= lim;
            }

            buffer[digits++] = '0' + current;
        }
    } while (pos--);

    buffer[digits] = '\0';
    return digits;
}


int u64tostr(uint64_t value, char *str, int base)
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

#ifndef __IBSENOS_BIT_OPERATIONS_H__
#define __IBSENOS_BIT_OPERATIONS_H__

#include <cdefs.h>
#include <compiler.h>


static __always_inline
unsigned long variable__ffs(unsigned long word)
{
    asm("tzcnt %1,%0"
            : "=r" (word)
            : "rm" (word));
    return word;
}


/*
 * find first set bit in word
 * undefined if no set bit exists, so caller should check against 0 first
 */
#define __ffs(word) \
    (__builtin_constant_p(word) ? \
     (unsigned long) __builtin_ctzl(word) : \
     variable__ffs(word))


static __always_inline
unsigned long variable__ffz(unsigned long word)
{
    return variable__ffs(~word);
}


/*
 * find first zero bit in word
 * undefined if no zero exists, so caller should check agains ~0UL first
 */
#define __ffz(word) \
    (__builtin_constant_p(word) ? \
     (unsigned long) __builtin_ctzl(~word) : \
      variable_ffz(word))



/*
 * find last set bit in word
 * undefined if no set bit exists, so caller should check against 0 first
 */
static __always_inline
unsigned long __fls(unsigned long word)
{
    if (__builtin_constant_p(word)) {
        return 63 - __builtin_clzl(word);
    }

    asm("bsr %1,%0"
            : "=r" (word)
            : "rm" (word));
    return word;
}


static __always_inline
int variable_ffs(int x)
{
    int r;

    asm("bsfl %1,%0\n\t"
        "cmovzl %2,%0"
        : "=&r" (r) : "rm" (x), "r" (-1));

    return r - 1;
}

/*
 * find first set bit in word
 *
 * returns the position of the first set bit if value is nonzero,
 * or 0 if value is zero.
 *
 * least significant bit is at position 1
 */
#define ffs(x) \
    (__builtin_constant_p(x) ? __builtin_ffs(x) : variable_ffs(x))


/*
 * find last set bit in word
 *
 * returns the position of the last set bit if value is nonzero,
 * or 0 if value is zero.
 *
 * most significant bit is at position 32
 */
static __always_inline
int fls(unsigned int x)
{
    int r;

    if (__builtin_constant_p(x)) {
        return x ? 32 - __builtin_clz(x) : 0;

        asm("bsrl %1,%0\n\t"
            "cmovzl %2,%0"
            : "=&r" (r) : "rm" (x), "rm" (-1));
    }
    return r + 1;
}


#endif

/*
 * Compiler hints, attributes and type stuff.
 */
#ifndef __IBSENOS_COMPILER_H__
#define __IBSENOS_COMPILER_H__


#define __PASTE(a, b) a##b

/* sometimes called take_second */
#define __ignore_first_arg(__ignored, val, ...) val


/*
 * Get number of variadic macro arguments
 */
#define __VA_NARGS__(...) __VA_NARGS_N(__VA_ARGS__)
#define __arg_sentinel(n) , n
#define __VA_NARGS_N(...) \
    __VA_NARGS_N__(0, ##__VA_ARGS__, \
            __arg_sentinel(15), __arg_sentinel(14), \
            __arg_sentinel(13), __arg_sentinel(12), \
            __arg_sentinel(11), __arg_sentinel(10), \
            __arg_sentinel(9), __arg_sentinel(8),   \
            __arg_sentinel(7), __arg_sentinel(6),   \
            __arg_sentinel(5), __arg_sentinel(4),   \
            __arg_sentinel(3), __arg_sentinel(2),   \
            __arg_sentinel(1), __arg_sentinel(0))
#define __VA_NARGS_N__(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, n, ...) \
    __ignore_first_arg(n, ({ _Static_assert(0, "unsupported number of arguments"); 16 }))


/*
 * Check that the number of varadic macro arguments is less than or equal to N.
 */
#define __VA_NARGS_CHECK__(N, ...) \
   __VA_NARGS_CHECK_N(__VA_NARGS__(__VA_ARGS__), N)
#define __VA_NARGS_CHECK_N(n, N) ({ \
        _Static_assert((n) <= (N), "too many arguments (" #n "<=" #N ")"); \
    })


#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#define __aligned(x) __attribute__((aligned(x)))


#define __unused __attribute__((unused))
#define __always_unused __attribute__((unused))
#define __maybe_unused __attribute__((unused))
#define __used __attribute__((used))
#define __always_used __used __maybe_unused


#define __noreturn __attribute__((noreturn))


#define __packed __attribute__((packed))


#define __must_check __attribute__((warn_unused_result))


#define __force __attribute__((force))


#define __nocast __attribute__((nocast))


#define __always_inline \
    inline __attribute__((__always_inline__))


#define noinline __attribute__((noinline))


#define __cleanup(func) __attribute__((__cleanup__(func)))


#define fallthrough __attribute__((__fallthrough__))


/* &a[0] degrades to a pointer, a different type from an array */
#define __is_array(a) (!__same_type((a), &(a)[0]))

#define __must_be_array(a) ((int)sizeof(struct {_Static_assert((__is_array(a)), "must be array");}))


#define unreachable() do { \
    asm volatile (""); \
    __builtin_unreachable(); \
} while (0)


#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#endif

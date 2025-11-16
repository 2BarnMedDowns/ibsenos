/*
 * Compiler hints, attributes and type stuff.
 */
#ifndef __IBSENOS_COMPILER_H__
#define __IBSENOS_COMPILER_H__

// TODO: check if gcc?

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


#define noinline __attribute__((noinline))


#define fallthrough __attribute__((__fallthrough__))


/* &a[0] degrades to a pointer, a different type from an array */
#define __is_array(a) (!__same_type((a), &(a)[0]))

#define __must_be_array(a) ((int)sizeof(struct {_Static_assert((__is_array(a)), "must be array");}))


#endif

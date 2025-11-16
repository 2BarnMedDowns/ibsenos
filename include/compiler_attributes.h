#ifndef __IBSENOS_COMPILER_ATTRIBUTES_H__
#define __IBSENOS_COMPILER_ATTRIBUTES_H__


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
//#define do {} while (0) // fallthrough 


#endif

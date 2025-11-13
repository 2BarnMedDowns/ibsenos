#ifndef __COMPILER_ATTRIBUTES_H__
#define __COMPILER_ATTRIBUTES_H__


#define __aligned(x) __attribute__((aligned(x)))


#define __unused __attribute__((unused))


#define __noreturn __attribute__((noreturn))


#define __packed __attribute__((packed))


#define __must_check __attribute__((warn_unused_result))


#endif

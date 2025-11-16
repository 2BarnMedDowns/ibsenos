#ifndef __IBSENOS_STDEF_H__
#define __IBSENOS_STDEF_H__

#include <compiler_types.h>

#undef NULL
#define NULL ((void*) 0)

typedef unsigned long long int size_t;

typedef _Bool bool;

enum {
    false = 0,
    true = 1
};


#undef offsetof
#define offsetof(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

#define offsetofend(TYPE, MEMBER) \
    (offsetof(TYPE, MEMBER)	+ sizeof_field(TYPE, MEMBER))

#define typeof_member(T, m)	typeof(((T*)0)->m)

#define container_of(ptr, type, member) ({ \
        void *__mptr = (void *)(ptr); \
        _Static_assert(__same_type(*(ptr), ((type *)0)->member) || \
                __same_type(*(ptr), void), \
                "pointer type mismatch in container_of()");	\
                ((type *)(__mptr - offsetof(type, member))); })


#define containerof(ptr, type, member) \
    _Generic(ptr, \
            const typeof(*(ptr)) *: ((const type *)_containerof(ptr, type, member)), \
            default: ((type *)_containerof(ptr, type, member)) \
            )

#endif


/*
 * Convenient C definitions.
 */
#ifndef __IBSENOS_C_DEFINITIONS_H__
#define __IBSENOS_C_DEFINITIONS_H__


#undef NULL
#define NULL ((void*) 0)


#undef offsetof
#define offsetof(type, member) __builtin_offsetof(type, member)


typedef _Bool bool;

enum {
    false = 0,
    true = 1
};


/*
 * Convenience macro to get the type of structure member.
 */
#define typeof_field(type, member) typeof(((type*)0)->member)


/*
 * Convenience macro to get the size of a structure member.
 */
#define sizeof_field(type, member) sizeof(typeof_field(type, member))


/*
 * The end offset of a structure member within the structure.
 */
#define offsetofend(type, member) \
    (offsetof(type, member) + sizeof_field(type, member))


/* We need some compiler attributes here */
#include <compiler.h>


/*
 * Number of elements in a fixed-size array.
 */
#define array_size(a) (sizeof(a) / sizeof((a)[0]) + __must_be_array(a))


/*
 * Cast member of a structure out to the containing structure.
 * This is unsafe, use below instead.
 */
#define _containerof(ptr, type, member) ({ \
        void *__mptr = (void *)(ptr); \
        _Static_assert(__same_type(*(ptr), ((type *)0)->member) || \
                __same_type(*(ptr), void), \
                "pointer type mismatch in container_of()");	\
                ((type *)(__mptr - offsetof(type, member))); })


/*
 * Cast member of a structure out to the containing structure.
 * This preserves const-correctness.
 */
#define containerof(ptr, type, member) \
    _Generic(ptr, \
            const typeof(*(ptr)) *: ((const type *)_containerof(ptr, type, member)), \
            default: ((type *)_containerof(ptr, type, member)) \
            )

#endif

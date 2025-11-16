#ifndef __IBSENOS_COMPILER_TYPES_H__
#define __IBSENOS_COMPILER_TYPES_H__

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#endif

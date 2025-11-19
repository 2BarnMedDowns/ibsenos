#ifdef DEBUG
/* Debug build */

#undef NDEBUG

#ifndef _DEBUG
#define _DEBUG
#endif

#else
/* Release build */

#undef _DEBUG

#ifndef NDEBUG
#define NDEBUG
#endif

#endif


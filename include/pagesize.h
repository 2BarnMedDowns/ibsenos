#ifndef __IBSENOS_PAGE_SIZE_H__
#define __IBSENOS_PAGE_SIZE_H__

#include <align.h>

#define PAGE_SHIFT          12
#define PAGE_SIZE           (1UL << PAGE_SHIFT)
#define PAGE_MASK           (~(PAGE_SIZE-1))
#define PAGES_MAX           (18446744073709551615ULL >> PAGE_SHIFT)

#define PAGE_ALIGN(size)    (ALIGN(size), PAGE_SIZE)

#define NUM_PAGES(size)     (PAGE_ALIGN(size) >> PAGE_SHIFT)

#endif


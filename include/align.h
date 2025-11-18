#ifndef __IBSENOS_ALIGN_H__
#define __IBSENOS_ALIGN_H__

#define ALIGN_MASK(x, mask)     (((x) + (mask)) & ~(mask))
#define ALIGN(x, align)         ALIGN_MASK(x, ((typeof(x))(align)) - 1)
#define ALIGN_DOWN(x, align)    ALIGN(((x) - ((align) - 1)), (align))
#define ALIGNED(x, align)       (((x) & ((typeof(x))(align) - 1)) == 0)

/*
 * Number of times the specified value x goes into an alignment.
 *
 * For example, to find out how many pages are needed for a given size,
 * ALIGN_DIV(size, PAGE_SIZE)
 */
#define ALIGN_DIV(x, align)     (((x) + (align) - 1) / (align))

#endif

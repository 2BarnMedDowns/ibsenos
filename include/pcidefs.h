/*
 * PCI standard defines.
 */
#ifndef __IBSENOS_PCI_DEFINES_H__
#define __IBSENOS_PCI_DEFINES_H__

#define PCI_CFG_SPACE_SIZE      256
#define PCI_CFG_SPACE_EXP_SIZE  4096

#define PCI_HEADER_SIZE 64
#define PCI_NUM_BARS    6       // number of standard bars

/* Configuration space definitions */
#define PCI_VENDOR_ID   0x00    // 16 bits
#define PCI_DEVICE_ID   0x02    // 16 bits

#define PCI_CLASS_REVISION  0x08    // 24 high bits are class, the lower 8 are revision
#define PCI_REVISION_ID     0x08
#define PCI_CLASS_PROG      0x09
#define PCI_CLASS_DEVICE    0x0a

/* Extended capabilities */
#define PCI_EXT_CAP_ID(header)      ((header) & 0x0000ffff)
#define PCI_EXT_CAP_VER(header)     (((header) >> 16) & 0xf)
#define PCI_EXT_CAP_NEXT(header)    (((header) >> 20) & 0xffc)

#define PCI_EXT_CAP_ID_DSN  // Device Serial Number

#endif

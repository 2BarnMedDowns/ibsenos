/*
 * PCI standard defines.
 * See https://wiki.osdev.org/PCI
 */
#ifndef __IBSENOS_PCI_DEFINES_H__
#define __IBSENOS_PCI_DEFINES_H__

#define PCI_CFG_SPACE_SIZE      256
#define PCI_CFG_SPACE_EXP_SIZE  4096

#define PCI_HEADER_SIZE 64
#define PCI_NUM_BARS    6       // number of standard bars

/* Configuration space definitions */
#define PCI_VENDOR_ID       0x00    // 16 bits
#define PCI_DEVICE_ID       0x02    // 16 bits

#define PCI_COMMAND     0x04    // 16 bits
#define PCI_COMMAND_IO              0x1     // enable response in I/O space
#define PCI_COMMAND_MEMORY          0x2     // enable response in memory space
#define PCI_COMMAND_MASTER          0x4     // enable bus mastering
#define PCI_COMMAND_SPECIAL         0x8     // enable response to special cycles
#define PCI_COMMAND_INVALIDATE      0x10    // memory write and invalidate
#define PCI_COMMAND_VGA_PALETTE     0x20    // enable palette snooping
#define PCI_COMMAND_PARITY          0x40    // enable parity checking
#define PCI_COMMAND_WAIT            0x80    // enable address/data steppin
#define PCI_COMMAND_SERR            0x100   // enable system error (SERR)
#define PCI_COMMAND_FAST_BACK       0x200   // enable back-to-back writes
#define PCI_COMMAND_INTX_DISABLE    0x400   // INTx emulation disable

#define PCI_STATUS      0x06    // 16 bits
#define PCI_STATUS_IMM_READY    0x01    // immediate readiness
#define PCI_STATUS_INTERRUPT    0x08    // interrupt status
#define PCI_STATUS_CAP_LIST     0x10    // support capability list
#define PCI_STATUS_66MHZ        0x20    // support 66 MHz
#define PCI_STATUS_RESERVED     0x40
#define PCI_STATUS_FAST_BACK    0x80    // accept fast-back to back
#define PCI_STATUS_PARITY       0x100   // detected parity error
#define PCI_STATUS_DEVSEL_MASK  0x600   // DEVSEL timing
#define PCI_STATUS_DEVSEL_FAST          0x000 
#define PCI_STATUS_DEVSEL_MEDIUM        0x200 
#define PCI_STATUS_DEVSEL_SLOW          0x400 
#define PCI_STATUS_SIG_TARGET_ABORT     0x800   // set on target abort
#define PCI_STATUS_REC_TARGET_ABORT     0x1000  // target abort is seen
#define PCI_STATUS_REC_MASTER_ABORT     0x2000  // set on master abort
#define PCI_STATUS_SIG_SYSTEM_ERROR     0x4000  // set when SERR 
#define PCI_STATUS_DETECTED_PARITY      0x8000  // set on parity error

#define PCI_CLASS_REVISION      0x08    // 32 bits: class (8 bits) | subclass (8 bits) | prog if | revision id
#define PCI_REVISION_ID         0x08
#define PCI_CLASS_PROG          0x09
#define PCI_CLASS_DEVICE        0x0a

#define PCI_CACHE_LINE_SIZE     0x0c    // 8 bits
#define PCI_LATENCY_TIMER       0x0d    // 8 bits

#define PCI_HEADER_TYPE     0x0e    // 8 bits
#define PCI_HEADER_TYPE_MASK    0x7f
#define PCI_HEADER_TYPE_GENERAL 0   // general device
#define PCI_HEADER_TYPE_BRIDGE  1   // PCI-to-PCI bridge
#define PCI_HEADER_TYPE_CARDBUS 2   // PCI-to-CardBus bridge
#define PCI_HEADER_TYPE_MFD     0x80    // Multi function device

// Built-in self test (BIST)
#define PCI_BIST                0x0f    // 8 bits
#define PCI_BIST_CODE_MASK      0x0f    // return result
#define PCI_BIST_START          0x40
#define PCI_BIST_CAPABLE        0x80    // 1 if BIST capable

// All base addresses (BARs) are 32 bits
#define PCI_BASE_ADDRESS_0      0x10
#define PCI_BASE_ADDRESS_1      0x14    // header types 0 and 1 only
#define PCI_BASE_ADDRESS_2      0x18    // header type 0 only
#define PCI_BASE_ADDRESS_3      0x1c
#define PCI_BASE_ADDRESS_4      0x20
#define PCI_BASE_ADDRESS_5      0x24
#define PCI_BASE_ADDRESS_SPACE          0x01    // 0 = memory, 1 = I/O space
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK  0x6
#define PCI_BASE_ADDRESS_MEM_TYPE_32    0x00    // 32-bit address
#define PCI_BASE_ADDRESS_MEM_TYPE_1M    0x02    // below 1M (obsolete from PCIe 3.0)
#define PCI_BASE_ADDRESS_MEM_TYPE_64    0x04    // 64-bit address
#define PCI_BASE_ADDRESS_MEM_PREFETCH   0x08    // prefetchable memory
#define PCI_BASE_ADDRESS_MEM_MASK       (~0x0fUL)
#define PCI_BASE_ADDRESS_IO_MASK        (~0x03UL)

/* Header type 0 (general device) */
#define PCI_CARDBUS_CIS         0x28    // 32 bits, pointer to card information structure
#define PCI_SUBSYSTEM_VENDOR_ID 0x2c
#define PCI_SUSYSTEM_ID         0x2e
#define PCI_ROM_ADDRESS         0x30    // bits 31-11 are address, 10-1 are reserved
#define PCI_ROM_ADDRESS_ENABLE  0x01
#define PCI_ROM_ADDRESS_MASK    (~0x7ffU)

#define PCI_CAP_LIST_PTR        0x34    // offset of the first capability entry (bottom two bits are reserved)

#define PCI_INTERRUPT_LINE      0x3c    // 8 bits
#define PCI_INTERRUPT_PIN       0x3d    // 8 bits
#define PCI_MIN_GNT             0x3e    // 8 bits, PCI min grant in 1/4 ms units
#define PCI_MAX_LAT             0x3f    // 8 bits, max latency in 1/4 ms units

/* Header type 1 (PCI-to-PCI bridge) */
#define PCI_PRIMARY_BUS         0x18    // primary bus number
#define PCI_SECONDARY_BUS       0x19    // secondary bus number
#define PCI_SUBORDINATE_BUS     0x1a    // highest bus number behind the bridge
#define PCI_SEC_LATENCY_TIMER   0x1b    // latency timer for secondary interface
#define PCI_IO_BASE             0x1c    /* I/O range behind the bridge */
#define PCI_IO_LIMIT            0x1d
#define PCI_IO_RANGE_TYPE_MASK  0x0fUL  // I/O bridging type
#define PCI_IO_RANGE_TYPE_16    0x00
#define PCI_IO_RANGE_TYPE_32    0x01
#define PCI_IO_RANGE_MASK       (~0x0fUL)
#define PCI_IO_1K_RANGE_MASK    (~0x03UL)
#define PCI_SEC_STATUS          0x1e    // secondary status register, only bit 14 is used
#define PCI_MEMORY_BASE         0x20    // memory range behind the bridge
#define PCI_MEMORY_LIMIT        0x22
#define PCI_MEMORY_RANGE_TYPE_MASK  0x0fUL
#define PCI_MEMORY_RANGE_MASK       (~0x0fUL)
#define PCI_PREF_MEMORY_BASE    0x24    // prefetchable memory range behind bridge
#define PCI_PREF_MEMORY_LIMIT   0x26
#define PCI_PREF_RANGE_TYPE_MASK    0x0fUL
#define PCI_PREF_RANGE_TYPE_32      0x00
#define PCI_PREF_RANGE_TYPE_64      0x01
#define PCI_PREF_RANGE_MASK         (~0x0fUL)
#define PCI_PREF_BASE_UPPER32   0x28    // upper half of prefetchable memory range
#define PCI_PREF_LIMIT_UPPER32  0x2c
#define PCI_IO_BASE_UPPER16     0x30    // upper half of I/O addresses
#define PCI_IO_LIMIT_UPPER16    0x32
#define PCI_ROM_ADDRESS1        0x38    // same as PCI_ROM_ADDRESS, but for header type 1
#define PCI_BRIDGE_CONTROL      0x3e
#define PCI_BRIDGE_CTL_PARITY       0x01    // enable parity detection on secondary interface
#define PCI_BRIDGE_CTL_SERR         0x02    // enable SERR forwarding
#define PCI_BRIDGE_CTL_ISA          0x04    // enable ISA mode
#define PCI_BRIDGE_CTL_VGA          0x08    // forward VGA addresses
#define PCI_BRIDGE_CTL_MASTER_ABORT 0x20    // report master aborts
#define PCI_BRIDGE_CTL_BUS_RESET    0x40    // secondary bus reset
#define PCI_BRIDGE_CTL_FAST_BACK    0x80    // fast back to back enabled on secondary interface


/* Capability lists */
#define PCI_CAP_LIST_ID         0   // Capability ID
#define PCI_CAP_LIST_NEXT       1   // Next capability
#define PCI_CAP_SIZE            4

#define PCI_CAP_ID_PM       0x01    // Power management
#define PCI_CAP_ID_AGP      0x02    // Accelerated graphics port
#define PCI_CAP_ID_VPD      0x03    // Vital product data
#define PCI_CAP_ID_SLOT     0x04    // Slot identification
#define PCI_CAP_ID_MSI      0x05    // Message signalled interrupts
#define PCI_CAP_ID_CHS      0x06    // CompactPCI hot-swap
#define PCI_CAP_ID_PCI      0x07    // PCI-X
#define PCI_CAP_ID_HT       0x08    // Hyper transport
#define PCI_CAP_ID_VNDR     0x09    // Vendor specific
#define PCI_CAP_ID_DBG      0x0a    // Debug port
#define PCI_CAP_ID_CCRC     0x0b    // CompactPCI central resource control
#define PCI_CAP_ID_SHPC     0x0c    // PCI standard hot-plug controller
#define PCI_CAP_ID_SSVID    0x0d    // Bridge subsystem vendor/device ID
#define PCI_CAP_ID_AGP3     0x0e    // AGP target PCI-to-PCI bridge
#define PCI_CAP_ID_SECDEV   0x0f    // Secure device
#define PCI_CAP_ID_EXP      0x10    // PCI express
#define PCI_CAP_ID_MSIX     0x11    // Extended MSI (MSI-X)
#define PCI_CAP_ID_SATA     0x12    // SATA data/index configuration
#define PCI_CAP_ID_AF       0x13    // PCI advanced features
#define PCI_CAP_ID_EA       0x14    // PCI enhanced allocation


/* Extended capabilities */
#define PCI_EXT_CAP_ID(header)      ((header) & 0x0000ffff)
#define PCI_EXT_CAP_VER(header)     (((header) >> 16) & 0xf)
#define PCI_EXT_CAP_NEXT(header)    (((header) >> 20) & 0xffc)

#define PCI_EXT_CAP_ID_DSN  // Device Serial Number

#endif

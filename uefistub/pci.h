#ifndef __IBSENOS_UEFISTUB_PCI_H__
#define __IBSENOS_UEFISTUB_PCI_H__

#include <efi.h>
#include <pcidefs.h>


enum efi_pci_io_protocol_width 
{
    EFI_PCI_IO_WIDTH_UINT8,
    EFI_PCI_IO_WIDTH_UINT16,
    EFI_PCI_IO_WIDTH_UINT32,
    EFI_PCI_IO_WIDTH_UINT64,
    EFI_PCI_IO_WIDTH_FIFO_UINT8,
    EFI_PCI_IO_WIDTH_FIFO_UINT16,
    EFI_PCI_IO_WIDTH_FIFO_UINT32,
    EFI_PCI_IO_WIDTH_FIFO_UINT64,
    EFI_PCI_IO_WIDTH_FILL_UINT8,
    EFI_PCI_IO_WIDTH_FILL_UINT16,
    EFI_PCI_IO_WIDTH_FILL_UINT32,
    EFI_PCI_IO_WIDTH_FILL_UINT64,
    EFI_PCI_TO_WIDTH_MAX
};


/*
 * PCI I/O protocol attribute operation
 */
enum efi_pci_io_protocol_attrop
{
    EFI_PCI_IO_ATTROP_GET,
    EFI_PCI_IO_ATTROP_SET,
    EFI_PCI_IO_ATTROP_ENABLE,
    EFI_PCI_IO_ATTROP_DISABLE,
    EFI_PCI_IO_ATTROP_SUPPORTED,
    EFI_PCI_IO_ATTROP_MAX
};


/* EFI PCI I/O protocol attribute bits */
#define EFI_PCI_IO_ATTR_ISA_MOTHERBOARD_IO      0x0001
#define EFI_PCI_IO_ATTR_ISA_IO                  0x0002
#define EFI_PCI_IO_ATTR_VGA_PALETTE_IO          0x0004
#define EFI_PCI_IO_ATTR_VGA_MEMORY              0x0008
#define EFI_PCI_IO_ATTR_VGA_IO                  0x0010
#define EFI_PCI_IO_ATTR_IDE_PRIMARY_IO          0x0020
#define EFI_PCI_IO_ATTR_IDE_SECONDARY_IO        0x0040
#define EFI_PCI_IO_ATTR_MEMORY_WRITE_COMBINE    0x0080
#define EFI_PCI_IO_ATTR_IO                      0x0100
#define EFI_PCI_IO_ATTR_MEMORY                  0x0200
#define EFI_PCI_IO_ATTR_BUS_MASTER              0x0400
#define EFI_PCI_IO_ATTR_MEMORY_CACHED           0x0800
#define EFI_PCI_IO_ATTR_MEMORY_DISABLE          0x1000
#define EFI_PCI_IO_ATTR_EMBEDDED_DEVICE         0x2000
#define EFI_PCI_IO_ATTR_EMBEDDED_ROM            0x4000
#define EFI_PCI_IO_ATTR_DUAL_ADDRESS_CYCLE      0x8000
#define EFI_PCI_IO_ATTR_ISA_IO_16               0x10000
#define EFI_PCI_IO_ATTR_VGA_PALETTE_IO_16       0x20000
#define EFI_PCI_IO_ATTR_VGA_IO_16               0x40000


/*
 * PCI I/O protocol operation
 */
enum efi_pci_io_protocol_operation
{
    EFI_PCI_IO_OPERATION_BUS_MASTER_READ,           // DMA read from system RAM
    EFI_PCI_IO_OPERATION_BUS_MASTER_WRITE,          // DMA write to system RAM
    EFI_PCI_IO_OPERATION_BUS_MASTER_COMMON_BUFFER,  // Read/write from coherent memory buffer
    EFI_PCI_IO_OPERATION_MAX
};


/* Forward declaration of EFI_PCI_IO_PROTOCOL */
struct efi_pci_io_protocol;


/*
 * EFI PCI I/O protocol
 * See section 14 in the EFI specification.
 */
struct efi_pci_io_protocol
{
    efi_status_t (__efiapi *poll_mem)(const struct efi_pci_io_protocol*,
                                      enum efi_pci_io_protocol_width width,
                                      uint8_t bar,
                                      uint64_t offset,
                                      uint64_t mask,
                                      uint64_t value,   // exit condition
                                      uint64_t delay,   // delay in number of 100ns units
                                      uint64_t *result);

    efi_status_t (__efiapi *poll_io)(const struct efi_pci_io_protocol*,
                                     enum efi_pci_io_protocol_width width,
                                     uint8_t bar,
                                     uint64_t offset,
                                     uint64_t mask,
                                     uint64_t value,   // exit condition
                                     uint64_t delay,   // delay in number of 100ns units
                                     uint64_t *result);

    efi_status_t (__efiapi *mem_read)(const struct efi_pci_io_protocol*,
                                      enum efi_pci_io_protocol_width width,
                                      uint8_t bar,
                                      uint64_t offset,
                                      uint64_t count,
                                      void *buffer);

    efi_status_t (__efiapi *mem_write)(const struct efi_pci_io_protocol*,
                                       enum efi_pci_io_protocol_width width,
                                       uint8_t bar,
                                       uint64_t offset,
                                       uint64_t count,      // number of memory operations to perform (bytes moved is width * count)
                                       const void *buffer);

    efi_status_t (__efiapi *io_read)(const struct efi_pci_io_protocol*,
                                     enum efi_pci_io_protocol_width width,
                                     uint8_t bar,
                                     uint64_t offset,
                                     uint64_t count,
                                     void *buffer);

    efi_status_t (__efiapi *io_write)(const struct efi_pci_io_protocol*,
                                      enum efi_pci_io_protocol_width width,
                                      uint8_t bar,
                                      uint64_t offset,
                                      uint64_t count, 
                                      const void *buffer);

    // Read & write from config space
    efi_status_t (__efiapi *pci_read)(const struct efi_pci_io_protocol*,
                                      enum efi_pci_io_protocol_width width,
                                      uint64_t offset,
                                      uint64_t count,
                                      void *buffer);

    efi_status_t (__efiapi *pci_write)(const struct efi_pci_io_protocol*,
                                       enum efi_pci_io_protocol_width width,
                                       uint64_t offset,
                                       uint64_t count,
                                       const void *buffer);

    efi_status_t (__efiapi *copy_mem)(const struct efi_pci_io_protocol*,
                                      enum efi_pci_io_protocol_width width,
                                      uint8_t destination_bar,
                                      uint64_t destionation_offset,
                                      uint8_t source_bar,
                                      uint64_t source_offset,
                                      uint64_t count);

    // Map & unmap DMA buffers
    efi_status_t (__efiapi *map)(const struct efi_pci_io_protocol*,
                                 enum efi_pci_io_protocol_operation operation,
                                 const void *host_ptr,
                                 uint64_t num_bytes,
                                 uint64_t *device_addr,
                                 void **mapping);

    efi_status_t (__efiapi *unmap)(const struct efi_pci_io_protocol*,
                                   void *mapping);

    efi_status_t (__efiapi *allocate_buffer)(const struct efi_pci_io_protocol*,
                                             enum efi_allocate_type type,
                                             enum efi_memory_type memory_type,
                                             uint64_t num_pages,
                                             void **host_ptr,
                                             uint64_t attributes);

    efi_status_t (__efiapi *free_buffer)(const struct efi_pci_io_protocol*,
                                         uint64_t num_pages,
                                         void *host_ptr);
    
    efi_status_t (__efiapi *flush)(const struct efi_pci_io_protocol*);

    // Get PCI controller's bus number, device number and function number (bus-device-function, BDF)
    efi_status_t (__efiapi *get_location)(const struct efi_pci_io_protocol*,
                                          uint64_t *segment_no,
                                          uint64_t *bus_no,
                                          uint64_t *device_no,
                                          uint64_t *function_no);

    // Perform attribute operation, like getting the set of supported
    // retrieving current attributes, setting current attributes,
    // enabling and disabling attributes
    efi_status_t (__efiapi *attributes)(const struct efi_pci_io_protocol*,
                                        enum efi_pci_io_protocol_attrop operation,
                                        uint64_t attributes,
                                        uint64_t *result);  // optional
    
    efi_status_t (__efiapi *get_bar_attributes)(const struct efi_pci_io_protocol*,
                                                uint8_t bar,
                                                uint64_t *supports,
                                                void **resources);  // must be freed with BootServices.FreePool
    
    efi_status_t (__efiapi *set_bar_attributes)(const struct efi_pci_io_protocol*,
                                                uint64_t attributes,
                                                uint8_t bar,
                                                uint64_t *offset,
                                                uint64_t *length);
    const uint64_t rom_size;
    const void *rom_image;
};


#endif

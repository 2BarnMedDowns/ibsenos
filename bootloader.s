.section .text.bootentry
.org 0
.code16

# Where we move ourselves in memory
.equ SEGM, 0x0060
.equ ADDR, 0x0000

# Where do we want our stack?
.equ STACK_SEGM, 0x0900

# Block size/sector size (usually 512)
.equ SECTOR_SIZE, 512

# Align size (in bytes) to sectors (2^9=512)
.equ KERNEL_SECTORS, (KERNEL_SIZE + 511) >> 9

# BIOS loads bootloader into 0000:7c00
# Start by relocating ourselves to new position.
.globl _start
_start:
    # Source DS:SI
    movw    $0x0000, %ax 
    movw    %ax, %ds     # Starting segment
    movw    $0x7c00, %si # Starting address

    # Destination ES:DI
    movw    $SEGM, %ax # Segment where we end up
    movw    %ax, %es
    movw    $ADDR, %di # Offset where we end up

    # Number of bytes to move
    movw    $SECTOR_SIZE, %cx

    # Relocate!
    cld     # Direction flag forward
    rep     # Repeat CX number of times
    movsb   # Move byte from DS:SI to ES:DI

    # Jump to next instruction in new position
    ljmp    $SEGM, $ADDR + _init


# Some strings for user friendly messages
_str_welcome:   .asciz  "Please wait while preparing OS\n\r"
_str_loading:   .asciz  "Loading image..."
_str_done:      .asciz  "DONE\n\r"
_str_fail:      .asciz  "FAILED :(\n\r"


# Initialize stack and data segments.
# This is after we moved ourselves.
_init:
    # We just relocated, so we need to update our view of 
    # the world by setting segment pointers
    movw    $SEGM, %ax
    movw    %ax, %es
    movw    %ax, %ds

    # Set up the stack and base pointer
    movw    $STACK_SEGM, %ax # Stack segment
    movw    %ax, %ss
    movw    $0xfffe, %sp     # Stack pointer
    movw    %sp, %bp

    # "Hello, world!"
    call    clear_screen
    pushw   $_str_welcome
    call    print_message
    movw    %bp, %sp

    # Set up Fast A20 gate
    in      $0x92, %al
    or      $2, %al
    out	    %al, $0x92

    # Load OS image from disk
    # First, let's initialize some variables
    subw    $10, %sp    # Reserve space for variables
    .equ    SECTOR,         -2
    .equ    HEAD,           -4
    .equ    TRACK,          -6
    .equ    DESTINATION,    -8
    .equ    NUM_SECTORS,    -10
    movw    $2, SECTOR(%bp)
    movw    $0, HEAD(%bp)
    movw    $0, TRACK(%bp)

    movw    $KERNEL_ADDR >> 4, DESTINATION(%bp)
    movw    $KERNEL_SECTORS, NUM_SECTORS(%bp)

    pushw   $_str_loading
    call    print_message

    # Main read loop
    # Load OS image from disk
_load_loop:
    cmpw    $0, NUM_SECTORS(%bp)
    jz      _load_done
    call    print_dot
    cmpw    $18, SECTOR(%bp)
    jle     _read_sector
    movw    $1, SECTOR(%bp)
    cmpw    $0, HEAD(%bp)
    jnz     _next_track
    movw    $1, HEAD(%bp)
    jmp     _read_sector

_next_track:
    movw     $0, HEAD(%bp)
    incw    TRACK(%bp)

_read_sector:
    movb    TRACK(%bp), %ch     # track number
    movb    SECTOR(%bp), %cl    # sector number
    movb    HEAD(%bp), %dh      # head number
    movb    $0x00, %dl          # drive number
    movw    DESTINATION(%bp), %ax

    # Destination in ES:BX
    movw    %ax,%es
    movw    $0x0000, %bx

    movb    $2, %ah             # function number
    movb    $1, %al             # number of sectors to read
    int     $0x13

    # Check if reading was successful,
    # and update sector counter
    cmpb    $0, %ah
    jne     _load_error
    incw    SECTOR(%bp)
    addw    $SECTOR_SIZE >> 4, DESTINATION(%bp)
    decw    NUM_SECTORS(%bp)
    jmp     _load_loop

    # Something went wrong :(
_load_error:
    pushw   $_str_fail
    call    print_message

_forever:
    jmp     _forever

_load_done:
    # Phew, we made it!
    pushw   $_str_done
    call    print_message
    movw    %bp, %sp

    # Wait for about 2 seconds so the drive can stop spinning
    call    delay
    call    delay

    # Set up video mode 13
    push    %ax
    movb    $0x13, %al
    movb    $0x0, %ah
    int     $0x10
    pop     %ax

_setup_gdt:
    # Segment selector table indicator flag (bit 2).
    # Specifies the descriptor table to use:
    #    0 = GDT, 1 = LDT
    .equ    SELECT_TI, 1 << 2

    # Segment selector requested privilege levels (bits 0 and 1)
    .equ    SELECT_RPL, 0
    
    # Segment selector for kernel code (and bootloader)
    .equ    CODE_SELECT,    1 << 3 & ~SELECT_TI | SELECT_RPL
    
    # Segment selector for kernel data
    .equ    DATA_SELECT,    2 << 3 & ~SELECT_TI | SELECT_RPL

    # Disable interrupts
    cli

    # Load the Global Descriptor Table Register
    # Use base address of the GDT
    lgdt    gdtr

    # Set the PE flag (bit 0) in control register CR0
    # This enables protected mode.
    smsw    %ax
    orw     $1, %ax
    lmsw    %ax

    # Far jump, forcing CS to be loaded with the
    # proper segment selector.
    ljmp    $CODE_SELECT, $protected_mode + (SEGM << 4)


.code32
protected_mode:
    # We just far jumped, need to update our view
    # of the world again by setting segment registers.
    movw    $DATA_SELECT, %ax
    movw    %ax, %ds
    movw    %ax, %es

    # Setup a stack for the kernel
    movw    %ax, %ss
    movl    $KERNEL_STACK, %esp

    # Place OS size in ECX
    movl    $KERNEL_SECTORS, %ecx
    
    # Jump to the kernel
    ljmp    $CODE_SELECT, $KERNEL_ADDR
.code16


# Delay for around 1 second
# Does not clobber registers.
delay:
    pushw   %bp
    movw    %sp, %bp
    pushw   %ax
    pushw   %cx
    pushw   %dx

    # Delays for CX:DX microseconds (CX is high, DX is low)
    # Dec 1 million = 0xf4240
    movw    $0x000f, %cx 
    movw    $0x4240, %dx 

    movb    $0, %al
    movb    $0x86, %ah  # function number
    int     $0x15

_delay_leave:
    popw    %dx
    popw    %cx
    popw    %ax
    movw    %bp, %sp
    popw    %bp
    ret


# Clear everything on the screen
# Does not clobber registers
clear_screen:
    pushw   %ax
    movb    $0, %ah	# Function number
    movb    $3, %al # Video mode
    int	    $0x10
    popw    %ax
    ret


# Print loading dot
# Does not clobber registers
print_dot:
    pushw   %ax
    pushw   %bx
    movb    $'.', %al
    call    _print
    popw    %bx
    popw    %ax
    ret


# Routine to print character
# Clobbers AX and BX
_print:
    movb    $0x0e, %ah  # Function number
    movb    $0x00, %bh  # Active page number
    movb    $0x02, %bl  # Foreground color
    int     $0x10
    ret

# Print a message to screen
# Does not clobber registers
print_message:
    # Store registers
    pushw   %bp
    movw    %sp, %bp
    pushw   %ax
    pushw   %bx
    pushw   %si

    # Extract argument and place in SI
    movw    4(%bp), %si

_print_loop:
    lodsb  # Load char from SI to AL, and incr SI
    cmpb    $0, %al # End of string?
    jz      _print_leave
    call    _print
    jmp     _print_loop

_print_leave:
    popw    %si
    popw    %bx
    popw    %ax
    movw    %bp, %sp
    popw    %bp
    ret

# Global Descriptor Table Register
# 32-bit base address and 16-bit limit for the GDT.
#
# Limit should be number of bytes - 1.
gdtr:
    .word   end - gdt - 1       # last byte of GDT
    .long   gdt + (SEGM << 4)   # base address

# Global Descriptor Table
# 8-byte aligned, and entries are 8 bytes long.
.align  8
gdt:
    # First entry of GDT is not used by processor
    .long   0
    .long   0

    # Kernel code segment descriptor
    .word   0xffff  # limit
    .word   0x0000  # base 15:00
    .byte   0x00    # base 23:16
    .word   0xc09a  # flags
    .byte   0x00    # base 31:24

    # Kernel data segment descriptor
    .word   0xffff  # limit
    .word   0x0000  # base 15:00
    .byte   0x00    # base 23:16
    .word   0xc092  # flags
    .byte   0x00    # base 31:24
end:

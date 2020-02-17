.section .text.bootentry
.org 0
.code16

# Where we move ourselves in memory
.equ SEGM, 0x0060
.equ ADDR, 0x0000

# Where do we want our stack?
.equ STACK_SEGM, 0x0900

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
    movw    $512, %cx

    # Relocate!
    cld     # Direction flag forward
    rep     # Repeat CX number of times
    movsb   # Move byte from DS:SI to ES:DI

    # Jump to next instruction in new position
    ljmp    $SEGM, $ADDR + _init


# Some strings for user friendly messages
_str_welcome:   .asciz  "Please wait while preparing OS.\n\r"
_str_loading:   .asciz  "Loading image..."
_str_done:      .asciz  "OK\n\r"
_str_fail:      .asciz  "FAIL\n\r"


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

    # Set up A20 gate
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
    # Align size (in bytes) to sectors (2^9=512)
    movw    $(KERNEL_SIZE + 511) >> 9, NUM_SECTORS(%bp)

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

    # destination in ES:BX
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
    addw    $512 >> 4, DESTINATION(%bp)
    decw    NUM_SECTORS(%bp)
    jmp     _load_loop

    # Something went wrong :(
_load_error:
    pushw   $_str_fail
    call    print_message

_forever:
    jmp     _forever

    # Phew, we made it!
    # Time to tell the user we're done loading from disk.
_load_done:
    pushw   $_str_done
    call    print_message
    movw    %bp, %sp
    jmp     _forever


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

_magic: .asciz "IbsenOS is made by Axel and Jonas"

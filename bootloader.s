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
    rep     # Repeat cx number of times
    movsb   # Move byte from DS:SI to ES:DI

# Jump to next instruction in new position
    ljmp    $SEGM, $ADDR + _init


# Some strings for user friendly messages
_str_welcome:   .asciz  "IbsenOS version 0.1\n\r"
_str_loading:   .asciz  "Loading"
_str_done:      .asciz  "OK"
_str_fail:      .asciz  "FAIL"


# Initialize stack and data segments.
# This is after we moved ourselves.
_init:
# Set up the stack and base pointer
    movw    $STACK_SEGM, %ax # Stack segment
    movw    %ax, %ss
    movw    $0xfffe, %sp     # Stack pointer
    movw    %sp, %bp

# We just jumped, so we need to update our view of 
# the world by setting segment pointers
    movw    $SEGM, %ax
    movw    %ax, %es
    movw    %ax, %ds

    # Set up A20 gate
    in      $0x92, %al
    or      $2, %al
    out	    %al, $0x92

    call    clear_screen
    pushw   $_str_welcome
    call    print_message
    addw    $2, %sp

    pushw   $_str_loading
    call    print_message
    addw    $2, %sp

    call    print_dot
    call    print_dot
    call    print_dot

forever:
    #cli
    #hlt
    jmp     forever

# Clear everything on the screen
clear_screen:
    pushw   %ax
    movb    $0, %ah	# Function number
    movb    $3, %al # Video mode
    int	    $0x10
    popw    %ax
    ret


# Print loading dot
print_dot:
    pushw   %ax
    pushw   %bx

    movb    $'.', %al
    movb    $0x0e, %ah # Function number
    movb    $0x00, %bh # Active page number
    movb    $0x02, %bl # Foreground color
    int     $0x10

    popw    %bx
    popw    %ax
    ret


# Print a message to screen
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
    movb    $0x0e, %ah # Function number
    movb    $0x00, %bh # Active page number
    movb    $0x02, %bl # Foreground color
    int     $0x10
    jmp     _print_loop

_print_leave:
    popw    %si
    popw    %bx
    popw    %ax
    movw    %bp, %sp
    popw    %bp
    ret


# This is a hack to write the magic 55AA signature in the 
# last two bytes of the boot sector.
_magic:
    .space 510-(.-_start)
    .word	0xaa55

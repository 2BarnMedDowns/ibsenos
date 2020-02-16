.text
.code16
.globl _start

.equ SEGMENT, 0x7c0
.equ POINTER, 0x0000

# Start by moving ourselves to new position.
_start:
	# Source DS:SI
	movw	$SEGMENT, %ax
	movw	%ax, %ds
	movw	$POINTER, %si
	
	# Destination ES:DI
	movw	$0x80, %ax
	movw	%ax, %es
	movw	$0, %di

	# Number of bytes to move
	movw	512, %cx

	# Execute it
	cld
	rep		# Repeat cx number of times
	movsb		# Copy data from source to destination

	# Jump to next instruction in new position
	ljmp	$SEGMENT,$POINTER + _init


# Initialize stack and data segments.
_init:
	# Set up the stack
	movw	$0x9000, %ax
	movw	%ax, %ss
	movw 	$0xfffe, %sp

	# Set up the data segments
	movw	$0x80, %ax
	movw	%ax, %ds
	movw	%ax, %es

	# Set up A20 gate
	in	$0x92, %al
	or	$2, %al
	out	%al, $0x92

	call	clear_screen
	call	load_os

forever:
	jmp	forever


load_os:
	# Save registers
	pushw	%bp
	movw	%sp, %bp
	subw	$10, %sp
	pushw	%ax
	pushw	%bx
	pushw	%cx
	pushw	%dx
	pushw	%es

	pushw	$load_init_string
	call	print_string
	jmp	forever
	
	# Restore registers
	popw	%es
	popw	%dx
	popw	%cx
	popw	%bx
	popw	%ax
	movw	%bp, %sp
	popw	%bp
	retw


clear_screen:
	pushw	%ax
	movb	$0, %ah		# function number
	movb	$3, %al 	# video mode
	int	$0x10
	popw	%ax
	retw


print_string:
	pushw	%bp
	movw	%sp, %bp
	pushw	%ax
	pushw	%bx
	pushw	%si

	movw	4(%bp), %si

print_str_while:
	lodsb	# load character to write into al, and increment si
	cmpb	$0, %al
	jz	print_str_end_while
	movb	$0x0e, %ah	# function number
	movb	$0x00, %bh	# active page number
	movb	$0x02, %bl	# foreground color
	int	$0x10
	jmp	print_str_while

print_str_end_while:
	popw	%si
	popw	%bx
	popw	%ax
	movw	%bp, %sp
	popw	%bp
	retw	$2
	
red_screen_of_death:
    #mov ax, 0A000h ; The offset to video memory
    movb        $0x13, %al
    movb        $0x0, %ah
    int         $0x10
    mov         $0x0A000, %ax
    mov         %ax, %es
    mov         $14464, %ax
red_screen_of_death_forever:
    mov         %ax, %di
    mov         $0x4, %dl
    mov         %dl, %es:(%di)
    inc         %ax
    jmp         red_screen_of_death_forever


# Some user friendly text
load_init_string:
	.asciz	"Loading\n\r"

load_done_string:
	.asciz	"\n\rDone"


# This is a hack to write the magic 55AA signature in the 
# last two bytes of the boot sector.
_magic:
    .space 510-(.-_start)
_magic_end:
	.word	0xaa55

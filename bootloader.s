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

	# Say "hello, world!"
	#call	enterMode13h
	call clear_screen

forever:
	jmp	forever


clear_screen:
	pushw	%ax
	movb	$0, %ah		# function number
	movb	$3, %al 	# video mode
	int	$0x10
	popw	%ax
	retw


#enterMode13h:
#    #mov ax, 0A000h ; The offset to video memory
#    movb        $0x13, %al
#    movb        $0x0, %ah
#    int         $0x10
#    mov         $0x0A000, %ax
#    mov         %ax, %es
#    mov         $14464, %ax
#loop2:
#    mov         %ax, %di
#    mov         $0x4, %dl
#    mov         %dl, %es:(%di)
#    inc         %ax
#    jmp         loop2


# This is a hack to write the magic 55AA signature in the 
# last two bytes of the boot sector.
_magic:
	.align	2
	.fill 	512, 2, 0xaa55
_magic_end:
	.word	0xaa55

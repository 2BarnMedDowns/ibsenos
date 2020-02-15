.set 	ALIGN  ,	1<<0
.set	MEMINFO,	1<<1
.set 	FLAGS  ,	ALIGN | MEMINFO
.set	MAGIC  ,	0x1BADB002
.set	CHECKSUM,	-(MAGIC + FLAGS)

.section multiboot
.align 	4
.long 	MAGIC
.long	FLAGS
.long	CHECKSUM

#.fill	509, 1, 0
#.byte 	0x55
#.byte	0xAA

.section .bss
.align	16

.text
.globl _start

_start:


.size	_start, . - _start

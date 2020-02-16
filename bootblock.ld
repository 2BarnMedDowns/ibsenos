ENTRY(_start)

SECTIONS
{
    /* . = 0x7c00 */ /* Either here or move in bootloader */
	. = 0x0;
	.text : { 
        /* .text.bootentry mus only be included once in the main bootloader.s */
        *(.text.bootentry);
        *(.text) 
    }
}

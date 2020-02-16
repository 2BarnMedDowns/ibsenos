ENTRY(_loader)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x2000;
    .text :
    {
        *(.text);
    }
    /*. = 0x4000;
    .data :
    {
        *(.data);
    }
    .bss :
    {
        *(.bss);
    }*/
    .size : AT(0x1000)
    {
        LONG(SIZEOF(.text))
    }
}

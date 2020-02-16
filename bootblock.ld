ENTRY(_start)
OUTPUT_ARCH(i386)

SECTIONS
{
    . = 0x0;
    .text :
    { 
        *(.text.bootentry);
    }
    .signature : AT(510)
    {
        SHORT(0xaa55)
    }
}

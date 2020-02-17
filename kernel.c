static short* screen = (short*) 0xb8000; /* Test if data segment works */

extern void clear_screen(short* screen); /* Test if linking works */
extern short* print_uint(short* screen, unsigned int i);

void _start(unsigned int size)
{
    clear_screen(screen);
    print_uint(&screen[80], size);

    while (1);
}

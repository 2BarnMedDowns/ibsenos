//static short* screen = (short*) 0xb8000; /* Test if data segment works */
static short* screen = (short*) 0xA0000; /* Test if data segment works */

extern void clear_screen(short* screen); /* Test if linking works */
extern short* print_uint(short* screen, unsigned int i);
extern void clear_screen_to_color(short color, short* screen);

void _start(unsigned int size)
{
    /* clear_screen(screen); */
    /* print_uint(&screen[80], size); */
    clear_screen_to_color(0x6, screen);
    while (1);
}

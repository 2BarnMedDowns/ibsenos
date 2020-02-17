static short* screen = (short*) 0xb8000; /* Test if data segment works */

extern void clear_screen(short* screen); /* Test if linking works */

void _start(void)
{
    clear_screen(screen);

    while (1);
}

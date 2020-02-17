/*
 * Just a proof of concept to see if we really ended up here
 */
void clear_screen(short* screen)
{
    for (int y = 0; y < 25; ++y) {
        for (int x = 0; x < 80; ++x) {
            int pos = y * 80 + x;
            screen[pos] = 0x0700;
        }
    }

    const char* string = "Protected mode :)";
    for (int i = 0; string[i] != '\0'; ++i) {
        screen[i] = (0x07 << 8) | string[i];
    }
}

short* print_uint(short* screen, unsigned int n)
{
    if (n >= 10) {
        screen = print_uint(screen, n / 10);
    }
    *screen = (0x07 << 8) | ('0' + (n % 10));
    return screen + 1;
}

void clear_screen_to_color(short color, short* screen)
{
    short colors[] = {
        0x19, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18
    };
    short k = 0;
    int colorpointer = 0;
    for (int i = 0xA0000; i <= 0xBFFFF; i++)
    {
        if (colorpointer % 17 == 0)
        {
            colorpointer = 1;
        }
        screen[k++] = colors[colorpointer++];
    }
}


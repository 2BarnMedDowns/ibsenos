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

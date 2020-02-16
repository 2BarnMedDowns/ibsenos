extern void some_symbol(const char*);

void _start(void)
{
    char test[] = "Hello";
    some_symbol(test);
}

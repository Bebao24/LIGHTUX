#include "ctype.h"

bool islower(char c)
{
    return c >= 'a' && c <= 'z';
}

char toupper(char c)
{
    return islower(c) ? (c - 'a' + 'A') : c;
}

bool isspace(char c)
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' ||
        c == '\b');
}


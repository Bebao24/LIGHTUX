#include "string.h"

const char* strchr(const char* string, char c)
{
    if (string == NULL)
    {
        return NULL;
    }

    while (*string)
    {
        if (*string == c)
        {
            return string;
        }

        string++;
    }

    return NULL;
}

char* strcpy(char* dst, const char* src)
{
    char* originalDst = dst;

    if (dst == NULL)
    {
        return NULL;
    }

    if (src == NULL)
    {
        *dst = '\0';
        return NULL;
    }

    while (*src)
    {
        *dst = *src;
        src++;
        dst++;
    }

    *dst = '\0';
    return originalDst;
}

size_t strlen(const char* string)
{
    size_t len = 0;

    while (*string)
    {
        len++;
        string++;
    }

    return len;
}

int strcmp(const char* a, const char* b)
{
    if (a == NULL && b == NULL)
    {
        return 0;
    }

    if (a == NULL || b == NULL)
    {
        return -1;
    }

    while (*a && *b && *a == *b)
    {
        ++a;
        ++b;
    }
    return (*a) - (*b);
}


#include <string.h>
#include "../inc/print.h"
#include "../inc/alloc.h"

size_t strlen(const char *str)
{
	const char *begin = str;

	while (*str++);
	return str - begin - 1;
}

void *memcpy(void *dst, const void *src, size_t size)
{
	char *to = dst;
	const char *from = src;

	while (size--)
		*to++ = *from++;
	return dst;
}

void *memset(void *dst, int fill, size_t size)
{
	char *to = dst;

	while (size--)
		*to++ = fill;
	return dst;
}

char * strdup(const char * tocopy) {
    size_t toreturnsize = strlen(tocopy);
    char * toreturn = mem_alloc(toreturnsize);
    memcpy((void *) toreturn, (void *) tocopy, toreturnsize);

    return toreturn;
}

char * strsep(register char **stringp, register const char *delim) {
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;

    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}


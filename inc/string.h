#ifndef __STRING_H__
#define __STRING_H__

#include <stddef.h>

size_t strlen(const char *str);
void *memcpy(void *dst, const void *src, size_t size);
void *memset(void *dst, int fill, size_t size);
char * strdup(const char * tocopy);
char * strsep(register char ** source, register const char * delimeter);

#endif /*__STRING_H__*/

#ifndef TR_STRUTILS_H
#define TR_STRUTILS_H

#include <stddef.h>

#ifndef POSIX

size_t tr_strnlen(const char *str, size_t len);
char * tr_strnchr(const char *str, size_t len, int ch);
char * tr_strndup(const char *str, size_t len);

#else

#define tr_strnlen strnlen
#define tr_strnchr strnchr
#define tr_strndup strndup

#endif // #ifndef POSIX

#if !defined(POSIX) && __STDC_VERSION__ < 199901L

int tr_snprintf(char *str, size_t size, const char *format, ...);

#else

#define tr_snprintf snprintf

#endif // #if !defined(POSIX) && __STDC_VERSION__ < 199901L


#endif //#ifndef TR_STRUTILS_H
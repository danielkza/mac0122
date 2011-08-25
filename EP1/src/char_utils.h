#ifndef TR_CHAR_UTILS_H
#define TR_CHAR_UTILS_H


#ifdef _GNU_C

#define tr_strnlen strnlen
#define tr_strnchr strnchr
#define tr_strndup strndup
#define tr_strnchrdup strnchrdup

#else

size_t tr_strnlen(const char *str, size_t len);
char * tr_strnchr(const char *str, int ch, size_t len);
char * tr_strndup(const char *str, size_t len);
char * tr_strnchrdup(const char *str, int ch, size_t len);

#endif // #ifdef _GNU_C


#endif // #ifndef TR_CHAR_UTILS_H
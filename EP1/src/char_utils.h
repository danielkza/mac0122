#ifndef TR_CHAR_UTILS_H
#define TR_CHAR_UTILS_H

size_t tr_strnlen(const char *str, size_t len);
char * tr_strnchr(const char *str, int ch, size_t len);
char * tr_strndup(const char *str, size_t len);
char * tr_strnchrdup(const char *str, int ch, size_t len);

#endif // #ifndef TR_CHAR_UTILS_H
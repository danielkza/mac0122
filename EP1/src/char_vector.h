#ifndef TR_CHAR_VECTOR_H
#define TR_CHAR_VECTOR_H

typedef struct {
	char* vector;
	size_t len;
	size_t size;
} char_vector;

char_vector* char_vector_new(size_t initial_size = 0);
int char_vector_append(char_vector* dest, const char* src, size_t len);
int char_vector_append_char(char_vector* dest, const char ch);
int char_vector_expand(char_vector* vec, size_t desired_size);

#endif //#ifndef TR_CHAR_VECTOR_H
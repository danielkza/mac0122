#ifndef TR_CHAR_VECTOR_H
#define TR_CHAR_VECTOR_H

typedef struct {
	char* vector;
	size_t len;
	size_t size;
} char_vector_t;

char_vector_t* char_vector_new(size_t initial_size);
int char_vector_append(char_vector_t* dest, const char* src, size_t len);
int char_vector_append_char(char_vector_t* dest, const char ch);
int char_vector_expand(char_vector_t* vec, size_t desired_size);

#endif //#ifndef TR_CHAR_VECTOR_H
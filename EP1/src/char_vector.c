#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "char_vector.h"

char_vector_t* char_vector_new(size_t initial_size) {
	char_vector_t *vec = (char_vector_t*)malloc(sizeof(char_vector_t));
	if(!vec)
		return NULL;

	vec->len = 0;
	vec->size = 0;
	vec->vector = NULL;

	if(initial_size) {
		if(!char_vector_expand(vec, initial_size)) {
			free(vec);
			return NULL;
		}
	}

	return vec;
}

int char_vector_append_char(char_vector_t* dest, const char ch) {
	return char_vector_append(dest, &ch, 1);
}

int char_vector_append(char_vector_t* dest, const char* src, size_t len) {
	size_t i;

	if(!dest || !src || !len)
		return 0;
	else if(!char_vector_expand(dest, dest->len + len))
		return 0;

	for(i = 0; i < len; i++)
		dest->vector[dest->len++] = src[i];

	return len;
}

int char_vector_expand(char_vector_t* vec, size_t desired_size) {
	char *new_vec;
	size_t new_size;

	if(!vec || !desired_size)
		return 0;

	if(vec->size >= desired_size)
		return 1;

	// Resize to the next power of 2 from the desired size.
	if(!IS_POWER_OF_2(desired_size)) {
		new_size = 1;
		while (new_size < desired_size) { new_size *= 2; };
	} else {
		new_size = desired_size;
	}

	new_vec = (char*)realloc(vec->vector, new_size);
	if(!new_vec) {
		// Try to allocate exactly the desired size, maybe we don't have enough
		// memory for the next power of 2.
		new_size = desired_size;

		new_vec = (char*)realloc(vec->vector, new_size);
		if(!new_vec)
			return 0;
	}

	vec->vector = new_vec;
	vec->size = new_size;
	return 1;
}
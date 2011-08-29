#include <stdlib.h>
#include <string.h>

#include "xmalloc.h"
#include "utils.h"
#include "char_vector.h"

char_vector_t* char_vector_new(size_t initial_size) {
	char_vector_t *vec = (char_vector_t*)xmalloc(sizeof(*vec));
	if(vec == NULL)
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

size_t char_vector_append_char(char_vector_t* dest, const char ch) {
	return char_vector_append(dest, &ch, 1);
}

size_t char_vector_append(char_vector_t* dest, const char* src, size_t len) {
	if(dest == NULL || src == NULL || !len)
		return 0;

	if(!char_vector_expand(dest, dest->len + len))
		return 0;

	memcpy(dest->vector + dest->len, src, len);
	dest->len += len;

	return len;
}

int char_vector_expand(char_vector_t* vec, size_t desired_size) {
	char *new_vec;
	size_t new_size;

	if(vec == NULL || desired_size == 0)
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
	if(new_vec == NULL) {
		// Try to allocate exactly the desired size, maybe we don't have enough
		// memory for the next power of 2.
		new_size = desired_size;

		new_vec = (char*)realloc(vec->vector, new_size);
		if(new_vec == NULL)
			return 0;
	}

	vec->vector = new_vec;
	vec->size = new_size;
	return 1;
}

void char_vector_free(char_vector_t* vec) {
	if(vec != NULL) {
		if(vec->vector != NULL) {
			free(vec->vector);
		}

		free(vec);
	}
}
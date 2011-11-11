#include <stddef.h>

typedef size_t (*hash_function(const unsigned char *data,
			                   size_t len,
                               void *params));

typedef struct {
	const char *name;
	hash_function hash_func;
	void *params;
} hash_algorithm;

typedef struct {
	size_t key;
	void *value;
} hash_entry;

typedef struct {
	size_t size;
	size_t used_slots;
	hash_entry *data;
	const hash_algorithm *hash_algo;
} hash_map;

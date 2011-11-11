#include <stdlib.h>

#include "hash_map.h"

#define hash_map_create(out, initial_size, hash_algo, hash_entry_type) \
{ \
	if(!hash_map_check_algorithm(algo) || initial_size == 0)
		out = NULL;

	map = malloc(sizeof(*map));
	if(map == NULL)
		return NULL;

	data = calloc(sizeof(hash_entry) * initial_size);
	if(data == NULL) {
		free(map);
		return NULL;
	}

	map->size = initial_size;
	map->used_slots = 0;
	map->data = data;
	map->hash_algo = hash_algo;

	return map;
}

void hash_map_grow(hash_map *map,
                   size_t min_size)
{
	const size_t prime_sizes[] = {
    	53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
    	196613, 393241, 786433, 1572869, 3145739, 6291469, 12582917, 25165843,
    	50331653, 100663319, 201326611, 402653189, 805306457, 1610612741
    };

	hash_entry  *old_data, *new_data;
	size_t old_size, new_size;
	int i;

	if(map == NULL || map->size >= min_size)
		return;

	old_size = map->size;

	for(i = 0; i < sizeof(prime_sizes) / sizeof(prime_sizes[0]); i++) {
		if(prime_sizes[i] >= min_size) {
			new_size = prime_sizes[i];
			break;
		}
	}

	new_data = calloc(sizeof(hash_entry), new_size);
	if(new_data == NULL)
		return;

	old_data = map->data;
	map->data = new_data;

	for(i = 0; i < old_size; i++) {



			size_t new_hash = hash_map_gen_key(map, new_data[i].key);
			size_t new_pos = new_hash % old_size;

			// A key is already occupying
			if(new_data[new_pos].key != NULL) {


			}








		}
	}






}

void hash_map_add(hash_map *map,
                  unsigned char *key,
                  size_t key_len,
                  void *value)
{
	size_t
	float load_factor;

	if(map == NULL || key == NULL)
		return;










}

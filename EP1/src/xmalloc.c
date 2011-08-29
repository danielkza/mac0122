#include <stdio.h>
#include <stdlib.h>

#include "xmalloc.h"

void * xmalloc(size_t size)
{
	void * res = malloc(size);
	if(res == NULL) {
		fprintf(stderr, "memory allocation error\n");
		exit(1);
	}

	return res;
}
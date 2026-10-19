#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_ALLOCATIONS 1024

typedef struct gc_ptr {
	void *ptr;
	size_t bytes;
} gc_ptr;

void gc_free();
void gc_free_all();
void *gc_malloc(size_t bytes);
void *gc_realloc(void *ptr, size_t bytes);

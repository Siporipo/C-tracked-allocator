#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "garbage_collector.h"

gc_ptr allocations[MAX_ALLOCATIONS];
size_t alloc_idx = 0;

static bool same_bytes(void *a, void *b, size_t size) {
	if (!a || !b) {
//		printf("same_bytes() - Passed in invalid pointers!\n");
		return false;
	}
	for (size_t i = 0; i < size; i++) {
		if (*((char*)a + i) != *((char*)b + i)) {
//			printf("same_bytes() - Byte mismatch!");
			return false;
		}
	}
	return true;
}

void gc_free_all() {
	while (alloc_idx > 0) {
		alloc_idx--;
//		printf("Freeing allocation at memory address: %x\n", allocations[alloc_idx]);
		if (allocations[alloc_idx].ptr) {
			free(allocations[alloc_idx].ptr);
		}
//		printf("%zu:%zu\n", alloc_idx, allocations[alloc_idx].bytes);
		allocations[alloc_idx].bytes = 0;
	}
	return;
}

void gc_free(void *ptr) {
	if (!ptr) {
		return;
	}
	for (size_t i = 0; i < alloc_idx; i++) {
		if (i >= MAX_ALLOCATIONS) {
			return;
		}
		if (same_bytes(&ptr, &(allocations[i].ptr), sizeof(void*))) {
			free(allocations[i].ptr);
			if (i == alloc_idx - 1) {
				allocations[i].bytes = 0;
				alloc_idx--;
				break;
			}
			memcpy(
				&allocations[i],			// destination
				&allocations[i + 1],			// source
				((alloc_idx - 1) - i) * sizeof(gc_ptr)	// size (in bytes)
			);
			alloc_idx--;
			break;
		}
	}
	return;
}

void *gc_malloc(size_t bytes) {
	if (bytes == 0) {
		return NULL;
	}
	if (alloc_idx + 1 >= MAX_ALLOCATIONS) {
		alloc_idx = MAX_ALLOCATIONS;
		printf("Garbage collector overflow with alloc_idx of: %zu!\n", alloc_idx);
		goto FAIL;
	}
	char *ptr = malloc(bytes);
	if (!ptr) {
		printf("Failed memory allocation No. %d!\n", alloc_idx);
		goto FAIL;
	}
	allocations[alloc_idx++] = (gc_ptr){ .ptr=ptr, .bytes=bytes };
	return (void*)ptr;

FAIL:
	gc_free_all();
	exit(1);
}

void *gc_realloc(void *ptr, size_t bytes) {
	if (alloc_idx == 0 || !ptr) {
		goto APPEND_NEW;
	}

	size_t i = 0;
	while (i < alloc_idx) {
		if (i >= MAX_ALLOCATIONS) {
			return NULL;
		}
		if (same_bytes(&ptr, &allocations[i].ptr, sizeof(void*))) {
			if (bytes == 0) {
				gc_free(ptr);
				return NULL;
			}
			goto REPLACE_OLD;
		}
		i++;
	}

REPLACE_OLD: ;
	void *temp = malloc(bytes);
	if (!temp) {
		printf("Memory allocation has failed in gc_realloc!\n");
		gc_free_all();
		exit(1);
	}
	memcpy(temp, allocations[i].ptr, allocations[i].bytes);
	free(allocations[i].ptr);
	allocations[i] = (gc_ptr){ .ptr=temp, .bytes=bytes };
	return temp;

APPEND_NEW: ;
	ptr = gc_malloc(bytes);
	return ptr;
}

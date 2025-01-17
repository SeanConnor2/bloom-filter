#ifndef _BLOOM_H
#define _BLOOM_H
#include <stddef.h>
#include <stdbool.h>

typedef unsigned char *(hash_function)(const void *data);

typedef struct bloom_filter * bloom_t;

/* Creates a new bloom filter with no hash functions and size * 8 bits. */
bloom_t bloom_create(size_t m, int k, int n, hash_function func);
/* Frees a bloom filter. */
void bloom_free(bloom_t filter);
/* Adds a hashing function to the bloom filter. You should add all of the
* functions you intend to use before you add any items. */
//void bloom_add_hash(bloom_t filter, hash_function func);
/* Adds an item to the bloom filter. */
void bloom_add(bloom_t filter, const void *item);
/* Tests if an item is in the bloom filter.
*
* Returns false if the item has definitely not been added before. Returns true
* if the item was probably added before. */
bool bloom_test(bloom_t filter, const void *item);
//Returns the FPR
double getFPR(bloom_t filter);
double getActualFPR(bloom_t bloom);

#endif
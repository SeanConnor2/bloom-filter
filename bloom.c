#define _CRT_SECURE_NO_WARNINGS
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "bloom.h"
#include "sha256.h"

struct bloom_filter {
	hash_function *func;
	void *bits;
	size_t m; //size
	int k;
	int n; //n
};

bloom_t bloom_create(size_t m, int k, int n, hash_function func) {

	bloom_t res = calloc(1, sizeof(struct bloom_filter));
	res->m = m;
	res->bits = malloc(m);
	memset(res->bits, '\0', m);
	res->k = k;
	res->func = func;
	res->n = n;
	return res;
}

void bloom_free(bloom_t filter) {
	if (filter) {
		free(filter->bits);
		free(filter);
	}
}

void bloom_add(bloom_t filter, const void *item) {

	unsigned char * digest1 = filter->func(item);
	unsigned char digest[32];
	for (int k = 0; k < 32; k++) {
		digest[k] = digest1[k];
	}
	uint8_t *bits = filter->bits;
	unsigned int hash = 0;
	unsigned int prev = 0;
	int chunkSize = 32 / filter->k;
	unsigned char * chunk = (unsigned char *)calloc(chunkSize, sizeof(unsigned char));

	for (int i = 0; i < filter->k; i++) {
		hash = prev;
		strncpy(chunk, digest + (chunkSize * i), chunkSize);
		for (int j = 0; j < chunkSize; j++) {
			hash = (unsigned int) chunk[j];
			hash = (prev << 8) | (hash);
			prev = hash;
		}
		hash %= filter->m * 8;
		//setting the hash/8th bit of bits to 1.
		bits[hash / 8] |= 1 << hash % 8;
		memset(chunk, '\0', sizeof(chunk));
	}
	free(chunk);
}

bool bloom_test(bloom_t filter, const void *item) {

	unsigned char * digest1 = filter->func(item);
	unsigned char digest[32];
	for (int k = 0; k < 32; k++) {
		digest[k] = digest1[k];
	}
	uint8_t *bits = filter->bits;
	unsigned int hash = 0;
	unsigned int prev = 0;
	int chunkSize = 32 / filter->k;

	unsigned char * chunk = (unsigned char *)calloc(chunkSize, sizeof(unsigned char));

	for (int i = 0; i < filter->k; i++) {
		hash = prev;
		strncpy(chunk, digest + (chunkSize * i), chunkSize);
		for (int j = 0; j < chunkSize; j++) {
			hash = (unsigned int) chunk[j];
			hash = (prev << 8) | (hash);
			prev = hash;
		}
		hash %= filter->m * 8;
		if (!(bits[hash / 8] & 1 << hash % 8)) {
			return false;
		}
		memset(chunk, '\0', sizeof(chunk));
	}
	free(chunk);
	return true;
}
double getFPR(bloom_t filter) {
	return pow(1 - pow(1 - ((double)1 / (filter->m * 8)), filter->k * filter->n), filter->k) * 100;
	
}
double getActualFPR(bloom_t bloom) {

	FILE *fptr;
	char *buffer;
	long numbytes;
	double falsePositive = 0;
	//open the file in read mode
	fptr = fopen("dictionary.txt", "r");
	if (fptr == NULL) {
		printf("File was not opened\n");
		return 1;
	}
	printf("Success!\n");

	//Get # of Bytes
	fseek(fptr, 0L, SEEK_END);
	numbytes = ftell(fptr);
	//reset the file position to beginning
	fseek(fptr, 0L, SEEK_SET);
	//grab memmory for buffer
	buffer = (char*)calloc(numbytes, sizeof(char));
	//if memory error return 1
	if (buffer == NULL)
		return 1;


	for (int i = 0; i < bloom->n; i++) {
		fscanf(fptr, "%s[^\n]", buffer);
		bloom_add(bloom, buffer);
	}
	for (int l = 0; l < bloom->n; l++) {
		fscanf(fptr, "%s[^\n]", buffer);
		//Test to see if it is there, if it is falsePositive++
		if (bloom_test(bloom, buffer)) {
			falsePositive++;
		}
	}
	fclose(fptr);
	//freeing the buffer.
	free(buffer);
	return (falsePositive / (bloom->n)) * 100;
	//return 0;
}
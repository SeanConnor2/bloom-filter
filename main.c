#define _CRT_SECURE_NO_WARNINGS
#include "bloom.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>
#include "sha256.h"

/*Hashing Functions*/

int main() {
	//bloom_create(m, k, n, func); 8192, 2, 1000, SHA256 || 512, 5, 1000, SHA256 || 8192, 6, 50000, SHA256
	bloom_t bloom = bloom_create(8192, 6, 50000, SHA256);
	double theory = getFPR(bloom);
	double actual = getActualFPR(bloom);
	printf("This is the Theoretical %1f\n", theory);
	printf("This is the Actual %1f\n", actual);
	bloom_free(bloom);

	return 0;
}
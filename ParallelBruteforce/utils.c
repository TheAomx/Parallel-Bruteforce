#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SIZE 256

void readFile(char *filename, FileReader callback, void *ctx) {
	char buffer[BUFFER_SIZE];
	size_t bytesRead;
	FILE *file;
	
	file = fopen(filename, "r");
	
	if (file == NULL) {
		printf("open of %s failed!\n", filename);
		exit(EXIT_FAILURE);
	}
	
	do {
		bytesRead = fread((void*) buffer, 1, BUFFER_SIZE, file);
		callback(buffer, bytesRead, ctx);
	}
	while (bytesRead != 0);
	
	if (!feof(file)) {
		printf("error at reading occured!\n");
		exit(EXIT_FAILURE);
	}
	
	fclose(file);
}

unsigned long pow_ul (unsigned long base, unsigned long exp) {
	unsigned long t = 1;
	unsigned long i = 0;
	
	for (i = 0; i < exp; i++) {
		t *= base;
	}
		
	return t;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha1.h"

#define BUFFER_SIZE 256

typedef void (*FileReader) (void *buffer, unsigned int bytesRead, void *ctx);

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

void update_sha1 (void *buffer, unsigned int bytesRead, void *ctx) {
	SHA1_CTX *sha_context = (SHA1_CTX*) ctx;
	sha1_update(sha_context, (uchar*) buffer, bytesRead);
}

void get_sha1_from_file(SHA1_CTX *ctx, char *filename, uchar *hash) {
	sha1_init(ctx);
	readFile(filename, update_sha1, ctx);
	sha1_final(ctx, hash);
}

int main (int argc, char **argv) {
	
	
	uchar sha1Hash[SHA1_SIZE];
	
	SHA1_CTX sha_context;
	
	get_sha1_from_file(&sha_context, "test", sha1Hash);

	sha1_print(sha1Hash);
	printf("\n");
	
	
	return 0;
}

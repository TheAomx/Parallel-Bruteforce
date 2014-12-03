#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha1.h"
#include "../Utils/utils.h"
#include "../Utils/bruteforce.h"

#define PW_LEN 5

void update_sha1 (void *buffer, unsigned int bytesRead, void *ctx) {
	SHA1_CTX *sha_context = (SHA1_CTX*) ctx;
	sha1_update(sha_context, (uchar*) buffer, bytesRead);
}

void get_sha1_from_file(SHA1_CTX *ctx, char *filename, uchar *hash) {
	sha1_init(ctx);
	readFile(filename, update_sha1, ctx);
	sha1_final(ctx, hash);
}

void get_sha1_from_string_iter(SHA1_CTX *ctx, char *string, uchar *hash, int numIterations) {
	int i = 0;	
	sha1_init(ctx);
	for (i = 0; i < numIterations; i++) {
		sha1_update(ctx, (uchar*) string, strlen(string));
	}
	sha1_final(ctx, hash);
}

void get_sha1_from_string(SHA1_CTX *ctx, char *string, uchar *hash) {
	 get_sha1_from_string_iter(ctx, string, hash, 1);
}

int checkPasswordSHA1 (void *ctx, char *password, uchar *toBreakHash) {
	static uchar passwordHash[SHA1_SIZE];
	static int counter = 0;
	
	counter++;

	
	get_sha1_from_string( ctx, password, passwordHash);

	if ((counter % 100000) == 0) {			
		printf("%s\n", password);
	}
	
	if (sha1_equal(toBreakHash, passwordHash)) {
		printf("broken the hash! password was %s\n", password);
		return 1;
	}	
	return 0;
}

int main (int argc, char **argv) {
	//int i, j, k, l;
	char passphrase[]={"aa1aa"};
	char alphabet[] = {"abcdefghiklmnopqrstuvwxyzABCDEFGHIKLMOPQRSTUVXYZ123456789"};
	
	uchar toBreakHash[SHA1_SIZE];
	
	SHA1_CTX sha_context;

	get_sha1_from_string( &sha_context, passphrase, toBreakHash);
	
	bruteforcePasswordAll(&sha_context, toBreakHash, checkPasswordSHA1, alphabet, PW_LEN);
	
	return 0;
}

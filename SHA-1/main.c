#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha1.h"
#include "../Utils/utils.h"

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

void get_sha1_and_print (SHA1_CTX *ctx, char *string, uchar *hash) {

	get_sha1_from_string(ctx, string, hash);
	
	sha1_print(hash);
	printf("\n");
}

int main (int argc, char **argv) {
	
	char 
	text1[]={"abc"}, 
	text2[]={"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"}, 
	text3[]={"aaaaaaaaaa"};
	
	uchar sha1Hash[SHA1_SIZE];
	
	SHA1_CTX sha_context;

	get_sha1_and_print (&sha_context, text1, sha1Hash);

	get_sha1_and_print (&sha_context, text2, sha1Hash);

	get_sha1_from_string_iter(&sha_context, text3, sha1Hash, 100000);
	
	sha1_print(sha1Hash);
	printf("\n");

	
	get_sha1_from_file(&sha_context, "test", sha1Hash);

	sha1_print(sha1Hash);
	printf("\n");
	
	
	return 0;
}

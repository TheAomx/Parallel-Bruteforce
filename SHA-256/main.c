#include <stdio.h>
#include <string.h>
#include "sha256.h"
#include "../Utils/utils.h"

/*
Output should be:
ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1
cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0
*/

void update_sha256 (void *buffer, unsigned int bytesRead, void *ctx) {
	SHA256_CTX *sha_context = (SHA256_CTX*) ctx;
	sha256_update(sha_context, (uchar*) buffer, bytesRead);
}

void get_sha256_from_file(SHA256_CTX *ctx, char *filename, uchar *hash) {
	sha256_init(ctx);
	readFile(filename, update_sha256, ctx);
	sha256_final(ctx, hash);
}

void get_sha256_from_string_iter(SHA256_CTX *ctx, char *string, uchar *hash, int numIterations) {
	int i = 0;	
	sha256_init(ctx);
	for (i = 0; i < numIterations; i++) {
		sha256_update(ctx, (uchar*) string,strlen(string));
	}
	sha256_final(ctx, hash);
}

void get_sha256_from_string(SHA256_CTX *ctx, char *string, uchar *hash) {
	 get_sha256_from_string_iter(ctx, string, hash, 1);
}



int main()
{
	char 
	text1[]={"abc"},
	text2[]={"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"},
	text3[]={"aaaaaaaaaa"};

	uchar hash[SHA256_SIZE];

	SHA256_CTX ctx;

	// Hash one
	get_sha256_from_string(&ctx, text1, hash);
	sha256_print(hash);
	printf("\n");

	// Hash two
	get_sha256_from_string(&ctx, text2, hash);
	sha256_print(hash);
	printf("\n");

	// Hash three
	get_sha256_from_string_iter(&ctx, text3, hash, 100000);
	sha256_print(hash);
	printf("\n");

	return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sha1.h"
#include "../Utils/utils.h"

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

void get_sha1_and_print (SHA1_CTX *ctx, char *string, uchar *hash) {

	get_sha1_from_string(ctx, string, hash);
	
	sha1_print(hash);
	printf("\n");
}

char toChar(int i) {
	return 'a' + i;
}

void bruteforcePassword(SHA1_CTX *ctx, uchar *toBreakHash, char *alphabet, char *currentPassphrase, unsigned int searchLength, unsigned int currentLength) {
	static int counter = 0;
	static uchar passwordHash[SHA1_SIZE];
	
	int i=0;
	/* if it isnt the last line in the password...*/
	if (searchLength != (currentLength+1)) {
		/* we'll have to do a recursive call... */
		for (i = 0; i < strlen(alphabet); i++) {
			currentPassphrase[currentLength] = alphabet[i];
			bruteforcePassword(ctx, toBreakHash, alphabet, currentPassphrase, searchLength, currentLength+1);
		}
	}
	else {
		/*if it is the last line in the password, we can check the hash codes...*/
		for (i = 0; i < strlen(alphabet); i++) {
			counter++;
			currentPassphrase[currentLength] = alphabet[i];
			
			get_sha1_from_string( ctx, currentPassphrase, passwordHash);
		
			if ((counter % 100000) == 0) {			
				printf("%s\n", currentPassphrase);
			}
			
			if (sha1_equal(toBreakHash, passwordHash)) {
				printf("broken the hash! password was %s\n", currentPassphrase);
				exit(0);
			}
		}
	}
}

void bruteforcePasswordAll(SHA1_CTX *ctx, uchar *toBreakHash, char *alphabet, char *currentPassphrase, unsigned int searchLength) {
	int pwLen;
	
	for (pwLen = 1; pwLen <= searchLength; pwLen++) {
		bruteforcePassword(ctx, toBreakHash, alphabet, currentPassphrase, pwLen, 0);
	}
}

int main (int argc, char **argv) {
	//int i, j, k, l;
	char passphrase[]={"aa1aa"};
	char alphabet[] = {"abcdefghiklmnopqrstuvwxyzABCDEFGHIKLMOPQRSTUVXYZ123456789"};
	char passwordBuffer[PW_LEN+1]; 
	
	uchar toBreakHash[SHA1_SIZE];
	
	SHA1_CTX sha_context;

	get_sha1_from_string( &sha_context, passphrase, toBreakHash);
	
	memset(passwordBuffer, 0, sizeof(passwordBuffer));
	
	bruteforcePasswordAll(&sha_context, toBreakHash, alphabet, passwordBuffer, PW_LEN);
	
	return 0;
}

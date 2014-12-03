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

void bruteforcePasswordAll(SHA1_CTX *ctx, uchar *toBreakHash, char *alphabet, char *currentPassphrase, unsigned int searchLength, unsigned int currentLength) {
	int pwLen;
	
	for (pwLen = 1; pwLen <= searchLength; pwLen++) {
		bruteforcePassword(ctx, toBreakHash, alphabet, currentPassphrase, pwLen, currentLength);
	}
}

int main (int argc, char **argv) {
	//int i, j, k, l;
	char passphrase[]={"aa1aa"};
	char alphabet[] = {"abcdefghiklmnopqrstuvwxyzABCDEFGHIKLMOPQRSTUVXYZ123456789"};
	char passwordBuffer[PW_LEN+1]; 
	
	uchar toBreakHash[SHA1_SIZE], passwordHash[SHA1_SIZE];
	
	SHA1_CTX sha_context;

	get_sha1_from_string( &sha_context, passphrase, toBreakHash);
	
	memset(passwordBuffer, 0, sizeof(passwordBuffer));
	
	unsigned int offset = 0;

	
	bruteforcePasswordAll(&sha_context, toBreakHash, alphabet, passwordBuffer, PW_LEN, 0);
	
	/*
	for (i = 0; i < 26; i++) {
		passwordBuffer[offset++] = toChar(i);
		for (j = 0; j < 26; j++) {
			passwordBuffer[offset++] = toChar(j);
			for (k = 0; k < 26; k++) {
				passwordBuffer[offset++] = toChar(k);
				for (l = 0; l < 26; l++) {
					passwordBuffer[offset] = toChar(l);
					
					get_sha1_from_string( &sha_context, passwordBuffer, passwordHash);
					
					if (sha1_equal(toBreakHash, passwordHash)) {
						printf("broken the hash! password was %s\n", passwordBuffer);
					}

					
				}
				offset--;
			}
			offset--;
		}
		offset--;
	}
	*/


	/*

	get_sha1_and_print (&sha_context, text1, sha1Hash);

	get_sha1_and_print (&sha_context, text2, sha1Hash);

	get_sha1_from_string_iter(&sha_context, text3, sha1Hash, 100000);
	
	sha1_print(sha1Hash);
	printf("\n");

	
	get_sha1_from_file(&sha_context, "test", sha1Hash);
	
	sha1_print(sha1Hash);
	printf("\n");
	*/
	
	return 0;
}

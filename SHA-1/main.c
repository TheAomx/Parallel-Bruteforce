#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "sha1.h"
#include "../Utils/utils.h"
#include "../Utils/bruteforce.h"

unsigned long counter = 0;
unsigned long needToCheck = 0;
char *currentPassword = "a";


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

	currentPassword = password;	
	counter++;

	get_sha1_from_string( ctx, password, passwordHash);
	
	if (sha1_equal(toBreakHash, passwordHash)) {
		printf("broken the hash! password was %s\n", password);
		return 1;
	}
	return 0;
}

void handleAlarm(int signal) {
	static unsigned long counterBefore;
	static unsigned long counterNow;

	counterBefore = counterNow;
	counterNow = counter;
	
	double curProcess = ((double)counterNow / (double) needToCheck);
	
	alarm(1);
	
	printf("%s %lu pws/s total:  %.2f%%\n", currentPassword, counterNow - counterBefore, curProcess*100);
}


int main (int argc, char **argv) {
	unsigned int passwordSearchLength = 6;
	char passphrase[]={"zaaa"};
	char alphabet[] = {"abcdefghiklmnopqrstuvwxyzABCDEFGHIKLMOPQRSTUVXYZ123456789"};
	
	needToCheck = calcNumPasswords(strlen(alphabet), passwordSearchLength);
	
	uchar toBreakHash[SHA1_SIZE];
	
	SHA1_CTX sha_context;

	get_sha1_from_string( &sha_context, passphrase, toBreakHash);
	
	signal(SIGALRM, handleAlarm );

	alarm(1);
	
	/* recursive bruteforce type */
	bruteforcePasswordAll(&sha_context, toBreakHash, checkPasswordSHA1, alphabet, passwordSearchLength);
	/* iterative version: 
	bruteforcePasswordAll(&sha_context, toBreakHash, checkPasswordSHA1, alphabet, passwordSearchLength); */
	
	return 0;
}

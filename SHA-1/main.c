#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>


#include "hashing_algo.h"
#include "sha256.h"
#include "../Utils/utils.h"
#include "../Utils/bruteforce.h"

unsigned long counter = 0;
unsigned long needToCheck = 0;
char *currentPassword = "a";

HashAlgorithm *algo;

void initSRand() {
	time_t t;

    time(&t);
    srand((unsigned int)t);
}

int rand_range(int low, int high) {
	return  (rand() % high + low);
}

int checkPasswordSHA1 (void *ctx, char *password, uchar *toBreakHash) {
	static uchar passwordHash[SHA256_SIZE];
	
	HashAlgorithm *algo = (HashAlgorithm*) ctx;

	currentPassword = password;	
	counter++;

	getHashFromString( algo, password, passwordHash);
	
	if (algo->equals(toBreakHash, passwordHash)) {
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
	char passphrase[]={"zaaaa"};
	char alphabet[] = {"abcdefghiklmnopqrstuvwxyzABCDEFGHIKLMOPQRSTUVXYZ123456789"};
	char *hashAlgos[]={"SHA1", "SHA1_PROP", "MD5", "SHA256"};
	int numHashAlgos = sizeof(hashAlgos)/sizeof(char*);
	char *randomHashAlgo;
	
	needToCheck = calcNumPasswords(strlen(alphabet), passwordSearchLength);
	
	initSRand();
	
	if (argc < 2) {
		randomHashAlgo = hashAlgos[rand_range(0,  numHashAlgos)];
	}	
	else {
		unsigned int hashAlgoSelected = strtol(argv[1], NULL, 10);
		hashAlgoSelected--;
		if (hashAlgoSelected >= numHashAlgos) {
			printf("invalid selection\n");
			exit(-1);		
		}
		randomHashAlgo = hashAlgos[hashAlgoSelected];
	}
	printf("using %s as hash algo!\n", randomHashAlgo);
	algo = createHashAlgorithm(randomHashAlgo);
	
	uchar toBreakHash[algo->hashSize];

	//get_sha1_from_string( &sha_context, passphrase, toBreakHash);
	getHashFromString( algo, passphrase, toBreakHash);
	
	signal(SIGALRM, handleAlarm );

	alarm(1);
	
	/* recursive bruteforce type */
	bruteforcePasswordAll(algo, toBreakHash, checkPasswordSHA1, alphabet, passwordSearchLength);
	/* iterative version: 
	bruteforcePasswordAll(&sha_context, toBreakHash, checkPasswordSHA1, alphabet, passwordSearchLength); */
	
	return 0;
}

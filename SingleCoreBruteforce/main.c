#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>


#include "hashing_algo.h"
#include "sha256.h"
#include "password_algo.h"
#include "utils.h"
#include "bruteforce.h"

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

int checkPassword (void *ctx, char *password, uchar *toBreakHash) {
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

void handleAlarm(int signalNum) {
	static unsigned long counterBefore;
	static unsigned long counterNow;

	counterBefore = counterNow;
	counterNow = counter;
	
	double curProcess = ((double)counterNow / (double) needToCheck);
	
	signal(SIGALRM, handleAlarm );
	alarm(1);
	
	printf("%s %lu pws/s total:  %.2f%%\n", currentPassword, counterNow - counterBefore, curProcess*100);
}


int main (int argc, char **argv) {
	unsigned int passwordSearchLength = 8;
	char passphrase[]={"99999999"};
	//char alphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMOPQRSTUVXYZ0123456789"};
	char alphabet[] = {"0123456789"};
	char *hashAlgos[]={"SHA1", "SHA1_PROP", "MD5", "MD5_SPH", "SHA256", "SHA256_SPH"};
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
	
	uchar hashBuffer[algo->hashSize];

	getHashFromString( algo, passphrase, hashBuffer);
	//getHashFromFile(algo, "main.c", hashBuffer);
	printf("generated hash is %s\n", algo->toString(hashBuffer));
	
	signal(SIGALRM, handleAlarm );

	alarm(1);
	
	/* passwordAt bruteforce type */
	bruteforcePasswordAt(algo, hashBuffer, checkPassword, alphabet, passwordSearchLength);
	
	/* recursive bruteforce type 
	bruteforcePasswordAll(algo, hashBuffer, checkPassword, alphabet, passwordSearchLength);*/
	/* iterative version: 
	bruteforcePasswordIter(algo, hashBuffer, checkPassword, alphabet, passwordSearchLength); */

	return 0;
}

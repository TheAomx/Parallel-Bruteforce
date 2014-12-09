#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "bruteforce.h"

static inline int isLastLine(unsigned int passwordLength, unsigned int currentIndex) {
	return passwordLength == (currentIndex+1);
}

void bruteforcePassword(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex) {
	
	int i=0;	
	for (i = 0; i < strlen(alphabet); i++) {
		currentPassphrase[currentIndex] = alphabet[i];
		
		/* if it isnt the last line in the password...*/
		if (!isLastLine(passwordLength, currentIndex)) {
			/* we'll have to do a recursive call... */
			bruteforcePassword(ctx, toBreakHash, callback, alphabet, currentPassphrase, passwordLength, currentIndex+1);
		}
		else {
			/*if it is the last line in the password, we can check the hash codes...*/
			if (callback((void*) ctx, currentPassphrase, toBreakHash)) {
				exit(0);
			}
		}
	}
}

void bruteforcePasswordAll(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength) {
	int pwLen;
	
	char passwordBuffer[maxPasswordLength+1]; 
	
	memset(passwordBuffer, 0, sizeof(passwordBuffer));
	
	for (pwLen = 1; pwLen <= maxPasswordLength; pwLen++) {
		bruteforcePassword(ctx, toBreakHash, callback, alphabet, passwordBuffer, pwLen, 0);
	}
}

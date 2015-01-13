/* 
 * File:   bruteforce.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include "core_headers.h"

int bruteforcePasswordIter(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength) {
    const size_t charset_max = strlen(alphabet);
    char permutation[maxPasswordLength + 1];
    int charset_array[maxPasswordLength];
	int i;

    memset(permutation, 0, sizeof(permutation));
    memset(charset_array, 0, sizeof(charset_array));

    int current_len = 0;
    int current_byte = 0;
    while (current_len < maxPasswordLength) {
        for (i = 0; i <= current_len; i++)
            permutation[i] = alphabet[charset_array[current_len - i]];

        if (callback((void*) ctx, permutation, toBreakHash))
            return 0;

        charset_array[current_byte]++;
        while (charset_array[current_byte] == charset_max) {
            charset_array[current_byte] = 0;
            current_byte++;
            if (current_byte > current_len) {
                current_len = current_byte;
                break;
            }
            charset_array[current_byte]++;
        }
        current_byte = 0;
    }

    return 1;
}
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

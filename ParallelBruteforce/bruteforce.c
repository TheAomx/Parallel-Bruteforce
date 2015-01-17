/* 
 * File:   bruteforce.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include "core_headers.h"

int bruteforcePasswordIter(void *ctx, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength) {
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

        if (callback((void*) ctx, permutation))
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

void bruteforcePassword(void *ctx, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex) {
	
    int i=0;
    #pragma omp parallel for 
    for (i = 0; i < strlen(alphabet); i++) {
        currentPassphrase[currentIndex] = alphabet[i];
		
        /* if it isnt the last line in the password...*/
        if (!isLastLine(passwordLength, currentIndex)) {
            /* we'll have to do a recursive call... */
            bruteforcePassword(ctx, callback, alphabet, currentPassphrase, passwordLength, currentIndex+1);
        }
        else {
            /*if it is the last line in the password, we can check the hash codes...*/
            if (callback((void*) ctx, currentPassphrase)) {
                exit(0);
            }
        }
    }
}

void bruteforcePasswordAll(void *ctx, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength, int rank, int nTasks) {
    int pwLen;
    int searchStart = 1;
	
    char passwordBuffer[maxPasswordLength+1]; 
	
    memset(passwordBuffer, 0, sizeof(passwordBuffer));       
        
    int i = 0;

    for(pwLen = 2; pwLen <= maxPasswordLength; pwLen++) {
        for(i = (rank-1); i < strlen(alphabet); i += (nTasks-1)) {
            passwordBuffer[0] = alphabet[i];
            printf("[%d]Testing %s....\n", rank, &passwordBuffer[0]);
            bruteforcePassword(ctx, callback, alphabet, passwordBuffer, pwLen, searchStart);
        }
    }
        
        

}

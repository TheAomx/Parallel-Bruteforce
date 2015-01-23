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

    memset(permutation, 0, sizeof (permutation));
    memset(charset_array, 0, sizeof (charset_array));

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
    return passwordLength == (currentIndex + 1);
}

static void bruteforcePasswordSimple(void *ctx, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex) {
    //int i=0;
    for (int i = 0; i < strlen(alphabet); i++) {
        currentPassphrase[currentIndex] = alphabet[i];

        /* if it isnt the last line in the password...*/
        if (!isLastLine(passwordLength, currentIndex)) {
            /* we'll have to do a recursive call... */
            bruteforcePasswordSimple(ctx, callback, alphabet, currentPassphrase, passwordLength, currentIndex + 1);
        } else {
            /*if it is the last line in the password, we can check the hash codes...*/
            if (callback((void*) ctx, currentPassphrase)) {
                exit(0);
            }
        }
    }
}

void bruteforcePassword(void *ctx, bruteforceCallback callback, char *alphabet, char **passphraseBuffer, unsigned int passwordLength, unsigned int currentIndex) {
    //int i=0;
#pragma omp parallel for 
    for (int i = 0; i < strlen(alphabet); i++) {

        int threadID = getThreadID();
        char *currentPassphrase = passphraseBuffer[threadID];



        currentPassphrase[currentIndex] = alphabet[i];
        /* if it isnt the last line in the password...*/
        if (!isLastLine(passwordLength, currentIndex)) {
            /* we'll have to do a recursive call... */
            bruteforcePasswordSimple(ctx, callback, alphabet, currentPassphrase, passwordLength, currentIndex + 1);
        } else {
            /*if it is the last line in the password, we can check the hash codes...*/
            if (callback((void*) ctx, currentPassphrase)) {
                exit(0);
            }
        }
    }
}

void bruteforcePasswordAll(void *ctx, bruteforceCallback callback, char *alphabet, char **passphraseBuffer, unsigned int maxPasswordLength, int rank, int nTasks) {

    int pwLen;
    int searchStart = 1;

    PasswordHashes *pwHashes = (PasswordHashes*) ctx;
    int i = 0, j = 0;
    int numThreads = pwHashes->numThreads;

    for (pwLen = 2; pwLen <= maxPasswordLength; pwLen++) {
        for (i = (rank - 1); i < strlen(alphabet); i += (nTasks - 1)) {
            for (j = 0; j < numThreads; j++) {
                passphraseBuffer[j][0] = alphabet[i];
            }
            //printf("[%d]Testing %s....\n", rank, passphraseBuffer[0]);
            bruteforcePassword(ctx, callback, alphabet, passphraseBuffer, pwLen, searchStart);
        }
    }



}

void countPasswords(void *ctx, bruteforceCallback callback,char* start, ulong count){
    
}

void bruteforcePasswordTask(PasswordGenTask* taskInfo, void *ctx, bruteforceCallback callback, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    char* tmpPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    char* tmpPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);

    memset(tmpPwd, '\0', sizeof (char)*MAX_PASSWORD);
    memset(tmpPwd2, '\0', sizeof (char)*MAX_PASSWORD);

    strncpy(tmpPwd, taskInfo->startPassword, len);

   
    //DBG_OK("           %s(%ld)      ->      %s(%ld)    :%ld   ", tmpPwd,(pwGenType->valueOf(tmpPwd)),tmpPwd2,(pwGenType->valueOf(tmpPwd2)),i);
    ulong count = context->passwordDiff(taskInfo->startPassword,taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count,taskInfo->startPassword,taskInfo->endPassword);
    //FIXME: Splitting work into peaces and do a parallel region with countPasswords.
    
    for (ulong i = 0; i <= count; i++) {    
        context->nextPassword(tmpPwd, tmpPwd2);
        
        strncpy(tmpPwd, tmpPwd2, MAX_PASSWORD);
         
    }
}

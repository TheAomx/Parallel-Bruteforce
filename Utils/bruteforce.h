#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

typedef unsigned char uchar;

typedef int (*bruteforceCallback) (void *ctx, char *password, uchar *toBreakHash);

void bruteforcePassword(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex);

void bruteforcePasswordAll(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);


#endif

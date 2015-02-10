#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

typedef unsigned char uchar;

typedef int (*bruteforceCallback) (void *ctx, char *password, uchar *toBreakHash);

void bruteforcePasswordAt(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);

void bruteforcePassword(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex);

int bruteforcePasswordIter(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);

void bruteforcePasswordAll(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);


#endif

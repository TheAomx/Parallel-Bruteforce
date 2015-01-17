/* 
 * File:   bruteforce.h
 *      Defines bruteforce password - hash - generation functions. 
 *      The hash generation algorithm can be user defined through the <code>bruteforceCallback</code> function pointer type.
 * Author: TBD
 *
 * Created on 13. Januar 2015, 15:17
 */

#ifndef BRUTEFORCE_H
#define BRUTEFORCE_H

typedef unsigned char uchar;
/**
 * Function pointer definition to be used as a generic function interface for hash generation functions.
 * Enables easy hashing algorithm replacement.
 */
typedef int (*bruteforceCallback) (void *ctx, char *password);
/**
 * 
 * @param ctx The hash generation context (arbitrary data).
 * @param toBreakHash The array of target hashes to search for.
 * @param callback The hash generation function to be used.
 * @param alphabet The password alphabet
 * @param currentPassphrase 
 * @param passwordLength 
 * @param currentIndex
 */
void bruteforcePassword(void *ctx, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex);
/**
 * 
 * @param ctx The hash generation context (arbitrary data).
 * @param toBreakHash The array of target hashes to search for.
 * @param callback The hash generation function to be used.
 * @param alphabet The password alphabet
 * @param maxPasswordLength
 * @return 
 */
int bruteforcePasswordIter(void *ctx, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);
/**
 * 
 * @param ctx The hash generation context (arbitrary data).
 * @param toBreakHash The array of target hashes to search for.
 * @param callback The hash generation function to be used.
 * @param alphabet The password alphabet
 * @param maxPasswordLength
 */
void bruteforcePasswordAll(void *ctx, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength, int rank, int nTasks);


#endif

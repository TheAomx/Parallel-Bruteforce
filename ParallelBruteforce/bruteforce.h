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
typedef int (*bruteforceCallback) (void *ctx, char *password, uchar *toBreakHash);
/**
 * 
 * @param ctx
 * @param toBreakHash The array of target hashes to search for.
 * @param callback The 
 * @param alphabet
 * @param currentPassphrase
 * @param passwordLength
 * @param currentIndex
 */
void bruteforcePassword(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, char *currentPassphrase, unsigned int passwordLength, unsigned int currentIndex);
/**
 * 
 * @param ctx
 * @param toBreakHash
 * @param callback
 * @param alphabet
 * @param maxPasswordLength
 * @return 
 */
int bruteforcePasswordIter(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);
/**
 * 
 * @param ctx
 * @param toBreakHash
 * @param callback
 * @param alphabet
 * @param maxPasswordLength
 */
void bruteforcePasswordAll(void *ctx, uchar *toBreakHash, bruteforceCallback callback, char *alphabet, unsigned int maxPasswordLength);


#endif
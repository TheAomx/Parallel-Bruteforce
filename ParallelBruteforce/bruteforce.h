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

#include "password_algo.h"

struct BruteforceProgressStats {
    double time;
    char takeTimeOffset;
    double timeDiff;
    double kiloHashesPerSecond;
    ulong checkedLast;
    ulong pwDiff;
};

typedef struct BruteforceProgressStats BruteforceProgressStats;

/**
 * Simple callback to be used with bruteforcePasswordTask function.
 */
typedef void (*hashFoundCallback)(char* password, char* hash);

/**
 * Function pointer definition to be used as a generic function interface for hash generation functions.
 * Enables easy hashing algorithm replacement.
 */
typedef int (*bruteforceCallbackObserved) (void *ctx, char *password, hashFoundCallback onHashFound);

/**
 * Function pointer definition to be used as a generic function interface for hash generation functions.
 * Enables easy hashing algorithm replacement.
 */
typedef int (*bruteforceCallback) (void *ctx, char *password);

/**
 * Performs a brute force operation from the information provided by the information supplied by <code>taskInfo</code>. 
 * It will use the supplied brute force callback (<code>callback</code>) for hash generation.
 * @param taskInfo The information of the task to be perform.
 * @param ctx 
 * @param callback
 * @param passphraseBuffer
 * @param mpiRank
 */
void bruteforcePasswordTask(PasswordGenTask* taskInfo, void *ctx, bruteforceCallback callback, char **passphraseBuffer, int mpiRank);

/**
 * Provides functionality equal to 'bruteforcePasswordTask'. In difference, this method enables the you to pass an user defined callback which is invoked when a password is found.
 * @param taskInfo
 * @param ctx
 * @param callback
 * @param onHashFound
 * @param passphraseBuffer
 * @param mpiRank
 */
void bruteforcePasswordTaskObserved(PasswordGenTask* taskInfo, void *ctx, bruteforceCallbackObserved callback, hashFoundCallback onHashFound, char **passphraseBuffer, int mpiRank);




#endif

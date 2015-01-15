/* 
 * File:   pb_client.h
 * Author: TBD
 *      Defines client side functions.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef PB_CLIENT_H
#define	PB_CLIENT_H

#include "hashing_algo.h"


#ifdef	__cplusplus
extern "C" {
#endif


    /**
     * Computes the next password.
     * 'Next' means the next number which results when interpreting the current password as a number of base #alphabet and incrementing by 1. (current+1) 
     * @param current The current password.
     * @param alphabet The alphabet
     * @return The next password.
     */
    char* nextPassword(char* current, char* alphabet);
    
    struct PasswordHashes {
        HashAlgorithm *algo;
        unsigned long numHashes;
        uchar **hashes;
    };
    
    typedef struct PasswordHashes PasswordHashes;
    
    void printHashes(PasswordHashes *pwHashes, int rank);
    PasswordHashes* generatePasswordHashes(MPI_File *in);
    void freePasswordHashes(PasswordHashes *pwHashes);

#ifdef	__cplusplus
}
#endif

#endif	/* PB_CLIENT_H */


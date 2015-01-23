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



    struct PasswordHashes {
        HashAlgorithm **algo;
        unsigned long numHashes;
        unsigned int numThreads;
        uchar **hashBuffer;
        uchar **hashes;
    };
    typedef struct PasswordHashes PasswordHashes;
   
    
    

    void printHashes(PasswordHashes *pwHashes, int rank);
    PasswordHashes* generatePasswordHashes(MPI_File *in, unsigned int numThreads);
    
    
    void freePasswordHashes(PasswordHashes *pwHashes);

    
    
#ifdef	__cplusplus
}
#endif

#endif	/* PB_CLIENT_H */


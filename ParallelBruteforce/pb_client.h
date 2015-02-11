/* 
 * File:   pb_client.h
 * Author: TBD
 *      Defines client side functions.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef PB_CLIENT_H
#define	PB_CLIENT_H

#include <mpi.h>

#include "bruteforce.h"
#include "uthash.h"
#include "HashAlgos/hashing_algo.h"

#ifdef	__cplusplus
extern "C" {
#endif


    /**
     * Scructure representing an element of a hash list. The list is build up and searched using macros of "uthash.h"
     */
    struct HashTableEntry {
        uchar* id; /* we'll use this field as the key */
        uchar* hash;
        UT_hash_handle handle; /* makes this structure hashable */
    };
    typedef struct HashTableEntry HashTableEntry;
    
    typedef struct ThreadContext ThreadContext;
    typedef struct AttackStrategy AttackStrategy;
    
    typedef void* (*HashFileParser)(ThreadContext *threadContext, char **data, unsigned int numLines);
    typedef void (*HashDataFreeMethod) (ThreadContext *threadContext);
    
    struct AttackStrategy {
        HashFileParser hashFileParser;
        HashDataFreeMethod free;
        bruteforceCallbackObserved bruteforceMethod;
        void *attackData;
    };
    
    struct ThreadContext {
        unsigned int numThreads;
        unsigned long numHashes;
        uchar **hashBuffer;
        AttackStrategy *attackStrategy;
    };
   
    
    /**
     * Structure containing a list of password hashes and the respecting hashing algorithm,
     *  which can be used to create such hashes.
     * 
     */
    struct PasswordHashes {
        HashAlgorithm **algo;
        
        uchar **hashes;
        HashTableEntry **hashesHashTables;
    };
    
    
    typedef struct PasswordHashes PasswordHashes;

	/**
     * Gets the hash of the hashID
     * @param hashID
     */
	
    uchar* getHash(ThreadContext *context, PasswordHashes *pwHashes, int threadID, unsigned int hashID);

    /**
     * Print the array of hashes supplied.
     * @param pwHashes
     * @param rank
     */
    void printHashes(ThreadContext *context, int rank);
    
    ThreadContext* createThreadContext(MPI_File *in, unsigned int numThreads);
    void freeThreadContext(ThreadContext *context);



#ifdef	__cplusplus
}
#endif

#endif	/* PB_CLIENT_H */


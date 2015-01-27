/* 
 * File:   pb_client.h
 * Author: TBD
 *      Defines client side functions.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef PB_CLIENT_H
#define	PB_CLIENT_H

#include "core_headers.h"


#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Structure containing a list of password hashes and the respecting hashing algorithm,
     *  which can be used to create such hashes.
     * 
     */
    struct PasswordHashes {
        /**
         * Array of hash algorithms.
         */
        HashAlgorithm **algo;
        /**
         * Number of hashes within this structure.
         */
        unsigned long numHashes;

        unsigned int numThreads;

        uchar **hashBuffer;
        /**
         * Array of hashes.
         */
        uchar **hashes;
    };
    typedef struct PasswordHashes PasswordHashes;

	/**
     * Gets the hash of the hashID
     * @param hashID
     */
	
    uchar* getHash(PasswordHashes *pwHashes, int threadID, int hashID);



    /**
     * Print the array of hashes supplied.
     * @param pwHashes
     * @param rank
     */
    void printHashes(PasswordHashes *pwHashes, int rank);

    /**
     * Create an instance of <code>PasswordHashes</code> from the supplied file.
     * @param in
     * @param numThreads
     * @return 
     */
    PasswordHashes* generatePasswordHashes(MPI_File *in, unsigned int numThreads);


    /**
     * Frees up previously allocated <code>PasswordHashes</code>.
     * @param pwHashes
     */
    void freePasswordHashes(PasswordHashes *pwHashes);



#ifdef	__cplusplus
}
#endif

#endif	/* PB_CLIENT_H */


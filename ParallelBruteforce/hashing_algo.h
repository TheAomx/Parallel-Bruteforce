/* 
 * File:   hashing_algo.h
 * Author: TBD
 *
 * Created on 13. Januar 2015, 20:30
 */

#ifndef HASHING_ALGO_H
#define	HASHING_ALGO_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "core_headers.h"

    /**
     * Defines supported types of hashing function.
     */
    enum HashTypes {
        SHA1, SHA256, MD5
    };

    typedef char* (*hash_toString_fct)(unsigned char* hash);
    typedef int (*hash_equals_fct)(unsigned char hash1[], unsigned char hash2[]);
    typedef void (*hash_init_fct)(void *ctx);
    typedef void (*hash_update_fct)(void *ctx, uchar data[], uint len);
    typedef void (*hash_final_fct)(void *ctx, uchar hash[]);
    typedef int (*hash_len)(void *ctx);

    /**
     * Encapsulates calculation, comparison and conversion
     * of password hashes.
     */
    struct HashAlgorithm {
        /**
         * The type of algorithm implemented by this.
         */
        enum HashTypes hashType;
        /**
         * The hashing context used by the hashing functions.
         */
        void *ctx;
        /**
         * The size if hashes the algorithm will create.
         */
        unsigned int hashSize;
        /**
         * Callback to convert hash to string.
         */
        hash_toString_fct toString;
        /**
         * Callback for checking hash equality.
         */
        hash_equals_fct equals;
        /**
         * Callback for initialization.
         */
        hash_init_fct init;
        /**
         * Callback for hash update function.
         */
        hash_update_fct update;
        /**
         * Callback for hash finalizing function. -> this function will create the resulting hash.
         */
        hash_final_fct final;
    };

    typedef struct HashAlgorithm HashAlgorithm;

    /**
     * Checks if the supplied string denotes a known hashing algorithm
     * @param line
     * @return 
     */
    static inline int isSupportedHashAlgorithm(char *line) {
        return (strcmp(line, "SHA1") == 0 || strcmp(line, "SHA256") == 0 || strcmp(line, "SHA1_PROP") == 0 || strcmp(line, "MD5") == 0);
    }

    /**
     * Creates the hashing algorithm defined by the provided string.
     * @param hashAlgorithm The name of the hash algorithm to create.
     * @return 
     */
    HashAlgorithm* createHashAlgorithm(char *hashAlgorithm);
    void freeHashAlgo(HashAlgorithm *algo);

    /**
     * Create the hash from the given file using the specified algorithm.
     * @param algo
     * @param filename
     * @param hash
     */
    void getHashFromFile(HashAlgorithm *algo, char *filename, uchar *hash);
    /**
     * Create the hash from the given string using the specified algorithm.
     * @param algo
     * @param string
     * @param hash
     */
    void getHashFromString(HashAlgorithm *algo, char *string, uchar *hash);
    /**
     * Create the hash from the given string using the specified algorithm and the given number of hash update iterations.
     * 
     * @param algo
     * @param string
     * @param hash
     */
    void getHashFromStringIter(HashAlgorithm *algo, char *string, uchar *hash, int numIterations);

    /**
     * Convert the given hash string (created by the specified algorithm) to binary form.
     * @param algo
     * @param hashString
     * @return 
     */
    uchar* convertHashStringToBinary(HashAlgorithm *algo, sds hashString);



#ifdef	__cplusplus
}
#endif

#endif	/* HASHING_ALGO_H */


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
    
enum HashTypes { 
    SHA1, SHA256
};
    
typedef void (*hash_print_fct)(unsigned char* hash);
typedef int (*hash_equals_fct)(unsigned char hash1[], unsigned char hash2[]);
typedef void (*hash_init_fct)(void *ctx);
typedef void (*hash_update_fct)(void *ctx, uchar data[], uint len);
typedef void (*hash_final_fct)(void *ctx, uchar hash[]);

struct HashAlgorithm {
    enum HashTypes hashType;
    void *ctx;
    hash_print_fct print;
    hash_equals_fct equals;
    hash_init_fct init;
    hash_update_fct update;
    hash_final_fct final;
};

typedef struct HashAlgorithm HashAlgorithm;

HashAlgorithm* createHashAlgorithm(char *hashAlgorithm);

void freeHash(HashAlgorithm *algo);

void getHashFromFile(HashAlgorithm *algo, char *filename, uchar *hash);
void getHashFromString(HashAlgorithm *algo, char *string, uchar *hash);
void getHashFromStringIter(HashAlgorithm *algo, char *string, uchar *hash, int numIterations);



#ifdef	__cplusplus
}
#endif

#endif	/* HASHING_ALGO_H */


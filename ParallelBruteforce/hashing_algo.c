#include <string.h>


#include "hashing_algo.h"
#include "sha1.h"

void freeHash(HashAlgorithm *algo) {
    free(algo->ctx);
    free(algo);
}

static HashAlgorithm* createSHA1() {
    HashAlgorithm *sha1 = (HashAlgorithm*) malloc(sizeof(HashAlgorithm));
    SHA1_CTX *sha_context = (SHA1_CTX*) malloc(sizeof(SHA1_CTX));
    sha1->hashType = SHA1;
    sha1->ctx = (void*) sha_context;
    sha1->print = sha1_print;
    sha1->equals = sha1_equal;
    sha1->init = sha1_init;
    sha1->update = sha1_update;
    sha1->final = sha1_final;
    return sha1;
}

static HashAlgorithm* createSHA256() {
    HashAlgorithm *sha256 = (HashAlgorithm*) malloc(sizeof(HashAlgorithm));
    SHA256_CTX *sha256_context = (SHA256_CTX*) malloc(sizeof(SHA256_CTX));
    sha256->hashType = SHA256;
    sha256->ctx = (void*) sha256_context;
    sha256->print = sha256_print;
    sha256->equals = sha256_equal;
    sha256->init = sha256_init;
    sha256->update = sha256_update;
    sha256->final = sha256_final;
    return sha256;
}

HashAlgorithm* createHashAlgorithm(char *hashAlgorithm) {
    if (!strcmp("SHA1", hashAlgorithm)) {
        return createSHA1();
    }
    else if(!strcmp("SHA256", hashAlgorithm)) {
        return createSHA256();
    }
    else {
        return NULL;
    }
}

void updateHash (void *buffer, unsigned int bytesRead, void *ctx) {
    HashAlgorithm *algo = (HashAlgorithm*) ctx;
    algo->update(algo->ctx, (uchar*) buffer, bytesRead);
}

void getHashFromFile(HashAlgorithm *algo, char *filename, uchar *hash) {
    algo->init(algo->ctx);
    readFile(filename, updateHash, algo);
    algo->final(algo->ctx, hash);
}

void getHashFromStringIter(HashAlgorithm *algo, char *string, uchar *hash, int numIterations) {
    int i = 0;	
    algo->init(algo->ctx);
    for (i = 0; i < numIterations; i++) {
        algo->update(algo->ctx, (uchar*) string, strlen(string));
    }
    algo->final(algo->ctx, hash);
}

void getHashFromString(HashAlgorithm *algo, char *string, uchar *hash) {
    getHashFromStringIter(algo, string, hash, 1);
}
/* 
 * File:   hashing_algo.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include <string.h>
#include <stdlib.h>



#include "hashing_algo.h"

#include "sha1.h"
#include "md5.h"
#include "sph_md5.h"
#include "sha256.h"
#include "sph_sha2.h"
#include "sha1_prop.h"
#include "../utils.h"

void freeHashAlgo(HashAlgorithm *algo) {
    free(algo->ctx);
    free(algo);
}

static HashAlgorithm* createSHA1() {
    HashAlgorithm *sha1 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    SHA1_CTX *sha_context = (SHA1_CTX*) malloc(sizeof (SHA1_CTX));
    sha1->hashType = SHA1;
    sha1->ctx = (void*) sha_context;
    sha1->hashSize = SHA1_SIZE;
    sha1->toString = sha1_toString;
    sha1->equals = sha1_equal;
    sha1->init = sha1_init;
    sha1->update = sha1_update;
    sha1->final = sha1_final;
    return sha1;
}

static HashAlgorithm* createSHA1Prop() {
    HashAlgorithm *sha1 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    sph_sha1_context *sha_context = (sph_sha1_context*) malloc(sizeof (sph_sha1_context));
    sha1->hashType = SHA1;
    sha1->ctx = (void*) sha_context;
    sha1->hashSize = SHA1_PROP_SIZE;
    sha1->toString = sha1_toString;
    sha1->equals = sha1_equal_prop;
    sha1->init = sha1_init_prop;
    sha1->update = sha1_update_prop;
    sha1->final = sha1_final_prop;
    return sha1;
}

static HashAlgorithm* createSHA256() {
    HashAlgorithm *sha256 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    SHA256_CTX *sha256_context = (SHA256_CTX*) malloc(sizeof (SHA256_CTX));
    sha256->hashType = SHA256;
    sha256->ctx = (void*) sha256_context;
    sha256->hashSize = SHA256_SIZE;
    sha256->toString = sha256_toString;
    sha256->equals = sha256_equal;
    sha256->init = sha256_init;
    sha256->update = sha256_update;
    sha256->final = sha256_final;
    return sha256;
}

static HashAlgorithm* createSHA256SPH() {
    HashAlgorithm *sha256 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    sph_sha256_context *sha256_context = (sph_sha256_context*) malloc(sizeof (sph_sha256_context));
    sha256->hashType = SHA256;
    sha256->ctx = (void*) sha256_context;
    sha256->hashSize = SHA256_SPH_SIZE;
    sha256->toString = sha256_toString;
    sha256->equals = sha256_equal_sph;
    sha256->init = sha256_init_sph;
    sha256->update = sha256_update_sph;
    sha256->final = sha256_final_sph;
    return sha256;
}

static HashAlgorithm* createMD5() {
    HashAlgorithm *md5 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    MD5_CTX *md5_context = (MD5_CTX*) malloc(sizeof (MD5_CTX));
    md5->hashType = MD5;
    md5->ctx = (void*) md5_context;
    md5->hashSize = MD5_SIZE;
    md5->toString = md5_toString;
    md5->equals = md5_equal;
    md5->init = md5_init;
    md5->update = md5_update;
    md5->final = md5_final;
    return md5;
}

static HashAlgorithm* createMD5SPH() {
    HashAlgorithm *md5 = (HashAlgorithm*) malloc(sizeof (HashAlgorithm));
    sph_md5_context *md5_context = (sph_md5_context*) malloc(sizeof (sph_md5_context));
    md5->hashType = MD5;
    md5->ctx = (void*) md5_context;
    md5->hashSize = MD5_SIZE;
    md5->toString = md5_toString;
    md5->equals = md5_equal_sph;
    md5->init = md5_init_sph;
    md5->update = md5_update_sph;
    md5->final = md5_final_sph;
    return md5;
}

HashAlgorithm* createHashAlgorithm(char *hashAlgorithm) {
    if (!strcmp("SHA1", hashAlgorithm)) {
        return createSHA1();
    }
    else if (!strcmp("SHA1_PROP", hashAlgorithm)) {
        return createSHA1Prop();
    }
    else if(!strcmp("SHA256", hashAlgorithm)) {
        return createSHA256();
    } 
	else if(!strcmp("SHA256_SPH", hashAlgorithm)) {
        return createSHA256SPH();
    } 
	else if (!strcmp("MD5", hashAlgorithm)) {
        return createMD5();
    }
	else if (!strcmp("MD5_SPH", hashAlgorithm)) {
        return createMD5SPH();
    }		 
	else {
        return NULL;
    }
}

static void updateHash(void *buffer, unsigned int bytesRead, void *ctx) {
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

static uchar hexCharToBin(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else {
        return 0;
    }
}

static uchar hexToBin(char c1, char c2) {
    uchar temp = 0;
    temp = hexCharToBin(c2);
    temp |= hexCharToBin(c1) << 4;
    return temp;
}

uchar* convertHashStringToBinary(HashAlgorithm *algo, char *hashString, uchar *hashBinary) {
    unsigned int i, j;
    //uchar *hashBinary = (uchar*) malloc(sizeof (uchar) * algo->hashSize);
    for (i = 0, j = 0; i < algo->hashSize; i++, j += 2) {
        hashBinary[i] = hexToBin(hashString[j], hashString[j + 1]);
    }
    return hashBinary;
}

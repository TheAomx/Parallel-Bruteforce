/* 
 * File:   pb_client.h
 * Author: TBD
 *      Defines SHA-1 algorithm functions and context.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef __SHA1_H
#define __SHA1_H
#include "core_headers.h"


#define SHA1_SIZE 20

/**
 * The SHA1 hashing context.
 */
typedef struct {
    uchar data[64];
    uint datalen;
    uint bitlen[2];
    uint state[5];
    uint k[4];
} SHA1_CTX;

char* sha1_toString(uchar hash[]);
int sha1_equal(uchar hash1[], uchar hash2[]);
void sha1_init(void *context);
void sha1_update(void *context, uchar data[], uint len);
void sha1_final(void *context, uchar hash[]);

#endif

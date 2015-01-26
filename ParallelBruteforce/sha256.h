/* 
 * File:   pb_client.h
 * Author: TBD
 *      Defines SHA-256 algorithm functions and context.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef __SHA256_H
#define __SHA256_H

#define SHA256_SIZE 32

/**
 * The SHA256 hashing context.
 */
typedef struct {
    uchar data[64];
    uint datalen;
    uint bitlen[2];
    uint state[8];
} SHA256_CTX;

char* sha256_toString(unsigned char hash[]);
int sha256_equal(unsigned char hash1[], unsigned char hash2[]);
void sha256_init(void *context);
void sha256_update(void *context, uchar data[], uint len);
void sha256_final(void *context, uchar hash[]);
#endif

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

typedef struct {
    uchar data[64];
    uint datalen;
    uint bitlen[2];
    uint state[5];
    uint k[4];
} SHA1_CTX;

void sha1_print(unsigned char hash[]);
int sha1_equal(unsigned char hash1[], unsigned char hash2[]);
void sha1_init(SHA1_CTX *ctx);
void sha1_update(SHA1_CTX *ctx, uchar data[], uint len);
void sha1_final(SHA1_CTX *ctx, uchar hash[]);

#endif

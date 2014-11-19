#ifndef __SHA256_H
#define __SHA256_H

typedef unsigned char uchar; // 8-bit byte
typedef unsigned int uint; // 32-bit word

#define SHA256_SIZE 32

typedef struct {
   uchar data[64];
   uint datalen;
   uint bitlen[2];
   uint state[8];
} SHA256_CTX;

void sha256_print(unsigned char hash[]);
void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, uchar data[], uint len);
void sha256_final(SHA256_CTX *ctx, uchar hash[]);
#endif

// Code by: B-Con (http://b-con.us) 
// Released under the GNU GPL 
// MD5 Hash Digest implementation (little endian byte order) 
// http://bradconte.com/sha1_c

#ifndef __SHA1_H
#define __SHA1_H

// Signed variables are for wimps 
typedef unsigned char uchar;
typedef unsigned int uint; 

#define SHA1_SIZE 20

typedef struct { 
   uchar data[64]; 
   uint datalen; 
   uint bitlen[2]; 
   uint state[5]; 
   uint k[4]; 
} SHA1_CTX; 

void sha1_print(unsigned char hash[]);
void sha1_init(SHA1_CTX *ctx);
void sha1_update(SHA1_CTX *ctx, uchar data[], uint len);
void sha1_final(SHA1_CTX *ctx, uchar hash[]);

#endif

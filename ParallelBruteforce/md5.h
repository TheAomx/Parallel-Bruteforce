/* 
 * File:   md5.h
 * Author: aomx
 *
 * Created on 19. Januar 2015, 00:41
 */

#ifndef MD5_H
#define	MD5_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MD5_SIZE   16

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/**************************** DATA TYPES ****************************/
typedef unsigned char uchar;             // 8-bit byte
typedef unsigned int  uint;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
   uchar data[64];
   uint datalen;
   unsigned long long bitlen;
   uint state[4];
} MD5_CTX;



char* md5_toString(unsigned char hash[]);
int md5_equal(unsigned char hash1[], unsigned char hash2[]);
void md5_init(void *context);
void md5_update(void *context, uchar data[], uint len);
void md5_final(void *context, uchar hash[]);

#endif



#ifdef	__cplusplus
}
#endif

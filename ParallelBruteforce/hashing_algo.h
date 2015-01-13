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

void (*hash_print)(unsigned char* hash);
bool (*hash_equals)(unsigned char hash1[], unsigned char hash2[]);
void (*hash_init)(void *ctx);
void (*hash_update)(void *ctx, uchar data[], uint len);
void (*hash_final)(void *ctx, uchar hash[]);



#ifdef	__cplusplus
}
#endif

#endif	/* HASHING_ALGO_H */


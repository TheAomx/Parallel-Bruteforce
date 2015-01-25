/* 
 * File:   sha1_prop.h
 * Author: TBD
 *      Header file for sha1 implementation from splashtop streamer(R) source package.
 * Created on 13. Januar 2015, 15:17
 */

#ifndef __SHA1_PROP_H
#define __SHA1_PROP_H

#include "hash_types.h"

int sha1_equal_prop(uchar hash1[], uchar hash2[]);
void sha1_init_prop(void *context);
void sha1_update_prop(void *context, uchar data[], uint len);
void sha1_final_prop(void *context, uchar hash[]);

#endif

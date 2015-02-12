/* 
 * File:   attack_types.h
 * Author: aomx
 *
 * Created on 11. Februar 2015, 23:29
 */

#ifndef ATTACK_TYPES_H
#define	ATTACK_TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "bruteforce.h"

int checkPasswordObservedHashTable(void *ctx, char *password, hashFoundCallback ohHashFound);

int checkPasswordObservedHashTableWithSalt(void *ctx, char *password, hashFoundCallback ohHashFound);


#ifdef	__cplusplus
}
#endif

#endif	/* ATTACK_TYPES_H */


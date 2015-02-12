/* 
 * File:   attack_types.c
 * Author: TBD
 *
 * Created on 11. Februar 2015, 23:28
*/

#include <stdio.h>

#include "uthash.h"
#include "attack_types.h"
#include "pb_client.h"
#include "openmp.h"
#include "debug_macros.h"


int checkPasswordObserved(void *ctx, char *password, hashFoundCallback ohHashFound) {
    unsigned int i;
    ThreadContext *context = (ThreadContext*) ctx;
    AttackStrategy *strategy = context->attackStrategy;
    PasswordHashes *pwHashes = (PasswordHashes*) strategy->attackData;

    int threadID = getThreadID();
    uchar* hashBuffer = context->hashBuffer[threadID];
    
    HashAlgorithm *algo = pwHashes->algo[threadID];

    getHashFromString(algo, password, hashBuffer);

    for (i = 0; i < context->numHashes; i++) {
        uchar *checkedHash = getHash(context, pwHashes, threadID, i);
        if (algo->equals(hashBuffer, checkedHash)) {
            //            printf("The hash of %s was %s. Notifying server...\n", password, algo->toString(checkedHash));
            fflush(stdout);
            lockMutex();
            ohHashFound(password, algo->toString(checkedHash));
            releaseMutex();

        }
    }

    return 0;
}

int checkPasswordObservedHashTable(void *ctx, char *password, hashFoundCallback ohHashFound) {
    ThreadContext *context = (ThreadContext*) ctx;
    AttackStrategy *strategy = context->attackStrategy;
    PasswordHashes *pwHashes = (PasswordHashes*) strategy->attackData;

    int threadID = getThreadID();
    uchar* hashBuffer = context->hashBuffer[threadID];
    HashAlgorithm *algo = pwHashes->algo[threadID];
    HashTableEntry** hashTables = pwHashes->hashesHashTables;
    getHashFromString(algo, password, hashBuffer);

    HashTableEntry *s;

    HASH_FIND(handle, hashTables[threadID], hashBuffer, algo->hashSize, s);

    if (s != NULL) {
        lockMutex();
        ohHashFound(password, algo->toString(s->hash));
        releaseMutex();
    }
    return 0;
}

int checkPasswordObservedHashTableWithSalt(void *ctx, char *password, hashFoundCallback ohHashFound) {
    ThreadContext *context = (ThreadContext*) ctx;
    AttackStrategy *strategy = context->attackStrategy;
    PasswordHashesWithSalt *pwHashes = (PasswordHashesWithSalt*) strategy->attackData;
    
    int threadID = getThreadID();
    uchar* hashBuffer = context->hashBuffer[threadID];
    HashAlgorithm *algo = pwHashes->algo[threadID];
    
    HashTableEntry* hashTable = pwHashes->hashTable;
    
    HashTableEntry *s;
    
    unsigned long i;
    
    DBG_OK("in checkPasswordObservedHashTableWithSalt before loop");
    
    for (i = 3; i < 4; i++) {
        sds saltAndPassword = sdsnew(pwHashes->saltValues[i]);
        sdstolower(saltAndPassword);
        saltAndPassword = sdscat(saltAndPassword, password);
        getHashFromString(algo, saltAndPassword, hashBuffer);
        
        HASH_FIND(handle, hashTable, hashBuffer, algo->hashSize, s);

        if (s != NULL) {
            lockMutex();
            ohHashFound(password, algo->toString(hashBuffer));
            releaseMutex();
        }
        
        sdsfree(saltAndPassword);
    }
    
    return 0;
}
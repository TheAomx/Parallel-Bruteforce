#include "core_headers.h"
#include "uthash.h"
#include "attack_types.h"

void freeThreadContext(ThreadContext *context) {
    unsigned int i = 0;
    
    context->attackStrategy->free(context);
    
    for (i = 0; i < context->numThreads; i++) {
        free(context->hashBuffer[i]);
    }
    free(context->attackStrategy);
    free(context->hashBuffer);
    free(context);
#if 0
    unsigned int i = 0;

    for (i = 0; i < pwHashes->numThreads; i++) {
        HashTableEntry* root = pwHashes->hashesHashTables[i];
        HashTableEntry *iter, *tmp;

        HASH_ITER(handle, root, iter, tmp) {
            HASH_DELETE(handle, root, iter);
            free(iter);
        }
        free(pwHashes->hashBuffer[i]);
        freeHashAlgo(pwHashes->algo[i]);
    }
    free(pwHashes->hashes);

    free(pwHashes);
#endif
}

static void initHashTablesForThreads(ThreadContext *context, PasswordHashes* hashes, int numThreads) {
    hashes->hashesHashTables = (HashTableEntry**) malloc(sizeof (HashTableEntry*) * numThreads);
    HashTableEntry* checkEntry;
    for (int i = 0; i < numThreads; i++) {
        HashTableEntry* currentRoot = NULL;
        HashAlgorithm *algo = hashes->algo[i];
        uint hashSize = algo->hashSize;
        for (uint j = 0; j < context->numHashes; j++) {
            uchar *checkedHash = (uchar*) getHash(context, hashes, 0, j);

            HashTableEntry* newEntry = (HashTableEntry*) malloc(sizeof (HashTableEntry));
            newEntry->hash = (uchar*) malloc(sizeof (uchar) * hashSize);
            memcpy(newEntry->hash, checkedHash, sizeof (uchar) * hashSize);

            newEntry->id = (uchar*) malloc(sizeof (uchar) * hashSize);
            memcpy(newEntry->id, checkedHash, sizeof (uchar) * hashSize);

            
            HASH_FIND(handle, currentRoot, checkedHash, algo->hashSize, checkEntry);
            if (checkEntry != NULL)
                continue;

            HASH_ADD_KEYPTR(handle, currentRoot, newEntry->id, hashSize, newEntry);

        }
        hashes->hashesHashTables[i] = currentRoot;

    }

}

PasswordGenTask* createClientTask(int pwGenAlgoType, char* start, char* end) {
    PasswordGenTask* result = NULL;
    PasswordGenerationContext* genContext = createPasswordGenerationContextByType(pwGenAlgoType);
    result = (PasswordGenTask*) malloc(sizeof (PasswordGenTask));
    result->startPassword = start;
    result->endPassword = end;
    result->generationContext = genContext;
    return result;
}

PasswordGenTask* createClientTaskWithAlpha(char*alphabet, int pwGenAlgoType, char* start, char* end) {
    PasswordGenTask* result = NULL;
    PasswordGenerationContext* genContext = createContextWithAlphabet(alphabet,pwGenAlgoType);
    result = (PasswordGenTask*) malloc(sizeof (PasswordGenTask));
    result->startPassword = start;
    result->endPassword = end;
    result->generationContext = genContext;
    return result;
}

void printHashes(ThreadContext *context, int rank) {
    for (unsigned int i = 0; i < context->numHashes; i++) {
        //printf("[%d] %s\n", rank, pwHashes->algo[0]->toString(getHash(pwHashes, 0, i)));
    }
}

static char** readLinesOfFile(MPI_File *in, int *linesFound) {
    MPI_Offset filesize;
    char *fileBuffer;

    MPI_File_get_size(*in, &filesize);

    fileBuffer = (char*) malloc((filesize + 1) * sizeof (char));
    MPI_File_read(*in, fileBuffer, filesize, MPI_CHAR, MPI_STATUS_IGNORE);
    fileBuffer[filesize] = '\0';

    char *seperator = "\n";

    sds *splittedString = sdssplitlen(fileBuffer, strlen(fileBuffer), seperator, strlen(seperator), linesFound);
    free(fileBuffer);
    return splittedString;
}

uchar* getHash(ThreadContext *context, PasswordHashes *pwHashes, int threadID, unsigned int hashID) {
    uchar *beginPtr = pwHashes->hashes[threadID];

    if (hashID >= context->numHashes) {
        DBG_ERR("hashID >= pwHashes->numHashes");
        return NULL;
    }

    return beginPtr + hashID * pwHashes->algo[0]->hashSize;
}

void freePasswordAlgo (ThreadContext *threadContext) {
    unsigned int i;
    PasswordHashes *pwHashes = (PasswordHashes*) threadContext->attackStrategy->attackData;
    
    for (i = 0; i < threadContext->numThreads; i++) {
        HashTableEntry* root = pwHashes->hashesHashTables[i];
        HashTableEntry *iter, *tmp;

        HASH_ITER(handle, root, iter, tmp) {
            HASH_DELETE(handle, root, iter);
            free(iter);
        }
        freeHashAlgo(pwHashes->algo[i]);
    }
    
    free(pwHashes->hashes);
    free(pwHashes);
}

static void* generatePasswordHashes(ThreadContext *threadContext, char **lines, unsigned int numLines) {
   
    unsigned long hashesFound = 0;
    unsigned int i;

    unsigned int numThreads = threadContext->numThreads;

    for (i = 1; i < numLines; i++) {
        sdstrim(lines[i], " \t");
        if (strcmp("", lines[i]) != 0) {
            hashesFound++;
        }
    }

    if (!isSupportedHashAlgorithm(lines[0])) {
        DBG_ERR("the hash algorithm %s is not supported!", lines[0]);
        return NULL;
    }

    if (hashesFound == 0) {
        DBG_ERR("hashesFound == 0");
        return NULL;
    }

    PasswordHashes *pwHashes = (PasswordHashes*) malloc(sizeof (PasswordHashes));
    threadContext->numHashes = hashesFound;

    pwHashes->algo = (HashAlgorithm**) malloc(sizeof (HashAlgorithm*) * numThreads);
    for (i = 0; i < numThreads; i++) {
        pwHashes->algo[i] = createHashAlgorithm(lines[0]);
    }
        
    unsigned int hashSize = pwHashes->algo[0]->hashSize;

    pwHashes->hashes = (uchar**) malloc(sizeof(uchar*) * numThreads);
    size_t hashArraySize = sizeof(uchar) * hashSize * hashesFound;
    
    for (i = 0; i < numThreads; i++) {
        pwHashes->hashes[i] = (uchar*) malloc(hashArraySize);
    }

    threadContext->hashBuffer = (uchar**) malloc(sizeof (uchar*) * numThreads);

    for (i = 0; i < numThreads; i++) {
        threadContext->hashBuffer[i] = (uchar*) malloc(sizeof (uchar) * pwHashes->algo[0]->hashSize);
    }

    int j = 0;

    for (i = 1; i < numLines; i++) {
        if (strcmp("", lines[i]) != 0) {
            uchar *binaryHash = getHash(threadContext, pwHashes, 0, j);
            convertHashStringToBinary(pwHashes->algo[0], lines[i], binaryHash);
            j++;
        }
    }
    
    initHashTablesForThreads(threadContext, pwHashes, numThreads);

    return pwHashes;
}


AttackStrategy* createNormalHashingAttack() {
    AttackStrategy *attack = (AttackStrategy*) malloc(sizeof(AttackStrategy));
    attack->hashFileParser = generatePasswordHashes;
    attack->bruteforceMethod = checkPasswordObservedHashTable;
    attack->free = freePasswordAlgo;
    
    return attack;
    
}

ThreadContext* createThreadContext(MPI_File *in, unsigned int numThreads) {
    unsigned int linesFound = 0;
    sds *lines = readLinesOfFile(in, &linesFound);
    
    if (linesFound == 0) {
        DBG_ERR("linesFound == 0");
        return NULL;
    }
    
    if (numThreads == 0) {
        DBG_ERR("numThreads == 0");
        return NULL;
    }
     
    ThreadContext *context = (ThreadContext*) malloc(sizeof(ThreadContext));
    context->numThreads = numThreads;
        
    context->attackStrategy = createNormalHashingAttack();
    
    // parse the actual content of the file with the hashFileParser of the attack
    context->attackStrategy->attackData = context->attackStrategy->hashFileParser(context, lines, linesFound);
    
    sdsfreesplitres(lines, linesFound);
    return context;
}
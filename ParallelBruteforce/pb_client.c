#include "core_headers.h"

void freePasswordHashes(PasswordHashes *pwHashes) {
    int i = 0;

	free(pwHashes->hashes);

    for (i = 0; i < pwHashes->numThreads; i++) {
        free(pwHashes->hashBuffer[i]);
        freeHashAlgo(pwHashes->algo[i]);
    }

    free(pwHashes);
}

PasswordGenTask* createClientTask(int pwGenAlgoType, char* start, char* end) {
    PasswordGenTask* result = NULL;
    PasswordGenerationContext* genContext = createPasswordGenerationContextByType(pwGenAlgoType);
    result=(PasswordGenTask*)malloc(sizeof(PasswordGenTask));
    result->startPassword=start;
    result->endPassword=end;
    result->generationContext=genContext;
    return result;
};

void printHashes(PasswordHashes *pwHashes, int rank) {
    for (int i = 0; i < pwHashes->numHashes; i++) {
        printf("[%d] %s\n", rank, pwHashes->algo[0]->toString(getHash(pwHashes,0,i)));
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

uchar* getHash(PasswordHashes *pwHashes, int threadID, int hashID){
	uchar *beginPtr = pwHashes->hashes[threadID];
        
        if (hashID >= pwHashes->numHashes) {
            DBG_ERR("hashID >= pwHashes->numHashes");
            return NULL;
        }

	return beginPtr + hashID * pwHashes->algo[0]->hashSize;
}

PasswordHashes* generatePasswordHashes(MPI_File *in, unsigned int numThreads) {
    int linesFound = 0;
    sds *lines = readLinesOfFile(in, &linesFound);
    unsigned long hashesFound = 0;
    int i;

    if (linesFound == 0) {
        DBG_ERR("linesFound == 0");
        return NULL;
    }

    if (numThreads == 0) {
        DBG_ERR("numThreads == 0");
        return NULL;
    }

    for (i = 1; i < linesFound; i++) {
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
    pwHashes->numHashes = hashesFound;

    pwHashes->numThreads = numThreads;

    pwHashes->algo = (HashAlgorithm**) malloc(sizeof (HashAlgorithm*) * numThreads);
    for (i = 0; i < numThreads; i++) {
        pwHashes->algo[i] = createHashAlgorithm(lines[0]);
    }

    pwHashes->hashes = (uchar**) malloc(sizeof (uchar*) * numThreads);

    size_t hashArraySize = sizeof (uchar) * pwHashes->algo[0]->hashSize * hashesFound;
	
    for (i = 0; i < numThreads; i++) {
        pwHashes->hashes[i] = (uchar*) malloc(hashArraySize);
    }
    
    pwHashes->hashBuffer = (uchar**) malloc(sizeof (uchar*) * numThreads);
	
    for (i = 0; i < numThreads; i++) {
        pwHashes->hashBuffer[i] = (uchar*) malloc(sizeof (uchar) * pwHashes->algo[0]->hashSize);
    }
	
    int j = 0;

    for (i = 1; i < linesFound; i++) {
        if (strcmp("", lines[i]) != 0) {
            uchar *binaryHash = getHash(pwHashes, 0, j);
            convertHashStringToBinary(pwHashes->algo[0], lines[i], binaryHash);
            j++;
        }
    }
    
    for (i = 1; i < numThreads; i++) {
        memcpy(pwHashes->hashes[i], pwHashes->hashes[0], hashArraySize);
    }

    sdsfreesplitres(lines, linesFound);

    return pwHashes;
}

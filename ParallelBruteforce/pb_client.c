#include "core_headers.h"

void freePasswordHashes(PasswordHashes *pwHashes) {
    int i = 0;
    
    for (i = 0; i < pwHashes->numHashes; i++) {
        free(pwHashes->hashes[i]);
    }
    
    free(pwHashes->hashBuffer);
    
    freeHashAlgo(pwHashes->algo);
    free(pwHashes);
}

void printHashes(PasswordHashes *pwHashes, int rank) {
    for (int i=0; i<pwHashes->numHashes; i++) {
        printf("[%d] %s\n",rank, pwHashes->algo->toString(pwHashes->hashes[i]));
    }
}

static char** readLinesOfFile(MPI_File *in, int *linesFound) {
    MPI_Offset filesize;
    char *fileBuffer;
    
    MPI_File_get_size(*in, &filesize);
    
    fileBuffer = (char*)malloc( (filesize + 1)*sizeof(char));
    MPI_File_read(*in, fileBuffer, filesize, MPI_CHAR, MPI_STATUS_IGNORE);
    fileBuffer[filesize] = '\0';
    
    char *seperator = "\n";
	
    sds *splittedString = sdssplitlen(fileBuffer, strlen(fileBuffer), seperator , strlen(seperator), linesFound);
    free(fileBuffer);
    return splittedString;
}

PasswordHashes* generatePasswordHashes(MPI_File *in) {
    int linesFound = 0;
    sds *lines = readLinesOfFile(in, &linesFound);
    unsigned long hashesFound = 0;
    int i;
    
    if (linesFound == 0) {
        DBG_ERR("linesFound == 0");
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
    
    PasswordHashes *pwHashes = (PasswordHashes*) malloc(sizeof(PasswordHashes));
    pwHashes->numHashes = hashesFound;
    pwHashes->hashes = (uchar**) malloc(sizeof(uchar*) * hashesFound);
    pwHashes->algo = createHashAlgorithm(lines[0]);
    
    int j = 0;
    
    for (i = 1; i < linesFound; i++) {
        if (strcmp("", lines[i]) != 0) {
            uchar *binaryHash = convertHashStringToBinary(pwHashes->algo, lines[i]);
            pwHashes->hashes[j] = binaryHash;
            j++;
        }
    }
    
    sdsfreesplitres(lines, linesFound);
    
    return pwHashes;
}

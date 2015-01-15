/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"

#if 1

char** readLinesOfFile(MPI_File *in, int *linesFound) {
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

static inline int isSupportedHashAlgorithm (sds line) {
    return (strcmp(line,"SHA1") == 0 || strcmp(line,"SHA256") == 0);
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
    pwHashes->hashAlgorithm = lines[0];
    pwHashes->numHashes = hashesFound;
    pwHashes->hashes = (uchar**) malloc(sizeof(uchar*) * hashesFound);
    
    int j = 0;
    
    for (i = 1; i < linesFound; i++) {
        if (strcmp("", lines[i]) != 0) {
            pwHashes->hashes[j] = lines[i];
            j++;
        }
        else {
            sdsfree(lines[i]);
        }
    }
    
    return pwHashes;
}

void printHashes(PasswordHashes *pwHashes, int rank) {
    for (int i=0; i<pwHashes->numHashes; i++) {
        printf("[%d] %s\n",rank, pwHashes->hashes[i]);
    }
}

uchar hexCharToBin(char c) {
    if (c >= '0' && c <='9') {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    else {
        return 0;
    }
}

uchar hexToBin(char c1, char c2) {
    uchar temp = 0;
    temp = hexCharToBin(c2);
    temp |= hexCharToBin(c1) << 4;
    return temp;
}

void convertHash(sds hashString, uchar *hashBinary) {
    uchar *hashBin = hashBinary;
    int i, j;
    uchar temp;
    for (i = 0, j = 0; i < SHA256_SIZE; i++, j+=2) {
        hashBin[i] = hexToBin(hashString[j], hashString[j+1]) ;
    }
}

int main(int argc, char** argv) {
    int maxNum;
    int nTasks, rank;
    MPI_Init( NULL, NULL );
    MPI_Comm_size( MPI_COMM_WORLD, &nTasks );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_File fh;
    int ret = MPI_File_open(MPI_COMM_WORLD, "hashes.txt", MPI_MODE_RDONLY, MPI_INFO_NULL,&fh);
    if(ret < 0) {
        printf("MPI_File_open failed");
        exit(1);
    }
    if (rank == 0)
	{
		// The master thread will need to receive all computations from all other threads.
		MPI_Status status;

		// MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
		// We need to go and receive the data from all other threads.
		// The arbitrary tag we choose is 1, for now.
                
                
                
                //MPI_Bcast(&fh,1,, 0,MPI_COMM_WORLD);
                
	}
	else
	{                
            int i = 0;

            PasswordHashes* pwHashes = generatePasswordHashes(&fh);
            printHashes(pwHashes, rank);

            HashAlgorithm* hashAlgo = createHashAlgorithm(pwHashes->hashAlgorithm);
            uchar hashBuffer[SHA256_SIZE];
            uchar convertedHash[SHA256_SIZE];
            getHashFromString(hashAlgo, "Julian", hashBuffer);
            
//            printf("searching for hash of ");
//            hashAlgo->print(hashBuffer);
//            printf("\n");
            printf("before check!\n");
            for (i = 0; i < pwHashes->numHashes; i++) {
                convertHash(pwHashes->hashes[i], convertedHash);
//                hashAlgo->print(convertedHash);
//                printf("\n");
                if (hashAlgo->equals(hashBuffer, convertedHash)) {
                    printf("the hash of Julian was ");
                    hashAlgo->print(convertedHash);
                }
            }
	}

	MPI_Finalize();
}
#endif

#if 0
int main(int argc, char** argv) {
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA256");
    uchar hash[SHA256_SIZE];
    
    /*oder auch:
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA1");
    uchar hash[SHA1_SIZE];*/
    
   //getHashFromFile(hashAlgo, "main.c", &hash);
    getHashFromString(hashAlgo,"Robin", hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Julian", hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Nils", hash);
    hashAlgo->print(hash);
    printf("\n");
  
    freeHash(hashAlgo);
    
    return 0;
}
#endif

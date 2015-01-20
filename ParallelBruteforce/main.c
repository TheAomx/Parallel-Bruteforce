/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>


#include "core_headers.h"

#if 1

int checkPassword (void *ctx, char *password) {
    int i;
    PasswordHashes *pwHashes = (PasswordHashes*) ctx;
	
    int threadID = omp_get_thread_num();
    uchar* hashBuffer = pwHashes->hashBuffer[threadID];
    
    getHashFromString(pwHashes->algo, password, hashBuffer);

    for (i = 0; i < pwHashes->numHashes; i++) {
        if (pwHashes->algo->equals(hashBuffer, pwHashes->hashes[i])) {
            printf("The hash of %s was %s \n", password, pwHashes->algo->toString(pwHashes->hashes[i]));
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    int maxNum;
    int nTasks, rank, alphaPos = 0;
    if( MPI_Init(&argc, &argv) != MPI_SUCCESS ){
        printf("MPI_init failed\n");
	exit(-1);
    }
    MPI_Comm_size( MPI_COMM_WORLD, &nTasks );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_File fh;
    DBG_OK("before MPI_File_open");
    int ret = MPI_File_open(MPI_COMM_WORLD, "hashes.txt", MPI_MODE_RDONLY, MPI_INFO_NULL,&fh);
    if(ret < 0) {
        printf("MPI_File_open failed");
        exit(1);
    }
    PasswordHashes* pwHashes = generatePasswordHashes(&fh);
    if (rank == 0)
	{
		// The master thread will need to receive all computations from all other threads.
		MPI_Status status;

		// MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
		// We need to go and receive the data from all other threads.
		// The arbitrary tag we choose is 1, for now.
                
                printHashes(pwHashes, rank);
                
                //MPI_Bcast(&fh,1,, 0,MPI_COMM_WORLD);
                
	}
	else
	{                
            int i = 0;
            char alphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"};
            unsigned int passwordSearchLength = 6;              
            
            int numThreads = omp_get_num_procs();
	   
	    printf("child process with %d threads!\n", numThreads);
	
	    pwHashes->hashBuffer = (uchar**) malloc(sizeof(uchar*) * numThreads);
	    for (i = 0; i < numThreads; i++) {
		pwHashes->hashBuffer[i] = (uchar*) malloc(sizeof(uchar) * pwHashes->algo->hashSize);
	    }

	    char **passphraseBuffer = (char**) malloc(sizeof(char*) * numThreads);
	    for (i = 0; i < numThreads; i++) {
		passphraseBuffer[i] = (char*) malloc(sizeof(char) * passwordSearchLength);
		memset(passphraseBuffer[i], 0, sizeof(passphraseBuffer[i]));
	    }
	
	    

            bruteforcePasswordAll(pwHashes, checkPassword, alphabet, passphraseBuffer, passwordSearchLength, rank, nTasks);
	}
	freePasswordHashes(pwHashes);

	MPI_Finalize();
}
#endif

#if 0
int main(int argc, char** argv) {
    HashAlgorithm *hashAlgo = createHashAlgorithm("MD5");
    uchar hash[hashAlgo->hashSize];
    
    /*oder auch:
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA1");
    uchar hash[SHA1_SIZE];*/
    
    getHashFromFile(hashAlgo, "main.c", hash);
    printf("%s\n", hashAlgo->toString(hash));
 /*
    getHashFromString(hashAlgo,"Robin", hash);
    printf("%s\n", hashAlgo->toString(hash));
    
    getHashFromString(hashAlgo,"Julian", hash);
    printf("%s\n", hashAlgo->toString(hash));
    
    getHashFromString(hashAlgo,"Nils", hash);
    printf("%s\n", hashAlgo->toString(hash));*/
  
    freeHashAlgo(hashAlgo);
    
    return 0;
}
#endif

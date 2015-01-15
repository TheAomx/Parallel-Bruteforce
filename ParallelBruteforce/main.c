/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"

#if 1

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

            uchar hashBuffer[pwHashes->algo->hashSize];
            getHashFromString(pwHashes->algo, "Julian", hashBuffer);
            
            for (i = 0; i < pwHashes->numHashes; i++) {
                if (pwHashes->algo->equals(hashBuffer, pwHashes->hashes[i])) {
                    printf("the hash of Julian was %s \n", pwHashes->algo->toString(pwHashes->hashes[i]));
                }
            }
            
            freePasswordHashes(pwHashes);
	}

	MPI_Finalize();
}
#endif

#if 0
int main(int argc, char** argv) {
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA1");
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
  
    freeHash(hashAlgo);
    
    return 0;
}
#endif

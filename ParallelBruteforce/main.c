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

void processlines(char **lines, const int nlines, const int rank) {
    for (int i=0; i<nlines; i++) {
        printf("[%d] %s\n",rank, lines[i]);
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
                printf("huhu\n");
                const int overlap=100;
                char **lines;
                int nlines;
                lines = readLinesOfFile(&fh, &nlines);
                printf("huhu2\n");
                processlines(lines, nlines, rank);
                printf("huhu3\n");
                
                sdsfreesplitres(lines, nlines);
		//int MPI_File_read(fh, void *buf,int count, MPI_CHAR, MPI_Status *status);
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
    getHashFromString(hashAlgo,"Robin", &hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Julian", &hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Nils", &hash);
    hashAlgo->print(hash);
    printf("\n");
  
    freeHash(hashAlgo);
    
    return 0;
}
#endif

/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"

#if 1

int checkPassword(void *ctx, char *password) {
    int i;
    static unsigned long pwCounter = 0;
    PasswordHashes *pwHashes = (PasswordHashes*) ctx;

    int threadID = getThreadID();
    uchar* hashBuffer = pwHashes->hashBuffer[threadID];
    HashAlgorithm *algo = pwHashes->algo[threadID];

    pwCounter++;

    if ((pwCounter % 1000000) == 0) {
        printf("[%d] %s\n", threadID, password);
        fflush(stdout);
    }

    getHashFromString(algo, password, hashBuffer);

    for (i = 0; i < pwHashes->numHashes; i++) {
        if (algo->equals(hashBuffer, pwHashes->hashes[i])) {
            printf("The hash of %s was %s \n", password, algo->toString(pwHashes->hashes[i]));
            fflush(stdout);
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    int maxNum;
    int nTasks, rank, alphaPos = 0;
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        printf("MPI_init failed\n");
        exit(EXIT_FAILURE);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_File fh;
    int ret = MPI_File_open(MPI_COMM_WORLD, "hashes.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (ret < 0) {
        printf("MPI_File_open failed");
        exit(EXIT_FAILURE);
    }
    PasswordHashes *pwHashes;
    if (rank == 0) {
        // The master thread will need to receive all computations from all other threads.
        MPI_Status status;



        /*
         * Calculate and initialize server side information about the job. 
         * The created context also holds the information about the work, each 
         * client will do.
         */
        ServerContext* context = initializeWithPW("hashes.txt", nTasks - 1, "a", "000000");

        printServerContext(context);



        /* Send the hash file name to the clients first. Tag parameter -> 0*/
        DBG_OK("Sending hash file name: %s to all clients", context->hashesFileName);
        int len = strlen(context->hashesFileName);
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(context->hashesFileName, len, MPI_CHAR, 0, MPI_COMM_WORLD);

        int passwordLengthValues[nTasks-1];
        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending startPass");
        /* Send the start password. Tag parameter -> 1*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i-1];

            passwordLengthValues[i - 1] = strlen(currentTask.startPass);
            
            MPI_Send(&passwordLengthValues[i - 1], 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            MPI_Send(currentTask.startPass, passwordLengthValues[i - 1], MPI_CHAR, i, 2, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending endPass");
        /* Send the end password. Tag parameter -> 2*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i-1];
            passwordLengthValues[i - 1] = strlen(currentTask.endPass);
            MPI_Send(&passwordLengthValues[i - 1], 1, MPI_INT, i, 3, MPI_COMM_WORLD);

            MPI_Send(currentTask.endPass, passwordLengthValues[i - 1], MPI_CHAR, i, 4, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending algo id");
        /* Send the password generation algorithm identifier. Tag parameter -> 3*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i-1];
            MPI_Send(&(currentTask.pwAlgoType), 1, MPI_INT, i, 5, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);


        // MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
        // We need to go and receive the data from all other threads.
        // The arbitrary tag we choose is 1, for now.
        pwHashes = generatePasswordHashes(&fh, 1);
        printHashes(pwHashes, rank);
        DBG_OK("Sent all data to clients ... now going to infinite loop (tbd)");

        //MPI_Bcast(&fh,1,, 0,MPI_COMM_WORLD);
       
         for (int i = nTasks-1; i <= 0; i) {
             free((context->tasks+i));
            
        }
        free(context);

    } else {
        MPI_Status status;

        int startPwLen, endPwLen, hashFileNameLen, pwAlgoValue;

        char* hashFileName = (char*) malloc(sizeof (char)*MAX_FILENAME_LEN);
        char* endPass = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        char* startPass = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(hashFileName, '\0', sizeof (char)*MAX_FILENAME_LEN);
        memset(endPass, '\0', sizeof (char)*MAX_PASSWORD);
        memset(startPass, '\0', sizeof (char)*MAX_PASSWORD);

    

        MPI_Bcast(&hashFileNameLen, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(hashFileName, hashFileNameLen, MPI_CHAR, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&startPwLen, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(startPass, startPwLen, MPI_CHAR, 0, 2, MPI_COMM_WORLD, &status);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&endPwLen, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
        MPI_Recv(endPass, endPwLen, MPI_CHAR, 0, 4, MPI_COMM_WORLD, &status);

        MPI_Barrier(MPI_COMM_WORLD);

        MPI_Recv(&pwAlgoValue, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);

        MPI_Barrier(MPI_COMM_WORLD);
        
        
        
        DBG_OK("Received data from server. Hash filename: %s\n          start: %s, end: %s, pwAlgoType:%d", hashFileName, startPass, endPass, pwAlgoValue);
        
        PasswordGenTask* clientTaskInfo = createClientTask(pwAlgoValue,startPass,endPass);
        
        
        int i = 0;
        int numThreads = getNumCores();
        setNumThreads(numThreads);
        pwHashes = generatePasswordHashes(&fh, numThreads);
        printf("child process with %d threads!\n", numThreads);

        char **passphraseBuffer = (char**) malloc(sizeof (char*) * numThreads);
        for (i = 0; i < numThreads; i++) {
            passphraseBuffer[i] = (char*) malloc(sizeof (char) * (MAX_PASSWORD + 1));
            memset(passphraseBuffer[i], 0, sizeof (passphraseBuffer[i]));
        }

        struct timeval timeBefore, timeAfter;
        gettimeofday(&timeBefore, NULL);

        bruteforcePasswordTask(clientTaskInfo, pwHashes, checkPassword,passphraseBuffer);
        gettimeofday(&timeAfter, NULL);
        printf("needed %ld secs %ld usecs\n", (ulong) (timeAfter.tv_sec - timeBefore.tv_sec), (ulong) (timeAfter.tv_usec - timeBefore.tv_usec));
}
    

    MPI_Finalize();
    return 0;
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

#if 0
//        PasswordGenerationContext* pwGenType = createPasswordGenerationContextByType(context->type);
//        int len = strlen(context->tasks->startPass);
//        char* tmpPwd = (char*)malloc(sizeof(char)*MAX_PASSWORD);
//        
//        memset(tmpPwd,'\0',sizeof(char)*MAX_PASSWORD);
//        strncpy(tmpPwd,context->tasks->startPass,len);
//        char* tmpPwd2 = (char*)malloc(sizeof(char)*MAX_PASSWORD);
//        memset(tmpPwd2,'\0',sizeof(char)*MAX_PASSWORD);
//        DBG_OK("Counting the first 10000000 pws first client task.");
//        
//        for(ulong i=0; i<=10000000;i++){
//            pwGenType->nextPassword(tmpPwd,tmpPwd2);
//            //DBG_OK("           %s(%ld)      ->      %s(%ld)    :%ld   ", tmpPwd,(pwGenType->valueOf(tmpPwd)),tmpPwd2,(pwGenType->valueOf(tmpPwd2)),i);
//            strncpy(tmpPwd,tmpPwd2,MAX_PASSWORD);
//        }
//        DBG_OK("Counting the first 10000000 pws first client task. finish ");
#endif

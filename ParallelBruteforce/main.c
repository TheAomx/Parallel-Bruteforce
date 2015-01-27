/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"

#if 1


#ifdef _OPENMP
omp_lock_t* lock = NULL;

void initLock() {
    lock = (omp_lock_t*) malloc(sizeof (omp_lock_t));
    omp_init_lock(lock);
}

void clearLock() {
    omp_destroy_lock(lock);
    free(lock);
}


#endif

int checkPassword(void *ctx, char *password) {
    unsigned int i;
    PasswordHashes *pwHashes = (PasswordHashes*) ctx;

    int threadID = getThreadID();
    uchar* hashBuffer = pwHashes->hashBuffer[threadID];
    HashAlgorithm *algo = pwHashes->algo[threadID];

    getHashFromString(algo, password, hashBuffer);

    for (i = 0; i < pwHashes->numHashes; i++) {
        uchar *checkedHash = getHash(pwHashes, threadID, i);
        if (algo->equals(hashBuffer, checkedHash)) {
            printf("The hash of %s was %s \n", password, algo->toString(checkedHash));
            fflush(stdout);
        }
    }

    return 0;
}

int checkPasswordObserved(void *ctx, char *password, hashFoundCallback ohHashFound) {
    unsigned int i;
    PasswordHashes *pwHashes = (PasswordHashes*) ctx;

    int threadID = getThreadID();
    uchar* hashBuffer = pwHashes->hashBuffer[threadID];
    HashAlgorithm *algo = pwHashes->algo[threadID];

    getHashFromString(algo, password, hashBuffer);
    
    

    for (i = 0; i < pwHashes->numHashes; i++) {
        uchar *checkedHash = getHash(pwHashes, threadID, i);
        if (algo->equals(hashBuffer, checkedHash)) {
            //            printf("The hash of %s was %s. Notifying server...\n", password, algo->toString(checkedHash));
            fflush(stdout);
#ifdef _OPENMP
            omp_set_lock(lock);
#endif
            ohHashFound(password, algo->toString(checkedHash));
#ifdef _OPENMP
            omp_unset_lock(lock);
#endif

        }
    }

    return 0;
}

void sendFoundPasswordAndHashToRoot(char* password, char* hash) {
    int lenPw = strlen(password);
    int lenHash = strlen(hash);

    MPI_Send(&lenPw, 1, MPI_INT, 0, 4711, MPI_COMM_WORLD);
    MPI_Send(password, lenPw, MPI_CHAR, 0, 4712, MPI_COMM_WORLD);
    MPI_Send(&lenHash, 1, MPI_INT, 0, 4713, MPI_COMM_WORLD);
    MPI_Send(hash, lenHash, MPI_CHAR, 0, 4714, MPI_COMM_WORLD);

}

int main(int argc, char** argv) {

    int nTasks, rank;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        printf("MPI_init failed\n");
        exit(EXIT_FAILURE);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nTasks);

    if (nTasks <= 1) {
        DBG_WARN("The MPI-Communicator contains only one (or less) tasks.");
        DBG_WARN("Disturbed brute force attack cannot be executed with the defined number (%d) of tasks.", nTasks);
        DBG_WARN("Consider setting up more MPI-Processors.");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_File fh;
    char *hashFile = (argc >= 2) ? argv[1] : "hashes.txt";
    int ret = MPI_File_open(MPI_COMM_WORLD, hashFile, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (ret < 0) {
        printf("MPI_File_open failed");
        exit(EXIT_FAILURE);
    }
    PasswordHashes *pwHashes;
    if (rank == 0) {

        /*
         * Calculate and initialize server side information about the job. 
         * The created context also holds the information about the work, each 
         * client will do.
         */
        ServerContext* context = initializeWithPW(hashFile, nTasks - 1, "a", "000000");
        DBG_OK("Test");
        printServerContext(context);



        /* Send the hash file name to the clients first. Tag parameter -> 0*/
        DBG_OK("Sending hash file name: %s to all clients", context->hashesFileName);
        int len = strlen(context->hashesFileName);
        MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(context->hashesFileName, len, MPI_CHAR, 0, MPI_COMM_WORLD);

        int passwordLengthValues[nTasks - 1];
        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending startPass");
        /* Send the start password. Tag parameter -> 1*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i - 1];

            passwordLengthValues[i - 1] = strlen(currentTask.startPass);

            MPI_Send(&passwordLengthValues[i - 1], 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            MPI_Send(currentTask.startPass, passwordLengthValues[i - 1], MPI_CHAR, i, 2, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending endPass");
        /* Send the end password. Tag parameter -> 2*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i - 1];
            passwordLengthValues[i - 1] = strlen(currentTask.endPass);
            MPI_Send(&passwordLengthValues[i - 1], 1, MPI_INT, i, 3, MPI_COMM_WORLD);

            MPI_Send(currentTask.endPass, passwordLengthValues[i - 1], MPI_CHAR, i, 4, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);
        DBG_OK("Sending algo id");
        /* Send the password generation algorithm identifier. Tag parameter -> 3*/
        for (int i = 1; i < nTasks; i++) {
            ClientTask currentTask = context->tasks[i - 1];
            MPI_Send(&(currentTask.pwAlgoType), 1, MPI_INT, i, 5, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);


        pwHashes = generatePasswordHashes(&fh, 1);
        //printHashes(pwHashes, rank);
        DBG_OK("Sent all data to clients ... now going to receive cracked hashes");

        //MPI_Bcast(&fh,1,, 0,MPI_COMM_WORLD);

        for (int i = nTasks - 1; i <= 0; i) {
            free((context->tasks + i));

        }
        ulong hashesToReceive = pwHashes->numHashes;

        for (ulong hashesFound = 0; hashesFound < hashesToReceive; hashesFound++) {
            MPI_Status status;
            int pwLen;
            int hashLen;
            char* pwBuffer;
            char* hashBuffer;
            printf("Waiting for cracked passwords!\n");

            MPI_Recv(&pwLen, 1, MPI_INT, MPI_ANY_SOURCE, 4711, MPI_COMM_WORLD, &status);
            pwBuffer = (char*) malloc(sizeof (char)*pwLen);
            MPI_Recv(pwBuffer, pwLen, MPI_CHAR, status.MPI_SOURCE, 4712, MPI_COMM_WORLD, &status);


            MPI_Recv(&hashLen, 1, MPI_INT, status.MPI_SOURCE, 4713, MPI_COMM_WORLD, &status);
            hashBuffer = (char*) malloc(sizeof (char)*hashLen);
            MPI_Recv(hashBuffer, hashLen, MPI_CHAR, status.MPI_SOURCE, 4714, MPI_COMM_WORLD, &status);

            printf("Password found: %s -> %s\n", pwBuffer, hashBuffer);
            free(pwBuffer);
            free(hashBuffer);


        }
        free(context);
        /*If we reach here, we have found any password, so we can terminate MPI execution environment.*/
        MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);

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

        PasswordGenTask* clientTaskInfo = createClientTask(pwAlgoValue, startPass, endPass);


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
        //printHashes(pwHashes, rank);
#ifdef _OPENMP
        initLock();
#endif

        bruteforcePasswordTaskObserved(clientTaskInfo, pwHashes, checkPasswordObserved, sendFoundPasswordAndHashToRoot, passphraseBuffer);
#ifdef _OPENMP
        clearLock();
#endif
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

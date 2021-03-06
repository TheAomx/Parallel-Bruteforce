/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"
#include "debug_macros.h"

#if 1



const char* CLIENT_FINISH_MESSAGE = "MPI_BRUTEFORCE_CLIENT_FINISH";
static int rank;
#define FOUND_PASSWORD_FILE_NAME "stats.txt"

void sendFoundPasswordAndHashToRoot(char* password, char* hash) {
    int lenPw = strlen(password);
    int lenHash = strlen(hash);

    MPI_Send(&lenPw, 1, MPI_INT, 0, 4711, MPI_COMM_WORLD);
    MPI_Send(password, lenPw, MPI_CHAR, 0, 4712, MPI_COMM_WORLD);
    MPI_Send(&lenHash, 1, MPI_INT, 0, 4713, MPI_COMM_WORLD);
    MPI_Send(hash, lenHash, MPI_CHAR, 0, 4714, MPI_COMM_WORLD);
}

void sendClientFinishMessageToRoot() {
    int messageLen = strlen(CLIENT_FINISH_MESSAGE);
    
    MPI_Send(&messageLen, 1, MPI_INT, 0, 4711, MPI_COMM_WORLD);
    MPI_Send((void*) CLIENT_FINISH_MESSAGE, messageLen, MPI_CHAR, 0, 4712, MPI_COMM_WORLD);
}

void printFoundPwToFile(char* password, char* hash) {
    sds textLine = sdsnew(password);

    textLine = sdscat(textLine, " -> ");
    textLine = sdscat(textLine, hash);
    appendToFile(FOUND_PASSWORD_FILE_NAME, textLine);

    sdsfree(textLine);
}

sds getTime() {
    time_t current_time;
    struct tm * time_info;
    sds timeString = sdsnew(" ");

    time(&current_time);
    time_info = localtime(&current_time);
    timeString = sdscatprintf(timeString, "%d.%d, %d:%d:%d", time_info->tm_mday, time_info->tm_mon + 1, time_info->tm_hour, time_info->tm_min, time_info->tm_sec);

    return timeString;
}

void writeTimeToFile(char* prefix) {
    sds result = sdsnew(prefix);
    sds timeAsString = getTime();
    result = sdscat(result, timeAsString);
    appendToFile(FOUND_PASSWORD_FILE_NAME, result);
    sdsfree(result);
    sdsfree(timeAsString);
}

void printEndTimeToFile() {
    writeTimeToFile("Bruteforce end: ");
}

int hasRunningClients(char runningClients[], int numClients) {
    char* testArray = (char*) malloc(sizeof (char)*numClients);
    memset(testArray, '0', sizeof (char)*numClients);

    int result = memcmp(runningClients, testArray, sizeof (char)*numClients);

    free(testArray);
    return result;
}

void printStartTimeToFile() {
    writeTimeToFile("Bruteforce start: ");
}

void exitWrongTaskCount(int nTasks) {
    DBG_WARN("The MPI-Communicator contains only one (or less) tasks.");
    DBG_WARN("Disturbed brute force attack cannot be executed with the defined number (%d) of tasks.", nTasks);
    DBG_WARN("Consider setting up more MPI-Processors.");
    MPI_Finalize();
    exit(EXIT_FAILURE);
}

void finishExecution() {
    MPI_Abort(MPI_COMM_WORLD, MPI_SUCCESS);
    exit(EXIT_SUCCESS);
}



int main(int argc, char** argv) {

    int nTasks;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS) {
        printf("MPI_init failed\n");
        exit(EXIT_FAILURE);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &nTasks);

    if (nTasks <= 1) {
        exitWrongTaskCount(nTasks);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_File fh;
    char *hashFile = (argc >= 2) ? argv[1] : "hashes.txt";
    int ret = MPI_File_open(MPI_COMM_WORLD, hashFile, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (ret < 0) {
        printf("MPI_File_open failed");
        exit(EXIT_FAILURE);
    }
    ThreadContext *threadContext;
    if (rank == 0) {
        char alphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_.+=&!$%?-*#|"};
        /*
         * Calculate and initialize server side information about the job. 
         * The created context also holds the information about the work, each 
         * client will do.
         */
        ServerContext* context = initializeWithAlphaAndPW(alphabet, hashFile, nTasks - 1, "a", "0000");

        printServerContext(context);
        appendServerContextToFile(FOUND_PASSWORD_FILE_NAME, context, alphabet);



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


        DBG_OK("Sending alphabet");

        int alphaLengthValues[nTasks - 1];
        sds alphabetBuffer[nTasks - 1];

        /* Send the password generation algorithm identifier. Tag parameter -> 3*/
        for (int i = 1; i < nTasks; i++) {

            alphaLengthValues[i - 1] = strlen(alphabet);
            alphabetBuffer[i - 1] = sdsnew(alphabet);
            MPI_Send(&alphaLengthValues[i - 1], 1, MPI_INT, i, 6, MPI_COMM_WORLD);

            MPI_Send(alphabetBuffer[i - 1], alphaLengthValues[i - 1], MPI_CHAR, i, 7, MPI_COMM_WORLD);
        }

        /*Wait until all clients received the data.*/
        MPI_Barrier(MPI_COMM_WORLD);

        threadContext = createThreadContext(&fh, 1);

        DBG_OK("Sent all data to clients ... now going to receive cracked hashes");



        for (int i = nTasks - 1; i <= 0; i) {
            free((context->tasks + i));
        }

        char runningClients[nTasks - 1];
        memset(runningClients, '1', sizeof (char)*(nTasks - 1));

        ulong hashesToReceive = threadContext->numHashes;

        printStartTimeToFile();
        ulong hashesFound = 0;
        for (; hashesFound < hashesToReceive; hashesFound++) {
            MPI_Status status;
            int pwLen;
            int hashLen;
            char* pwBuffer;
            char* hashBuffer;


            MPI_Recv(&pwLen, 1, MPI_INT, MPI_ANY_SOURCE, 4711, MPI_COMM_WORLD, &status);
            pwBuffer = (char*) malloc(sizeof (char)*(pwLen + 1));
            MPI_Recv(pwBuffer, pwLen, MPI_CHAR, status.MPI_SOURCE, 4712, MPI_COMM_WORLD, &status);
            pwBuffer[pwLen] = '\0';

            if (strcmp(pwBuffer, CLIENT_FINISH_MESSAGE) == 0) {
                printf("Client %d finished job.\n", status.MPI_SOURCE);

                runningClients[status.MPI_SOURCE - 1] = '0';
                if (hasRunningClients(runningClients, nTasks - 1) == 0) {
                    printf("All clients finished their jobs! MPI-Master is terminating\n");
                    free(pwBuffer);
                    break;
                }
            }else{
            MPI_Recv(&hashLen, 1, MPI_INT, status.MPI_SOURCE, 4713, MPI_COMM_WORLD, &status);
            hashBuffer = (char*) malloc(sizeof (char)*(hashLen + 1));
            MPI_Recv(hashBuffer, hashLen, MPI_CHAR, status.MPI_SOURCE, 4714, MPI_COMM_WORLD, &status);

            hashBuffer[hashLen] = '\0';

            printf("%sPassword found on client %d: %s -> %s%s\n", KGRN, status.MPI_SOURCE, pwBuffer, hashBuffer, KNRM);
            printFoundPwToFile(pwBuffer, hashBuffer);
            free(hashBuffer);
            }
            free(pwBuffer);

        }
        free(context);

        printEndTimeToFile();
        /*If we reach here, we have found any password, so we can terminate MPI execution environment.*/
        sleep(4);
        if (hashesFound == hashesToReceive) {
            printf("All passwords found. Finishing execution.\n");
            finishExecution();
        }

    } else {
        MPI_Status status;

        int startPwLen, endPwLen, hashFileNameLen, pwAlgoValue, alphabetLen;

        char* hashFileName = (char*) malloc(sizeof (char)*MAX_FILENAME_LEN);
        char* alpha = (char*) malloc(sizeof (char)*MAX_ALPHA_LEN);
        char* endPass = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        char* startPass = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(hashFileName, '\0', sizeof (char)*MAX_FILENAME_LEN);
        memset(alpha, '\0', sizeof (char)*MAX_FILENAME_LEN);
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

        MPI_Recv(&alphabetLen, 1, MPI_INT, 0, 6, MPI_COMM_WORLD, &status);
        MPI_Recv(alpha, alphabetLen, MPI_CHAR, 0, 7, MPI_COMM_WORLD, &status);

        MPI_Barrier(MPI_COMM_WORLD);

        DBG_OK("Received data from server. Hash filename: %s\n          start: %s, end: %s, pwAlgoType:%d\n          alphabet: %s", hashFileName, startPass, endPass, pwAlgoValue, alpha);

        PasswordGenTask* clientTaskInfo = createClientTask(pwAlgoValue, startPass, endPass);


        int i = 0;
        int numThreads = getCpuCount();
        setNumThreads(numThreads);
        threadContext = createThreadContext(&fh, numThreads);
        printf("child process with %d threads!\n", numThreads);

        char **passphraseBuffer = (char**) malloc(sizeof (char*) * numThreads);
        for (i = 0; i < numThreads; i++) {
            passphraseBuffer[i] = (char*) malloc(sizeof (char) * (MAX_PASSWORD + 1));
            memset(passphraseBuffer[i], 0, sizeof (passphraseBuffer[i]));
        }

        //        struct timeval timeBefore, timeAfter;
        //        gettimeofday(&timeBefore, NULL);
        //printHashes(pwHashes, rank);

        initLock();
        bruteforcePasswordTaskObserved(clientTaskInfo, threadContext, threadContext->attackStrategy->bruteforceMethod, sendFoundPasswordAndHashToRoot, passphraseBuffer, rank);
        clearLock();

        //        gettimeofday(&timeAfter, NULL);
        //        printf("needed %ld secs %ld usecs\n", (ulong) (timeAfter.tv_sec - timeBefore.tv_sec), (ulong) (timeAfter.tv_usec - timeBefore.tv_usec));
        
        
        sendClientFinishMessageToRoot();
        sleep(3);
    }
    freeThreadContext(threadContext);

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

int main(int argc, char **argv) {
    char *alphabet = "abcd";
    //char *alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    PasswordGenerationContext* context = createDefaultContextWithAlphabet(alphabet);
    initializeGlobals(alphabet);
    ulong numpass = context->passwordDiff("a", "dddd");
    char result[MAX_PASSWORD];
    memset(result, 0, MAX_PASSWORD);
    DBG_OK("need to check %lu pws!", numpass);
#if 1
    for (ulong i = 0; i < numpass + 1; i++) {
        context->passwordAt(i, result);
        printf("%s\n", result);
    }
#endif
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

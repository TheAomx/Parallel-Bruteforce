
#include "core_headers.h"

/**
 * The default alphabet used.
 */
static char defaultAlphabet[] = {"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"};

static void initClientTask(ClientTask* outTask, long numPasswords, char* startPW, char* endPw, PwGenAlgoType pwGenAlgoType) {
    outTask->startPass = startPW;
    outTask->endPass = endPw;
    outTask->numPass = numPasswords;
    outTask->pwAlgoType = pwGenAlgoType;
}

static void createClientTasks(int numTasks, long numPasswords, char* startPW, char* endPw, ClientTask* out, PwGenAlgoType pwGenAlgoType) {
    int isDivider = ((numPasswords % numTasks) == 0) ? 1 : 0;
    ulong passwordsPerTask = numPasswords / numTasks;
    ulong currentOffset = toNumberIndefaultAlphabet(startPW);

    /*Calculate the number of tasks which are all calculating the same amount of work.*/
    int fullLengthTasks = (isDivider == 1) ? (numTasks) : (numTasks - 1);
    char* tempPwd;
    char* tempPwd2;

    /*Initialize the first 'fullLengthTasks' tasks with an equal amount of work...*/
    for (int i = 0; i < fullLengthTasks; i++) {
        /*Temp Pointers for start and end*/
        tempPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        tempPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(tempPwd, '\0', sizeof (char)*MAX_PASSWORD);
        memset(tempPwd2, '\0', sizeof (char)*MAX_PASSWORD);

        getPasswordAt(currentOffset, tempPwd);
        getPasswordAt(passwordsPerTask + currentOffset, tempPwd2);

        currentOffset += passwordsPerTask;

        initClientTask((out + i), passwordsPerTask, tempPwd, tempPwd2, pwGenAlgoType);
    }
    /* If the number of passwords could not be divided into equal parts, the last client has to do a bit more work than others.
     * The amount of work to do for this task can be defined as follows: work_this = work_all - (work_others*num_others)
     */

    if (isDivider == 0) {
        /*Temp Pointers for start and end*/
        tempPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        tempPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);
        memset(tempPwd, '\0', sizeof (char)*MAX_PASSWORD);
        memset(tempPwd2, '\0', sizeof (char)*MAX_PASSWORD);

        int index = (numTasks - 1);
        ulong numPwd = numPasswords - (passwordsPerTask * (numTasks - 1));
        getPasswordAt(currentOffset, tempPwd);
        getPasswordAt(numPwd + currentOffset, tempPwd2);
        initClientTask((out + index), numPwd, tempPwd, tempPwd2, pwGenAlgoType);
    }
}

static void initServerContext(char* hashesFileName, char* start, char* end, int numWorkers, ulong numPasswords, ServerContext* out, PwGenAlgoType pwGenAlgo) {
    
    out->startPassword = start;
    out->endPassword = end;
    out->numClients = numWorkers;
    out->numPasswords = numPasswords;
    out->type = DEFAULT;
    out->hashesFileName = hashesFileName;
    out->tasks = (ClientTask*) malloc(sizeof (ClientTask) * numWorkers);

    createClientTasks(numWorkers, numPasswords, start, end, out->tasks, DEFAULT);
}

ServerContext* initializeWithPW(char* hashesFileName, int numWorkers, char* startPW, char* endPW) {
    return initializeWithAlphaAndPW(defaultAlphabet,hashesFileName,numWorkers,startPW,endPW);
}
ServerContext* initializeWithAlphaAndPW(char* alphabet, char* hashesFileName, int numWorkers, char* startPW, char* endPW) {
    initializeGlobals(alphabet);
    ulong numPWD = getPasswordDiff(startPW, endPW);
    ServerContext* result = (ServerContext*) malloc(sizeof (ServerContext));
    initServerContext(hashesFileName, startPW, endPW, numWorkers, numPWD, result, DEFAULT);
    freeGlobals();
    return result;
}

ServerContext* initializeWithLenght(char* hashesFileName, int numWorkers, char* startPW, ulong numPasswords) {
    return initializeWithAlphaAndLenght(defaultAlphabet,hashesFileName,numWorkers,startPW,numPasswords);
}
ServerContext* initializeWithAlphaAndLenght(char* alphabet, char* hashesFileName, int numWorkers, char* startPW, ulong numPasswords) {
    initializeGlobals(alphabet);
    ulong startValue = toNumberIndefaultAlphabet(startPW);
    char* tmp = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    memset(tmp, '\0', sizeof (char)*MAX_PASSWORD);
    getPasswordAt(startValue + numPasswords, tmp);
    ulong numPWD = numPasswords;
    ServerContext* result = (ServerContext*) malloc(sizeof (ServerContext));
    initServerContext(hashesFileName, startPW, tmp, numWorkers, numPWD, result, DEFAULT);
    freeGlobals();
    return result;
}

static void printClientTask(ClientTask task) {
    printf("      Client task configuration:\n         Start: %s, end: %s, count: %ld\n", task.startPass, task.endPass, task.numPass);
}

void printServerContext(ServerContext* ctx) {
    printf("\nServer context with %d clients\n", ctx->numClients);
    printf("   Operation is to check %ld passwords. Each client will have to generate at least %ld passwords\n", ctx->numPasswords, ctx->tasks[0].numPass);
    printf("      Start password is: %s, end is: %s\n", ctx->startPassword, ctx->endPassword);
    printf("   Client configurations:\n");
    for (int i = 0; i < ctx->numClients; i++) {
        printClientTask(ctx->tasks[i]);
    }
    printf("   Each client will use %s algorithm to generate new passwords.\n", PW_GEN_TYPE_TO_STRING(ctx->type));
    printf("   Target hashes are pulled from file with name %s.\n\n", ctx->hashesFileName);
    fflush(stdout);
}

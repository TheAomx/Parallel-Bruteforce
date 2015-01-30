/* 
 * File:   bruteforce.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include "core_headers.h"

static BruteforceProgressStats* createStats() {
    BruteforceProgressStats* stats = (BruteforceProgressStats*) malloc(sizeof(BruteforceProgressStats));
    
    stats->time = 0;
    stats->takeTimeOffset = 1;
    stats->timeDiff;
    stats->kiloHashesPerSecond = 0;
    stats->checkedLast = 0;
    stats->pwDiff = 0;
        
    return stats;
}

static BruteforceProgressStats** createStatsForThreads (int numThreads) {
    int i = 0;
    BruteforceProgressStats** stats = (BruteforceProgressStats**) malloc(sizeof(BruteforceProgressStats*) * (numThreads+1));
    
    for (i = 0; i < numThreads; i++) {
        stats[i] = createStats();
    }
    
    stats[i] = NULL;
    
    return stats;
}

static void freeStats ( BruteforceProgressStats **stats) {
    int i = 0;
    
    while (stats[i] != NULL) {
        free(stats[i]);
    }
    
    free(stats);
}

static void printProgess(BruteforceProgressStats *stats, int numThreads, ulong currentPasswordIndex, ulong numPasswords, char *currentPassphrase) {
    const ulong perfCounterCheckIntervall = 10000000;

    #ifdef _OPENMP
    if (stats->takeTimeOffset == 1) {
        stats->time = omp_get_wtime();
        stats->takeTimeOffset = 0;
    }
    #endif
    
    int threadID = getThreadID();

    if ((currentPasswordIndex % perfCounterCheckIntervall) == 0) {
        ulong needToCheck = (numPasswords / numThreads);
        ulong checkedPws = currentPasswordIndex - threadID * needToCheck;
        double percentFinished = (double) checkedPws / (double) needToCheck;
        percentFinished *= 100.0;

#ifdef _OPENMP
        stats->timeDiff = omp_get_wtime() - stats->time;
        stats->takeTimeOffset = 1;
        stats->pwDiff = checkedPws - stats->checkedLast;
        if (currentPasswordIndex == 0 || stats->pwDiff <= 0) {
            stats->kiloHashesPerSecond = 0.0;
            stats->timeDiff = 0.0;
        } else {
            stats->kiloHashesPerSecond = (((double) stats->pwDiff) * (1.0 / stats->timeDiff)) / 1000.0;

        }
        printf("[%d] %.2f%% %s, %.3f msec(per %ld hashes) -> %.3f kHashes/s.\n", threadID, percentFinished, currentPassphrase, stats->timeDiff * 1000.0, perfCounterCheckIntervall, stats->kiloHashesPerSecond);
#else
        printf("[%d] %.2f%% %s\n", threadID, percentFinished, currentPassphrase);
#endif
        fflush(stdout);
        stats->checkedLast = checkedPws;
    }
}

void bruteforcePasswordTask(PasswordGenTask* taskInfo, void *ctx, bruteforceCallback callback, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    char* tmpPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    char* tmpPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);

   

#ifdef _OPENMP

#endif

    memset(tmpPwd, '\0', sizeof (char)*MAX_PASSWORD);
    memset(tmpPwd2, '\0', sizeof (char)*MAX_PASSWORD);

    strncpy(tmpPwd, taskInfo->startPassword, len);

    PasswordHashes* pwHashes = (PasswordHashes*) ctx;
    int numThreads = pwHashes->numThreads;
        
    BruteforceProgressStats **statsData = createStatsForThreads(numThreads);

    //DBG_OK("           %s(%ld)      ->      %s(%ld)    :%ld   ", tmpPwd,(pwGenType->valueOf(tmpPwd)),tmpPwd2,(pwGenType->valueOf(tmpPwd2)),i);
    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("need to check %lu passwords!", count);
    //FIXME: Splitting work into peaces and do a parallel region with countPasswords.

    ulong offset = context->valueOf(taskInfo->startPassword);

#pragma omp parallel for
    for (ulong i = 0; i <= count; i++) {
        int threadID = getThreadID();
        char *currentPassphrase = passphraseBuffer[threadID];

        context->passwordAt(i + offset, currentPassphrase);
        callback((void*) ctx, currentPassphrase);

        printProgess(statsData[threadID], numThreads, i, count, currentPassphrase);
    }
    freeStats(statsData);
}

void bruteforcePasswordTaskObserved(PasswordGenTask* taskInfo, void *ctx, bruteforceCallbackObserved callback, hashFoundCallback onHashFound, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    char* tmpPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    char* tmpPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);

    memset(tmpPwd, '\0', sizeof (char)*MAX_PASSWORD);
    memset(tmpPwd2, '\0', sizeof (char)*MAX_PASSWORD);

    strncpy(tmpPwd, taskInfo->startPassword, len);

    PasswordHashes* pwHashes = (PasswordHashes*) ctx;
    int numThreads = pwHashes->numThreads;
    
    BruteforceProgressStats **statsData = createStatsForThreads(numThreads);


    context->initData(context->alphabet);

    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("need to check %lu passwords!", count);
    //FIXME: Splitting work into peaces and do a parallel region with countPasswords.

    ulong offset = context->valueOf(taskInfo->startPassword);

#pragma omp parallel for
    for (ulong i = 0; i <= count; i++) {
        int threadID = getThreadID();
        char *currentPassphrase = passphraseBuffer[threadID];

        context->passwordAt(i + offset, currentPassphrase);
        callback((void*) ctx, currentPassphrase, onHashFound);

        printProgess(statsData[threadID], numThreads, i, count, currentPassphrase);

    }
    freeStats(statsData);
    context->clearData();
}

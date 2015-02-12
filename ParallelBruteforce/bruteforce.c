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
        i++;
    }
    
    free(stats);
}

static void printProgess(BruteforceProgressStats *stats, int numThreads, ulong currentPasswordIndex, ulong numPasswords, char *currentPassphrase) {
    const ulong perfCounterCheckIntervall = 50000000;

    if (stats->takeTimeOffset == 1) {
        stats->time = getElapsedTime();
        stats->takeTimeOffset = 0;
    }

    if ((currentPasswordIndex % perfCounterCheckIntervall) == 0) {
        int threadID = getThreadID();
        ulong needToCheck = (numPasswords / numThreads);
        ulong checkedPws = currentPasswordIndex - threadID * needToCheck;
        double percentFinished = (double) checkedPws / (double) needToCheck;
        percentFinished *= 100.0;

        stats->timeDiff = getElapsedTime() - stats->time;
        stats->takeTimeOffset = 1;
        stats->pwDiff = checkedPws - stats->checkedLast;
        
        if (currentPasswordIndex == 0 || stats->pwDiff <= 0) {
            stats->kiloHashesPerSecond = 0.0;
            stats->timeDiff = 0.0;
        } 
        else {
            stats->kiloHashesPerSecond = (((double) stats->pwDiff) * (1.0 / stats->timeDiff)) / 1000.0;

        }
        
        printf("[%d] %.2f%% %s, %.3f msec(per %ld hashes) -> %.3f kHashes/s.\n", threadID, percentFinished, currentPassphrase, stats->timeDiff * 1000.0, perfCounterCheckIntervall, stats->kiloHashesPerSecond);
        fflush(stdout);
        
        stats->checkedLast = checkedPws;
    }
}

void bruteforcePasswordTask(PasswordGenTask* taskInfo, void *ctx, bruteforceCallback callback, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    ThreadContext* threadContext = (ThreadContext*) ctx;
    
    int numThreads = threadContext->numThreads;
    BruteforceProgressStats **statsData = createStatsForThreads(numThreads);

    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);

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
    ThreadContext* threadContext = (ThreadContext*) ctx;
    
    int numThreads = threadContext->numThreads;
    BruteforceProgressStats **statsData = createStatsForThreads(numThreads);
    context->initData(context->alphabet);

    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);

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

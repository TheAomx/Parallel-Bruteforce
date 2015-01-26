/* 
 * File:   bruteforce.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include "core_headers.h"

void bruteforcePasswordTask(PasswordGenTask* taskInfo, void *ctx, bruteforceCallback callback, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    char* tmpPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    char* tmpPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);

    const ulong perfCounterCheckIntervall = 10000000;

#ifdef _OPENMP
    double time = 0;
    char takeTimeOffset = 1;
    double timeDiff;
    double kiloHashesPerSecond = 0;
    ulong checkedLast = 0;
    ulong pwDiff = 0;
#endif


    memset(tmpPwd, '\0', sizeof (char)*MAX_PASSWORD);
    memset(tmpPwd2, '\0', sizeof (char)*MAX_PASSWORD);

    strncpy(tmpPwd, taskInfo->startPassword, len);

    PasswordHashes* pwHashes = (PasswordHashes*) ctx;
    int numThreads = pwHashes->numThreads;



    //DBG_OK("           %s(%ld)      ->      %s(%ld)    :%ld   ", tmpPwd,(pwGenType->valueOf(tmpPwd)),tmpPwd2,(pwGenType->valueOf(tmpPwd2)),i);
    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("need to check %lu passwords!", count);
    //FIXME: Splitting work into peaces and do a parallel region with countPasswords.

    ulong offset = context->valueOf(taskInfo->startPassword);

#pragma omp parallel for private(checkedLast,pwDiff,timeDiff,takeTimeOffset,kiloHashesPerSecond,time)
    for (ulong i = 0; i <= count; i++) {
        int threadID = getThreadID();
        char *currentPassphrase = passphraseBuffer[threadID];
#ifdef _OPENMP

        if (takeTimeOffset == 1) {

            time = omp_get_wtime();
            takeTimeOffset = 0;
        }
#endif

        context->passwordAt(i + offset, currentPassphrase);
        callback((void*) ctx, currentPassphrase);


        if ((i % perfCounterCheckIntervall) == 0) {
            ulong needToCheck = (count / numThreads);
            ulong checkedPws = i - threadID * needToCheck;
            double percentFinished = (double) checkedPws / (double) needToCheck;
            percentFinished *= 100.0;
#ifdef _OPENMP
            timeDiff = omp_get_wtime() - time;
            takeTimeOffset = 1;
            if (checkedLast == 0) {
                pwDiff = 0;
            } else {
                pwDiff = checkedPws - checkedLast;
            }
            if (i == 0 || pwDiff <= 0) {
                kiloHashesPerSecond = 0.0;
                timeDiff = 0.0;

            } else {
                kiloHashesPerSecond = (((double) pwDiff) * (1.0 / timeDiff)) / 1000.0;

            }
            printf("[%d] %.2f%% %s, %.3f msec(per %ld hashes) -> %.3f kHashes/s.\n", threadID, percentFinished, currentPassphrase, timeDiff * 1000.0, perfCounterCheckIntervall, kiloHashesPerSecond);
#else
            printf("[%d] %.2f%% %s\n", threadID, percentFinished, currentPassphrase);
#endif
            fflush(stdout);
            checkedLast = checkedPws;
        }

    }
}

void bruteforcePasswordTaskObserved(PasswordGenTask* taskInfo, void *ctx, bruteforceCallbackObserved callback, hashFoundCallback onHashFound, char **passphraseBuffer) {
    PasswordGenerationContext* context = taskInfo->generationContext;
    int len = strlen(taskInfo->startPassword);
    char* tmpPwd = (char*) malloc(sizeof (char)*MAX_PASSWORD);
    char* tmpPwd2 = (char*) malloc(sizeof (char)*MAX_PASSWORD);

    const ulong perfCounterCheckIntervall = 5000000;

#ifdef _OPENMP
    double time = 0;
    char takeTimeOffset = 1;
    double timeDiff;
    double kiloHashesPerSecond = 0;
    ulong checkedLast = 0;
    ulong pwDiff = 0;
#endif


    memset(tmpPwd, '\0', sizeof (char)*MAX_PASSWORD);
    memset(tmpPwd2, '\0', sizeof (char)*MAX_PASSWORD);

    strncpy(tmpPwd, taskInfo->startPassword, len);

    PasswordHashes* pwHashes = (PasswordHashes*) ctx;
    int numThreads = pwHashes->numThreads;


    context->initData(context->alphabet);

    ulong count = context->passwordDiff(taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("Generating %ld passwords from %s to %s.", count, taskInfo->startPassword, taskInfo->endPassword);
    DBG_OK("need to check %lu passwords!", count);
    //FIXME: Splitting work into peaces and do a parallel region with countPasswords.

    ulong offset = context->valueOf(taskInfo->startPassword);

#pragma omp parallel for private(checkedLast,pwDiff,timeDiff,takeTimeOffset,kiloHashesPerSecond,time)
    for (ulong i = 0; i <= count; i++) {
        int threadID = getThreadID();
        char *currentPassphrase = passphraseBuffer[threadID];
#ifdef _OPENMP

        if (takeTimeOffset == 1) {

            time = omp_get_wtime();
            takeTimeOffset = 0;
        }
#endif

#ifdef PROFILE_ALGOS 
        long double testTime = (long double) omp_get_wtime()*1000000.0;
#endif

        context->passwordAt(i + offset, currentPassphrase);

#ifdef PROFILE_ALGOS
        printf("passwordAt time: %Lf usec.\n", ((long double) omp_get_wtime()*1000000.0) - testTime);
        fflush(stdout);
#endif
#ifdef PROFILE_ALGOS
        testTime = (long double) omp_get_wtime()*1000000.0;
#endif

        callback((void*) ctx, currentPassphrase, onHashFound);

#ifdef PROFILE_ALGOS
        printf("callback time: %Lf usec.\n", ((long double) omp_get_wtime()*1000000.0) - testTime);
#endif


        if ((i % perfCounterCheckIntervall) == 0) {
            ulong needToCheck = (count / numThreads);
            ulong checkedPws = i - threadID * needToCheck;
            double percentFinished = (double) checkedPws / (double) needToCheck;
            percentFinished *= 100.0;
#ifdef _OPENMP
            timeDiff = omp_get_wtime() - time;
            takeTimeOffset = 1;
            if (checkedLast == 0) {
                pwDiff = 0;
            } else {
                pwDiff = checkedPws - checkedLast;
            }
            if (i == 0 || pwDiff <= 0) {
                kiloHashesPerSecond = 0.0;
                timeDiff = 0.0;
            } else {
                kiloHashesPerSecond = (((double) pwDiff) * (1.0 / timeDiff)) / 1000.0;

            }
            if (kiloHashesPerSecond <= 1.0) {
                printf("[%d] %.2f%% %s.\n", threadID, percentFinished, currentPassphrase);
            } else {
                printf("[%d] %.2f%% %s, %.3f msec(per %ld hashes) -> %.3f kHashes/s.\n", threadID, percentFinished, currentPassphrase, timeDiff * 1000.0, perfCounterCheckIntervall, kiloHashesPerSecond);

            }
#else
            printf("[%d] %.2f%% %s\n", threadID, percentFinished, currentPassphrase);
#endif
            fflush(stdout);
            checkedLast = checkedPws;
        }

    }
    context->clearData();
}

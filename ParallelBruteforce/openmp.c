#ifdef _OPENMP
    #include <omp.h>
#else
    #include <sys/time.h>
    #include <stdlib.h>
#endif

#include "openmp.h"
#include "bruteforce.h"

#ifdef _OPENMP
    omp_lock_t* lock = NULL;
#endif
    
void initLock() {
#ifdef _OPENMP 
    lock = (omp_lock_t*) malloc(sizeof (omp_lock_t));
    omp_init_lock(lock);
#endif
}

void clearLock() {
#ifdef _OPENMP 
    omp_destroy_lock(lock);
    free(lock);
#endif
}

void lockMutex() {
#ifdef _OPENMP 
    omp_set_lock(lock);
#endif
}

void releaseMutex() {
#ifdef _OPENMP 
    omp_unset_lock(lock);
#endif
}

int getThreadID() {
    int threadID = 0;
    
#ifdef _OPENMP 
    threadID = omp_get_thread_num();
#endif
    
    return threadID;
}

int getNumThreads() {
    int numThreads = 1;
    
#ifdef _OPENMP 	
    numThreads = omp_get_num_threads();
#endif
    
    return numThreads;
}

int getNumCores() {
    int numCores = 1;
    
#ifdef _OPENMP 
    numCores = omp_get_num_procs();
#endif
    
    return numCores;
}

void setNumThreads(int numThreads) {
#ifdef _OPENMP 
    omp_set_num_threads(numThreads);
#endif
}

double getElapsedTime () {
    double elapsedTime = 0.00;
    
#ifdef _OPENMP 
    elapsedTime = omp_get_wtime();
#else
    struct timeval timeStruct;
    gettimeofday(&timeStruct, NULL);
    elapsedTime = (double) timeStruct.tv_sec;
    elapsedTime += (double) timeStruct.tv_usec * 0.0000001;
#endif
    
    return elapsedTime;
}

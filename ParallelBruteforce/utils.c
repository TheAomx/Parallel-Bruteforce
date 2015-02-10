/* 
 * File:   utils.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#include "core_headers.h"


#define BUFFER_SIZE 256

void readFile(char *filename, FileReader callback, void *ctx) {
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    FILE *file;

    file = fopen(filename, "r");

    if (file == NULL) {
        printf("open of %s failed!\n", filename);
        exit(EXIT_FAILURE);
    }

    do {
        bytesRead = fread((void*) buffer, 1, BUFFER_SIZE, file);
        callback(buffer, bytesRead, ctx);
    } while (bytesRead != 0);

    if (!feof(file)) {
        printf("error at reading occured!\n");
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

void appendToFile(char* fileName, char* text){
    FILE *file;
    file=fopen(fileName, "a");
    
    if(!file) {
        perror("Error opening file.");
        return;
    }
        fprintf(file, "%s\n", text);
    
    fclose(file);
}

unsigned long pow_ul(unsigned long base, unsigned long exp) {
    unsigned long t = 1;
    unsigned long i = 0;

    for (i = 0; i < exp; i++) {
        t *= base;
    }
    return t;
}

long getCpuCount() {
    long nprocs = -1;
#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
    SYSTEM_INFO info;
    GetSystemInfo(&info);
#define sysconf(a) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
    nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        fprintf(stderr, "Could not determine number of CPUs online:\n%s\n",
            strerror(errno));
        return 1L;
    }
    printf("%ld processors online\n", nprocs);

   
    return nprocs;
#else
    fprintf(stderr, "Could not determine number of CPUs");
    return result;
#endif

}


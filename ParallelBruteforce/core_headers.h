/* 
 * File:   core_headers.h
 *      Includes headers and defines types used by the application.
 * Author: TBD
 *
 * Created on 13. Januar 2015, 18:41
 */

#ifndef CORE_HEADERS_H
#define	CORE_HEADERS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DEBUG 1
#define MAX_FILENAME_LEN 130
#define MAX_ALPHA_LEN 200

    /* ansi color codes used at the dbg macros for coloured output. */

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

    /* define DBG-macros for debugging purposes if DEBUG is defined...*/

#ifdef DEBUG
#define DBG_MSG(color, stream, format, ...) do {\
                fprintf(stream, "%sDBG:%s \"", color, KNRM);\
                fprintf(stream, format, ##__VA_ARGS__);\
                fprintf(stream, "\" function: %s file: %s line: %d\n",(char*) __func__, (char*)__FILE__, __LINE__);} while(0)

#define DBG_OK(format, ...) do {\
                            DBG_MSG(KGRN, stdout, format, ##__VA_ARGS__);\
                } while(0)
#define DBG_WARN(format, ...) do {\
                            DBG_MSG(KYEL, stderr, format, ##__VA_ARGS__);\
                } while(0)
#define DBG_ERR(format, ...) do {\
                    DBG_MSG(KRED, stderr, format, ##__VA_ARGS__);\
                    exit(EXIT_FAILURE);\
                    } while(0)
#else
#define DBG_MSG(color, stream, format, ...) do {} while(0)
#define DBG_OK(format, ...) do {} while(0)
#define DBG_WARN(format, ...) do {} while(0)
#define DBG_ERR(format, ...) do {} while(0)
#endif


#include "HashAlgos/hash_types.h"

    /* System headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/time.h>
#include <time.h>
#include <mpi.h>
    /* Application headers */
#ifdef _OPENMP 
//#define PROFILE_ALGOS
#endif 

#include "sds.h"
#include "openmp.h"
#include "password_algo.h"
#include "utils.h"
#include "HashAlgos/hashing_algo.h"
#include "bruteforce.h"


#include "pb_server.h"
#include "pb_client.h"





#ifdef	__cplusplus
}
#endif

#endif	/* CORE_HEADERS_H */



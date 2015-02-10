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

#include "debug_macros.h"

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



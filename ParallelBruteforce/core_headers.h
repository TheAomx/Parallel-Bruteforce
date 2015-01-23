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


    // Signed variables are for wimps 
    typedef unsigned char uchar;
    typedef unsigned int uint;
    typedef unsigned long ulong;
    typedef unsigned short ushort;

    /* System headers */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/time.h>
#include <mpi.h>
#include <omp.h>
    /* Application headers */
#include "sds.h"
#include "openmp.h"
#include "password_algo.h"
#include "utils.h"
#include "sha1.h"
#include "sha256.h"
#include "md5.h"
#include "sha1_prop.h"
#include "bruteforce.h"
#include "hashing_algo.h"

#include "pb_server.h"
#include "pb_client.h"





#ifdef	__cplusplus
}
#endif

#endif	/* CORE_HEADERS_H */





//
//int main(int argc, char* argv[])
//{
//
//	int maxNum;
//
//	int  nTasks, rank; 
//    MPI_Init( &argc, &argv );
//    MPI_Comm_size( MPI_COMM_WORLD, &nTasks );
//    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
//
//	if (rank == 0)
//	{
//		printf ("Number of threads = %d\n", nTasks);
//		do
//		{
//			cout << "Please input number between 0 and 50000: ";
//			cin >> maxNum;
//		} while(maxNum < 0 || maxNum > 50000);
//	}
//
//	MPI_Barrier(MPI_COMM_WORLD);
//	
//	MPI_Bcast(&maxNum, 1, MPI_INT, 0, MPI_COMM_WORLD);
//
//	//printf("Thread %d Recieved %d\n", rank, maxNum);
//
//	// Each thread only needs to compute a portion of the problem.
//	int portion = maxNum / nTasks;
//
//	int startNum = rank * portion;	// calculate the starting number for this thread
//	int endNum = (rank + 1) * portion;	// calculate the ending number +1 for this thread
//	if (rank == nTasks-1)
//	{
//		// if We're the last thread, then we should automatically set the ending number.
//		// This is because when we divided maxNum by nTasks, there could have been a remainder left off.
//		// My manually setting the ending number, we ensure that all numbers are properly computed.
//		endNum = maxNum+1;
//	}
//
//	int total = 0;
//	for (int i=startNum; i < endNum; i++)
//	{
//		total += i;
//	}
//
//	printf("Thread %d computed %d\n", rank, total);
//
//	if (rank == 0)
//	{
//		// The master thread will need to receive all computations from all other threads.
//		MPI_Status status;
//
//		// MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
//		// We need to go and receive the data from all other threads.
//		// The arbitrary tag we choose is 1, for now.
//
//		for (int i=1; i < nTasks; i++)
//		{
//			int temp;
//			MPI_Recv(&temp, 1, MPI_INT, i,1, MPI_COMM_WORLD, &status);
//			//printf("RECEIVED %d from thread %d\n", temp, i);
//			total += temp;
//		}
//	}
//	else
//	{
//		// We are finished with the results in this thread, and need to send the data to thread 1.
//		// MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
//		// The destination is thread 0, and the arbitrary tag we choose for now is 1.
//		MPI_Send(&total, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
//	}
//
//	if (rank == 0)
//	{
//		// Display the final calculated value
//		printf("The calculated value is %d\n", total);
//	}
//
//	MPI_Finalize();
//	return 0;
//}


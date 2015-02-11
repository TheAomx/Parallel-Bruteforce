/* 
 * File:   server.h
 *      Defines a set of server (mpi master) side functions and types.
 *      
 * Author: TBD
 *
 * 
 * Created on 13. Januar 2015, 11:29
 */

#ifndef PB_SERVER_H
#define	PB_SERVER_H
#include "core_headers.h"
#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Holds information about a portion of work to be done by one client.
     */
    struct clientTask {
        /**
         * The type of algorithm to be used by the client.
         */
        enum PwGenAlgoType pwAlgoType;
        /**
         * The start password.
         */
        char* startPass;
        /**
         * The end password.
         */
        char* endPass;
        /**
         * Number of passwords between start and end .
         */
        ulong numPass;
    };

    typedef struct clientTask ClientTask;

    /**
     * Type definition for server meta data.
     */
    struct ServerContext {
        /**
         * The number of clients used by the server.
         */
        int numClients;
        /**
         * The first password to be generated by one of the clients.
         */
        char* startPassword;
        /**
         * The last password to be generated by one of the clients.
         */
        char* endPassword;
        /**
         * The array of tasks (portion of work) the server will distribute to its clients.
         */
        ClientTask* tasks;
        /**
         * The type of password generation algorithm to be used by the clients.
         */
        int type;
        /**
         * The overall number of passwords to be generated during the process runtime.
         */
        ulong numPasswords;

        /**
         * The name of the file to read the target hashes from.
         */
        char* hashesFileName;

    };

    typedef struct ServerContext ServerContext;


    /**
     * Does the same like initializeWithAlphaAndPW but uses a default alphabet (letters and numbers) 
     */
    ServerContext* initializeWithPW(char* hashListFileName, int numWorkers, char* startPW, char* endPW);
    /**
     * Initializes a ServerContext which holds the required information needed by the (MPI) server.
     * @param alphabet The alphabet to use.
     * @param hashesFileName The name of the file containing the hash list.
     * @param numWorkers The number of (MPI) tasks that will do the hard job.
     * @param startPW The start password to be used by the first task.
     * @param endPW The end password to be used by the last task.
     * @return A newly allocated and initialized ServerContext. 
     */
    ServerContext* initializeWithAlphaAndPW(char* alphabet, char* hashesFileName, int numWorkers, char* startPW, char* endPW);
    

    
    /**
     * Does the same like initializeWithAlphaAndLenght but uses a default alphabet (letters and numbers) 
     */
    ServerContext* initializeWithLenght(char* hashListFileName, int numWorkers, char* startPW, ulong numPasswords);
    /**
     * Initializes a ServerContext which holds the required information needed by the (MPI) server.
     * @param alphabet The alphabet to use.
     * @param hashesFileName The name of the file containing the hash list.
     * @param numWorkers The number of (MPI) tasks that will do the hard job.
     * @param startPW The start password to be used by the first task.
     * @param numPasswords The number of passwords to be generated by all tasks.
     * @return A newly allocated and initialized ServerContext. 
     */
    ServerContext* initializeWithAlphaAndLenght(char* alphabet, char* hashesFileName, int numWorkers, char* startPW, ulong numPasswords);
    /**
     * Prints the content of the given ServerContext
     * @param ctx
     */
    void printServerContext(ServerContext* ctx);



#ifdef	__cplusplus
}
#endif

#endif	/* PB_SERVER_H */


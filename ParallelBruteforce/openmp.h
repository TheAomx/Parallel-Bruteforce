/* 
 * File:   openmp.h
 * Author: TBD
 *
 * Created on 20. Januar 2015, 23:48
 */

#ifndef OPENMP_H
#define	OPENMP_H

#ifdef	__cplusplus
extern "C" {
#endif
    /**
     * Retrieves the current thread number associated with the callers thread
     * @return 
     */
    int getThreadID();
    /**
     * Retrieves the number of threads
     * @return 
     */
    int getNumThreads();
    /**
     * Retrieves the number of cores available to the callers machine
     * @return 
     */
    int getNumCores();

    /**
     * Sets the number of threads executed by OpenMP in parallel.
     * @param numThreads
     */
    void setNumThreads(int numThreads);
   
    /**
     * Gets a relative time of elapsed seconds from a certain starting point as a double.
     * @return time elapsed from starting point, e.g. 2.3123f
     */
    double getElapsedTime();


#ifdef	__cplusplus
}
#endif

#endif	/* OPENMP_H */


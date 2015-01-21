/* 
 * File:   openmp.h
 * Author: aomx
 *
 * Created on 20. Januar 2015, 23:48
 */

#ifndef OPENMP_H
#define	OPENMP_H

#ifdef	__cplusplus
extern "C" {
#endif

    int getThreadID();
    int getNumThreads();
    int getNumCores();

    void setNumThreads(int numThreads);


#ifdef	__cplusplus
}
#endif

#endif	/* OPENMP_H */


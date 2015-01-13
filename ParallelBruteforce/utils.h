/* 
 * File:   utils.h
 *      Defines general purpose utility functions.
 * Author: TBD
 *
 * Created on 13. Januar 2015, 15:17
 */

#ifndef UTILS_H
#define UTILS_H
/**
 * Callback function pointer used to update data (the hash value) from streaming input.
 * @param buffer A buffer of data with minimum length of <code>bytesRead</code>
 * @param bytesRead Minimal number of elements contained in <code>buffer</code>
 * @param ctx The hash generation context.
 */
typedef void (*FileReader) (void *buffer, unsigned int bytesRead, void *ctx);
/**
 * 
 * @param filename
 * @param callback
 * @param ctx
 */
void readFile(char *filename, FileReader callback, void *ctx);
/**
 * Power function
 * @param base
 * @param exp
 * @return  <code>result = base^exp </code>
 */
unsigned long pow_ul(unsigned long base, unsigned long exp);

#endif

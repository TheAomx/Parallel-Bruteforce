/* 
 * File:   utils.h
 *      Defines general purpose utility functions.
 * Author: TBD
 *
 * Created on 13. Januar 2015, 15:17
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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

/**
 * Retrieves the current systems CPU count without using OMP methods.
 * (So the value returned is not influenced )
 * @return 
 */
long getCpuCount();

/**
 * Appends the given 'text' to a file with the given 'fileName'
 * @param fileName The file name.
 * @param text The text to append to the file.
 */
void appendToFile(char* fileName, char* text);


#endif
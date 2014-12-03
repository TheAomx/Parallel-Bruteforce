#ifndef UTILS_H
#define UTILS_H
typedef void (*FileReader) (void *buffer, unsigned int bytesRead, void *ctx);
void readFile(char *filename, FileReader callback, void *ctx);
#endif

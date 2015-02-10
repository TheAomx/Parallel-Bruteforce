#ifndef UTILS_H
#define UTILS_H
typedef void (*FileReader) (void *buffer, unsigned int bytesRead, void *ctx);
void readFile(char *filename, FileReader callback, void *ctx);
unsigned long pow_ul (unsigned long base, unsigned long exp);
unsigned long calcNumPasswords(unsigned long alphabetLength, unsigned long maxPasswordLength);
#endif

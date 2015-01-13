/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include "core_headers.h"



int main(int argc, char** argv) {
    printf("%i",argc);
    
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA256");
    uchar hash[SHA256_SIZE];
    
    /*oder auch: 
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA1");
    uchar hash[SHA1_SIZE];*/
    
    getHashFromFile(hashAlgo, "main.c", &hash);
   
    hashAlgo->print(&hash);
    printf("\n");
  
    freeHash(hashAlgo);
    
    return 0;
}


#include <stdio.h>
#include <string.h>
#include "sha256.h"

/*
Output should be:
ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1
cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0
*/

int main()
{
   unsigned char text1[]={"abc"},
                 text2[]={"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"},
                 text3[]={"aaaaaaaaaa"},
                 hash[SHA256_SIZE];
   int idx;
   SHA256_CTX ctx;

   // Hash one
   sha256_init(&ctx);
   sha256_update(&ctx,text1,strlen(text1));
   sha256_final(&ctx,hash);
   sha256_print(hash);
   printf("\n");

   // Hash two
   sha256_init(&ctx);
   sha256_update(&ctx,text2,strlen(text2));
   sha256_final(&ctx,hash);
   sha256_print(hash);
   printf("\n");

   // Hash three
   sha256_init(&ctx);
   for (idx=0; idx < 100000; ++idx)
      sha256_update(&ctx,text3,strlen(text3));
   sha256_final(&ctx,hash);
   sha256_print(hash);
   printf("\n");

   getchar();
   return 0;
}

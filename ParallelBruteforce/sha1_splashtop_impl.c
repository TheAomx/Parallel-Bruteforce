/*
 * sha1.c
 *
 * (C) Copyright 2007-2010 Splashtop Inc.
 *
 * Apr 20, 2010	CVSROOT		Added Splashtop Copyright
 *
 * SHA-1 in C By Steve Reid <steve@edmweb.com>
 */

/************************************************************************
 * 
 ************************************************************************/
#include "sha1.h"



typedef union {
    unsigned char	c[64];
    unsigned int	l[16];
} CHAR64LONG16;

/************************************************************************
 * 
 ************************************************************************/
#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
            |(rol(block->l[i],8)&0x00FF00FF))
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
                ^block->l[(i+2)&15]^block->l[i&15],1))

#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

/************************************************************************
 * Hash a single 512-bit block
 ************************************************************************/
static void
sha1_transform(unsigned int state[5], unsigned char buffer[64])
{

 unsigned int	a, b, c, d, e;
 CHAR64LONG16 *	block;

 block = (CHAR64LONG16 *) buffer;

 /* Copy context->state[] to working vars */
 a = state[0];
 b = state[1];
 c = state[2];
 d = state[3];
 e = state[4];

 /* 4 rounds of 20 operations each. Loop unrolled. */
 R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
 R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
 R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
 R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
 R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
 R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
 R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
 R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
 R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
 R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
 R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
 R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
 R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
 R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
 R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
 R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
 R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
 R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
 R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
 R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

 /* Add the working vars back into context.state[] */
 state[0] += a;
 state[1] += b;
 state[2] += c;
 state[3] += d;
 state[4] += e;

}

/************************************************************************
 * Initialize new context
 ************************************************************************/
static void
sha1_init(SHA1_CTX *context)
{

 context->state[0] = 0x67452301;
 context->state[1] = 0xEFCDAB89;
 context->state[2] = 0x98BADCFE;
 context->state[3] = 0x10325476;
 context->state[4] = 0xC3D2E1F0;
 context->datalen    = 0;

}

/************************************************************************
 * Run your data through this
 ************************************************************************/
static void
sha1_update(SHA1_CTX *context, unsigned char *data, unsigned int len)
{

 unsigned long long	more;
 unsigned int	i;

 more = (context->datalen >> 3) & 63;

 for (i = 0; i < len; i++)
    {
     context->datalen += 8;
     context->data[more++] = data[i];

     if (more == 64)
	{
	 sha1_transform(context->state, context->data);
	 more = 0;
	}
    }

}

/************************************************************************
 * Add padding and return the message digest
 ************************************************************************/
static void
sha1_final(unsigned char *digest, SHA1_CTX *context)
{

 unsigned int	i;
 unsigned char	count[8];

 for (i = 0; i < 8; i++)
    {
     count[i] = (unsigned char)((context->datalen >> ((7-(i&7))<<3)) & 0xff);
    }

 sha1_update(context, (unsigned char *)"\200", 1);

 while (((context->datalen >> 3) & 63) != 56)
    {
     sha1_update(context, (unsigned char *)"\0", 1);
    }

 sha1_update(context, count, 8);

 for (i = 0; i < 20; i++)
    {
     digest[i] = (unsigned char)((context->state[i>>2] >> ((3-(i&3))<<3)) & 0xff);
    }

}

/************************************************************************
 * 
 ************************************************************************/
void
sha1hash(unsigned char *in, unsigned int len, unsigned int ver, unsigned char *out)
{

 SHA1_CTX	ctx;

 sha1_init(&ctx);
 sha1_update(&ctx, in, len);
 sha1_final(out, &ctx);

}

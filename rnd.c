/*
** Copyright (c) 1999, 2000, 2001
** Adel I. Mirzazhanov. All rights reserved
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 
**     1.Redistributions of source code must retain the above copyright notice,
**       this list of conditions and the following disclaimer. 
**     2.Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution. 
**     3.The name of the author may not be used to endorse or promote products
**       derived from this software without specific prior written permission. 
** 		  
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR  ``AS IS'' AND ANY EXPRESS
** OR IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN  NO  EVENT  SHALL THE AUTHOR BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE
** GOODS OR SERVICES;  LOSS OF USE,  DATA,  OR  PROFITS;  OR BUSINESS
** INTERRUPTION)  HOWEVER  CAUSED  AND  ON  ANY  THEORY OF LIABILITY,
** WHETHER  IN  CONTRACT,   STRICT   LIABILITY,  OR  TORT  (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "rnd.h"
#include "./cast/cast.h"

UINT32 __rnd_seed[2]; /* Random Seed 2*32=64 */

/*
** randint(int n) - Produces a Random number from 0 to n-1 .
*/
UINT
randint(int n)
{
 return ( (UINT)( x917cast_rnd() % (UINT32)n ) );
}

/*
** ANSI X9.17 pseudorandom generator that uses CAST algorithm instead of DES
** m = 1
*/
UINT32
x917cast_rnd (void)
{
 struct timeval local_time; 
 UINT32 I[2] = {0L,0L};
 UINT32 I_plus_s[2] = {0L,0L};
 UINT32 Xi[2] = {0L,0L};
 UINT32 Xi_plus_I[2] = {0L,0L};
 cast_key ky;
 
/**********************************************************************
* ENCRYPTION KEY HEX : 0x000102030405060708090A0B0C0D0E0F (128-bit)   *
* YOU CAN CHANGE IT IF YOU WANT                                       *
**********************************************************************/
u8 ro_key[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
/**********************************************************************
* ENCRYPTION KEY HEX : 0x000102030405060708090A0B0C0D0E0F (128-bit)   *
* YOU CAN CHANGE IT IF YOU WANT                                       *
**********************************************************************/
 (void) gettimeofday (&local_time, 0);
 cast_setkey(&ky, (u8*)&ro_key[0], 16);
 cast_encrypt (&ky, (u8 *)&local_time, (u8*)&I[0]);            /* I=Ek(D), D-time  */
 I_plus_s[0] = I[0] ^ __rnd_seed[0];                           /* I0 (+) s0        */
 I_plus_s[1] = I[1] ^ __rnd_seed[1];                           /* I1 (+) s1        */
 cast_encrypt (&ky, (u8 *)&I_plus_s[0], (u8*)&Xi[0]);          /* Xi=Ek( I (+) s ) */
 Xi_plus_I[0] = Xi[0] ^ I[0];                                  /* Xi0 (+) I0       */
 Xi_plus_I[1] = Xi[1] ^ I[1];                                  /* Xi1 (+) I1       */
 cast_encrypt (&ky, (u8 *)&Xi_plus_I[0], (u8*)&__rnd_seed[0]); /* s=Ek( Xi (+) I ) */
 return (Xi[0]);
}

/*
** x917cast_setseed (UINT32 seed) - Initializes seed
** UINT32 seed - seed value
*/
void
x917cast_setseed (UINT32 seed)
{
 FILE * dr;
 UINT32 drs[2];

 if ( (dr = fopen(APG_DEVRANDOM, "r")) != NULL)
  {
   (void) fread( (void *)&drs[0], 8, 1, dr);
   __rnd_seed[0] = seed ^ drs[0];
   __rnd_seed[1] = seed ^ drs[1];
   (void) fclose(dr);
  }
 else if ( (dr = fopen(APG_DEVURANDOM, "r")) != NULL)
  {
   (void) fread( (void *)&drs[0], 8, 1, dr);
   __rnd_seed[0] = seed ^ drs[0];
   __rnd_seed[1] = seed ^ drs[1];
   (void) fclose(dr);
  }
 else
  {
#ifndef CLISERV
   fprintf(stderr,"CAN NOT USE /dev/random TO GENERATE RANDOM SEED\n");
   fprintf(stderr,"USEING LOCAL TIME FOR SEED GENERATION !!!\n");
   fflush(stderr);
#endif /* CLISERV */
   __rnd_seed[0] = seed;
   __rnd_seed[1] = seed;
  }
}

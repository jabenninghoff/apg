/***************************************************************************/
/* sha.h								   */
/*									   */
/* SHA-1 code header file.						   */
/* Taken from the public domain implementation by Peter C. Gutmann	   */
/* on 2 Sep 1992, modified by Carl Ellison to be SHA-1.			   */
/***************************************************************************/

#ifndef _SHA_H_
#define _SHA_H_

/* Define APG_LITTLE_ENDIAN if the machine is little-endian */

#define APG_LITTLE_ENDIAN

/* Useful defines/typedefs */

typedef unsigned char   BYTE ;
typedef unsigned long   LONG ;

/* The SHA block size and message digest sizes, in bytes */

#define SHA_BLOCKSIZE   64
#define SHA_DIGESTSIZE  20

/* The structure for storing SHA info */

typedef struct {
  LONG digest[ 5 ] ;            /* Message digest */
  LONG countLo, countHi ;       /* 64-bit bit count */
  LONG data[ 16 ] ;             /* SHA data buffer */
  LONG slop ;			/* # of bytes saved in data[] */
} apg_SHA_INFO ;

void apg_shaInit( apg_SHA_INFO *shaInfo ) ;
void apg_shaUpdate( apg_SHA_INFO *shaInfo, BYTE *buffer, int count ) ;
void apg_shaFinal( apg_SHA_INFO *shaInfo, BYTE hash[SHA_DIGESTSIZE] ) ;

#endif /* _SHA_H_ */

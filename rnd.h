/*
** Copyright (c) 1999, 2000, 2001, 2002, 2003
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

#ifndef APG_RND_H
#define APG_RND_H	1

#ifndef APG_OWN_TYPES_H
#include "owntypes.h"
#endif /* OWN_TYPES_H */

extern UINT32 __rnd_seed[2];

#define RND_MX 0x7FFFFFFF
#ifdef __OpenBSD__
#define APG_DEVRANDOM "/dev/arandom"
#else
#define APG_DEVRANDOM "/dev/random"
#endif /* __OpenBSD__ */
#define APG_DEVURANDOM "/dev/urandom"

extern void x917_setseed (UINT32 seed, int quiet);
extern UINT randint (int n);
#ifndef APG_USE_SHA
UINT32 x917cast_rnd (void);
#else /* APG_USE_SHA */
UINT32 x917sha1_rnd (void);
#endif /* APG_USE_SHA*/

#endif /* APG_RND_H */

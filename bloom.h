/*
** Copyright (c) 2001, 2002
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



/*
** Header file  for bloom filter algorithm implementation
*/
#ifndef APG_BLOOM_H
#define APG_BLOOM_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>

#include "sha/sha.h"

#define FSIZE_BIT(word_count) ((unsigned long int)(5.0/(1.0-pow( 0.84151068, 1.0/((double)word_count)))))
#define FSIZE_BYTE(word_count) ((((unsigned long int)(5.0/(1.0-pow( 0.84151068, 1.0/((double)word_count)))))/8)+1)

#define APGBFHDRSIZE      12

#define TRUE              1
#define FALSE             0

#define MAX_DICT_STRLEN   255
#define H_NUM   5

typedef unsigned long int      h_val; /* should be 32-bit */
typedef unsigned short int     flag;

struct apg_bf_hdr {
  char id[8];           /* ID          */
  unsigned long int fs; /* filter size */
};

extern int insert_word(char *word, FILE *file, h_val filter_size);
extern int check_word(char *word, FILE *file, h_val filter_size);
extern FILE * create_filter(char * f_name, unsigned long int n_words); 
extern FILE * open_filter(char * f_name, const char *mode); 
extern int close_filter(FILE *f_dsk);
extern h_val get_filtersize(FILE *f); 
extern h_val count_words(FILE *dict_file);

#endif /* APG_BLOOM_H */

/*
** Copyright (c) 2001, 2002, 2003
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
** C module for APG (Automated Password Generator)
** Bloom filter implementation.
** Functions:
**   insert_word    - insert word in the filter file
**   check_word     - check word
**   create_filter  - create initial(empty) filter file 
**   open_filter    - open APG Bloom filter file
**   get_filtersize - get APG Bloom filter size
**   get_filtermode - get APG Bloom filter mode
**   count_words    - count words in plain dictionary file
**   print_flt_info - print filter info
**=============================================================
**   hash2bit       - generates 5 values (should be 5 values of independent
**                    hash functions) from input string.
**   getbit         - get the bit value from file.
**   putbit         - put the bit in the file.
*/

#include "bloom.h"
#include "convert.h"

#define FSIZE_BIT(word_count) ((unsigned long int)(5.0/(1.0-pow( 0.84151068, 1.0/((double)word_count)))))
#define FSIZE_BYTE(word_count) ((((unsigned long int)(5.0/(1.0-pow( 0.84151068, 1.0/((double)word_count)))))/8)+1)

h_val * hash2bit(char * word, h_val *b);
int getbit(FILE * f, h_val bitnum);
int putbit(FILE * f, h_val bitnum);

#ifdef APGBFM
/*
** print_flt_info - print filter information
** INPUT:
**    FILE * filter - filter file descriptor
** OUTPUT:
**    int
**      0 - everything OK
**     -1 - something wrong
*/
int
print_flt_info(FILE * filter)
{
 struct apg_bf_hdr bf_hdr;
 int i = 0;

 if (fseek (filter, 0, SEEK_SET) == -1)
    return(-1);
 if (fread ( (void *)&bf_hdr, APGBFHDRSIZE, 1, filter) != 1)
    if (ferror (filter) != 0)
       return(-1);
 printf ("**************************************\n");
 printf ("** APGBFM: Bloom-filter information **\n");
 printf ("**************************************\n");
 printf ("Filter ID     : ");
 for (i=0; i < sizeof(bf_hdr.id); i++)
   printf ("%c", bf_hdr.id[i]);
 printf ("\n");
 printf ("Filter Version: ");
 printf ("%c.", bf_hdr.version[0]);
 printf ("%c.", bf_hdr.version[1]);
 printf ("%c", bf_hdr.version[2]);
 printf ("\n");
 printf ("Filter size   : %lu bits\n", (unsigned long int)bf_hdr.fs);
 printf ("Filter mode   : ");
 if (bf_hdr.mode == 0x00) printf ("PLAIN\n");
 if (bf_hdr.mode == 0x01) printf ("CASE_INSENSITIVE\n");
 printf ("**************************************\n");
 if (fseek (filter, 0, SEEK_SET) == -1)
    return(-1);
 return(0);
}
#endif /* APGBFM */

/*
** insert_word   - insert word in the filter file
** INPUT:
**    char *word        - word to incert in the filter
**    FILE *file        - filter file descriptor
**    h_val filter_size - filter size in bits
**    f_mode mode       - filter mode
** OUTPUT:
**    int
**      0 - everything OK
**     -1 - something wrong
*/
int
insert_word(char *word, FILE *file, h_val filter_size, f_mode mode)
{
 h_val h[H_NUM];
 int i = 0;

#ifdef APG_DEBUG
 fprintf (stdout, "DEBUG> insert_word: word to insert: %s\n", word);
 fflush (stdout);
#endif /* APG_DEBUG */

 if ((mode & BF_CASE_INSENSITIVE) > 0)
  {
   decapitalize(word);
#ifdef APG_DEBUG
   fprintf (stdout, "DEBUG> insert_word: decapitalized word: %s\n", word);
   fflush (stdout);
#endif /* APG_DEBUG */
  }

 hash2bit (word, &h[0]);
 for(i = 0; i < H_NUM; i++)
   if (putbit (file, h[i] % filter_size)== -1)
      return (-1);
 return(0);
}

/*
** check_word    - check word
** INPUT:
**    char *word        - word to check
**    FILE *file        - filter file descriptor
**    h_val filter_size - filter size in bits
**    f_mode            - filter mode
** OUTPUT:
**    int
**      0 - word is not in dictionary
**      1 - word is in dictionary
**     -1 - something wrong
*/
int
check_word(char *word, FILE *file,  h_val filter_size, f_mode mode)
{
 h_val h[H_NUM];
 int i = 0;
 char * tmp_word;
 
 if ((tmp_word = (char *) calloc(1,MAX_DICT_STRLEN)) == NULL)
    return(-1);
 (void)memcpy ((void *) tmp_word, (void *) word, strlen(word));

#ifdef APG_DEBUG
 fprintf (stdout, "DEBUG> check_word: word to check: %s\n", word);
 fflush (stdout);
#endif /* APG_DEBUG */
 if ((mode & BF_CASE_INSENSITIVE) > 0)
  {
   decapitalize(tmp_word);
#ifdef APG_DEBUG
   fprintf (stdout, "DEBUG> check_word: decapitalized word: %s\n", tmp_word);
   fflush (stdout);
#endif /* APG_DEBUG */
  }

 hash2bit (tmp_word, &h[0]);

 free ((void *)tmp_word);

 for(i = 0; i < H_NUM; i++)
  {
   switch(getbit(file, h[i] % filter_size))
     {
      case 0:
        return(0);
	break;
      case -1:
        return(-1);
	break;
      default:
        break;
     }
  }
 return (1);
}

/*
** open_filter - open APG Bloom filter file
** open filter file and check is this the real bloom filter file
** INPUT:
**    char * f_name - filter filename
**    const char *mode - "r" or "r+"
** OUTPUT:
**    FILE * - file pointer
**    NULL   - something wrong. 
*/
FILE *
open_filter(char * f_name, const char *mode)
{
 FILE *f;
 char etalon_bf_id[] = APGBF_ID;
 char etalon_bf_ver[] = APGBF_VERSION;
 struct apg_bf_hdr bf_hdr;
 
 if ((f = fopen (f_name, mode)) == NULL)
   return(NULL);
 if (fread ( (void *)&bf_hdr, APGBFHDRSIZE, 1, f) != 1)
    if (ferror (f) != 0)
       return(NULL);
 if ((bf_hdr.id[0] != etalon_bf_id[0]) || (bf_hdr.id[1] != etalon_bf_id[1]) ||
     (bf_hdr.id[2] != etalon_bf_id[2]) || (bf_hdr.id[3] != etalon_bf_id[3]) ||
     (bf_hdr.id[4] != etalon_bf_id[4]) )
        return (NULL);
 if ((bf_hdr.version[0] != etalon_bf_ver[0]) ||
     (bf_hdr.version[1] != etalon_bf_ver[1]) ||
     (bf_hdr.version[2] != etalon_bf_ver[2]) )
        return (NULL);
 else
  {
   if (fseek (f, 0, SEEK_SET) == -1)
      return(NULL);
   return(f);
  }
}

/*
** close_filter - close APG Bloom filter file
** close filter file
** INPUT:
**    FILE * f_dsk - filter file pointer
** OUTPUT:
**    int - same as fclose() return value
*/
int
close_filter(FILE *f_dsk)
{
 return(fclose(f_dsk));
}

/*
** get_filtersize - get APG Bloom filter size
** INPUT:
**    FILE *f - filter file descriptor
** OUTPUT:
**    h_val - size of APG Bloom filter.
**    0     - something wrong 
*/
h_val
get_filtersize(FILE * f)
{
 struct apg_bf_hdr bf_hdr;
 if (fseek (f, 0, SEEK_SET) == -1)
    return(0);
 if (fread ( (void *)&bf_hdr, APGBFHDRSIZE, 1, f) != 1)
    if (ferror (f) != 0)
       return(0);
 if (fseek (f, 0, SEEK_SET) == -1)
    return(0);
 return( (h_val)bf_hdr.fs);
} 

/*
** get_filtermode - get APG Bloom filter mode
** INPUT:
**    FILE *f - filter file descriptor
** OUTPUT:
**    f_mode - APG Bloom filter mode.
**    0      - something wrong 
*/
f_mode
get_filtermode(FILE *f)
{
 struct apg_bf_hdr bf_hdr;
 if (fseek (f, 0, SEEK_SET) == -1)
    return(0);
 if (fread ( (void *)&bf_hdr, APGBFHDRSIZE, 1, f) != 1)
    if (ferror (f) != 0)
       return(0);
 if (fseek (f, 0, SEEK_SET) == -1)
    return(0);
 return( (f_mode)bf_hdr.mode);
}

/*
** create_filter - create initial(empty) filter file 
** 5 - number of hash functions
** 0.0001 (0.01%) - probability of false positives
** INPUT:
**    char * f_name - filter filename
**    unsigned long int n_words - number of words in filter
** OUTPUT:
**    FILE * - filter file descriptor
**    NULL   - something wrong
** NOTES:
**   n - number of words in the filter
**   N - size of filter(?)
**
**   a=(1-(4/N))^n
**   0.0001=(1-a)^5 ==> 1-a=0.15849... ==> a=0.84151068 ==>
**   0.84151068=(1-(5/N))^n ==> 0.84151068^(1/n)=1-(5/N) ==>
**
**   N=5/(1-[0.84151068^(1/n)])
**
**               5
**   N = -----------------
**                     1/n
**       1 - 0.84151068
*/
FILE *
create_filter(char * f_name, unsigned long int n_words, f_mode mode) 
{
 FILE *f;
 char zero = 0x00;
 long int i = 0L;
 char etalon_bf_id[] = APGBF_ID;
 char etalon_bf_ver[] = APGBF_VERSION;
 struct apg_bf_hdr bf_hdr;

 bf_hdr.id[0] = etalon_bf_id[0];
 bf_hdr.id[1] = etalon_bf_id[1];
 bf_hdr.id[2] = etalon_bf_id[2];
 bf_hdr.id[3] = etalon_bf_id[3];
 bf_hdr.id[4] = etalon_bf_id[4];
 
 bf_hdr.version[0] = etalon_bf_ver[0];
 bf_hdr.version[1] = etalon_bf_ver[1];
 bf_hdr.version[2] = etalon_bf_ver[2];

 bf_hdr.fs = FSIZE_BIT(n_words);
 
 bf_hdr.mode = mode;

 if ((f = fopen (f_name, "w+")) == NULL)
   return(NULL);

 if (fwrite ( (void *)&bf_hdr, APGBFHDRSIZE, 1, f) != 1)
    if (ferror (f) != 0)
       return(NULL);

 for (i = 0; i < FSIZE_BYTE(n_words); i++)
   if ( fwrite ( (void *)&zero, 1, 1, f) < 1)
      if (ferror (f) != 0)
         return(NULL);
 if (fseek (f, 0, SEEK_SET) == -1)
    return (NULL);

 return (f);
}

/*
** count_words - count words in plain dictionary file
** INPUT:
**    FILE *dict_file -plain dicionary file descriptor
** OUTPUT:
**    h_val - amount of words in dictionary file
**    0     - something wrong
*/
h_val
count_words(FILE *dict_file)
{
 h_val i = 0L; /* word counter */
 char *string; /* temp string holder */
 char *tmp;    /* just tmp char pointer and nothing more it has no memory assigned */
 if ((string = (char *) calloc(1,MAX_DICT_STRLEN)) == NULL)
    return(0);
 while ((fgets(string, MAX_DICT_STRLEN, dict_file) != NULL))
  {
   tmp = (char *)strtok (string," \t\n\0");
   if (tmp != NULL) i++;
  }
 if (fseek (dict_file, 0, SEEK_SET) == -1)
    return (0);
 free ((void *) string);
 return (i);
}

/*
** hash2bit      - generates 4 values (should be 4 values of independent
**                 hash functions) from input string.
** INPUT:
**    char *word - word to hash
**    h_val *b   - pointer to bitnumber array
** OUTPUT:
**    h_val * - pointer to bitnumber array
*/
h_val *
hash2bit(char * word, h_val *b)
{
 apg_SHA_INFO context;
 BYTE cs[SHA_DIGESTSIZE];

 apg_shaInit (&context);
 apg_shaUpdate (&context, (BYTE *)word, strlen(word));
 apg_shaFinal (&context, cs);
 return ( (h_val *)memcpy( (void *)b, (void *)&cs[0], SHA_DIGESTSIZE));
}

/*
** getbit        - get the bit value from file.
** INPUT:
**    FILE *f - file descriptor
**    h_val bitnum - bit number
** OUTPUT:
**    int
**      0,1 - bit value
**     -1   - something wrong
*/
int
getbit(FILE * f, h_val bitnum)
{
 long int bytenum = 0L;
 short int bit_in_byte = 0;
 unsigned char read_byte = 0x00;
 unsigned char test_byte = 0x01;
 int i = 0;
 
 bit_in_byte = bitnum % 8;
 bytenum =  APGBFHDRSIZE + (bitnum/8);
 if (fseek (f, bytenum, SEEK_SET) == -1)
    return(-1);
 if (fread ((void*)&read_byte,1,1,f) < 1)
    if (ferror(f) != 0)
       return (-1);
 for (i=0;i < bit_in_byte;i++)
   test_byte = test_byte*2;
 if ((read_byte & test_byte) > 0) return (1);
 else return (0);
}

/*
** putbit        - put the bit in the file.
** INPUT:
**    FILE *f - file descriptor
**    h_val bitnum - bit number
** OUTPUT:
**    int
**      0 - everything OK
**     -1 - something wrong
*/
int
putbit(FILE * f, h_val bitnum)
{
 long int bytenum = 0L;
 short int bit_in_byte = 0;
 unsigned char read_byte = 0x00;
 unsigned char test_byte = 0x01;
 int i = 0;
 bit_in_byte = bitnum % 8;
 bytenum =  APGBFHDRSIZE + (bitnum/8);
 if (fseek (f, bytenum, SEEK_SET) == -1)
    return(-1);
 if (fread ((void*)&read_byte,1,1,f) < 1)
    if (ferror(f) != 0)
       return (-1);
 for (i=0;i < bit_in_byte;i++)
   test_byte = test_byte*2;
 read_byte = read_byte | test_byte;
 if (fseek (f, bytenum, SEEK_SET) == -1)
    return(-1);
 if (fwrite ((void*)&read_byte,1,1,f) < 1)
    if (ferror(f) != 0)
       return (-1);
 return (0);
}
/* END OF bloom.c file */

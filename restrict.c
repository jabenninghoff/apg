/*
** Copyright (c) 1999, 2000, 2001, 2002
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
** restrict.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "restrict.h"
extern struct sym smbl[94];
/*
** check_pass() - routine that checks if password exist in dictionary
** INPUT:
**   char * - password to check.
**   char * - dictionary filename.
** OUTPUT:
**   int
**    -1 - error 
**     1 - password exist in dictionary
**     0 - password does not exist in dictionary
** NOTES:
**   none.
*/
int
check_pass(char *pass, char *dict)
{
 FILE *dct;
 char *string;
 string = (char *) calloc(1,MAX_DICT_STRING_SIZE);
 
 /*
 ** Open dict file an report of error
 */
 if ( (dct = fopen(dict,"r")) == NULL)
   return(-1);

 while ((fgets(string, MAX_DICT_STRING_SIZE, dct) != NULL))
  {
   string = strtok (string," \t\n\0");
   if(strlen(string) != strlen(pass)) continue;
   else if (strncmp(string, pass, strlen(pass)) == 0)
    {
     free ( (void *)string);
     fclose (dct);
     return (1);
    }
  }
 free ( (void *)string);
 fclose (dct);
 return (0); 
}

/*
** bloom_check_pass() - routine that checks if password exist in dictionary
** using Bloom filter.
** INPUT:
**   char * - password to check.
**   char * - bloom-filter filename.
** OUTPUT:
**   int
**    -1 - error 
**     1 - password exist in dictionary
**     0 - password does not exist in dictionary
** NOTES:
**   none.
*/
int
bloom_check_pass (char *word, char *filter)
{
 int ret = 0;
 FILE *f_filter;
 h_val filter_size = 0L;
 if ( (f_filter = open_filter(filter,"r")) == NULL)
    return(-1);
 filter_size = get_filtersize(f_filter);
 ret = check_word (word, f_filter, filter_size);
 close_filter(f_filter);
 return(ret);
}

/*
** filter_check_pass() - routine that checks password against filter string
**
** INPUT:
**   char * - password to check.
**   char * - bloom-filter filename.
** OUTPUT:
**   int
**    -1 - error 
**     1 - password do not pass the filter
**     0 - password pass the filter
** NOTES:
**   none.
*/

int
filter_check_pass(const char * word, unsigned int cond)
{
 int i = 0;
 int sl_ret = 0;
 int cl_ret = 0;
 int nb_ret = 0;
 int ss_ret = 0;
 if ((cond & S_SS) > 0)
    for (i=0; i < 94; i++)
       if ((smbl[i].type & S_SS) > 0)
          if ((strchr(word,smbl[i].ch)) != NULL)
	     ss_ret = 1;
 i = 0;
 if ((cond & S_SL) > 0)
    for (i=0; i < 94; i++)
       if ((smbl[i].type & S_SL) > 0)
          if ((strchr(word,smbl[i].ch)) != NULL)
	     sl_ret = 1;
 i = 0;
 if ((cond & S_CL) > 0)
    for (i=0; i < 94; i++)
       if ((smbl[i].type & S_CL) > 0)
          if ((strchr(word,smbl[i].ch)) != NULL)
	     cl_ret = 1;
 i = 0;
 if ((cond & S_NB) > 0)
    for (i=0; i < 94; i++)
       if ((smbl[i].type & S_NB) > 0)
          if ((strchr(word,smbl[i].ch)) != NULL)
	     nb_ret = 1;
 if (((cond & S_SS) > 0) &&(ss_ret != 1)) return (1);
 if (((cond & S_SL) > 0) &&(sl_ret != 1)) return (1);
 if (((cond & S_CL) > 0) &&(cl_ret != 1)) return (1);
 if (((cond & S_NB) > 0) &&(nb_ret != 1)) return (1);

 return(0);
}

/*
** set_exclude_list() - set up character list that should
** be excluded from password generation process
**
** INPUT:
**   char * - string of characters.
** OUTPUT:
**   int - return code
**    0 - OK
**   -1 - char_string is too long (max 93) 
** NOTES:
**   none.
*/
int set_exclude_list(const char * char_string)
{
 int i = 0;
 if (strlen(char_string) > 93)
  return(-1);
 for(i=0; i < 94; i++)
  if ((strchr(char_string, smbl[i].ch)) != NULL)
    smbl[i].type = smbl[i].type | S_RS;
 return(0);
}

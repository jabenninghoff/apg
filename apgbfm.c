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


#include <stdlib.h>
#include <unistd.h>
#include "bloom.h"
#include "errs.h"
#include "getopt.h"


#define VERSION    "2.2.3"


#define FOUND "FOUND"
#define NOT_FOUND "NOT FOUND"
/*
#define FOUND "YES"
#define NOT_FOUND "NO"
*/

int main (int argc, char *argv[]);
void print_help(void);
void checkopt(char *opt);
void print_filter_info(char * filter);

int
main (int argc, char *argv[])
{
 int option = 0;
 
 char *dictfile;         /* dictionary filename                 */
 FILE *f_dictfile;       /* dictionary file descriptor          */

 char *filter;           /* filter file name                    */
 FILE *f_filter;         /* filter file descriptor              */

 char *word;             /* word to add or check                */
 char *tmp;              /* just tmp char pointer               */
 h_val wc = 0L;          /* amount of words to build dictionaty */
 h_val filter_size =0L;  /* filter size in bits                 */
 int dummy_test = 0;     /* variable to make dummy test for     */
                         /* options correctness                 */
 h_val i = 0L;           /* counter                             */
 f_mode flt_mode = 0x00; /* filter mode                         */

 /* flags */
 flag add_word_flag         = FALSE; /* -a */
 flag add_file_flag         = FALSE; /* -A */
 flag check_word_flag       = FALSE; /* -c */
 flag check_file_flag       = FALSE; /* -C */
 flag new_flag              = FALSE; /* -n */
 flag new_from_dict_flag    = FALSE; /* -d */
 flag filter_flag           = FALSE; /* -f */
 flag silent_flag           = FALSE; /* -q */
 flag case_insensitive_flag = FALSE; /* -q */
 /* end of flags section */

 /* Analize options */
 if (argc < 2) 
    {
     print_help();
     exit(-1);
    }
 
 while ((option = apg_getopt (argc, argv, "a:A:c:C:n:d:f:i:hvqs")) != -1)
   {
    switch(option)
      {
       case 'a':
         word = apg_optarg;
	 add_word_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'A':
         dictfile = apg_optarg;
	 add_file_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'c':
         word = apg_optarg;
	 check_word_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'C':
         dictfile = apg_optarg;
	 check_file_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'n':
         checkopt(apg_optarg);
         wc = atoi(apg_optarg);
	 new_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'd':
         dictfile = apg_optarg;
	 new_from_dict_flag = TRUE;
	 dummy_test = dummy_test + 2;
         break;
       case 'f':
         filter = apg_optarg;
	 filter_flag = TRUE;
	 dummy_test = dummy_test + 1;
         break;
       case 'h':
         print_help();
	 return (0);                                                               
       case 'v':
         printf ("APG Bloom filter management programm");
	 printf ("\nversion %s", VERSION);
	 printf ("\nCopyright (c) 2001, 2002, 2003 Adel I. Mirzazhanov\n");
	 return (0);
       case 'i':
	 print_filter_info(apg_optarg);
	 return (0);
       case 'q':
         silent_flag = TRUE;
	 break;
       case 's':
         flt_mode = flt_mode | BF_CASE_INSENSITIVE;
         case_insensitive_flag = TRUE;
	 break;
       default:
         print_help();
	 exit(-1);
      }
   }
 if (filter_flag != TRUE) err_app_fatal ("apg", "-f option is required");
 if (dummy_test != 3) err_app_fatal ("apg", "too many options");
 /* Main part */
 /* At this point we can be sure that all options a correct */
 if (add_word_flag == TRUE) /* -a word */
    {
     if ( (f_filter = open_filter(filter, "r+")) == NULL)
        err_sys_fatal("open_filter");
     filter_size = get_filtersize(f_filter);
     flt_mode    = get_filtermode(f_filter);
     if (filter_size == 0) err_sys_fatal("get_filtersize");
     if ( insert_word (word, f_filter, filter_size, flt_mode) == -1)
	    err_sys_fatal("insert_word");
     if (silent_flag != TRUE)
        printf ("Word %s added\n",word);
     return (0);
    }
 if (add_file_flag == TRUE) /* -A dictfile */
    {
     word = (char *) calloc(1,MAX_DICT_STRLEN);
     if ( (f_dictfile = fopen(dictfile,"r")) == NULL)
        err_sys_fatal("fopen");
     if( (f_filter = open_filter(filter,"r+")) == NULL)
        err_sys_fatal("open_filter");
     filter_size = get_filtersize(f_filter);
     flt_mode    = get_filtermode(f_filter);
     if (filter_size == 0) err_sys_fatal("get_filtersize");
     while ((fgets(word, MAX_DICT_STRLEN, f_dictfile) != NULL))
        {
         tmp = (char *)strtok (word," \t\n\0");
	 if( tmp != NULL)
	   word = tmp;
	 else continue;
         if ( insert_word (word, f_filter, filter_size, flt_mode) == -1)
	    err_sys_fatal("insert_word");
	 i++;
	 if (silent_flag != TRUE)
	    {
             if ( i % 100 == 0)
	        {
	         fprintf (stdout,".");
	         fflush (stdout);
	        }
	    }
         (void)memset((void *)word, 0, MAX_DICT_STRLEN);
        }
     if (silent_flag != TRUE) printf ("\n");
     free ( (void *)word);
     fclose (f_dictfile);
     close_filter (f_filter);
     return (0);
    }
 if (check_word_flag == TRUE) /* -c word */
    {
     if ( (f_filter = open_filter(filter, "r")) == NULL)
        err_sys_fatal("open_filter");
     filter_size = get_filtersize(f_filter);
     flt_mode    = get_filtermode(f_filter);

     if (filter_size == 0) err_sys_fatal("get_filtersize");
     switch(check_word (word, f_filter, filter_size, flt_mode))
	{
	 case -1:
	    err_sys_fatal("check_word");
	    break;
         case 1:
	    printf ("%s: %s \n",word, FOUND);
	    break;
         case 0:
	    printf ("%s: %s\n",word, NOT_FOUND);
	    break;
	}
     return (0);
    }
 if (check_file_flag == TRUE) /* -C dictfile */
    {
     word = (char *) calloc(1,MAX_DICT_STRLEN);
     if ( (f_dictfile = fopen(dictfile,"r")) == NULL)
        err_sys_fatal("fopen");
     wc = count_words (f_dictfile);
     if (wc == 0) err_sys_fatal("count_words");
     if( (f_filter = open_filter(filter, "r")) == NULL)
        err_sys_fatal("open_filter");
     filter_size = get_filtersize(f_filter);
     flt_mode    = get_filtermode(f_filter);

     if (filter_size == 0) err_sys_fatal("get_filtersize");
     while ((fgets(word, MAX_DICT_STRLEN, f_dictfile) != NULL))
        {
         tmp = (char *)strtok (word," \t\n\0");
	 if( tmp != NULL)
	   word = tmp;
	 else continue;
         switch(check_word (word, f_filter, filter_size, flt_mode))
	    {
	     case -1:
	        err_sys_fatal("check_word");
	        break;
             case 1:
	        printf ("%s: %s\n",word, FOUND);
	        break;
             case 0:
	        printf ("%s: %s\n",word, NOT_FOUND);
	        break;
	    }
         (void)memset((void *)word, 0, MAX_DICT_STRLEN);
        }
     free ( (void *)word);
     fclose (f_dictfile);
     close_filter (f_filter);
     return (0);
    }
 if (new_flag == TRUE) /* -n nwords */
    {
     if ((f_filter = create_filter(filter, wc, flt_mode)) == NULL)
         err_sys_fatal("create_filter");
     close_filter(f_filter);
     return (0);
    }
 if (new_from_dict_flag == TRUE) /* -d dictfile */
    {
     word = (char *) calloc(1,MAX_DICT_STRLEN);
     if ( (f_dictfile = fopen(dictfile,"r")) == NULL)
        err_sys_fatal("fopen");
     if (silent_flag != TRUE)
        {
         fprintf (stdout,"Counting words in dictionary. Please wait...\n");
         fflush (stdout);
	}
     wc = count_words (f_dictfile);
     if (wc == 0) err_sys_fatal("count_words");
     if( (f_filter = create_filter(filter, wc, flt_mode)) == NULL)
        err_sys_fatal("create_filter");
     filter_size = get_filtersize(f_filter);
     if (filter_size == 0) err_sys_fatal("get_filtersize");
     while ((fgets(word, MAX_DICT_STRLEN, f_dictfile) != NULL))
        {
         tmp = (char *)strtok (word," \t\n\0");
	 if( tmp != NULL)
	  {
	   word = tmp;
	  }
	 else
	  {
	   continue;
	  }
         if ( insert_word (word, f_filter, filter_size, flt_mode) == -1)
	    err_sys_fatal("insert_word");
	 i++;
	 if (silent_flag != TRUE)
	    {
             if ( i % 100 == 0)
	        {
                 fprintf (stdout, ".");
	         fflush (stdout);
	        }
	    }
         (void)memset((void *)word, 0, MAX_DICT_STRLEN);
        }
     if (silent_flag != TRUE) printf ("\n");
     free ( (void *)word);
     fclose (f_dictfile);
     close_filter (f_filter);
     return (0);
    }
return (0);
}
/*
** print_help()  - prints short help info
** INPUT:
**   none.
** OUTPUT:
**   prints help info to the stdout.
** NOTES:
**   none.
*/
void
print_help(void)
{
 printf ("\napgbfm   APG Bloom filter management\n");
 printf ("         Copyright (c) 2001 Adel I. Mirzazhanov\n");
 printf ("\napgbfm   -f filter < [-a word] | [-A dictfile] | [-n numofwords] |\n");
 printf ("                     [-c word] | [-C dictfile] | [-d dictfile] > [-s]\n");
 printf ("apgbfm   -i filter\n");
 printf ("apgbfm   [-v] [-h]\n\n");
 printf ("-a word        add word to filter\n");
 printf ("-A dictfile    add words from dictfile to filter\n");
 printf ("-c word        check word against filter\n");
 printf ("-C dictfile    check dictfile against filter\n");
 printf ("-n numofwords  create new empty filter\n");
 printf ("-d dictfile    create new filter and add all words from dictfile\n");
 printf ("-f filtername  use filtername as the name for filter\n");
 printf ("-q             quiet mode (do not print dots for -A and -d)\n");
 printf ("-s             create case insentive filter\n");
 printf ("-i filter      print filter information\n");
 printf ("-v             print version information\n");
 printf ("-h             print  help (this screen)\n");
}

/*
** checkopt() - check options
** INPUT:
**   char * - option string.
** OUTPUT:
**   none.
** NOTES:
**   checks only is the option string numeral.
*/
void
checkopt(char *opt)
{
 int i;

 for(i=0; i < strlen(opt);i++)
  if(opt[i] != '0' && opt[i] != '1' && opt[i] != '2' && opt[i] != '3' &&
     opt[i] != '4' && opt[i] != '5' && opt[i] != '6' && opt[i] != '7' &&
     opt[i] != '8' && opt[i] != '9')
      err_app_fatal ("checkopt", "wrong option format");
}

/*
** print_filter_info(char * filter) - print filter information
** INPUT:
**   char * - filter file name.
** OUTPUT:
**   none.
** NOTES:
**   none.
*/
void
print_filter_info(char * filter)
{
FILE * f_filter;

if ( (f_filter = open_filter(filter, "r")) == NULL)
   err_sys_fatal("open_filter");
if (( print_flt_info(f_filter)) == -1)
   err_sys_fatal("print_flt_info");
close_filter(f_filter);
}

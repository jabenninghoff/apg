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

/*
** Main Module of apg programm
*/
#include <stdio.h>
#include <stdlib.h>
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(__WIN32__)
#include <strings.h>
#endif
#include <string.h>
#include <time.h>

#ifndef APG_USE_SHA
#define APG_VERSION "2.2.3 (PRNG: X9.17/CAST)"
#else /* APG_USE_SHA */
#define APG_VERSION "2.2.3 (PRNG: X9.17/SHA-1)"
#endif /* APG_USE_SHA */

#ifdef __NetBSD__
#include <unistd.h>
#endif

#if defined(__sun) || defined(sun) || defined(linux) || defined(__linux) || defined(__linux__)
#include <crypt.h>
#endif

#define MAX_MODE_LENGTH   4
#define DEFAULT_MIN_PASS_LEN 8
#define DEFAULT_MAX_PASS_LEN 10
#define DEFAULT_NUM_OF_PASS 6

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#ifndef __NetBSD__
#include <unistd.h>
#endif

#ifdef __CYGWIN__
#undef APG_USE_CRYPT
#endif /* __CYGWIN__ */

#ifdef CLISERV
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#define MAXSOCKADDDR 128
#endif /* CLISERV */

#include "owntypes.h"
#include "pronpass.h"
#include "randpass.h"
#include "restrict.h"
#include "bloom.h"
#include "rnd.h"
#include "errs.h"
#include "getopt.h"
#include "convert.h"

 struct pass_m {
        unsigned int pass;	         /* password generation mode        */
        unsigned int filter;	         /* password generation mode        */
	};
#ifndef CLISERV
UINT32 get_user_seq (void);
UINT32 com_line_user_seq (char * seq);
char *crypt_passstring (const char *p);
void print_help (void);
#endif /* CLISERV */

int main (int argc, char *argv[]);
void checkopt(char *opt);
int construct_mode(char *str_mode, struct pass_m * mde);

/*
** main()
*/
int
main (int argc, char *argv[])
{
 int i = 0;
 int restrict_res = 0;
 
 char *pass_string;
 char *hyph_pass_string;
 time_t tme;
 
 
 int option = 0;                         /* programm option                 */

 int algorithm = 0;                      /* algorithm for generation        */
 int restrictions_present = FALSE;       /* restrictions flag               */
 int plain_restrictions_present = FALSE; /* dictionary restrictions_flag    */
 int bloom_restrict_present = FALSE;     /* bloom filter restrictions flag  */
 int paranoid_bloom_restrict_present = FALSE;     /* paranoid bloom filter restrictions flag  */
 int filter_restrict_present = FALSE;    /* filter restrictions flag        */
 int exclude_list_present = FALSE;       /* exclude list present            */
 int quiet_present = FALSE;              /* quiet mode flag                 */
 int hyph_req_present = FALSE;           /* Request to print hyphenated password              */
 char *restrictions_file;                /* dictionary file name            */
 char *plain_restrictions_file;          /* dictionary file name            */
 struct pass_m mode;
 unsigned int pass_mode_present = FALSE; /* password generation mode flag   */
 USHORT min_pass_length = DEFAULT_MIN_PASS_LEN;             /* min password length             */
 USHORT max_pass_length = DEFAULT_MAX_PASS_LEN;             /* max password length             */
 USHORT min_substr_len = 0;              /* min substring length to check if
                                         ** paranoid check is used          */
 int number_of_pass = DEFAULT_NUM_OF_PASS;                 /* number of passwords to generate */
 UINT32 user_defined_seed = 0L;          /* user defined random seed        */
 int user_defined_seed_present = FALSE;  /* user defined random seed flag   */
 char *str_mode;                         /* string mode pointer             */
#ifndef CLISERV
 char *com_line_seq;
 char *spell_pass_string;
 int spell_present = FALSE;              /* spell password mode flag        */
 unsigned int delimiter_flag_present = FALSE;
#ifdef APG_USE_CRYPT
 char *crypt_string;
 unsigned int show_crypt_text = FALSE;   /* display crypt(3)'d text flag    */
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
#ifdef CLISERV
#if defined(sgi) || defined(__APPLE__) || defined(__QNX__) /* Thanks to Andrew J. Caird */
 typedef unsigned int socklen_t;
#endif
 socklen_t len;
 struct sockaddr_in *cliaddr;
 char delim[2]={0x0d,0x0a};
 char *out_pass;
 char *peer_ip_unknown = "UNKNOWN";
 char *peer_ip;

 openlog(argv[0], LOG_PID, LOG_DAEMON);
 cliaddr = (struct sockaddr_in *)calloc(1,MAXSOCKADDDR);
 len = MAXSOCKADDDR;
 if( getpeername(0, (struct sockaddr *)cliaddr, &len) != 0)
  {
   err_sys("getpeername");
   peer_ip = peer_ip_unknown;
  }
 else
  {
   peer_ip = inet_ntoa(cliaddr->sin_addr);
  }
 syslog (LOG_INFO, "password generation request from %s.%d\n", peer_ip, htons(cliaddr->sin_port));
#endif /* CLISERV */

 /*
 ** Analize options
 */
#ifndef CLISERV
#ifdef APG_USE_CRYPT
 while ((option = apg_getopt (argc, argv, "M:E:a:r:b:p:sdc:n:m:x:htvylq")) != -1)
#else /* APG_USE_CRYPT */
 while ((option = apg_getopt (argc, argv, "M:E:a:r:b:p:sdc:n:m:x:htvlq")) != -1)
#endif /* APG_USE_CRYPT */
#else /* CLISERV */
 while ((option = apg_getopt (argc, argv, "M:E:a:r:b:p:n:m:x:vt")) != -1)
#endif /* CLISERV */
  {
   switch (option)
    {
     case 'M': /* mode parameter */
      str_mode = apg_optarg;
      if( (construct_mode(str_mode,&mode)) == -1)
         err_app_fatal("construct_mode","wrong parameter");
      pass_mode_present = TRUE;
      if(mode.filter != 0)
        {
         filter_restrict_present = TRUE;
	 restrictions_present = TRUE;
	}
      break;
     case 'E': /* exclude char */
      if(set_exclude_list(apg_optarg)==-1)
         err_app_fatal("set_exclude_list","string is too long (max. 93 characters)");
      exclude_list_present = TRUE;
      break;
     case 'a': /* algorithm specification */
      checkopt(apg_optarg);
      algorithm = atoi (apg_optarg);
      break;
     case 'r': /* restrictions */
      restrictions_present = TRUE;
      plain_restrictions_present = TRUE;
      plain_restrictions_file = apg_optarg;
      break;
     case 'b': /* bloom restrictions */
      restrictions_present = TRUE;
      bloom_restrict_present = TRUE;
      restrictions_file = apg_optarg;
      break;
     case 'p': /* paranoid bloom restrictions */
      checkopt(apg_optarg);
      min_substr_len = atoi (apg_optarg);
      paranoid_bloom_restrict_present = TRUE;
      break;
#ifndef CLISERV
     case 'l':
      spell_present = TRUE;
      break;
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(__WIN32__)
     case 's': /* user random seed required */
      user_defined_seed = get_user_seq ();
      user_defined_seed_present = TRUE;
      break;
#endif /* WIN32 */
     case 'c': /* user random seed given in command line */
      com_line_seq = apg_optarg;
      user_defined_seed = com_line_user_seq (com_line_seq);
      user_defined_seed_present = TRUE;
      break;
     case 'd': /* no delimiters option */
      delimiter_flag_present = TRUE;
      break;
     case 'q': /* quiet mode */
      quiet_present = TRUE;
      break;
#ifdef APG_USE_CRYPT
     case 'y': /* display crypt(3)'d text next to passwords */
      show_crypt_text = TRUE;
      break;                                                                  
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     case 'n': /* number of password specification */
      checkopt(apg_optarg);
      number_of_pass = atoi (apg_optarg);
      break;
     case 'm': /* min password length */
      checkopt(apg_optarg);
      min_pass_length = (USHORT) atoi (apg_optarg);
      break;
     case 'x': /* max password length */
      checkopt(apg_optarg);
      max_pass_length = (USHORT) atoi (apg_optarg);
      break;
     case 't': /* request to print hyphenated password */
      hyph_req_present = TRUE;
      break;
#ifndef CLISERV
     case 'h': /* print help */
      print_help ();
      return (0);
#endif /* CLISERV */
     case 'v': /* print version */
      printf ("APG (Automated Password Generator)");
      printf ("\nversion %s", APG_VERSION);
      printf ("\nCopyright (c) 1999, 2000, 2001, 2002, 2003 Adel I. Mirzazhanov\n");
      return (0);
     default: /* print help end exit */
#ifndef CLISERV
      print_help ();
#endif /* CLISERV */
      exit (-1);
    }
  }
 if (pass_mode_present != TRUE)
    mode.pass = S_SS | S_NB | S_CL | S_SL;
 if (exclude_list_present == TRUE)
    mode.pass = mode.pass | S_RS;
 if( (tme = time(NULL)) == ( (time_t)-1))
    err_sys("time");
 if (user_defined_seed_present != TRUE)
    x917_setseed ( (UINT32)tme, quiet_present);
 else
    x917_setseed (user_defined_seed ^ (UINT32)tme, quiet_present);
 if (min_pass_length > max_pass_length)
    max_pass_length = min_pass_length;
 /* main code section */
 
 /*
 ** reserv space for password and hyphenated password and report of errors
 ** 18 because the maximum length of element for hyphenated password is 17
 */
 if ( (pass_string = (char *)calloc (1, (size_t)(max_pass_length + 1)))==NULL ||
      (hyph_pass_string = (char *)calloc (1, (size_t)(max_pass_length*18)))==NULL)
      err_sys_fatal("calloc");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
 if (show_crypt_text == TRUE)
   if ((crypt_string = (char *)calloc (1, 255))==NULL)      
      err_sys_fatal("calloc");
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
#ifdef CLISERV
 if ( (out_pass = (char *)calloc(1, (size_t)(max_pass_length*19 + 4))) == NULL)
      err_sys_fatal("calloc");
#endif /* CLISERV */
 /*
 ** generate required amount of passwords using specified algorithm
 ** and check for restrictions if specified with command line parameters
 */
 while (i < number_of_pass)
  {
   if (algorithm == 0)
    {
     if (gen_pron_pass(pass_string, hyph_pass_string,
                       min_pass_length, max_pass_length, mode.pass) == -1)
        err_app_fatal("apg","wrong password length parameter");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
     if (show_crypt_text == TRUE)
         (void) memcpy ((void *)crypt_string,
	                (void *)crypt_passstring (pass_string), 255);
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     /***************************************
     ** ALGORITHM = 0 RESTRICTIONS = PRESENT
     ****************************************/
     if (restrictions_present == TRUE)
       {
        /* Filter check */
        if (filter_restrict_present == TRUE)
	  restrict_res = filter_check_pass(pass_string, mode.filter);
	/* Bloom-filter check */
	if (restrict_res == 0)
	 {
	  if (bloom_restrict_present == TRUE)
	   {
	    if(paranoid_bloom_restrict_present != TRUE)
              restrict_res = bloom_check_pass(pass_string, restrictions_file);
	    else
	      restrict_res = paranoid_bloom_check_pass(pass_string, restrictions_file, min_substr_len);
	   }
	 }
	 /* Dictionary check */
	 if (restrict_res == 0)
	  if (plain_restrictions_present == TRUE)
            restrict_res = check_pass(pass_string, plain_restrictions_file);


        switch (restrict_res)
	  {
	  case 0:
#ifndef CLISERV
            fprintf (stdout, "%s", pass_string);
            if (hyph_req_present == TRUE)
	      fprintf (stdout, " (%s)", hyph_pass_string);
#ifdef APG_USE_CRYPT
            if (show_crypt_text == TRUE)
	      fprintf (stdout, " %s", crypt_string);
#endif /* APG_USE_CRYPT */
	    if (spell_present == TRUE)
	     {
	      spell_pass_string = spell_word(pass_string, spell_pass_string);
	      fprintf (stdout, (" %s"), spell_pass_string);
	      free((void*)spell_pass_string);
	     }
	    if ( delimiter_flag_present == FALSE )
	       fprintf (stdout, "\n");
	    fflush (stdout);
#else /* CLISERV */
            if (hyph_req_present == TRUE)
             snprintf(out_pass, max_pass_length*19 + 4, "%s (%s)", pass_string, hyph_pass_string);
	    else
             snprintf(out_pass, max_pass_length*19 + 4, "%s", pass_string);	    
	    write (0, (void*) out_pass, strlen(out_pass));
	    write (0, (void*)&delim[0],2);
#endif /* CLISERV */
	    i++;
	    break;
	  case 1:
	    break;
	  case -1:
	    err_sys_fatal ("check_pass");
	  default:
	    break;
	  } /* switch */
       }
     /******************************************
     ** ALGORITHM = 0 RESTRICTIONS = NOT_PRESENT
     *******************************************/
     else
       {
#ifndef CLISERV
        fprintf (stdout, "%s", pass_string);
        if (hyph_req_present == TRUE)
	  fprintf (stdout, " (%s)", hyph_pass_string);
#ifdef APG_USE_CRYPT
        if (show_crypt_text == TRUE)
	  fprintf (stdout, " %s", crypt_string);
#endif /* APG_USE_CRYPT */
	if (spell_present == TRUE)
	 {
	  spell_pass_string = spell_word(pass_string, spell_pass_string);
	  fprintf (stdout, (" %s"), spell_pass_string);
	  free((void*)spell_pass_string);
	 }
        if ( delimiter_flag_present == FALSE )
	   fprintf (stdout, "\n");
	fflush (stdout);
#else /* CLISERV */
        if (hyph_req_present == TRUE)
         snprintf(out_pass, max_pass_length*19 + 4, "%s (%s)", pass_string, hyph_pass_string);
	else
         snprintf(out_pass, max_pass_length*19 + 4, "%s", pass_string);	    
	write (0, (void*) out_pass, strlen(out_pass));
	write (0, (void*)&delim[0],2);
#endif /* CLISERV */
	i++;
       }
    }
   /***************************************
   ** ALGORITHM = 1
   ****************************************/
   else if (algorithm == 1)
    {
     if (gen_rand_pass(pass_string, min_pass_length,
                       max_pass_length, mode.pass) == -1)
        err_app_fatal("apg","wrong password length parameter");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
     if (show_crypt_text == TRUE)
         (void)memcpy ((void *)crypt_string,
	               (void *)crypt_passstring(pass_string), 255);
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     /***************************************
     ** ALGORITHM = 1 RESTRICTIONS = PRESENT
     ****************************************/
     if ( (restrictions_present == TRUE))
       {
        /* Filter check */
        if (filter_restrict_present == TRUE)
	  restrict_res = filter_check_pass(pass_string, mode.filter);
	/* Bloom-filter check */
	if (restrict_res == 0)
	 {
	  if (bloom_restrict_present == TRUE)
	   {
	    if(paranoid_bloom_restrict_present != TRUE)
              restrict_res = bloom_check_pass(pass_string, restrictions_file);
	    else
	      restrict_res = paranoid_bloom_check_pass(pass_string, restrictions_file, min_substr_len);
	   }
	 }
	 /* Dictionary check */
	 if (restrict_res == 0)
	  if (plain_restrictions_present == TRUE)
            restrict_res = check_pass(pass_string, plain_restrictions_file);


        switch (restrict_res)
	  {
	  case 0:
#ifndef CLISERV
#ifdef APG_USE_CRYPT
            if (show_crypt_text==TRUE)
	      fprintf (stdout, "%s %s", pass_string, crypt_string);
	    else
#endif /* APG_USE_CRYPT */
	      fprintf (stdout, "%s", pass_string);
	    if (spell_present == TRUE)
	     {
	      spell_pass_string = spell_word(pass_string, spell_pass_string);
	      fprintf (stdout, (" %s"), spell_pass_string);
	      free((void*)spell_pass_string);
	     }
	    if ( delimiter_flag_present == FALSE )
	       fprintf (stdout, "\n");
	    fflush (stdout);
#else /* CLISERV */
	    write (0, (void*)pass_string, strlen(pass_string));
	    write (0, (void*)&delim[0],2);
#endif /* CLISERV */
	    i++;
	    break;
	  case 1:
	    break;
	  case -1:
	    err_sys_fatal ("check_pass");
	  default:
	    break;
	  } /* switch */
       }
     /***************************************
     ** ALGORITHM = 1 RESTRICTIONS = PRESENT
     ****************************************/
     else
       {
#ifndef CLISERV
#ifdef APG_USE_CRYPT
        if (show_crypt_text==TRUE)
          fprintf (stdout, "%s %s", pass_string, crypt_string);
	else
#endif /* APG_USE_CRYPT */
          fprintf (stdout, "%s", pass_string);
	if (spell_present == TRUE)
	 {
	  spell_pass_string = spell_word(pass_string, spell_pass_string);
	  fprintf (stdout, (" %s"), spell_pass_string);
	  free((void*)spell_pass_string);
	 }
	if ( delimiter_flag_present == FALSE )
	   fprintf (stdout, "\n");
	fflush (stdout);
#else /* CLISERV */
	write (0, (void*)pass_string, strlen(pass_string));
	write (0, (void*)&delim[0],2);
#endif /* CLISERV */
	i++;
       }
    } /* end of if (algorithm == 1) */
   else
     err_app_fatal ("apg","wrong algorithm type");

   restrict_res = 0;
  } /* end of while (i <= number_of_pass) */
 free((void*)pass_string);
 free((void*)hyph_pass_string);
#ifndef CLISERV
#ifdef APG_USE_CRYPT
 if (show_crypt_text==TRUE)
    free((void*)crypt_string);
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
#ifdef CLISERV
 free ((void *)out_pass);
 free ((void *)cliaddr);
 close (0);
 closelog();
#endif /* CLISERV */
 return(0);
} /* end of main */

#ifndef CLISERV
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(__WIN32__)
/*
** get_user_seq() - Routine that gets user random sequense
** and generates sutable random seed according to it.
** INPUT:
**   void
** OUTPUT:
**   UINT32 - random seed
** NOTES:
**   none
*/
UINT32
get_user_seq (void)
{
 char * seq;
 UINT32 prom[2] = { 0L, 0L };
 UINT32 sdres = 0L;
 printf ("\nPlease enter some random data (only first %d are significant)\n", sizeof(prom));
 seq = (char *)getpass("(eg. your old password):>");
 if (strlen(seq) < sizeof(prom))
  (void)memcpy((void *)&prom[0], (void *)seq, (int)strlen(seq));
 else
  (void)memcpy((void *)&prom[0], (void *)seq, sizeof(prom));
 sdres = prom[0]^prom[1];
 return (sdres);
}
#endif /* WIN32 */
/*
** com_line_user_seq() - Routine that gets user random sequense
** from command line and generates sutable random seed according to it
** INPUT:
**   char * - command line seed
** OUTPUT:
**   UINT32 - random seed
** NOTES:
**   none
*/
UINT32
com_line_user_seq (char * seq)
{
 UINT32 prom[2] = { 0L, 0L };
 UINT32 sdres = 0L;
 if (strlen(seq) < sizeof (prom))
  (void)memcpy((void *)&prom[0], (void *)seq, (int)strlen(seq));
 else
  (void)memcpy((void *)&prom[0], (void *)seq, sizeof(prom));
 sdres = prom[0]^prom[1];
 return (sdres);
}

/*
** print_help() - print help :)))
** INPUT:
**   none.
** OUTPUT:
**   help info to the stdout.
** NOTES:
**   none.
*/
void
print_help (void)
{
 printf ("\napg   Automated Password Generator\n");
 printf ("        Copyright (c) Adel I. Mirzazhanov\n");
 printf ("\napg   [-a algorithm] [-r file] \n");
 printf ("      [-M mode] [-E char_string] [-n num_of_pass] [-m min_pass_len]\n");
 printf ("      [-x max_pass_len] [-c cl_seed] [-d] [-s] [-h] [-y] [-q]\n");
 printf ("\n-M mode         new style password modes\n");
 printf ("-E char_string  exclude characters from password generation process\n");
 printf ("-r file         apply dictionary check against file\n");
 printf ("-b filter_file  apply bloom filter check against filter_file\n");
 printf ("                (filter_file should be created with apgbfm(1) utility)\n");
 printf ("-p substr_len   paranoid modifier for bloom filter check\n");
 printf ("-a algorithm    choose algorithm\n");
 printf ("                 1 - random password generation according to\n");
 printf ("                     password modes\n");
 printf ("                 0 - pronounceable password generation\n");
 printf ("-n num_of_pass  generate num_of_pass passwords\n");
 printf ("-m min_pass_len minimum password length\n");
 printf ("-x max_pass_len maximum password length\n");
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32) && !defined(__WIN32__)
 printf ("-s              ask user for a random seed for password\n");
 printf ("                generation\n");
#endif /* WIN32 */
 printf ("-c cl_seed      use cl_seed as a random seed for password\n");
 printf ("-d              do NOT use any delimiters between generated passwords\n");
 printf ("-l              spell generated password\n");
 printf ("-t              print pronunciation for generated pronounceable password\n");
#ifdef APG_USE_CRYPT
 printf ("-y              print crypted passwords\n");
#endif /* APG_USE_CRYPT */
 printf ("-q              quiet mode (do not print warnings)\n");
 printf ("-h              print this help screen\n");
 printf ("-v              print version information\n");
}

#ifdef APG_USE_CRYPT
/*
** crypt_passstring() - produce crypted password.
** INPUT:
**   const char * - password string
** OUTPUT:
**   char * - crypted password 
** NOTES:
**   none.
*/
char * crypt_passstring (const char *p)
{
 char salt[10];
 gen_rand_pass (salt, 10, 10, S_SL|S_CL|S_NB);
 return (crypt(p, salt));
}
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */

/*
** checkopt() - check options.
** INPUT:
**   char * - options string.
** OUTPUT:
**   none.
** NOTES:
**   option should contain only numeral symbols.
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
** construct_mode() - construct mode for password
** generation from string.
** INPUT:
**   char * - string mode.
** OUTPUT:
**   int - return code.
**     0 - OK
**    -1 - ERROR
** NOTES:
**   none.
*/
int construct_mode(char *s_mode, struct  pass_m * mde)
{
 unsigned int mode = 0;
 unsigned int filter = 0;
 int ch = 0;
 int i = 0;
 int str_length = 0;
 
 str_length = strlen(s_mode);
 
 if (str_length > MAX_MODE_LENGTH)
     return(-1);
 for (i=0; i < str_length; i++)
  {
   ch = (int)*s_mode;
   switch(ch)
    {
     case 'S':
      mode = mode | S_SS;
      filter = filter | S_SS;
      break;
     case 'N':
      mode = mode | S_NB;
      filter = filter | S_NB;
      break;
     case 'C':
      mode = mode | S_CL;
      filter = filter | S_CL;
      break;
     case 'L':
      mode = mode | S_SL;
      filter = filter | S_SL;
      break;
     case 's':
      mode = mode | S_SS;
      break;
     case 'n':
      mode = mode | S_NB;
      break;
     case 'c':
      mode = mode | S_CL;
      break;
     case 'l':
      mode = mode | S_SL;
      break;
     default:
      return(-1);
      break;
    }
   s_mode++;
  }
 mde->pass = mode;
 mde->filter = filter;
 return (0);
}


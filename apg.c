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

/*
** Main Module of apg programm
*/
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#define MAX_MODE_LENGTH   5

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE
#endif

#include <unistd.h>
/*#include <crypt.h>*/
#ifdef __CYGWIN__
#include <getopt.h>
#undef APG_USE_CRYPT
#endif /* __CYGWIN__ */

#ifdef CLISERV
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <syslog.h>
#  define MAXSOCKADDDR 128
#endif /* CLISERV */

#include "owntypes.h"
#include "pronpass.h"
#include "randpass.h"
#include "restrict.h"
#include "rnd.h"
#include "errs.h"

#ifndef CLISERV
UINT32 get_user_seq (void);
UINT32 com_line_user_seq (char * seq);
char *crypt_passstring (const char *p); /*!!*/
void print_help (void);
#endif /* CLISERV */

int main (int argc, char *argv[]);
void checkopt(char *opt);
unsigned int construct_mode(char *str_mode);

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
 char *restrictions_file;                /* dictionary file name            */
 unsigned int pass_mode = 0;	         /* password generation mode        */
 unsigned int pass_mode_present = FALSE; /* password generation mode flag   */
 USHORT min_pass_length = 6;             /* min password length             */
 USHORT max_pass_length = 8;             /* max password length             */
 int number_of_pass = 6;                 /* number of passwords to generate */
 UINT32 user_defined_seed = 0L;          /* user defined random seed        */
 int user_defined_seed_present = FALSE;  /* user defined random seed flag   */
 char *str_mode;                         /* string mode pointer             */
#ifndef CLISERV
 char *com_line_seq;
 unsigned int delimiter_flag_present = FALSE;
#ifdef APG_USE_CRYPT
 char *crypt_string;
 unsigned int show_crypt_text = FALSE;   /* display crypt(3)'d text flag    */
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
#ifdef CLISERV
#ifdef sgi /* Thanks to Andrew J. Caird */
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
 while ((option = getopt (argc, argv, "SNCLRM:a:r:sdc:n:m:x:hvy")) != -1)
#else /* APG_USE_CRYPT */
 while ((option = getopt (argc, argv, "SNCLRM:a:r:sdc:n:m:x:hv")) != -1)
#endif /* APG_USE_CRYPT */
#else /* CLISERV */
 while ((option = getopt (argc, argv, "SNCLRM:a:r:n:m:x:v")) != -1)
#endif /* CLISERV */
  {
   switch (option)
    {
     case 'S': /* special symbols required */
      pass_mode = pass_mode | S_SS;
      pass_mode_present = TRUE;
      break;
     case 'R': /* special symbols required */
      pass_mode = pass_mode | S_SS;
      pass_mode = pass_mode | S_RS;
      pass_mode_present = TRUE;
      break;
     case 'N': /* numbers required */
      pass_mode = pass_mode | S_NB;
      pass_mode_present = TRUE;
      break;
     case 'C': /* capital letters required */
      pass_mode = pass_mode | S_CL;
      pass_mode_present = TRUE;
      break;
     case 'L': /* small letters required */
      pass_mode = pass_mode | S_SL;
      pass_mode_present = TRUE;
      break;
     case 'M':
      str_mode = optarg;
      if( (pass_mode = construct_mode(str_mode)) == 0xFFFF)
         err_app_fatal("construct_mode","wrong parameter");
      pass_mode_present = TRUE;
      break;
     case 'a': /* algorithm specification */
      checkopt(optarg);
      algorithm = atoi (optarg);
      break;
     case 'r': /* restrictions */
      restrictions_present = TRUE;
      restrictions_file = optarg;
      break;
#ifndef CLISERV
     case 's': /* user random seed required */
      user_defined_seed = get_user_seq ();
      user_defined_seed_present = TRUE;
      break;
     case 'c': /* user random seed given in command line */
      com_line_seq = optarg;
      user_defined_seed = com_line_user_seq (com_line_seq);
      user_defined_seed_present = TRUE;
      break;
     case 'd': /* No delinmiters option */
      delimiter_flag_present = TRUE;
      break;
#ifdef APG_USE_CRYPT
     case 'y': /* display crypt(3)'d text next to passwords */ /*!!*/
      show_crypt_text = TRUE;
      break;                                                                  
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     case 'n': /* number of password specification */
      checkopt(optarg);
      number_of_pass = atoi (optarg);
      break;
     case 'm': /* min password length */
      checkopt(optarg);
      min_pass_length = (USHORT) atoi (optarg);
      break;
     case 'x': /* max password length */
      checkopt(optarg);
      max_pass_length = (USHORT) atoi (optarg);
      break;
#ifndef CLISERV
     case 'h': /* print help */
      print_help ();
      return (0);
#endif /* CLISERV */
     case 'v': /* print version */
      printf ("APG (Automated Password Generator)");
      printf ("\nversion 1.2.11");
      printf ("\nCopyright (c) 1999, 2000, 2001 Adel I. Mirzazhanov\n");
      return (0);
     default: /* print help end exit */
#ifndef CLISERV
      print_help ();
#endif /* CLISERV */
      exit (-1);
    }
  }
 if (pass_mode_present != TRUE)
    pass_mode = S_SS | S_NB | S_CL | S_SL;
 if( (tme = time(NULL)) == ( (time_t)-1))
    err_sys("time");
 if (user_defined_seed_present != TRUE)
    x917cast_setseed ( (UINT32)tme);
 else
    x917cast_setseed (user_defined_seed ^ (UINT32)tme);
 if (min_pass_length > max_pass_length)
    max_pass_length = min_pass_length;
 /* main code section */
 
 /*
 ** reserv space for password and hyphenated password and report of errors
 */
 if ( (pass_string = (char *)calloc (1, (size_t)(max_pass_length + 1)))==NULL ||
      (hyph_pass_string = (char *)calloc (1, (size_t)(max_pass_length*2)))==NULL)
      err_sys_fatal("calloc");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
 if (show_crypt_text == TRUE)
   if ((crypt_string = (char *)calloc (1, 255))==NULL)      
      err_sys_fatal("calloc");
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
#ifdef CLISERV
 if ( (out_pass = (char *)calloc(1, (size_t)(max_pass_length*3 + 4))) == NULL)
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
                       min_pass_length, max_pass_length, pass_mode) == -1)
        err_app_fatal("apg","wrong password length parameter");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
     if (show_crypt_text == TRUE)
         bcopy ((void *)crypt_passstring (pass_string),
	        (void *)crypt_string, 255);
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     if (restrictions_present == 1)
       {
        restrict_res = check_pass(pass_string, restrictions_file);
        switch (restrict_res)
	  {
	  case 0:
#ifndef CLISERV
#ifdef APG_USE_CRYPT
            if (show_crypt_text == TRUE)
	      fprintf (stdout, "%s (%s) %s", pass_string, hyph_pass_string,
	               crypt_string);
	    else
#endif /* APG_USE_CRYPT */
	      fprintf (stdout, "%s (%s)", pass_string, hyph_pass_string);
	    if ( delimiter_flag_present == FALSE )
	       fprintf (stdout, "\n");
	    fflush (stdout);
#else /* CLISERV */
            snprintf(out_pass, max_pass_length*3 + 4,
	             "%s (%s)", pass_string, hyph_pass_string);	    
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
     else /* if (restrictions_present == 0) */
       {
#ifndef CLISERV
#ifdef APG_USE_CRYPT
        if (show_crypt_text == TRUE)
          fprintf (stdout, "%s (%s) %s", pass_string, hyph_pass_string,
	           crypt_string);
	else
#endif /* APG_USE_CRYPT */
          fprintf (stdout, "%s (%s)", pass_string, hyph_pass_string);
        if ( delimiter_flag_present == FALSE )
	   fprintf (stdout, "\n");
	fflush (stdout);
#else /* CLISERV */
        snprintf(out_pass, max_pass_length*3 + 4,
	         "%s (%s)", pass_string, hyph_pass_string);
	write (0, (void*) out_pass, strlen(out_pass));
	write (0, (void*)&delim[0],2);
#endif /* CLISERV */
	i++;
       }
    } /* end of if (algorithm == 0) */
   else if (algorithm == 1)
    {
     if (gen_rand_pass(pass_string, min_pass_length,
                       max_pass_length, pass_mode) == -1)
        err_app_fatal("apg","wrong password length parameter");
#ifndef CLISERV
#ifdef APG_USE_CRYPT
     if (show_crypt_text == TRUE)
         bcopy ((void *)crypt_passstring(pass_string),
	        (void *)crypt_string, 255);
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */
     if (restrictions_present == 1)
       {
        restrict_res = check_pass(pass_string, restrictions_file);
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
     else /* if (restrictions_present == 0) */
       {
#ifndef CLISERV
#ifdef APG_USE_CRYPT
        if (show_crypt_text==TRUE)
          fprintf (stdout, "%s %s", pass_string, crypt_string);
	else
#endif /* APG_USE_CRYPT */
          fprintf (stdout, "%s", pass_string);
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
/*
** Routine that gets user random sequense and generates
** sutable random seed according to it
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
  bcopy((void *)seq, (void *)&prom[0], (int)strlen(seq));
 else
  bcopy((void *)seq, (void *)&prom[0], sizeof(prom));
 sdres = prom[0]^prom[1];
 return (sdres);
}

/*
** Routine that gets user random sequense from command line and generates
** sutable random seed according to it
*/
UINT32
com_line_user_seq (char * seq)
{
 UINT32 prom[2] = { 0L, 0L };
 UINT32 sdres = 0L;
 if (strlen(seq) < sizeof (prom))
  bcopy((void *)seq, (void *)&prom[0], (int)strlen(seq));
 else
  bcopy((void *)seq, (void *)&prom[0], sizeof(prom));
 sdres = prom[0]^prom[1];
 return (sdres);
}

void
print_help (void)
{
 printf ("\napg   Automated Password Generator\n");
 printf ("        Copyright (c) Adel I. Mirzazhanov\n");
 printf ("\napg   [-a algorithm] [-r file] [-S] [-C] [-L] [-R]\n");
 printf ("      [-N] [-M mode] [-n num_of_pass] [-m min_pass_len]\n");
 printf ("      [-x max_pass_len] [-c cl_seed] [-d] [-s] [-h] [-y]\n");
 printf ("\n-S -N -C -L -R  password modes\n");
 printf ("-M mode         new style pasword modes\n");
 printf ("-r file         apply dictionary check against file\n");
 printf ("-a algorithm    choose algorithm\n");
 printf ("                 1 - random password generation according to\n");
 printf ("                     password modes\n");
 printf ("                 0 - pronounceable password generation\n");
 printf ("-n num_of_pass  generate num_of_pass passwords\n");
 printf ("-m min_pass_len minimum password length\n");
 printf ("-x max_pass_len maximum password length\n");
 printf ("-s              ask user for a random seed for password\n");
 printf ("                generation\n");
 printf ("-c cl_seed      use cl_seed as a random seed for password\n");
 printf ("-d              do NOT use any delimiters between generated passwords\n");
#ifdef APG_USE_CRYPT
 printf ("-y              print crypted passwords\n");
#endif /* APG_USE_CRYPT */
 printf ("-h              print this help screen\n");
 printf ("-v              print version information\n");
}

#ifdef APG_USE_CRYPT
char * crypt_passstring (const char *p)
{
 char salt[10];
 gen_rand_pass (salt, 10, 10, S_SL|S_CL|S_NB);
 return (crypt(p, salt));
}
#endif /* APG_USE_CRYPT */
#endif /* CLISERV */

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

unsigned int construct_mode(char *s_mode)
{
 unsigned int mode = 0;
 int ch = 0;
 int i = 0;
 int str_length = 0;
 
 str_length = strlen(s_mode);
 
 if (str_length > MAX_MODE_LENGTH)
     return(0xFFFF);
 for (i=0; i < str_length; i++)
  {
   ch = (int)*s_mode;
   switch(ch)
    {
     case 'S':
      mode = mode | S_SS;
      break;
     case 'N':
      mode = mode | S_NB;
      break;
     case 'C':
      mode = mode | S_CL;
      break;
     case 'L':
      mode = mode | S_SL;
      break;
     case 'R':
      mode = mode | S_SS;
      mode = mode | S_RS;
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
     case 'r':
      mode = mode | S_SS;
      mode = mode | S_RS;
      break;
     default:
      mode = mode | 0xFFFF;
      break;
    }
   s_mode++;
  }
 return (mode);
}

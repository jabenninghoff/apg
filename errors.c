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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "errs.h"

#ifdef CLISERV
#  include <syslog.h>
#endif

/*
** err_sys() - routine that handles non-fatal system errors
** like calloc, open, etc.
** INPUT:
**   const char * - error name.
** OUTPUT:
**   prints error to stderr.
** NOTES:
**   none.
*/
void
err_sys(const char *string)
{

#ifndef CLISERV
 perror(string);
#else
 syslog (LOG_DEBUG, "%s: %s",string, (char *)strerror(errno));
#endif
}

/*
** err_sus_fatal() - routine that handles fatal system errors
** like calloc, open, etc.
** INPUT:
**   const char * - error name.
** OUTPUT:
**   prints error to stderr and then exit.
** NOTES:
**   none.
*/
void
err_sys_fatal(const char *string)
{

#ifndef CLISERV
 perror(string);
#else
 syslog (LOG_DEBUG, "%s: %s", string, (char *)strerror(errno));
 closelog();
 close(0);
#endif
 exit (-1);
}

/*
** err_app() - routine that handles non-fatal application errors.
** INPUT:
**   const char * - error name.
**   const char * - error description.
** OUTPUT:
**   prints error to stderr.
** NOTES:
**   none.
*/
void
err_app(const char *string, const char * err)
{
#ifndef CLISERV
 fprintf (stderr, "%s: ", string);
 fprintf (stderr, "%s\n", err);
 fflush (stderr);
#else
 syslog (LOG_DEBUG, "%s: %s",string, err);
#endif
}

/*
** err_app_fatal() - routine that handles fatal application errors.
** INPUT:
**   const char * - error name.
**   const char * - error description.
** OUTPUT:
**   prints error to stderr and then exit.
** NOTES:
**   none.
*/
void
err_app_fatal(const char *string, const char *err)
{

#ifndef CLISERV
 fprintf (stderr, "%s: ", string);
 fprintf (stderr, "%s\n", err);
 fflush (stderr);
#else
 syslog (LOG_DEBUG, "%s: %s",string, err);
 closelog();
 close(0);
#endif
 exit (-1);
}

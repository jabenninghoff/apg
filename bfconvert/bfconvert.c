/*
** Copyright (c) 2003
** Adel I. Mirzazhanov. All rights reserved
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 
**     1.Redistributions  of  source  code  must  retain  the  above
**       copyright notice, this list of conditions and the following
**       disclaimer. 
**     2.Redistributions  in  binary  form  must reproduce the above
**       copyright notice, this list of conditions and the following
**       disclaimer  in  the  documentation  and/or  other materials
**       provided with the distribution. 
**     3.The  name  of  the  author  may  not  be used to endorse or
**       promote  products  derived   from   this  software  without
**       specific prior written permission. 
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
***************************************************************
** NAME        : BFCONVERT                                   **
** DESCRIPTION : convert APG Bloom-filter version 1.0.1 to   **
**               version 1.1.0                               **
** USAGE       : bfconvert old_bf_file_name new_bf_file_name **
** RETURN      :  0 - success                                **
**               -1 - something wrong                        **
***************************************************************
*/

#include <stdio.h>

#define OLD_APGBF_HEADER_LEN  12
#define OLD_APGBF_HEADER_ID   "APGBF101"

#define NEW_APGBF_HEADER_LEN  13
#define NEW_APGBF_HEADER_ID   "APGBF"
#define NEW_APGBF_HEADER_VER  "110"
#define NEW_APGBF_HEADER_MODE 0x00


int main (int argc, char *argv[]);

int
main(int argc, char *argv[])
{
 typedef unsigned char f_mode;

 struct new_apg_bf_hdr {
   char id[5];           /* filter ID      */
   char version[3];      /* filter version */
   unsigned long int fs; /* filter size    */
   f_mode mode;          /* filter flags   */
 };

 struct old_apg_bf_hdr {
   char id[8];           /* ID          */
   unsigned long int fs; /* filter size */
 };
 struct new_apg_bf_hdr  new_bf_hdr;
 struct old_apg_bf_hdr  old_bf_hdr;
 
 char old_etalon_bf_id[]  = OLD_APGBF_HEADER_ID;

 char new_etalon_bf_id[]  = NEW_APGBF_HEADER_ID;
 char new_etalon_bf_ver[] = NEW_APGBF_HEADER_VER;

 FILE *old_f;  /* old filter file descriptor */
 FILE *new_f;  /* new filter file descriptor */
 
 unsigned char tmp_buf; /* Temporary buffer */

 /* Checking arguments */
 if (argc != 3)
  {
   printf ("Usage: bfconvert old_bf_file_name new_bf_file_name\n");
   return(-1);
  }

 /* Opening input and output files */
 if ((old_f = fopen (argv[1], "r")) == NULL)
  {
   perror("open the old bloom-filter file");
   return(-1);
  }
 if ((new_f = fopen (argv[2], "w")) == NULL)
  {
   perror("open the new bloom-filter file");
   return(-1);
  }
 if (fread ( (void *)&old_bf_hdr, OLD_APGBF_HEADER_LEN, 1, old_f) != 1)
    if (ferror (old_f) != 0)
     {
      perror("read from the old bloom-filter file");
      return(-1);
     }

 /* Checking input file */
 if ((old_bf_hdr.id[0] != old_etalon_bf_id[0]) || (old_bf_hdr.id[1] != old_etalon_bf_id[1]) ||
     (old_bf_hdr.id[2] != old_etalon_bf_id[2]) || (old_bf_hdr.id[3] != old_etalon_bf_id[3]) ||
     (old_bf_hdr.id[4] != old_etalon_bf_id[4]) || (old_bf_hdr.id[5] != old_etalon_bf_id[5]) ||
     (old_bf_hdr.id[6] != old_etalon_bf_id[6]) || (old_bf_hdr.id[7] != old_etalon_bf_id[7]) )
     {
      fprintf(stderr,"Input file is not APG bloom filter file v1.0.1\n");
      fflush (stderr);
      return (-1);
     }

 /* Constructing output BF file header */
 new_bf_hdr.id[0] = new_etalon_bf_id[0];
 new_bf_hdr.id[1] = new_etalon_bf_id[1];
 new_bf_hdr.id[2] = new_etalon_bf_id[2];
 new_bf_hdr.id[3] = new_etalon_bf_id[3];
 new_bf_hdr.id[4] = new_etalon_bf_id[4];
 new_bf_hdr.version[0] = new_etalon_bf_ver[0];
 new_bf_hdr.version[1] = new_etalon_bf_ver[1];
 new_bf_hdr.version[2] = new_etalon_bf_ver[2];
 new_bf_hdr.fs = old_bf_hdr.fs;
 new_bf_hdr.mode = NEW_APGBF_HEADER_MODE;
 
 /* Writing new filter header to output file */
 if (fwrite ( (void *)&new_bf_hdr, NEW_APGBF_HEADER_LEN, 1, new_f) != 1)
     {
      perror("write to the new bloom-filter file");
      return(-1);
     }
 /* Reading filter content from the old BF file and writing it to the new BF file */
 while (fread ( (void *)&tmp_buf, 1, 1, old_f) == 1)
  {
   if(fwrite( (void *)&tmp_buf, 1, 1, new_f) != 1)
    {
     perror("write to the new bloom-filter file");
     return(-1);
    }
  }
 if (ferror (old_f) != 0)
  {
   perror("read from the old bloom-filter file");
   return(-1);
  }
 /* Close input and output files */
 if (fclose(old_f) == EOF)
  {
   perror("close old bloom-filter file");
   return(-1);
  }
 if (fclose(new_f) == EOF)
  {
   perror("close new bloom-filter file");
   return(-1);
  }
   
 printf("\nInput file has been successfuly converted\n");
 return(0);
}

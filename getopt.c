/*
 * Modified by Adel I. Mirzazhanov 2002, 2003
 * getopt - get option letter from argv
 *
 * This is a version of the public domain getopt() implementation by
 * Henry Spencer, changed for 4.3BSD compatibility (in addition to System V).
 * It allows rescanning of an option list by setting optind to 0 before
 * calling, which is why we use it even if the system has its own (in fact,
 * this one has a unique name so as not to conflict with the system's).
 * Thanks to Dennis Ferguson for the appropriate modifications.
 *
 * This file is in the Public Domain.
 */

#include <stdio.h>
#include "getopt.h"

static int badopt(const char *mess,int ch);


char	*apg_optarg;	/* Global argument pointer. */
int	apg_optind = 0;	/* Global argv index. */
int	apg_opterr = 1;	/* for compatibility, should error be printed? */
int	apg_optopt;	/* for compatibility, option character checked */

static char	*scan = NULL;	/* Private scan pointer. */
static const char	*prog = "apg";

/*
 * Print message about a bad option.
 */
static int
badopt(const char *mess,int ch)
{
	if (apg_opterr) {
		fprintf(stderr,"%s%s%c\n", prog, mess, ch);
		fflush(stderr);
	}
	return ('?');
}

int
apg_getopt(int argc,char *argv[],const char *optstring)
{
	register char c;
	register const char *place;

	prog = argv[0];
	apg_optarg = NULL;

	if (apg_optind == 0) {
		scan = NULL;
		apg_optind++;
	}
	
	if (scan == NULL || *scan == '\0') {
		if (apg_optind >= argc
		    || argv[apg_optind][0] != '-'
		    || argv[apg_optind][1] == '\0') {
			return (EOF);
		}
		if (argv[apg_optind][1] == '-'
		    && argv[apg_optind][2] == '\0') {
			apg_optind++;
			return (EOF);
		}
	
		scan = argv[apg_optind++]+1;
	}

	c = *scan++;
	apg_optopt = c & 0377;
	for (place = optstring; place != NULL && *place != '\0'; ++place)
	    if (*place == c)
		break;

	if (place == NULL || *place == '\0' || c == ':' || c == '?') {
		return (badopt(": unknown option -", c));
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			apg_optarg = scan;
			scan = NULL;
		} else if (apg_optind >= argc) {
			return (badopt(": option requires an argument -", c));
		} else {
			apg_optarg = argv[apg_optind++];
		}
	}

	return (c & 0377);
}

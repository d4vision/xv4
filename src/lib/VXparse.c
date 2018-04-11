/*
 * VXparse.c: Parse parameters by substring
 *	
 *  changed for new parameter structure and VXparse 10/97 APR
 *
 *  This routine is passed the argument count (-argc-), the
 *  parameter list (-argv-) and an array of structures (-par-)
 *  and returns with the structure elements filled as specified.
 *
 *  The argument count and argument list are the same as is
 *  passed to the main routine by the system, and the format
 *  of the structure is the same as -params- below.
 *
 *  Parsing is done by examining each parameter in order
 *  and trying to match its initial substring to the supplied
 *  pattern for each structure NOT ALREADY MATCHED. When a
 *  match occurs a pointer to the next character in the parameter
 *  after the matched initial substring is set in the
 *  structure.
 *
 *  The array is ended by a zero match pointer. The number of
 *  matched arguments is returned as the value of the function.
 *
 *  The pointers to the matched string are cleared before parsing.
 *  Multiple occurrences of the same match string are allowed
 *  and will be filled in order of occurence. A pointer to a null
 *  character (i.e., "\0") will match anything.
 *  This is useful for pulling off up to a fixed number of
 *  file names, for instance.
 *
 *  VisionX file name resolution:
 *  In general parameters of the form "...=" will be matched to any
 *  unprefixed arguments in the order that they appear in the params
 *  structure then the "\0" parameters will be satisfied.
 *  The one excepyion to this is an unmatched parameter of "of="
 *  This parameter must be preceded by a "-o" argument for a match
 *  to occur.
 *
 *
 */

/*
 *   Copyright 2014 Anthony P. Reeves
 *
 *   contributions by Mike Zuhl
 * 
 *   This file is part of xv4Package.
 * 
 *   xv4Package is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   any later version.
 *
 *   xv4Package is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with xv4Package.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <time.h>
#include "VisXV4.h"
#include "Vutil.h"
#include "Verror.h"

extern int     g_argc;
extern char  **g_argv;
#ifdef PDEBUG
static int debug = 0;
#endif


static int VXparse2();

int
VXparse(ac, av, parm)
int    *ac;
char  ***av;
VXparam_t *parm;		/* terminated by  match == 0  */

{
    VXparam_t *p;
    int     count,
            i,
            j;
    int     mm;			/* match end */
    int	argc, u_argc;
    char **argv, **u_argv;

    argc = *ac;
    argv = *av;

    count = 0;
    g_argc = argc;
    g_argv = argv;


    /* Initialize value pointers to zero */

    for (p = parm; p->par != 0; p++->val = 0);

    /* look for a help in argv[1] */

    if (argc == 2 && strcmp(argv[1], "-H") == 0) {
	VXusage(argc, argv, parm);
	exit(0);
    }
    if ((strcmp(argv[argc - 1], "-") == 0) ||
	(strcmp(argv[argc - 1], "-help") == 0) ) {
	VXfusage(argc, argv, parm);
	exit(0);
    }

#ifdef PDEBUG
    if (argc >= 2 && strcmp(argv[1], "-H-") == 0) {
	debug = 1;
	argv++;
	argc--;
	for (i = 0; i < argc; i++)
	     (void) fprintf(stderr, "%d: %s\n", i, argv[i]);
    }
#endif
    

    /* Now scan for params */

    u_argv = NEWN(char *, argc + 1);
    u_argc = 0;
    u_argv[u_argc++] = *argv;
#ifdef PDEBUG
    if (debug)
	(void) fprintf(stderr, "Exact match:\n");
#endif
	
    while (--argc > 0) {
	argv++;
	/* Search for match */

	for (p = parm; (p->par != 0) && (p->val || !*p->par ||
			(mm = VXparse2(p->par, *argv)) == -1); p++);

	/* If there is a match, set value */

	if (p->par != 0 && *p->par) {	/* if we didn't hit the end */
	    count++;
	    p->val = *argv + mm;
#ifdef PDEBUG
	    if (debug)
		(void) fprintf(stderr, "\tMatched %s with %s value %s\n", 
			       *argv, p->par, p->val);
#endif
	}
	else  {
	    u_argv[u_argc++] = *argv;
#ifdef PDEBUG
	    if (debug)
		(void) fprintf(stderr, "\tDid not match %s\n", *argv);
#endif
	}
    }
    if (u_argc > 1) {		/* we need to match unprefixed args */
#ifdef PDEBUG
	if (debug)
	    (void) fprintf(stderr, "Unprefixed Args:\n");
#endif
	/* first resolve a missing of= */
	for (p = parm; p->par != 0; p++) 
	    if (p->val == NULL && strcmp(p->par, "of=") == 0)
		break;
	if (p->par != 0) {	/* if we didn't hit the end */
	    /* look for -o */
	    for (i = 1; i < u_argc; i++)
		if (strcmp(u_argv[i], "-o") == 0) {
		    if (i + 1 == u_argc) {
			*ac = u_argc;
			*av = u_argv;
			Verrno = PE_MIS_PAR;
			Verror("%s: Command syntax (%s).\n", *g_argv, 
			       Verrorstr());
		    }
		    p->val = u_argv[i + 1];
#ifdef PDEBUG
		    if (debug)
			(void) fprintf(stderr, 
				       "\tMatched %s with %s value %s\n", 
				       u_argv[i + 1], p->par, p->val);
#endif
		    for (j = i + 2; j < u_argc; j++)
			u_argv[j - 2] = u_argv[j];
		    u_argc -= 2;
		    count++;
		    break;
		}
	}
    }
    i = 1;
#ifdef PDEBUG
    if (debug)
	(void) fprintf(stderr, "Missing XXX=:\n");
#endif
    while (i < u_argc) {	/* now resolve a missing XXX= */
	for (p = parm; p->par != 0; p++) 
	     if (!p->val && p->par[strlen(p->par) - 1] == '=' &&
	         strcmp(p->par, "of=") != 0)
		 break;
	if (p->par != 0) {	/* if we didn't hit the end */
	    p->val = u_argv[i];
#ifdef PDEBUG
	    if (debug)
		(void) fprintf(stderr, 
			       "\tMatched %s with %s value %s\n", 
			       u_argv[i], p->par, p->val);
#endif
	    for (j = i; j < u_argc; j++)
		u_argv[j] = u_argv[j+1];
	    u_argc--;
	    count++;
	}
	else
	    break;
    }
#ifdef PDEBUG
    if (debug)
	(void) fprintf(stderr, "Missing \\0:\n");
#endif
    while (i < u_argc) {	/* now resolve any  missing  "\0" match
				 * strings */
	for (p = parm; (p->par != 0) && (p->val || *p->par != '\0'); p++);
	if (p->par != 0) {	/* if we didn't hit the end */
	    p->val = u_argv[i];
#ifdef PDEBUG
	    if (debug)
		(void) fprintf(stderr, 
			       "\tMatched %s with %s value %s\n", 
			       u_argv[i], p->par, p->val);
#endif
	    for (j = i; j < u_argc; j++)
		u_argv[j] = u_argv[j+1];
	    u_argc--;
	    count++;
	}
	else
	    break;
    }
    if (u_argc > 1) {
	*ac = u_argc;
	*av = u_argv;
	Verrno = PE_UNM_PAR;
	Verror("%s: Command syntax (%s).\n", *g_argv, Verrorstr());
    }
    *ac = u_argc;
    *av = u_argv;
    return (count);
}

/*
 *  VXparse2 does the matching of the string. Now it
 *  matches initial substrings.
 */
static int
VXparse2(aa, bb)
char   *aa,
       *bb;
{
    register char *a,
           *b;
    register int mm;

    a = aa;
    b = bb;

    mm = 0;

    while ((*a != '\0') && (*b != '\0') && (*a == *b++)) {
	a++;
	mm++;
    }
    return (*a == '\0' ? mm : -1);
}

void
VXusage(argc, argv, parm)
int     argc;
char  **argv;
VXparam_t *parm;		/* terminated by  match == 0  */

{
    VXparam_t *p;

    (void) fprintf(stderr, "Usage: %s [-H] ", argv[0]);
    for (p = parm; p->par != 0; p++) {
	if (strcmp(p->par, "id=") == 0)
	    (void) fprintf(stderr, "[id=<rawdatafile>] ");
	else if (strcmp(p->par, "if=") == 0)
	    (void) fprintf(stderr, "[if=<inputfile>] ");
	else if (strcmp(p->par, "ig=") == 0)
	    (void) fprintf(stderr, "[ig=<inputfile2>] ");
	else if (strcmp(p->par, "gf=") == 0)
	    (void) fprintf(stderr, "[gf=<inputfile2>] ");
	else if (strcmp(p->par, "of=") == 0)
	    (void) fprintf(stderr, "[of=<outputfile>] ");
	else if (strchr(p->par, '=') != 0)
	    (void) fprintf(stderr, "[%s<value>] ", p->par);
	else if (p->par[0] == '+')
	    (void) fprintf(stderr, "[+<xpos>,<ypos>] ");
	else if (p->par[0] != '\0')
	    (void) fprintf(stderr, "[%s] ", p->par);
    }
    (void) fprintf(stderr, "\n");
}				/* end VXusage */
void
VXfusage(argc, argv, parm)
int     argc;
char  **argv;
VXparam_t *parm;		/* terminated by  match == 0  */

{
    VXparam_t *p;

    (void) fprintf(stderr, "Usage: %s [-H] [-] [-help]\n", argv[0]);
    for (p = parm; p->par != 0; p++) {
	if ( p->par[0] != 0 )
	    (void) fprintf(stderr, " [%-3s] %s\n", p->par, p->def);
    }
    (void) fprintf(stderr, "\n");
}				/* end VXfusage */

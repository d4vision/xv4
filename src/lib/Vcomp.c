/*
 * uncompress routines:
 *	Vcomp() - returns 0 if not recognized, spawns uncompresses 
 *		   if recognized
 *	uncompress(method, buf, n ) - uncompress buf, 
 *					    using method, return sizeof buf
 */


/*
 *   Copyright 2014 Anthony P. Reeves
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


#include "VisXV4.h"
#include <stdio.h>
#include <stdlib.h>
#ifndef BSD
#include <string.h>
#else
#include <strings.h>
#endif
#ifndef WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif




static struct {
   char *magic;
   int   maglen;
   char *argv[3];
   int	 silent;
} compr[] = {
    { "\037\235", 2, { "uncompress", "-c", NULL }, 0 },	/* compressed */
    { "\037\213", 2, { "gzip", "-cdq", NULL }, 1 },	/* gzipped */
    { "\037\236", 2, { "gzip", "-cdq", NULL }, 1 },	/* frozen */
    { "\037\240", 2, { "gzip", "-cdq", NULL }, 1 },	/* SCO LZH */
    /* the standard pack utilities do not accept standard input */
    { "\037\036", 2, { "gzip", "-cdq", NULL }, 0 },	/* packed */
    /* now look for a wrong endian VisionX                           */
    { VX_MAGICX, 3, { "vxendian", NULL, NULL }, 0 },  /* VisionX: o endian */
    /* now to deal with pgm ppm and pbm   */
    { "P1", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P1: Pbm ascii */
    { "P2", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P2: Pgm ascii */
    { "P3", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P3: Ppm ascii */
    { "P4", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P4: Pbm binary */
    { "P5", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P5: Pgm binary */
    { "P6", 2, { "vpnmtovx", NULL, NULL }, 0 },	      /* P6: Ppm binary */
};

static int ncompr = sizeof(compr) / sizeof(compr[0]);


static int uncompress __P((int, unsigned char *, int, int *));

int
Vcomp(buf, nbytes, fdptr)
unsigned char *buf;
int nbytes;
int *fdptr;
{
	int i;

	for (i = 0; i < ncompr; i++) {
		if (nbytes < compr[i].maglen)
			continue;
		if (memcmp(buf, compr[i].magic,  compr[i].maglen) == 0)
			break;
	}

          /*fprintf(stderr, "vcomp: i is %d %s %s\n",i,VX_MAGICX,VX_MAGIC);*/
	if (i == ncompr)
		return 0;

	uncompress(i, buf, nbytes, fdptr);
	if (i < 5 ) return 2; /* compression */
	return 1;             /* format conversion */
}


static int
uncompress(method, buf, n, fdptr)
int method;
unsigned char *buf;
int n;
int *fdptr;
{
#ifdef WIN32
    fprintf(stderr, "cannot fork %s\n", compr[method].argv[0]);
    exit(1);
    return(0);
    /*NOTREACHED*/
#else
    unsigned char dbuf[BUFSIZ];
	int fdin[2], fdout[2];
	int ofd;

	ofd = *fdptr;

	if (pipe(fdin) == -1 || pipe(fdout) == -1) {
		/* error("cannot create pipe (%s).\n", strerror(errno)); */
		fprintf(stderr,"VXopen:Vcomp error 1\n");
		exit(1);
		/*NOTREACHED*/
	}
	switch (fork()) {
	case 0:	/* child */
		(void) close(0);
		(void) dup(fdin[0]);
		(void) close(fdin[0]);
		(void) close(fdin[1]);

		(void) close(1);
		(void) dup(fdout[1]);
		(void) close(fdout[0]);
		(void) close(fdout[1]);
		if (compr[method].silent)
		    (void) close(2);

		execvp(compr[method].argv[0], compr[method].argv);
		fprintf(stderr,"VXopen:Vcomp error 2\n");
		exit(1);
		/*NOTREACHED*/
	case -1:
		fprintf(stderr,"VXopen:Vcomp error 3\n");
		exit(1);
		/*NOTREACHED*/

	default: /* parent */
		(void) close(fdin[0]);
		(void) close(fdout[1]);

    switch (fork()) {
	case 0:	/* child */
		(void) close(fdin[1]);

		*fdptr = fdout[0];
		/* this presumes that the appilication will not write
		   until after it has read n */
		Vread(fdout[0], buf, n);
		return n;
	case -1:
		fprintf(stderr,"VXopen:Vcomp error 4\n");
		exit(1);
		/*NOTREACHED*/

	default: /* parent */
		(void) close(fdout[0]);
		if (write(fdin[1], buf, n) != n) {
			fprintf(stderr,"VXopen:Vcomp error 1\n");
			exit(1);
			/*NOTREACHED*/
		}
		while ((n = read(ofd, dbuf, BUFSIZ)) > 0){
		     if (write(fdin[1], dbuf, n) != n) {
			fprintf(stderr,"VXopen:Vcomp error 1\n");
			exit(1);
		     }	   /*NOTREACHED*/
		}
		(void) close(fdin[1]);
		(void) wait(NULL);
		(void) wait(NULL);
		exit(0);
	}
	}
#endif /*WIN32*/
}

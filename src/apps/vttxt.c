/****************************************************************/
/* Example program to create a VisionX text file  		*/
/* 								*/
/* these files contain the history in lines starting with #     */
/* the end of the VisionX header is indicated  by the first     */
/* blank line                                                   */
/* 								*/
/* Syntax:							*/
/*        vttxt if=infile [of=outfile]            		*/
/****************************************************************/


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
#include "Vutil.h"

VisXfile_t *VSin, *VSout;
VisXelem_t *VXlist, *VXpt;
VisXelem_t *vfpt;

VXparam_t par[] = /* command line structure   */
{
   {"if=",   0, "input image file"},
   {"of=",   0, "output text file"},
   {0,       0, ""},
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val

FILE *tfile;
char ctitle[BUFSIZ];
char *ttle, *cpt;

int
main(argc, argv)
int argc;
char *argv[];
{
int  i;

       VXparse(&argc, &argv, par);	/* parse the command line */
       VSin = VXopen(IVAL, 0);		/* open input file        */

/* open a text file */

     VSout = VXopen(OVAL, 1);
#ifdef WIN32
    setmode(VSout->fd,O_TEXT);
#endif
    /*
       VXtextf should be called after the input file
       is opened so that the file history is available.
       It writes a valid visionx text header.
       The second argument gives the file type
       --- this will be idendified by vls
     */
    tfile = VXtextf(VSout, "example text file");

    VXpt = VXlist = VXreadframe(VSin);

    fprintf(tfile, "#---data section follows---\n\n");

    i = 0;
    while ( VXpt != VXNIL ) {
       VXpt = VXpt->next;
       i++;
    }

    fprintf(tfile, "Number of data elements is %d\n", i);
    exit (0);
}

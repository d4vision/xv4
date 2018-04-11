/****************************************************************/
/* Example VisX4 program 					*/
/*            Copy a VisX data file                             */
/* Syntax:                                                      */
/*        vcp [-f] if=infile of=outfile                         */
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

#include "VisXV4.h"       /* VisX structure include file        */
                          /*                                    */
VisXfile_t *VXin,         /* input file structure               */
	   *VXout;        /* output file structure              */
VisXelem_t *vxlist;       /* VisX data structure                */
                          /*                                    */
VXparam_t par[] =         /* command line structure             */
{
{    "if=",    0,          " input file   vcp: copy a VisionX file"},
{    "of=",    0,          " output file      "},
{    "-f",     0,          " frame-i/o option "},
{     0,       0,           0}
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  FFLAG  par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{
    VXparse(&argc, &argv, par);    /* parse the command line       */
    VXin  = VXopen(IVAL, 0);       /* open input file              */
    VXout = VXopen(OVAL, 1);       /* open the output file         */

    if(FFLAG){    /* do complete file i/o */
    
         vxlist = VXread(VXin);    /* read data                    */
         VXwrite(VXout, vxlist);   /* write data                   */

    }else {        /* do frame i/o */

	 while ((vxlist = VXreadframe(VXin)) != VXNIL){
					 /* for each frame read     */ 
	     VXfupdate(VXout, VXin);     /* update global constants */
	     VXwriteframe(VXout,vxlist); /* write frame             */
	     VXdellist(vxlist);          /* delete the frame        */
	 }
    }

    VXclose(VXin);                 /* close input file              */
    VXclose(VXout);                /* close output file             */
    exit(0);
}

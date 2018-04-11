/****************************************************************/
/* Example VisX4 program v3df					*/
/*            Compute function on a 3D image structure          */
/* Syntax:                                                      */
/*        v3df if=infile of=outfile [-v]                        */
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

#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */
VisXfile_t *VXin,            /* input file structure            */
	   *VXout;           /* output file structure           */
VisXelem_t *VXlist;          /* VisX data structure             */


VXparam_t par[] =            /* command line structure          */
{
{    "if=",    0,   " input file  v3df: 3D template"},
{    "of=",    0,   " output file "},
{    "-v",     0,   " verbose flag "},
{     0,       0,    0}
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  VFLAG  par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{

VisX3dim_t im;                     /* input v3dim structure        */
VisXelem_t *vptr;
int	   i,j,k;                  /* index counters               */
    VXparse(&argc, &argv, par);     /* parse the command line       */
    VXin  = VXopen(IVAL, 0);       /* open input file              */
    VXout = VXopen(OVAL, 1);       /* open the output file         */
    VXlist = VXread(VXin);         /* read file                    */
    if(VXNIL == (vptr = VXfind(VXlist, VX_PBYTE))){
       fprintf(stderr, "v3df: no byte images found\n");
       exit(1);
    }
    VXset3dim(&im, vptr, VXin); /* initialize input structure   */
    if(VFLAG){
       fprintf(stderr,"bbx is %f %f %f %f %f %f\n", im.bbx[0],
		 im.bbx[1],im.bbx[2],im.bbx[3],im.bbx[4],im.bbx[5]);
    }

    /* simple pixel computation -- notice the order of the loops */
    for (k = im.zlo ; k <= im.zhi ; k++)
      for (i = im.ylo ; i <= im.yhi ; i++)
        for (j = im.xlo; j <= im.xhi; j++)
	     im.u[k][i][j] = im.u[k][i][j]/2;

    VXwrite(VXout, VXlist);       /* write data                   */
    VXclose(VXin);                /* close files                  */
    VXclose(VXout);
    exit(0);
}

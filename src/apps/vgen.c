/****************************************************************/
/* vgen:    generate a single image from descriptions		*/
/*                                                              */
/* Template program that does not read an input file            */
/*                                                              */
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

#include "VisXV4.h"          /* VisionX structure include file  */
#include "Vutil.h"

VisXfile_t *VXout;           /* output file structure           */
VisXelem_t *VXlist;          /* VisX data structure             */

VXparam_t par[] =            /* command line structure          */
{
{    "of=",    0,   " output file  vgen: generate an image"},
{    "n=",     0,   " size parameter "},
{     0,       0,    0}
};

#define  OVAL   par[0].val
#define  NVAL   par[1].val

int
main(argc, argv)
int argc;
char *argv[];
{

VisXimage_t im;        /* image structure  */
int	   i,j;        /* index counters         */
int n, chn;
float scale;
float bbx[6];

    VXparse(&argc, &argv, par);    /* parse the command line       */
    VXout = VXopen(OVAL, 1);       /* open the output file         */
    chn = 1;
    n=256;
    if(NVAL)
       n = atoi (NVAL);
    bbx[0] = bbx[2] = 0.0;
    bbx[1] = bbx[3] = (float)n;
    bbx[4] = bbx[5] = 0.0;

    VXmakeimage(&im, VX_PFLOAT, bbx, chn); /* output structure */
    scale = (2 * M_PI) / (float) im.yhi;

    for (i = im.ylo; i <= im.yhi; i++)
        for (j = im.xlo; j <= im.xhi; j++)
	     im.f[i][j] = (- cos(i * scale)) * (- cos (j * scale));

    VXwrite(VXout, im.list);      /* write data                   */
    VXclose(VXout);
    exit(0);
}

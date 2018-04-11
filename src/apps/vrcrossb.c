/****************************************************************/
/* VisX4 program Vrcross				        */
/*            Compute a roberts cross edge operation            */
/* Syntax:                                                      */
/*        vrcross if=infile  of=outfile                         */
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
VisXelem_t *VXlist,*VXpt;    /* VisX data structure             */

VXparam_t par[] =            /* command line structure          */
{
{    "if=",    0,   " input file  vrcrossb: roberts cross edge operation"},
{    "of=",    0,   " output file "},
{     0,       0,    0}
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val

int
main(argc, argv)
int argc;
char *argv[];
{

VisXimage_t im;                    /* input image structure          */
VisXimage_t tm;                    /* temp image  structure          */
int	   i,j;                    /* index counters                 */
    VXparse(&argc, &argv, par);     /* parse the command line         */
    VXin  = VXopen(IVAL, 0);       /* open input file                */


    VXout = VXopen(OVAL, 1);       /* open the output file           */
    while((VXlist= VXpt = VXreadframe(VXin)) != VXNIL){/* every frame*/
      VXfupdate(VXout, VXin);      /* update global constants        */
      while(VXsetimage(&im, VXpt, VXin)){ /* for every image         */

/***********************************************************************/
        if(im.base == VX_BYTE && im.chan == 1){ 
		    /* only modify byte images with a single channel */

            VXfloatimage(&tm,&im, 0, 1, 0, 1);
				      /* temp image copy with border */

	     /* Roberts Cross Operation */
             for (i = im.ylo ; i <= im.yhi ; i++)
               for (j = im.xlo; j <= im.xhi; j++){
	          im.u[i][j] = ABS (tm.f[i][j] - tm.f[i+1][j+1]) +
			       ABS (tm.f[i+1][j] - tm.f[i][j+1]);
              }
	    VXresetimage(&tm);        /* free the tm image structure  */
	}
/***********************************************************************/

        VXpt = im.imitem->next;      /* move to the next image       */
	VXresetimage(&im);           /* free the im image structure  */
      }                              /* end of every image section   */
      VXwriteframe(VXout,VXlist);    /* write frame                  */
      VXdellist(VXlist);             /* delete the frame             */
    }                                /* end of every frame section   */
    VXclose(VXin);                   /* close files                  */
    VXclose(VXout);
    exit(0);
}

/****************************************************************/
/*  vssum:    Compute mean of image frames in a sequence        */
/*                                                              */
/*  Example of using a n-frame temporal frame buffer            */
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
#include "Vutil.h"           /* VisionX utility header files    */
VisXfile_t *VXin,            /* input file structure            */
	   *VXout;           /* output file structure           */
VisXelem_t *VXlist,*VXpt;    /* VisX data structure             */
VXparam_t par[] =            /* command line structure          */
{
{    "if=",    0,   " input file   vssum: n-frame temporal mean filter"},
{    "of=",    0,   " output file      "},
{    "n=",     0,   " number of frames "},
{     0,       0,   0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  NVAL   par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{
VisXimage_t * ib;                  /* multiframe image buffer        */
VisXimage_t tm;                    /* temp image  structure          */
int	   i,j,k;                  /* index counters                 */
int        t;
int	   n;                      /* number of frames to average    */
int     first;
    VXparse(&argc, &argv, par);    /* parse the command line         */
    VXin  = VXopen(IVAL, 0);       /* open input file                */
    VXout = VXopen(OVAL, 1);       /* open the output file           */
    n = NVAL ? atoi(NVAL) : 1;     /* read n, default is n=1         */

    ib = VXsbufimage ( VXin, n);   /* allocate buffer and read n-1 frames */
    first = 1;
    while(VXrbufimage(ib, VXin, n) ){/* every frame*/
      VXfupdate(VXout, VXin);      /* update global constants        */

/**************** Application specifiec section **********************/

      if (first){ /* initialize buffer (first frame only)                 */
         VXmakeimage(&tm,VX_PINT, ib[0].bbx,ib[0].chan);/* temp int image */
         for(k = 1; k < n; k++)
          for (i = tm.ylo ; i <= tm.yhi ; i++) /* sum first n-1 frames  */
            for (j = tm.xlo; j <= tm.xhi; j++)
	       tm.i[i][j] += ib[k].u[i][j];
         first = 0;
      }
        for (i = tm.ylo ; i <= tm.yhi ; i++)
          for (j = tm.xlo; j <= tm.xhi; j++){
	     tm.i[i][j] += ib[0].u[i][j];    /* add most recent frame */
	     t  = ib[n-1].u[i][j];           /* make a copy of oldest frame */ 
	     ib[n-1].u[i][j] = tm.i[i][j] / n;  /* update  oldest frame     */ 
	     tm.i[i][j] -=  t;               /* subtract oldest from sum    */ 
	  }
        VXwriteframe(VXout,ib[n-1].list);    /* write oldest frame          */

/************** End of the Application specifiec section *************/

    }                                /* end of every frame section   */
    if (first){
       fprintf(stderr,"vssum: not enough frames in image set\n");
       exit(1);
    }
    VXclose(VXin);                   /* close files                  */
    VXclose(VXout);
    exit(0);
}

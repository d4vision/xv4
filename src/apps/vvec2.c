/****************************************************************/
/*  vvec2: Template Program to illustrate how to manipulate     */
/*  (2D) vectors                                                */
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

#include "VisXV4.h"       /* VisionX structure include file     */
#include "Vutil.h"        /* VisionX support utilities          */
                          /*                                    */
VisXfile_t *VXin,         /* input file structure               */
	   *VXout;        /* output file structure              */
VisXelem_t *vxlist;       /* VisX data structure                */
                          /*                                    */
VXparam_t par[] =         /* command line structure             */
{
{    "if=",    0,          " input file  vvec2: half polygon lengths"},
{    "of=",    0,          " output file      "},
{    "-p",     0,          " inhibit packing of result "},
{     0,       0,          0}
};

#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  PFLAG  par[2].val

int
main(argc, argv)
int argc;
char *argv[];
{

VisXelem_t *vptr;       /* VisX list pointer                */
VisXelem_t *nptr;       /* VisX new element pointer         */
float *ibuf;            /* input vector pointer             */
float *vbuf;            /* output vector buffer             */
int   vblen;            /* number of elements in vbuf       */
int   ne, n, n2, n2d, i;

    VXparse(&argc, &argv, par);    /* parse the command line       */
    VXin  = VXopen(IVAL, 0);       /* open input file              */
    VXout = VXopen(OVAL, 1);       /* open the output file         */

    if(!PFLAG)VXpack(1);           /* option to inhibit packing    */

    vblen= 10000;              /* set length to an initial value      */
    vbuf = NEWN(float, vblen); /* allocate memory for vector buffer   */
	   

    while ((vxlist = VXreadframe(VXin)) != VXNIL){/* for each frame read */ 
        VXfupdate(VXout, VXin);     /* update global constants */

/****************  application section starts here **************************/
   /* this program halfs the number of points in each vector by
      skipping every second point   
    */
        vptr = vxlist;
	while ((vptr = VXfind(vptr,VX_V2D)) != VXNIL){/* for each vector */

          /* compute length of result */
	  ne =  VXnumelem(vptr);
	  n = ne/2;       /* number of x,y elements in 2D vector         */
	  n2 = n/2;       /* number of x,y elements in result 2D vector  */
	  n2d = 2 * n2;   /* number of float elements in result          */

	  /* Make sure that the vector buffer is large enough            */
	  if(vblen < n2d){
	     vblen = n2d;
	     vbuf = RENEWN(vbuf, float, vblen);
	  }

	  /* preliminaries complete, now do the actual work */
	  ibuf = (float*) VXdata(vptr);
	  for (i=0; i < n2; i++) { /* copy every other pair of elements */
             vbuf[2 * i]     = ibuf[4 * i];
             vbuf[2 * i + 1] = ibuf[4 * i + 1];
          }

	  /* make sure that a closed boundary remains closed             */
	  /* i.e., for a poly the last pt must be the same as the first  */
	  if(ibuf[0] == ibuf[ne - 2] &&ibuf[1] == ibuf[ne - 1]){
             vbuf[n2d - 2] = vbuf[0]; /* make last = first */
	     vbuf[n2d - 1] = vbuf[1];
	  }

          /* create new list element for result and add to list    */
	  nptr = VXaddelem(vptr, VX_V2D, (char*)vbuf ,n2d * sizeof(float));
	  VXdelelem(vptr);      /* delete the old vector */
	  vptr = nptr->next;    /* move to next element in the list */
        }
/****************  application section ends here   **************************/

        VXwriteframe(VXout,vxlist); /* write frame             */
        VXdellist(vxlist);          /* delete the frame        */
    }

    VXclose(VXin);                 /* close input file              */
    VXclose(VXout);                /* close output file             */
    exit(0);
}

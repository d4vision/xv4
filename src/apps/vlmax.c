/*********************************************************************/
/* vlmax      Compute local max operation on byte images             */
/*********************************************************************/

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

#include "VisXV4.h"          /* VisionX structure include file       */
#include "Vutil.h"           /* VisionX utility header files         */
VisXfile_t *VXin,            /* input file structure                 */
           *VXout;           /* output file structure                */
VisXelem_t *VXlist,*VXptr;   /* VisX data structure                  */
VXparam_t par[] =            /* command line structure               */
{
{    "if=",    0,   " input file  vlmax: compute local maximum"},
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
int        i,j;                    /* index counters                 */

    VisXimage_t tm;                /* temp image  structure          */

    VXparse(&argc, &argv, par);    /* parse the command line         */
    VXin  = VXopen(IVAL, 0);       /* open input file                */
    VXout = VXopen(OVAL, 1);       /* open the output file           */

/************ Parameter and initialization section ************/

    /* deal with any intialization and optional parameters here */

/************ End of Parameter and initialization section ************/

    while((VXlist = VXptr = VXreadframe(VXin)) != VXNIL){ /* every frame */
        VXfupdate(VXout, VXin); /* update global constants */
	/* find next byte image */
        while (VXNIL != (VXptr = VXfind(VXptr, VX_PBYTE)))  { 
            VXsetimage(&im, VXptr, VXin); /* initialize input structure */

/***************** Application specific section **********************/

            VXembedimage(&tm, &im, 1,1,1,1); /* temp image copy with border */
            for (i = im.ylo; i <= im.yhi; i++) {/* compute the function */
                for (j = im.xlo; j <= im.xhi; j++) {
                    im.u[i][j] = MAX(tm.u[i][j],
                                     MAX(tm.u[i][j+1],
                                         MAX(tm.u[i+1][j],
                                             MAX(tm.u[i][j-1], tm.u[i-1][j]))));
                }
	    }
            VXresetimage(&tm); /* free the tm image structure  */

/************** End of the Application specific section **************/

            VXresetimage(&im); /* free the im image structure  */
            VXptr = VXptr->next; /* move to the next image */
        } /* end of every image section */
        VXwriteframe(VXout,VXlist); /* write frame */
        VXdellist(VXlist);          /* delete the frame */
    } /* end of every frame section */
    VXclose(VXin);  /* close files */
    VXclose(VXout);
    exit(0);
}

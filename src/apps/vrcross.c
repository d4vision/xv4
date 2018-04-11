/****************************************************************/
/*  vrcross   Compute a roberts cross edge operation            */
/*                                                              */
/*  Example of polymorphic programming                          */
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
{    "if=",    0,   " input file  vrcross: roberts cross edge operation"},
{    "of=",    0,   " output file "},
{    "-f",     0,   " do computations in float  "},
{     0,       0,    0}
};
#define  IVAL   par[0].val
#define  OVAL   par[1].val
#define  FFLAG  par[2].val

/* list of image pixel types supported */
int VX_ptypelist[] = {VX_PBYTE, VX_PSHORT, VX_PFLOAT, VX_PDOUBLE,
			    VX_PCHAR, VX_PBIT, VX_PINT, VX_NULLT};

int
main(argc, argv)
int argc;
char *argv[];
{

VisXimage_t im;                    /* input image structure          */
VisXimage_t tm;                    /* temp image  structure          */
int	   i,j;                    /* index counters                 */
    VXparse(&argc, &argv, par);    /* parse the command line         */
    VXin  = VXopen(IVAL, 0);       /* open input file                */
    VXout = VXopen(OVAL, 1);       /* open the output file           */
    while((VXlist= VXpt = VXreadframe(VXin)) != VXNIL){/* every frame*/
      VXfupdate(VXout, VXin);      /* update global constants        */
      while((VXpt = VXfindin(VXpt,VX_ptypelist)) != VXNIL){
        VXsetimage(&im, VXpt, VXin);      /* for every image         */

/****************  Application Specific Section*************************/

       if(im.chan == 1){ /* only process single chanel images */

/***********************************************************************/
/* Macro for operation */

# define RCROSS(ot,it)  /* ot = out-type   it = in-type */              \
             for (i = im.ylo ; i <= im.yhi ; i++)                       \
              for (j = im.xlo; j <= im.xhi; j++){                       \
	         im.ot[i][j] = (ABS (tm.it[i][j] - tm.it[i+1][j+1]) +   \
	 	    	        ABS (tm.it[i+1][j] - tm.it[i][j+1]))/2; \
              }                                                      
/***********************************************************************/

	     /* Roberts Cross Operation */
           if(FFLAG){  
             VXfloatimage(&tm,&im, 0, 1, 0, 1);
	     switch(im.base){
	       case VX_BYTE:   RCROSS(u, f) break;
	       case VX_SHORT:  RCROSS(s, f) break;
	       case VX_CHAR:   RCROSS(c, f) break;
	       case VX_INT:    RCROSS(i, f) break;
	       case VX_FLOAT:  RCROSS(f, f) break;
	       case VX_DOUBLE: RCROSS(d, f) break;
	        default:        break;
             }
           }else{
             VXembedimage(&tm,&im, 0, 1, 0, 1);
	     switch(im.base){
	       case VX_BYTE:   RCROSS(u, u) break;
	       case VX_SHORT:  RCROSS(s, s) break;
	       case VX_CHAR:   RCROSS(c, c) break;
	       case VX_INT:    RCROSS(i, i) break;
	       case VX_FLOAT:  RCROSS(f, f) break;
	       case VX_DOUBLE: RCROSS(d, d) break;
	        default:        break;
             }
           }
           VXresetimage(&tm);        /* free the tm image structure  */
	}
/************* End of  Application Specific Section ********************/
      
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

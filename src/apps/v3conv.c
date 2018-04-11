/****************************************************************/
/* VisX4 program v3conv                                         */
/* Perform three-dimensional convolution                        */
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

#include "VisXV4.h"          /* VisX structure include file     */
#include "Vutil.h"           /* VisX utility header files       */

extern char *VisXhist;
VisXfile_t *VXin,            /* input file structure            */
           *kin,             /* input file structure            */
	   *VXout;           /* output file structure           */
VisXelem_t *VXlist,**VXpt;   /* VisX data structure             */
VisXelem_t *klist;           /* VisX data structure             */
VisX3dim_t  sim;             /* source image structure          */
VisX3dim_t  tim;             /* temporary image structure       */
VisX3dim_t  rim;             /* result image structure          */
VisX3dim_t  kim;             /* kernel image structure          */

VXparam_t par[] = {
    {"if=",    0,   "input file"},
    {"of=",    0,   "output file"},
    {"k=",     0,   "kernel file"},
    {"-c",     0,   "cancel auto center"},
    {"-v",     0,   "verbose flag"},
    { 0,       0,   0},
};

typedef double RTYPE;
typedef RTYPE TBO[3];

#define  IVAL    par[0].val
#define  OVAL    par[1].val
#define  KVAL    par[2].val
#define  CFLAG   par[3].val
#define  VFLAG   par[4].val

void Vconv();

int KDTYPE;
int IDTYPE;

/*
#define P_DEBUG
*/

int
main(argc, argv)
int argc;
char *argv[];
{
  int xo, yo, zo;                  /* centering offsets         */
  int vx_typelist[] = {VX_PBYTE, VX_PFLOAT, VX_NULLT};
  VisXelem_t *vptr = NULL, *kptr = NULL;
  int xlpad, xhpad, ylpad, yhpad, zlpad, zhpad;
  float padbbx[6];
  
  VXparse(&argc, &argv, par);    /* parse the command line      */

  VXin  = VXopen(IVAL, 0);       /* open input file             */
  kin  = VXopen(KVAL, 0);        /* open kernel file            */
  VXout = VXopen(OVAL, 1);       /* open the output file        */

  VXlist = VXread(VXin);         /* read input file             */

  if(VXNIL == (vptr = VXfindin(VXlist, vx_typelist))){
    fprintf(stderr, "v3conv: no acceptable input image found, exiting\n");
    exit(1);
  }
  else {
    IDTYPE = VXtype(vptr);
    if (VFLAG) {
      switch(IDTYPE)
	{
	case VX_PBYTE:
	  fprintf(stderr, "v3conv: image type is VX_PBYTE\n");
	  break;
	case VX_PFLOAT:
	  fprintf(stderr, "v3conv: image type is VX_PFLOAT\n");
	  break;
	default:
	  fprintf(stderr, "v3conv: image type is unknown, exiting\n");
	  exit(1);
	}
    }
  }

  /* Initialize input image structure */
  VXset3dim(&sim, vptr, VXin);
  if(sim.chan != 1){
    fprintf(stderr, "v3conv: multi-channel images not supported, exiting\n");
    exit(1);
  }
  
  klist = VXread(kin);           /* read kernel file            */
  
  if(VXNIL == (kptr = VXfindin(klist, vx_typelist))){
    fprintf(stderr, "v3conv: no acceptable kernel found, exiting\n");
    exit(1);
  }
  else {
    KDTYPE = VXtype(kptr);
    if (VFLAG) {
      switch(KDTYPE)
	{
	case VX_PBYTE:
	  fprintf(stderr, "v3conv: kernel type is VX_PBYTE\n");
	  break;
	case VX_PFLOAT:
	  fprintf(stderr, "v3conv: kernel type is VX_PFLOAT\n");
	  break;
	default:
	  fprintf(stderr, "v3conv: kernel type is unknown, exiting\n");
	  exit(1);
	}
    }
  }
  
  /* Initialize kernel image structure */
  VXset3dim(&kim, kptr, kin);
  VXclose(kin);

  /* Create result image structure */
  VXmake3dim(&rim, IDTYPE, sim.bbx, sim.chan);

  /* center operation */
  xo = yo = zo = 0;
  
  if(!CFLAG){
    xo = (kim.xlo + kim.xhi)/2;
    yo = (kim.ylo + kim.yhi)/2;
    zo = (kim.zlo + kim.zhi)/2;
  }

#ifdef P_DEBUG  
  if(VFLAG)
    fprintf(stderr,"Offsets xo = %d, yo = %d, zo = %d\n",xo,yo,zo);
#endif
  
  xlpad = MAX(0, kim.xhi - xo);
  xhpad = MAX(0, xo - kim.xlo) + ((kim.xlo + kim.xhi)%2);
  ylpad = MAX(0, kim.yhi - yo);
  yhpad = MAX(0, yo - kim.ylo) + ((kim.ylo + kim.yhi)%2);
  zlpad = MAX(0, kim.zhi - zo);
  zhpad = MAX(0, zo - kim.zlo) + ((kim.zlo + kim.zhi)%2);
  
#ifdef P_DEBUG  
  if(VFLAG)fprintf(stderr,"z size of kernel = %d\n",n);
  
  if(VFLAG)fprintf(stderr,"pads: x=%d,%d y=%d,%d z=%d,%d\n",
		   xlpad,xhpad,ylpad,yhpad,zlpad,zhpad);
#endif
  
  /* time to fix the history for the data file */
  VXhistset(VXin->list, 1);
  
  padbbx[0] = (float)(rim.xlo - xlpad);
  padbbx[1] = (float)(1 + rim.xhi + xhpad);
  padbbx[2] = (float)(rim.ylo - ylpad);
  padbbx[3] = (float)(1 + rim.yhi + yhpad);
  padbbx[4] = (float)(rim.zlo - zlpad);
  padbbx[5] = (float)(1 + rim.zhi + zhpad);
  
  /* temp image copy with border */
  VXembed3dim(&tim, &sim, xlpad, xhpad, ylpad, yhpad, zlpad, zhpad);
  
#ifdef P_DEBUG
  fprintf(stderr, "kim bbx = [%6.4f][%6.4f][%6.4f][%6.4f][%6.4f][%6.4f]\n",
      kim.bbx[0], kim.bbx[1], kim.bbx[2], kim.bbx[3], kim.bbx[4], kim.bbx[5]);
  fprintf(stderr, "sim bbx = [%6.4f][%6.4f][%6.4f][%6.4f][%6.4f][%6.4f]\n",
      sim.bbx[0], sim.bbx[1], sim.bbx[2], sim.bbx[3], sim.bbx[4], sim.bbx[5]);
  fprintf(stderr, "tim bbx = [%6.4f][%6.4f][%6.4f][%6.4f][%6.4f][%6.4f]\n",
      tim.bbx[0], tim.bbx[1], tim.bbx[2], tim.bbx[3], tim.bbx[4], tim.bbx[5]);
  fprintf(stderr, "rim bbx = [%6.4f][%6.4f][%6.4f][%6.4f][%6.4f][%6.4f]\n",
      rim.bbx[0], rim.bbx[1], rim.bbx[2], rim.bbx[3], rim.bbx[4], rim.bbx[5]);
#endif
  
  /* perform the 3D convolution */
  Vconv(&tim, &rim, &kim);
  
  VXwrite(VXout, rim.list);     /* write data                   */
  VXclose(VXin);                /* close files                  */
  VXclose(VXout);

  exit(0);
}


void Vconv(ip, ir, kern)
     VisX3dim_t *ip;       /* padded input image structure  */
     VisX3dim_t *ir;       /* output image structure  */
     VisX3dim_t *kern;     /* kernel image structure */
{
  int i, j, k;
  int ik, jk, kk;
  int xo, yo, zo;
  
  xo = (kern->xlo + kern->xhi)/2;
  yo = (kern->ylo + kern->yhi)/2;
  zo = (kern->zlo + kern->zhi)/2;

#ifndef P_DEBUG
  fprintf(stderr, "kern xo = %d, yo = %d, zo = %d\n", xo, yo, zo);
  fprintf(stderr, "kern xlo = %d, xhi = %d, ylo = %d, yhi = %d, "
	  "zlo = %d, zhi = %d\n", kern->xlo, kern->xhi, kern->ylo,
	  kern->yhi, kern->zlo, kern->zhi);
#endif

  if (IDTYPE == VX_PBYTE) {
    if (KDTYPE == VX_PBYTE) {
      for (k = ir->zlo; k <= ir->zhi; k++){
	for (j = ir->ylo; j <= ir->yhi; j++){
	  for (i = ir->xlo; i <= ir->xhi; i++){
	    ir->u[k][j][i] = 0;
	    
	    for (kk = kern->zlo; kk <= kern->zhi; kk++){
	      for (jk = kern->ylo; jk <= kern->yhi; jk++){
		for (ik = kern->xlo; ik <= kern->xhi; ik++){
		  if (kern->u[kk][jk][ik] != 0)
		    ir->u[k][j][i] +=
		      (kern->u[kk][jk][ik] * ip->u[k+zo-kk][j+yo-jk][i+xo-ik]);
		}
	      }
	    }
	  }
	}
      }
    }
    else if (KDTYPE == VX_PFLOAT) {
      for (k = ir->zlo; k <= ir->zhi; k++){
	for (j = ir->ylo; j <= ir->yhi; j++){
	  for (i = ir->xlo; i <= ir->xhi; i++){
	    ir->u[k][j][i] = 0;
	    
	    for (kk = kern->zlo; kk <= kern->zhi; kk++){
	      for (jk = kern->ylo; jk <= kern->yhi; jk++){
		for (ik = kern->xlo; ik <= kern->xhi; ik++){
		  if (kern->f[kk][jk][ik] != 0.0)
		    ir->u[k][j][i] +=
		      (unsigned char) (kern->f[kk][jk][ik]
				       * ip->u[k+zo-kk][j+yo-jk][i+xo-ik]);
		}
	      }
	    }
	  }
	}
      }
    }
  }
  else if (IDTYPE == VX_PFLOAT) {
    if (KDTYPE == VX_PFLOAT) {
      for (k = ir->zlo; k <= ir->zhi; k++){
	for (j = ir->ylo; j <= ir->yhi; j++){
	  for (i = ir->xlo; i <= ir->xhi; i++){
	    ir->f[k][j][i] = 0.0;
	    
	    for (kk = kern->zlo; kk <= kern->zhi; kk++){
	      for (jk = kern->ylo; jk <= kern->yhi; jk++){
		for (ik = kern->xlo; ik <= kern->xhi; ik++){
		  if (kern->f[kk][jk][ik] != 0.0)
		    ir->f[k][j][i] +=
		      (kern->f[kk][jk][ik] * ip->f[k+zo-kk][j+yo-jk][i+xo-ik]);
		}
	      }
	    }
	  }
	}
      }
    }
    else if (KDTYPE == VX_PBYTE) {
      for (k = ir->zlo; k <= ir->zhi; k++){
	for (j = ir->ylo; j <= ir->yhi; j++){
	  for (i = ir->xlo; i <= ir->xhi; i++){
	    ir->f[k][j][i] = 0.0;
	    
	    for (kk = kern->zlo; kk <= kern->zhi; kk++){
	      for (jk = kern->ylo; jk <= kern->yhi; jk++){
		for (ik = kern->xlo; ik <= kern->xhi; ik++){
		  if (kern->u[kk][jk][ik] != 0)
		    ir->f[k][j][i] +=
		      (float) (kern->u[kk][jk][ik]
			       * ip->f[k+zo-kk][j+yo-jk][i+xo-ik]);
		}
	      }
	    }
	  }
	}
      }
    }
  }
  else {
    fprintf(stderr, "v3conv: image type error\n");
  }
}

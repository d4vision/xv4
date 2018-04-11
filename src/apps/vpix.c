/****************************************************************/
/* VPIX apply a funciton to each pixel of an image		*/
/* Syntax:							*/
/*        vpix if=infile of=outfile				*/
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

/*
FFLOAT Macro */

# define FFLOAT(type,op1)                                         	\
	fbuf = (float*) Vrealloc (fbuf, num * sizeof(float));		\
        op.f = fbuf;                                			\
        ip.op1 = (type *) inputbuf;                                     \
        for (i = 0; i < num; i++)                                       \
            *op.f++ = (float) *ip.op1++;	           		\
	tmpbuf = outputbuf;						\
	outputbuf = inputbuf = (char*) fbuf;

/*
FFIX Macro */

# define FFIX(type,op1)                                         	\
        ip.f = fbuf;                                			\
	outputbuf = tmpbuf;						\
        op.op1 = (type *) outputbuf;                                    \
        for (i = 0; i < num; i++)                                       \
            *op.op1++ = (type) *ip.f++;

/*
FMAXMIN Macro */

# define FMAXMIN()                                         	\
        ip.f = (float *) inputbuf;                 			\
	vxfmax = vxfmin = *ip.f++;					\
        for (i = 1; i < num; i++) {                          		\
            vxfmax =  *ip.f > vxfmax ? *ip.f : vxfmax;			\
            vxfmin =  *ip.f < vxfmin ? *ip.f : vxfmin;			\
	    ip.f++;							\
	}


# define FSCALEX()                                         	\
        ip.f = (float *) inputbuf;                 			\
        op.f = (float *) outputbuf;                 			\
        for (i = 0; i < num; i++)                           		\
            *op.f++ =  (*ip.f++ + bf) * tf;


/*
INVERT Macro */

# define INVERT(type,op1)                                         \
        op.op1 = (type *) outputbuf;                                \
        ip.op1 = (type *) inputbuf;                                \
        for (i = 0; i < num; i++)                                       \
            *op.op1++ = ~*ip.op1++;

/*
THRESH Macro */

# define THRESH(type,op1)                                            \
     op.op1 = (type *) outputbuf;                                \
     ip.op1 = (type *) inputbuf;                                 \
     for (i = 0; i < num; i++)                                       \
      *op.op1++ = *ip.op1++ >= th.op1 ? hi.op1 : lo.op1;

/*
WINDOW Macro */

# define WINDOW(type,op1)                                        \
     op.op1 = (type *) outputbuf;                                \
     ip.op1 = (type *) inputbuf;                                 \
     mf = (hi.d - lo.d)/(th.d - tl.d);                           \
     for (i = 0; i < num; i++) {                                 \
      *op.op1 = *ip.op1 >= tl.op1 ? *ip.op1 :tl.op1;             \
      *op.op1 = *op.op1 <= th.op1 ? *op.op1 :th.op1;             \
      *op.op1 = (*op.op1 - tl.op1) * mf + lo.op1;                \
      op.op1++ ;                                                 \
      ip.op1++ ; }

/*
RANGE Macro */

# define RANGE(type,op1)                                         \
     op.op1 = (type *) outputbuf;                                \
     ip.op1 = (type *) inputbuf;                                 \
     for (i = 0; i < num; i++) {                                 \
      *op.op1 = *ip.op1 >= tl.op1 ? *ip.op1 :lo.op1;             \
      *op.op1 = *op.op1 <= th.op1 ? *op.op1 :lo.op1;             \
      op.op1++ ;                                                 \
      ip.op1++ ; }

/*
CLIP Macro */

# define CLIP(type,op1)                                            \
     op.op1 = (type *) outputbuf;                                \
     ip.op1 = (type *) inputbuf;                                 \
     for (i = 0; i < num; i++) {                                     \
      *op.op1 = *ip.op1 > hi.op1 ? hi.op1 :*ip.op1; \
      *op.op1 = *op.op1 < lo.op1 ? lo.op1 :*op.op1; \
      op.op1++ ; \
      ip.op1++ ; }

/*
MATHOP Macro */

# define MATHOP(type,op1,func)                                         \
        op.op1 = (type *) outputbuf;                                \
        ip.op1 = (type *) inputbuf;                                \
        for (i = 0; i < num; i++)                                       \
            *op.op1++ = (type) func (*ip.op1++);

float gaussr ();
double gnoise ( double arg) {
    return (arg + gaussr() );
}

VisXfile_t *VXin,         /* input file structure               */
           *VXout;        /* output file structure              */
VisXelem_t *vxlist;       /* VisX data structure                */

VXparam_t par[] = /* command line structure   */
{
    {"if=",    0,   "input file    vpix: primitive pixel operations"},
    {"of=",    0,   "output file"},
    {"th=",    0,   "threshold value"},
    {"tl=",    0,   "low threshold value"},
    {"hi=",    0,   "high pixel value in output"},
    {"lo=",    0,   "low pixel value in output"},
    {"bf=",    0,   "additive factor"},
    {"tf=",    0,   "multiplicative factor"},
    {"gn=",    0,   "add gaussian noise"},
    {"rs=",    0,   "random seed for the gn= option"},
    {"-neg",   0,   "logically negate all pixel values"},
    {"-log",   0,   "compute log of all pixel values"},
    {"-sqrt",  0,   "compute square-root of all pixel values"},
    {"-scale", 0,   "scale output from lo to hi"},
    {"-clip",  0,   "clip from tl (-> lo) to th (-> hi)"},
    {"-range", 0,   "clip from tl to th, set other pixels to lo"},
    {"-wind",  0,   "clip from tl to th and scale from lo to hi"},
    {"-v",     0,   "verbose operation"},
    {0,        0,   ""},
};

#define  IVAL     par[0].val
#define  OVAL     par[1].val
#define  TVAL     par[2].val
#define  TLVAL    par[3].val
#define  HIVAL    par[4].val
#define  LOVAL    par[5].val
#define  BFVAL    par[6].val
#define  TFVAL    par[7].val
#define  GNVAL    par[8].val
#define  RSVAL    par[9].val
#define  INFLAG   par[10].val
#define  LGFLAG   par[11].val
#define  SQFLAG   par[12].val
#define  SCFLAG   par[13].val
#define  CLFLAG   par[14].val
#define  RANFLAG  par[15].val
#define  WINFLAG  par[16].val
#define  VFLAG    par[17].val

/*
 * VisXpt_t -- pixel element pointer
 */
typedef struct visxppt{
unsigned char *u;
char *c;
short *s;
long *l;
float *f;
double *d;
} VisXpt_t;

VisXpt_t ip, op;

typedef struct visxpv{
unsigned char u;
char c;
short s;
long l;
float f;
double d;
} VisXp_t;

VisXp_t lo, hi, th, tl;

char   *inputbuf,
       *outputbuf,
       *tmpbuf;
float  *fbuf;
float vxfmax, vxfmin;
float tf, bf;
float thresh;   /* threshold  */
float sd, seed; /* std dev and seed for gn= */

int
main(argc, argv)
int argc;
char *argv[];
{
int stype;	/* source type code */
int fstype;	/* source type code for original when -f */
int num;	/* elements/vector */
int i;		/* element counter */
int fflag;	/* float calc flag */
float mf;       /* range factor for window */

VisXelem_t *ptr;
VisXelem_t *nptr;
       VXparse(&argc, &argv, par);     		/* parse the command line */
       VXin  = VXopen(IVAL, 0);       /* open input file              */
       VXout = VXopen(OVAL, 1);       /* open the output file         */

       th.d = 1.0;
       if(TVAL)
	   th.d = atof(TVAL);
       th.c = th.f = th.d;
       th.u = th.s = th.l = th.d;
       hi.d = 1.0;
       tl.d = 1.0;
       if(TLVAL)
	   tl.d = atof(TLVAL);
       tl.c = tl.f = tl.d;
       tl.u = tl.s = tl.l = tl.d;
       hi.d = 1.0;
       if(HIVAL)
	   hi.d = atof(HIVAL);
       hi.c = hi.f = hi.d;
       hi.u = hi.s = hi.l = hi.d;
       lo.d = 0.0;
       if(LOVAL)
	   lo.d = atof(LOVAL);
       lo.c = lo.f = lo.d;
       lo.u = lo.s = lo.l = lo.d;
       tf = 1.0;
       if(TFVAL)
	   tf = atof(TFVAL);
       bf = 0.0;
       if(BFVAL)
	   bf = atof(BFVAL);
       sd = 0.0;
       if(GNVAL)
	   sd = atof(GNVAL);
       if (RSVAL) {
           if ( 0 != strlen(RSVAL) ) {
               srand((int) atof(RSVAL));
	   } else {
               srand((int) getpid());
	   }
       }

	   fbuf = (float *) Vmalloc(sizeof(float)); /* intialize fbuf */

       while ((vxlist = VXreadframe(VXin)) != VXNIL){
                                         /* for each frame read     */
         VXfupdate(VXout, VXin);         /* update global constants */

         /* find each pixel element in the frame   */
         ptr = VXfirst(vxlist);
         while (ptr != VXNIL){
              switch(ptr->type){
                  case VX_PBYTE:
                  case VX_PSHORT:
                  case VX_PFLOAT:
                  case VX_PDOUBLE:
                  case VX_PBIT:
                  case VX_PINT:
                                break;
              default: ptr = ptr->next;
                       continue;
              }


             /* set the source type  */
             stype = ptr->type;


              inputbuf = ptr->data;
              outputbuf = Vmalloc(ptr->size);

              num = VXnumelem(ptr);
              if(VFLAG) fprintf(stderr, "num is %d\n", num);



		/* process the data       */
	      fflag =  (LGFLAG || SQFLAG || SCFLAG || TFVAL || BFVAL || GNVAL);
		if (fflag){
		     fstype = stype;
		     stype = VX_PFLOAT;
		switch(fstype){
		    case  VX_PBYTE: if(!HIVAL) hi.u = 255;
		                    if(!HIVAL) hi.d = 255.0;
		                    if(!HIVAL) hi.f = 255.0;
		                    if(!TVAL)  th.u = 255;
		                    if(!TVAL)  th.f = 255.0;
		                    if(!TVAL)  th.d = 255.0;
				    if( GNVAL ) RANFLAG = "1";
		                      FFLOAT( unsigned char, u) break;
		    case  VX_PSHORT:  FFLOAT( short, s)         break;
		    case  VX_PDOUBLE: FFLOAT( double, d)        break;
		    case  VX_PCHAR:  FFLOAT( char, c)          break;
		    case  VX_PINT:   FFLOAT( long, l)          break;
		    case  VX_PFLOAT:  			         break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
		}
		}
		if(INFLAG){
		  switch(stype){
		    case  VX_PBYTE: INVERT( unsigned char, u) break;
		    case  VX_PSHORT: INVERT( short, s) break;
		    case  VX_PCHAR: INVERT( char, c) break;
		    case  VX_PINT:  INVERT( long, l) break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(LGFLAG){
		  switch(stype){
		    case  VX_PBYTE: 
		    case  VX_PSHORT:
		    case  VX_PCHAR:
		    case  VX_PINT: 
		    case  VX_PFLOAT: MATHOP( float, f, log) break;
		    case  VX_PDOUBLE:MATHOP( double, d, log) break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(SQFLAG){
		  switch(stype){
		    case  VX_PBYTE: 
		    case  VX_PSHORT:
		    case  VX_PCHAR:
		    case  VX_PINT: 
		    case  VX_PFLOAT: MATHOP( float, f, sqrt) break;
		    case  VX_PDOUBLE:MATHOP( double, d, sqrt) break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(TVAL && !(WINFLAG || RANFLAG)){
		  switch(stype){
		    case  VX_PBYTE: if(!HIVAL) hi.u = 255;
		                      THRESH( unsigned char, u) break;
		    case  VX_PSHORT: THRESH( short, s) break;
		    case  VX_PCHAR: THRESH( char, c) break;
		    case  VX_PINT:  THRESH( long, l) break;
		    case  VX_PFLOAT:  THRESH( float, f) break;
		    case  VX_PDOUBLE: THRESH( double, d)break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(GNVAL){
		  switch(stype){
		    case  VX_PBYTE: 
		    case  VX_PSHORT:
		    case  VX_PCHAR:
		    case  VX_PINT: 
		    case  VX_PFLOAT: MATHOP( float, f, gnoise) break;
		    case  VX_PDOUBLE:MATHOP( double, d, gnoise) break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(RANFLAG){ /* range operation */
		  switch(stype){
		    case  VX_PBYTE: if(!HIVAL) hi.u = 255;
		                    if(!HIVAL) hi.d = 255.0;
		                    if(!TVAL)  th.u = 255;
		                    if(!TVAL)  th.d = 255.0;
		                      RANGE( unsigned char, u) break;
		    case  VX_PSHORT: RANGE( short, s) break;
		    case  VX_PCHAR: RANGE( char, c) break;
		    case  VX_PINT:  RANGE( long, l) break;
		    case  VX_PFLOAT:  RANGE( float, f) break;
		    case  VX_PDOUBLE: RANGE( double, d)break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(WINFLAG){ /* window operation */
		  switch(stype){
		    case  VX_PBYTE: if(!HIVAL) hi.u = 255;
		                    if(!HIVAL) hi.d = 255.0;
		                    if(!TVAL)  th.u = 255;
		                    if(!TVAL)  th.d = 255.0;
		                      WINDOW( unsigned char, u) break;
		    case  VX_PSHORT: WINDOW( short, s) break;
		    case  VX_PCHAR: WINDOW( char, c) break;
		    case  VX_PINT:  WINDOW( long, l) break;
		    case  VX_PFLOAT:  WINDOW( float, f) break;
		    case  VX_PDOUBLE: WINDOW( double, d)break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(CLFLAG){
		  switch(stype){
		    case  VX_PBYTE: if(!HIVAL) hi.u = 255;
		                      CLIP( unsigned char, u) break;
		    case  VX_PSHORT: CLIP( short, s) break;
		    case  VX_PCHAR: CLIP( char, c) break;
		    case  VX_PINT:  CLIP( long, l) break;
		    case  VX_PFLOAT:  CLIP( float, f) break;
		    case  VX_PDOUBLE: CLIP( double, d)break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if(SCFLAG|| BFVAL ||TFVAL){  /* scale option */
		  switch(stype){
		    case  VX_PBYTE: 
				     if(!HIVAL) hi.f = (float)255.9;
		    case  VX_PSHORT:
		    case  VX_PCHAR:
		    case  VX_PINT: 
		    case  VX_PFLOAT:
			     if(SCFLAG){
			  	FMAXMIN()
				tf = (hi.f - lo.f)/(vxfmax - vxfmin);
				bf =  lo.f / tf - vxfmin;
			     }
	     if(VFLAG) fprintf(stderr, "vxfmin is %f vxfmax is %f\n", vxfmin, vxfmax);
	     if(VFLAG) fprintf(stderr, "bf is %f tf is %f\n", bf, tf);
				FSCALEX()
			  	break;
		    case  VX_PDOUBLE:
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
	 	   }
		   inputbuf = outputbuf;
		}
		if (fflag) {
		stype = fstype;
		switch(stype){
		    case  VX_PBYTE:  FFIX( unsigned char, u) break;
		    case  VX_PSHORT:  FFIX( short, s)         break;
		    case  VX_PDOUBLE: FFIX( double, d)        break;
		    case  VX_PCHAR:  FFIX( char, c)          break;
		    case  VX_PINT:   FFIX( long, l)          break;
		    case  VX_PFLOAT:  			       break;
		   default:
			(void) fprintf(stderr, "vpix: format not supported\n");
			exit(1);
		}
		}
             nptr = VXlnkelem(ptr, stype, outputbuf, ptr->size);
             VXdelelem(ptr);
             ptr = nptr->next;
	     if(VFLAG) fprintf(stderr, "inputbuf is %d\n", (int)*inputbuf);
	     if(VFLAG) fprintf(stderr, "outputbuf is %d\n",(int) *outputbuf);
             }
       VXwriteframe(VXout,vxlist); /* write frame             */
       VXdellist(vxlist);          /* delete the frame        */
       }
       exit(0);
}



float gaussr() {
/*
        This is a gaussian random number generator. The method
        used is based on the central limit theorem whereby 12
        uniformly distributed ramdon numbers are added to
        obtain a gaussian random number.
 
        The user is allowed to set the seed in the random number
        generator by calling srandom() in the main program.
                                                                */
    float   val,
            aa,
            bb;
    int     ii;
 
    val = 32767.0;
    aa = 0.0;
    for (ii = 0; ii < 12; ii++) {
        bb = 077777 & rand();
        aa += bb / val;
    }
    aa = (aa - 6.0) * sd;
    return (aa);
}


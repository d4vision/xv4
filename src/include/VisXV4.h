/*
 * V4.h	--  VisX V4 format key definitions
 */


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

#ifndef _h_visx4
#define _h_visx4

#include "cdefs.h"

#define VX_magic  "#VX"
__BEGIN_CDECLS

#include "VisXtype.h"

/*
 *   General rule: last digit indicates data type according to
 *
 *	0 = text,   1 = unsigned byte,  2 = short
 *	3 = float,  4 = double,         5 = signed byte
 *	6 = bit     7 = long (4 byte)
 *	8 = mixed type structures       9 = binary flag (no data)
 *	a = scalar value
 *      b,c,d,e,f... reserved for future use
 */

/* base types 

        type         value		description
*/
#define VX_TXT       0x0     /*  Character string type             */
#define VX_BYTE      0x1     /*  Unsigend byte type                */
#define VX_SHORT     0x2     /*  16-bit signed int type            */
#define VX_FLOAT     0x3     /*  Float type                        */
#define VX_DOUBLE    0x4     /*  Double type                       */
#define VX_CHAR      0x5     /*  Signed byte type                  */
#define VX_BIT       0x6     /*  Packed bit type                   */
#define VX_INT       0x7     /*  32-bit signed int type            */
#define VX_NULL      0x9     /*  Null base type                    */
#define VX_SCALAR    0xa     /*  Scalar base type                  */


/* VisX file defines */
#ifdef VBIG_ENDIAN
#define VX_MAGIC    "#VisX\n" /*   VisX file Identifier              */
#define VX_MAGICX   "#vIsX\n" /*   VisX file Identifier other endian */
#else
#define VX_MAGIC    "#vIsX\n" /*   VisX file Identifier              */
#define VX_MAGICX   "#VisX\n" /*   VisX file Identifier other endian */
#endif

#define VX_ELEMX    8       /*   VisX element base size bytes        */


extern int VisXbase[16]; /* base sizes in bytes   */
extern int VisXfree;     /* delete option         */
extern int VisXupackopt; /* auto unpacking option */
extern int VisXpackopt;  /* auto packing option   */

/*
 * VisXelem_t -- list element structure
 */
typedef struct visxels{
int	type;
int	size;
struct visxels *next;
struct visxels *prev;
char   *data;
} VisXelem_t;

#define VXNIL            ((VisXelem_t *) 0)

/*
 * VisXvlist_t --  "virtual" list header structure
 * (internal use only)
 */
typedef struct visxvls{
VisXelem_t * list;
} VisXvlist_t;
/*
 * VisXfile_t -- file information
 */
typedef struct visxfls{ /* internal file information */
int	fd;             /* file descriptor */
int	access;         /* access used to open files */
int	mode;           /* 0=v4_read 1=V4_write  3=v3_read 4=v3_write  */
VisXelem_t * list;      /* additional attributes list */
int first;              /* file preamble not yet written (read)  */
                        /* for maintaining frame constants       */
VisXelem_t * last;      /* last elem of list -- used as a marker */
VisXvlist_t * vlist;    /* structure for "virtual" file access   */
int     bswap;          /* flag to indicate byte swapping required */
} VisXfile_t;

/*
 * VisXimage_t -- image structure
 */
typedef struct VisXimage_t {
VisXelem_t * list;
int xlo, xhi, ylo, yhi;
float bbx[6];
char *data;
int base;
int type;
char ** indx; /* array index vector */
unsigned char **u;
short  **s;
float  **f;
double **d;
char   **c;
unsigned char **b;
int    **i;
int    chan;
VisXelem_t * glist; /* global vars for setimage */
VisXelem_t * imitem; /* location of image in list for setimage */
} VisXimage_t;

/*
 * VisX3dim_t -- 3D image structure
 */
typedef struct VisX3dim_t {
VisXelem_t * list;
int xlo, xhi, ylo, yhi, zlo, zhi;
float bbx[6];
/* char *data;             */
int base;     /* base type */
int type;     /* pixel type */
char *** indx; /* array index vector */
unsigned char ***u;
short         ***s;
float         ***f;
double        ***d;
char          ***c;
unsigned char ***b;
int           ***i;
int    chan;
VisXelem_t * glist;  /* global vars for setimage */
VisXelem_t * imitem; /* location of image in list for setimage */
VisXimage_t * image;
} VisX3dim_t;

/*
 * VisXbase_t -- Base type for image macros
 */
typedef struct VisX3base_t {
unsigned char u;
short         s;
float         f;
double        d;
char          c;
unsigned char b;
int           i;
}VisXbase_t;

/*
 * Vparse parameter list
 */
typedef struct {
  char *par;
  char *val;
} VisXparam_t;

/*
 * Vparse parameter list
 */
typedef struct {
  char *par;
  char *val;
  char *def;
} VXparam_t;

typedef unsigned char pixel_t;


/*
 * Resolution information defines and struct
 */

#define VX_UNIT_PX 0
#define VX_UNIT_MM 1


typedef struct {
    int bbxfl;
    int xl, xh , yl, yh, zh, zl;
    int szfl;
    int xs, ys, zs;
    int unit;
    double xres, yres, zres;
    int rsclfl;
    float ri,rs;
} VisXiinfo_t;



/*
 * Support Macros
 *
 */

#define VXnumelem(a) ((a->size) / VisXbase[ (a->type & 0xf)])
#define VXbase(a)    (a->type & 0xf)
#define VXtype(a)    (a->type )
#define VXsize(a)    (a->size )
#define VXdata(a)    (a->data)
/* #define VXdata(a)    ((void*)a->data) */

#define VXitextf(a)  (a->mode == 82)

/*
 * Function prototypes
 */
   /* list management  */
extern VisXelem_t *      VXinit         __P(());
extern void              VXdellist      __P((VisXelem_t *));
extern VisXelem_t *      VXfirst        __P((VisXelem_t *));
extern VisXelem_t *      VXlast         __P((VisXelem_t *));
extern VisXelem_t *      VXdelelem      __P((VisXelem_t *));
extern VisXelem_t *      VXaddelem      __P((VisXelem_t *, int, char *, int));
extern VisXelem_t *      VXlnkelem      __P((VisXelem_t *, int, char *, int));
extern VisXelem_t *      VXfind         __P((VisXelem_t *, int ));
extern VisXelem_t *      VXfindin       __P((VisXelem_t *, int * ));
extern VisXelem_t *      VXbfind        __P((VisXelem_t *, int ));
extern VisXelem_t *      VXmovelem      __P((VisXelem_t *, VisXelem_t *));
extern void              VXlnklist      __P((VisXelem_t *, VisXelem_t *));
extern void              VXjoin         __P((VisXelem_t *, VisXelem_t *));
extern VisXelem_t *      VXdelobject    __P((VisXelem_t *));
extern VisXelem_t *      VXcpy          __P((VisXelem_t *));
extern void              VXpklist       __P((VisXelem_t * ));
extern void              VXupklist      __P((VisXelem_t * ));
extern void	         VXpack	        __P((int));
extern void	         VXupack        __P((int));
extern void              VXframeset     __P((VisXelem_t *, int ));
extern void              VXbsdata       __P((VisXelem_t *));

    /* image management */
extern int               VXsetimage     __P((VisXimage_t *, VisXelem_t *, VisXfile_t *));
extern int               VXmakeimage    __P((VisXimage_t *, int, float *, int));
extern int               VXembedimage   __P((VisXimage_t *, VisXimage_t *, int, int, int, int));
extern int               VXfloatimage   __P((VisXimage_t *, VisXimage_t *, int, int, int, int));
extern void              VXufloatimage  __P((VisXimage_t *, VisXimage_t * ));
extern void              VXresetimage   __P((VisXimage_t *));
extern void              VXbitimage     __P((VisXimage_t *));
extern VisXimage_t *     VXsbufimage    __P((VisXfile_t *, int));
extern int               VXrbufimage    __P((VisXimage_t *, VisXfile_t *, int));
extern int               VXidximage     __P((int));
extern int               VXpchan        __P((VisXelem_t *));
extern float             VXpzval        __P((VisXelem_t *));
#ifdef sgi
extern void              VXpzset        ();
#else
#ifdef WIN32
extern void              VXpzset        (); /* RJP - keeps complaining about arg 2 */
#else
extern void              VXpzset        __P((VisXelem_t *, float));
#endif
#endif


   /* 3D image management */
extern int               VXset3dim      __P((VisX3dim_t *, VisXelem_t *, VisXfile_t *));
extern int               VXmake3dim     __P((VisX3dim_t *, int, float *, int));
extern int               VXembed3dim    __P((VisX3dim_t *, VisX3dim_t *, int, int, int, int, int, int));
extern int               VXfloat3dim    __P((VisX3dim_t *, VisX3dim_t *, int, int, int, int, int, int));
extern void              VXufloat3dim   __P((VisX3dim_t *, VisX3dim_t * ));
extern void              VXreset3dim    __P((VisX3dim_t *));
extern void              VXbit3dim      __P((VisX3dim_t *));

   /* File i/o */
extern VisXfile_t *      VXopen         __P((char* , int));
extern void              VXclose        __P((VisXfile_t *));
extern void              VXfupdate      __P((VisXfile_t *, VisXfile_t *));
extern VisXelem_t *      VXread         __P((VisXfile_t *));
extern VisXelem_t *      VXreadframe    __P((VisXfile_t *));
extern void              VXwrite        __P((VisXfile_t *, VisXelem_t *));
extern void              VXwriteframe   __P((VisXfile_t *, VisXelem_t *));
extern VisXelem_t *      VXreadelem     __P((VisXfile_t *));
extern void              VXwritelem     __P((VisXfile_t *, VisXelem_t *));
extern void              VXhistset      __P((VisXelem_t *, int));

   /* "virtual" File i/o */
extern VisXelem_t *      VXvreadelem	__P(( VisXfile_t *));
extern VisXelem_t *	 VXvread 	__P(( VisXfile_t *));
extern char *		 VXvdata	__P(( VisXelem_t *, VisXfile_t * ));
extern int 	         VXvsetimage	__P(( VisXimage_t *, VisXelem_t *, VisXfile_t *));
extern void	         VXvselectim	__P(( VisXimage_t *, int idx,  VisXfile_t *));

   /* V4 utilities */
extern char *            Vcvtbytobp     __P((char*, int));
extern char *            Vcvtbptoby     __P((char*, int));
extern float *           VXfloatbuf     __P((VisXelem_t * ));

   /* dynamic memory management */
extern ptr_t	Vmalloc		__P((unsigned));
extern ptr_t	Vcalloc		__P((unsigned, unsigned));
extern ptr_t	Vrealloc	__P((ptr_t, unsigned));
extern void	Vfree		__P((ptr_t));
extern ptr_t	Varray		__P((int, ...));

   /* VisX-3 utilities */
extern pixel_t *Vcvt1to1	__P((pixel_t *, int, int));
extern pixel_t *Vcvt1to8	__P((pixel_t *, int, int));
extern pixel_t *Vcvt8to1	__P((pixel_t *, int, int));
extern pixel_t *Vcvt8to8	__P((pixel_t *, int, int));
extern void 	Vcvtflip	__P((pixel_t *, int));
extern pixel_t *Vcvtpad		__P((pixel_t *, int, int, int, int));
extern pixel_t *Vcvtswab	__P((pixel_t *, int, int));
extern void	Vdith_setup	__P((int, int, int, int, 
				     pixel_t *, pixel_t *, pixel_t *));
extern int	Vdith_color	__P((double, double, double));
extern void	Vdith_3		__P((int, int, pixel_t *, pixel_t *, 
				     pixel_t *, pixel_t *));
extern void	Vdith_rgb	__P((int, int, pixel_t *, pixel_t *));
extern void	Vendfft		__P((void));
extern void	Vfft		__P((float *, float *, int));
extern void	Vifft		__P((float *, float *, int));
extern int	Vfilter		__P((int, char *));
extern char *	Vtilde		__P((char *));
extern char *	Vfname		__P((char *));

  /* basic utility functions */
extern char *	Vgetdir		__P((char *));
extern int	Vparse		__P((int *, char ***, VisXparam_t *));
extern void	Vusage		__P((int, char **, VisXparam_t *));
extern void	Vfusage		__P((int, char **, VisXparam_t *));
extern int	VXparse		__P((int *, char ***, VXparam_t *));
extern void	VXusage		__P((int, char **, VXparam_t *));
extern void	VXfusage	__P((int, char **, VXparam_t *));
extern int	Veof		__P((void));
extern int	Vread		__P((int, char *, int));
extern int      Vwrite          __P((int, char *, int));
extern void	Vstrtoxy	__P((char *, int *, int *));
extern void	Verror		__P((char *, ...));
extern int	Vseterror	__P((int));
extern char *	Verrorstr	__P((void));

__END_CDECLS

#endif

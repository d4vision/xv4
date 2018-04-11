/* $Header$ */
/* 
 * VisX.h -- visx picture header description.
 *
 * $Log$
 *
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

#ifndef _h_visx
#define _h_visx

#ifndef BSD
# include <string.h>
#else
# include <strings.h>
#endif

#include "cdefs.h"

__BEGIN_CDECLS

typedef short 	twobyte;    /* for VAX and 11/70 			*/
typedef long	forbyte;    /* for VAX and 11/70 			*/
typedef char	onebyte;    /* for VAX and 11/70 			*/
#ifndef _h_visx4
typedef unsigned char pixel_t;
#endif

#define VisXMAGIC 052525     /* visx file indentification code		*/

#define P_TSIZE	416	    /* size of title				*/
#define P_ESIZE 68	    /* size of extra data			*/
#define P_MSIZE 4	    /* size of machine name field		*/

typedef struct {
    twobyte	hdr;	    /*   0 magic number to indicate visx file	*/
    twobyte	fmt;	    /*   2 format descriptor of this file	*/
    twobyte	xsize;	    /*   4 number pixels/line 			*/
    twobyte	ysize;	    /*   6 number of lines 			*/
    twobyte	zsize;	    /*   8 number of bits/pixel 		*/
    twobyte	nchan;	    /*  10 number multi-spectral chans in file 	*/
    twobyte	uchan;	    /*  12 number multi-spectral chans in use 	*/
    twobyte	uid;	    /*  14 owner/creater's UID 			*/
    forbyte 	crdate;     /*  18 creation date 			*/
    twobyte	modified;   /*  20 flag -- file has a physical header 	*/
    twobyte	zero;	    /*  22 zero here was the title before 	*/
    onebyte 	machine[P_MSIZE]; /*  24 machine name 			*/

    onebyte	title[P_TSIZE];	/* 443 character identification title	*/

    onebyte	extra[P_ESIZE];	/* 511 extra for "format specific" data	*/
} VisXhdr_t;


/*
 * Vparse parameter list
 */
#ifndef _h_visx4
typedef struct {
    char *par;
    char *val;
} VisXparam_t;
#endif

/* 
 * Vopen parameters 
 */
#define READ	00	/* open for reading				*/
#define WRITE	01	/* open for writing				*/
#define MODIFY  02	/* rewrite-in-place access (exisiting file) 	*/
#define	RANDSK  03	/* read and seek input                      	*/
#define WANDSK  04	/* write and seek output                    	*/
#define NMODIFY 05	/* rewrite-in-place access (new file)       	*/

/*
 * Visx Formats
 */
#define VFMT_ZERO	0x00	/* invalid 			*/

#define VFMT_UBYTE	0x01	/* unsigned byte		*/
#define VFMT_SHORT	0x02	/* short integer		*/
#define VFMT_FLOAT	0x03	/* single precision float	*/
#define VFMT_DOUBLE	0x04	/* double precision float	*/
#define VFMT_SBYTE	0x05	/* signed byte			*/
#define VFMT_BPL	0x06	/* bit plane			*/
#define VFMT_LONG	0x07	/* long integer			*/

#define VFMT_ASCII	0x0a	/* ascii text			*/
#define VFMT_SET	0x64	/* image set			*/

#define VFMT_GRAPH	0x73	/* graph			*/
#define VFMT_FEAT_VEC	0x79	/* feature vector		*/
#define VFMT_CHAIN_CODE 0x7c	/* chain code 			*/
#define VFMT_BIT_VEC	0x7e	/* bit vector			*/

#define _VFMT_VEC	0x80	/* Generic vector format	*/
#define VFMT_UBYTE_VEC	0x81	/* unsigned byte vector		*/
#define VFMT_SHORT_VEC	0x82	/* short integer vector		*/
#define VFMT_FLOAT_VEC	0x83	/* single precision float vector*/
#define VFMT_DOUBLE_VEC 0x84	/* double precision float vector*/
#define VFMT_SBYTE_VEC	0x85	/* signed byte vector		*/
#define VFMT_BPL_VEC	0x86	/* bit plane vector		*/
#define VFMT_LONG_VEC	0x87	/* long integer vector		*/



typedef struct {
        int number;
        char *name;
        char *shortname;
} VisXformat_t;

extern VisXformat_t visxfmt[];

/*
 *   VisX graph format key definitions
 *
 *   General rule: last digit indicates data type according to
 *
 *		0 = text, 1 = unsigned byte, 2 = short
 *		3 = float 4 = double, 5 = signed byte
 *		6 = bit   7 = long (4 byte)
 *		8 = mixed type structures 9 = binary flag (no data)
 *
 *      key    value		description
 */
#define	VFMT_G_V2D	0x33	/*  2 dimensional verices x,y   	*/
#define	VFMT_G_V3D	0x43	/*  3 dimensional verices x,y,z 	*/
#define	VFMT_G_INVY	0x39	/*  Inverted Y scale (ymax-not known) 	*/
#define	VFMT_G_ID	0x07	/*  Object Identifier		 	*/


/*
 * Macros
 */
#define Vlinesize(a)	(((a)->xsize * (a)->nchan * (a)->zsize) >> 3)
/*
 * Function prototypes
 */

extern ptr_t	Vmalloc		__P((unsigned));
extern ptr_t	Vcalloc		__P((unsigned, unsigned));
extern ptr_t	Vrealloc	__P((ptr_t, unsigned));
extern void	Vfree		__P((ptr_t));
extern ptr_t	Varray		__P((int, ...));
extern void	Vclose		__P((int, char *, int));
extern void	Vcopy		__P((int, int));
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
extern void	Verror		__P((char *, ...));
extern int	Vseterror	__P((int));
extern char *	Verrorstr	__P((void));
extern void	Vendfft		__P((void));
extern void	Vfft		__P((float *, float *, int));
extern void	Vifft		__P((float *, float *, int));
extern int	Vfilter		__P((int, char *));
extern void	Vparam		__P((char *, VisXhdr_t *));
extern char *	Vtilde		__P((char *));
extern char *	Vfname		__P((char *));
extern char *	Vgetdir		__P((char *));
extern int	Vpathopen	__P((char *, char *, VisXhdr_t *, int));
extern int	Vheader		__P((int, VisXhdr_t *));
extern void	Vmake		__P((VisXhdr_t *, char *, 
				     int, int, int, int, int));
extern int	Vopen		__P((char *, VisXhdr_t *, int));
#ifndef _h_visx4
extern int	Vparse		__P((int *, char ***, VisXparam_t *));
extern void	Vusage		__P((int, char **, VisXparam_t *));
#endif
extern int	Veof		__P((void));
extern int	Vread		__P((int, char *, int));
extern void	Vreverse	__P((char *));
extern void	Vstrtoxy	__P((char *, int *, int *));
extern void	Vtitle		__P((char *, char *));
extern int	Vvbuf		__P((VisXhdr_t *, char **, char **));
extern int	Vget		__P((int, VisXhdr_t *, float *, char *, 
				     int, int));
extern int	Vput		__P((int, VisXhdr_t *, float *, char *, long));
extern int	Vvread		__P((int, VisXhdr_t *, char *, char *));
extern long	Vvsread		__P((int, VisXhdr_t *, char *));
extern int	Vvwrite		__P((int, VisXhdr_t *, char *, char *, long));
extern int	Vwrite		__P((int, char *, int));

__END_CDECLS

#endif

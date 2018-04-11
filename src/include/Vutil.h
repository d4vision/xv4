/*
 *  Vutil.h  standard utilities for VisionX V4
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


#ifndef _h_util
#define _h_util

#ifndef BSD
# include <string.h>
#else
# include <strings.h>
#endif

#ifndef MAXFLOAT
#include <math.h>
#endif

#ifndef FILE 
#include <stdio.h>
#endif 
/* end  FILE */

#include "cdefs.h"


#ifndef TRUE
# define TRUE 			1
# define FALSE 			0
#endif
/* end  TRUE */

#ifndef __cplusplus
# define private			static
# define public
#endif

#define NEW(a)			(a *) Vmalloc(sizeof(a))
#define NEWN(a, n)		(a *) Vmalloc(sizeof(a) * (unsigned) (n))
#if __STDC__
# define RENEWN(o, a, n)        (a *) Vrealloc((void *) (o), sizeof(a) * \
					      (unsigned) (n))
# define DISPOSE(a)		Vfree((void *) (a))
# define SIZEOF(a)		MemPtr((void *) (a))
#else
# define RENEWN(o, a, n)        (a *) Vrealloc((char *) (o), sizeof(a) * \
					      (unsigned) (n))
# define DISPOSE(a)		Vfree((char *) (a))
# define SIZEOF(a)		MemPtr((char *) (a))
#endif
#ifndef NIL
# define NIL(a)			((a *) 0)
#endif
#ifndef MIN
# define MIN(a, b) 		((a) > (b) ? (b) : (a))
#endif
#ifndef MAX
# define MAX(a, b) 		((a) < (b) ? (b) : (a))
#endif
#ifndef ABS
# define ABS(a)			((a) < 0 ? -(a) : (a))
#endif
#ifndef SGN
# define SGN(a)			((a) > 0 ? 1 : ((a) < 0 ? -1 : 0))
#endif
#ifndef SQR
# define SQR(a)			((a) * (a))
#endif
#ifndef SWAP
# define SWAP(a,b)		(a ^= b, b ^= a, a ^= b)
#endif
#ifndef MASK
# define MASK(a)		((1 << (a)) - 1)
#endif
#ifndef LOW
# define LOW(x)			((x) & 0xff)
#endif
#ifndef HIGH
# define HIGH(x)		(((x) >> 8) & 0xff)
#endif

/*
 * To give a good debugging message and die
 */
#ifdef DEBUG
#define ASSERT(a, m)		if (!(a)) { \
				    (void) fprintf(stderr, \
					"%s (%s at %d), aborting\n", \
					(m), __FILE__, __LINE__); \
				    (void) abort(); \
				} else ;
#else
#define ASSERT(a, m)		if (!(a)) { \
				    (void) fprintf(stderr, \
					"%s (%s at %d), exiting\n", \
					(m), __FILE__, __LINE__); \
				    (void) exit(-1); \
				} else ;
			
#endif
				
#ifndef BSD
# define PROGNAME(b)  ((((char *) strrchr(*(b), '/')) != NIL(char)) ? \
		        ((char *) (strrchr(*(b), '/') + 1)) : *(b))
#else
# define PROGNAME(b)  ((((char *) rindex(*(b), '/')) != NIL(char)) ? \
		        ((char *) (rindex(*(b), '/') + 1)) : *(b))
#endif
/*
 *	Utilties for setjmp, longjump
 */
#define CHECKPOINT(m, v)	if ( setjmp(v) ) { \
				    (void) fprintf(stdout, "%s\n", (m)); \
				    return; \
				} 
#define ABORT(c, m, v)		if ( !(c) ) { \
				    (void) fprintf(stdout, "%s, ", (m)); \
				    longjmp(v, 1); \
				} 


#define forever()		for(;;)
#define Vstrdup(_s)		strcpy((char *) \
				    Vmalloc((unsigned) (strlen(_s) + 1)), _s)

/* some useful constants */
#ifndef M_PI		/* hack for 4.2 or 4.3 (4.2 has it 4.3 not) */
# define M_E			2.7182818284590452354
# define M_LOG2E		1.4426950408889634074
# define M_LOG10E		0.43429448190325182765
# define M_LN2			0.69314718055994530942
# define M_LN10			2.30258509299404568402
# define M_PI			3.14159265358979323846
# define M_PI_2			1.57079632679489661923
# define M_PI_4			0.78539816339744830962
# define M_1_PI			0.31830988618379067154
# define M_2_PI			0.63661977236758134308
# define M_2_SQRTPI		1.12837916709551257390
# define M_SQRT2		1.41421356237309504880
# define M_SQRT1_2		0.70710678118654752440
#endif
/* end   M_PI */

#ifndef MAXFLOAT
# define MAXFLOAT		((float)1.701411733192644299e+38)
#endif
/* end   MAXFLOAT */

#ifndef MINFLOAT
# define MINFLOAT		((float)-1.701411733192644299e+38)
#endif
/* end  MINFLOAT */

#ifndef  MAXINT
# define MAXINT			0x7fffffff
#endif
/* end   MAXINT */

#ifndef MININT
# define MININT			0x80000000
#endif
/* end   MININT */

__BEGIN_CDECLS
/*
 *	From alloc.c
 */
extern ptr_t 		Vmalloc __P((unsigned));
extern ptr_t		Vrealloc __P((ptr_t, unsigned));
extern ptr_t		Vcalloc __P((unsigned, unsigned));
extern void 		Vfree __P((ptr_t));
extern unsigned 	MemStat __P((void));
extern unsigned 	MemPtr __P((ptr_t));

/*
 *	From arrayn.c
 */
extern ptr_t		arrayn __P((int, ...));


#ifdef SYSV
extern long lrand48();
# define SETRANDOM(a) srand48(a)
# define RANDOM() lrand48()
#else
#if !(defined(linux) || defined(LinuxPPC))
extern long random();
#endif
# define SETRANDOM(a) srandom(a)
# define RANDOM() random()
#endif
#ifdef BSD
# define strchr(a, b) index(a, b)
# define strrchr(a, b) rindex(a, b)
#endif

__END_CDECLS

extern FILE *  VXtextf      __P((VisXfile_t *, char *));

#endif
/* end  _h_util */


/*
 * Varray.c: Dynamic memory allocator for an n - dimensional array.
 *
 */



/*
 *   Copyright 2014 Anthony P. Reeves
 * 
 *   COntributions by Christos Zoulas, Richard J. Prokop
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


#ifdef DEBUG
#include <stdio.h>
#endif /* DEBUG */

union alignment {
   char a;
   short b;
   int c;
   long d;
   float e;
   double f;
   union alignment *u;
};

#define ALIGN(type) ((sizeof(struct { char c; type t; })) - sizeof(type))

#define MAXALIGN ALIGN(union alignment)

#if __STDC__
  typedef void *genptr_t;
# include <stdarg.h>
# include <stdlib.h>
# else
  typedef char *genptr_t;
# include <varargs.h>
#endif /* __STDC__ */

extern genptr_t Vmalloc();
extern void Vfree();

typedef unsigned long	_u_long;


typedef struct _dimension_t {
    _u_long    	size;		/* Number of elements in the dimension	*/
    _u_long 	totalsize;	/* Total number of elements 		*/
    _u_long	elsize;		/* Size of a single element in bytes	*/
    _u_long    	offset;		/* offset from beginning of array	*/
} dimension_t;

/* Varray():
 *	Allocate an n-dimensional array. 
 *	We want to allocate only one chunk, so that free
 *	will free the whole array.
 */
#if __STDC__
genptr_t
Varray(int numdim, ...)
#else
/*VARARGS*/
genptr_t
Varray(va_alist)
va_dcl
#endif
{
    int 	i; /* , j; RJP */
    unsigned int j;
    dimension_t	*dim;		/* Info about each dimension		*/
    _u_long	total_size, 	/* Total size of the array		*/
		pointer_size, 	/* Pointer size of the array		*/
		element_size; 	/* Element size of the array		*/
    genptr_t	array;		/* the multi-dimensional array		*/
    genptr_t	*ptr, data;	/* Pointer and data offsets		*/
    va_list 	va;		/* Current pointer in the argument list	*/

#if __STDC__
    va_start(va, numdim);
#else
    int 	numdim;		/* Number of dimensions			*/
    va_start(va);
    numdim = va_arg(va, int);
#endif

    if ((dim = (dimension_t *) 
	        Vmalloc((unsigned) (numdim * sizeof(dimension_t)))) ==
	(dimension_t *) 0 ) 
	return((genptr_t) 0);

    for (i = 0; i < numdim; i++) {
	dim[i].size = va_arg(va, _u_long);
	dim[i].elsize = sizeof(genptr_t);
    }
    dim[numdim-1].elsize = va_arg(va, _u_long);

    va_end(va);
    
    /*
     *	Compute the size of the array to be allocated 
     *	elements : (x * y * z ... w * elementsize)
     *	pointers : (x * pointersize +
     *              y * x * pointersize +
     *              z * y * x * pointersize ..
     */
    dim[0].totalsize = dim[0].size;
    dim[0].offset    = 0;
    for (i = 1; i < numdim; i++) {
	dim[i].totalsize = dim[i-1].totalsize * dim[i].size;
	dim[i].offset = dim[i-1].offset + dim[i-1].totalsize * dim[i-1].elsize;
    }

    /*
     * Align the element array.
     */
#ifdef WIN32 /* noise about unnamed struct in MAXALIGN */
#pragma warning(disable : 4116)
#endif
    if ((dim[numdim - 1].offset % MAXALIGN) != 0) {
	dim[numdim - 1].offset += MAXALIGN;
	dim[numdim - 1].offset /= MAXALIGN;
	dim[numdim - 1].offset *= MAXALIGN;
    }

    element_size = dim[numdim-1].offset + 
			dim[numdim-1].totalsize * dim[numdim-1].elsize;
    pointer_size = dim[numdim-1].totalsize;

    total_size = element_size + pointer_size;

#ifdef DEBUG
    (void) fprintf(stderr, "%20s : %10d\n", "Number of dimensions", 
	numdim, numdim);
    (void) fprintf(stderr, "%20s : %10d (%.8x)\n", "Unit size", 
	dim[numdim-1].elsize, dim[numdim-1].elsize);
    (void) fprintf(stderr, "%20s : %10d (%.8x)\n", "Size of pointers", 
	pointer_size, pointer_size);
    (void) fprintf(stderr, "%20s : %10d (%.8x)\n", "Size of elements", 
	element_size, element_size);
    (void) fprintf(stderr, "%20s : %10d (%.8x)\n", "Total size", 
	total_size, total_size);
    (void) fprintf(stderr, "%20s : %10d\n", "Machine Alignment", MAXALIGN);
    (void) fprintf(stderr, "\nDimension\tSize\tTotal Size\tOffset\tElement\n");
    for ( i = 0; i < numdim; i++ ) 
	(void) fprintf(stderr, "%9d\t%4d\t%10d\t%6d\t%7d\n", i, dim[i].size,
		dim[i].totalsize, dim[i].offset, dim[i].elsize);
#endif /* DEBUG */

    /*
     * Allocate space to hold the array
     */
    if ((array = (genptr_t) Vmalloc((unsigned) total_size)) == 
	(genptr_t) 0) {
	(void) Vfree((genptr_t) dim);
	return((genptr_t) 0);
    }

    /*
     * Thread the pointers
     */
    for (i = 0; i < numdim - 1; i++) {
#ifndef lint
	/* we know this is aligned, but how do we convince lint? */
	ptr  = (genptr_t *) (((char *) array) + dim[i].offset);
#else
	ptr = (genptr_t *) 0;
#endif
	data = (genptr_t) (((char *) array) + dim[i+1].offset);
	for ( j = 0; j < dim[i].totalsize; j++ ) 
	    ptr[j] = ((char *) data) + j * dim[i+1].elsize * dim[i+1].size;
    }

    (void) Vfree((genptr_t) dim);
    return(array);

} /* end arrayn */

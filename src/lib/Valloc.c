/*
 *
 * alloc.c : Memory checked Vmalloc special for lint
 *	     until lint becomes intelligent. This malloc
 *	     keeps track of memory usage.
 *
 * Routines:
 *	char * 		Vmalloc();
 *	char *		Vcalloc();
 *	char *		Vrealloc();
 *	void 		Vfree();
 *	unsigned	MemStat();
 *	unsigned	MemPtr();
 *	void		MemChain();
 *
 */


/*
 *   Copyright 2014 Anthony P. Reeves
 * 
 *   Contributions by Christos Zoulas
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


#include <stdio.h>
#ifdef __STDC__
# include <stdlib.h>
# include <memory.h>
#else
extern char *malloc();
extern char *calloc();
extern char *realloc();
extern void free();
extern void abort();
extern char *memset();
#endif

#ifndef NIL
# define NIL(a) ((a *) 0)
#endif /* NIL */

#ifndef MIN
# define MIN(a, b) 		((a) > (b) ? (b) : (a))
#endif /* MIN */

#ifndef MAX
# define MAX(a, b) 		((a) < (b) ? (b) : (a))
#endif /* MAX */

#ifndef private
# define private static
#endif /* private */

#ifndef public
# define public 
#endif /* public */


#define SIG_GOOD	0x01020304
#define SIG_FREE	0x04030201
#define OVERHEAD	(sizeof(long) + sizeof(unsigned))

#ifdef __STDC__
typedef void *Ptr;
#else
typedef char *Ptr;
#endif

private unsigned _memused = 0;
private unsigned _memalloc = 0;
private Ptr	 _memtop = (Ptr) 0;
private Ptr	 _membot = (Ptr) 0xffffffff;

/* _chaina():
 *	Check things for validity and allocate space
 */
private Ptr
_chaina(n, routine, action, tptr)
unsigned n;
Ptr (*routine)();
char *action;
Ptr tptr;
{
    char *ptr;

    if (tptr != (Ptr) 0 && (tptr < _membot || tptr > _memtop ||
	((int) tptr) & 0x3)) {
	(void) fprintf(stderr, "*** %s invalid pointer.\n", action);
	abort();
    }
    if (n == 0) {
	(void) fprintf(stderr, "*** %s zero length block.\n", 
		       action);
	if (tptr != (Ptr) 0) {
	    ptr = tptr;
	    *((long *) ptr) = SIG_GOOD;
	    _memused += *((unsigned *) &ptr[sizeof(long)]);
	    _memalloc++;
	}
	abort();
    }

    ptr = (tptr == (Ptr) 0) ? (char *) (*routine)(n+OVERHEAD) :
	                     (char *) (*routine)(tptr, n+OVERHEAD);
    
    _memtop = MAX(_memtop, (Ptr) ptr);
    _membot = MIN(_membot, (Ptr) ptr);
    if (ptr == NIL(char)) {
	if (tptr != (Ptr) 0)
	    *((long *) tptr) = SIG_GOOD;
	(void) fprintf(stderr, 
		       "*** Out of memory in %s (current %d, total %d).\n", 
		       action, n, _memused);
	
	abort();
    }
    *((long *) ptr) = SIG_GOOD;
    _memused += (*((unsigned *) &ptr[sizeof(long)]) = n);
    _memalloc++;
    ptr += OVERHEAD;
    return((Ptr) ptr);
} /* end _chaina */


/* _chainc():
 *	Check the pointer given
 */
private unsigned
_chainc(ptr, action)
char **ptr;
char *action;
{
    static char *msg = "*** %s %s pointer.\n";

    if (*ptr == NIL(char)) {
	(void) fprintf(stderr, msg, action, "nil");
	abort();
    }
    *ptr -= OVERHEAD;
    if ((Ptr) *ptr < _membot || (Ptr) *ptr > _memtop || ((int) *ptr) & 0x3) {
	(void) fprintf(stderr, "*** %s invalid pointer.\n", action);
	abort();
    }
    switch (*((long *) *ptr)) {
    case SIG_GOOD:
	return(*((unsigned *) &((*ptr)[sizeof(long)])));
    case SIG_FREE:
	(void) fprintf(stderr, msg, action, "free");
	abort();
    default:
	(void) fprintf(stderr, msg, action, "invalid");
	abort();
    }
    return(0);
} /* end _chainc */


/* Vmalloc():
 *	real alloc 
 */
public Ptr
Vmalloc(n)
unsigned n;
{
    static char *routine = "malloc";
    return(_chaina(n, malloc, routine, (Ptr) 0));
} /* end Vmalloc */


/* Vcalloc():
 *	real alloc 
 */
public Ptr
Vcalloc(n, sz)
unsigned n, sz;
{
    Ptr ptr;
    static char *routine = "calloc";

    n *= sz;
    ptr = _chaina(n, malloc, routine, (Ptr) 0);
    memset((char *) ptr, 0, n);
    return(ptr);
} /* end Vcalloc */


/* Vrealloc():
 *	real alloc 
 */
public Ptr
Vrealloc(ptr, n)
Ptr ptr;
unsigned n;
{
    static char *routine = "realloc";

    _memused -= _chainc((char **) &ptr, routine);
    _memalloc--;
    *((long *) ptr) = SIG_FREE;
    return(_chaina(n, realloc, routine, ptr));
} /* end Vrealloc */


/* Vfree():
 *	free memory counting the number of bytes freed
 */
public void
Vfree(ptr)
Ptr ptr;
{
    static char *routine = "free";

    _memused -= _chainc((char **) &ptr, routine);
    _memalloc--;
    *((long *) ptr) = SIG_FREE;
    free(ptr);
} /* end Vfree */


/* MemChain():
 *	Dump the chain
 */
public void
MemChain()
{
    if (_memused == 0 && _memalloc == 0) 
	(void) fprintf(stdout, "\tNo memory allocated.\n");
    else {
	(void) fprintf(stdout, "\t%u Bytes allocated in %u chunks.\n", _memused,
		       _memalloc);
	(void) fprintf(stdout, "\tAverage chunk length %u bytes.\n", 
		       _memused / _memalloc);
    }
} /* end MemChain */


/* MemStat():
 *	return the amount of memory in use
 */
public unsigned
MemStat()
{
    return(_memused);
} /* end MemStat */


/* MemPtr():
 *	return the amount of memory used by the pointer
 */
public unsigned
MemPtr(ptr)
Ptr ptr;
{
    static char *routine = "get size";
    return(_chainc((char **) &ptr, routine));
} /* end MemPtr */


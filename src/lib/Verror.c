/*
 * Verror.c : VisX error handling routines
 *
 */


/*
 *   Copyright 2014 Anthony P. Reeves
 *
 *   contribution by Christos Zoulas
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
#ifndef NOVPRINTF
#ifndef __STDC__
#include <varargs.h>
#else
#include <stdarg.h>
#endif
#endif /* NOVPRINTF */
#include <errno.h>
#include "VisX.h"
#include "Verror.h"
#include "Vborder.h"

char *visx_pname;
VisXhdr_t VisX3hdr;

int Verrno;
int visx_sys_nerr = 9;
char *visx_sys_errlist[] =  {
    "Error 0",
    "Not a VisX File",
    "Incomplete VisX Header", 
    "Invalid machine type", 
    "Bad Header Modifier",
    "Incomplete VisX Header", 
    "Unsupported Format",
    "Unmatched parameters",
    "Missing parameters",
    "Bad parameter",
    0
};

static int Verrorflag = 0;

/* Verror():
 *	Handle an error depending on the value of errorflag
 */
/*VARARGS*/
void
#ifndef NOVPRINTF
# ifdef __STDC__
Verror(char *fmt, ...)
# else
Verror(va_alist)
va_dcl
# endif
#else
Verror(fmt, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)
char *fmt, *a0, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11, *a12;
#endif /* NOVPRINTF */
{
#ifndef NOVPRINTF
    va_list ap;

# ifndef __STDC__
    char *fmt;
    va_start (ap);
    fmt = va_arg(ap, char *);
# else
    va_start(ap, fmt);
# endif
#endif /* NOVPRINTF */
    if ( !(Verrorflag & PE_ERR_SILENT) )
#ifdef NOVPRINTF
	(void) fprintf(stderr, fmt, a0, a1, a2, a3, a4, a5, a6, 
	    a7, a8, a9, a10, a11, a12);
#else
	(void) vfprintf(stderr, fmt, ap);
    va_end (ap);
#endif /* NOVPRINTF */
    if ( !(Verrorflag & PE_ERR_NOEXIT) )
	exit(Verrno);
} /* end Verror */

/* Verrorstr():
 *	Return an ascii string explaining why an error occured.
 */
char *
Verrorstr()
{
    if ( Verrno < 0 ) 
	return(visx_sys_errlist[-Verrno]);
    else
#ifdef linux
	return(strerror(Verrno));
#else
	return(sys_errlist[Verrno]);
#endif
} /* end Verrorstr */

/* Vseterror():
 *	Set the error action of a visx system routine
 */
int
Vseterror(flag)
int flag;
{
    int tmp = Verrorflag;
    Verrorflag = flag;
    return(tmp);
} /* end Vseterror */

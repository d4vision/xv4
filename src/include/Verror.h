/* 
 * Verror.h: VisX Error related definitions
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

#ifndef _H_VisXERROR
#define _H_VisXERROR

#include "cdefs.h"

#ifdef __STDC__
# include <errno.h>
# include <string.h>
# define PERROR strerror(errno)
#else
__BEGIN_CDECLS
/* 
 *	unix error stuff
 */
extern int	errno;
extern int	sys_nerr;
extern char *	sys_errlist[];
# define PERROR (errno < sys_nerr && errno >= 0 ? sys_errlist[errno] : \
	"Unknown Error")
#endif

/*
 *	VisX error stuff
 */
extern int 	Verrno;
extern int 	visx_sys_nerr;
extern char *	visx_sys_errlist[];

__END_CDECLS


#define PE_ERR_DEFAULT	 0
#define PE_ERR_NOEXIT	 1
#define PE_ERR_SILENT	 2

#define PE_NO_VisX 	-1
#define PE_INC_VisX 	-2
#define PE_BAD_MACH 	-3
#define PE_BAD_MOD 	-4
#define PE_INC_HDR 	-5 
#define PE_UNS_FMT 	-6
#define PE_UNM_PAR	-7
#define PE_MIS_PAR	-8
#define PE_BAD_PAR	-9

#endif
/* end  _H_VisXERROR */

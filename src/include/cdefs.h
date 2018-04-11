/*
 * cdefs.h: Defines for c, ansi and cplusplus
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


#ifndef _h_cdefs
#define _h_cdefs

#ifdef __cplusplus
# define __BEGIN_CDECLS	extern "C" {
# define __END_CDECLS	}
#else
# define __BEGIN_CDECLS
# define __END_CDECLS
#endif

#if __STDC__
#include <stdlib.h>
#include <unistd.h>
# endif

#if __STDC__
# ifndef __P 
#  define __P(a) a
# endif
typedef void * ptr_t;
#else
# ifndef __P 
#  define __P(a) ()
# endif
typedef char * ptr_t;
#endif

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#if __STDC__
#define sys_errlist _sys_errlist
#define sys_nerr    _sys_nerr
#define access      _access
#define chmod       _chmod
#define chsize      _chsize
#define close       _close
#define creat       _creat
#define dup         _dup
#define dup2        _dup2
#define eof         _eof
#define fdopen      _fdopen
#define filelength  _filelength
#define isatty      _isatty
#define locking     _locking
#define lseek       _lseek
#define mktemp      _mktemp
#define open        _open
#define read        _read
#define setmode     _setmode
#define sopen       _sopen
#define tell        _tell
#define umask       _umask
#define unlink      _unlink
#define write       _write

#define swab        _swab
#define O_RDWR      _O_RDWR
#define O_RDONLY    _O_RDONLY
#define O_WRONLY    _O_WRONLY
#define O_BINARY    _O_BINARY
#define O_TEXT      _O_TEXT

#include <process.h>

#define execlp      _execlp

#endif
extern int getuid();
extern int getpid();
extern int fork();
extern int wait();
extern int pipe();
#endif

#endif
/* end  _h_cdefs */

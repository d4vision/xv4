/*
 * Vfname.c: Filename expansion for visix
 *
 */



/*
 *   Copyright 2014 Anthony P. Reeves
 *
 *   contribtutions from Christos Zoulas, Richard J. Prokop, Anthony B. Chan
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

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
#include <sys/param.h>
#include <pwd.h>
#endif


#include "VisXV4.h"
#include "Vutil.h"
#include "VisX.h"
#include "Verror.h"

#ifndef VISXHOME 
# define VISXHOME "/home/visx"
#endif /* VISXHOME */
extern char *visx_pname;

static char pathbuf[BUFSIZ];

char *
Vtilde(fname)
char *fname;
{
#ifdef WIN32
    return fname;
#else
    struct passwd *pwd;
    char *ptr;

    if (fname[0] != '~')
	return(fname);
    if ((ptr = (char *) strchr(fname, '/')) != NULL)
	*ptr++ = '\0';
    else
	ptr = fname;
    if ((pwd = getpwnam(fname)) == NULL)
	return(NULL);
    (void) sprintf(pathbuf, "%s/%s", pwd->pw_dir, ptr);
    return(pathbuf);
#endif
}

char *
Vfname(fname)
char *fname;
{
#ifdef WIN32
    (void) sprintf(pathbuf, "%s", fname);
#else
    struct passwd *pwd;

    if (fname[0] == '/') 
	return fname;
    if (fname[0] == '~')
	return Vtilde(fname);
    /*
     * First try ~visx
     */
    if ((pwd = getpwnam("visx")) != NULL) {
	(void) sprintf(pathbuf, "%s/%s", pwd->pw_dir, fname);
    } else {
	(void) sprintf(pathbuf, "%s/%s", VISXHOME, fname);
    }
#endif
    return(pathbuf);
}

#ifdef WIN32
#ifdef DOSPATHDELIMITER
#ifndef VISXMAN
#define VISXMAN "c:\\VisionX\\v4\\man"
#endif
#ifndef VISXINC
#define VISXINC "c:\\VisionX\\v4\\include"
#endif
#ifndef VISXLIB
#define VISXLIB "c:\\VisionX\\v4\\lib"
#endif
#ifndef VISXBIN
#define VISXBIN "c:\\VisionX\\v4\\bin"
#endif
#ifndef VISXETC
#define VISXETC "c:\\VisionX\\v4\\etc"
#endif
#else
#ifndef VISXMAN
#define VISXMAN "c:/VisionX/v4/man"
#endif
#ifndef VISXINC
#define VISXINC "c:/VisionX/v4/include"
#endif
#ifndef VISXLIB
#define VISXLIB "c:/VisionX/v4/lib"
#endif
#ifndef VISXBIN
#define VISXBIN "c:/VisionX/v4/bin"
#endif
#ifndef VISXETC
#define VISXETC "c:/VisionX/v4/etc"
#endif
#endif


#define stat _stat
#define access _access
#endif

/* Vgetdir():
 *	Return the standard visx directory.
 *	1. Look in the environment
 *	2. Tilde expand the standard path
 *	3. Return the compiled in constant
 */
char * 
Vgetdir(comp)
char *comp;
{
    char *ptr, *ptr1;
    extern char *getenv();
    struct stat st;
    static struct {
	char *name;
	int attr;
	char *env;
	char *dir;
    } *lp, lst[]= {
	{ "man",     0, "VISIONXMAN", VISXMAN },
	{ "include", 0, "VISIONXINC", VISXINC },
	{ "lib",     1, "VISIONXLIB", VISXLIB },
	{ "bin",     1, "VISIONXBIN", VISXBIN },
	{ "etc",     1, "VISIONXETC", VISXETC },
	{ NIL(char), 0, NIL(char), NIL(char)  },
    };
    
    for (lp = lst; lp->name != NIL(char); lp++)
	if (strncmp(lp->name, comp, 3) == 0)
	    break;

    if (lp->name == NIL(char)) {
	Verrno = PE_BAD_PAR;
	Verror("%s: Cannot locate `%s' (%s).\n", visx_pname, comp, Verrorstr());
	return(NIL(char));
    }

#if (defined(WIN32) && !(defined(WINMinGW)))
#define isdir(a) (stat(a, &st) != -1 && (st.st_mode & _S_IFDIR) == _S_IFDIR && !access(a, 2))
#else
#define isdir(a) (stat(a, &st) != -1 && (st.st_mode & 040000) == 040000 && !access(a, 1))
#endif

    if ((ptr = getenv(lp->env)) != NIL(char) && isdir(ptr))
	return(ptr);

    if (isdir(lp->dir)) 
	return(lp->dir);
    
    if ((ptr1 = getenv("VISIONXDIR"))) {
        sprintf(pathbuf, "%s/%s", ptr1, lp->name); 
        if (isdir(pathbuf)) 
            return pathbuf;
    }

    ptr = Vfname(lp->name);
    if (lp->attr && (ptr1 = getenv("HOSTTYPE"))) {
	(void) strcat(ptr, "/");
	(void) strcat(ptr, ptr1);
    }
    return(ptr);
}

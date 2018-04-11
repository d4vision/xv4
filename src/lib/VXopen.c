/*
 *  V X o p e n   -  open visx format file for read or write
 *
 *   contributions by:
 *
 *      Mike Zuhl   1977
 *      Doug Kimber 1980 
 *      Christos Zoulas 1989
 *      R. J. Prokop 1992
 *      W. J. Kostis 1998
 *      A. B. Chan 2003
 *
 *  This routine is internal to the system VisionX utility programs
 *  and handles the opening of files that may or may not be in
 *  VisionX format. The call is:
 *
 *      fd = VXopen(name, header, access)
 *
 *  Where:
 *      fd  	the file descriptor for this file if >= 0.
 *      	the error code if < 0.
 *
 *      name    a pointer to the string containing the (optional)
 *      	file name and (optional) modifiers
 *
 *      header  pointer to the VisionX header structure
 *
 *      access  read = 0; write = 1; modify-existing file = 2;
 *      	read and seek = 3; write and seek = 4;
 *      	modify-new file = 5;
 *
 *
 *
 *  If the file name is null, then standard input or output is assumed,
 *  depending on the access code. If there are no modifiers the file
 *  is assumed to be in VisionX format and (for the read case) the header is
 *  read in. In the write case the element "modified" in the header
 *  structure is set to one (zero otherwise).
 *
 *  If present, modifiers replace or alter the header. In any case, a header
 *  with all the information is generated. Modifiers are of the form:
 *      ,<modifier>
 *  i.e., separated from themselves and the file name by a comma. This
 *  means that a file name may not contain a comma. The modifiers may
 *  be any of the following, and in the case of conflicts the most
 *  recently used take precidence.
 *
 *      raw		(write access files only) causes new file
 *      		NOT to have a header.
 *
 *      nnn		where "nnn" is a number. This sets both the
 *      		X and Y sizes to be "nnn".
 *
 *      t="title" 	sets title as indicated. Defaults to command
 *          		line. Note that title may not contain a comma and
 *      		any funny characters must be escaped.
 *      		The title is ended by a comma or a zero char.
 *
 *	m="machine"  	sets machine as indeicated. Defaults to the local
 *			machine.
 *		
 *      The visionx header structure element "modified" is set to 1 if a file
 *      to be read has a physical header block and 0 otherwise. If a file
 *      is to be written then "modified" set to 1 indicates that a
 *      header block needs to be written for it.
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

#include    <sys/types.h>
#include    <stdio.h>
#include    <time.h>
#include    <errno.h>
#include    <fcntl.h>
#ifndef WIN32
#include    <sys/file.h>
#endif
#include    "VisXV4.h"
#include    "Vutil.h"
#include    "VisX.h"     /* visx header format */
#include    "Verror.h"
#include    "Vborder.h"
#include    "VXpnm.h"   /* PNM, TIFF File i/o */


/*extern char *strncpy(), *strcpy(), *strcat(); RJP */
/*extern int strlen(); RJP */
#ifndef linux
extern char *mktemp();
#endif
/*extern long time();*/
#define VisXSIZE sizeof(VisXhdr_t)

extern char **g_argv;
extern int g_argc;

/* global data information */
static char _visx_cmdl[P_TSIZE];
extern VisXhdr_t VisX3hdr; /* saved on first read (save header)    */
static VisXhdr_t base_hdr; /* used for current file */
static char _visx_buf[BUFSIZ];
static int  first_read = TRUE;
static float bbx[4]; /* box for v-3 image file */
/* char *VisXhist = NIL(char); */    /* generic history */
extern char *VisXhist;     /* generic history */

static char *_visx_pidname  = "/tmp/VopenXXXXXX";
extern char *visx_pname;
static int rdhead(), fix_machine();

#define READ_ACCESS(a) (((a) == READ) || ((a) == RANDSK) || ((a) == MODIFY))


VisXfile_t *
VXopen(name, access)
char *name;
int access; /* 0=read, 1=write, 2=modify 3=read seek, 4=write seek */
{
    register char *p;
    char *mod;                 /* pntr to modifiers */
    register int i;
    int fd  = -1;
    int deflag; 		/* default flag */
    char *comma;    		/* used to restore zeroed comma */
    char *Gname = (char *) 0;	/* the file name */
    int nread;
    VisXhdr_t *qhdr;
    VisXfile_t *file;           /* return file structure */
    VisXelem_t *lptr;

    extern void addtitle(); /* RJP */
#ifdef WIN32
    _fmode = _O_BINARY;
#endif

    Verrno = 0;
    comma = 0;
    qhdr = &base_hdr;
    qhdr->title[0] = '\0';
    qhdr->zero = 0;

    file = NEW(VisXfile_t);
    file->access = access;
    file->mode = 0;
    file->list = VXinit();
    file->last = VXNIL;
    file->first = 1;
    file->bswap = 0;

    /*
     *  get title from argument list, see if g_argc, g_argv
     *  is set by Vparse
     */
    if ( g_argv != (char **) 0 ) {  
	visx_pname = *g_argv;

	p = _visx_cmdl;
	for (i = 0; i < g_argc; i++) {
	    for (mod = g_argv[i]; *mod; *p++ = *mod++) 
		if (p >= &_visx_cmdl[P_TSIZE - 1]) {
		    *--p = '\0';
		    break;
		}
	    if (p >= &_visx_cmdl[P_TSIZE - 1]) 
		break;
	    else
		*p++ = ' ';
	}
	*p = '\0';

	while(*p == ' ' && p > _visx_cmdl)
	    p--;

	*p++ = '\n';

	while (p < &_visx_cmdl[P_TSIZE]) 
	    *p++ = '\0';
    }
    else  {
	(void) strcpy(_visx_cmdl, "VISX Standard Title");
	visx_pname = "Vopen";

    }

    qhdr->modified = 1;
    mod = 0;
    deflag = ((access == READ)||(access == RANDSK)||(access == MODIFY));
    if(!deflag) file->mode = 1; /* quick fix XXXX */

    if(name != 0) {
	for(p = name; (*p != '\0') && (*p != ','); p++);
	if(*p == ',') {
	    comma = p;
	    *p = '\0';
	    mod = p + 1;
	}
    }


    /* set some default values if DISPLAY or READ or */
    if (deflag) {
	qhdr->hdr = VisXMAGIC;
	(void) strncpy(qhdr->machine, LOCAL_MACHINE_NAME, P_MSIZE);
	qhdr->fmt = VFMT_UBYTE;
	qhdr->ysize = qhdr->xsize = 512;
	qhdr->zsize = 8;
	qhdr->nchan = 1;
    }


    /* open the file, if necessary */

    if ((name != 0) && (*name != '\0')) {
	if ((access > 2) && (access != 5)) {
	    access -= 3;
	}

	if(access == WRITE) {
	  Gname = name;
	  if ((fd = creat(name, 0644)) < 0) {
	    Verrno = errno;
	    Verror("%s: Cannot create `%s' (%s).\n",
		   visx_pname, name, Verrorstr());
	    goto failed;
	  }
	}
	else if (access == NMODIFY) {
	  Gname = name;
	  if ((fd = creat(name, 0644)) < 0) {
	    Verrno = errno;
	    Verror("%s: Cannot create `%s' (%s).\n",
		   visx_pname, name, Verrorstr());
	    goto failed;
	  }
	  (void) close(fd);
	  if ((fd = open(name, O_RDWR)) < 0) {
	    Verrno = errno;
	    Verror("%s: Cannot open `%s' (%s).\n",
		   visx_pname, name, Verrorstr());
	    goto failed;
	  }
	  access = WRITE;
	} 
	else {
	  Gname = name;
	  if ((fd = open(name, access)) < 0) {
	    Verrno = errno;
	    Verror("%s: Cannot open `%s' (%s).\n",
		   visx_pname, name, Verrorstr());
	    goto failed;
	  }
	}
    } 
    else {
	switch(access) {
	case READ:  
	    Gname = "stdin";
	    fd = 0;
	    break;
	case WRITE: 
	    Gname = "stdout";
	    fd = 1;
	    break;
	case MODIFY:    
	    Verrno = 0;
	    Verror("%s: Cannot modify `stdin'. (%s)\n",
		visx_pname, Verrorstr());
	    goto failed;
	case RANDSK:    
	    Gname = "stdin";
	    if( lseek(0, 0L, 1) < 0L ) {
#ifndef linux
		_visx_pidname = mktemp(_visx_pidname);
		if ((fd = creat(_visx_pidname, 0644)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot create `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
#else
		if ((fd = mkstemp(_visx_pidname)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot create `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
#endif

		while ((nread = read(0, _visx_buf, BUFSIZ)) >= 0 || 
			errno == EINTR) {
		    if (nread == 0) 
			break;
		    if(Vwrite(fd, _visx_buf, nread) != nread) {
			Verrno = errno;
			Verror("%s: write failed (%s).\n", 
			    visx_pname, Verrorstr());
			goto failed;
		    }
		}
		if ( nread != 0 ) {
		    Verrno = errno;
		    Verror("%s: read failed (%s).\n", 
			visx_pname, Verrorstr());
		    goto failed;
		}
		(void) close(fd);
		if ((fd = open(_visx_pidname, O_RDWR)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot open `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
		if ( unlink(_visx_pidname) < 0 ) { 
		    Verrno = errno;
		    Verror("%s: Cannot unlink `%s' (%s).\n",
			visx_pname, name, Verrorstr()); 
		    goto failed; 
		}
	    }
	    else {
		Gname = "stdin";
		fd = 0;
	    }
	    break;

	case WANDSK:
	case NMODIFY:   
	    Gname = "stdin";
	    access = WRITE;
	    if( lseek(1, 0L, 1) < 0L ) {
#ifndef linux
/* this code does not do anything useful?? apr 9/20/03 */
		_visx_pidname = mktemp(_visx_pidname);
		if ((fd = creat(_visx_pidname, 0644)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot create `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
#else 
		if ((fd = mkstemp(_visx_pidname)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot create `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
#endif
		(void) close(fd);
		if ((fd = open(_visx_pidname, O_RDWR)) < 0) {
		    Verrno = errno;
		    Verror("%s: Cannot open `%s' (%s).\n",
			visx_pname, name, Verrorstr());
		    goto failed;
		}
		if ( unlink(_visx_pidname) < 0 ) { 
		    Verrno = errno;
		    Verror("%s: Cannot unlink `%s' (%s).\n",
			visx_pname, name, Verrorstr()); 
		    goto failed; 
		}
	    }
	    else 
        {
            fd = 1;
        }
	    break;
	}
    }

    /* check for processing modifiers */

    if(mod != 0) {
	if(access != WRITE) qhdr->modified = 0;
	while(*mod) {

	    /* If it is a number, set sizes to that value */
	    if((*mod >= '0') && (*mod <= '9')) 
		qhdr->xsize = qhdr->ysize = atoi(mod);
	    else switch(*mod) {

	    /* Else search for meaningful modifier */
	    case 'r':
		if(access == WRITE)
		    qhdr->modified = 0;
		break;

	    case 'f':   /* set format */
		qhdr->fmt = atoi(mod += 2);
		break;

	    case 't':   /* set title */
		mod += 2;
		i = 0;
		p = _visx_cmdl;
		while((i < P_TSIZE - 2) && (*mod != ',') && *mod) {
		    *p++ = *mod++;
		    i++;
		}
		*p++ = '\n';
		*p++ = '\0';
		break;
	    
	    case 'm': /* set machine */
		mod += 2;
		i = 0;
		p = qhdr->machine;
		while((i < P_MSIZE - 2) && (*mod != ',') && *mod){
		    *p++ = *mod++;
		    i++;
		}
		*p++ = '\0';
		break;
	    case 'o':  /* v3 format for write */
		if(access == WRITE)
		    file->mode = 3;
		break;
	    case 'p':  /* pnm format for write */
		if(access == WRITE)
		    file->mode = 4;
		break;
	    case 'a':  /* pnm ascii format for write */
		if(access == WRITE)
		    file->mode = 5;
		break;
	    case 'i':  /* tiff format for write */
		if(access == WRITE)
		    file->mode = 6;
		break;

	    default:    /* handle unrecognized modifiers */
		Verrno = PE_BAD_MOD;
		Verror("%s: Cannot open `%s' (%s).\n",
		    visx_pname, Gname, Verrorstr());
		goto bad_mod;
	    }

	    /* skip to next option */

	    while((*mod != ',') && (*mod != '\0')) mod++;
	    if(*mod == ',') mod++;
	}

    } 
    else {

	/* 
	 * if there are no modifiers and access type is
	 * MODIFY or READ, read the header 
	 */
	if((access != WRITE) && qhdr->modified) {
#ifdef WIN32
        setmode(fd, _O_BINARY);
#endif
	    if( rdhead(&fd, qhdr, file)) {
		if (qhdr->hdr != VisXMAGIC) {
		    Verrno = PE_NO_VisX;
		    Verror("%s: Cannot open `%s' (%s).\n", 
			visx_pname, Gname, Verrorstr());
		    goto hdr_err;
		}
		else {
		    Verrno = PE_INC_HDR;
		    Verror("%s: Cannot open `%s' (%s).\n", 
			visx_pname, Gname, Verrorstr());
		    goto hdr_err;
		}
	    }
	    if (!fix_machine(qhdr, Gname))
		goto badmachine;
		
	}
	qhdr->modified = 1;
    }

/* File is opened correctly so set up header and return */
    
    if (!deflag) {
       /* create list information for new file  */
       lptr = VXaddelem(file->list, VX_FNAME, Gname, 1 + strlen(Gname));
       lptr = VXaddelem(lptr, VX_FCMND, _visx_cmdl, 1 + strlen(_visx_cmdl));
       lptr = VXaddelem(lptr, VX_FMACH, LOCAL_MACHINE_NAME,
			     1 + strlen(LOCAL_MACHINE_NAME));
       lptr = VXaddelem(lptr, VX_FUID, NIL(char), getuid());
       lptr = VXaddelem(lptr, VX_FTIME, NIL(char), (long)time(NULL));
       if (VisXhist != NIL (char))
           lptr = VXaddelem(lptr, VX_FHIST, VisXhist, strlen(VisXhist) + 1);
   
	/* 
	 * fill zero values with defaults 
	 */
	if (machine_type(qhdr->machine) == UNKNOWN_TYPE) 
	    (void) strncpy(qhdr->machine, LOCAL_MACHINE_NAME, P_MSIZE);

	if (!qhdr->fmt)
	    qhdr->fmt = VFMT_UBYTE;
	if (!qhdr->xsize)
	    qhdr->xsize = 512;
	if (!qhdr->ysize)
	    qhdr->ysize = 512;
	if (!qhdr->zsize)
	    qhdr->zsize = 8;
	if (!qhdr->nchan)
	    qhdr->nchan = 1;
    }

    /* set creation date and creator */
    if(access == WRITE || access == WANDSK) {
	/*
	 * for now we inherit only the extra bytes and the title
	 */
	memcpy(qhdr->extra, VisX3hdr.extra, P_ESIZE);
	memcpy(qhdr->title, VisX3hdr.title, P_TSIZE);
	qhdr->crdate = (long)time(NULL);
	qhdr->uid = (short) getuid();
#ifdef DEBUG
      (void) fprintf(stderr, "addtitle >%s< >%s<\n", qhdr->title, _visx_cmdl);
#endif
	addtitle(qhdr, _visx_cmdl);
    }

    if ( first_read ) {
	/* save the header */
	VisX3hdr = *qhdr;
        if (file->mode ==  2){ /* VisX 3 file */
	     VisXhist = Vmalloc(strlen(VisX3hdr.title) + 1 );
	     strcpy(VisXhist, VisX3hdr.title);
	     /* VisXhist = VisX3hdr.title; */
	}
#ifdef DEBUG
	(void) fprintf(stderr, "savetitle >%s< \n", VisX3hdr.title);
	(void) fprintf(stderr, "original >%s< \n", qhdr->title);
#endif
	if ( READ_ACCESS(access) )
	    first_read = FALSE;
    }

    /* good return */
    if(comma)  *comma = ',';
    qhdr->hdr = VisXMAGIC;
    if (qhdr->fmt < VFMT_SET)
	/*
	 * Broken visx files had this non zero
	 */
	qhdr->uchan = 0;

#ifdef WIN32
    setmode(fd, _O_BINARY);
#endif

    file->fd = fd;

    /* compatability mode ****     */
    if (file->mode ==  2){ /* VisX 3 file */
       file->mode += (qhdr->fmt <<3); /* add format to mode */
       lptr = VXaddelem(file->list, VX_FNAME, Gname, 1 + strlen(Gname));
       lptr = VXaddelem(lptr, VX_FCMND, _visx_cmdl, 1 + strlen(_visx_cmdl));
       lptr = VXaddelem(lptr, VX_FMACH, LOCAL_MACHINE_NAME,
			     1 + strlen(LOCAL_MACHINE_NAME));
       lptr = VXaddelem(lptr, VX_FUID, NIL(char), qhdr->uid);
       lptr = VXaddelem(lptr, VX_FTIME, NIL(char), qhdr->crdate);
       lptr = VXaddelem(lptr, VX_FHIST, qhdr->title, strlen(qhdr->title) + 1);
       if (qhdr->fmt < 8){ /* regular image file so get size params */
	   bbx[0] = 0;
	   bbx[1] = (float) qhdr->xsize;
	   bbx[2] = 0;
	   bbx[3] = (float) qhdr->ysize;
           lptr = VXaddelem(lptr, VX_BBX, (char *) bbx, sizeof(bbx));
	   if(qhdr->nchan != 1)
               lptr = VXaddelem(lptr, VX_PCHAN, NIL(char), qhdr->nchan);

       }

    }

    return(file); /* the good open return */

    /*
     *  handle header format errors
     */
badmachine :
    /* Unknown machine */
    if(comma)  *comma = ',';
    file->fd = -fd - 100;
    return(file);
    
bad_mod :
    /* Bad Command line modifier */
    if(comma)  *comma = ',';
    qhdr->hdr = 0;
    file->fd = -1;
    return(file);

hdr_err :
    /* Header level errors */
    if(comma)  *comma = ',';
    qhdr->hdr = 0;
    close(fd);
    file->fd = -1;
    return(file);
   
failed :
    /* System level errors */
    if(comma)  *comma = ',';
    qhdr->hdr = 0;
    file->fd = -1;
    return(file);
} /* end Vopen */

extern int Vcomp(); /* RJP */

static int
rdhead(fdp, qhdr, file)
VisXhdr_t *qhdr;
int *fdp;
VisXfile_t *file;
{
    char *p, *q, *r;
    int i, ea;
    int okflag;

    okflag = 0;
    if ( read(*fdp, (char *) qhdr, 4) != 4) 
	return(1);

     r = q = (char*) &qhdr->hdr;
     if (*q++ == VX_MAGIC[0] && *q++ == VX_MAGIC[1] &&
            *q++ == VX_MAGIC[2] && *q++ == VX_MAGIC[3] ){
	    okflag = 1;
      } else if (*r++ == VX_MAGICX[0] && *r++ == VX_MAGICX[1] &&
            *r++ == VX_MAGICX[2] && *r++ == VX_MAGICX[3] ){
	    file->bswap = 1;
	    okflag = 1;
      } else {   /* format conversion */

              i = Vcomp(qhdr, 4, fdp);
              if( i == 2){ /* compression... lets try for conversion also */
                  i = Vcomp(qhdr, 4, fdp);
              }
      }

#ifdef P_DEBUG
     fprintf(stderr, "fd is %d\n",*fdp);
     fprintf(stderr, "i is %d\n",i);
     fprintf(stderr, "fd is %d\n",*fdp);
     fprintf(stderr, "n is %d\n",i);
#endif

     /* check for a VisX 4 key */
     /* right now this is 6 chars long */
     q = (char*) &qhdr->hdr;
     r = q + 4;
     if (okflag || ( *q++ == VX_MAGIC[0] && *q++ == VX_MAGIC[1] &&
		     *q++ == VX_MAGIC[2] && *q++ == VX_MAGIC[3] ) ) {
	 if ( read(*fdp, (char *) r, 2) != 2) /* get next 2 */ 
	     return(1);
	 if (*r++ == VX_MAGIC[4] && *r++ == VX_MAGIC[5]) {
	     return(0);
	 } else {
	     return 1;
	 }
     }

     /* The rest is for a VisX3 format file */
     /* update the mode to compatability mode */
     file->mode += 2;

     /* check for text format */
     /* fix for embedded text format  */
     q = (char*) &qhdr->hdr;
     ea = 0;
     if (*q == '#'){ /* starts #UU. */
	 if ( *(q + 1) == 'U' && *(q + 2) == 'U' &&
	      *(q + 3) == '.' ) {
	     *q = 'U';
	     *(q+2) = '#';
	     ea = 1;
	 }
     }/* end of embedded fix */

     if(ea || qhdr->fmt == 053056 || qhdr->fmt == 027126){ /* "V." or ".V */
	 /* read a text header */
	 p = qhdr->title;
	 q = (char *) &qhdr->hdr;
	 for (i = 0; i< 4; i++)
	     *p++ = *q++;

	 qhdr->fmt = 10; /* ascii text format */
	 q = p;
	 --p;
	 /* now get the rest up to a blank line */
	 i = P_TSIZE - 5;
	 while ( *q != '\n' || *p != '\n') {
	     q = p; 
	     p++;
	     i--;
	     if (!i){  /* no more space in title */
		 /* bummer lets get rid of the rest of the story at least */
		 p--;
		 q--;
		 while (( *q != '\n' || *p != '\n') ) {
		     *q=*p;
		     if   (read(*fdp, p, 1) != 1) 
			 break;
		 } 
		 break;
	     }
	     if ( read(*fdp, p, 1) != 1 )
		 break;
	 }
	 *p = '\0';

	 /* set local machine correct */
	 (void) strncpy(qhdr->machine, LOCAL_MACHINE_NAME, P_MSIZE);
	 return(0);
     }
     else 
	 if (qhdr->hdr != VisXMAGIC) {/* last check VisX-3 hdr */
	     return (1);
	 }
     /* read the rest and exit */
     return ( read(*fdp, 4 + (char*) qhdr, VisXSIZE - 4) != (VisXSIZE - 4));
} /* end rdhead */

/* fix_machine():
 * return true if we can fix the machine type
 */
static int
fix_machine(qhdr, Gname)
VisXhdr_t *qhdr;
char *Gname;
{
    char mach[P_MSIZE+1];

    /*
     * if the error handler is not the default 
     * we don't fix the machine type automatically
     */
     /* but why not?? lets give it a go.... APR 5/98
    if ((nerr = Vseterror(0)) != PE_ERR_DEFAULT) {	
	(void) Vseterror(nerr);
	return(1);
    }
    */

    switch (machine_type(qhdr->machine)) {
    case UNKNOWN_TYPE :
	Verrno = PE_BAD_MACH;
	Verror("%s: Cannot open `%s' (%s `%s').\n", 
		visx_pname, Gname, Verrorstr(), "*unknown*");
	return(0);
    case LOCAL_MACHINE_TYPE :
	break;
    default :
	/* 
	 * we know what we are dealing with, but we can
	 * only fix byte files now... This might change..
	 */
	(void) strncpy(mach, qhdr->machine, P_MSIZE);
	mach[P_MSIZE] = '\0';
	(void) fix_header(qhdr, qhdr->machine, LOCAL_MACHINE_NAME);
	if ( qhdr->fmt != VFMT_UBYTE ) {
	    Verrno = PE_BAD_MACH;
	    Verror("%s: Cannot open `%s' (%s `%s').\n", 
		    visx_pname, Gname, Verrorstr(), mach);
	    return(0); 
	}
    }
    return(1);
} /* end fix_machine */


/* addtitle():
 *	Prepend the string to the title  (old v3 used once)
 */
void
addtitle(qhdr, title)
VisXhdr_t *qhdr;
char *title;
{
    char *p, *q;
    int i;
    char buffer[P_TSIZE+1];

    if ( title == (char *) 0 ) 
	return;

    /*
     * Save the current title
     */
    memcpy(buffer, qhdr->title, P_TSIZE);
    /*
     * copy the new title
     */
    p = qhdr->title; 
    for ( i = 0, q = title ; i < P_TSIZE - 2 && *q != '\0'; i++ )
	*p++ = *q++;
    for (        q = buffer; i < P_TSIZE - 2 && *q != '\0'; i++ )
	*p++ = *q++;

    /* 
     * make sure we terminate 
     */
    if ( *q != '\0' ) {
	*p++ = '\n';
	*p++ = '\0';
    }
} /* end addtitle */

#ifdef WINMinGW
#include "VXwin.c"
#endif

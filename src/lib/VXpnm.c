
/*
 *  VisionX Conversion to and from Portable Bitmap/Graymap/Pixmap Format
 *      and
 *  VisionX Conversion to TIFF Format
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


/*
 * pnmtovx:  create a VisionX list from an opened file
 *   (the pbm format pads each row and starts each row with a fresh byte)
 * vxtopsnm: write just one (the first) image in list to an opened file
 * 
 */

#ifndef BSD
#include <string.h>
#else
#include <strings.h>
#endif
#include "VisXV4.h"
#include "Vutil.h"
#include "Verror.h"

extern char *VisXhist;

#define FMT_PBM 1
#define FMT_PGM	2
#define FMT_PPM	3

static char *history = NIL(char);
/*
 *  token reader for pnm files
 *
 */
static char *
gettoken(fp)
FILE *fp;
{
    static char line[BUFSIZ];
    static char *ptr = NIL(char);

    while (1) {
	if (ptr == NIL(char)) {
	    if (fgets(line, BUFSIZ, fp) == NIL(char)) {
		(void) fprintf(stderr, "Unexpected end of pnm file.\n" ); 
		exit(1);
	    }
	    if (*line == '#'){
		if (history == NIL(char)){
		    history = Vmalloc(strlen(line) );
		    strcpy(history, line + 1);
		}else{
		    history = Vrealloc(history, strlen(history) +
					        strlen(line) );
		    strcat(history, line + 1);
		}
		ptr = NIL(char);
		continue;
	    }
	    if ((ptr = (char *) strtok(line, "\n\t ")) == NIL(char))
		continue;
	}
	else
	    if ((ptr = (char *) strtok(NIL(char), "\n\t ")) == NIL(char))
		continue;
	return(ptr);
    }
}

 /* pnmtovx
  *
  * create a VisionX list from a opened file
  * --fix to have the format preread
  * sflag == 1, scale input data from 0-255
  */

void
pnmtovx(fp, list, sflag)
FILE    *fp;
VisXelem_t *list;
int sflag;
{
    VisXelem_t *vpt;
    unsigned char   *buffer, *p;
    char *ptr;
    int   format, binary, x, y, maxval, nr;
    int xb;
    int dsize;

    float bbx[6];
    int ptype;
    int chn;

    /* check here if first line read */
    ptr = gettoken(fp);
    /*
     * P1: Pbm ascii
     * P2: Pgm ascii
     * P3: Ppm ascii
     * P4: Pbm binary
     * P5: Pgm binary
     * P6: Ppm binary
     */
    if (ptr[0] != 'P' || strchr("123456", ptr[1]) == NIL(char)) {
	(void) fprintf(stderr, " pbm/pgm/pnm file format error\n"); 
	exit(1);
    }
    if ((format = ptr[1] - '0') > FMT_PPM) {
	format -= FMT_PPM;
	binary = 1;
    }
    else
	binary = 0;

    ptr = gettoken(fp);
    if (sscanf(ptr, "%d", &x) != 1) {
	(void) fprintf(stderr, "Error reading x size from pnmfile.\n"); 
	exit(1);
    }
    ptr = gettoken(fp);
    if (sscanf(ptr, "%d", &y) != 1) {
	(void) fprintf(stderr, "Error reading y size from pnmfile\n");
	exit(1);
    }
    if (format != FMT_PBM) {
	ptr = gettoken(fp);
	if (sscanf(ptr, "%d", &maxval) != 1) {
	    (void) fprintf(stderr, "Error reading maxval from pnm file\n"); 
	    exit(1);
	}
    }

    /* create image structure */

    vpt = list = VXinit();

    bbx[0] = bbx[2] = 0.0;
    bbx[1] = x;
    bbx[3] = y;
    bbx[4] = bbx[5] = 0.0;
    vpt = VXaddelem(vpt, VX_BBX, (char*)bbx, 6 * sizeof(float));

    chn = format == FMT_PPM ? 3 : 1;
    if (chn != 1)
	vpt = VXaddelem(vpt, VX_PCHAN, NIL(char), chn);

    ptype = format == FMT_PBM ? VX_PBIT : VX_PBYTE;
    nr = x * y * chn;
    dsize = nr;
    if (format == FMT_PBM)
       dsize = (dsize +7)/8;
    vpt = VXlnkelem(vpt, ptype, buffer = Vcalloc(dsize, 1), chn);

    /* there is the problem for the 
       funny sized pbm binary.... not a simple fix
    */



    if (binary) {
     if (format == FMT_PBM) { /* this needs to be fixed !!!!!!! */
	/* find rsize */
	xb = x >> 3;
	if (x != (xb << 3)) xb++;
	x = xb << 3;
        nr = (x * y ) / 8;
     }
	if (fread(buffer, sizeof(unsigned char), nr, fp) != nr) {
	    (void) fprintf(stderr, "Unexpected end of pnm file.\n");
	    exit(1);
	}
    }
    else {
	if (format != FMT_PBM)
	    for (p = buffer; p < &buffer[nr]; *p++ = atoi(gettoken(fp)));
	else
	  for (p = buffer; p < &buffer[nr]; p++) /* fix this no need to pad */
		for (*p = 0, x = 0; x < 8; x--)
		    *p |= atoi(gettoken(fp)) << x;
    }

    if (sflag) {
	if (format == FMT_PBM) {
	    (void) fprintf(stderr, "scale is invalid with PBM files.\n");
	    exit(1);
	}
	else if (maxval != 255) /* this will not work for big maxval */
	    for (p = buffer; p < &buffer[nr]; p++)
		*p = (int) (255 * *p) / maxval;
    }

    if (history != NIL(char)){ /* set the history to this file? */
	/* think about this... should only set if VisXhist not set */
	if(VisXhist != NIL(char)) Vfree(VisXhist);
	VisXhist = history;
	history = NIL(char);
    }

}



static void dopidx();
static void writepnm();

static int VX_ptypelist[] = {VX_PBYTE, VX_PSHORT, VX_PFLOAT, VX_PDOUBLE,
			    VX_PCHAR, VX_PBIT, VX_PINT, VX_PIDX, VX_NULLT};

/*
 *  vxtopnm: write just one (the first) image in list to an opened file
 *
 */
void
vxpnm(fd, list,  aflag)
int fd;
VisXelem_t *list;
int aflag;
{
VisXelem_t *ptr;
VisXelem_t *flist;
FILE    *fp;

      fp = fdopen(fd, "w");

      flist = VXNIL;
      ptr = VXfindin(list,VX_ptypelist);
      if(ptr == VXNIL){
	 fprintf(stderr, "vxtopnm: no image found\n");
	 exit(1);
      }
      if (VXtype(ptr) != VX_PIDX)
         writepnm ( fp, ptr, flist, aflag);
      else
         dopidx ( fp, ptr, flist, aflag);
      fflush(fp);
}


static void
writepnm (fp, vp, hlist, aflag)
FILE    *fp;
VisXelem_t *vp;
VisXelem_t *hlist;
char *aflag;
{
    unsigned char   *buffer, *p;
    char *ttle, *cpt;
    int chan;
    int   i, x, y, format, nr;
    float *bbx;
    char ctitle[BUFSIZ];
    VisXelem_t *vfpt;

    bbx = (float*)VXdata(vp->prev);
    x = (int)(bbx[1] - bbx[0]);
    y = (int)(bbx[3] - bbx[2]);

    chan = VXpchan(vp);

    format = -1;
    if( VXtype(vp) == VX_PBIT && chan == 1)
	format = FMT_PBM;

    if( VXtype(vp) == VX_PBYTE ){
	if (chan == 3) 
	    format = FMT_PPM;
	if (chan == 1) 
	    format = FMT_PGM;
    }

    if (format == -1) {
	(void) fprintf(stderr, 
		       "%s: Unsupported file: (format=%d,chan=%d).\n",
		       "vxpnm", VXtype(vp), chan );
	exit(1);
    }
    nr = VXsize(vp);
    buffer = VXdata(vp);
    /* print magic number */
    (void) fprintf(fp, "P%d\n", format + (aflag == NULL) * 3);

    /* Print header info */
  
     if ((vfpt = VXfind(hlist, VX_FHIST)) != VXNIL){
       ttle = VXdata(vfpt);
       while ((cpt = strchr(ttle,'\n')) != NIL(char )){
            memcpy(ctitle, ttle, (int) 1+ (cpt - ttle));
            ctitle[(int) 1+ (cpt - ttle)] = '\0';
            ttle = cpt + 1;
            fprintf(fp, "# %s",ctitle);
       }
     }
     if ((vfpt = VXfind(hlist, VX_FCMND)) != VXNIL){
       ttle = VXdata(vfpt);
       while ((cpt = strchr(ttle,'\n')) != NIL(char )){
            memcpy(ctitle, ttle, (int) 1+ (cpt - ttle));
            ctitle[(int) 1+ (cpt - ttle)] = '\0';
            ttle = cpt + 1;
            fprintf(fp, "# %s",ctitle);
       }
     }

    /* print xy size */
    (void) fprintf(fp, "%d %d\n", x, y);
    if (format != FMT_PBM)
	(void) fprintf(fp, "255\n");

    if (aflag) {
	if (format != FMT_PBM) 
	    for (i = 0, p = buffer; p < &buffer[nr]; p++, i += 4) {
		(void) fprintf(fp, "%3d ", *p);
		if (i >= 60) {
		    (void) fputc('\n', fp);
		    i = 0;
		}
	    }
	else
	    for (i = 0, p = buffer; p < &buffer[nr]; p++, i += 16) {
		for (x = 7; x >= 0; x--)
		    (void) fputs((*p & (1 << x)) ? "1 " : "0 ", fp);
		if (i >= 60) {
		    (void) fputc('\n', fp);
		    i = 0;
		}
	    }
    }
    else 
	if (fwrite(buffer, sizeof(unsigned char), nr, fp) != nr) {
	    (void) fprintf(stderr, "pnm Write failed.\n");
	    exit(1);
	}

    return;
}

static void
dopidx(fp, ptr, hlist, aflag)
VisXelem_t *ptr;
VisXelem_t *hlist;
FILE *fp;
char *aflag;
{
VisXelem_t *tptr;
VisXelem_t *tlist, *tlptr;
int imax;
int npix, ncpix;
unsigned char *lut;
unsigned char *ibuf, *dbuf;
unsigned char *iptr;
int clr, ncl;
int i,j;


	if(VXNIL == (tptr = VXbfind(ptr, VX_CMAP))){ /* get map */
	     /* no map so fake it */
	     VXtype(ptr) = VX_PBYTE;
             writepnm (fp, ptr, hlist, aflag);
	 }else{
	     /* find if a grey map */
	      lut = VXdata(tptr);
	      ncl = VXsize(tptr);
	      imax = ncl/3;
	      clr = 1;
	      for (i = 0; i< ncl && clr; i += 3){
		   if(lut[i] != lut[i + 1] ||
		      lut[i] != lut[i + 2] )
		          clr = 0;
	      }
	      npix = VXsize(ptr);
	      if(!clr){
		ncpix = 3 * npix;
		dbuf = (unsigned char*) VXdata(ptr);
		iptr = ibuf = (unsigned char*) Vmalloc(ncpix);

		for (i = 0; i< npix; i++){
		  j = 3 * MIN((imax -1), dbuf[i]);
		  *iptr++  = lut[j  ];
		  *iptr++  = lut[j+1];
		  *iptr++  = lut[j+2];
		}
		tlist = tlptr = VXinit();
		tptr = VXbfind(ptr, VX_BBX);
		tlptr = VXaddelem(tlptr, VX_BBX, VXdata(tptr),
		        VXsize(tptr));
		tlptr = VXlnkelem(tlptr, VX_PBYTE, ibuf, ncpix);
                writepnm (fp, tlptr, hlist, aflag);
		VXdellist(tlist);
	      }else{
	         VXtype(ptr) = VX_PBYTE;
		 dbuf = (unsigned char*) VXdata(ptr);
		 iptr = dbuf;
		 for (i = 0; i< npix; i++){
		    j = 3 * MIN((imax -1), dbuf[i]);
		    *iptr++  = lut[j  ];
		 }
                 writepnm (fp, ptr, hlist, aflag);
	      }
	 }
   return;     
}



/*
 * vxtotif: write just one (the first) image in list to an opened file
 */


#ifdef VLITTLE_ENDIAN
#undef VBIG_ENDIAN
#endif

typedef struct {
short id;
short field;
long  cnt;
union{
long  lv;
short sv;
} v;
} tagt;

tagt *tag;

static int VX_tiflist[] = {VX_PFLOAT, VX_PSHORT, VX_PBYTE, VX_PIDX, VX_NULLT};

void
vxtotif(fd, list )
int    fd;
VisXelem_t *list;
{

    unsigned char   *buffer;
    int chan;
    int   i, x, y, format, nr;
    float *bbx;
    unsigned char thdr [256];
    VisXelem_t *vp;
    VisXelem_t *vm;
    long *hptr;
    long clr, mapl;
    long base;
    unsigned short *ntag, *sptr;
    unsigned char * mvp;

    unsigned short map[768];

    vp = VXfindin(list,VX_tiflist);
    if(vp == VXNIL){
	 fprintf(stderr, "vxtotif: no image found.\n");
	 exit(1);
    }

    bbx = (float*)VXdata(vp->prev);
    x = (int)(bbx[1] - bbx[0]);
    y = (int)(bbx[3] - bbx[2]);

    chan = VXpchan(vp);
    if(chan != 1 && chan != 3){
	 fprintf(stderr, "vxtotif: no image found,\n");
	 exit(1);
    }


    base = 156; /* start of offsets */

    clr = 0;
    mapl = 0;
    if (chan == 3) {
       base += 12;
       clr = 8;
    }
    if (VXtype(vp) == VX_PIDX){
       base += 12;
       mapl =  768 * 2;
    }
    if (VXtype(vp) == VX_PSHORT){
       base += 12;
       //mapl =  768 * 2;
    }

    format = 1;

    if (format == -1) {
	(void) fprintf(stderr, 
		       "vxtogif: Unsupported file: (format=%d,chan=%d).\n",
		      VXtype(vp), chan );
	exit(1);
    }

    /* step 1 write the tif signature */
#ifdef VBIG_ENDIAN
    thdr[0] = thdr[1] = 'M';
    thdr[2] = 0; thdr[3] = 42;
#else
    thdr[0] = thdr[1] = 'I';
    thdr[2] = 42; thdr[3] = 0;
#endif
    hptr = (long*) thdr;
    hptr++;
    *hptr++ = 10; /* offset to first IFD */
    ntag = (short*) hptr;
    ntag++; /* lets have 4 byte boundary for offsets */   
    *ntag = 11;
    hptr++;
    tag = (tagt*) hptr;

    /* declare 11 + term records */
     i=0;
    tag[i].id    = 256; /* image width */
    tag[i].field =   4; /* LONG */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   x;
    i++;
    tag[i].id    = 257; /* image length */
    tag[i].field =   4; /* LONG */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   y;
    i++;
    tag[i].id    = 258; /* Bits per sample */
    tag[i].field =   3; /* SHORT  */
    tag[i].cnt   =   1; 
    tag[i].v.sv  =   8;
    if (VXtype(vp) == VX_PSHORT){
     tag[i].v.sv  =   16;
    }
    if (VXtype(vp) == VX_PFLOAT){
     tag[i].v.sv  =   32;
    }
    if(chan == 3){
       tag[i].cnt   =   3; 
       tag[i].v.sv  =   base + 16;
    }
    i++;
    tag[i].id    = 259; /* compression (grey level) */
    tag[i].field =   3; /* SHORT  */
    tag[i].cnt   =   1; 
    tag[i].v.sv  =   1;
    i++;
    tag[i].id    = 262; /* photomet */
    tag[i].field =   3; /* SHORT  */
    tag[i].cnt   =   1; 
    tag[i].v.sv  =   1;
    if(chan == 3){
        tag[i].v.sv  =   2;
    }
    if (VXtype(vp) == VX_PIDX){
        tag[i].v.sv  =   3;
    }
    i++;
    tag[i].id    = 273; /* strip offsets */
    tag[i].field =   4; /* LONG  */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   base + 16 + clr + mapl;  /* offset to data */
    if(chan == 3 ){ /* RGB */
      /* if(VFLAG) fprintf(stderr,"RGB (samples/pixel) tag added\n"); */
      i++;
      tag[i].id    = 277; /* samples/pixel */
      tag[i].field =   3; /* SHORT  */
      tag[i].cnt   =   1; 
      tag[i].v.sv  =   chan;
      (*ntag)++;
    }
    i++;
    tag[i].id    = 278; /* Rows/strip */
    tag[i].field =   4; /* LONG  */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   y;
    i++;
    tag[i].id    = 279; /* stripbyte counts */
    tag[i].field =   4; /* LONG  */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   x * y * chan;
    if (VXtype(vp) == VX_PSHORT){
     tag[i].v.lv  =  x * y * chan * 2;
    }
    if (VXtype(vp) == VX_PFLOAT){
     tag[i].v.lv  =  x * y * chan * 4;
    } 
    i++;
    tag[i].id    = 282; /* xresolution */
    tag[i].field =   5; /* RATIONAL  */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   base; /* base + 0 */
    i++;
    tag[i].id    = 283; /* yresolution */
    tag[i].field =   5; /* RATIONAL  */
    tag[i].cnt   =   1; 
    tag[i].v.lv  =   base + 8;
    i++;
    tag[i].id    = 296; /* resolution  unit*/
    tag[i].field =   3; /* SHORT  */
    tag[i].cnt   =   1; 
    tag[i].v.sv  =   2; /* inch */
    if (VXtype(vp) == VX_PIDX){
       i++;
       tag[i].id    = 320; /* color map */
       tag[i].field =   3; /* SHORT  */
       tag[i].cnt   =   768; 
       tag[i].v.lv  =   base + 16;
       (*ntag)++;
    }
    if (VXtype(vp) == VX_PSHORT){
      i++;
      tag[i].id    = 339; /* SampleFormat*/
      tag[i].field =   3; /* SHORT  */
      tag[i].cnt   =   1; 
      tag[i].v.sv  =   2; /* two's complement signed integer data */
      (*ntag)++;
    }
    if (VXtype(vp) == VX_PFLOAT){
      i++;
      tag[i].id    = 339; /* SampleFormat*/
      tag[i].field =   3; /* SHORT  */
      tag[i].cnt   =   1; 
      tag[i].v.sv  =   3; /* floating point */
      (*ntag)++;
    }
    i++;
    tag[i].id    =   0; /* the end*/
    tag[i].field =   0; /* SHORT  */
    tag[i].cnt   =   0; 
    tag[i].v.lv  =   0;

    /* OK so we start with header=8 + cnt = 4 + 12 * 12 */
    /* offset here for the data is 156    */
    hptr = (long *)(thdr + base);
    *hptr++ = 26; /* xres */
    *hptr++ = 1;
    *hptr++ = 26;/* yres */
    *hptr++ = 1;
    if(chan == 3){
     sptr = (unsigned short *)hptr;
     *sptr++ = 8;
     *sptr++ = 8;
     *sptr++ = 8;
     hptr++;
    }

    /* end except for data at base + 16 */


    Vwrite(fd, thdr, base + 16 + clr);

    if (VXtype(vp) == VX_PIDX){
       for (i=0; i<256; i++) {
	  map[i] = map[i+256] = map[i+512] = i <<8;
       }
       if (VXNIL != (vm = VXbfind(vp, VX_CMAP))){
          mvp  = (unsigned char *) VXdata(vm);
          for (i=0; i<(VXsize(vm)/3); i++) {
              map[i] = ((unsigned short)mvp[3*i]) << 8;
              map[i+256] = ((unsigned short)mvp[1 + 3*i]) << 8;
              map[i+512] = ((unsigned short)mvp[2 + 3*i]) << 8;
	  }
       }
       Vwrite(fd, (char*)map, 768 * 2);
    }

    /* step 5 now the data */
    nr = VXsize(vp);
    buffer = VXdata(vp);
    Vwrite(fd, buffer, nr);

    /* done */
}

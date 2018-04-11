/*
 *    standard file utilities for VisX 4
 *
 *    (for VXopen see Vxopen.c)
 *    VXclose     -- close a file
 *
 *    VXreadelem  -- read an element from a file
 *    VXwritelem  -- write an element to a file
 *
 *    VXread    -- read an entire data file 
 *    VXwrite   -- write an entire data file including header
 *
 *    VXfupdate      -- update global constants
 *    VXreadframe    -- read a data frame
 *    VXwriteframe   -- write a data frame
 *
 *    Vread     -- VisX file read
 *    Vwrite    -- VisX file write
 *
 *    VXpchan -- find the number of channels in an image
 *
 *    VXhistset -- change the global file history
 *
 *    VXtextf -- write the header and return file pointer to a text file
 *
 *    VXvread        -- scan the list structure from a file
 *    VXvreadelem    -- read a virtual list element
 *    VXvdata        -- return the vector data for an element
 *
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


#include "VisXV4.h"
#include "Vutil.h"
/* for V3 compatability */
#include "VisX.h"
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include    "VXpnm.h"   /* PNM, TIFF File i/o */

char *VisXhist= NIL(char);

extern char *strtok();

/*
 *    byteswap code for native byteswap
 */

char t;
#define  SWAP2(A)  t = ((char*)A)[0];               \
                   ((char*)A)[0] = ((char*)A)[1];   \
                   ((char*)A)[1] = t;   
#define  SWAP4(A)  t = ((char*)A)[0];               \
                   ((char*)A)[0] = ((char*)A)[3];   \
                   ((char*)A)[3] = t;               \
                   t = ((char*)A)[1];               \
                   ((char*)A)[1] = ((char*)A)[2];   \
                   ((char*)A)[2] = t;   
#define  SWAP8(A)  t = ((char*)A)[0];               \
                   ((char*)A)[0] = ((char*)A)[7];   \
                   ((char*)A)[7] = t;               \
                   t = ((char*)A)[1];               \
                   ((char*)A)[1] = ((char*)A)[6];   \
                   ((char*)A)[6] = t;               \
                   t = ((char*)A)[2];               \
                   ((char*)A)[2] = ((char*)A)[5];   \
                   ((char*)A)[5] = t;               \
                   t = ((char*)A)[3];               \
                   ((char*)A)[3] = ((char*)A)[4];   \
                   ((char*)A)[4] = t;   


private void convertcomp(dbuf)
char *dbuf;
{
VisXelem_t * eptr;
int base;
int numl, flag, i;
char *ptr;
     eptr = (VisXelem_t *)dbuf;
     flag = 1;
     while(flag){
         SWAP4(&eptr->type);
         SWAP4(&eptr->size);
         SWAP4(&eptr->next);
         SWAP4(&eptr->prev);
         SWAP4(&eptr->data);
         base =  eptr->type & 0xf;
         if (base != 0xa && base != 0x9 && eptr->size != 0){
            ptr = dbuf + (int)eptr->data;
            switch (base){
    		case VX_SHORT:
	      		numl = eptr->size/2;
	      		for (i=0 ; i< numl; i++){
		 		SWAP2(ptr);
		 		ptr+= 2;
	      		}
	      		break;
    		case VX_FLOAT:
    		case VX_INT:
	      		numl = eptr->size/4;
	      		for (i=0 ; i< numl; i++){
		 		SWAP4(ptr);
		 		ptr+= 4; } break;
    		case VX_DOUBLE:
	      		numl = eptr->size/8;
	      		for (i=0 ; i< numl; i++){
		 		SWAP8(ptr);
		 		ptr+= 8;
	      		}
	      		break;
    		case VX_COMP:  fprintf(stderr,
			   "packed packed data not yet supported\n");
	      		exit(1);
	      		break;
    		default: break;
    		}
	 }
	 if (0 == (int)eptr->next){
	    flag = 0;
	 }else{
            eptr = (VisXelem_t *)(dbuf + (int)eptr->next);
	 }
    }
    return;

}

void VXbsdata(ele)
VisXelem_t * ele;
{
char *ptr;
int base,size;
int numl, i;
    base =  ele->type & 0xf;
    size =  ele->size;
    if (base != 0xa && base != 0x9 && size != 0){

    ptr = ele->data;
    switch (base){
    case VX_SHORT:
	      numl = size/2;
	      for (i=0 ; i< numl; i++){
		 SWAP2(ptr);
		 ptr+= 2;
	      }
	      break;
    case VX_FLOAT:
    case VX_INT:
	      numl = size/4;
	      for (i=0 ; i< numl; i++){
		 SWAP4(ptr);
		 ptr+= 4; } break;
    case VX_DOUBLE:
	      numl = size/8;
	      for (i=0 ; i< numl; i++){
		 SWAP8(ptr);
		 ptr+= 8;
	      }
	      break;
    case VX_COMP:  convertcomp(ptr);
	      break;
    default: break;
    }
    return;
    }
}
/*
 *    VXreadelem ( fl ) - read an element from a file
 */

VisXelem_t *
VXreadelem( fl)
VisXfile_t *fl;
{
VisXelem_t * item;
int base;

    item = (VisXelem_t*) Vmalloc( sizeof (VisXelem_t));
    if( 0 == Vread(fl->fd,(char *) &item->type, VX_ELEMX))
	 return NIL(VisXelem_t);
    if(fl->bswap){
              SWAP4(&item->type);
              SWAP4(&item->size);
    }
    /* get format and length */
    base =  item->type & 0xf;
    if (base != 0xa && base != 0x9){
        item->data = Vmalloc( item->size );
        Vread(fl->fd, item->data, item ->size); /* get data */
        if(fl->bswap){
             VXbsdata(item);
        }
    }
    return item;
}

/*
 *    VXreadv3g( fl ) - read an element from a  V3 graph file
 *    For V3 compatability
 */

VisXelem_t *
VXreadv3g( fl)
VisXfile_t *fl;
{
VisXelem_t * item;
int buf[2];

    item = (VisXelem_t*) Vmalloc( sizeof (VisXelem_t));
    if( 0 == Vread(fl->fd,(char *) buf, 8))
	 return NIL(VisXelem_t);
    item->type = buf[1];
    item->size = buf[0];
    /* get format and length */
        item->data = Vmalloc( item->size );
        Vread(fl->fd, item->data, item ->size); /* get data */
    /* fix for object id */
    if(item->type == 0x07){
       item->type = VX_ID;
       item->size = *((int*) item->data);
       Vfree(item->data);
       item->data = NIL(char);
    }
    return item;
}

/*
 *
 *    VXreadv3im -- read a VisX image
 *
 */
static int VisXimindex = 1;
int
VXreadv3im(list, fl)
VisXfile_t *fl;
VisXelem_t * list;
{
VisXelem_t * item;
VisXelem_t * ptr;
VisXhdr_t imhdr;
float bbx[4];
       if (sizeof(VisXhdr_t) !=
			 Vread(fl->fd, (char *) &imhdr, sizeof(VisXhdr_t))){
	  return 0;
	  }
       ptr = VXlast(list);
       ptr = VXaddelem(ptr, VX_FRAME, NIL(char), VisXimindex);
       if (imhdr.fmt < 8 && imhdr.fmt > 0){
       /* regular image file so get size params */
	   bbx[0] = 0;
           bbx[1] = (float) imhdr.xsize;
	   bbx[2] = 0;
	   bbx[3] = (float) imhdr.ysize;
           ptr = VXaddelem(ptr, VX_BBX, (char *) bbx, sizeof(bbx));
	   /* PCHAN not needed
	   if(imhdr.nchan != 1)
		 ptr = VXaddelem(ptr, VX_PCHAN, NIL(char), imhdr.nchan);
	    */

       }else{
	  fprintf(stderr, "V3 image set format error fmt=%d\n",imhdr.fmt);
	  exit(1);
       }
        item = (VisXelem_t*) Vmalloc( sizeof (VisXelem_t));
	item->next = VXNIL;
	item->prev = ptr;
	switch (imhdr.fmt){
	case VFMT_UBYTE:
		    item->type = VX_PBYTE; break;
	case VFMT_SHORT:
		    item->type = VX_PSHORT; break;
	case VFMT_FLOAT:
		    item->type = VX_PFLOAT; break;
	case VFMT_DOUBLE:
		    item->type = VX_PDOUBLE; break;
	case VFMT_SBYTE:
		    item->type = VX_PCHAR; break;
	case VFMT_BPL:
		    item->type = VX_PBIT; break;
	case VFMT_LONG:
		    item->type = VX_PINT; break;
		   
	}
	ptr->next = item;
	ptr = item;


    /* get format and length */
	item->size = ((int) imhdr.xsize * (int) imhdr.ysize *
		     (int) imhdr.zsize * (int) imhdr.nchan )/8;
    
        item->data = Vmalloc( item->size );
        Vread(fl->fd, item->data, item->size); /* get data */
	  ptr = VXaddelem(ptr, VX_EFRAME, NIL(char), VisXimindex);
    VisXimindex++;
    return 1;
}
/*
 *
 *    VX4rdv3im -- read a VisX3 image with V4 info
 *
 */
int
VX4rdv3im(ptr, VSin, numpix)
VisXelem_t * ptr;
VisXfile_t *VSin;
int numpix;
/* return Vread response */
{
int fmt;
    fmt = (VSin->mode >> 3);
    switch(fmt){
       case  VFMT_UBYTE:
			ptr->type = VX_PBYTE;
			ptr->size = numpix;
		        break;
       case  VFMT_SHORT:
			ptr->type = VX_PSHORT;
			ptr->size = numpix * sizeof(short);
		        break;
       case VFMT_FLOAT:
			ptr->type = VX_PFLOAT;
			ptr->size = numpix * sizeof(float);
		        break;
       case VFMT_DOUBLE:
			ptr->type = VX_PDOUBLE;
			ptr->size = numpix * sizeof(double);
		        break;
       case VFMT_SBYTE:
			ptr->type = VX_PCHAR;
			ptr->size = numpix;
		        break;
       case VFMT_BPL:
			ptr->type = VX_PBIT;
			ptr->size = numpix/8;
		        break;
       case VFMT_LONG:
			ptr->type = VX_PINT;
			ptr->size = numpix * sizeof(long);
		        break;
    }
    ptr->data = Vmalloc(ptr->size);
    return( Vread(VSin->fd, ptr->data, ptr->size));
}

/*
 * VXwritelem ( fl) - write an element to a file
 */
void
VXwritelem (fl, item)
VisXfile_t *fl;
VisXelem_t * item;
{
int base;
    Vwrite (fl->fd, (char *) &item->type, VX_ELEMX);
    base =  item->type & 0xf;
    if (base != 0xa && base != 0x9){
        Vwrite (fl->fd, item->data, item->size);
    }
}

#include "Verror.h"
char **g_argv = 0;
int g_argc = 0;

extern char *visx_pname;

int _visx_eof = 0;

int
Veof()
{
    return(_visx_eof);
}

int
Vread(fd, buf, len)
int fd,len;
char *buf;
{
    int tl,tr;
    tl = len;

    _visx_eof = 0;
    if (g_argc != 0)
	visx_pname = *g_argv;
    else
	visx_pname = "Vread";

    while(tl > 0){
	if ((tr = read(fd, buf, tl)) < 0) {
	    Verrno = errno;
	    Verror("%s: Read Failed (%s).\n",
		visx_pname, Verrorstr());
	    return(tr);
	}
	else if ( tr == 0 ) {
	    if (len > 0) _visx_eof = 1;
	    return(0);
	}
		
	buf += tr;
	tl -= tr;
    }

    if (len > 0) _visx_eof = 0;
    return(len);
}


int
Vwrite(fd, buf, len)
int fd,len;
char *buf;
{
    int tl,tr;
    tl = len;

    _visx_eof = 0;
    if (g_argc != 0)
	visx_pname = *g_argv;
    else
	visx_pname = "Vwrite";

    while(tl > 0){
	if ((tr = write(fd, buf, tl)) < 0) {
	    Verrno = errno;
	    Verror("%s: Write Failed (%s).\n",
		visx_pname, Verrorstr());
	    return(tr);
	}
	else if ( tr == 0 ) {
	    if (len > 0) _visx_eof = 1;
	    return(0);
	}
		
	buf += tr;
	tl -= tr;
    }

    if (len > 0) _visx_eof = 0;
    return(len);
}

/* 
 *  VXread -- read an entire data file
 */
/*      includes VXinit       	 	 */

VisXelem_t *
VXread (VSin)
VisXfile_t *VSin;

{
VisXelem_t * list;
VisXelem_t * last;
VisXelem_t * flast;
VisXelem_t * ptr;
int v3fmt, numpix;
float * bbx;
int pchan;
char *ttle, *ohist;
int hlen;
int fflag;

   if(VSin->mode == 0 ){
       list = VXinit();
       last = list;
       if (VSin->list == NIL (VisXelem_t)){ 
	   VSin->list = VXinit();
       }
       flast = VXlast(VSin->list);
       /* first skip over the file info */
	    ttle = ohist = "";
       fflag = 1;
       while (fflag && (ptr = VXreadelem(VSin)) != NIL(VisXelem_t)) {
	    switch(ptr->type){
	    case  VX_FNAME:
	    case  VX_FUID:
	    case  VX_FMACH:    /* mem leak should plug one day */
	    case  VX_FTIME:
			break;
	    case  VX_FCMND:
			ttle = ptr->data;
			break;
	    case  VX_FHIST:
			ohist = ptr->data;
			break;
	    default:
	    	ptr->next =  NIL(VisXelem_t );
	    	last->next = ptr;
	   	ptr->prev = last;
	        /* DECODE packed  */
		if(VisXupackopt != 2 && ptr->type == VX_COMP){
		    VXupklist(ptr);
		    last = VXlast(last);
		}else
	    	    last = ptr;

	        fflag = 0;
	        break;
	    }
	    if(fflag) { /* add to file list */
	    	ptr->next =  NIL(VisXelem_t );
	    	flast->next = ptr;
	   	ptr->prev = flast;
	    	flast = ptr;
	    }
       }
       while ((ptr = VXreadelem(VSin)) != NIL(VisXelem_t)) {
	    ptr->next =  NIL(VisXelem_t );
	    last->next = ptr;
	    ptr->prev = last;
	    /* DECODE packed  */
	    if(VisXupackopt != 2 && ptr->type == VX_COMP){
	         VXupklist(ptr);
	         last = VXlast(last);
	    }else
	        last = ptr;
       }
       if(VisXhist == NIL(char)){
	     hlen = strlen(ttle) + strlen(ohist);
	     if(hlen){
		  VisXhist = Vmalloc(hlen + 1);
		  strcpy(VisXhist, ohist);
		  strcat(VisXhist, ttle);
	     }
       }
       return list;
   }else{ /* compatability mode */
       v3fmt = (VSin->mode >> 3);
       list = VXinit();
       last = list;
       numpix = 0;
       if((ptr=VXfind(VSin->list, VX_BBX)) != NIL(VisXelem_t )){
		   bbx = (float *)ptr->data;
		   numpix = (int) (bbx[1] - bbx[0]) * (bbx[3] - bbx[2]);
		   last = VXaddelem(last, ptr->type, ptr->data, ptr->size);
	}
       if((ptr = VXfind(VSin->list, VX_PCHAN)) != NIL(VisXelem_t )){
		   pchan = ptr->size;
		   numpix *= pchan;
       }
       v3fmt = (VSin->mode >> 3);
       /* check here for # channels */
       switch(v3fmt){
       case  VFMT_UBYTE:
       case VFMT_SHORT:
       case VFMT_FLOAT:
       case VFMT_DOUBLE:
       case VFMT_SBYTE:
       case VFMT_BPL:
       case VFMT_LONG:
       		     ptr = NEW(VisXelem_t);
                     VX4rdv3im(ptr, VSin, numpix);
		     break;
       case  VFMT_GRAPH:
                while ((ptr = VXreadv3g(VSin)) != NIL(VisXelem_t)) {
	             ptr->next =  NIL(VisXelem_t );
	             last->next = ptr;
	             ptr->prev = last;
	             last = ptr;
                }
		       return list;
       case  VFMT_SET:
                while ( VXreadv3im(list, VSin)) {
		}
		break;
       default:
           fprintf(stderr, "VisX-3 file format %d not supported\n",v3fmt);
	   exit(1);
       }
       if (v3fmt != VFMT_SET){
          ptr->next =  NIL(VisXelem_t );
          last->next = ptr;
          ptr->prev = last;
          last = ptr;
	}
       return list;
   }
}

/* 
 *  VXreadframe -- read a data frame
 */
/*      includes VXinit       	 	 */

VisXelem_t *
VXreadframe (VSin)
VisXfile_t *VSin;

{
VisXelem_t * list;
VisXelem_t * last;
VisXelem_t * flast;
VisXelem_t * ptr;
int v3fmt, numpix;
float * bbx;
int pchan;
char *ttle, *ohist;
int hlen;
int fflag;

   if(VSin->mode == 0 ){
       list = VXinit();
       last = list;
       if (VSin->list == VXNIL ){ 
	   VSin->list = VXinit();
       }

       /* first ever */
       fflag = 0;
       if(VSin->last == VXNIL){ /* first skip over the file info */
	 ttle = ohist = "";
         fflag = 1;
         flast = VXlast(VSin->list);
         while (fflag && (ptr = VXreadelem(VSin)) != NIL(VisXelem_t)) {
	    switch(ptr->type){
	    case  VX_FNAME:
	    case  VX_FUID:
	    case  VX_FMACH:    /* mem leak should plug one day */
	    case  VX_FTIME:
			break;
	    case  VX_FCMND:
			ttle = ptr->data;
			break;
	    case  VX_FHIST:
			ohist = ptr->data;
			break;
	    default:
	    	ptr->next =  NIL(VisXelem_t );
	    	last->next = ptr;
	   	ptr->prev = last;
	        /* DECODE packed  */
	        if(VisXupackopt != 2 && ptr->type == VX_COMP){
	             VXupklist(ptr);
	             last = VXlast(last);
	        }else
	            last = ptr;
	        fflag = 0;
	        break;
	    }
	    if(fflag) { /* add to file list */
	    	ptr->next =  NIL(VisXelem_t );
	    	flast->next = ptr;
	   	ptr->prev = flast;
	    	flast = ptr;
	    }
         }
         if(VisXhist == NIL(char)){
	     hlen = strlen(ttle) + strlen(ohist);
	     if(hlen){
		  VisXhist = Vmalloc(hlen + 1);
		  strcpy(VisXhist, ohist);
		  strcat(VisXhist, ttle);
	     }
         }
	 VSin->last = flast;
	 fflag = 1;
       }
       /* end of first ever */
       /* look for a frame marker */
       VSin->last = VXlast(VSin->last);
       if (!fflag ||(ptr != VXNIL && ptr->type != VX_FRAME)){ /* keep looking */
          while ((ptr = VXreadelem(VSin)) != VXNIL) {
	    ptr->next =  VXNIL;
	    if (ptr->type == VX_FRAME){ /* update flist and carry on */
		if (list->next != VXNIL){
		   VSin->last->next = list->next;
		   list->next->prev = VSin->last;
		}
		/* VSin->last = VXlast(VSin->last); */
		list->next = ptr;
		ptr->prev = list;
		last = ptr;
		goto frameit;
            }
	    last->next = ptr;
	    ptr->prev = last;
	    /* DECODE packed  */
	    if(VisXupackopt != 2 && ptr->type == VX_COMP){
	         VXupklist(ptr);
	         last = VXlast(last);
	    }else
	        last = ptr;
          }
	  goto listreturn; /* no frame found */
       }

frameit:
       /* look for an end frame marker */
       while ((ptr = VXreadelem(VSin)) != VXNIL) {
	    ptr->next =  VXNIL;
	    last->next = ptr;
	    ptr->prev = last;
	    /* DECODE packed  */
	    if(VisXupackopt != 2 && ptr->type == VX_COMP){
	         VXupklist(ptr);
	         last = VXlast(last);
	    }else
	        last = ptr;
	    if (ptr->type == VX_EFRAME)
		goto listreturn;
       }
       goto listreturn;

   }else{ /* compatability mode */
       v3fmt = (VSin->mode >> 3);
       list = VXinit();
       last = list;
       numpix = 0;
       if((ptr=VXfind(VSin->list, VX_BBX)) != NIL(VisXelem_t )){
		   bbx = (float *)ptr->data;
		   numpix = (int) (bbx[1] - bbx[0]) * (bbx[3] - bbx[2]);
		   last = VXaddelem(last, ptr->type, ptr->data, ptr->size);
	}
       if((ptr = VXfind(VSin->list, VX_PCHAN)) != NIL(VisXelem_t )){
		   pchan = ptr->size;
		   numpix *= pchan;
       }
       v3fmt = (VSin->mode >> 3);
       /* check here for # channels */
       switch(v3fmt){
       case  VFMT_UBYTE:
       case VFMT_SHORT:
       case VFMT_FLOAT:
       case VFMT_DOUBLE:
       case VFMT_SBYTE:
       case VFMT_BPL:
       case VFMT_LONG:
       		     ptr = NEW(VisXelem_t);
                     if( VX4rdv3im(ptr, VSin, numpix) <= 0)
			       goto v3exit;
		     break;
       case  VFMT_GRAPH:
                while ((ptr = VXreadv3g(VSin)) != NIL(VisXelem_t)) {
	             ptr->next =  NIL(VisXelem_t );
	             last->next = ptr;
	             ptr->prev = last;
	             last = ptr;
                }
		       goto listreturn;
       case  VFMT_SET:
		/* just read one frame */
                VXreadv3im(list, VSin);
		break;
       default:
           fprintf(stderr, "VisX-3 file format %d not supported\n",v3fmt);
	   exit(1);
       }
       if (v3fmt != VFMT_SET){
          ptr->next =  NIL(VisXelem_t );
          last->next = ptr;
          ptr->prev = last;
          last = ptr;
	}
       goto listreturn;
   }
v3exit:
    VXdellist(list);
    return VXNIL;
listreturn:
   if(list->next == VXNIL){
      Vfree(list);
      return VXNIL;
   }else
       return list;
}


void VXwriteV3 ();


/*
 *   VXwrite  write an entire data file including header
 */
void
VXwrite (VSout, list)
VisXfile_t *VSout;
VisXelem_t * list;
{
VisXelem_t * ptr;
VisXelem_t * ptr2;

  if (VSout->mode == 1){/* write out header */
     Vwrite(VSout->fd , VX_MAGIC, 6);
     /* and the other stuff */
     ptr = VXfirst(VSout->list);
     if ((ptr2 = VXfind(ptr, VX_FHIST)) == NIL (VisXelem_t)
                   && VisXhist != NIL(char)){ /* add a hist */
         ptr2= VXaddelem ( VXlast(ptr), VX_FHIST, VisXhist, strlen(VisXhist)+1);
     }
     while (ptr != NIL(VisXelem_t)){
         VXwritelem(VSout, ptr);
         ptr = ptr->next;
     }
     /* now write all the data */
     ptr = VXfirst(list);
     /* PACK here */
     if (VisXpackopt){
	VXpklist(ptr);
        ptr = VXfirst(list);
     }
     while (ptr != NIL(VisXelem_t)){
         VXwritelem(VSout, ptr);
         ptr = ptr->next;
     }
  } else { /* V3 compatability */
     switch(VSout->mode){
       case 4:
         vxpnm (VSout->fd, list, 0);
	 break;
       case 5:
         vxpnm (VSout->fd, list, 1);
	 break;
       case 6:
         vxtotif (VSout->fd, list);
	 break;
       default:
         VXwriteV3 (VSout, list);
	 break;
     }
  }
}

/*
 *   VXwriteframe  write a data frame
 */
void
VXwriteframe (VSout, list)
VisXfile_t *VSout;
VisXelem_t * list;
{
VisXelem_t * ptr;
VisXelem_t * ptr2;
int hist;

  if (VSout->mode == 1){/* write out header */
   /* check for first time  or new globals */
    if (VSout->last == VXNIL || VSout->last->next !=VXNIL){
						  /* write out header */
       if (VSout->last == VXNIL){
           Vwrite(VSout->fd , VX_MAGIC, 6); /* first time */
           ptr = VXfirst(VSout->list);
           if ((ptr2 = VXfind(ptr, VX_FHIST)) == NIL (VisXelem_t)
                     && VisXhist != NIL(char)){ /* add a hist */
	      /* palce at the end of file stuff */
	      hist = 1;
	      ptr2 = ptr;
	      while( ptr2->next != VXNIL && hist){
		 switch(VXtype(ptr2->next)){
		    case VX_FNAME:
		    case VX_FMACH:
		    case VX_FCMND:
		    case VX_FTIME:
		    case VX_FUID:
			  ptr2 = ptr2->next;
			  break;
		    default: hist = 0;
			  break;
		 }
	      }
              ptr2= VXaddelem ( ptr2, VX_FHIST, VisXhist, strlen(VisXhist)+1);
           }
       } else /* just add new info */
	   ptr = VSout->last->next;

       while (ptr != NIL(VisXelem_t)){
           VXwritelem(VSout, ptr);
           ptr = ptr->next;
       }
       VSout->last = VXlast(VSout->list);
    }
     /* now write all the data */
     /* assume that frame markers are where they should be */
     ptr = VXfirst(list);
     /* PACK here */
     if (VisXpackopt){
	VXpklist(ptr);
        ptr = VXfirst(list);
     }
     while (ptr != NIL(VisXelem_t)){
         VXwritelem(VSout, ptr);
         ptr = ptr->next;
     }
  }else { /* V3 compatability */
     /* this is kind of dumb and will only work on simple files */
     switch(VSout->mode){
       case 4:
         vxpnm (VSout->fd, list, 0);
	 break;
       case 5:
         vxpnm (VSout->fd, list, 1);
	 break;
       case 6:
         vxtotif (VSout->fd, list);
	 break;
       default:
         VXwriteV3 (VSout, list);
	 break;
     }
  }
}
  
  
extern VisXhdr_t VisX3hdr;
  
/* pixel types */  
static int VX_ptypelist[] = {VX_PBYTE, VX_PSHORT, VX_PFLOAT, VX_PDOUBLE,
			    VX_PCHAR, VX_PBIT, VX_PINT, VX_NULLT};
void
VXwriteV3 (VSout, list)
VisXfile_t *VSout;
VisXelem_t * list;
{
VisXelem_t * ptr;
VisXelem_t * ptr2;
VisXhdr_t vx3;
float* bbox;
int idfix;
  
   /* V3 compatability */
   vx3 = VisX3hdr;
   vx3.hdr=VisXMAGIC;
   /* a pix type makes an image    */
   ptr = VXfirst(list);
   if (VXNIL != (ptr = VXfindin(ptr, VX_ptypelist))){
      /* two pix types make an image set */
       if (VXNIL != (ptr2 = VXfindin(ptr->next, VX_ptypelist))){
       /* image set write header */
            vx3.fmt = VFMT_SET;
            vx3.zsize = 8;
            vx3.xsize = -1;
            vx3.ysize = -1;
	    vx3.nchan = 1; 
            (void) Vwrite(VSout->fd, (char *) &vx3, sizeof(vx3));
       }
       ptr = list;
       while(VXNIL != (ptr = VXfindin(ptr, VX_ptypelist))){ /* write all */
	    switch (ptr->type){
	    case VX_PBYTE:
                 vx3.fmt = VFMT_UBYTE;
	         vx3.zsize = 8;
		 break;
	    case VX_PSHORT:
                 vx3.fmt = VFMT_SHORT;
	         vx3.zsize = 16;
		 break;
	    case VX_PFLOAT:
                 vx3.fmt = VFMT_FLOAT;
	         vx3.zsize = 32;
		 break;
	    case VX_PDOUBLE:
                 vx3.fmt = VFMT_DOUBLE;
	         vx3.zsize = 64;
		 break;
	    case VX_PCHAR:
                 vx3.fmt = VFMT_SBYTE;
	         vx3.zsize = 8;
		 break;
	    case VX_PBIT:
                 vx3.fmt = VFMT_BPL;
	         vx3.zsize = 1;
		 break;
	    case VX_PINT:
                 vx3.fmt = VFMT_LONG;
	         vx3.zsize = 32;
	    }
	    vx3.nchan = VXpchan(ptr); 
            /* find bounding box */
            if (VXNIL != (ptr2 = VXfind(list, VX_BBX)) ||
                VXNIL != (ptr2 = VXfind(VSout->list, VX_BBX))){
		     bbox = (float *) ptr2->data;
	             vx3.xsize = (int) bbox[1] - bbox[0];
	             vx3.ysize = (int) bbox[3] - bbox[2];
	    }else{
		     vx3.xsize = ptr->size / vx3.nchan;
		     vx3.ysize = 1;
	    }
	   
            /* write header */
            (void) Vwrite(VSout->fd, (char *) &vx3, sizeof(vx3));
            /* write data */
            (void) Vwrite(VSout->fd, ptr->data, ptr->size);
	    ptr = ptr->next;
      }


   }else{ 
      /* graph file */
      vx3.fmt = VFMT_GRAPH;
      vx3.xsize = -1;
      vx3.uchan = 4;
      vx3.ysize = -1; /* could be num elem */
      vx3.fmt = VFMT_GRAPH;
      (void) Vwrite(VSout->fd, (char *) &vx3, sizeof(vx3));
      ptr = VXfirst(list);
      while (ptr != NIL(VisXelem_t)){
           /* write node */
	   if(ptr->type != VX_ID){
            (void) Vwrite(VSout->fd, (char *)&ptr->size, sizeof(int));
            (void) Vwrite(VSout->fd, (char *)&ptr->type, sizeof(int));
            (void) Vwrite(VSout->fd, ptr->data, ptr->size);
	  }else{ /* special case for ID */
	    idfix = sizeof(int);
            (void) Vwrite(VSout->fd, (char *)&idfix, sizeof(int));
	    idfix = 0x07;
            (void) Vwrite(VSout->fd, (char *)&idfix, sizeof(int));
            (void) Vwrite(VSout->fd, (char *)&ptr->size, sizeof(int));
	  }
          ptr = ptr->next;
      }

   }
 
}

/*
 * VXclose.c: - close visx files and exit.
 *
 *	VXclose(VisXfile_t *) 
 *
 *	This routine is necessary for any visx routine that
 *	will seek its output file so that pipes may be set
 *	up correctly, but will close the file descriptor
 *	of any other file it is called with.
 *
 *	fd is the file descriptor of the  file in the
 *	  calling program.
 *
 *	name is a pointer to the string containing the name
 *	  that the file was opened with (this will usually be
 *	  an element in the array "par" used by Vparse).
 *
 *	access is the same access that the file was opened
 *	  with. (See VXopen).
 *
 * 
 */

void
VXclose( file)
VisXfile_t * file;

{
  char *name;
  char *buf;
  int nread;
  int fd;
  int access;
  VisXelem_t *ptr;

    fd = file->fd;
    access = file->access;
    ptr = VXfind(file->list, VX_FNAME);
    if (ptr != VXNIL)
        name = ptr->data;
    else
	name = NIL(char);

    if(access == WANDSK){
	if((name == (char *)(0)) || (*name == '\0') || (*name == ',')){
	    if(lseek(fd,0L,0)<0){
		(void) fprintf(stderr, "Vclose: lseek error (%s)\n", PERROR);
		exit(1);
	    }
	    buf=Vmalloc(512);
	    while((nread = read(fd,buf,512)) > 0){
		if(Vwrite(1,buf,nread) != nread){
		    (void) fprintf(stderr, "Vclose: write error on pipe (%s)\n",
			PERROR);
		    exit(1);
		}
	    }
	}
	else{
	    (void) close(fd);
	}
    }
    else{
	(void) close(fd);
    }
}

/*
 *    VXfupdate      -- update global constants
 *
 *   copy any new global constants to outputfile
 *   
 */
void VXfupdate(ofile, ifile)
VisXfile_t * ifile;
VisXfile_t * ofile;
{
VisXelem_t * ptr;
VisXelem_t * iptr;
    if (ifile->last != VXNIL && ifile->last->next != VXNIL){
       iptr = ifile->last;
       ptr = VXlast(ofile->list);
       while (iptr != VXNIL){
	   ptr = VXaddelem( ptr, iptr->type, iptr->data, iptr->size);
           iptr = iptr->next;
       }
    }
}

/*
 *
 *   VXpchan -- find the number of channels in an image
 *   this function moved to VisXu.c
 *
 */
int
VXpchan(ptr)
VisXelem_t *ptr;
{
 float *bbx;
 int   numpix;
   /* return zero if not correct */
   /* check if BBX */
   if (ptr == VXNIL ) return 0;
   if (VXtype(ptr) != VX_BBX) ptr = ptr->prev;
   if (ptr == VXNIL || VXtype(ptr) != VX_BBX) return 0; 
   bbx = (float*)VXdata(ptr);
   if(VXNIL == (ptr = ptr->next)) return 0;
   /* OK assume all is well and compute */
     numpix = (((int) bbx[1]) - ((int) bbx[0])) *
                (((int) bbx[3]) - ((int) bbx[2]));
     if (VXtype(ptr) == VX_PBIT)
	   return ((8 * VXnumelem(ptr))/numpix) ;
     else
	   return(VXnumelem(ptr)/numpix);
}

/*
 *    VXhistset -- change the global file history
 */
void
VXhistset(ptr, flag)
VisXelem_t *ptr;
int flag;

{ /* flag ==1 change history to file list */
VisXelem_t *pc;
VisXelem_t *ph;

   if(VisXhist == NIL(char)) Vfree(VisXhist);
   if (VXNIL != (ph = VXfind(ptr, VX_FHIST))){
      VisXhist=Vmalloc(VXsize(ph));
      memcpy(VisXhist, VXdata(ph), VXsize(ph));
   }else{
      VisXhist=Vmalloc(1);
      *VisXhist = '\0';
   }
   if (VXNIL != (pc = VXfind(ptr, VX_FCMND))){
      VisXhist=Vrealloc(VisXhist, strlen(VisXhist) + VXsize(pc));
      strcat(VisXhist, VXdata(pc));
   }

}

/*
 *    VXtextf -- write the header and return file pointer to a text file
 */
FILE *
VXtextf(VSout, tag)
     VisXfile_t *VSout;
     char *tag;
{
  FILE *ofp;
  char *wptr;
  VisXelem_t *vptr;

  /*
    extern char *VisXhist;
  */

  /* Get stream (file) pointer from file descriptor */
  ofp = fdopen(VSout->fd, "w");
  if(!ofp) {
    fprintf(stderr, "VXtextf: ofp = <NULL>\n");
    return(ofp);
  }
  
  /* Write VisX tag */
  if(tag != NULL)
     fprintf(ofp, "#UU.VISX %s\n", tag);
  else
     fprintf(ofp, "#UU.VISX\n");
  
  /* Copy each line, prepending a "#" */
  if(VisXhist != NULL) {
    wptr = strtok(VisXhist, "\n");
    while(wptr) {
      fprintf(ofp, "# %s\n", wptr);
      wptr = strtok(NULL, "\n");
    }
  }

  if ((vptr = VXfind(VSout->list, VX_FCMND)) != VXNIL)
    fprintf(ofp, "# %s\n", VXdata(vptr));
  else
    fprintf(ofp, "\n", wptr);

  return(ofp);
}



/*
 *    VXvreadelem ( fl ) - (virtual) read an element from a file
 */

VisXelem_t *
VXvreadelem( fl)
VisXfile_t *fl;
{
VisXelem_t * item;
int base;

    item = (VisXelem_t*) Vmalloc( sizeof (VisXelem_t));
    if( 0 == Vread(fl->fd,(char *) &item->type, VX_ELEMX))
         return NIL(VisXelem_t);
    if(fl->bswap){
              SWAP4(&item->type);
              SWAP4(&item->size);
    }
    /* get format and length */
    base =  item->type & 0xf;
    if (base != 0xa && base != 0x9){
        item->data = (char*) lseek(fl->fd, 0L, SEEK_CUR);
        if (-1 == lseek(fl->fd, item->size, SEEK_CUR)){
          fprintf(stderr, "VX virtual file read data seek error\n");
          exit(1);
        }
    }
    return item;
}

/*
 *  VXvread -- read an entire data file
 */
/*      includes VXinit                  */

VisXelem_t *
VXvread (VSin)
VisXfile_t *VSin;

{
VisXelem_t * list;
VisXelem_t * last;
VisXelem_t * flast;
VisXelem_t * ptr;
char *ttle, *ohist;
int hlen;
int fflag;
int base;

   if(VSin->mode == 0 ){
       VSin->vlist = NEW(VisXvlist_t);
       list = VXinit();
       last = list;
       if (VSin->list == NIL (VisXelem_t)){
           VSin->list = VXinit();
       }
       flast = VXlast(VSin->list);
       /* first skip over the file info */
            ttle = ohist = "";
       fflag = 1;
       while (fflag && (ptr = VXreadelem(VSin)) != NIL(VisXelem_t)) {
            switch(ptr->type){
            case  VX_FNAME:
            case  VX_FUID:
            case  VX_FMACH:    /* mem leak should plug one day */
            case  VX_FTIME:
                        break;
            case  VX_FCMND:
                        ttle = ptr->data;
                        break;
            case  VX_FHIST:
                        ohist = ptr->data;
                        break;
            default:
                ptr->next =  NIL(VisXelem_t );
                last->next = ptr;
                ptr->prev = last;
		last = ptr;
                /* fix the last read element */
                base =  ptr->type & 0xf;
                if (base != 0xa && base != 0x9){
                    Vfree(ptr->data);
                    ptr->data = ((char *)lseek(VSin->fd, 0L, SEEK_CUR)) -
                                                                   ptr->size;
                }
                fflag = 0;
                break;
            }
            if(fflag) { /* add to file list */
                ptr->next =  NIL(VisXelem_t );
                flast->next = ptr;
                ptr->prev = flast;
                flast = ptr;
            }
       }
       while ((ptr = VXvreadelem(VSin)) != VXNIL) {
            ptr->next =  NIL(VisXelem_t );
            last->next = ptr;
            ptr->prev = last;
	    last = ptr;
       }
       if(VisXhist == NIL(char)){
             hlen = strlen(ttle) + strlen(ohist);
             if(hlen){
                  VisXhist = Vmalloc(hlen + 1);
                  strcpy(VisXhist, ohist);
                  strcat(VisXhist, ttle);
             }
       }
       VSin->vlist->list = list;
       return list;
   }else {
      fprintf(stderr, "VXvread: file mode/format not supported\n");
      exit(1);
   }
}

/*
 *
 *   return the data vector associated with a "virtual" element
 *
 */
char *
VXvdata(ptr, VXin) 
VisXfile_t * VXin;
VisXelem_t * ptr;
{
VisXelem_t  dum;
int fd;
char *data;
    /* should probably check that type is ok?   */
    /* later make compatible with regular lists */
    fd = VXin->fd;
    if( -1 == lseek(fd, (long)VXdata(ptr), SEEK_SET)) {
      fprintf(stderr, "VX file seek problem\n");
      exit(1);
    }
    data = (char*) Vmalloc(ptr->size);
    Vread(fd, data, ptr->size);
    if ( VXin->bswap ) {
       dum.type = ptr->type;
       dum.size = ptr->size;
       dum.data = data;
       VXbsdata( &dum);
    }
    return(data);
}


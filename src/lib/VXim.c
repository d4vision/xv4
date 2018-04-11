/*
 *    standard image file utilities for VisX 4
 *
 *    VXsetimage  -- create an image reference structure
 *    VXmakeimage -- create an image structure
 *    VXembedimage -- create a border image structure
 *    VXfloatimage -- create a border float image structure
 *    VXresetimage -- free memory associated with an image
 *    VXbitimage   -- update a bitplane image structure
 *    VXsbufimage  -- set up a buffer for n image frames
 *    VXrbufimage  -- read the next image frame into the buffer
 *    VXidximage  -- set the image index convention
 *    VXufloatimage  -- copy a float image back to an original image
 *
 *    VXvsetimage   -- set image from a v-list
 *    VXvselectim   -- realocate image from a v-list
 *
 */

/*
 *    VXsetimage  -- create an image reference structure
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
#include "Verror.h"


/*
   VXsetimage locates an image and constructs an image reference structure
   for it.

   1. It first checks to see it the list is at an image data type.
   2. If not it searches FORWARD from the given position to
      locate a numeric image data type (not color index images ).
   3. If no image is found by the end of the list then zero is returned
   4. If an image is found the critical parameters are obtained
      Basic params are bounding box and number of channels
      (if bbx is known we could deduce #chan from size!!!!!)
   5. Find BBX by searching back from image 
      If not found then look for bbx by searching back from end
      of file list. 
   6. if no BBX found then return zero.
   7. Look for PCHAN by searching back from image.
      If not found then look for PCHAN by searching back from end
      of file list.
   8. Compute the reference structure.
   9. If all OK return 1. imitem points to data elem.
			  flist points to entry pt.
			  glist points to any global list

*/

static int VXimidx = 0;


int
VXsetimage(image, flist, finfo)
VisXimage_t *image;
VisXelem_t *flist;
VisXfile_t *finfo;
{
VisXelem_t * item;
char** vec;
int i;
int bsize, rsize;
int numrow, numcol, offset;

    image->list = flist;
    image->glist = VXNIL;
    image->u  = NIL(unsigned char*);
    image->s  = NIL(short*);
    image->f  = NIL(float*);
    image->d  = NIL(double*);
    image->c  = NIL(char*);
    image->b  = NIL(unsigned char*);
    image->i  = NIL(int*);

    if (finfo != NIL(VisXfile_t))
       image->glist = finfo->list;
    item = flist;
    if(item != VXNIL && item->type == VX_PIDX)
	     goto imfound;
    while (item != VXNIL ){
        switch(item->type){
	case VX_PBYTE: 
	case VX_PSHORT: 
	case VX_PFLOAT: 
	case VX_PDOUBLE: 
	case VX_PCHAR: 
	case VX_PBIT: 
	case VX_PINT: 
	     goto imfound;
        }
	item = item->next;
    }
    return 0; /* no image found */
imfound:
    image->imitem = item;
    image->data   = item->data;
    image->type   = item->type;
    image->base   = VXbase(item);

    /* find bounding box */
    item = VXbfind(item, VX_BBX); /* check local first */
    if(item == VXNIL) /*then check most recent global */
        item = VXbfind(VXlast(image->glist), VX_BBX);
    if(item == VXNIL)
	return 0; /* no bounding box found */
    image->bbx[0] = ((float*) item->data)[0];
    image->bbx[1] = ((float*) item->data)[1];
    image->bbx[2] = ((float*) item->data)[2];
    image->bbx[3] = ((float*) item->data)[3];

    if(VXnumelem(item) >= 6 ){
        image->bbx[4] = ((float*) item->data)[4];
        image->bbx[5] = ((float*) item->data)[5];
    }else{
        image->bbx[4] = 0.0;
        image->bbx[5] = 0.0;
    }

       image->chan = VXpchan(image->imitem);

    /* define index range      */
       image->xlo = ((int) image->bbx[0]) * image->chan;
       image->ylo = ((int) image->bbx[2]);
       image->xhi = ((int) image->bbx[1]) * image->chan - 1;
       image->yhi = (int)image->bbx[3] - 1;

        numrow = image->yhi + 1 - image->ylo ;
        numcol = image->xhi + 1 - image->xlo ;

	if (image->type == VX_PBIT)
	    image->data = Vcvtbptoby(image->data, numrow * numcol);

    /* allocate pointer array  */
        vec = (char**) Vmalloc( numrow * sizeof (char*));
	image->indx = vec;
        bsize = VXsize(image->imitem)/VXnumelem(image->imitem);
        rsize = bsize * numcol;
        offset = - image->xlo * bsize;
	if (VXimidx){
            for (i = 0; i < numrow; i++)
	        vec[i] = image->data + offset + i * rsize;
	}else{
            for (i = 0; i < numrow; i++)
	        vec[numrow - 1 - i] = image->data + offset + i * rsize;
	}
	vec -= image->ylo;
    switch(image->imitem->type){
	case VX_PBYTE:
	case VX_PIDX:
	               image->u = (unsigned char**) vec;
		       break;
	case VX_PSHORT: image->s = (short**) vec;
		       break;
	case VX_PFLOAT: image->f = (float**) vec;
		       break;
	case VX_PDOUBLE: image->d = (double**) vec;
		       break;
	case VX_PCHAR: image->c = (char**) vec;
		       break;
	case VX_PBIT: image->b = (unsigned char**) vec;
		       break;
	case VX_PINT: image->i = (int**) vec;
		       break;
    }
    

    return 1;
}


/*
 *    VXmakeimage -- create an image structure
 */
int
VXmakeimage(image, type, bbx, chan)
VisXimage_t *image;
int type;
float *bbx;
int chan;
{
VisXelem_t * item;
char** vec;
int dsize;
int i;
int numrow, numcol, offset;
int bsize, rsize;

    image->list = VXinit();
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char*);
    image->s  = NIL(short*);
    image->f  = NIL(float*);
    image->d  = NIL(double*);
    image->c  = NIL(char*);
    image->b  = NIL(unsigned char*);
    image->i  = NIL(int*);

    image->type = type;
    image->base = type & 0xf;
    image->chan = chan;

    /* make bbx */
    image->bbx[0] = bbx[0];
    image->bbx[1] = bbx[1];
    image->bbx[2] = bbx[2];
    image->bbx[3] = bbx[3];
    /* this might change */
    image->bbx[4] = 0.0;
    image->bbx[5] = 0.0;
    item = image->list;
    item = VXaddelem(item, VX_BBX, (char*)image->bbx, 6 * sizeof(float));

    /* define index range      */
       image->xlo = ((int) image->bbx[0]) * image->chan;
       image->ylo = ((int) image->bbx[2]);
       image->xhi = ((int) image->bbx[1]) * image->chan - 1;
       image->yhi = ((int) image->bbx[3]) - 1;

    /* make data entry */
        numrow = image->yhi + 1 - image->ylo ;
        numcol = image->xhi + 1 - image->xlo ;
	dsize = numcol * numrow * VisXbase[image->base];
        image->data = Vcalloc (dsize,1); /* cheap and dirty zero */
	if (image->base != VX_BIT){
           item = VXlnkelem(item, type, image->data, dsize);
	}else{
	   dsize = (dsize + 7)/8;
           item = VXlnkelem(item, type, Vcalloc (dsize,1), dsize);
	}

    /* allocate pointer array  */
        vec = (char**) Vmalloc( numrow * sizeof (char*));
	image->indx = vec;
        bsize = VXsize(item)/VXnumelem(item);
        rsize = bsize * numcol;
        offset = - image->xlo * bsize;
	if (VXimidx){
            for (i = 0; i < numrow; i++)
	        vec[i] = image->data + offset + i * rsize;
	}else{
            for (i = 0; i < numrow; i++)
	        vec[numrow - 1 - i] = image->data + offset + i * rsize;
	}
	vec -= image->ylo;
    switch(image->type){
	case VX_PIDX:
	case VX_PBYTE: image->u = (unsigned char**) vec;
		       break;
	case VX_PSHORT: image->s = (short**) vec;
		       break;
	case VX_PFLOAT: image->f = (float**) vec;
		       break;
	case VX_PDOUBLE: image->d = (double**) vec;
		       break;
	case VX_PCHAR: image->c = (char**) vec;
		       break;
	case VX_PBIT: image->b = (unsigned char**) vec;
		       break;
	case VX_PINT: image->i = (int**) vec;
		       break;
    }
    
    return 1;
}


/*
 *    VXembedimage -- create a border image structure
 */
int
VXembedimage( image, imagep, x1, x2, y1, y2)
VisXimage_t *image;
VisXimage_t *imagep;
int x1, x2, y1, y2;
{
VisXelem_t * item;
char** vec;
int dsize;
int i,j;
int bsize, rsize;
int numrow, numcol, offset;
int type;

    image->list = VXinit();
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char*);
    image->s  = NIL(short*);
    image->f  = NIL(float*);
    image->d  = NIL(double*);
    image->c  = NIL(char*);
    image->b  = NIL(unsigned char*);
    image->i  = NIL(int*);

    image->chan = imagep->chan;
    image->type = imagep->type;
    image->base = imagep->base;
    type = image->type;

    /* make bbx */
    item = image->list;
    image->bbx[0] = imagep->bbx[0] - x1;
    image->bbx[1] = imagep->bbx[1] + x2;
    image->bbx[2] = imagep->bbx[2] - y1;
    image->bbx[3] = imagep->bbx[3] + y2;
    image->bbx[4] = imagep->bbx[4];
    image->bbx[5] = imagep->bbx[5];

    item = VXaddelem(item, VX_BBX, (char*)image->bbx, 6 * sizeof(float));
    /* make chan entry */
    /*
    if (image->chan != 1)
        item = VXaddelem(item, VX_PCHAN, NIL(char), image->chan);
    */

    /* define index range      */
       image->xlo = ((int) image->bbx[0]) * image->chan;
       image->ylo = ((int) image->bbx[2]);
       image->xhi = ((int) image->bbx[1]) * image->chan - 1;
       image->yhi = (int)image->bbx[3] - 1;
    /* make data entry */
        numrow = image->yhi + 1 - image->ylo ;
        numcol = image->xhi + 1 - image->xlo ;
	dsize = numcol * numrow * VisXbase[image->base];
        image->data = Vcalloc (dsize,1); /* cheap and dirty zero */
	if (image->base != VX_BIT){
           item = VXlnkelem(item, type, image->data, dsize);
	}else{
	   dsize = (dsize + 7)/8;
           item = VXlnkelem(item, type, Vcalloc (dsize,1), dsize);
	}

    /* allocate pointer array  */
        vec = (char**) Vmalloc( numrow * sizeof (char*));
	image->indx = vec;
        bsize = VXsize(item)/VXnumelem(item);
        rsize = bsize * numcol;
        offset = - image->xlo * bsize;
	if (VXimidx){
            for (i = 0; i < numrow; i++)
	        vec[i] = image->data + offset + i * rsize;
	}else{
            for (i = 0; i < numrow; i++)
	        vec[numrow - 1 - i] = image->data + offset + i * rsize;
	}
	vec -= image->ylo;
    switch(image->type){
	case VX_PIDX:
	case VX_PBYTE: image->u = (unsigned char**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->u[i][j] = imagep->u[i][j];
		       break;
	case VX_PSHORT: image->s = (short**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->s[i][j] = imagep->s[i][j];
		       break;
	case VX_PFLOAT: image->f = (float**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->f[i][j];
		       break;
	case VX_PDOUBLE: image->d = (double**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->d[i][j] = imagep->d[i][j];
		       break;
	case VX_PCHAR: image->c = (char**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->c[i][j] = imagep->c[i][j];
		       break;
	case VX_PBIT: image->b = (unsigned char**) vec;
		  /* this probably needs to be fixed */
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->b[i][j] = imagep->b[i][j];
		       break;
	case VX_PINT: image->i = (int**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->i[i][j] = imagep->i[i][j];
		       break;
    }
    
    return 1;
}


/*
 *    VXfloatimage -- create a float image structure
 */
int
VXfloatimage( image, imagep, x1, x2, y1, y2)
VisXimage_t *image;
VisXimage_t *imagep;
int x1, x2, y1, y2;
{
VisXelem_t * item;
char** vec;
int dsize;
int i,j;
int bsize, rsize;
int numrow, numcol, offset;
int type;

    image->list = VXinit();
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char*);
    image->s  = NIL(short*);
    image->f  = NIL(float*);
    image->d  = NIL(double*);
    image->c  = NIL(char*);
    image->b  = NIL(unsigned char*);
    image->i  = NIL(int*);

    image->chan = imagep->chan;
    image->type = VX_PFLOAT;
    image->base = VX_FLOAT;
    type = image->type;

    /* make bbx */
    item = image->list;
    image->bbx[0] = imagep->bbx[0] - x1;
    image->bbx[1] = imagep->bbx[1] + x2;
    image->bbx[2] = imagep->bbx[2] - y1;
    image->bbx[3] = imagep->bbx[3] + y2;
    image->bbx[4] = imagep->bbx[4];
    image->bbx[5] = imagep->bbx[5];

    item = VXaddelem(item, VX_BBX, (char*)image->bbx, 6 * sizeof(float));

    /* define index range      */
       image->xlo = ((int) image->bbx[0]) * image->chan;
       image->ylo = ((int) image->bbx[2]);
       image->xhi = ((int) image->bbx[1]) * image->chan - 1;
       image->yhi = (int)image->bbx[3] - 1;
    /* make data entry */
        numrow = image->yhi + 1 - image->ylo ;
        numcol = image->xhi + 1 - image->xlo ;
	dsize = numcol * numrow * VisXbase[image->base];
        image->data = Vcalloc (dsize,1); /* cheap and dirty zero */
        item = VXlnkelem(item, type, image->data, dsize);

    /* allocate pointer array  */
        vec = (char**) Vmalloc( numrow * sizeof (char*));
	image->indx = vec;
        bsize = VXsize(item)/VXnumelem(item);
        rsize = bsize * numcol;
        offset = - image->xlo * bsize;
	if (VXimidx){
            for (i = 0; i < numrow; i++)
	        vec[i] = image->data + offset + i * rsize;
	}else{
            for (i = 0; i < numrow; i++)
	        vec[numrow - 1 - i] = image->data + offset + i * rsize;
	}
	vec -= image->ylo;
	image->f = (float**) vec;
    switch(imagep->type){
	case VX_PIDX:
	case VX_PBYTE: image->u = (unsigned char**) vec;
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->u[i][j];
		       break;
	case VX_PSHORT:
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->s[i][j];
		       break;
	case VX_PFLOAT:
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->f[i][j];
		       break;
	case VX_PDOUBLE:
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->d[i][j];
		       break;
	case VX_PCHAR:
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->c[i][j];
		       break;
	case VX_PBIT:
		  /* this probably needs to be fixed */
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->b[i][j] >>7;
		       break;
	case VX_PINT:
	               for (i = imagep->ylo; i <= imagep->yhi; i++)
		          for (j = imagep->xlo; j <= imagep->xhi; j++)
			     image->f[i][j] = imagep->i[i][j];
		       break;
    }
    
    return 1;
}


/*
 *    VXresetimage -- free memory associated with an image
 */
void
VXresetimage( image )
VisXimage_t *image;
{
   if ( image->imitem == VXNIL){
      /* temp image so delete data list */
      VXdellist(image->list);
   }
   /* delete any index array */

   if( image->indx  != NIL(char*)) Vfree(image->indx);
   if( image->type == VX_BIT && image->data  != NIL(char)) {
	Vfree(image->data);
   }
}


/*
 *    VXbitimage   -- update a bitplane image structure
 */
void
VXbitimage( image )
VisXimage_t *image;
{
VisXelem_t *ptr;
   if ( image->base == VX_BIT){
      ptr = VXfind(image->list, VX_PBIT);
      Vfree(ptr->data);
      ptr->data = Vcvtbytobp(image->data,
	(1 + image->yhi - image->ylo) * (1 + image->xhi - image->xlo));
   }
}


static int VX_ptypelist[] = {VX_PBYTE, VX_PSHORT, VX_PFLOAT, VX_PDOUBLE,
			    VX_PCHAR, VX_PBIT, VX_PINT, VX_PIDX, VX_NULLT};
/*
 *    VXsbufimage  -- set up a buffer for n image frames
 *
 */
VisXimage_t *
VXsbufimage( finfo, n)
VisXfile_t *finfo;
int n;
{
VisXimage_t *setl;
VisXelem_t * list;
int i;

   setl = (VisXimage_t *) Vmalloc(n * sizeof(VisXimage_t));
    if ( n > 1 ){
       if((list = VXreadframe(finfo)) == VXNIL){
            fprintf(stderr,"Not enough images in image set input\n");
            exit(1);
	}
       for (i=0; i<( n-1); i++){
	 if (list == VXNIL || VXNIL == (list = VXfindin(list, VX_ptypelist))){
           if((list = VXreadframe(finfo)) == VXNIL ||
	       VXNIL == (list = VXfindin(list, VX_ptypelist))){
            fprintf(stderr,"Not enough images in image set input\n");
            exit(1);
           }
	 }
	 VXsetimage(&setl[n-i-2], list, finfo);
	 list = list->next;
       }
   }
   setl[n-1].list = VXNIL;
   return (setl);
}
VisXimage_t *
VXsbufimageX( finfo, n)
VisXfile_t *finfo;
int n;
{
VisXimage_t *setl;
VisXelem_t * list;
int i;

   setl = (VisXimage_t *) Vmalloc(n * sizeof(VisXimage_t));
   for (i=0; i<( n-1); i++){
       if((list = VXreadframe(finfo)) == VXNIL){
            fprintf(stderr,"Not enough images in image set input\n");
            exit(1);
       }
       if(!VXsetimage(&setl[n-i-2], list, finfo)){
            fprintf(stderr,"No image found in frame \n");
            exit(1);
       }
   }
   setl[n-1].list = VXNIL;
   return (setl);
}


/*
 *    VXrbufimage  -- read the next image frame into the buffer
 *
 */
int
VXrbufimageX(setl, finfo, n)
VisXimage_t *setl;
VisXfile_t *finfo;
int n;
{
VisXelem_t * list;
int i;
   if (setl[n-1].list != VXNIL){
      setl[n-1].imitem = VXNIL; /* fool resetimage */
      VXresetimage(&setl[n-1]);
   }
   for (i=n-1; i>0;i--){
      setl[i] = setl[i-1];
   }

   if((list = VXreadframe(finfo)) == VXNIL){
      return (0);
   }
   if(!VXsetimage(&setl[0], list, finfo)){
      fprintf(stderr,"No image found in frame \n");
      exit(1);
   }
   return(1);
}
int
VXrbufimage(setl, finfo, n)
VisXimage_t *setl;
VisXfile_t *finfo;
int n;
{
VisXelem_t * list;
VisXelem_t * xlist;
VisXelem_t * xpt;
int i;
   xpt = xlist = setl[n-1].list;
   if (setl[n-1].list != VXNIL){
     VXresetimage(&setl[n-1]);
     xpt = xlist->next;
     /* do we delete this list? */
     if (VXNIL == VXfindin (xpt,VX_ptypelist)) {
       VXdellist(VXfirst(xlist)->prev); /* whole list + dum */  
     }
   }

   list = setl[0].list;
   if(VXNIL != list) list = list->next;
   for (i=n-1; i>0;i--){
      setl[i] = setl[i-1];
   }
   if (VXNIL != (xpt = VXfindin (list,VX_ptypelist))){
      VXsetimage(&setl[0], xpt, finfo);
      return(1);
   }

   if((list = VXreadframe(finfo)) == VXNIL){
      return (0);
   }
   if (VXNIL != (xpt = VXfindin (list,VX_ptypelist))){
      VXsetimage(&setl[0], xpt, finfo);
      return(1);
   }else{
      fprintf(stderr,"No image found in frame \n");
      exit(1);
   }
   return(1);
}

/*
 *    VXidximage  -- set the image index convention
 *                   return the old setting for restoring, if needed
 *
 */
int
VXidximage(idx)
int idx;
{
int old;
   old = VXimidx;
   VXimidx = idx;
   return old;
}


/*
 *
 *   VXpzval -- find the the z value of an image
 *
 */
float
VXpzval(ptr)
VisXelem_t *ptr;
{
 float *bbx;

   /* return MAXFLOAT if not correct */
   /* check if BBX */
   if (ptr == VXNIL ) return MAXFLOAT;
   if (VXtype(ptr) != VX_BBX) ptr = ptr->prev;
   if (ptr == VXNIL || VXtype(ptr) != VX_BBX) return MAXFLOAT; 
   bbx = (float*)VXdata(ptr);
   /* OK assume all is well and compute */
   if (VXnumelem(ptr) < 6) return MAXFLOAT;
   if (bbx[4] == bbx[5])  return MAXFLOAT;
   return bbx[4];
}


/*
 *
 *   VXpzset -- set the z value in an image structure
 *
 */
void
VXpzset(ptr, val)
VisXelem_t *ptr;
float val;
{
 float *bbx, *bbx2;
 VisXelem_t *vptr;
   /* return if not correct */
   /* check if BBX */
   if (ptr == VXNIL ) return ;
   if (VXtype(ptr) != VX_BBX) ptr = ptr->prev;
   if (ptr == VXNIL || VXtype(ptr) != VX_BBX) return ; 
   bbx = (float*)VXdata(ptr);
   if(VXnumelem(ptr) < 6 ){ /* make it 6 elements */
     vptr = VXaddelem(ptr,VX_BBX,Vmalloc(6 * sizeof(float)), 6 * sizeof(float));
     bbx = (float*)VXdata(ptr);
     bbx2 = (float*)VXdata(vptr);
     bbx2[0] = bbx[0];
     bbx2[1] = bbx[1];
     bbx2[2] = bbx[2];
     bbx2[3] = bbx[3];
     ptr = VXdelelem(ptr);
   }
   bbx = (float*)VXdata(ptr);
   bbx[4] = val;
   bbx[5] = val + 1;
   return;

}


/*
 *    VXufloatimage  -- copy a float image back to an original image
 *
 */
void VXufloatimage(im, fm) /* copy a float image to an original image */
VisXimage_t *im, *fm;   
{
int i,j;
     switch(im->base){
         /* in the future could write this faster as a single loop ?  */
	 /* if dimensions of both images are the same                 */
       case VX_BYTE:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->u[i][j] = fm->f[i][j];
         break;
       case VX_FLOAT:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->f[i][j] = fm->f[i][j];
         break;
       case VX_DOUBLE:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->d[i][j] = fm->f[i][j];
         break;
       case VX_CHAR:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->c[i][j] = fm->f[i][j];
         break;
       case VX_SHORT:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->s[i][j] = fm->f[i][j];
         break;
       case VX_INT:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->i[i][j] = fm->f[i][j];
         break;
       case VX_BIT:
         for (i = im->ylo ; i <= im->yhi ; i++) /* copy data */
          for (j = im->xlo; j <= im->xhi; j++)
	    im->b[i][j] = fm->f[i][j];
	 VXbitimage(im);
         break;
       default:
	 fprintf(stderr, "Vunfloat(): format conversion not supported\n");
	 exit(1);
     }
}


/* 
 *
 *    VXvsetimage -- virtual vsetimage
 *
 */
int
VXvsetimage(image, flist, finfo)
VisXimage_t *image;
VisXelem_t *flist;
VisXfile_t *finfo;
{
VisXelem_t * item;
char** vec;
int i;
int bsize, rsize;
int numrow, numcol, offset;
int numpix;
float *bbx;
char *data;

    image->list = flist;
    image->glist = VXNIL;
    image->u  = NIL(unsigned char*);
    image->s  = NIL(short*);
    image->f  = NIL(float*);
    image->d  = NIL(double*);
    image->c  = NIL(char*);
    image->b  = NIL(unsigned char*);
    image->i  = NIL(int*);

    if (finfo != NIL(VisXfile_t))
       image->glist = finfo->list;
    item = flist;
    if(item != VXNIL && item->type == VX_BBX)
	     item = item->next;
    if (item != VXNIL ){
        switch(item->type){
	case VX_PBYTE: 
	case VX_PSHORT: 
	case VX_PFLOAT: 
	case VX_PDOUBLE: 
	case VX_PCHAR: 
	case VX_PBIT: 
	case VX_PINT: 
	case VX_PIDX:
	     goto imfound;
        }
	return 0;
    }
    return 0; /* no image found */
imfound:
    image->imitem = item;
    image->data   = VXvdata(item , finfo);
    image->type   = item->type;
    image->base   = VXbase(item);

    /* find bounding box */
    item = VXbfind(item, VX_BBX); /* check local first */
    if(item == VXNIL) /*then check most recent global */
        item = VXbfind(VXlast(image->glist), VX_BBX);
    if(item == VXNIL)
	return 0; /* no bounding box found */

    bbx = (float*) VXvdata(item,finfo);
    image->bbx[0] = bbx[0];
    image->bbx[1] = bbx[1];
    image->bbx[2] = bbx[2];
    image->bbx[3] = bbx[3];

    if(VXnumelem(item) >= 6 ){
        image->bbx[4] = bbx[4];
        image->bbx[5] = bbx[5];
    }else{
        image->bbx[4] = 0.0;
        image->bbx[5] = 0.0;
    }

    /* image->chan = VXpchan(image->imitem); */
    numpix = (((int) bbx[1]) - ((int) bbx[0])) *
		     (((int) bbx[3]) - ((int) bbx[2]));
    Vfree(bbx);

    if (VXtype(image->imitem) == VX_PBIT)
	  image->chan =  (8 * VXnumelem(image->imitem)/numpix) ;
    else
	  image->chan = VXnumelem(image->imitem)/numpix;


    /* define index range      */
       image->xlo = ((int) image->bbx[0]) * image->chan;
       image->ylo = ((int) image->bbx[2]);
       image->xhi = ((int) image->bbx[1]) * image->chan - 1;
       image->yhi = (int)image->bbx[3] - 1;

        numrow = image->yhi + 1 - image->ylo ;
        numcol = image->xhi + 1 - image->xlo ;


	if (image->type == VX_PBIT) {
	    data = image->data;
	    image->data = Vcvtbptoby(image->data, numrow * numcol);
	    Vfree(data);
	}

    /* allocate pointer array  */
        vec = (char**) Vmalloc( numrow * sizeof (char*));
	image->indx = vec;
        bsize = VXsize(image->imitem)/VXnumelem(image->imitem);
        rsize = bsize * numcol;
        offset = - image->xlo * bsize;
	if (VXimidx){
            for (i = 0; i < numrow; i++)
	        vec[i] = image->data + offset + i * rsize;
	}else{
            for (i = 0; i < numrow; i++)
	        vec[numrow - 1 - i] = image->data + offset + i * rsize;
	}
	vec -= image->ylo;
    switch(image->imitem->type){
	case VX_PBYTE:
	case VX_PIDX:
	               image->u = (unsigned char**) vec;
		       break;
	case VX_PSHORT: image->s = (short**) vec;
		       break;
	case VX_PFLOAT: image->f = (float**) vec;
		       break;
	case VX_PDOUBLE: image->d = (double**) vec;
		       break;
	case VX_PCHAR: image->c = (char**) vec;
		       break;
	case VX_PBIT: image->b = (unsigned char**) vec;
		       break;
	case VX_PINT: image->i = (int**) vec;
		       break;
    }
    

    return 1;
}


/*
 *    VXvselectim -- currently only for homogeneous virtual files
 *                       not yet defined for bit data 
 */
void
VXvselectim ( image, idx, VXin  )
VisXfile_t *VXin;
VisXimage_t *image;
int idx;
{
VisXelem_t *ptr;
VisXelem_t *nptr;
VisXelem_t dum;
int i;
        nptr = VXin->vlist->list;
        for (i = 1; i <= idx; i++){
	  if(VXNIL == (ptr = VXfind(nptr,image->type))){
	    fprintf(stderr, "virtual list runoff\n");
	    return;
          }
	  nptr = ptr->next;
	}
	lseek(VXin->fd, (long)VXdata(ptr), SEEK_SET); 
	Vread(VXin->fd, image->data, VXsize(ptr));
	if (VXin->bswap) {
	   dum.type = ptr->type;
	   dum.size = ptr->size;
	   dum.data = image->data;
	   VXbsdata(&dum);
	}
}


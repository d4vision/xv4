/*
 *    standard 3dim file utilities for xv4Package
 *
 *    VXset3dim  -- create a 3dim reference structure from a list
 *    VXmake3dim -- create a 3dim structure
 *    VXembed3dim -- create a border 3dim structure
 *    VXfloat3dim -- create a border float 3dim structure
 *    VXreset3dim -- free memory associated with an 3dim
 *    VXbit3dim   -- update a bitplane 3dim structure
 *    VXufloat3dim  -- copy a float 3dim back to an original 3dim
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



/*
 *    VXset3dim -- create a 3dim reference structure from a list
 */
int
VXset3dim(image, flist, finfo)
VisX3dim_t *image;
VisXelem_t *flist;
VisXfile_t *finfo;
{
VisXelem_t *item, *bitem;
char*** vec;
int i;
int n;
int zslice;

    image->list = flist;
    image->glist = VXNIL;
    image->u  = NIL(unsigned char**);
    image->s  = NIL(short**);
    image->f  = NIL(float**);
    image->d  = NIL(double**);
    image->c  = NIL(char**);
    image->b  = NIL(unsigned char**);
    image->i  = NIL(int**);

    /* find the first image */
    if (finfo != NIL(VisXfile_t))
       image->glist = finfo->list;
    item = flist;
    while (item != VXNIL ){
        switch(item->type){
	case VX_PIDX: 
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
    image->type   = item->type;
    image->base   = VXbase(item);

    /* now find how many images of this type */
    n=0;
    while (VXNIL != (item = VXfind(item, image->type))){
      n++;
      item = item->next;
    }

    image->image = (VisXimage_t *) Vmalloc (n * sizeof(VisXimage_t));
    image->indx = (char *** ) Vmalloc (n * sizeof(char **));
    vec = image->indx;
    
    item = image->imitem;

    /* create the image structures */
    zslice = 0;
    item = image->imitem;
    for (i = 0; i < n; i++){
      item = VXfind(item, image->type);
      if (i == 0)
	bitem = VXbfind(item, VX_BBX);
      if(!VXsetimage(&(image->image[i]), item, finfo)){ 
	fprintf(stderr,"VXsetimage problem\n");
	exit(1);
      }
      item = item->next;
    }

    if((VXnumelem(bitem) >= 6) &&
       (fabs(image->image[0].bbx[5] - image->image[0].bbx[4]) == 1.0))
      zslice = 1;
    /*
      fprintf(stderr, "VXset3dim: zslice = %d\n", zslice);
    */

    /* set the overall bounding box */
    for (i = 0; i < 4; i++)
      image->bbx[i] = image->image[0].bbx[i];
    if(zslice) {
      image->bbx[4] = image->image[0].bbx[4];
      image->bbx[5] = image->image[n-1].bbx[5];
    }
    else {
      image->bbx[4] = 0.0;
      image->bbx[5] = (float) n;
    }

    /* set the index values */
    image->xlo = image->image[0].xlo;
    image->xhi = image->image[0].xhi;
    image->ylo = image->image[0].ylo;
    image->yhi = image->image[0].yhi;
    if(zslice) {
      image->zlo = (int) image->image[0].bbx[4];
      image->zhi = (int) image->image[n-1].bbx[5] - 1;
    }
    else {
      image->zlo = 0;
      image->zhi = n-1;
    }
      
    image->chan = image->image[0].chan;

    vec -= image->zlo;

    switch(image->type){
        case VX_PBYTE:
        case VX_PIDX:
	  image->u = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].u);
	  }
	  break;
        case VX_PSHORT:
	  image->s = (short***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].s);
	  }
	  break;
        case VX_PFLOAT:
	  image->f = (float***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].f);
	  }
	  break;
        case VX_PDOUBLE:
	  image->d = (double***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].d);
	  }
	  break;
        case VX_PCHAR:
	  image->c = (char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].c);
	  }
	  break;
        case VX_PBIT:
	  image->b = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].b);
	  }
	  break;
        case VX_PINT:
	  image->i = (int***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].i);
	  }
	  break;
    }
    
    return 1;
}


/*
 *    VXmake3dim -- create an 3dim structure
 *
 * if no zrange, then an effective 2d image is created
 */
int
VXmake3dim(image, type, bbx, chan)
VisX3dim_t *image;
int type;
float bbx[6];
int chan;
{
VisXelem_t *vptr;
char*** vec;
int i;
int zlo, zhi;
int czlo;
int n;
float *tbbx;
 
    image->list = VXNIL;
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char**);
    image->s  = NIL(short**);
    image->f  = NIL(float**);
    image->d  = NIL(double**);
    image->c  = NIL(char**);
    image->b  = NIL(unsigned char**);
    image->i  = NIL(int**);

    image->type = type;
    image->base = type & 0xf;
    image->chan = chan;

    zlo = (int) bbx[4]; 
    zhi = (int) bbx[5] - 1; 
    n = 1 + zhi - zlo;
    if ( n == 0 ) { /* a 2d image */
       n++;
       zhi++;
    }

   /* allocate dynamic structures */
   image->image = (VisXimage_t *) Vmalloc (n * sizeof(VisXimage_t ));
   image->indx = (char *** ) Vmalloc (n * sizeof(char **));
   vec = image->indx;

   /* allocate the images */
   image->list = VXinit();
   czlo = zlo;
   for (i = 0 ; i < n; i++){
     VXmakeimage(&image->image[i], type, bbx, chan);
     vptr = VXfind(image->image[i].list, VX_BBX);
     
     /* fix the z bounds for each slice */
     tbbx = (float *) VXdata(vptr);
     tbbx[4] = czlo;
     if ( bbx[4] == bbx[5] ) /* just a 2d image */ 
       tbbx[5] = czlo;
     else
       tbbx[5] = czlo + 1;
     image->image[i].bbx[4] = czlo;
     image->image[i].bbx[5] = czlo + 1;
     VXlnklist(VXlast(image->list), image->image[i].list);
     czlo += 1;
   }

   /* set the overall bounding box */
   for (i = 0 ; i < 6; i++){
     image->bbx[i] = bbx[i];
   }

   /* set the index values */
   image->xlo = image->image[0].xlo;
   image->xhi = image->image[0].xhi;
   image->ylo = image->image[0].ylo;
   image->yhi = image->image[0].yhi;
   image->zlo = (int) bbx[4];
   if ( bbx[4] == bbx[5] ) /* just a 2d image */ 
     image->zhi = (int) bbx[5];
   else
     image->zhi = (int) bbx[5] - 1;
   image->chan = image->image[0].chan;

   vec -= image->zlo;
   
   switch(type){
        case VX_PBYTE:
        case VX_PIDX:
	  image->u = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].u);
	  }
	  break;
        case VX_PSHORT:
	  image->s = (short***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].s);
	  }
	  break;
        case VX_PFLOAT:
	  image->f = (float***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].f);
	  }
	  break;
        case VX_PDOUBLE:
	  image->d = (double***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].d);
	  }
	  break;
        case VX_PCHAR:
	  image->c = (char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].c);
	  }
	  break;
        case VX_PBIT:
	  image->b = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].b);
	  }
	  break;
        case VX_PINT:
	  image->i = (int***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].i);
	  }
	  break;
   }

   return 1;
}

/*
 *    VXembed3dim -- create a border 3dim structure
 */
int
VXembed3dim(image, imagep, x1, x2, y1, y2, z1, z2)
VisX3dim_t *image;
VisX3dim_t *imagep;
int x1, x2, y1, y2, z1, z2;
{
VisXelem_t *item, *bitem;
char ***vec;
int i;
int type;
int oldn, n, n2;
int czlo = 0;
int zslice;
float *sbbx;
 
#ifdef P_DEBUG
    fprintf(stderr, "VXembed3dim: x1=%d, x2=%d, y1=%d, y2=%d, z1=%d, z2=%d\n",
	 x1, x2, y1, y2, z1, z2);
#endif
 
    image->list = VXinit();
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char**);
    image->s  = NIL(short**);
    image->f  = NIL(float**);
    image->d  = NIL(double**);
    image->c  = NIL(char**);
    image->b  = NIL(unsigned char**);
    image->i  = NIL(int**);

    image->chan = imagep->chan;
    image->type = imagep->type;
    image->base = imagep->base;
    type = image->type;

    if((fabs(imagep->image[0].bbx[5] - imagep->image[0].bbx[4]) == 1.0))
      zslice = 1;
    else
      zslice = 0;

#ifdef P_DEBUG
    fprintf(stderr, "zslice = %d\n", zslice);
#endif
    
    /* make bbx */
    item = image->list;
    image->bbx[0] = imagep->bbx[0] - x1;
    image->bbx[1] = imagep->bbx[1] + x2;
    image->bbx[2] = imagep->bbx[2] - y1;
    image->bbx[3] = imagep->bbx[3] + y2;
    image->bbx[4] = imagep->bbx[4] - z1;
    image->bbx[5] = imagep->bbx[5] + z2;

    /* set the index values */
    image->xlo = imagep->xlo - x1;
    image->xhi = imagep->xhi + x2;
    image->ylo = imagep->ylo - y1;
    image->yhi = imagep->yhi + y2;
    image->zlo = imagep->zlo - z1;
    image->zhi = imagep->zhi + z2;

    oldn = 1 + imagep->zhi - imagep->zlo;
    n = 1 + image->zhi - image->zlo;

    /* Fix z bbx, if necessary */
    if(zslice) {
      image->bbx[4] = imagep->image[0].bbx[4] - z1;
      image->bbx[5] = imagep->image[oldn-1].bbx[5] + z2;
      czlo = image->zlo;
    }
    
    /* allocate dynamic structures */
    image->image = (VisXimage_t *) Vmalloc (n * sizeof(VisXimage_t));
    image->indx = (char ***) Vmalloc (n * sizeof(char **));
    vec = image->indx;

    /* allocate the images */
    /* create first pad images */
    for (i = 0; i < z1; i++){
      VXmakeimage(&image->image[i], type, image->bbx, image->chan);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }
#ifdef P_DEBUG
      fprintf(stderr, "VXembed3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif

      VXlnklist(VXlast(image->list), image->image[i].list);
    }

    /* copy the main images */
    n2 = z1 + 1 + imagep->zhi - imagep->zlo;
    for (i = z1; i < n2; i++){
      VXembedimage(&image->image[i], &imagep->image[i - z1], x1, x2, y1, y2);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }

#ifdef P_DEBUG
      fprintf(stderr, "VXembed3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif  
      VXlnklist(VXlast(image->list), image->image[i].list);
    }

    /* create end pad images */
    for (i = n2; i < n; i++){
      VXmakeimage(&image->image[i], type, image->bbx, image->chan);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }
#ifdef P_DEBUG
      fprintf(stderr, "VXembed3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif  
      VXlnklist(VXlast(image->list), image->image[i].list);
    }

    vec -= image->zlo;

    switch(type){
        case VX_PBYTE:
        case VX_PIDX:
	  image->u = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].u);
	  }
	  break;
        case VX_PSHORT:
	  image->s = (short***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].s);
	  }
	  break;
        case VX_PFLOAT:
	  image->f = (float***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].f);
	  }
	  break;
        case VX_PDOUBLE:
	  image->d = (double***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].d);
	  }
	  break;
        case VX_PCHAR:
	  image->c = (char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].c);
	  }
	  break;
        case VX_PBIT:
	  image->b = (unsigned char***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].b);
	  }
	  break;
        case VX_PINT:
	  image->i = (int***) vec;
	  for(i=0; i<n; i++){
	    vec[i+image->zlo] = (char**)(image->image[i].i);
	  }
	  break;
    }

    return 1;
}

/*
 *    VXfloat3dim -- create a float 3dim structure
 */
int
VXfloat3dim(image, imagep, x1, x2, y1, y2, z1, z2)
VisX3dim_t *image;
VisX3dim_t *imagep;
int x1, x2, y1, y2, z1, z2;
{
VisXelem_t *item, *bitem;
char ***vec;
int i;
int type;
int oldn, n, n2;
int czlo = 0;
int zslice;
float *sbbx;
 
#ifdef P_DEBUG
    fprintf(stderr, "VXfloat3dim: x1=%d, x2=%d, y1=%d, y2=%d, z1=%d, z2=%d\n",
	 x1, x2, y1, y2, z1, z2);
#endif

    image->list = VXinit();
    image->glist = VXNIL;
    image->imitem = VXNIL;
    image->u  = NIL(unsigned char**);
    image->s  = NIL(short**);
    image->f  = NIL(float**);
    image->d  = NIL(double**);
    image->c  = NIL(char**);
    image->b  = NIL(unsigned char**);
    image->i  = NIL(int**);

    image->chan = imagep->chan;
    image->type = VX_PFLOAT;
    image->base = VX_FLOAT;
    type = image->type;

    if((fabs(imagep->image[0].bbx[5] - imagep->image[0].bbx[4]) == 1.0))
      zslice = 1;
    else
      zslice = 0;

#ifdef P_DEBUG
    fprintf(stderr, "zslice = %d\n", zslice);
#endif
    
    /* make bbx */
    item = image->list;
    image->bbx[0] = imagep->bbx[0] - x1;
    image->bbx[1] = imagep->bbx[1] + x2;
    image->bbx[2] = imagep->bbx[2] - y1;
    image->bbx[3] = imagep->bbx[3] + y2;
    image->bbx[4] = imagep->bbx[4] - z1;
    image->bbx[5] = imagep->bbx[5] + z2;


    /* set the index values */
    image->xlo = imagep->xlo - x1;
    image->xhi = imagep->xhi + x2;
    image->ylo = imagep->ylo - y1;
    image->yhi = imagep->yhi + y2;
    image->zlo = imagep->zlo - z1;
    image->zhi = imagep->zhi + z2;

    oldn = 1 + imagep->zhi - imagep->zlo;
    n = 1 + image->zhi - image->zlo;

    /* Fix z bbx, if necessary */
    if(zslice) {
      image->bbx[4] = imagep->image[0].bbx[4] - z1;
      image->bbx[5] = imagep->image[oldn-1].bbx[5] + z2;
      czlo = image->zlo;
    }
    
    /* allocate dynamic structures */
    image->image = (VisXimage_t *) Vmalloc (n * sizeof(VisXimage_t ));
    image->indx = (char *** ) Vmalloc (n * sizeof(char **));
    vec = image->indx;

    /* allocate the images */
    /* create first pad images */
    for (i = 0; i < z1; i++){
      VXmakeimage(&image->image[i], type, image->bbx, image->chan);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }
#ifdef P_DEBUG
      fprintf(stderr, "VXfloat3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif

      VXlnklist(VXlast(image->list), image->image[i].list);
    }

    /* copy the main images */
    n2 = z1 + 1 + imagep->zhi - imagep->zlo;
    for (i = z1; i < n2; i++){
      VXfloatimage(&image->image[i], &imagep->image[i - z1], x1, x2, y1, y2);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }

#ifdef P_DEBUG
      fprintf(stderr, "VXfloat3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif  
      VXlnklist(VXlast(image->list), image->image[i].list);
    }

    /* create end pad images */
    for (i = n2; i < n; i++){
      VXmakeimage(&image->image[i], type, image->bbx, image->chan);
      if(zslice){
	image->image[i].bbx[4] = czlo + i;
	image->image[i].bbx[5] = czlo + i + 1;
	bitem = VXfind(image->image[i].list, VX_BBX);
	sbbx = (float *) VXdata(bitem);
	sbbx[4] = czlo + i;
	sbbx[5] = czlo + i + 1;
      }
#ifdef P_DEBUG
      fprintf(stderr, "VXembed3dim: image->image[%d] = "
	      "%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\t%6.4g\n", i,
	      image->image[i].bbx[0], image->image[i].bbx[1], image->image[i].bbx[2],
	      image->image[i].bbx[3], image->image[i].bbx[4], image->image[i].bbx[5]);
#endif  
      VXlnklist(VXlast(image->list), image->image[i].list);
    }

   vec -= image->zlo;

   image->f = (float***) vec;
   for(i=0; i<n; i++){
     vec[i+image->zlo] = (char**)(image->image[i].f);
   }

   return 1;
}

/*
 *    VXreset3dim -- free memory associated with a 3dim
 */
void
VXreset3dim(image)
VisX3dim_t *image;
{
int i, n;

    if (image->imitem == VXNIL ){
       /* temp image so delete list */
       VXdellist(image->list);
    }

    n = 1 + image->zhi - image->zlo;

    /* this frees actual frames/slices, not those in image->indx */
    /* therefore, no need to offset by image->zlo */
    for (i = 0 ; i < n; i++){
      /*
	VXresetimage(&image->image[i+image->zlo]);
      */
      VXresetimage(&image->image[i]);
    }
    if(image->image != NIL(VisXimage_t)) Vfree((char *)image->image);
    if(image->indx  != NIL(char**)) Vfree((char *)image->indx);
}

/*
 *    VXbit3dim   -- update a bitplane 3dim structure
 */
void
VXbit3dim(image)
VisX3dim_t *image;
{
int i, n;

    n = 1 + image->zhi - image->zlo;
    for (i = 0; i < n; i++){
      /*
       VXbitimage(&image->image[i+image->zlo]);
      */
       VXbitimage(&image->image[i]);
    }

}


/*
 *    VXufloat3dim  -- copy a float 3dim back to an original 3dim
 *
 */
void VXufloat3dim(im, fm) /* copy a float 3dim to an original 3dim */
VisX3dim_t *im, *fm;   
{
int i;
int zl, zh, zoff, n;

     zl = im->zlo;
     zh = im->zhi;
     n = 1 + zh - zl;
     zoff = zl - fm->zlo;
     for (i = 0; i < n; i++){
       /*
	 VXufloatimage(&im->image[i+im->zlo], &fm->image[i+zoff]);
       */
	 VXufloatimage(&im->image[i], &fm->image[i+zoff]);
     }
}




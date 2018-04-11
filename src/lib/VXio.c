/* VisX list access functions
 * List structures contain a dummy element at the start of each list 
 */

/*
 *     Complete List Functions
 *
 *    VXinit    -- create a new list   
 *    VXdellist -- delete a complete list
 *    VXcpy     -- make a copy of a list 
 *    VXjoin    -- join one list to the end of another
 *    VXlnklist -- link a list into another list
 *    VXpklist  -- pack a list structure
 *    VXupklist -- unpack a list structure
 *    VXframeset -- set frame markers on a list
 *
 */

/*
 *      Element Functions
 *
 *    VXfirst     -- move to first element of a list 
 *    VXlast      -- move to last element of a list
 *    VXfind      -- find an element with a given type 
 *    VXfindin    -- find an element with one of a set of types
 *    VXbfind     -- search backwards for an element with a given type 
 *    VXdelelem   -- delete the current element  
 *    VXaddelem   -- add a node after the given pointer position 
 *    VXlnkelem   -- link a node after the given pointer position 
 *    VXmovelem   -- move an element from one list to another
 *    VXdelobject -- delete a complete object
 *
 *    cause it would not go in VXcvt
 *  VXfloatbuf -- convert any buf to float vec
 *  (VXintbuf   -- convert any buf to int vec -not yet defined)
 *
 *     Global Functions
 *
 *    VXdelete    -- set the delete policy (currently automatic)
 *    VXupack     -- set auto unpacking policy
 *                   0 = complete, 1= fast, 2 = none
 *    VXpack      -- set auto packing policy 
 *                   0 = none, 1= complete
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
int VisXbase[16]= {1,1,2,4,8,1,1,4,1,1,1,1,1,1,1,1};
int VisXfree = 1;       /* delete strategy 1 = OK to free  */
int VisXupackopt = 0;   /* 0 = complete, 1= fast, 2 = none */
int VisXpackopt = 0;    /* 0 = none, 1= pack all data      */




/*
 *  VXinit -- create a new list
 */
/* just make the dummy node        */
VisXelem_t *
VXinit ()
{
VisXelem_t * list;
     list = (VisXelem_t *) Vmalloc(sizeof (VisXelem_t));
     list->next = NIL(VisXelem_t );
     list->prev = NIL(VisXelem_t );
     list->type = 0;
     list->size = 0;
     list->data = NIL(char);
     return list;
}

/* VXfirst -- find the first element of a list     */
/* returns a ptr to dummy node for an empty list    */
VisXelem_t *
VXfirst (ptr)
VisXelem_t * ptr;
{
VisXelem_t * first;
    first = ptr;
    while (first->prev != NIL(VisXelem_t ))
	  first = first->prev;
    if (first->next != NIL(VisXelem_t ))
          first = first->next;
    return first;
}

/*
 *      VXlast -- find the last node of a list 
 *
 *      returns a ptr to dummy node for an empty list
 */
VisXelem_t *
VXlast (ptr)
VisXelem_t * ptr;
{
VisXelem_t * last;
    last = ptr;
    while (last->next != NIL(VisXelem_t ))
         last = last->next;
    return last;
}



/* VXdelelem   --  delete the current element    */
/*                 return a ptr to the next node */
/*                 unless it is the last         */

VisXelem_t *
VXdelelem ( list)
VisXelem_t * list;
{
VisXelem_t * prev;
VisXelem_t * next;
int btype;
    prev = list->prev;
    next = list->next;
    if (prev != NIL( VisXelem_t )){
       prev->next = list->next; /* make forward link */
    }
    if(next !=NIL( VisXelem_t )){
       list->next->prev = prev; /* make backward link */
    }else{
       next = prev;
    }
    if(VisXfree){ /* delete the element */
       btype =  list->type & 0xf;
       if(btype != 0xa && btype != 0x9)
           Vfree(list->data);
       Vfree(list);
    }
    return next;
}

/*  VXaddelem add an element after the given pointer position  */
/*  the data for the node is specified by the arguments        */
/*  the function returns a pointer to the new element          */

VisXelem_t *
VXaddelem ( list, type, buf, size)
VisXelem_t * list;
int type;
int size;
char *buf;
{
VisXelem_t * ptr;
    ptr = (VisXelem_t *) Vmalloc(sizeof (VisXelem_t));
    ptr->type = type;
    ptr->size =  size;
    if(VXbase(ptr) == VX_NULL || VXbase(ptr) == VX_SCALAR || ptr-size == 0){ 
        ptr->data = NIL(char);
    }
    else{
        ptr->data = Vmalloc(ptr->size);
        memcpy( ptr->data, buf, ptr->size);
    }
    ptr->next =  list->next;
    ptr->prev = list;
    if(list->next !=NIL( VisXelem_t )){
       list->next->prev = ptr;       /* make backward link */
    }
    list->next = ptr;
    return ptr;
}

/*
 *
 * VXlnkelem -- link an element at the current pointer position
 *
 * similar to VXaddelem except that that the given data buffer is
 * used directly rather than copied.
 * this should only be used by experienced programmers!
 *
 */
VisXelem_t *
VXlnkelem ( list, type, buf, size)
VisXelem_t * list;
int type;
int size;
char *buf;
{
VisXelem_t * ptr;
    ptr = (VisXelem_t *) Vmalloc(sizeof (VisXelem_t));
    ptr->type = type;
    ptr->size =  size;
    if(VXbase(ptr) == VX_NULL || VXbase(ptr) == VX_SCALAR || ptr-size == 0){ 
        ptr->data = NIL(char);
    }
    else{
	ptr->data = buf;
    }
    ptr->next =  list->next;
    ptr->prev = list;
    if(list->next !=NIL( VisXelem_t )){
       list->next->prev = ptr;       /* make backward link */
    }
    list->next = ptr;
    return ptr;
}


/* VXfind -- find an element with a given type  */
/* a null pointer is returned if not found  */
VisXelem_t *
VXfind (ptr, type)
VisXelem_t * ptr;
int type;
{
VisXelem_t * found;
    found = ptr;
    while (found != NIL(VisXelem_t ) && found->type != type  )
	  found = found->next;
    return found;
}

/* VXfindin -- find an element of one of several types */
/* the list must be terminated by VX_NULLT             */
/* a null pointer is returned if not found  */
VisXelem_t *
VXfindin (ptr, types)
VisXelem_t * ptr;
int *types;
{
VisXelem_t * found;
int i,n;
    /* first find number of types */
    n=0;
    while (types[n] != VX_NULLT) n++;

    found = ptr;
    while (found != NIL(VisXelem_t ) ){
       for (i=0; i < n; i++)
	 if (found->type == types[i])
	   return(found);
       found = found->next;
    }
    return found;
}

/* VXbfind -- search backwards for a node with a given type */
/* if not found a ptr to null is returned ??? */
/* if it is already there just return it */
VisXelem_t *
VXbfind (ptr, type)
VisXelem_t * ptr;
int type;
{
VisXelem_t * found;
    found = ptr;
      while (found != VXNIL && found->type != type )
	  found = found->prev;
    return found;
}



/*
 *
 *  VXlnklist -- link a list into another list
 *  (makes VXjoin redundant)
 *
 *  ptra is the main list
 *  ptrb is the list to be linked
 */
void
VXlnklist (ptra, ptrb)
VisXelem_t *ptra;
VisXelem_t *ptrb;
{
VisXelem_t * lend;
VisXelem_t * lstart;
    /* lend = VXlast(ptra); */
    lstart = VXfirst(ptrb); /* start of list to be linked */
    lend   = VXlast(ptrb);
    /* detach the old */
    if(lstart->prev != VXNIL)
       lstart->prev->next =  VXNIL;
    else /* empty list */
       return;
    /* attach the new */
    lend->next = ptra->next;
    ptra->next = lstart;
    /* and now the back pointers */
    lstart->prev = ptra;
    if (lend->next != VXNIL)
       lend->next->prev = lend;
}

void
VXjoin (ptra, ptrb)
VisXelem_t *ptra;
VisXelem_t *ptrb;
{
VisXelem_t * lend;
VisXelem_t * lstart;
    lend = VXlast(ptra);
    lstart = VXfirst(ptrb);
    /* detach the old */
    lstart->prev->next =  NIL(VisXelem_t );
    /* attach the new */
    lend->next = lstart;
    lstart->prev = lend;
}

/* VXdelobject -- delete a whole object  */
/*     if the node is not an ID then backtrack until one is found */
/*     it is a fatal error is none is found    */
/*     then delete all nodes for the ID until the next ID */
/*     OR an EFRAME *****                                 */

int objelist [] = {VX_ID, VX_EFRAME, VX_NULLT};
VisXelem_t *
VXdelobject (ptr )
VisXelem_t * ptr;
{
VisXelem_t * ostart;
VisXelem_t * oend;
VisXelem_t * prev;
     ostart = VXbfind(ptr, VX_ID);
     if (ostart == NIL(VisXelem_t )){
	fprintf( stderr, "VXdelobject: object id not found/n");
	exit(1);
     }
     prev = ostart->prev;
     oend = VXfindin(ptr->next, objelist );

     /* delete the thing */
     while (ostart != oend && ostart != prev)
	ostart = VXdelelem(ostart);
     return ostart;
}


/* VXcpy -- make a copy of a list    */
/* This includes initailization with VXinit */
/* newlist = VXcpy (oldlist);		     */
/* a copy is made from the current location on the old list */
VisXelem_t *
VXcpy (olist)
VisXelem_t * olist;
{
VisXelem_t * nlist, *ptr;
     ptr = nlist = VXinit();           /* intialize new list         */
     if(olist->prev ==  NIL(VisXelem_t ))/* move to first real element */
	   olist = olist->next;
     while(olist !=  NIL(VisXelem_t )){   /* copy one element at a time */
	   ptr = VXaddelem(ptr, olist->type, olist->data, olist->size);
	   olist = olist->next;
     }
     return(nlist); /* RJP added return value */
}


/*
 *     VXdellist -- delete a complete list
 *         from and including the element pointed to
 */
void
VXdellist (lptr)
VisXelem_t * lptr;
{
VisXelem_t * ptr;

     ptr = VXlast(lptr);
     while (ptr != lptr){
	ptr = VXdelelem(ptr);
     }
     if (ptr->prev == VXNIL)
	Vfree(ptr); /* dummy node */
     else
	ptr = VXdelelem(ptr);
 
}

/*
 *    VXpklist  -- pack a list structure
 *
 *    pack all the list from this point on
 *    (do not pack frame markers)
 *
 */

#define RND8(a) ((a) + (((a) & 0X3) ? 8 - ((a) & 0X3) : 0))
void
VXpklist (lptr)
VisXelem_t * lptr;
{
VisXelem_t * ptr;
VisXelem_t * eptr;
unsigned int n, tsize;
char * rptr;
char * dptr;

   /* check for empty list */
   if (VXNIL == lptr->next && VXNIL == lptr->prev) return;
   /* skip over a dummy first node */
   if (VXNIL == lptr->prev) lptr = lptr->next;

   tsize = RND8(sizeof(VisXelem_t));
   n = tsize;
   ptr = lptr; /* skip a frame marker */
   if (ptr != VXNIL && ptr->type == VX_FRAME){
      ptr = lptr = lptr->next;
   }
   ptr = lptr;
   /* now find how many bytes */
   n = 0;
   while (ptr != VXNIL && ptr->type != VX_EFRAME && ptr->type != VX_FRAME){
       n += tsize;
       if(VXbase(ptr) != VX_NULL && VXbase(ptr) != VX_SCALAR)
	  n += RND8(VXsize(ptr));
       ptr = ptr->next;
   }
   rptr = (char *) Vmalloc(n);

   /* copy elements into buffer and delete nodes */
   ptr = lptr;
   lptr = lptr->prev;
   dptr = rptr;
   while (ptr != VXNIL && ptr->type != VX_EFRAME && ptr->type != VX_FRAME){
       memcpy(dptr, (char *)ptr, sizeof(VisXelem_t));
       eptr = (VisXelem_t *) dptr;
       eptr->prev = (VisXelem_t *)0; /* do not need back ptrs */
       dptr += tsize;
       eptr->next = (VisXelem_t *)( dptr - rptr); 
       if(VXbase(ptr) != VX_NULL && VXbase(ptr) != VX_SCALAR){
	   eptr->data = (char *) (dptr - rptr); /* store offset */
           memcpy(dptr, ptr->data, VXsize(ptr));
	   dptr += RND8(VXsize(ptr));
           eptr->next = (VisXelem_t *) (dptr - rptr); 
       }
       if(ptr->next == VXNIL){ /* since VXdelem does not return nil */
           ptr = VXdelelem(ptr);
	   ptr = VXNIL;
	}else
           ptr = VXdelelem(ptr);
   }
   eptr->next = (VisXelem_t *)0; /* set end marker */
   /* finish the new element */
   lptr = VXlnkelem(lptr,VX_COMP, rptr, n);
   ptr = lptr; /* check for more frames */
   if(lptr->next != VXNIL &&
	     (lptr->next->type == VX_EFRAME || lptr->next->type == VX_FRAME)){
      ptr = lptr->next;
      if(ptr->type == VX_EFRAME) ptr = ptr->next;
      if(ptr != VXNIL) VXpklist(ptr);  /* look mom recursion */
   }
   return;
}


/*
 *    VXupklist  --un pack a list structure
 *
 *    unpack in place right now... interlocks later
 *    (better not delete after this)
 *
 */

void
VXupklist (lptr)
VisXelem_t * lptr;
{
VisXelem_t * ptr;
VisXelem_t * prev;
VisXelem_t * pptr;
unsigned int base, base2;

     base = (unsigned int) lptr->data;
     ptr = (VisXelem_t *) lptr->data;
     if(VisXupackopt == 1){ /* fast unpack */
         VisXfree = 0;
         /* first elem link back */
         lptr->prev->next =  ptr;
         /* first elem link on */
         prev = lptr->prev;
     }else{ /* complete unpack */
	 prev = VXNIL;
     }

     while ( ptr != VXNIL ){
#ifdef P_DEBUG
	 fprintf(stderr, "VXupack bae %d \n",base);
	 fprintf(stderr, "VXupack ptr %d \n",(int)ptr);
	 fprintf(stderr, "VXupack unpacking \n");
	 fprintf(stderr, "VXupack VXsize %d \n",VXsize(ptr));
	 fprintf(stderr, "VXupack VXnext %d \n",(int)ptr->next);
#endif

	 ptr->prev = prev;
	 base2 = ((unsigned int)ptr->next) + base;
	 if( ptr->next != VXNIL )
	      ptr->next = (VisXelem_t *) base2;
	 if(NIL(char) != ptr->data) ptr->data += base;
	 prev = ptr;
	 ptr = ptr->next;
     }

#ifdef P_DEBUG
     fprintf(stderr, "VXupack loop done \n"); */
#endif

     if(VisXupackopt == 1){ /* fast unpack */
         prev->next = lptr->next;
         if(VXNIL != prev->next) {
	     prev->next->prev = prev; /*back link from next elem */
	 }
         Vfree(lptr); /* get rid of pack elem */
     }else{ /* complete unpack */
	 ptr = lptr;
         pptr = (VisXelem_t *) lptr->data;
         while(pptr !=  NIL(VisXelem_t )){   /* copy one element at a time */
	       ptr = VXaddelem(ptr, pptr->type, pptr->data, pptr->size);
	       pptr = pptr->next;
         }
	 VXdelelem(lptr); /* delete the original */
     }

#ifdef P_DEBUG
     fprintf(stderr, "VXupack exiting \n");
#endif

     return;
}

/*
 *    VXupack     -- set auto unpacking policy
 *
 */
void
VXupack(opt)
int opt;
{
    VisXupackopt = opt;
}


/*
 *    VXpack      -- set auto packing policy 
 *
 */
void
VXpack(opt)
int opt;
{
    VisXpackopt = opt;
}


/*
 *
 *  VXmovelem -- move an element from one list to another
 *
 *  returns ptr to moved elem
 */
VisXelem_t * VXmovelem(ptra, ptrb)
VisXelem_t *ptra, *ptrb;
{
      /* exit the old */
      if (ptrb->prev != VXNIL)
	  ptrb->prev->next = ptrb->next;
      if (ptrb->next != VXNIL)
	  ptrb->next->prev = ptrb->prev;
      /* add the new */
      ptrb->next = ptra->next;
      if (ptrb->next != VXNIL)
	  ptrb->next->prev = ptrb;
      ptra->next = ptrb;
      ptrb->prev = ptra;
      return ptrb;
}


/*
 *
 *  VXframeset -- set frame markers on an a list
 *
 * if frame markers exist  upste them
 * if they do not exist, create them
 * be sure to give the start of the list
 */
void
VXframeset(list, n)
VisXelem_t *list;
int n;
{
VisXelem_t *vpt;
   vpt = VXfirst(list);
   if (VXtype(vpt) == VX_FRAME){
      vpt->size = n;
   }else{
      vpt = VXaddelem(list, VX_FRAME, "", n);
   }
   vpt = VXlast(list);
   if (VXtype(vpt) == VX_EFRAME){
      vpt->size = n;
   }else{
      vpt = VXaddelem(vpt, VX_EFRAME, "", n);
   }

}


/*
 *
 *  VXfloatbuf -- convert any buf to float vec
 */
float *
VXfloatbuf ( ptr )
VisXelem_t *ptr;
/* uses number of bytes so may be a bit long for odd bp length */
{
int base;
int num;
int i;
float *obuf, *pt;
unsigned char *upt;
short *spt;
int *ipt;
char *cpt;
float *fpt;
double *dpt;
char * bbuf;
    
    base = VXbase(ptr);
    num = VXnumelem(ptr);
    pt = obuf = (float *) Vmalloc(num * sizeof(float));
    switch (base){
        case VX_BYTE:
            upt = (unsigned char *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*upt++; /* RJP added casts */
            }
            break;
        case VX_SHORT:
            spt = (short *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*spt++;
            }
            break;
        case VX_INT:
            ipt = (int *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*ipt++;
            }
            break;
        case VX_FLOAT:
            fpt = (float *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*fpt++;
            }
            break;
        case VX_CHAR:
            cpt = (char *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*cpt++;
            }
            break;
        case VX_BIT:
            cpt = (char *) VXdata(ptr);
            bbuf = Vcvtbptoby(cpt, num * 8);
            for (i=0; i < num; i++){
              *pt++ = (float)(*bbuf++ >> 7);
            }
            break;
        case VX_DOUBLE:
            dpt = (double *) VXdata(ptr);
            for (i=0; i < num; i++){
              *pt++ = (float)*dpt++;
            }
            break;
   }
   return(obuf);
}

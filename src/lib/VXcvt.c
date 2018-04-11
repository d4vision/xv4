/*
 *  Vcvtbytobp -- convert byte to bit-plane
 *  Vcvtbptoby -- convert bit-plane to byte
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

#include    "VisXV4.h"
#include    "Vutil.h"

/*
 *  Vcvtbytobp -- convert byte to bit-plane
 */
char *
Vcvtbytobp( buf, num )
char * buf; /* input byte buffer */
int num;    /* number of bits in result */
{
int n8, nb, i;
int rem;
char *obuf, *pt;
char *ipt;
   n8 = num >> 3;
   rem = num - (n8 << 3);
   nb = n8;
   if(rem != 0) nb++;
   pt = obuf = Vmalloc(nb);
   ipt = buf;
   for (i=0; i<n8; i++){
     *pt = 0;
     if( *ipt++ ) *pt |= 128;
     if( *ipt++ ) *pt |= 64;
     if( *ipt++ ) *pt |= 32;
     if( *ipt++ ) *pt |= 16;
     if( *ipt++ ) *pt |= 8;
     if( *ipt++ ) *pt |= 4;
     if( *ipt++ ) *pt |= 2;
     if( *ipt++ ) *pt |= 1;
     pt++;
   }
     if (!rem ) goto done2;/* fix any remaining bits */
     if( *ipt++ ) *pt |= 128;
     if( rem == 1 ) goto done2;
     if( *ipt++ ) *pt |= 64;
     if( rem == 2 ) goto done2;
     if( *ipt++ ) *pt |= 32;
     if( rem == 3 ) goto done2;
     if( *ipt++ ) *pt |= 16;
     if( rem == 4 ) goto done2;
     if( *ipt++ ) *pt |= 8;
     if( rem == 5 ) goto done2;
     if( *ipt++ ) *pt |= 4;
     if( rem == 6 ) goto done2;
     if( *ipt++ ) *pt |= 2;
     if( rem == 7 ) goto done2;
     if( *ipt++ ) *pt |= 1;
done2:
   return(obuf);

}

/*
 *  Vcvtbptoby -- convert bit-plane to byte
 */
char *
Vcvtbptoby( buf, num )
char * buf; /* input byte buffer */
int num;    /* number of bytes in result */
{
int n8, i;
int rem;
char *obuf, *pt;
char *ipt;
    ipt = buf;
    pt = obuf = Vmalloc(num);
    n8 = num >> 3;
    rem = num - (n8 << 3);
    for (i=0; i < n8; i++){
        *pt++ = (*ipt & 128) ? 255 : 0;
        *pt++ = (*ipt &  64) ? 255 : 0;
        *pt++ = (*ipt &  32) ? 255 : 0;
        *pt++ = (*ipt &  16) ? 255 : 0;
        *pt++ = (*ipt &   8) ? 255 : 0;
        *pt++ = (*ipt &   4) ? 255 : 0;
        *pt++ = (*ipt &   2) ? 255 : 0;
        *pt++ = (*ipt &   1) ? 255 : 0;
        ipt++;
    }
     if( rem == 0 ) goto done1;
     *pt++ = (*ipt & 128) ? 255 : 0;
     if( rem == 1 ) goto done1;
     *pt++ = (*ipt &  64) ? 255 : 0;
     if( rem == 2 ) goto done1;
     *pt++ = (*ipt &  32) ? 255 : 0;
     if( rem == 3 ) goto done1;
     *pt++ = (*ipt &  16) ? 255 : 0;
     if( rem == 4 ) goto done1;
     *pt++ = (*ipt &   8) ? 255 : 0;
     if( rem == 5 ) goto done1;
     *pt++ = (*ipt &   4) ? 255 : 0;
     if( rem == 6 ) goto done1;
     *pt++ = (*ipt &   2) ? 255 : 0;
     if( rem == 7 ) goto done1;
     *pt++ = (*ipt &   1) ? 255 : 0;
done1:
   return(obuf);
}

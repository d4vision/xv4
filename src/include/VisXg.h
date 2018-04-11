/*
 * VisXg.h	--  VisX graph format key definitions
 */
/*
 *   General rule: last digit indicates data type according to
 *
 *		0 = text, 1 = unsigned byte, 2 = short
 *		3 = float 4 = double, 5 = signed byte
 *		6 = bit   7 = long (4 byte)
 *		8 = mixed type structures 9 = binary flag (no data)
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

        key         value		description
*/
#define	VG_V2D	    0x33    /*  2 dimensional verices x,y   		*/
#define	VG_V3D	    0x43    /*  3 dimensional verices x,y,z 		*/
#define	VG_INVY	    0x39    /*  Inverted Y scale (ymax-not known) 	*/
#define VG_BBX	    0x53    /*  Bounding Box xm,xM,ym,yM,...    	*/
#define VG_COLOR    0x63    /*  Color Vector r,g,b (float)	    	*/
#define VG_CMAP     0x61    /*  Color Map r,g,b (byte)		    	*/
#define	VG_ID	    0x07    /*  Object Identifier		 	*/
#define	VG_CIDX	    0x67    /*  Color Index			 	*/
#define VG_GFV      0x73    /*  General Feature Vector                  */




typedef struct visxgsxs{
int	key;
int	size;
int     num;
char   *data;
struct visxgsxs *next;
struct visxgsxs *prev;
} VisXGst_t;

extern VisXGst_t *	VSGinit		__P(());
extern VisXGst_t *	VSGfirst	__P((VisXGst_t *));
extern VisXGst_t *	VSGlast 	__P((VisXGst_t *));
extern VisXGst_t *	VSGread		__P((VisXst_t *));
extern void		VSGwrite  	__P((VisXst_t *, VisXGst_t *));
extern VisXGst_t *	VSGdeletenode	__P((VisXGst_t *));
extern VisXGst_t *	VSGaddnode    __P((VisXGst_t *, int, int, int, char *));
extern VisXGst_t *	VSGfind       	__P((VisXGst_t *, int ));
extern VisXGst_t *	VSGbfind       	__P((VisXGst_t *, int ));
extern void 		VSGjoin       	__P((VisXGst_t *, VisXGst_t *));
extern VisXGst_t *	VSGdelobject	__P((VisXGst_t *));
extern VisXGst_t *	VSGgcpy		__P((VisXGst_t *));

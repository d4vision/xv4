/*
 * Vborder.c: Byteorder definitions
 *
 */



/*
 *   Copyright 2014 Anthony P. Reeves
 *
 *   contributions by Chirstos Zoulas
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

/* for the swab function... */
#define _XOPEN_SOURCE

#include "VisXV4.h"
#include "VisX.h"
#include "Vborder.h"
#include <stdio.h>

static char buffer[4000];

struct machine_t machine[] =
{
	{SUN_NAME,	SUN_TYPE   },
	{ENCORE_NAME,	ENCORE_TYPE},
	{VAX_NAME,  	VAX_TYPE   },
	{ATT_NAME,  	SUN_TYPE   },
	{RTPC_NAME, 	SUN_TYPE   },
	{HPUX_NAME, 	SUN_TYPE   },
	{D3100_NAME,	VAX_TYPE   },
	{GS1000_NAME,	SUN_TYPE   },
	{TEK4320_NAME,	SUN_TYPE   },
	{IBM370_NAME,	SUN_TYPE   },
	{APOLLO_NAME,   SUN_TYPE   },
	{LINUX_NAME,    ENCORE_TYPE},
	{FBSD_NAME,     ENCORE_TYPE},
	{LINUXPPC_NAME, SUN_TYPE   },
	{SGI_NAME,      SUN_TYPE   },
	{WIN32_NAME,    ENCORE_TYPE},
	{LITTLE_NAME,	VAX_TYPE   },
	{BIG_NAME,	SUN_TYPE   },
	{UNKNOWN_NAME,	UNKNOWN_TYPE},
	{0,             0 }
};
/*
 *	byte ordering for data types
 *
 *	machine-type       float data    long data   short data
 *	Sun-type           ABCD          ABCD        AB
 *	Encore-type        DCBA          DCBA        BA
 *	Vax-type           BADC          DCBA        BA
 */

/*
 * 	the following arrays are of the form :
 *
 *		conversion[src][dst]
 *
 *		dst0	dst1	dst2	...
 *  	src0	(0-0)	(0-1)	(0-2)
 *  	src1	(1-0)	(1-1)	(1-2)
 *	src2	(2-0)	(2-1)	(2-2)
 * 	.
 *	Gwhere (a-b) is the conversion from machine-type a to machine-type b
 */

/* 	amount added to each byte in float representation after byte swapping */
static int float_bias[MACHINE_TYPES][MACHINE_TYPES][4] =	
{
	{{ 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 1, 0, 0}}, 
	{{ 0, 0, 0, 0}, { 0, 0, 0, 0}, { 0, 1, 0, 0}},
	{{-1, 0, 0, 0}, { 0, 0, 0,-1}, { 0, 0, 0, 0}}
};

static int float_order[MACHINE_TYPES][MACHINE_TYPES] =
{
	{ABCD, DCBA, BADC},
	{DCBA, ABCD, CDAB},
	{BADC, CDAB, ABCD}
};

static int long_order[MACHINE_TYPES][MACHINE_TYPES] =	
{
	{ABCD, DCBA, DCBA},
	{DCBA, ABCD, ABCD},
	{DCBA, ABCD, ABCD}
};

static int short_order[MACHINE_TYPES][MACHINE_TYPES] =	
{
	{  AB,   BA,   BA},
	{  BA,   AB,   AB},
	{  BA,   AB,   AB}
};

void
machine_names(fp)
FILE *fp;
{
	int m;

	for (m = 0; !strcmp(machine[m].name, UNKNOWN_NAME); m++)
	{
		(void)fprintf(fp, "     %s\n", machine[m].name);
	}
}

char *
full_name(nam)
char *nam;
{
	int k;

	for ( k = 0; machine[k].name; k++ )
		if ( !strncmp(machine[k].name, nam, P_MSIZE - 1) )
			break;

	if ( ! machine[k].name )
		k--;

	return( machine[k].name );
}

int
machine_type(name)
char *name;
{
	int m;

	if  (name == (char *)NULL) return(UNKNOWN_TYPE);

	for (m = 0; machine[m].name != 0; m++)
		if (strncmp(name, machine[m].name, P_MSIZE-1) == 0) 
			return(machine[m].type);

	return (UNKNOWN_TYPE);
}

int
fix_header(hin, src_name, dst_name)
VisXhdr_t *hin;
char *src_name;
char *dst_name;
{
	int src, dst;

	src = machine_type(src_name);
	dst = machine_type(dst_name);

	if ((src == UNKNOWN_TYPE) || (dst == UNKNOWN_TYPE))
	{
		return(-1);
	}

	strncpy(hin->machine, dst_name, P_MSIZE);
	hin->machine[P_MSIZE - 1] = '\0';

	swab2(&hin->hdr,       2, SHORT_DATA, src, dst);
	swab2(&hin->fmt,       2, SHORT_DATA, src, dst);
	swab2(&hin->xsize,     2, SHORT_DATA, src, dst);
	swab2(&hin->ysize,     2, SHORT_DATA, src, dst);
	swab2(&hin->zsize,     2, SHORT_DATA, src, dst);
	swab2(&hin->nchan,     2, SHORT_DATA, src, dst);
	swab2(&hin->uchan,     2, SHORT_DATA, src, dst);
	swab2(&hin->uid,       2, SHORT_DATA, src, dst);
	swab4(&hin->crdate,    4,  LONG_DATA, src, dst, TRUE);
	swab2(&hin->modified,  2, SHORT_DATA, src, dst);
	swab2(&hin->zero,      2, SHORT_DATA, src, dst);

	return(0);
}

void
swap_copy(ifd, ofd, bytes, size, src, dst, addbias)
int size, ifd, ofd, bytes, src, dst, addbias;
{
	switch (size)
	{
	case BYTE_DATA :
        	Vread(ifd, buffer, bytes);
		if (ifd == ofd) lseek(ifd, -bytes, 1);
		Vwrite(ofd, buffer, bytes);
		break;

	case SHORT_DATA :
        	Vread(ifd, buffer, bytes);
	        if (ifd == ofd) lseek(ifd, -bytes, 1);
		swab2(buffer, bytes, SHORT_DATA, src, dst);
	        Vwrite(ofd, buffer, bytes);
		break;

	case LONG_DATA  :
	      	Vread(ifd, buffer, bytes);
	        if (ifd == ofd) lseek(ifd, -bytes, 1);
		swab4(buffer, bytes, LONG_DATA, src, dst, addbias);
	        Vwrite(ofd, buffer, bytes);
		break;

	case FLOAT_DATA :
	      	Vread(ifd, buffer, bytes);
	        if (ifd == ofd) lseek(ifd, -bytes, 1);
		swab4(buffer, bytes, FLOAT_DATA, src, dst, addbias);
	        Vwrite(ofd, buffer, bytes);
		break;
	}
}

void
swab2(buf, n, size, src, dst)
char *buf;
int  n, size, src, dst;
{
	if (short_order[src][dst] == BA) swab(buf, buf, n);
}


void
swab4(buf, n, size, src, dst, addbias)
char *buf;
int  n;
int  size, src, dst;
int  addbias;
{
	int i, k;
	int  order;
	char a,b,c,d;
	char *p1, *p2;
	int bias[4];

	k = n/4;
	p1 = buf;
	p2 = buf;
	bias[0] = bias[1] = bias[2] = bias[3] = 0;

	switch (size)
	{
	case LONG_DATA :
		order = long_order[src][dst];
		break;
	case FLOAT_DATA :
		order = float_order[src][dst];
		if (addbias)
		{
			bias[0] = float_bias[src][dst][0];
			bias[1] = float_bias[src][dst][1];
			bias[2] = float_bias[src][dst][2];
			bias[3] = float_bias[src][dst][3];
		}
		break;
	case SHORT_DATA :
		return;
	default:
		order = 0;
		break;
	}

        switch (order) 
	{
	case DCBA :
		for (i=0; i<k; i++){
		  	a = *p1++;
			b = *p1++;
			c = *p1++;
			d = *p1++;
			*p2++ = d + bias[0];
			*p2++ = c + bias[1];
			*p2++ = b + bias[2];
			*p2++ = a + bias[3];
		}
		break;

	case BADC :
		for (i=0; i<k; i++){
		  	a = *p1++;
			b = *p1++;
			c = *p1++;
			d = *p1++;
			*p2++ = b + bias[0];
			*p2++ = a + bias[1];
			*p2++ = d + bias[2];
			*p2++ = c + bias[3];
		}
		break;

	case CDAB :
		for (i=0; i<k; i++){
		  	a = *p1++;
			b = *p1++;
			c = *p1++;
			d = *p1++;
			*p2++ = c + bias[0];
			*p2++ = d + bias[1];
			*p2++ = a + bias[2];
			*p2++ = b + bias[3];
		}
		break;

        case ABCD :
		for (i=0; i<k; i++){
		  	a = *p1++;
			b = *p1++;
			c = *p1++;
			d = *p1++;
			*p2++ = a + bias[0];
			*p2++ = b + bias[1];
			*p2++ = c + bias[2];
			*p2++ = d + bias[3];
		}
		break;
	}
}

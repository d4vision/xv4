/* 
 * VXpnm.h: xv4Package PNM and TIFF i/o function prototypes
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

#ifndef _H_VisXPNM
#define _H_VisXPNM

#include "cdefs.h"
#include <stdio.h>

extern void pnmtovx  __P((FILE *, VisXelem_t *, int));
extern void vxpnm    __P((int, VisXelem_t *, int));
extern void vxtotif  __P((int, VisXelem_t *));


#endif
/* end  _H_VisXPNM */

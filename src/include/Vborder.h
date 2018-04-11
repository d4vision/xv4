/* $Header$ */
/*
 * border.h: Machine byte order defs
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


#ifndef _h_border
#define _h_border

#include "cdefs.h"

#ifndef TRUE
#define TRUE		1
#endif
#ifndef FALSE
#define FALSE		0
#endif

struct machine_t {
	char *name;
	int type;
};

#define SUN_NAME	"sun"
#define ENCORE_NAME	"encore"
#define VAX_NAME	"vax"
#define ATT_NAME	"att"
#define RTPC_NAME	"rtpc"
#define	HPUX_NAME	"hpux"
#define	D3100_NAME	"ds3100"
#define GS1000_NAME	"gs1000"
#define TEK4320_NAME	"Tek4320"
#define IBM370_NAME	"ibm370"
#define APOLLO_NAME	"apollo"
#define LINUX_NAME	"linux"
#define LINUXPPC_NAME	"LinuxPPC"
#define FBSD_NAME	"FreeBSD"
#define SGI_NAME	"sgi"
#define WIN32_NAME  "win32"
#define LITTLE_NAME	"little-endian"
#define BIG_NAME	"big-endian"
#define UNKNOWN_NAME 	"unknown"

#define SUN_TYPE    	0
#define ENCORE_TYPE 	1
#define VAX_TYPE    	2
#define UNKNOWN_TYPE 	3
#define MACHINE_TYPES	3

#define NAME_LEN 3
#define BYTE_DATA 3

#if defined(sun) || defined(solaris)
# define LOCAL_MACHINE_NAME		SUN_NAME
# define LOCAL_MACHINE_TYPE		SUN_TYPE
#endif
/* end  sun      */

#ifdef encore
#  define LOCAL_MACHINE_NAME		ENCORE_NAME
#  define LOCAL_MACHINE_TYPE		ENCORE_TYPE
# endif
/* end  encore   */

#ifdef vax
#   define LOCAL_MACHINE_NAME		VAX_NAME
#   define LOCAL_MACHINE_TYPE		VAX_TYPE
#endif
/* end  vax      */

#ifdef att
#    define LOCAL_MACHINE_NAME		ATT_NAME
#    define LOCAL_MACHINE_TYPE		SUN_TYPE
#endif
/* end  att      */

#ifdef rtpc
#   define LOCAL_MACHINE_NAME		RTPC_NAME
#   define LOCAL_MACHINE_TYPE		SUN_TYPE
#endif
/* end  rtpc     */

#ifdef hpux
#  define LOCAL_MACHINE_NAME	HPUX_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  hpux     */

#ifdef ds3100
#   define LOCAL_MACHINE_NAME	D3100_NAME
#   define LOCAL_MACHINE_TYPE	VAX_TYPE
#endif
/* end  ds3100   */

#ifdef gs1000
#   define LOCAL_MACHINE_NAME	GS1000_NAME
#   define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  gs1000   */

#ifdef Tek4320
#  define LOCAL_MACHINE_NAME	TEK4320_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  Tek4320  */

#ifdef ibm370
#  define LOCAL_MACHINE_NAME	IBM370_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  ibm370   */

#ifdef apollo
#  define LOCAL_MACHINE_NAME	APOLLO_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  apollo   */

#ifdef linux
#  define LOCAL_MACHINE_NAME	LINUX_NAME
#  define LOCAL_MACHINE_TYPE	ENCORE_TYPE
#endif
/* end  linux   */

#ifdef FreeBSD
#  define LOCAL_MACHINE_NAME	FBSD_NAME
#  define LOCAL_MACHINE_TYPE	ENCORE_TYPE
#endif
/* end  FreeBSD   */

#ifdef LinuxPPC
#  define LOCAL_MACHINE_NAME	LINUXPPC_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  LinuxPPC   */


#ifdef sgi
#  define LOCAL_MACHINE_NAME	SGI_NAME
#  define LOCAL_MACHINE_TYPE	SUN_TYPE
#endif
/* end  sgi   */

#ifdef WIN32
#  define LOCAL_MACHINE_NAME	WIN32_NAME
#  define LOCAL_MACHINE_TYPE	ENCORE_TYPE
#endif
/* end  winpc   */

#ifndef LOCAL_MACHINE_NAME
# ifdef VLITTLE_ENDIAN
#  define LOCAL_MACHINE_NAME		LITTLE_NAME
#  define LOCAL_MACHINE_TYPE		VAX_TYPE
# endif
# endif

#ifndef LOCAL_MACHINE_NAME
# ifdef VBIG_ENDIAN
#  define LOCAL_MACHINE_NAME		BIG_NAME
#  define LOCAL_MACHINE_TYPE		SUN_TYPE
# endif
#endif

#ifndef LOCAL_MACHINE_NAME
**** ERROR Must at least know machine byte order ****
#endif

#define AB	0
#define BA	1

#define ABCD	0
#define DCBA	1
#define BADC	2
#define CDAB	3

#define SHORT_DATA 	0
#define LONG_DATA	1
#define FLOAT_DATA	2

__BEGIN_CDECLS

extern int  machine_type();
extern void machine_names();

extern int  fix_header();
extern void swab2();
extern void swab4();
extern void swap_copy();
extern char *full_name();

extern struct machine_t machine[];

__END_CDECLS

#endif
/* end  _h_border */

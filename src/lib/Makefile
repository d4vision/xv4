# DO NOT EDIT
XV4DIR = /opt/xv4
OPTIM = -O -Wall
DESTBIN = /opt/xv4/bin
DESTLIB = /opt/xv4/lib
DESTETC = /opt/xv4/etc
DESTMAN = /opt/xv4/man
DESTINC = /opt/xv4/include
# Begin of platform-specific file
#
# lib Makefile for Linux (CentOS / RHEL)
#
#
#
#  Copyright 2014 Anthony P. Reeves
#
#  This file is part of xv4Package.
#
#  xv4Package is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  any later version.
#
#  xv4Package is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with xv4Package.  If not, see <http://www.gnu.org/licenses/>.
#
#


OPSYS	= 
MACH	= -Dlinux -m32
CDEFS	=  -DVLITTLE_ENDIAN
FDEFS	= 
SYSLIB	= 
XROOT	= /usr/X11R6
MAKE	= make
CC	= gcc
EXTRA	= 
EXTLIB	=


#
# BINARIES/LIBRARIES
#

SHELL	= /bin/sh
INCLUDE	= -I../include
CFLAGS  = ${OPTIM} ${OPSYS} ${MACH} ${CDEFS} ${INCLUDE} ${EXTRA}
FFLAGS	= ${OPTIM} ${FDEFS}

.SUFFIXES:.out

# Rules for c files
.c.out : 
	${CC} ${CFLAGS} $*.c -o $*.out ${CLIBS}

.c.o :
	${CC} -c ${CFLAGS} $*.c 

.o.out :
	@(if test -f $*.c; then \
		echo "${CC} ${CFLAGS} $*.o -o $*.out ${CLIBS}";\
		${CC} ${CFLAGS} $*.o -o $*.out ${CLIBS};\
	else \
		echo "I cannot find any c source for $*.o";\
	fi;)


CLIBS	= ${DESTLIB}/vxlib.a \
           ${SYSLIB} ${EXTLIB} -lm

EXTRA   = -DVISXBIN=\"${DESTBIN}\" \
	  -DVISXMAN=\"${DESTMAN}\" \
	  -DVISXLIB=\"${DESTLIB}\" \
	  -DVISXETC=\"${DESTETC}\" \
	  -DVISXINC=\"${DESTINC}\" \
	  -DVISXHOME=\"${VISX}\" 
LIB	= vxlib.a
CSRC 	=   Vparse.c VXio.c VXu.c Valloc.c Verror.c Varray.c VXopen.c \
	    Vborder.c Vcomp.c VXim.c VXcvt.c Vgetdir.c \
	    VX3dim.c VXparse.c VXpnm.c

COBJ 	=   Vparse.o VXio.o VXu.o Valloc.o Verror.o Varray.o VXopen.o \
	    Vborder.o Vcomp.o VXim.o VXcvt.o Vgetdir.o \
	    VX3dim.o VXparse.o VXpnm.o

OBJ=${COBJ}

all:: ${LIB}

${LIB}: ${COBJ}
	rm -f $@
	@(if test "X${OPSYS}" = "X-DSYSV"; then\
		echo "Creating SYSV $@ library.";\
		ar cr $@ `lorder ${OBJ} | tsort`;\
	    else\
		echo "Creating static $@ library.";\
		ar cr $@ ${OBJ};\
		ranlib $@;\
	    fi)
	@echo "All done"

install:: ${DESTLIB}/${LIB}
	@echo "Install done"

${DESTLIB}/${LIB} : ${LIB}
	@(if test "X${OPSYS}" = "X-DSYSV"; then\
		cp ${LIB} ${DESTLIB}/${LIB};\
		else\
		cp ${LIB} ${DESTLIB}/${LIB};\
		ranlib ${DESTLIB}/${LIB};\
		fi)

clean::
	rm -f ${OBJ} ${LIB}
	@echo "Clean done"

step::
	${MAKE} ${MFLAGS}
	${MAKE} ${MFLAGS} install
	${MAKE} ${MFLAGS} clean



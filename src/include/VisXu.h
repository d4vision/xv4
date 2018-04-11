/*
 * VisXu.h  VisX measurement / resolution unit names
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

#ifndef _h_visx4units
#define _h_visx4units

typedef struct {
int val;
char *sname;	/* short name */
char *lname;	/* long name */
} VisXunit_t;

VisXunit_t VisXunit[] =
{
  {0, "px", "pixels"},
  {1, "mm", "millimeters"},
{0, 0, 0}};

#endif

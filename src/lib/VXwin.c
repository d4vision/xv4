/*
 * Windows stub for missing functions
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


#include <stdlib.h>
#include <stdio.h>

int getuid() { return 0; }
int getpid() { return _getpid(); }

/* ABC 03-25-03 : fix these later, for now, they exist for compiling */

int fork() {
  fprintf(stderr, "ERROR: fork() not supported in windows.\n");
  exit(1); 
}

int wait(int x) {
  fprintf(stderr, "ERROR: wait() not supported in windows.\n");
  exit(1);
}

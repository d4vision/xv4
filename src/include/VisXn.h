/*
 * VisXn.h  VisX element names
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

typedef struct {
int val;
char *name;
char *pname;
} VisXename_t;

VisXename_t VisXename[] =
{
  {0x1a, "Image Frame    ", "VX_FRAME"},
  {0x2a, "End Frame      ", "VX_EFRAME"},
  {0x08, "Compact List   ", "VX_COMP"},
  {0x33, "2-D vector     ", "VX_V2D"},
  {0x43, "3-D vector     ", "VX_V3D"},
  {0x39, "Inverted Y     ", "VX_INVY"},
  {0x49, "Z is Frame     ", "VX_ZISF"},
  {0xda, "Vertex ID      ", "VX_VID"},
  {0xd7, "Adjacency Index", "VX_ADJ"},
  {0x53, "Bounding Box   ", "VX_BBX"},
  {0x83, "Coord Trans    ", "VX_TRANMX"},
  {0x6a, "Render option  ", "VX_REND"},
  {0x63, "Color Triple   ", "VX_COLOR"},
  {0x61, "Color Map      ", "VX_CMAP"},
  {0x5a, "Object ID.     ", "VX_ID"},
  {0x57, "ID Vector      ", "VX_IDV"},
  {0x67, "Color Index    ", "VX_CIDX"},
  {0xd0, "Feature Hdr.   ", "VX_GFH"},
  {0x73, "Feature Vec.   ", "VX_GFV"},
  {0xd3, "Distance Vec.  ", "VX_DIST"},
  {0x20, "File History   ", "VX_FHIST"},
  {0x30, "File Name      ", "VX_FNAME"},
  {0x50, "File Machine   ", "VX_FMACH"},
  {0x40, "File Command   ", "VX_FCMND"},
  {0x4a, "File Date      ", "VX_FTIME"},
  {0x3a, "File User ID   ", "VX_FUID"},
  {0xa1, "Pixels (byte)  ", "VX_PBYTE"},
  {0xa2, "Pixels (short) ", "VX_PSHORT"},
  {0xa3, "Pixels (float) ", "VX_PFLOAT"},
  {0xa4, "Pixels (Double)", "VX_PDOUBLE"},
  {0xa5, "Pixels (S-byte)", "VX_PCHAR"},
  {0xa6, "Pixels (Bit)   ", "VX_PBIT"},
  {0xa7, "Pixels (Int)   ", "VX_PINT"},
  {0xb1, "Pixels (Index) ", "VX_PIDX"},
  {0xaa, "Pixel Channels ", "VX_PCHAN"},
  {0xc0, "Generic(text)  ", "VX_GTEXT"},
  {0xc1, "Generic(byte)  ", "VX_GBYTE"},
  {0xc2, "Generic(short) ", "VX_GSHORT"},
  {0xc3, "Generic(float) ", "VX_GFLOAT"},
  {0xc4, "Generic(Double)", "VX_GDOUBLE"},
  {0xc5, "Generic(S-byte)", "VX_GCHAR"},
  {0xc6, "Generic(Bit)   ", "VX_GBIT"},
  {0xc7, "Generic(Int)   ", "VX_GINT"},
  {0xca, "Generic(Scalar)", "VX_GSINT"},
  {0x70, "Text Table(tab)", "VX_TTABLE"},
  {0x80, "Text Table(,)  ", "VX_CTABLE"},
  {0xd1, "ANALYZE 7.5 hdr", "VX_ANHDR"},
  {0xe1, "DICOM P10 hdr  ", "VX_DICOM"},
  {0x60, "Annotation     ", "VX_ANNOTE"},
  {0x00, "Not Defined    ", "VX_NULLT"},
{0, 0, 0}};

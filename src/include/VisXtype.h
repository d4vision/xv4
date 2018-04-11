/* 
 *    VisXtype.h    VisX List Types
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


#define  VX_FRAME	 0x1a /* Image Frame     */
#define  VX_EFRAME	 0x2a /* End Frame       */
#define  VX_COMP	 0x08 /* Compact List    */
#define  VX_V2D	 0x33 /* 2-D vector      */
#define  VX_V3D	 0x43 /* 3-D vector      */
#define  VX_INVY	 0x39 /* Inverted Y      */
#define  VX_ZISF	 0x49 /* Z is Frame      */
#define  VX_VID	 0xda /* Vertex ID       */
#define  VX_ADJ	 0xd7 /* Adjacency Index */
#define  VX_BBX	 0x53 /* Bounding Box    */
#define  VX_TRANMX	 0x83 /* Coord Trans     */
#define  VX_REND	 0x6a /* Render option   */
#define  VX_COLOR	 0x63 /* Color Triple    */
#define  VX_CMAP	 0x61 /* Color Map       */
#define  VX_ID	 0x5a /* Object ID.      */
#define  VX_IDV	 0x57 /* ID Vector       */
#define  VX_CIDX	 0x67 /* Color Index     */
#define  VX_GFH	 0xd0 /* Feature Hdr.    */
#define  VX_GFV	 0x73 /* Feature Vec.    */
#define  VX_DIST	 0xd3 /* Distance Vec.   */
#define  VX_FHIST	 0x20 /* File History    */
#define  VX_FNAME	 0x30 /* File Name       */
#define  VX_FMACH	 0x50 /* File Machine    */
#define  VX_FCMND	 0x40 /* File Command    */
#define  VX_FTIME	 0x4a /* File Date       */
#define  VX_FUID	 0x3a /* File User ID    */
#define  VX_PBYTE	 0xa1 /* Pixels (byte)   */
#define  VX_PSHORT	 0xa2 /* Pixels (short)  */
#define  VX_PFLOAT	 0xa3 /* Pixels (float)  */
#define  VX_PDOUBLE	 0xa4 /* Pixels (Double) */
#define  VX_PCHAR	 0xa5 /* Pixels (S-byte) */
#define  VX_PBIT	 0xa6 /* Pixels (Bit)    */
#define  VX_PINT	 0xa7 /* Pixels (Int)    */
#define  VX_PIDX	 0xb1 /* Pixels (Index)  */
#define  VX_PCHAN	 0xaa /* Pixel Channels  */
#define  VX_GTEXT	 0xc0 /* Generic(text)   */
#define  VX_GBYTE	 0xc1 /* Generic(byte)   */
#define  VX_GSHORT	 0xc2 /* Generic(short)  */
#define  VX_GFLOAT	 0xc3 /* Generic(float)  */
#define  VX_GDOUBLE	 0xc4 /* Generic(Double) */
#define  VX_GCHAR	 0xc5 /* Generic(S-byte) */
#define  VX_GBIT	 0xc6 /* Generic(Bit)    */
#define  VX_GINT	 0xc7 /* Generic(Int)    */
#define  VX_GSINT	 0xca /* Generic(Scalar) */
#define  VX_TTABLE	 0x70 /* Text Table(tab) */
#define  VX_CTABLE	 0x80 /* Text Table(,)   */
#define  VX_ANHDR	 0xd1 /* ANALYZE 7.5 hdr */
#define  VX_DICOM	 0xe1 /* DICOM P10 hdr   */
#define  VX_ANNOTE	 0x60 /* Annotation      */
#define  VX_NULLT	 0x00 /* Not Defined     */

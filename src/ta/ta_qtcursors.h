/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

// file to be included only in ta_qtdata.cc

// TODO: Qt Docs recommend 32x32 cursors as being supported on all platforms

// Record Cursor, 16x16

#define record_cursor_width 16
#define record_cursor_height 16
#define record_cursor_x_hot 1
#define record_cursor_y_hot 1
static uchar record_cursor_bits[] = {
   0x03, 0x00, 0x1f, 0x00, 0x7e, 0x00, 0xfe, 0x01, 0xfe, 0x00, 0x7c, 0x00,
   0xfc, 0x00, 0xd8, 0x01, 0x88, 0x03, 0x00, 0x00, 0xb8, 0x3b, 0xa8, 0x08,
   0xb8, 0x09, 0xa8, 0x08, 0xa8, 0x3b, 0x00, 0x00};

static uchar record_mask_bits[] = {
   0x03, 0x00, 0x1f, 0x00, 0x7e, 0x00, 0xfe, 0x01, 0xfe, 0x00, 0x7c, 0x00,
   0xfc, 0x00, 0xd8, 0x01, 0x88, 0x03, 0x00, 0x00, 0xb8, 0x3b, 0xa8, 0x08,
   0xb8, 0x09, 0xa8, 0x08, 0xa8, 0x3b, 0x00, 0x00};


// Wait Cursor, 16x16

#define wait_cursor_width 16
#define wait_cursor_height 16
#define wait_cursor_x_hot 7
#define wait_cursor_y_hot 8
static uchar wait_cursor_bits[] = {
   0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f, 0x08, 0x10, 0xd0, 0x0b, 0xe0, 0x07,
   0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0x40, 0x02, 0x20, 0x05, 0xd0, 0x0b,
   0xf8, 0x1f, 0xfc, 0x3f, 0x00, 0x00, 0xfc, 0x3f};

static uchar wait_mask_bits[] = {
   0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfc, 0x3f, 0xfc, 0x3f, 0xf8, 0x1f,
   0xf8, 0x1f, 0xf0, 0x0f, 0xf0, 0x0f, 0xf8, 0x1f, 0xf8, 0x1f, 0xfc, 0x3f,
   0xfc, 0x3f, 0xfe, 0x7f, 0xfe, 0x7f, 0xfe, 0x7f};

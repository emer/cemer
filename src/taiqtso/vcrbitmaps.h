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

#ifndef vcrbitmaps_h
#define vcrbitmaps_h

#include "taiqtso_def.h"

#define VCR_BUTTON_WIDTH 32

// full rewind

#define fr_width 12
#define fr_height 12
static char TAIQTSO_API fr_bits[] = {
   0x04, 0x0c, 0x0e, 0x0e, 0x0e, 0x0f, 0x8e, 0x0f, 0xce, 0x0f, 0xee, 0x0f,
   0xee, 0x0f, 0xce, 0x0f, 0x8e, 0x0f, 0x0e, 0x0f, 0x0e, 0x0e, 0x04, 0x0c};

// fast rewind

#define fsr_width 12
#define fsr_height 12
static char TAIQTSO_API fsr_bits[] = {
   0x20, 0x08, 0x30, 0x0c, 0x38, 0x0e, 0x3c, 0x0f, 0xbe, 0x0f, 0xff, 0x0f,
   0xff, 0x0f, 0xbe, 0x0f, 0x3c, 0x0f, 0x38, 0x0e, 0x30, 0x0c, 0x20, 0x08};


// rewind

#define r_width 12
#define r_height 12
static char TAIQTSO_API r_bits[] = {
   0x00, 0x0c, 0x00, 0x0e, 0x00, 0x0f, 0x80, 0x0f, 0xc0, 0x0f, 0xe0, 0x0f,
   0xe0, 0x0f, 0xc0, 0x0f, 0x80, 0x0f, 0x00, 0x0f, 0x00, 0x0e, 0x00, 0x0c};

// forward

#define f_width 12
#define f_height 12
static char TAIQTSO_API f_bits[] = {
   0x03, 0x00, 0x07, 0x00, 0x0f, 0x00, 0x1f, 0x00, 0x3f, 0x00, 0x7f, 0x00,
   0x7f, 0x00, 0x3f, 0x00, 0x1f, 0x00, 0x0f, 0x00, 0x07, 0x00, 0x03, 0x00};

// fast forward

#define fsf_width 12
#define fsf_height 12
static char TAIQTSO_API fsf_bits[] = {
   0x41, 0x00, 0xc3, 0x00, 0xc7, 0x01, 0xcf, 0x03, 0xdf, 0x07, 0xff, 0x0f,
   0xff, 0x0f, 0xdf, 0x07, 0xcf, 0x03, 0xc7, 0x01, 0xc3, 0x00, 0x41, 0x00};

// full forward

#define ff_width 12
#define ff_height 12
static char TAIQTSO_API ff_bits[] = {
   0x03, 0x02, 0x07, 0x07, 0x0f, 0x07, 0x1f, 0x07, 0x3f, 0x07, 0x7f, 0x07,
   0x7f, 0x07, 0x3f, 0x07, 0x1f, 0x07, 0x0f, 0x07, 0x07, 0x07, 0x03, 0x02};



#endif



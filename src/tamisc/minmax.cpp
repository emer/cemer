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

// minmax.cc

#include <minmax.h>

/* from xmgr, graphutils.c, copyright P. Turner
 * nicenum: find a "nice" number approximately equal to x
 * round if round=true, ceil if round=false
 */

double nicenum(double x, bool round) {
  double y;
  if(x <= 0.0)
     return 0.0;
  int exp = (int)floor(log10(x));
  double f = x / pow(10.0, (double) exp);	/* fraction between 1 and 10 */
  if (round)
    if (f < 1.5)
      y = 1.;
    else if (f < 3.)
      y = 2.;
    else if (f < 7.)
      y = 5.;
    else
      y = 10.;
  else if (f <= 1.)
    y = 1.;
  else if (f <= 2.)
    y = 2.;
  else if (f <= 5.)
    y = 5.;
  else
    y = 10.;
  return y * pow(10.0, (double)exp);
}


void FixedMinMax::Initialize() {
  min = max = 0.0f;
  fix_min = fix_max = false;
}

void FixedMinMax::Copy_(const FixedMinMax& cp) {
  min = cp.min; max = cp.max;
  fix_min = cp.fix_min; fix_max = cp.fix_max;
}

void Modulo::Initialize() {
  flag = true; m = 1; off = 0;
}

void Modulo::UpdateAfterEdit() {
  taOBase::UpdateAfterEdit();
  if(owner != NULL) owner->UpdateAfterEdit();
}

void Modulo::Copy_(const Modulo& cp) {
  flag = cp.flag; m = cp.m; off = cp.off;
}


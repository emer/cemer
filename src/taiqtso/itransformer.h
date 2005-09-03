/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the PDP++ software package.			      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, and modify this software and its documentation    //
// for any purpose other than distribution-for-profit is hereby granted	      //
// without fee, provided that the above copyright notice and this permission  //
// notice appear in all copies of the software and related documentation.     //
//									      //
// Permission to distribute the software or modified or extended versions     //
// thereof on a not-for-profit basis is explicitly granted, under the above   //
// conditions. 	HOWEVER, THE RIGHT TO DISTRIBUTE THE SOFTWARE OR MODIFIED OR  //
// EXTENDED VERSIONS THEREOF FOR PROFIT IS *NOT* GRANTED EXCEPT BY PRIOR      //
// ARRANGEMENT AND WRITTEN CONSENT OF THE COPYRIGHT HOLDERS.                  //
// 									      //
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
// itransform.h -- Iv/Qt-compatible transformation class

/*
  The iTransformer class provides an interface compatible with the Iv Transform class.
  It is implemented on top of the Qt QWMatrix class, which is similar to the Iv class.

*/
#ifndef ITRANSFORMER_H
#define ITRANSFORMER_H

#include "taiqtso_def.h"

class TAIQTSO_API iTransformer {
public:
  iTransformer();
//  iTransformer(const Transformer*);
  iTransformer (float a00, float a01, float a10, float a11, float a20, float a21);
  ~iTransformer();

  iTransformer(const iTransformer& val);
  iTransformer& operator=(const iTransformer& val);

  bool identity() const;
  void premultiply(const iTransformer&);
  void postmultiply(const iTransformer&);
  void invert();
  void translate(float dx, float dy);
  void scale(float sx, float sy);
  void rotate(float angle);
  void skew(float sx, float sy);
//  void transform(floatx, floaty);
//  void transform(float x, float y, floattx, floatty);
//  void inverse_transform(floatx, floaty);
//  void inverse_transform(float tx, float ty, floatx, floaty);
  void matrix(float a00, float a01, float a10, float a11, float a20, float a21);

  iTransformer(const QWMatrix& val);
  iTransformer& operator=(const QWMatrix& val);
//  operator QWMatrix() const;
protected:
  QWMatrix* mat; // actual font data is encapsulated
};

#endif

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
// ifont.h -- Qt and Inventor-compatible font class

#ifndef IFONT_H
#define IFONT_H

#include "ta_def.h"
#include "ta_string.h"

#ifdef TA_GUI
class QFont; // #IGNORE
#endif

#ifdef TA_USE_INVENTOR
class SoFont; // #IGNORE
#endif


class TA_API iFont { // #NO_TOKENS
public:
  enum Attributes { // #BITS
    Bold		= 0x01,
    Italic		= 0x02,
    Underline		= 0x04
  };

  String 	fontName; // family name of the font, such as 'Helvetica' or 'Arial'
  Attributes	attribs; // #BITS font modifiers
  float		pointSize; // point size of the font -- will be aproximated in 3D
  int		stretch; // #DETAIL #DEF_100 the degree of stretch, less is narrower

  bool 		bold() const {return  (attribs && Bold);}
  void 		setBold(bool value);
  bool 		italic() const {return  (attribs && Italic);}
  void 		setItalic(bool value);
  void		setPointSize(float value);
  void 		setPointSize(int value);
  void 		setStretch(int value);
  bool 		underline() const {return (attribs && Underline);}
  void 		setUnderline(bool value);

  void		copy(const iFont& src);

  iFont& operator=(const iFont& val); // must use value copy

  iFont();
  iFont(const iFont& val); // must use value copy
  ~iFont();

#ifdef TA_GUI
  void		copyFrom(const QFont& src); //#IGNORE
  void		copyTo(QFont& dst) const; //#IGNORE
  void		setRawName(const char* raw_name);
  iFont(const char* raw_name); //Iv-compatible constructor TODO: this s/b nuked!
  iFont(const QFont& val);
  iFont& operator=(const QFont& val);
  operator QFont() const;
#endif

#ifdef TA_USE_INVENTOR
  void		copyTo(SoFont* dst, float pts_per_unit = 36.0f) const; //#IGNORE
#endif
protected:
  static String defName; // #IGNORE
  void		init(); // #IGNORE
};

#endif


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
// imisc_so.h -- misc Inventor helpers

#ifndef IMISC_SO_H
#define IMISC_SO_H

#include "taglobal.h"
#include "igeometry.h"

/* T3RenderHelper
 * This class is used to help rendering, since you almost always require to issue a series
 * of translations when drawing objects -- this helper lets you think and render in terms
 * of the absolute coordinate of an element with respect to the starting frame.
 *
 * Example usage:

 T3RenderHelper rh(par_so);
 SoNode* nd1 = //create some shape node
 rh.render(nd1);
 // now draw a second instance, 2 units right
 rh.render(nd1, 2, 0, 0);

*/

class T3RenderHelper { // #IGNORE class used to help rendering -- issues the translation transforms
public:
  iVec3f	cur; // current position (initially 0,0,0)
  SoGroup*	par; // default parent
  void		moveTo(float x, float y, float z); // move to (maybe) new position
  void		render(SoNode* node); // render at current point (no translation)
  void		render(SoNode* node, float x, float y, float z); // render at new position
  void 		setOrigin(float x, float y, float z); // used to (re)set origin -- doesn't issue a transform
  void		setParent(SoGroup* par_)  {par = par_;}
  T3RenderHelper(SoGroup* par_ = NULL) {par = par_;}
protected:
  void		checkIssueTransform(float x, float y, float z);
};


#endif

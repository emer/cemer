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

// css_qt.h: Qt-css interface

#ifndef CSS_QTDATA_H
#define CSS_QTDATA_H

#include "qtdefs.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"

// forward declarations
class cssClassInst;
class cssMbrScriptFun;
class cssEnumType;
class cssProgSpace;


//////////////////////////////
//   taiData for css       //
//////////////////////////////

class cssiPolyData : public taiData {
  // supports INLINE members for css
public:
  cssClassInst*	obj;
  taiDataList 	data_el;
  taiType_List type_el;	// type elements (not stored on classes, so kept here)
  int		show;


  cssiPolyData(cssClassInst* ob, TypeDef* typ_, taiDataHost* host_, taiData* par,
      QWidget* gui_parent, int flags_ = 0);
  ~cssiPolyData();

  QFrame*	rep() const { return (QFrame*)m_rep; } //note: actual class may be subclass of QFrame

  virtual void  GetImage(void* base);
  virtual void	GetValue(void* base);

protected:
  virtual void	Constr(QWidget* gui_parent_);
};


class cssiMethMenu : public taiMethMenu {
  // css member functions of css classes
  Q_OBJECT
public:
  cssClassInst*		obj;		// parent object
  cssProgSpace*		top;		// top-level progspace where edit was called
  cssMbrScriptFun*	css_fun; 	// the function itself
  cssClassInst*		arg_obj; 	// argument object (if necc)

  cssiMethMenu(cssClassInst* ob, cssProgSpace* tp, cssMbrScriptFun* cfn,
	TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent, int flags_ = 0);
//  ~cssiMethMenu();

  void		ShowReturnVal(cssEl* rval);
  void		ApplyBefore();	// apply changes before performing menu call
  void		UpdateAfter();	// update display after performing menu call

  void		GenerateScript(); // output script code equivalent if recording

  void		AddToMenu(taiMenu* menu);

public slots:
  void		CallFun();	// (override) call the function..
};


#endif // CSS_QTDATA_H

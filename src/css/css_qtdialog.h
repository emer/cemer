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

#ifndef CSS_QTDIALOG_H
#define CSS_QTDIALOG_H

#include "ta_qttype.h"
#include "ta_qtdialog.h"
#include "css_qttype.h"


class cssClassInst;
class cssMbrScriptFun;
class cssEnumType;
class cssProgSpace;
class cssClassType;
class taiType_List;
class cssClassInst;

class cssiEditDialog : public taiEditDataHost {
  // edit dialog for editing css classes
public:
  cssClassInst*	obj;		// class object to edit
  cssProgSpace*	top;		// top-level progspace where edit was called
  taiType_List type_el;	// type elements (not stored on classes, so kept here)

  cssiEditDialog(cssClassInst* ob, cssProgSpace* tp = NULL, bool read_only_ = false, bool modal_ = false,
  	QObject* parent = 0);
  cssiEditDialog()		{ };
  ~cssiEditDialog();

  override void		GetImage();
  override void		GetValue();
  override int		Edit(bool modal_ = false); // ati is for when leading argument(s) are predetermined, and user shouldn't be prompted for them

//   void		Constr_Methods(); // construct the methods (buttons and menus)
// NN  void		Constr_Body();

//  void		GetMenuRep(MethodDef* md) { taiEditDialog::GetMenuRep(md); }
//  void		GetMenuRep(cssMbrScriptFun* md);

  static cssiType*	GetTypeFromEl(cssEl* el, bool read_only);
  // this decodes cssEl types into cssiTypes, which are then used to render edit
protected:
  void	 		GetName(int idx, cssEl* md, String& name, String& desc);
  override void		ClearBody_impl(); // delete the data items
  override void		Constr_Labels();
  override void		Constr_Data(); // construct the data and label elements of the dialog
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
};


///////////////////////////
//    cssiArgDialog       //
///////////////////////////

class cssiArgDialog : public cssiEditDialog {
  // #IGNORE edit dialog for editing function arguments
public:
  bool		err_flag; 	// true if an arg was improperly set
  MethodDef*	md;
  TypeDef*	typ;
  void*		base;
  int		use_argc;
  int 		hide_args; // number of leading args to hide in dialogs to user (because context has already predetermined them)

  cssiArgDialog(MethodDef* md, TypeDef* typ, void* base, int use_argc, int hide_args,
    bool read_only_ = false, bool modal_ = true, QObject* parent = 0);
  cssiArgDialog()		{ };
  ~cssiArgDialog();

  void		Constr_ArgTypes();
  void		GetImage();
  void		GetValue();
  int		Edit(bool modal_); // ati is for when n leading args are predetermined by context, and so shouldn't be shown to the user

  void		Ok();


  taiArgType*	GetBestArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
protected:
  override void		Constr_impl();
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void		Constr_Data(); // construct the static elements of the dialog
  override void		Constr_Labels(); // construct the labels of the dialog
};


#endif // CSS_QTDIALOG_H

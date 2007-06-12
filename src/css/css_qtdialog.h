// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.


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

class CSS_API cssiEditDialog : public taiEditDataHost {
  // edit dialog for editing css classes
public:
  static cssiType*	GetTypeFromEl(cssEl* el, bool read_only);
  // this decodes cssEl types into cssiTypes, which are then used to render edit
  
  cssClassInst*		obj;	// class object to edit
  cssProgSpace*		top;	// top-level progspace where edit was called
  taiType_List 		type_el; // type elements (not stored on classes, so kept here)

  cssiEditDialog(cssClassInst* ob, cssProgSpace* tp = NULL,
    bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
//nn  cssiEditDialog() {obj=NULL; top=NULL;} // not really used
  ~cssiEditDialog();

  override void		GetImage_Membs();
  override void		GetValue_Membs();
  override int		Edit(bool modal_ = false); // ati is for when leading argument(s) are predetermined, and user shouldn't be prompted for them

protected:
  void	 		GetName(int idx, cssEl* md, String& name, String& desc);
  override void		ClearBody_impl(); // delete the data items
  override void		Constr_Inline() {} // n/a
  override void		Constr_Data_Labels(); // construct the data and label elements of the dialog
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
private:
  void	Initialize();
};


///////////////////////////
//    cssiArgDialog       //
///////////////////////////

class CSS_API cssiArgDialog : public cssiEditDialog {
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
//nn  cssiArgDialog()		{ };
  ~cssiArgDialog();

  void		Constr_ArgTypes();
  void		GetImage(bool force); // force ignored
  void		GetValue();
  int		Edit(bool modal_); // ati is for when n leading args are predetermined by context, and so shouldn't be shown to the user

  void		Ok();


  taiArgType*	GetBestArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
protected:
  override void		Constr_impl();
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void		Constr_Data_Labels(); // construct the static elements of the dialog
};


#endif // CSS_QTDIALOG_H

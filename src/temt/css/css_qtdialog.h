// Copyright 2006-2017, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#include "css_qttype.h"

#include <taiEditorOfClass>
#include <taiType_List>

class cssClassInst;
class cssMbrScriptFun;
class cssEnumType;
class cssProgSpace;
class cssClassType;
class cssClassInst;
class taiArgType;
class iCodeCompleter;

class CSS_API cssiEditDialog : public taiEditorOfClass {
  // edit dialog for editing css classes
public:
  static cssiType*	GetTypeFromEl(cssEl* el, bool read_only);
  // this decodes cssEl types into cssiTypes, which are then used to render edit
  
  cssClassInst*		obj;	// class object to edit
  cssProgSpace*		top;	// top-level progspace where edit was called
  taiType_List 		type_el; // type elements (not stored on classes, so kept here)

  cssiEditDialog(cssClassInst* ob, cssProgSpace* tp = NULL,
    bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  ~cssiEditDialog();

  void		GetImage_Membs() override;
  void		GetValue() override;
  int		  Edit(bool modal_ = false, int min_width=-1, int min_height=-1) override;
  // ati is for when leading argument(s) are predetermined, and user shouldn't be prompted for them

protected:
  void	 	GetName(int idx, cssEl* md, String& name, String& desc);
  void		ClearBody_impl() override; // delete the data items
  void		Constr_Inline() override {} // n/a
  void		Constr_Widget_Labels() override; // construct the data and label elements of the dialog
  void		Constr_Strings() override;
private:
  void	Initialize();
};


///////////////////////////
//    cssiArgDialog       //
///////////////////////////

class CSS_API cssiArgDialog : public cssiEditDialog {
  // #IGNORE edit dialog for editing function arguments
INHERITED(cssiEditDialog)
public:
  bool                  err_flag; 	// true if an arg was improperly set
  MethodDef*            md;
  TypeDef*              typ;
  int                   use_argc;
  int                   hide_args; // number of leading args to hide in dialogs (context has predetermined them)
  iCodeCompleter*       code_completer;  

  cssiArgDialog(MethodDef* md, TypeDef* typ, void* base, int use_argc, int hide_args,
    bool read_only_ = false, bool modal_ = true, QObject* parent = 0);
  ~cssiArgDialog();

  using inherited::GetImage;
  void        GetImage(bool force) override; // force ignored
  void        GetValue() override;
  String      GetArgValue(int index);  // index is based on visible args
  int         Edit(bool modal_ = false, int min_width=-1, int min_height=-1) override;
  // ati is for when n leading args are predetermined by context, and so shouldn't be shown to the user
  taBase*     GetBaseForArg(const String& arg_name);

  void        Ok() override;

protected:
  void		Constr_impl() override;
  void		Constr_Strings() override;
  void		Constr_Widget_Labels() override; // construct the static elements of the dialog
};


#endif // CSS_QTDIALOG_H

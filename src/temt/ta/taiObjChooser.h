// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef taiObjChooser_h
#define taiObjChooser_h 1

// parent includes:
#include "ta_def.h"
#include <iDialog>

// member includes:
#include <taString>
// smartptr, ref includes
#include <taBase>
#include <taSmartRefT>
#include <taSmartPtrT>
#include <String_Array>

// declare all other types mentioned but not required to include:
class taList_impl;
class taBase;


class TA_API taiObjChooser: iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS select objects from a list, much like a file chooser.  can be tokens from typedef or items on a list
  Q_OBJECT
  INHERITED(iDialog)
public:
  static taiObjChooser* createInstance(taBase* parob, const char* captn, bool selonly = true, QWidget* par_window_ = NULL);
    // create method for lists/groups
  static taiObjChooser* createInstance(TypeDef* tpdf, const char* captn, taBase* scope_ref_ = NULL, QWidget* par_window_ = NULL);
    // create method for tokens

  bool                  select_only;    // if true, only for selecting objects from current parent object
  String                caption;        // current caption at top of chooser
  String                path_str;       // current path string
  taList_impl*          lst_par_obj;    // parent object that is a list object
  taBase*               reg_par_obj;    // parent object that is *not* a list object
  TypeDef*              typ_par_obj;    // parent object that is a typedef (get tokens)
  taBase*               scope_ref;      // reference object for scoping

  taBase*               sel_obj() const {return msel_obj;}// current selected object
  void                  setSel_obj(const taBase* value);        //
  String                sel_str() const {return msel_str;}      // string rep of current selection
  String_Array          items;          // the items in the list

  QGridLayout*          layOuter;
  QListWidget*          browser;        // list of items
  QLineEdit*            editor;
  QHBoxLayout*          layButtons;
  QPushButton*          btnOk;
  QPushButton*          btnCancel;

  taiObjChooser(taBase* parob, const char* captn, bool selonly, QWidget* par_window_); // USE createInstance instead!!!
  taiObjChooser(TypeDef* tpdf, const char* captn, taBase* scope_ref_, QWidget* par_window_); // USE createInstance instead!!!

  virtual bool  Choose();
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void  Build();        // called as constructed
  virtual void  Clear();        // reset data
  virtual void  Load();         // reload data
  virtual void  ReRead();       // update browser for new parent

  virtual void  GetPathStr();   // get current path string
  virtual void  AddItem(const char* itm, const void* data_ = NULL); // add one item to dialog
  virtual void  UpdateFmSelStr(); // update selection based on sel_str

protected:
  taBase*                       msel_obj;       // current selected object
  String                msel_str;       // string rep of current selection

  void          init(const char* captn, bool selonly, QWidget* par_window_); // called by constructors
  virtual void  AcceptEditor_impl(QLineEdit* e);
  void          AddObjects(taBase* obj);
  void          AddTokens(TypeDef* td); // add all tokens of given type

protected slots:
  void accept(); // override
  void reject(); // override
  // callbacks
  void          browser_currentItemChanged(QListWidgetItem* itm, QListWidgetItem* prv);
  void          browser_itemDoubleClicked(QListWidgetItem* itm);
  virtual void  DescendBrowser();
  virtual void  AcceptEditor();
};

#endif // taiObjChooser_h

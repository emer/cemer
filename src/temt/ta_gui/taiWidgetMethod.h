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

#ifndef taiWidgetMethod_h
#define taiWidgetMethod_h 1

// parent includes:
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:
class taiWidgetActions;
class cssClass;                 // #IGNORE
class cssiArgDialog;            // #IGNORE

// unlike real taiWidget, functions are not subject to updating
// so the constructor is the one that does all the work..

class TA_API taiWidgetMethod : public taiWidget {
  // all representations of member functions must inherit from this one -- note, the CallFun calls can make an inst of this guy
  Q_OBJECT
public:
  void*         base;           // base is of parent type
  MethodDef*    meth;
  bool          is_menu_item;   // true if this is supposed to be a menu item (else QWidget*)
  cssClass*     args;
  cssiArgDialog* arg_dlg;
  int           use_argc;

  taiWidgetMethod(void* bs, MethodDef* md, TypeDef* typ_, IWidgetHost* host, taiWidget* par,
      QWidget* gui_parent_, int flags_ = 0);

  virtual QWidget*      GetButtonRep() {return buttonRep;}
  // button connected to method -- only created on demand by subclasses

  virtual bool  UpdateButtonRep();
  // if needed, can update the button representation -- default does GHOST stuff

  virtual bool  hasButtonRep() { return (buttonRep != NULL); }

  virtual void  GenerateScript(); // output script code equivalent if recording

  virtual void  AddToMenu(taiWidgetActions* mnu);

public slots:
  virtual void  CallFun() {CallFun_impl();} // call the function (button callback)

protected:
  static void   ShowReturnVal(cssEl* rval, IWidgetHost* host,
     const String& meth_name); // show return value after menu call

#ifndef __MAKETA__
  QPointer<QWidget> buttonRep;
#endif
  QWidget*      gui_parent;
  QWidget*      MakeButton(); // makes the button if necessary, and returns a reference

  bool          CallFun_impl();         // impl -- we could delete partway through, returns true if we still exist!
// note that some situations can cause this guy to delete during processing
  void          ApplyBefore();  // apply changes before performing menu call
  void          UpdateAfter();  // update display after performing menu call
};

#endif // taiWidgetMethod_h

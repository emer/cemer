// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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

// ta_qtdata.h: Qt-based graphical data representations

#ifndef TA_QTDATA_DEF_H
#define TA_QTDATA_DEF_H

#include "qtdefs.h"
#include "ta_base.h"

#ifndef __MAKETA__
#include <qobject.h>
#include <qwidget.h>
#endif

//??nn in win32  #include <unistd.h>


// Forward declarations

class taiData;
class taiDataHost;
class taiAction;
class taiMenuAction;
class taiMenuEl;
class taiMenu;
class iAction;
class iAction_List;
class taiMethodData;

class QWidget;


//////////////////////////////////////////////////////////
// 	taiAction: wrapper for Qt callbacks		//
//////////////////////////////////////////////////////////

class taiAction {
// taiAction is for generic parameterless callbacks
public:
  QObject* 	receiver;
  String	member;

  taiAction() {receiver = NULL;}
  taiAction(QObject* receiver_, const char* member_) {receiver = receiver_; member = member_;}
  taiAction(const taiAction& src) {receiver = src.receiver; member = src.member;}

  void		connect(QObject* sender, const char* signal) const;
  taiAction& 	operator=(const taiAction& rhs);
};

#define TAI_ACTION_INSTANCE(x,y) \
x(): y() { }        				\
x(QObject* receiver_, const char* member_) 	\
  : y(receiver_, member_) 	{ }					\
x(x& src): y(src) {}			\
x& operator=(const x& rhs) {y::operator=(rhs); return *this;}			\

class taiMenuAction: public taiAction {
//taiMenuAction takes a MenuEl as a parameter
public:
  TAI_ACTION_INSTANCE(taiMenuAction, taiAction)
};

//////////////////////////////////////////////////////////
// 	taiData: objects to wrap gui widgets for data	//
//////////////////////////////////////////////////////////

// NOTE: see ta_qtviewer.h for clipboard handling details.

class taiData: public QObject {
  // #INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for data elements
  Q_OBJECT
  friend class taiCompData; // for parenting
#ifndef __MAKETA__
typedef QObject inherited;
#endif
public:
  enum Flags { // flags are so we don't have to keep adding parameters to constructor...
    flgReadOnly		= 0x001,
    flgPosOnly		= 0x002, // used by SpinEdit to limit to non-negative values only
    flgNullOk		= 0x004, // used typically by menus of tokens or objects, to allow NULL
    flgEditOk		= 0x008,  // used typically by menus to allow Edit of item
    flgNoList		= 0x010,  // used typically by menus to include lists of item
    flgNoGroup		= 0x010,  // used typically by menus to include groups of items -- note this is the same as NoList
    flgNoInGroup	= 0x020,  // used by gpiGroupEls
    flgEditOnly		= 0x040  // used by EditButton
  };

  TypeDef* 		typ;		// type for the gui object
  taiDataHost*		host;		// dialog or edit panel that this belongs to (optional)
  String		orig_val;	// text of original data value
  taiData*		parent() {return mparent;} // #GET_Parent if data is contained within data, this the parent container
  void			setParent(taiData* value); // #SET_Parent
  taiData(); // for ta_TA.cc only
  taiData(TypeDef* typ_, taiDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags_ = 0);
  virtual ~taiData();

  int			defSize();		// default taiMisc::SizeSpec value, for sizing controls (taken from parent, else dlg, else "default")
  void			emit_UpdateUi();
  virtual bool		isConstructed();	// true if our parents (ex dialog) are fully constructed
  bool			hiBG() { return mhiBG; }	// #GET_HiBG  background highlight, if applicable -- default is suitable for text-like or bool controls;
  virtual void		setHiBG(bool value);	// #SET_HiBG
  virtual bool		useHiBG() {return museHiBG;}
  virtual void		setUseHiBG(bool value) {}	// enable background highlight, if applicable -- control must override to implement
  virtual bool		readOnly();	// #GET_ReadOnly true if the control should be read only -- partially delegates to parent

  bool 			eventFilter(QObject* watched, QEvent* ev); // override
  virtual QWidget*	GetRep()	{ return m_rep; }
  bool			HasFlag(int flag_) {return (mflags & flag_);} // returns true if has the indicated Flag (convenience method)
  void			SetFlag(int flags_, bool value = true) {if (value) mflags |= flags_; else mflags &= ~flags_;} // sets or clears a flag or set of flags

  void			SetThisAsHandler(bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset)
  void			DataChanged(taiData* chld = NULL);
  // indicates something changed in the data from user input, if chld is passed then it called parent->DataChanged(this); ignored if parent or ourself is not fully constructed

#ifndef __MAKETA__
signals:
  void 			settingHiBG(bool value);
  void			UpdateUi(); // cliphandler callback, to get it to requery the ui items
#endif

protected:
  bool			mhiBG;
  bool			museHiBG;	// true if control should attempt to use background hilighting
  QWidget*		m_rep;		// widget that represents the data
  taiData*		mparent;		// if data is contained within data, this the parent container
  int			mflags;
  void 			emit_settingHiBG(bool value); // emits the settingHiBg signal
  virtual void		setHiBG_impl(bool value); // can be overriden by subclasses that handle HiBG; defaults to emitting signal
  virtual void		SetRep(QWidget* val);
  virtual void		ChildAdd(taiData* child) {}
  virtual void		ChildRemove(taiData* child) {}
  virtual void		DataChanged_impl(taiData* chld) {} // only called if isConstructed;
protected slots:
  void 			repChanged(); //signal from rep that data has changed
  virtual void 		repDestroyed(QObject* obj); // available if instance wants to insure its rep member is not invalid -- not connected by default, but class can override SetRep conveniently to connect

  // handler controls do nothing in base class, and in most subclasses
  virtual void 		this_GetEditActionsEnabled(int& ea) {} // for when control is clipboard handler
  virtual void 		this_EditAction(int param) {} // for when control is clipboard handler
  virtual void 		this_SetActionsEnabled() {} // for when control is clipboard handler

};

//////////////////////////
//    taiDataList	//
//////////////////////////

class taiDataList : public taPtrList<taiData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS taiData list, OBJECTS ARE DELETED ON REMOVAL
protected:
//  void*		El_Ref_(void* it)	{ taRefN::Ref((taiData*)it); return it; }
//  void* 	El_unRef_(void* it)	{ taRefN::unRef((taiData*)it); return it; }
  void		El_Done_(void* it);

public:
  ~taiDataList()                       { Reset(); }
};

// note: we define taiEditButton here, because it is needed for all the group buttons,
// and we don't want to drag the main ta_qtdata into the defs of group datas

//////////////////////////////////
// 	taiEditButton		//
//////////////////////////////////

class taiEditButton : public taiData {
  // actually an edit menu... -- flgReadOnly creates menu which only allows for #EDIT_READ_ONLY members
  Q_OBJECT
public:
  void*		cur_base;
  taiMenu*	imenu;
  taiEdit*	ie;
  taiDataList 	meth_el;	// method elements

  taiEditButton(void* base, taiEdit *taie, TypeDef* typ_, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgReadOnly, flgEditOnly -- internally sets flgEditOnly if appropriate
  ~taiEditButton();

  QPushButton*	rep() { return (QPushButton*)m_rep; }
  taiMenu*	menu() {return imenu;}
  virtual void	GetMethMenus();

  virtual void	GetImage(void* base);
  virtual void	SetLabel();

protected:
  QWidget*	mgui_parent;

public slots:
  virtual void	Edit();		// edit callback
  void setRepLabel(const char* label);
};

#endif


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


// ta_qtdata.h: Qt-based graphical data representations

#ifndef TA_QTDATA_DEF_H
#define TA_QTDATA_DEF_H

#include "ta_base.h"
#include "ta_variant.h"

#ifndef __MAKETA__
#include <qobject.h>
#include <qwidget.h>
#endif

//??nn in win32  #include <unistd.h>

// externals
class iMainWindowViewer; //

// Forward declarations

class taiData;
class IDataHost; // interface for top level host of data items, mostly for DataChanged
class taiDataHost;
class taiAction;
class taiMenuAction;
class taiMenu;
class taiMethodData;

class QWidget;


//////////////////////////////////////////////////////////
// 	taiMenuAction: wrapper for Qt callbacks		//
//////////////////////////////////////////////////////////

class TA_API taiMenuAction { // object that holds specs for receiver for Action(taiAction*) callback
public:
  QObject* 	receiver;
  String	member;

  taiMenuAction() {receiver = NULL;}
  taiMenuAction(QObject* receiver_, const char* member_) {receiver = receiver_; member = member_;}
  taiMenuAction(const taiMenuAction& src) {receiver = src.receiver; member = src.member;} //

//  void		connect(QObject* sender, const char* signal) const; // #IGNORE
  taiMenuAction& 	operator=(const taiMenuAction& rhs);
};


class TA_API IDataHost: public ITypedObject { // #VIRT_BASE #NO_INSTANCE #NO_CSS
public:
  virtual const iColor* colorOfCurRow() const = 0; // #IGNORE background color of cur row
  virtual bool		isConstructed() = 0;
  virtual bool		isModal() = 0;
  virtual bool		isReadOnly() = 0;
  virtual iMainWindowViewer* viewerWindow() const {return NULL;} // used to set cliphandler
  virtual bool		HasChanged() = 0; // 'true' if has changed
  virtual void*		Base() = 0; // base of the object
  virtual TypeDef*	GetBaseTypeDef() = 0; // TypeDef on the base, for casting
  virtual void		GetValue() = 0; // copy gui to value
  virtual void		GetImage() = 0; // copy value to gui
  virtual void		Changed() {} // called by embedded item to indicate contents have changed
  virtual void		SetItemAsHandler(taiData* item, bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset); in ta_qtviewer.cpp

  IDataHost() {}
  virtual ~IDataHost() {} //
};

//////////////////////////////////////////////////////////
// 	taiData: objects to wrap gui widgets for data	//
//////////////////////////////////////////////////////////

// NOTE: see ta_qtviewer.h for clipboard handling details.
/*
  Use of GetImage_ and GetValue_ (and _impl routines)
  
  The GetImage_ and GetValue_ provide a generic way for strongly typed data manipulators to
  get/set data values in a generic way. Not every subclass of taiData provides the generic
  implementations. However, every subclass of taiCompData, which is used for all inline
  editing of compound data, must provide an implementation.
  
  taiData classes that take/provide strongly typed atomic data, such as int's, bool's,
  and strings use "void* base" to refer to the address of an instance of that type,
    ex.  *((int*)base) is an int reference
  
  taiData classes that take/provide pointers to instances of classes, ex. TypeDef* use
  the void* base pointer to refer to an instance of the class
    ex.  (TypeDef*)base refers to a TypeDef 
   
  taiMember and derived classes use "void* base" to refer to the address of the member
*/

class TA_API taiData: public QObject {
  // #INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for data elements
  Q_OBJECT
  friend class taiCompData; // for parenting
#ifndef __MAKETA__
typedef QObject inherited;
#endif
public:
  enum Flags { // flags are so we don't have to keep adding parameters to constructor... flags >= 10000 are custom per type
    flgReadOnly		= 0x001,
    flgNullOk		= 0x004, // used typically by menus of tokens or objects, to allow NULL
    flgEditOk		= 0x008,  // used typically by menus to allow Edit of item
    flgNoList		= 0x010,  // used typically by menus to include lists of item
    flgNoGroup		= 0x010,  // used typically by menus to include groups of items -- note this is the same as NoList
    flgNoInGroup	= 0x020,  // used by gpiGroupEls
    flgEditOnly		= 0x040,  // used by EditButton
    flgInline		= 0x080,   // used by types and members that support INLINE directive, esp. Array
    flgEditDialog	= 0x100,   // for taiField, enables dialog for EDIT_DIALOG directive; for token menu, adds ... edit
    flgNoUAE		= 0x200  // for things like polydata, don't issue an UpdateAfterEdit
  };

  TypeDef* 		typ;		// type for the gui object
  IDataHost*		host;		// dialog or edit panel that this belongs to (optional)
  String		orig_val;	// text of original data value
  taiData*		parent() {return mparent;} // #GET_Parent if data is contained within data, this the parent container
  void			setParent(taiData* value); // #SET_Parent
  virtual void		Delete(); // use this to dynamically delete this, including the rep
  taiData(); // for ta_TA.cc only
  taiData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags_ = 0);
  virtual ~taiData();

  int			defSize() const;		// default taiMisc::SizeSpec value, for sizing controls (taken from parent, else dlg, else "default")
  void			emit_UpdateUi();
  virtual bool		isConstructed();	// true if our parents (ex dialog) are fully constructed
  bool			highlight() const { return mhighlight; }	// #GET_highlight  changed highlight
  virtual void		setHighlight(bool value);	// #SET_Highlight
  virtual bool		readOnly();	// #GET_ReadOnly true if the control should be read only -- partially delegates to parent
  virtual bool		fillHor() {return false;} // override to true to fill prop cell, ex. edit controls
  bool 			eventFilter(QObject* watched, QEvent* ev); // override
  virtual QWidget*	GetRep()	{ return m_rep; }
  virtual QLayout*	GetLayout() {return NULL;} // returns a top layout, if one is used
  bool			HasFlag(int flag_) const {return (mflags & flag_);} // returns true if has the indicated Flag (convenience method)
  void			SetFlag(int flags_, bool value = true) {if (value) mflags |= flags_; else mflags &= ~flags_;} // sets or clears a flag or set of flags

  void			SetThisAsHandler(bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset)
  void			DataChanged(taiData* chld = NULL);
  // indicates something changed in the data from user input, if chld is passed then it called parent->DataChanged(this); ignored if parent or ourself is not fully constructed

  void			GetImage_(const void* base) {GetImage_impl(base);} // base points to value of type
  void			GetValue_(void* base) const {GetValue_impl(base);} // base points to value of type
  
  void			GetImageVar_(const Variant& val) {GetImageVar_impl(val);} // get as Var, mostly used by css
  void			GetValueVar_(Variant& val) const {GetValueVar_impl(val);} // set as Var, mostly used by css
  
  iLabel*		MakeLabel(QWidget* gui_parent = NULL, int font_spec = 0) const; // convenience func, sets fonts etc.
  iLabel*		MakeLabel(const String& text, QWidget* gui_parent = NULL,
    int font_spec = 0) const; // convenience func, sets fonts etc.
  
#ifndef __MAKETA__
signals:
  bool 			settingHighlight(bool setting); // invoked when highlight state changes
  void			UpdateUi(); // cliphandler callback, to get it to requery the ui items
#endif

protected:
  bool			mhighlight;
  QWidget*		m_rep;		// widget that represents the data
  taiData*		mparent;		// if data is contained within data, this the parent container
  int			mflags;
  virtual void		SetRep(QWidget* val);
  virtual void		ChildAdd(taiData* child) {}
  virtual void		ChildRemove(taiData* child) {}
  virtual void		DataChanged_impl(taiData* chld) {} // only called if isConstructed
  virtual void		GetImage_impl(const void* base) {}
  virtual void		GetValue_impl(void* base) const {} 
  virtual void		GetImageVar_impl(const Variant& val) {}
  virtual void		GetValueVar_impl(Variant& val) const {} 
  
protected slots:
  void 			repChanged(); //signal from rep that data has changed
  virtual void 		repDestroyed(QObject* obj); // available if instance wants to insure its rep member is not invalid -- not connected by default, but class can override SetRep conveniently to connect

  // handler controls do nothing in base class, and in most subclasses
  virtual void 		this_GetEditActionsEnabled(int& ea) {} // for when control is clipboard handler
  virtual void 		this_EditAction(int param) {} // for when control is clipboard handler
  virtual void 		this_SetActionsEnabled() {} // for when control is clipboard handler

};


class TA_API taiDataList : public taPtrList<taiData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS taiData list, OBJECTS ARE DELETED ON REMOVAL
protected:
//  void*		El_Ref_(void* it)	{ taRefN::Ref((taiData*)it); return it; }
//  void* 	El_unRef_(void* it)	{ taRefN::unRef((taiData*)it); return it; }
  void		El_Done_(void* it);

public:
  ~taiDataList()                       { Reset(); }
};


class TA_API Member_List : public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS list of members -- simpler than MemberSpace, no ownership
public:
  ~Member_List()            { Reset(); }
};


#endif


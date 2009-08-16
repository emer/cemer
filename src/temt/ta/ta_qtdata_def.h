// Copyright, 1995-2007, Regents of the University of Colorado,
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


// ta_qtdata.h: Qt-based graphical data representations

#ifndef TA_QTDATA_DEF_H
#define TA_QTDATA_DEF_H

#include "ta_base.h"
#include "ta_variant.h"

#ifndef __MAKETA__
# include <QObject>
# include <QLabel>
# include <QPointer>
# include <QWidget>
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
  virtual const iColor	colorOfCurRow() const = 0; // #IGNORE background color of cur row
  virtual bool		isConstructed() = 0;
  virtual bool		isModal() = 0;
  virtual bool		isReadOnly() = 0;
  virtual taMisc::ShowMembs show() const = 0;
  virtual iMainWindowViewer* viewerWindow() const {return NULL;} // used to set cliphandler
  virtual bool		HasChanged() = 0; // 'true' if has changed
  virtual void*		Root() const = 0; // root of the object
  virtual taBase*	Base() const = 0; // root of the object, if a taBase 
  virtual TypeDef*	GetRootTypeDef()const  = 0; // TypeDef on the root, for casting
  virtual void		GetValue() = 0; // copy gui to value
  virtual void		GetImage() = 0; // copy value to gui
  virtual void		Changed() {} // called by embedded item to indicate contents have changed
  virtual void		SetItemAsHandler(taiData* item, bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset); in ta_qtviewer.cpp
  virtual void		Apply_Async() = 0; // provided for the APPLY_IMMED functionality
  
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
  // ##IGNORE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for data elements
  Q_OBJECT
  friend class taiCompData; // for parenting
#ifndef __MAKETA__
typedef QObject inherited;
#endif
public:
  enum Flags { // flags are so we don't have to keep adding parameters to constructor... flags >= 10000 are custom per type
    flgReadOnly		= 0x0001,
    flgNoTokenDlg	= 0x0002, // for tokenptr, not keeping tokens, so don't enable but
    flgNullOk		= 0x0004, // used typically by menus of tokens or objects, to allow NULL
    flgEditOk		= 0x0008,  // used typically by menus to allow Edit of item
    flgNoList		= 0x0010,  // used typically by menus to include lists of item
    flgNoGroup		= 0x0010,  // used typically by menus to include groups of items -- note this is the same as NoList
    flgNoInGroup	= 0x0020,  // used by gpiGroupEls
    flgEditOnly		= 0x0040,  // used by EditButton
    flgInline		= 0x0080,   // used by types and members that support INLINE directive, esp. Array
    flgEditDialog	= 0x0100,   // for taiField, enables dialog for EDIT_DIALOG directive; for token menu, adds ... edit
    flgNoUAE		= 0x0200,  // for things like polydata, don't issue an UpdateAfterEdit
    flgToggleReadOnly	= 0x0400, // for taiPlusToggle, makes the toggle itself ro
    flgAutoApply	= 0x0800,  // when user finishes editing this control, auto apply the edits
    flgFlowLayout	= 0x1000, // for polyguys (inline) us a flowlayout, not hboxlayout
    flgCondEditUseRO	= 0x2000, // for taiCondEditMember, used to keep its use_ro flag
    flgNoEditDialogAutoApply = 0x4000, // overrides the implicit APPLY_IMMED for the edit dialog of fields
    flgNoAlpha		= 0x10000, // for taiColor, don't use the alpha channel
    flgNoHelp		= 0x20000, // for taiPtr guys, don't add a Help button
#ifndef __MAKETA__
    flg_INHERIT_MASK	= (flgReadOnly) // flags to pass on to nested children, in inline
#endif
  };

  TypeDef* 		typ;		// type for the gui object
  MemberDef*		mbr; // for members
  IDataHost*		host;		// dialog or edit panel that this belongs to (optional)
  String		orig_val;	// text of original data value
  taiData*		parent() {return mparent;} // if data is contained within data, this the parent container
  
  void			setParent(taiData* value); // 
  virtual taBase*	Base() const; // base, typically the parent::ChildBase else the IHost::Base
  void			SetBase(taBase* base_) const; 
    // we often try to set the base, so SelEdit ctxt menus can work 
  virtual taBase*	ChildBase() const {return Base();} 
   // child base, typically obtained from parent or host, except ex. PolyData
  virtual void		Delete(); // use this to dynamically delete this, including the rep
  taiData(); // for ta_TA.cc only
  taiData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags_ = 0);
  virtual ~taiData();

  int			defSize() const;		// default taiMisc::SizeSpec value, for sizing controls (taken from parent, else dlg, else "default")
  virtual bool		isConstructed();	// true if our parents (ex dialog) are fully constructed
  bool			highlight() const { return mhighlight; }	// #GET_highlight  changed highlight
  QLabel*		label() const {return m_label;}
  void			setLabel(QLabel* value) {m_label = value;}
  virtual void		setHighlight(bool value);	// #SET_Highlight
  virtual bool		readOnly() const; // #GET_ReadOnly true if the control should be read only -- partially delegates to parent
  virtual bool		fillHor() {return false;} // override to true to fill prop cell, ex. edit controls
  bool			visible() const {return m_visible;}
  bool			setVisible(bool value); // for CONDSHOW guys, makes lbl/rep vis or not, also triggers VisibleChanged if changes; ret true if visible changed
  bool 			eventFilter(QObject* watched, QEvent* ev); // override
  virtual QWidget*	GetRep()	{ return m_rep; }
  virtual QLayout*	GetLayout() {return NULL;} // returns a top layout, if one is used
  inline int		flags() const {return mflags;}
  bool			HasFlag(int flag_) const {return (mflags & flag_);} // returns true if has the indicated Flag (convenience method)
  void			SetFlag(int flags_, bool value = true) {if (value) mflags |= flags_; else mflags &= ~flags_;} // sets or clears a flag or set of flags
  virtual void		SetMemberDef(MemberDef* mbr_) {mbr = mbr_;} // taiMember sets this
  
  void			SetThisAsHandler(bool set_it = true); // called by compatible controls to set or unset the control as clipboard/focus handler (usually don't need to unset)
  virtual void		DataChanged(taiData* chld = NULL);
  // indicates something changed in the data from user input, if chld is passed then it called parent->DataChanged(this); ignored if parent or ourself is not fully constructed

  void			GetImage_(const void* base) {GetImage_impl(base);} // base points to value of type
  void			GetValue_(void* base) const {GetValue_impl(base);} // base points to value of type
  
  void			GetImageVar_(const Variant& val) {GetImageVar_impl(val);} // get as Var, mostly used by css
  void			GetValueVar_(Variant& val) const {GetValueVar_impl(val);} // set as Var, mostly used by css
  
  iLabel*		MakeLabel(QWidget* gui_parent = NULL, int font_spec = 0) const; // convenience func, sets fonts etc.
  iLabel*		MakeLabel(const String& text, QWidget* gui_parent = NULL,
    int font_spec = 0) const; // convenience func, sets fonts etc.
  QWidget*		MakeLayoutWidget(QWidget* gui_parent = NULL) const;
    // makes a layout widget, with max height set, and tweaked for Mac
   
#ifndef __MAKETA__
  void			emit_UpdateUi();
  void 			emit_settingHighlight(bool setting);
signals:
  bool 			settingHighlight(bool setting); // invoked when highlight state changes
  void			UpdateUi(); // cliphandler callback, to get it to requery the ui items
  void			DataChangedNotify(taiData* sender); // raised when data changed for us (but not when invoked by a child)
#endif

protected:
  bool			mhighlight;
  bool			m_visible; // defaults to true, only for CONDSHOW guys
#ifndef __MAKETA__
  QPointer<QWidget>	m_rep;		// widget that represents the data
  QPointer<QLabel>	m_label; // an associate label, typically for condshow show/hide
#endif
  taiData*		mparent;		// if data is contained within data, this the parent container
  int			mflags;
  mutable taBaseRef	m_base; // this is a hack for when the Host is not the Base, particularly ProgCtrl panels, etc., and enables Seledit to work

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
  virtual void		applyNow(); // calling this will force the host to apply editing data -- same action as if user pressed Apply button

  // handler controls do nothing in base class, and in most subclasses
  virtual void 		this_GetEditActionsEnabled(int& ea) {} // for when control is clipboard handler
  virtual void 		this_EditAction(int param) {} // for when control is clipboard handler
  virtual void 		this_SetActionsEnabled() {} // for when control is clipboard handler
private:
  void			Destroy();
};


class TA_API taiDataList : public taPtrList<taiData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS taiData list, OBJECTS ARE DELETED ON REMOVAL
INHERITED(taPtrList<taiData>)
public:
#ifndef __MAKETA__
  USING(inherited::Add)
  template<class T>
  T*			Add(T* it) {Add_((void*)it); return it;}
    // convenience method, returns strongly typed guy that it adds
#endif
  ~taiDataList();
protected:
//  void*		El_Ref_(void* it)	{ taRefN::Ref((taiData*)it); return it; }
//  void* 	El_unRef_(void* it)	{ taRefN::unRef((taiData*)it); return it; }
  void		El_Done_(void* it);

};



#endif


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

#ifndef TA_QTDATA_H
#define TA_QTDATA_H

#include "ta_variant.h"
#include "ta_base.h"
#include "ta_qtdata_def.h"

#ifndef __MAKETA__
# include <qdialog.h>
# include <qobject.h>
# include <QAction>
# include <qwidget.h>
# include <QList>
# include "icheckbox.h"
#endif

#include "igeometry.h"

//nn? #include <unistd.h>

// externals
class QKeySequence; // #IGNORE
class MatrixGeom;
class ScriptVar; //

// forwards
class taiToggle;
class taiToken;
class taiTypeHier;
class taiActions;
class taiMenu;
class taiAction; //


/* //TODO: re-evaluate
class IconGlyph : public QWidget {
  // #IGNORE a special glyph to put as the first thing in a window, sets pointers..
public:
  ivManagedWindow*	window;		// this is the window
  void*			obj;		// this is the object that owns the window
  int			(*SetIconify)(void*, int on_off);
  // set iconify function: -1 get value, 0 = set off, 1 = set on
  void			(*ScriptIconify)(void* , int on_off);
  // script record iconify: 0 = set off, 1 = set on

  void	SetWindow(ivManagedWindow* w);

  void  draw(ivCanvas*, const ivAllocation&) const;
  void  undraw();

  IconGlyph(QWidget* g, ivManagedWindow* w=NULL,void* o=NULL);
}; */


class TA_API taiCompData : public taiData {
  //  #NO_INSTANCE base class for composite data elements
public:

  taiCompData(TypeDef* typ_, IDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags = 0);
  override ~taiCompData();

  virtual QLayout*	GetLayout() {return (QLayout*)lay;}
  taiDataList  data_el;

  QWidget*	widgets(int index);
  int		widgetCount();

  virtual void	InitLayout(); // default creates a QHBoxLayout in the Rep
  void		AddChildWidget(QWidget* child_widget, int space_after = -1, int spacing = 0);
  void 		AddChildMember(MemberDef* md); // adds label and control for the member
  virtual void	EndLayout(); // default adds a stretch
protected:
  QHBoxLayout*	lay;	// may be ignored/unused by subclasses
  int		last_spc;	// space after last widget, -1 = none
  override void		ChildAdd(taiData* child);
  override void		ChildRemove(taiData* child);
  virtual void	AddChildWidget_impl(QWidget* child_widget, int spacing);// default does an add to layout
private:
  QObjectList*	mwidgets; // list of child widgets
};

class TA_API taiField : public taiData {
  Q_OBJECT
public:
  iLineEdit*		rep() const { return leText; }
  bool			fillHor() {return true;} // override 
  taiField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  void 	GetImage(const String& val);
  String GetValue() const;

protected slots:
  void			selectionChanged();
  void			btnEdit_clicked(bool);

protected:
  iLineEdit*		leText;
  QToolButton*		btnEdit; // if requested, button to invoke dialog editor
  override void		GetImage_impl(const void* base) {GetImage(*((String*)base));}
  override void		GetValue_impl(void* base) const {*((String*)base) = GetValue();} 
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};

// this is for integers -- includes up and down arrow buttons
class TA_API taiIncrField : public taiData { // increment/decrement field
  Q_OBJECT
public:
  iSpinBox*	rep() const { return (iSpinBox*)m_rep; }
  taiIncrField(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: flgPosOnly

  void 			GetImage(int val);
  int 			GetValue() const;

protected slots:
  void			selectionChanged();

protected:
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};


class TA_API taiToggle : public taiData {
public:
  iCheckBox*	rep() const { return (iCheckBox*)m_rep; }

  taiToggle(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
//  ~taiToggle() {}

  void 	GetImage(bool val);
  bool	GetValue() const;
};


class TA_API taiPlusToggle : public taiCompData {
// a regular field plus a toggle..
  Q_OBJECT
public:
  taiData*	data; // set by caller after creating contained class
  QCheckBox* 	but_rep;
  QFrame* 	rep() {return (QFrame*)m_rep;} //parent of contained controls

  taiPlusToggle(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiPlusToggle();

  override void	InitLayout();

  virtual int	GetValue();
  virtual void 	GetImage(bool chk);
public slots:
  virtual void	Toggle_Callback();

protected:
  override void	DataChanged_impl(taiData* chld);
};

//////////////////////////
//     taiComboBox	//
//////////////////////////

class TA_API taiComboBox : public taiData {
  Q_OBJECT
public:
  iComboBox*	rep() const { return (iComboBox*)m_rep; }

  taiComboBox(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiComboBox(bool is_enum, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  void		AddItem(const String& val, const QVariant& userData = QVariant()); // add an item to the list
  void		Clear(); //clears all items (only needed if refilling list)
  void 		RemoveItemByData(const QVariant& userData); // remove item with indicated data
  void 		RemoveItemByText(const String& val); // remove item with indicated text

  void 		GetImage(int itm);  // set to this item number
  void		GetValue(int& itm) const;

  void		SetEnumType(TypeDef* enum_typ, bool force = false); // sets a new enum type
  void 		GetEnumImage(int enum_val);  // set to item number corresponding to this enum value, enum mode only
  void		GetEnumValue(int& enum_val) const; // set to enum value corresponding to item number, enum mode only
  
#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-IDataHost users, forwards chkbox signal
#endif

private:
  void		Initialize(QWidget* gui_parent_);
};

//////////////////////////
//     taiBitBox	//
//////////////////////////

class TA_API iBitCheckBox: public iCheckBox { // #IGNORE specialized checkbox for the taiBitBox class
  Q_OBJECT
public:
  int		val;
  iBitCheckBox(int val_, String label, QWidget* parent);

public slots:
  void		this_toggled(bool on);

#ifndef __MAKETA__
signals:
  void		toggledEx(iBitCheckBox* sender, bool on);
#endif
};



class TA_API taiBitBox : public taiData { // supports enums that are bit fields (TODO: maybe support int fields too)
  Q_OBJECT
public:
  QFrame*	rep() const { return (QFrame*)m_rep; }
  bool			fillHor() {return true;} // override 

  taiBitBox(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiBitBox(bool is_enum, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  //TODO: maybe default highlighting???
  void		AddBoolItem(String name, int val); // add an item to the list

  void 		GetImage(int val);  // set to this value, according to bit fields
  void		GetValue(int& val) const;

public slots:
  void		bitCheck_toggled(iBitCheckBox* sender, bool on); // #IGNORE

#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-IDataHost users, forwards chkbox signal
#endif

protected:
  QHBoxLayout* 	lay; //#IGNORE
  int		m_val; //#IGNORE
private:
  void		Initialize(QWidget* gui_parent_);
};

class TA_API taiDimEdit : public taiData { // specify number of dims and each dim value, ex. for Matrix dims
INHERITED(taiData)
  Q_OBJECT
public:
  iDimEdit*		rep() const {return (iDimEdit*)m_rep;}
  bool			fillHor() {return true;} // override 
  taiDimEdit(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: 

  void 			GetImage(const MatrixGeom* arr);
  void 			GetValue(MatrixGeom* arr) const;

protected:
  override void		GetImage_impl(const void* base) {GetImage((const MatrixGeom*)base);}
  override void		GetValue_impl(void* base) const {GetValue((MatrixGeom*)base);} 
/*TODO  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler */
private:
  void 			Initialize(QWidget* gui_parent_);

};


//////////////////////////
//     taiPolyData	//
//////////////////////////

// this class supports the use of hierarchical sub-data within a data item
// its default behavior is to put everything in an hbox with labels

class TA_API taiPolyData : public taiCompData {
public:
  QWidget*	rep() const { return (QWidget*)m_rep; } //note: actual class may be subclass of QFrame
  bool			fillHor() {return true;} // override 
  int		show;

  taiPolyData(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiPolyData();

protected:
  virtual void		Constr(QWidget* gui_parent_);
  override void		GetImage_impl(const void* base);
  override void		GetValue_impl(void* base) const; 
  virtual bool		ShowMember(MemberDef* md) const;
};

//////////////////////////
//     taiDataDeck	//
//////////////////////////

// contains sub-data's within a deck (aka Qt Stack) -- can toggle between them

class TA_API taiDataDeck : public taiCompData {
public:
  Q3WidgetStack*	rep() {return (Q3WidgetStack*)m_rep;}
  int		cur_deck; // current deck for operations

  void		setNumDecks(int n); // sets # decks

  override void	InitLayout() {} // nothing in this class
  override void	EndLayout() {} // nothing in this class
  taiDataDeck(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  virtual void  GetImage(int i);
protected:
  override void AddChildWidget_impl(QWidget* child_widget, int spacing); //
};


class TA_API taiVariantBase: public taiCompData {
  // common code/members for complex types that use a variant
INHERITED(taiCompData)
  Q_OBJECT
public:
  enum CustomFlags { // #BITS
    flgNoInvalid	= 0x010000, // don't let user choose Invalid
    flgNoAtomics	= 0x020000, // don't let user choose any atomics
    flgIntOnly		= 0x040000, // Int only (used for Enum)
    flgNoPtr		= 0x080000, // don't let user choose raw ptr
    flgNoBase		= 0x100000 // don't let user choose taBase or taMatrix
  };
  
  bool			fillHor() {return true;} // override 
  taiVariantBase(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiVariantBase();

protected:
  enum StackControls { // #IGNORE indexes of controls in the stack
    scInvalid,
    scBool,
    scAtomics, // includes string and char
    scPtr,
    scBase,
    scMatrix
  };
  
  int			m_updating;
  
  taiComboBox*		cmbVarType;
  QStackedWidget*	stack; // holds the subfields for different types
  
  taiToggle*		togVal; // for: bool
  taiField*		fldVal; // for: char, string, numbers
  taiToken*		tabVal; // for taBase token (note: browsing could be hairy!!!)
  taiToken*		matVal; // for Matrix token
  
  void			Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void		Constr_impl(QWidget* gui_parent_, bool read_only_); 
    // (possibly) extend, and called in your constructor
  void  		GetImage_Variant(const Variant& var);
  void	 		GetValue_Variant(Variant& var);

  
protected slots:
  void			cmbVarType_itemChanged(int itm);

};


class TA_API taiVariant: public taiVariantBase {
INHERITED(taiVariantBase)
  Q_OBJECT
public:
  
  QWidget*	rep() const { return (QWidget*)m_rep; } //note: actual class may be subclass of QFrame

  void  	GetImage(const Variant& var) {GetImage_Variant(var);}
  void	 	GetValue(Variant& var) {GetValue_Variant(var);}
  
  taiVariant(IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiVariant();
};


class TA_API taiScriptVarBase: public taiCompData { 
  //note: this set of classes uses a static New instead of new because of funky virtual Constr
INHERITED(taiCompData)
  Q_OBJECT
public:
  QWidget*	rep() const { return (QWidget*)m_rep; } 
  bool		fillHor() {return true;} // override 
  ~taiScriptVarBase();

  void			Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void  	GetImage(const ScriptVar* var);
  virtual void	 	GetValue(ScriptVar* var) const;

protected:
  int			m_changing; // used to prevent recursions
  taiField*		fldName;
  
  virtual void		Constr_impl(QWidget* gui_parent_, bool read_only_); //override
  override void		GetImage_impl(const void* base) {GetImage((const ScriptVar*)base);}
  override void		GetValue_impl(void* base) const {GetValue((ScriptVar*)base);} 
  taiScriptVarBase(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
};


class TA_API taiScriptVar: public taiScriptVarBase { 
  //note: this set of classes uses a static New instead of new because of funky virtual Constr
INHERITED(taiScriptVarBase)
  Q_OBJECT
public:
  static taiScriptVar*	New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
  
  ~taiScriptVar();

  void  		GetImage(const ScriptVar* var);
  void	 		GetValue(ScriptVar* var) const;

protected:
  taiVariant*		vfVariant;
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); //override
  taiScriptVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


class TA_API taiEnumScriptVar: public taiScriptVarBase {
INHERITED(taiScriptVarBase)
  Q_OBJECT
public:
  static taiEnumScriptVar* New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
  ~taiEnumScriptVar();

  void  		GetImage(const ScriptVar* var); // override
  void	 		GetValue(ScriptVar* var) const; // override

protected:
  taiTypeHier*		thEnumType;
  taiComboBox*		cboEnumValue;
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); 
  void			DataChanged_impl(taiData* chld); // override
  taiEnumScriptVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


class TA_API taiObjectScriptVar: public taiScriptVarBase {
INHERITED(taiScriptVarBase)
  Q_OBJECT
public:
  static taiObjectScriptVar* New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);

  void  		GetImage(const ScriptVar* var); // override
  void	 		GetValue(ScriptVar* var) const; // override
  ~taiObjectScriptVar();

protected:
  taiTypeHier*		thValType;
  taiToggle*		chkMakeNew;
  QLabel*		lblObjectValue;
  taiToken*		tkObjectValue;
  
  void			MakeNew_Setting(bool value); // common code jigs visibility
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); 
  void			DataChanged_impl(taiData* chld); // override
  taiObjectScriptVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};

//////////////////////////////////
//   Menus and Toolbars		//
//////////////////////////////////


class TA_API taiAction: public QAction {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS holds menu and/or toolbar item data -- can be the root item of a submenu
  Q_OBJECT
friend class taiActions;
friend class taiAction_List;
public:
  enum CallbackType {
    none,		// callback parameters ignored
    action,		// corresponds to IV "action" type of callback -- slot for signal is parameterless
    men_act,		// corresponds to IV "men_act" type of callback -- slot for signal takes taiAction* as a sender
    int_act,		// corresponds to Qt menu/action signals -- slot for signal takes an int parameter
    ptr_act,		// slot for signal takes a void* parameter
    var_act		// slot for signal takes a "const Variant&" parameter
  };

  int			sel_type;
  String		label() {return String(text());} // TODO: nuke, replace with refs to text()
  Variant		usr_data; // default is Invalid, can also be int or void* 

  bool			canSelect(); // true if item can be taiMenu::curSel value
  virtual bool		isSubMenu() 	{ return false; }
  bool			isGrouped(); // indicates if item in a group, ie is a radio group action
  
  void			AddTo(taiActions* targ); // convenience function
  
  taiAction(int sel_type_, const String& label_); // used by taiMenu etc.
  taiAction(const QString& label_, const QKeySequence& accel, const char* name); // used by viewer/browser
  taiAction(const QString& label_, QObject* receiver, const char* member, const QKeySequence& accel); 
    // used by viewer/browser
  taiAction(const Variant& usr_data_, const QString& label_, const QKeySequence& accel, const char* name); // used by viewer/browser
  virtual ~taiAction();

  void connect(CallbackType ct_, const QObject *receiver, const char* member); // connect callback to given
  void connect(const taiMenuAction* mact); // convenience function

  virtual QMenu*	SubMenu() 	{ return NULL; }

#ifndef __MAKETA__
signals:
  void Action();			// for a direct call-back
  void MenuAction(taiAction* sender);	// for a menu-action call-back (returns item selected)
  void IntParamAction(int param);	// for Qt-style parameter ((int)usr_data)
  void PtrParamAction(void* ptr);	// for ptr parameter ((void*)usr_data)
  void VarParamAction(const Variant& var);	// for variant parameter ((void*)usr_data)
#endif // ndef __MAKETA__

protected:
  int			nref; // ref'ed when put onto a taiAction_List, deleted when ref=0
  int			m_changing; // for supressing recursive signals
  void			init(int sel_type_); 
  void 			emitActions();
  
protected slots:
  virtual void		this_triggered_toggled(bool checked); // annoying, but self connect, so we can re-raise
  
private:
  taiAction(const taiAction&); // no copying
  void operator=(const taiAction&); 
};

class TA_API taiSubMenuEl: public taiAction { // an action used exclusively to hold a submenu
  Q_OBJECT
friend class taiActions;
public:
  taiMenu* 		sub_menu_data; // the taiMenu for this submenu

  QMenu*		SubMenu() 	{ return menu(); } // override
  bool			isSubMenu() 	{ return true; } // override
  
protected: // only allowed to be used internally when creating submenus
  void		this_triggered_toggled(bool checked) {} // override submenu items don't signal anything
  
  taiSubMenuEl(const String& label_, taiMenu* sub_menu_data); //
  ~taiSubMenuEl();

};

class TA_API taiAction_List : public taPtrList<taiAction> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  int			count() {return size;} // for Qt-api compat
  
  taiAction*		PeekNonSep(); // returns last non-separator item, if any
  
  ~taiAction_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiAction*)it)->text()); }
  
protected:
  override void*	El_Ref_(void* it)  { ((taiAction*)it)->nref++; return it; }	// when pushed
  override void* 	El_unRef_(void* it)  { ((taiAction*)it)->nref--; return it; }	// when popped
  override void		El_Done_(void* it);
};


//////////////////////////////////
//   taiActions		//
//////////////////////////////////

class TA_API taiActions : public taiData {
  // #VIRT_BASE common subtype for menus and menubars
  Q_OBJECT
INHERITED(taiData)
friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  enum RepType { // for parameterized creation of a compatible subtype
    popupmenu, 	// for a taiMenu
    buttonmenu // for a taiButtonMenu
  };
  
  enum SelType {
    st_none =		0x00,	// no action on selection -- primarily for SubItem entries
    normal =		0x01,	// selecting the item causes an action; no persistent check mark by menu item
    normal_update =	0x81,
    radio = 		0x02,	// only 1 of n items on the menu can be selected; selecting one deselects any other
    radio_update =	0x82,
    toggle =		0x04,	// item can be checked or unchecked, independent of any other item
    toggle_update =	0x84,

    use_default =	0x10,	// only for taiActions::AddAction -- inherits from the taiActions

    update = 		0x80	// flag added to normal selection types, causes parent item to indicate Changed when item selected
  };
  
  static taiActions* New(RepType rt, int sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL); // note: this is primarily for compatibility with Qt3 version code

  SelType		sel_type;
  int			font_spec; //taiMisc::FontSpec
  taiAction_List	items;
  QWidget*		gui_parent; // needed for submenus
  
  int			count() const {return items.size;} // qt compatability -- note that count won't have separators
  String		label() const; //#GET_Label
  QMenu*		menu()	{ return m_menu; }	 // for when actions stored in a menu, otherwise NULL
  virtual void		setLabel(const String& val); // #SET_Label replaces the SetMLabel call in IV
  taiAction*		curSel() const;
  void			setCurSel(taiAction* value);
  QWidget*		actionsRep(); // where actions are stored, in menu if a menu, else in Rep
  void			SetRep(QWidget* val); // override
  virtual void		AddSep(bool new_radio_grp = false); // add menu separator -- can also be used to create new radio groups --  won't add initial sep, or 2 separators in a row; seps don't count as taiActions
  virtual void		AddAction(taiAction* act); // add the already created action
  taiAction* 		AddItem(const String& val, SelType st = use_default, 
    taiAction::CallbackType ct = taiAction::none, const QObject *receiver = NULL, const char* member = NULL,
    const Variant& usr = _nilVariant);
  taiAction* 		AddItem(const String& val, SelType st, const taiMenuAction* men_act, 
    const Variant& usr = _nilVariant);
  taiAction* 		AddItem(const String& val, const Variant& usr);
	
  virtual taiMenu*	AddSubMenu(const String& val, TypeDef* typ_ = NULL); // add a submenu -- this also works for toolbars, and will create a toolbar menu button
  
//obs  virtual taiAction*	GetValue()	{ return cur_sel; }
  virtual bool 		GetImageByData(const Variant& usr); // for radio menus, set to item with this data; recursive
  void 			GetImageByIndex(int item);
  taiMenu*		FindSubMenu(const char* nm); // find specified submenu, or NULL if not found

  void			DeleteItem(uint index); // deletes the indicated item -- deletes the gui representation as well
  virtual void		NewRadioGroup();	// start a new radio group (must also preceed first group)
  virtual void		Reset();
  
  taiAction*		operator[](int index) const {return items.SafeEl(index);}
  taiActions(int sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL,
      bool has_menu = false, QMenu* exist_menu = NULL);
  ~taiActions();
#ifndef __MAKETA__
signals:
  void labelChanged(const char* val); //
#endif

  
protected:  
  QActionGroup*		cur_grp; // for radio groups, current group, if any
  taiAction*		cur_sel;  // selection for getting value of menu -- only used by top-level menu
  String		mlabel; // string contents of current menu label
  QMenu*		m_menu; // for when items add to a menu
  taiActions*	par_menu; // parent menu, if any -- many methods delegate their calls upward if there is a parent
  taiSubMenuEl*		par_menu_el; // parent submenu element, if any
  void 			emitLabelChanged(const String& val); // #IGNORE
  virtual bool 		GetImage_impl(const Variant& usr);  // #IGNORE set to this usr item, returns false if not found -- recursive for hiermenus
  virtual void 		ActionAdded(taiAction* it); // add to rep, def adds to mrep, but overridden for menubutton type
  virtual void 		ActionRemoving(taiAction* it); // remove from rep, def removes from mrep, but overridden for menubutton type
  
protected slots:
  virtual void 		child_triggered_toggled(taiAction* act);
  void			repDestroyed(QObject* obj); //override
};

//////////////////////////////////
// 	    taiMenu		//
//////////////////////////////////

class TA_API taiMenu : public taiActions {
  // (possibly) hierarchical menu for selecting a single item
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
friend class taiMenu_List; // hack because lists return refs to strings, not values
friend class taiActions;
public:
  taiMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL);
//nbg  taiMenu(int rt, int st, int ft, QWidget* gui_parent_); // constructor for WinBase and other non-taiDialog uses
  taiMenu(QWidget* gui_parent_, int sel_type_= normal, int font_spec_ = 0, QMenu* exist_menu = NULL);
    // constructor for Browser and context menus ft=0 means default font size; 
  ~taiMenu();

  void			exec(const iPoint& pos);
  taiAction*		insertItem(const char* val, const QObject *receiver = NULL, const char* member = NULL,
    const QKeySequence* accel = NULL); // OBS compatability routine with QMenu
  void			insertSeparator() {AddSep();} // Qt-convenience

private:
  void			init(); // #IGNORE
};


//////////////////////////////////
//  taiButtonMenu		//
//////////////////////////////////

class TA_API taiButtonMenu: public taiActions { // a button, in which the actions appear as a popup menu
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
public:
  QPushButton*		rep() {return (QPushButton*)m_rep;}
  
  taiButtonMenu(int  sel_type_, int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiActions* par_menu_ = NULL);
private:
  void 			init();

};

//////////////////////////////////
// 	    taiMenuBar		//
//////////////////////////////////

class TA_API taiMenuBar : public taiActions {
  // top level menu bar
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  QMenuBar*		rep_bar() {return (QMenuBar*)m_rep;}
  
  override void		AddSep(bool new_radio_grp = false) {} // no seps or groups allowed in a menubar
  
  taiMenuBar(int font_spec_, TypeDef* typ_, IDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // used by taiEditDataHost
  taiMenuBar(QWidget* gui_parent_, int ft, QMenuBar* exist_menu); // used by iDataViewer
  ~taiMenuBar();
protected:
  void 			init(QMenuBar* exist_menu); // #IGNORE
  void 			ConstrBar(QWidget* gui_parent_, QMenuBar* exist_bar = NULL); // #IGNORE
};

class TA_API taiToolBar: public taiActions { // a toolbar, in which the actions appear as toolbuttons or button menus
  Q_OBJECT
#ifndef __MAKETA__
typedef taiActions inherited;
#endif
public:
  QToolBar*		rep() {return (QToolBar*)m_rep;}
  
  taiToolBar(QWidget* gui_parent_, int ft, QToolBar* exist_bar); // used by iDataViewer
protected:
  void 			init(QToolBar* exist_bar);

};

//////////////////////////////////
// 	taiMenu_List		//
//////////////////////////////////

//NOTE: this class provided for winbase.h and others

class TA_API taiMenu_List : public taPtrList<taiActions> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ if (own_items) delete (taiMenu*)it; }

public:
  bool own_items; //generally false
  taiMenu_List()            { own_items = false; }
  ~taiMenu_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiActions*)it)->mlabel); }
};

//////////////////////////////////
// 	iAction		//
//////////////////////////////////

/*
  An iAction encapsulates a menu, toolbar, accelerator, etc. user action. We don't
  use Actions in Edit Dialogs, but we do use them in DataViewers; they work with
  taiMenu, which is why we define it here. iAction inherits Qt's QAction.
*/
/*obs

class iAction: public QAction {
  Q_OBJECT
friend class taiMenu;
public:
  iAction(int param_, QObject* parent)
      :QAction(parent) {init(param_);}
  iAction(int param_, const QString& menuText, const QKeySequence& accel, QObject* parent)
      :QAction(menuText, parent) {setShortcut(accel), init(param_);}
  iAction(int param_, const QIcon& icon, const QString& menuText, const QKeySequence& accel, QObject* parent)
      :QAction(icon, menuText, parent) {setShortcut(accel), init(param_);}

  iAction(QObject* parent)
      :QAction(parent) {init();}
  iAction(const QString& menuText, const QKeySequence& accel, QObject* parent)
      :QAction(menuText, parent) {setShortcut(accel), init();}
  iAction(const QIcon& icon, const QString& menuText, const QKeySequence& accel, QObject* parent)
      :QAction(icon, menuText, parent) {setShortcut(accel), init();}

  void		AddTo(taiMenu* menu); // adds to a taiMenu, using the text, accel key, respects toggle, sets the param in the usr_data

#ifndef __MAKETA__
signals:
  void activated(int param);
#endif
protected:
  QAction*	m_action;
  int 		param;
  void 		init(int param_ = -1); // -1 generally indicates none

protected slots:
  void this_activated();
};

class iAction_List: public QList<iAction*> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS corresponds to a menu item and/or toolbar button -- items are stickily created as needed (browsewin creates the common set)

  override String 	El_GetName_(void*) const; // name is Q object name
};
*/

//////////////////////////////////
// 	taiEditButton		//
//////////////////////////////////

class TA_API taiEditButton : public taiButtonMenu {
  // actually an edit menu... -- flgReadOnly creates menu which only allows for #EDIT_READ_ONLY members
  Q_OBJECT
public:
  void*		cur_base;
  taiEdit*	ie;
  taiDataList 	meth_el;	// method elements

  taiEditButton(void* base, taiEdit *taie, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgReadOnly, flgEditOnly -- internally sets flgEditOnly if appropriate
  ~taiEditButton();

  virtual void	GetMethMenus();

  virtual void	SetLabel();

public slots:
  virtual void	Edit();		// edit callback
  void setRepLabel(const char* label);
protected:
  override void		GetImage_impl(const void* base); 
};


//////////////////////////////////
// 	taiObjChooser		//
//////////////////////////////////

class TA_API taiObjChooser: QDialog {
// ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
// select objects from a list, much like a file chooser.  can be tokens from typedef or items on a list
  Q_OBJECT
public:
  static taiObjChooser* createInstance(TAPtr parob, const char* captn, bool selonly = true, QWidget* par_window_ = NULL);
    // create method for lists/groups
  static taiObjChooser* createInstance(TypeDef* tpdf, const char* captn, TAPtr scope_ref_ = NULL, QWidget* par_window_ = NULL);
    // create method for tokens

  bool	        	select_only;	// if true, only for selecting objects from current parent object
  String		caption; 	// current caption at top of chooser
  String		path_str;	// current path string
  TABLPtr		lst_par_obj;	// parent object that is a list object
  TAPtr			reg_par_obj; 	// parent object that is *not* a list object
  TypeDef*		typ_par_obj; 	// parent object that is a typedef (get tokens)
  TAPtr			scope_ref;	// reference object for scoping

  TAPtr			sel_obj() {return msel_obj;}	// #GET_Sel_obj current selected object
  void			setSel_obj(const TAPtr value);	// #SET_Sel_obj
  String		sel_str() {return msel_str;} 	// #GET_Sel_str string rep of current selection
  String_Array		items;		// the items in the list

  QGridLayout*		layOuter;
  Q3ListBox* 		browser; 	// list of items
  QLineEdit* 		editor;
  QHBoxLayout*		layButtons;
  QPushButton*		btnOk;
  QPushButton*		btnCancel;

  taiObjChooser(TAPtr parob, const char* captn, bool selonly, QWidget* par_window_); // USE createInstance instead!!!
  taiObjChooser(TypeDef* tpdf, const char* captn, TAPtr scope_ref_, QWidget* par_window_); // USE createInstance instead!!!

  virtual bool	Choose();
  // main user interface: this actually puts up the dialog, returns true if Ok, false if cancel

  virtual void 	Build();	// called as constructed
  virtual void 	Clear();	// reset data
  virtual void 	Load();		// reload data
  virtual void 	ReRead();	// update browser for new parent

  virtual void	GetPathStr();	// get current path string
  virtual void  AddItem(const char* itm, const void* data_ = NULL); // add one item to dialog
  virtual void	UpdateFmSelStr(); // update selection based on sel_str

protected:
  TAPtr			msel_obj;	// current selected object
  String		msel_str; 	// string rep of current selection

  void 		init(const char* captn, bool selonly, QWidget* par_window_); // called by constructors
  virtual void 	AcceptEditor_impl(QLineEdit* e);
  void 		AddObjects(TAPtr obj);
  void		AddTokens(TypeDef* td); // add all tokens of given type

protected slots:
  void accept(); // override
  void reject(); // override
  // callbacks
  void 		browser_selectionChanged(Q3ListBoxItem* itm);
  void 		browser_doubleClicked(Q3ListBoxItem* itm);
  virtual void 	DescendBrowser();
  virtual void 	AcceptEditor();
};

//////////////////////////////////
// 	taiFileButton		//
//////////////////////////////////

class TA_API taiFileButton : public taiButtonMenu {
  Q_OBJECT
public:
  bool			read_only;	// only reading streams is an option
  bool			write_only;	// only writing streams are available

  virtual void		SetFiler(taFiler* gf_); // you can use your own filer if you want; ref counted
  virtual void  	GetImage();
  taFiler* 		GetFiler() {return gf;} //NOTE: make sure you ref it properly!

  virtual void		GetGetFile();	// make sure we have a getfile..

  taiFileButton(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_,
		  bool rd_only = false, bool wrt_only = false);
  ~taiFileButton();

public slots:
  virtual void		Open();		// callback
  virtual void		Save();		// callback
  virtual void		SaveAs();	// callback
  virtual void		Append();	// callback
  virtual void		Close();	// callback
  virtual void		Edit();		// callback
protected:
  taFiler*		gf;
};


//////////////////////////////////
// 	taiElBase		//
//////////////////////////////////

// base class for sundry taiData items that use a menu, and have a taBase-derived current item

class TA_API taiElBase: public taiData {
public:
  TAPtr		cur_obj;
//  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  virtual void  GetMenu(taiActions* actions, taiMenuAction* actn = NULL) {} // variant provided for MenuGroup_impl in winbase
  void 		setCur_obj(TAPtr value, bool do_chng = true); // set cur_obj and notifies change if different
  taiElBase(taiActions* actions_, TypeDef* tp, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiElBase();
protected:
  taiActions*	ta_actions;
  bool		ownflag;
};

//////////////////////////////////
// 	taiToken,Sub		//
//////////////////////////////////

class TA_API taiToken : public taiElBase {
  // for making menus of tokens
  Q_OBJECT
public:
  TAPtr		scope_ref;	// reference object for scoping
//  bool		over_max;	// over max_menu

  virtual void	GetImage(TAPtr ths, TAPtr scp_obj);
  virtual TAPtr	GetValue();
  override QWidget*	GetRep() { return (ta_actions == NULL) ? NULL : ta_actions->GetRep(); }
  virtual void	SetTypeScope(TypeDef* new_typ, TAPtr new_scope = NULL, bool force = false); 
   // dynamically set a new base type and/or scope; calls GetMenu
  
  virtual void	GetMenu(const taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(const taiMenuAction* actn = NULL);
  virtual void	GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* actn = NULL);


  taiToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = (flgNullOk | flgEditOk)); // uses flags flgNullOk, flgEditOk,
//            bool nul_not=false, bool edt_not=false);
protected slots:
  virtual void	Edit();		// for edit callback
  virtual void	Chooser();	// for chooser callback
  void ItemChosen(taiAction* menu_el); // when user chooses from menu
};

class TA_API taiSubToken : public taiElBase {
  // Menu for sub tokens of a giventype
  Q_OBJECT
public:
  void*		menubase;

  QWidget*	GetRep();

  virtual void	GetMenu(taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);
  virtual void	GetMenuImpl(void* base, taiMenuAction* actn = NULL);

  virtual void	GetImage(const void* ths, void* sel=NULL);
  virtual void*	GetValue();

  taiSubToken(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk
  taiSubToken(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk

public slots:
  virtual void	Edit();		// for edit callback
};

//////////////////////////////////
// 	taiMemberDef		//
//////////////////////////////////

class TA_API taiMemberDefMenu : public taiData {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
public:
  MemberDef*	md;
  TypeDef*	targ_typ;	// target type from which to get list of members -- may be same as typ, but could differ
  void*		menubase;	// the address of the object
  taiActions*	ta_actions;

  override QWidget* GetRep();


  virtual void		GetMenu(const void* base);
  virtual void 		GetImage(const void* base, bool get_menu = true, void* cur_sel = NULL);
  virtual MemberDef*	GetValue();

  taiMemberDefMenu(taiActions::RepType rt, int ft, MemberDef* m, TypeDef* targ_typ_, TypeDef* typ_,
     IDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0); // if targ_type is null, it will be determined
  ~taiMemberDefMenu();
protected:
  void GetTarget(); // determines the target type for the lookup menu
};

//////////////////////////////////
// 	taiMethodDef		//
//////////////////////////////////

class TA_API taiMethodDefMenu : public taiData {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
public:
  MethodDef*	md;
  void*		menubase;	// the address of the object
  MethodSpace*  sp;
  taiActions*	ta_actions;

  override QWidget* GetRep();

  virtual void	GetMenu(const taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(const taiMenuAction* actn = NULL);

  virtual void 	GetImage(TypeDef* type, MethodDef* memb);
  virtual void 	GetImage(MethodSpace* space, MethodDef* memb);
  virtual void	GetImage(void* ths, void* sel=NULL);
  virtual void*	GetValue();

  taiMethodDefMenu(taiActions::RepType rt, int ft, MethodDef* m, TypeDef* typ_, IDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  ~taiMethodDefMenu();
};


//////////////////////////////////
// 	taiTypeHier		//
//////////////////////////////////

class TA_API taiTypeHier : public taiData {
// for menus of type hierarchy
public:
  taiActions* 	ta_actions;
  bool		ownflag;
  bool		enum_mode; // when set, creates a hier of enum TypeDefs, for picking an enum type

  virtual void	GetMenu() {GetMenu(NULL);}
  virtual void	GetMenu(const taiMenuAction* acn);
  virtual void	UpdateMenu() {UpdateMenu(NULL);}
  virtual void	UpdateMenu(const taiMenuAction* acn);

  override QWidget* GetRep();
  virtual void		GetImage(TypeDef* ths);
  virtual TypeDef*	GetValue();

  taiTypeHier(taiActions::RepType rt, int ft, TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags_ = 0);
  taiTypeHier(taiMenu* existing_menu, TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags_ = 0);
  ~taiTypeHier();
protected:
  bool		AddType_Enum(TypeDef* typ_); 
  bool		AddType_Class(TypeDef* typ_); 
  int		CountChildren(TypeDef* typ_);
  int		CountEnums(TypeDef* typ_);
  virtual void	GetMenu_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn); 
  void	GetMenu_Enum_impl(taiActions* menu, TypeDef* typ_, const taiMenuAction* acn); 
};

//////////////////////////////////
//	taiMethMenu,Button	//
//////////////////////////////////

// unlike real taiData, functions are not subject to updating
// so the constructor is the one that does all the work..

class cssClass;			// #IGNORE
class cssiArgDialog;		// #IGNORE

class TA_API taiMethodData : public taiData {
  // all representations of member functions must inherit from this one
  Q_OBJECT
public:
  void*		base;		// base is of parent type
  MethodDef* 	meth;
  bool		is_menu_item;   // true if this is supposed to be a menu item (else QWidget*)
  cssClass*	args;
  cssiArgDialog* arg_dlg;
  int		use_argc;

  taiMethodData(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  virtual QPushButton*	GetButtonRep() {return buttonRep;}
    // button connected to method -- only created on demand by subclasses

  virtual bool	hasButtonRep() { return (buttonRep != NULL); }
  virtual void  ShowReturnVal(cssEl* rval); // show return value after menu call
  virtual void	ApplyBefore();	// apply changes before performing menu call
  virtual void	UpdateAfter();	// update display after performing menu call

  virtual void	GenerateScript(); // output script code equivalent if recording

  virtual void 	AddToMenu(taiActions* mnu);
protected:
  QPushButton*	buttonRep;
  QWidget*	gui_parent;
  QPushButton*	makeButton(); // makes the button if necessary, and returns a reference
public slots:
  virtual void 	CallFun();		// call the function (button callback)

};

class TA_API taiMethMenu : public taiMethodData {
  // all representations of member functions must inherit from this one
  // NOTE: gui_parent is the parent of the Button, since the menu item is already parented in menu
public:
  taiMethMenu(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  override QPushButton*	GetButtonRep() {return makeButton();}
  QWidget*	GetRep()		{ return (QWidget*)buttonRep; }
};

class TA_API taiMethButton : public taiMethodData {
  // button representation of a method -- uses the QPushButton, and sets it as the Rep as well
public:
  taiMethButton(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);
//  ~taiMethButton();
};

//TODO: double check proper base class (was taiMethMenu in IV)
class TA_API taiMethToggle : public taiMethodData {
  // toggle representation of a method (does not call directly, but checks flag)
  Q_OBJECT
public:
  QCheckBox* 	rep() {return (QCheckBox*)m_rep;}

  taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

public slots:
  void	CallFun(); // override
};



#endif // TA_QTDATA_H

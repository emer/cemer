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

#ifndef TA_QTDATA_H
#define TA_QTDATA_H

#include "qtdefs.h"
#include "ta_base.h"
#include "ta_qtdata_def.h"

#ifndef __MAKETA__
  #include <qdialog.h>
  #include <qobject.h>
  #include <qaction.h>
  #include <qwidget.h>

  #include "icheckbox.h"
#endif

#include "igeometry.h"

//nn? #include <unistd.h>

// externals
class QKeySequence; // #IGNORE

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

//////////////////////////////////////////////////////////
// 	taiData: glyphs to represent kinds of data	//
//////////////////////////////////////////////////////////

// class taiData;
// class   taiField;
// class   taiIncrField;
 class   taiToggle;
// class   taiLabel;
// class   taiPlusToggle;
// class   taiPolyData;
// class   taiDataDeck; // contains subdata within a deck
// class   taiMenu;
// class     taiMenu;
 class   taiToken;  // for making menus of tokens
// class   taiSubToken;   // Menu for sub tokens of a giventype
// class   taiMemberDefMenu;   // Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
// class   taiMethodDefMenu;   // Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
 class   taiTypeHier;   // for menus of type hierarchy
// class   taiMethodData;  // all representations of member functions must inherit from this one
// class     taiMethMenu;  // menu and optional button representation of a method
// class     taiMethButton;   // button representation of a method
// class     taiMethToggle;   // toggle representation of a method (does not call directly, but


class taiCompData : public taiData {
  //  #NO_INSTANCE base class for composite data elements
public:

  taiCompData(TypeDef* typ_, taiDataHost* host_, taiData* parent_, QWidget* gui_parent_, int flags = 0);
  override ~taiCompData();

  virtual QLayout*	GetLayout() {return (QLayout*)lay;}
  taiDataList  data_el;

  QWidget*	widgets(int index);
  int		widgetCount();

  virtual void	InitLayout(); // default creates a QVHBoxLayout in the Rep
  void		AddChildWidget(QWidget* child_widget, int space_after = -1);
  virtual void	EndLayout(); // default adds a stretch
protected:
  QHBoxLayout*	lay;	// may be ignored/unused by subclasses
  int		last_spc;	// space after last widget, -1 = none
  override void		ChildAdd(taiData* child);
  override void		ChildRemove(taiData* child);
  virtual void	AddChildWidget_impl(QWidget* child_widget);	// default does an add to layout
private:
  QObjectList*	mwidgets; // list of child widgets
};

class taiField : public taiData {
  Q_OBJECT
public:
  iLineEdit*		rep() const { return (iLineEdit*)m_rep; }
  taiField(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  override void		setUseHiBG(bool value) { museHiBG = value;} // this control can use background highlighting

  void 	GetImage(const String& val);
  String GetValue() const;

protected slots:
  void			selectionChanged();

protected:
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};

// this is for integers -- includes up and down arrow buttons
class taiIncrField : public taiData { // increment/decrement field
  Q_OBJECT
public:
  iSpinBox*	rep() const { return (iSpinBox*)m_rep; }
  taiIncrField(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
    // uses flags: flgPosOnly

  override void		setUseHiBG(bool value) { museHiBG = value;} // this control can use background highlighting
  void 			GetImage(int val);
  int 			GetValue() const;

protected slots:
  void			selectionChanged();

protected:
  override void 	this_GetEditActionsEnabled(int& ea); // for when control is clipboard handler
  override void 	this_EditAction(int param); // for when control is clipboard handler
  override void 	this_SetActionsEnabled(); // for when control is clipboard handler
};


class taiToggle : public taiData {
public:
  iCheckBox*	rep() const { return (iCheckBox*)m_rep; }

  taiToggle(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
//  ~taiToggle() {}

  override void		setUseHiBG(bool value) { museHiBG = value;} // this control can use background highlighting
  void 	GetImage(bool val);
  bool	GetValue() const;
};


class taiPlusToggle : public taiCompData {
// a regular field plus a toggle..
  Q_OBJECT
public:
  taiData*	data; // set by caller after creating contained class
  QCheckBox* 	but_rep;
  QFrame* 	rep() {return (QFrame*)m_rep;} //parent of contained controls

  taiPlusToggle(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
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

class taiComboBox : public taiData {
  Q_OBJECT
public:
  iComboBox*	rep() const { return (iComboBox*)m_rep; }

  taiComboBox(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiComboBox(bool is_enum, TypeDef* typ_, taiDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  override void		setUseHiBG(bool value) { museHiBG = value;} // this control can use background highlighting
  void		AddItem(const String& val); // add an item to the list
  void		Clear(); //clears all items (only needed if refilling list)

  void 		GetImage(int itm);  // set to this item number
  void		GetValue(int& itm) const;

#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-taiDataHost users, forwards chkbox signal
#endif

private:
  void		Initialize(QWidget* gui_parent_);
};

//////////////////////////
//     taiBitBox	//
//////////////////////////

class iBitCheckBox: public iCheckBox { // #IGNORE specialized checkbox for the taiBitBox class
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



class taiBitBox : public taiData { // supports enums that are bit fields (TODO: maybe support int fields too)
  Q_OBJECT
public:
  QFrame*	rep() const { return (QFrame*)m_rep; }

  taiBitBox(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiBitBox(bool is_enum, TypeDef* typ_, taiDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0); // treats typ as enum, and fills values

  //TODO: maybe default highlighting???
  void		AddBoolItem(String name, int val); // add an item to the list

  void 		GetImage(int val);  // set to this value, according to bit fields
  void		GetValue(int& val) const;

public slots:
  void		bitCheck_toggled(iBitCheckBox* sender, bool on); // #IGNORE

#ifndef __MAKETA__
signals:
  void		itemChanged(int itm); // for use by non-taiDataHost users, forwards chkbox signal
#endif

protected:
  QHBoxLayout* 	lay; //#IGNORE
  int		m_val; //#IGNORE
private:
  void		Initialize(QWidget* gui_parent_);
};

//////////////////////////
//     taiPolyData	//
//////////////////////////

// this class supports the use of hierarchical sub-data within a data item
// its default behavior is to put everything in an hbox with labels

class taiPolyData : public taiCompData {
public:
  QFrame*	rep() const { return (QFrame*)m_rep; } //note: actual class may be subclass of QFrame
  int		show;

  taiPolyData(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
  ~taiPolyData();

  void  	GetImage(void* base);
  void	 	GetValue(void* base);

protected:
  virtual void	Constr(QWidget* gui_parent_);
  virtual bool	ShowMember(MemberDef* md);
};

//////////////////////////
//     taiDataDeck	//
//////////////////////////

// contains sub-data's within a deck (aka Qt Stack) -- can toggle between them

class taiDataDeck : public taiCompData {
public:
  QWidgetStack*	rep() {return (QWidgetStack*)m_rep;}
  int		cur_deck; // current deck for operations

  void		setNumDecks(int n); // sets # decks

  override void	InitLayout() {} // nothing in this class
  override void	EndLayout() {} // nothing in this class
  taiDataDeck(TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);

  virtual void  GetImage(int i);
protected:
  override void AddChildWidget_impl(QWidget* child_widget);
};


//////////////////////////////////////////////////////////
// 		Menus					//
//////////////////////////////////////////////////////////

class taiMenu;
class taiMenuEl;


class taiMenuEl: public QObject {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS holds menu data -- can be the root item of a submenu
  Q_OBJECT
friend class taiMenu;
friend class taiMenuEl_List;
public:
  enum CallbackType {
    none,		// callback parameters ignored
    action,		// corresponds to IV "action" type of callback -- slot for signal is parameterless
    men_act,		// corresponds to IV "men_act" type of callback -- slot for signal takes taiMenuEl* as a sender
    int_act,		// corresponds to Qt menu/action signals -- slot for signal takes an int parameter
    ptr_act		// slot for signal takes a void* parameter
  };

  taiMenu*	owner;
  QMenuItem* 	rep;
  int		sel_type;
  String	label;
  void*		usr_data; // (note: we also overload this as "int param" for actions)
  int		radio_grp;	// all radio items in a group are contiguous, and share the same value;

  int			id(); // the MenuItem id, or -1 if no menu item (shouldn't happen)
  int			index() {return m_index;} // note: this is our own index in menu_el_list, not the underlying menu index in menu
  taiMenuEl(taiMenu* owner_, QMenuItem* item_, int radio_grp_, int sel_type_, const String& label_,
      void* usr_data_, CallbackType ct_ = none, const QObject *receiver = NULL, const char* member = NULL); // 'member' is result of Qt SLOT() macro
  virtual ~taiMenuEl();

  void connect(CallbackType ct_, const QObject *receiver, const char* member); // connect callback to given
  void connect(const taiAction* actn); // connect Action to given callback (ignored if NULL)
  void connect(const taiMenuAction* actn);  // connect MenuAction to given callback (ignored if NULL)

  bool 			hasCallbacks() { return (connectCount > 0); }
  virtual QPopupMenu*	SubMenu() 	{ return NULL; }
  bool			canSelect(); // true if item can be taiMenu::curSel value
  virtual bool		isSubMenu() 	{ return false; }
  bool			isChecked(); // #GET_Checked returns 'true' if a radio or toggle item, and checked, false otherwise
  void			setChecked(bool value); // #SET_Checked

#ifndef __MAKETA__
signals:
  void Action();			// for a direct call-back
  void MenuAction(taiMenuEl* sender);	// for a menu-action call-back (returns item selected)
  void IntParamAction(int param);	// for Qt-style parameter ((int)usr_data)
  void PtrParamAction(void* ptr);	// for ptr parameter (usr_data)
#endif // ndef __MAKETA__

public slots:
  virtual void	Select();	// slot for selecting an menu item, which then decides
				// if action or men_act should be called subsequently
protected:
  int		connectCount;
  int		m_index; //ordinal position in parent; note: not the same as its id
  void emitAction();
  void emitMenuAction();
  void emitIntParamAction();	// for Qt-style parameter ((int)usr_data)
  void emitPtrParamAction();	// for ptr parameter (usr_data)
  void Select_impl(bool selecting); // called by Select(), taiMenu::setCurSel and by taiMenu::GetImage -- doesn't trigger events
};

class taiSubMenuEl: public taiMenuEl {
public:
  taiMenu* 		sub_menu_data; // the taiMenu for this submenu
  taiMenuAction 	default_child_action;

  taiSubMenuEl(taiMenu* owner_, QMenuItem* item_, const String& label_,
      void* usr_data_, QPopupMenu* sub_menu_, taiMenu* sub_menu_data,
      const taiMenuAction* default_child_action_ = NULL); //
  override ~taiSubMenuEl();

  override QPopupMenu*	SubMenu() 	{ return sub_menu; }
  override bool		isSubMenu() 	{ return true; }
protected:
  QPopupMenu*		sub_menu;
};

class taiMenuEl_List : public taPtrList<taiMenuEl> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (taiMenuEl*)it; }
  void	El_SetIndex_(void* it, int idx) 	{((taiMenuEl*)it)->m_index = idx;}

public:
  ~taiMenuEl_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiMenuEl*)it)->label); }
};


//////////////////////////////////
// 	    taiMenu		//
//////////////////////////////////

class taiMenu : public taiData {
  // (possibly) hierarchical menu for selecting a single item
  Q_OBJECT
friend class taiMenu_List; // hack because lists return refs to strings, not values
public:
  enum RepType {
    menubar,			// represent menu as a menubar
    popupmenu,			// represent menu as a popup menu (for use in menu bars, submenus, or user-supplied buttons)
    buttonmenu			// menu invoked from a button
  };
//    menuitem, 			// represent menu as menuitem (for use in a bigger bar)
  enum SelType {
    st_none =		0x00,	// no action on selection -- primarily for SubItem entries
    normal =		0x01,	// selecting the item causes an action; no persistent check mark by menu item
    normal_update =	0x81,
    radio = 		0x02,	// only 1 of n items on the menu can be selected; selecting on deselects any other
    radio_update =	0x82,
    toggle =		0x04,	// item can be checked or unchecked, independent of any other item
    toggle_update =	0x84,

    use_default =	0x10,	// only for taiMenu::additem -- inherits from its menu
    submenu =		0x20,	// only for taiMenu::makeitem

    update = 		0x80	// flag added to normal selection types, causes parent item to indicate Changed when item selected
  };

  RepType		rep_type;
  SelType		sel_type;
  int			font_spec; //taiMisc::FontSpec
  taiMenuEl_List	items;
  QWidget*		gui_parent; // needed for submenus

  taiMenu(int rep_type_, int  sel_type_, int font_spec_, TypeDef* typ_, taiDataHost* host,
      taiData* par, QWidget* gui_parent_, int flags_ = 0, taiMenu* par_menu_ = NULL);
//nbg  taiMenu(int rt, int st, int ft, QWidget* gui_parent_); // constructor for WinBase and other non-taiDialog uses
  taiMenu(QWidget* gui_parent_, int rep_type_ = popupmenu, int sel_type_= normal,
      int font_spec_ = 0, QMenuData* exist_menu = NULL);
    // constructor for Browser and context menus ft=0 means default font size; (optional) exist_menu MUST be proper type according to rep_type
  ~taiMenu();

  taiMenuEl*		curSel() const;
  void			setCurSel(taiMenuEl* value);
  String		label() const	{ return mlabel; } //#GET_Label
  void			setLabel(const String& val); // #SET_Label replaces the SetMLabel call in IV
  QMenuData*		menu();		 // generic represenation
  QMenuBar*		rep_bar()	{ return (rep_type == menubar) ? mrep_bar : NULL; } // strongly typed, safe
  QPopupMenu*		rep_popup()	{ return (rep_type == popupmenu) ? mrep_popup : NULL; } // strongly typed, safe

  virtual taiMenuEl*	GetValue()	{ return cur_sel; }
  virtual bool 		GetImage(void* usr); // set to this usr item, uses a default if not found
  void 			GetImageByIndex(int item);

//NOTE: pass slot with no param for "ct=action", and slot with QObject* (sender) for "ct=men_act"
  taiMenuEl* 		AddItem(const char* val, void* usr, SelType st, const taiAction* acn) ;
  taiMenuEl* 		AddItem(const char* val, void* usr, SelType st, const taiMenuAction* acn) ;
  taiMenuEl* 		AddItem(const char* val, void* usr, SelType st,	taiMenuEl::CallbackType ct, const taiMenuAction& acn)
    {return AddItem(val, usr, st, ct, acn.receiver, acn.member);}
  taiMenuEl* 		AddItem(const char* val, void* usr = NULL, SelType st = use_default,
	taiMenuEl::CallbackType ct = taiMenuEl::none,
        const QObject *receiver = NULL, const char* member = NULL, const QKeySequence* accel = NULL);
    // 'member' is the result of the SLOT() macro
  taiMenuEl* 		AddItem_FromAction(iAction* act); // #IGNORE only used by iAction, which adds the menu item
  void			DeleteItem(uint index); // deletes the indicated item -- deletes the gui representation as well
  virtual void		AddSep(bool new_radio_grp = false); // add menu separator -- can also be used to create new radio groups --  won't add 2 separators in a row
  virtual void		NewRadioGroup() {++cur_radio_grp;}	// start a new radio group (must also preceed first group)
  taiMenu*		AddSubMenu(const char* val, void* usr = NULL, SelType st = use_default,
  	const taiMenuAction* default_child_action = NULL, TypeDef* typ_ = NULL); // returns new sub; dca will get added automatically to all sub items (but not recursively) -- NOTE: action items generally require the menu have a TypeDef!!!
  taiMenu*		AddSubMenu(const char* val, TypeDef* typ_) { return AddSubMenu(val, NULL, use_default,
    				NULL, typ_);} // for adding popup menus to main menus
  taiMenu*		AddSubMenu(const char* val, const taiMenuAction* default_child_action);
  taiMenu*		AddSubMenu(const char* val, const taiMenuAction& default_child_action);
  taiMenu*		FindSubMenu(const char* nm); // find specified submenu, or NULL if not found
  virtual void		ResetMenu();

  taiMenuEl*		operator[](int index) const {return items.SafeEl(index);}

  int			count() const {return items.size;} // qt compatability -- note that count won't have separators
  int			exec (const iPoint& pos, int indexAtPoint = 0);
  int			insertItem(const char* val, const QObject *receiver = NULL, const char* member = NULL,
    const QKeySequence* accel = NULL); // compatability routine with QMenuData, returns id; slot is parameterless
  void			insertSeparator() {AddSep();} // Qt-convenience

#ifndef __MAKETA__
signals:
  void labelChanged(const char* val);
#endif

protected:
  String		mlabel; // string contents of current menu label
  QMenuBar*		mrep_bar;
  QPopupMenu*		mrep_popup;
  QPushButton*		button; // when used as popup edit button -- note that EditButton creates/manages its own button
  taiMenu*		par_menu; // parent menu, if any -- many methods delegate their calls upward if there is a parent
  taiMenuEl*		cur_sel;  // selection for getting value of menu -- only used by top-level menu
  int			cur_radio_grp;	// current id of current radio group -- bumped by AddSep (ok to have unused group ids)
  taiSubMenuEl*		par_menu_el; // parent submenu element, if any
  void			init(int rt, int st, int ft, QWidget* gui_parent_,
      taiMenu* par_menu_, QMenuData* exist_menu = NULL); // #IGNORE -- exist_menu MUST be proper type (main/popup)
  void 			ConstrBar(QWidget* gui_parent_, QMenuBar* exist_bar = NULL); // #IGNORE
  void 			ConstrPopup(QWidget* gui_parent_, QPopupMenu* exist_popup = NULL); // #IGNORE
  void emitLabelChanged(const char* val); // #IGNORE
  taiSubMenuEl*		AddSubItem(const char* val, void* usr, QPopupMenu* child, taiMenu* sub_menu_data,
  				const taiMenuAction* default_child_action); // #IGNORE
  virtual QMenuItem*	NewItem(const char* val, SelType st, QPopupMenu* child = NULL,
      const QKeySequence* accel = NULL); // #IGNORE
  virtual bool 		GetImage_impl(void* usr);  // #IGNORE set to this usr item, returns false if not found -- recursive for hiermenus
};

//////////////////////////////////
// 	taiMenu_List		//
//////////////////////////////////

//NOTE: this class provided for winbase.h and others

class taiMenu_List : public taPtrList<taiMenu> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ if (own_items) delete (taiMenu*)it; }

public:
  bool own_items; //generally false
  taiMenu_List()            { own_items = false; }
  ~taiMenu_List()            { Reset(); }
  virtual String El_GetName_(void* it) const { return (((taiMenu*)it)->mlabel); }
};


//////////////////////////////////
// 	iAction		//
//////////////////////////////////

/*
  An iAction encapsulates a menu, toolbar, accelerator, etc. user action. We don't
  use Actions in Edit Dialogs, but we do use them in DataViewers; they work with
  taiMenu, which is why we define it here. iAction inherits Qt's QAction.
*/

class iAction: public QAction {
  Q_OBJECT
public:
  iAction(int param_, QObject* parent, const char* name = 0)
      :QAction(parent, name) {init(param_);}
  iAction(int param_, const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0)
      :QAction(menuText, accel, parent, name) {init(param_);}
  iAction(int param_, const QIconSet& icon, const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0 )
      :QAction(icon, menuText, accel, parent, name) {init(param_);}

  iAction(QObject* parent, const char* name = 0)
      :QAction(parent, name) {init();}
  iAction(const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0)
      :QAction(menuText, accel, parent, name) {init();}
  iAction(const QIconSet& icon, const QString& menuText, QKeySequence accel, QObject* parent, const char* name = 0 )
      :QAction(icon, menuText, accel, parent, name) {init();}

  void		AddTo(taiMenu* menu); // adds to a taiMenu, using the text, accel key, respects toggle, sets the param in the usr_data

#ifndef __MAKETA__
signals:
  void activated(int param);
#endif
protected:
  QAction*	m_action;
  int 		param;
  void addedTo(int index, QPopupMenu * menu ); // override -- set param too
  void init(int param_ = -1); // -1 generally indicates none

protected slots:
  void this_activated();
};

class iAction_List: public QPtrList<iAction> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS corresponds to a menu item and/or toolbar button -- items are stickily created as needed (browsewin creates the common set)

  override String 	El_GetName_(void*) const; // name is Q object name
};

//////////////////////////////////
// 	taiObjChooser		//
//////////////////////////////////

class taiObjChooser: QDialog {
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
  QListBox* 		browser; 	// list of items
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
  void 		browser_selectionChanged(QListBoxItem* itm);
  void 		browser_doubleClicked(QListBoxItem* itm);
  virtual void 	DescendBrowser();
  virtual void 	AcceptEditor();
};

//////////////////////////////////
// 	taiFileButton		//
//////////////////////////////////

class taiFileButton : public taiData {
  Q_OBJECT
public:
  taiMenu* 	filemenu;	// has the menu options
  taFiler*	gf;
  bool		read_only;	// only reading streams is an option
  bool		write_only;	// only writing streams are available

  virtual void	GetImage(void* base);
  virtual void  GetImage();
  virtual void* GetValue();

  virtual void	GetGetFile();	// make sure we have a getfile..
  override QWidget* GetRep();

  taiFileButton(void* base, TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_,
		  bool rd_only = false, bool wrt_only = false);
  ~taiFileButton();

public slots:
  virtual void	Open();		// callback
  virtual void	Save();		// callback
  virtual void	SaveAs();	// callback
  virtual void	Append();	// callback
  virtual void	Close();	// callback
  virtual void	Edit();		// callback
};


//////////////////////////////////
// 	taiElBase		//
//////////////////////////////////

// base class for sundry taiData items that use a menu, and have a taBase-derived current item

class taiElBase: public taiData {
public:
  TAPtr		cur_obj;
//  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  virtual void  GetMenu(taiMenu* menu, taiMenuAction* actn = NULL) {} // variant provided for MenuGroup_impl in winbase
  void 		setCur_obj(TAPtr value, bool do_chng = true); // set cur_obj and notifies change if different
  taiElBase(taiMenu* menu_, TypeDef* tp, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiElBase();
protected:
  taiMenu*	ta_menu;
  bool		ownflag;
};

//////////////////////////////////
// 	taiToken,Sub		//
//////////////////////////////////

class taiToken : public taiElBase {
  // for making menus of tokens
  Q_OBJECT
public:
  TAPtr		scope_ref;	// reference object for scoping
//  bool		over_max;	// over max_menu

  virtual void	GetImage(TAPtr ths, TAPtr scp_obj);
  virtual TAPtr	GetValue();
  override QWidget*	GetRep() { return (ta_menu == NULL) ? NULL : ta_menu->GetRep(); }

  virtual void	GetMenu(const taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(const taiMenuAction* actn = NULL);
  virtual void	GetMenu_impl(taiMenu* menu, TypeDef* typ_, const taiMenuAction* actn = NULL);


  taiToken(int rt, int ft, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = (flgNullOk | flgEditOk)); // uses flags flgNullOk, flgEditOk,
//            bool nul_not=false, bool edt_not=false);
protected slots:
  virtual void	Edit();		// for edit callback
  virtual void	Chooser();	// for chooser callback
  void ItemChosen(taiMenuEl* menu_el); // when user chooses from menu
};

class taiSubToken : public taiElBase {
  // Menu for sub tokens of a giventype
  Q_OBJECT
public:
  void*		menubase;

  QWidget*	GetRep();

  virtual void	GetMenu(taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);
  virtual void	GetMenuImpl(void* base, taiMenuAction* actn = NULL);

  virtual void	GetImage(void* ths, void* sel=NULL);
  virtual void*	GetValue();

  taiSubToken(int rt, int ft, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk
  taiSubToken(taiMenu* existing_menu, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = flgEditOk); // uses flgNullOk, and flgEditOk

public slots:
  virtual void	Edit();		// for edit callback
};

//////////////////////////////////
// 	taiMemberDef		//
//////////////////////////////////

class taiMemberDefMenu : public taiData {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
public:
  MemberDef*	md;
  TypeDef*	targ_typ;	// target type from which to get list of members -- may be same as typ, but could differ
  void*		menubase;	// the address of the object
  taiMenu*	ta_menu;

  override QWidget* GetRep();


  virtual void		GetMenu(void* base);
  virtual void 		GetImage(void* base, bool get_menu = true, void* cur_sel = NULL);
  virtual MemberDef*	GetValue();

  taiMemberDefMenu(int rt, int ft, MemberDef* m, TypeDef* targ_typ_, TypeDef* typ_,
     taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0); // if targ_type is null, it will be determined
  ~taiMemberDefMenu();
protected:
  void GetTarget(); // determines the target type for the lookup menu
};

//////////////////////////////////
// 	taiMethodDef		//
//////////////////////////////////

class taiMethodDefMenu : public taiData {
// Menu for memberdefs of a typedef in the object with a MDTYPE_xxx option
public:
  MethodDef*	md;
  void*		menubase;	// the address of the object
  MethodSpace*  sp;
  taiMenu*	ta_menu;

  override QWidget* GetRep();

  virtual void	GetMenu(const taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(const taiMenuAction* actn = NULL);

  virtual void 	GetImage(TypeDef* type, MethodDef* memb);
  virtual void 	GetImage(MethodSpace* space, MethodDef* memb);
  virtual void	GetImage(void* ths, void* sel=NULL);
  virtual void*	GetValue();

  taiMethodDefMenu(int rt, int ft, MethodDef* m, TypeDef* typ_, taiDataHost* host, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  ~taiMethodDefMenu();
};


//////////////////////////////////
// 	taiTypeHier		//
//////////////////////////////////

class taiTypeHier : public taiData {
// for menus of type hierarchy
public:
  taiMenu* ta_menu;
  bool		ownflag;

  virtual void	GetMenu() {GetMenu(NULL);}
  virtual void	GetMenu(const taiMenuAction* acn);
  virtual void	UpdateMenu() {UpdateMenu(NULL);}
  virtual void	UpdateMenu(const taiMenuAction* acn);
  virtual int	CountChildren(TypeDef* typ_);
  virtual bool	AddType(TypeDef* typ_); 	// true if type should be added

  override QWidget* GetRep();
  virtual void		GetImage(TypeDef* ths);
  virtual TypeDef*	GetValue();

  taiTypeHier(int rt, int ft, TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  taiTypeHier(taiMenu* existing_menu, TypeDef* typ_, taiDataHost* host, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  ~taiTypeHier();
protected:
  virtual void	GetMenu_impl(taiMenu* menu, TypeDef* typ_, const taiMenuAction* acn);
};

//////////////////////////////////
//	taiMethMenu,Button	//
//////////////////////////////////

// unlike real taiData, functions are not subject to updating
// so the constructor is the one that does all the work..

class cssClass;			// #IGNORE
class cssiArgDialog;		// #IGNORE

class taiMethodData : public taiData {
  // all representations of member functions must inherit from this one
  Q_OBJECT
public:
  void*		base;		// base is of parent type
  MethodDef* 	meth;
  bool		is_menu_item;   // true if this is supposed to be a menu item (else QWidget*)
  cssClass*	args;
  cssiArgDialog* arg_dlg;
  int		use_argc;

  taiMethodData(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  virtual QPushButton*	GetButtonRep() {return buttonRep;}
    // button connected to method -- only created on demand by subclasses

  virtual bool	hasButtonRep() { return (buttonRep != NULL); }
  virtual void  ShowReturnVal(cssEl* rval); // show return value after menu call
  virtual void	ApplyBefore();	// apply changes before performing menu call
  virtual void	UpdateAfter();	// update display after performing menu call

  virtual void	GenerateScript(); // output script code equivalent if recording

  virtual void 	AddToMenu(taiMenu* mnu);
protected:
  QPushButton*	buttonRep;
  QWidget*	gui_parent;
  QPushButton*	makeButton(); // makes the button if necessary, and returns a reference
public slots:
  virtual void 	CallFun();		// call the function (button callback)

};

class taiMethMenu : public taiMethodData {
  // all representations of member functions must inherit from this one
  // NOTE: gui_parent is the parent of the Button, since the menu item is already parented in menu
public:
  taiMethMenu(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

  override QPushButton*	GetButtonRep() {return makeButton();}
  QWidget*	GetRep()		{ return (QWidget*)buttonRep; }
};

class taiMethButton : public taiMethodData {
  // button representation of a method -- uses the QPushButton, and sets it as the Rep as well
public:
  taiMethButton(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);
//  ~taiMethButton();
};

//TODO: double check proper base class (was taiMethMenu in IV)
class taiMethToggle : public taiMethodData {
  // toggle representation of a method (does not call directly, but checks flag)
  Q_OBJECT
public:
  QCheckBox* 	rep() {return (QCheckBox*)m_rep;}

  taiMethToggle(void* bs, MethodDef* md, TypeDef* typ_, taiDataHost* host, taiData* par,
      QWidget* gui_parent_, int flags_ = 0);

public slots:
  void	CallFun(); // override
};



#endif // TA_QTDATA_H

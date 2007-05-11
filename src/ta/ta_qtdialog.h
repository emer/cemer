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


// ta_qtdialog.h: Qt-based data hosts and dialogs

#ifndef TA_QTDIALOG_H
#define TA_QTDIALOG_H//

#include "ta_type.h"
#include "ta_qt.h"
#include "ta_qtdata.h" //for taiMenu_List
#include "ta_qtviewer.h"

#ifndef __MAKETA__
# include <ilabel.h>
# include <QEvent>
# include <QMessageBox>
# include <QObject>
# include <QPushButton>
#endif

// externals
class taiDataLink;  //
class iDataPanel; //

// forwards
class taiDataHostBase;
class taiDataHost;
class taiEditDataHost; 
class taiStringDataHost; //


//////////////////////////////////
// 	HiLightButton		//
//////////////////////////////////

class TA_API HiLightButton: public QPushButton {
  // combines v3.2 Script Button and HiLightButton -- set script_ to script name, to enable ScriptButton functionality
public:
  int			mouse_button;	// (Qt::Button) the mouse button that pressed the button (defaults to NoButton, when actuated by keyboard)

  bool			hiLight() {return mhiLight;} // #GET_HiLight
  virtual void		setHiLight(bool value); // #SET_HiLight
  HiLightButton(QWidget* parent = 0, const char* script_ = 0);
  HiLightButton(const String& text, QWidget* parent, const char* script_ = 0);
  ~HiLightButton();

protected:
  iColor		mhiLight_color;
  bool 			mhiLight;
  String		mscript;

  void			init(const char* script_); // #IGNORE
  void 			released(); // override
/*no more hacks!!!  override void		mousePressEvent(QMouseEvent* mev); //we hack these to accept right mouse click
  override void		mouseReleaseEvent(QMouseEvent* mev);
  override void		mouseMoveEvent(QMouseEvent*  mev); */
};


//////////////////////////
// 	Script Button 	//
//////////////////////////
/* TODO:
// this button generates script when pressed
class ScriptButton : public ivButton{ // #IGNORE
public:
  String script;
  ScriptButton(char* name, ivAction *a, char* srp);
  void release(const ivEvent&);
};


*/ //TODO

class taiToken;
class taiTypeHier;

//////////////////////////////////
// 	taiChoiceDialog		//
//////////////////////////////////

// Button 0 is the default if user presses Enter
// Button 1 is the Cancel, if user presses Esc (only if "no_cancel" option false)
// User can also press the number associated with the button, starting from 0

class TA_API taiChoiceDialog : public QMessageBox {
  Q_OBJECT
INHERITED(QMessageBox)
public:
  static const String	delimiter;
  
  static int	ChoiceDialog(QWidget* parent_, const String& msg, const String& but_list, 
    const char* win_title = "");

  static void 	ErrorDialog(QWidget* parent_, const char* msg, 
    const char* win_title = "Error", bool copy_but = true);
    
  static void 	ConfirmDialog(QWidget* parent_, const char* msg, 
    const char* win_title = "", bool copy_but = true);

  QButtonGroup* bgChoiceButtons; //note: not a widget

  override void	done(int r);
  
  ~taiChoiceDialog()		{ }

protected:
  int		num_chs; // number of choices, we return last idx for Esc
  virtual void keyPressEvent(QKeyEvent* ev); // override

protected slots:
  void		copyToClipboard(); // copy text of msg to clipboard

#ifndef __MAKETA__
private:
  taiChoiceDialog(Icon icon, const QString& title, const QString& text, String but_list, QWidget* parent, bool copy_but);
  
  virtual QAbstractButton* Constr_OneBut(String lbl, int id, ButtonRole role);
#endif
};

//////////////////////////
// 	iDialog		//
//////////////////////////

class TA_API iDialog : public QDialog { // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
  Q_OBJECT
friend class taiDataHostBase;
public:
  iDialog(taiDataHostBase* owner_, QWidget* parent = 0, int wflags = 0);
  ~iDialog();

  bool		post(bool modal); // simplified version of post_xxx routines, returns true if accepted or false (if modal) if cancelled
  void		dismiss(bool accept_);

  void		iconify();   // Iv compatibility routine
  void		deiconify(); // Iv compatibility routine
  void		setCentralWidget(QWidget* widg); // is put in a scroll area; dialog is limited to screen size
  void		setButtonsWidget(QWidget* widg); // is put at the bottom, not in a scroll
protected:
  taiDataHostBase* 	owner;
  QVBoxLayout*  layOuter;
  QWidget* 	mcentralWidget;
  QScrollArea*	scr;
  override void closeEvent(QCloseEvent* ev);
};


//////////////////////////
// 	EditDataPanel	//
//////////////////////////

class TA_API EditDataPanel: public iDataPanelFrame { // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(iDataPanelFrame)
friend class taiEditDataHost;
public:
  iColor*		bgcol; // temp holding spot
  taiEditDataHost*	editDataHost() {return owner;}
  override String	panel_type() const; // this string is on the subpanel button for this panel
  override void		Closing(CancelOp& cancel_op);
  override const iColor GetTabColor(bool selected, bool& ok) const; // special color for tab; NULL means use default
  override bool		HasChanged(); // 'true' if user has unsaved changes -- used to prevent browsing away
  override void		Refresh(); // always do it, even when hidden; the edit sorts it out
  override void 	GetImage() {GetImage_impl();} // edit handles HasChanged stuff

  EditDataPanel(taiEditDataHost* owner_, taiDataLink* dl_);
  ~EditDataPanel();

public: // IDataLinkClient interface
  override TypeDef*	GetTypeDef() const {return &TA_EditDataPanel;}

protected:
  taiEditDataHost* 	owner;
  override void		GetImage_impl(); // sheesh, THIS is the refresh guy!
  override void		Render_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};


class TA_API iMethodButtonMgr: public QObject, virtual public IDataLinkClient
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS an that can be conveniently used anywhere to provide the meth buttons of an edit -- note: requires an IDataHost and gui objects
INHERITED(QObject)
  Q_OBJECT
public:
  inline QLayout*	lay() const {return m_lay;} 

  bool			show_meth_buttons; // set when we have any guys to show
  

  void			setBase(taBase* value);
  
  void 			Constr(taBase* base,
    IDataHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void 			Constr(QWidget* widg, QLayout* lay, taBase* base,
    IDataHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void 			AddMethButton(taiMethodData* mth_rep,
    const String& label = _nilString);
  void 			GetImage();
  void			Reset(); 

  iMethodButtonMgr(QObject* parent = NULL);
  iMethodButtonMgr(QWidget* widg, QLayout* lay, QObject* parent = NULL);
  ~iMethodButtonMgr();
  
protected:
  QWidget*		widg; // the host widget
  QLayout*		m_lay; // usually an iFlowLayout or QHBoxLayout, margins/spacing set
  IDataHost*	host; // must have outer lifetime to us!
  taBase*		base; // the object that has the methods
  TypeDef*		typ;
  taiMenu_List		ta_menu_buttons; // menu representations (from methods -- menubuttons only)
  taiActions*		cur_menu_but; // current menu button to add to (if not otherwise   
  taiDataList 		meth_el;	// method elements
  
  void 			Constr_impl(taBase* base, IDataHost* host); // #IGNORE 
  virtual void 		Constr_Methods_impl(); // #IGNORE
  void 			DoAddMethButton(QAbstractButton* but); // #IGNORE
  void 			SetCurMenuButton(MethodDef* md);

public: // ITypedObject i/f (common to IDLC and IDH)
  void*		This() {return this;} // override
  TypeDef* 	GetTypeDef() const {return &TA_QWidget;/*&TA_iMethodButtonMgr;*/} // override
public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
  void		DataLinkDestroying(taDataLink* dl); 
  void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  
private:
  void		Init();
};

//TODO: taiDHB should be used to rebase taiDH
class TA_API taiDataHostBase: public QObject, virtual public IDataLinkClient
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(QObject)
  Q_OBJECT
friend class iDialog;
public:
  enum Dlg_State {
    EXISTS		= 0x01,
    DEFERRED1, // only the first part of entire construction
    CONSTRUCTED,
    ACTIVE,
    ACCEPTED,
    CANCELED,
    ZOMBIE, // for when gui stuff deleted before we did
    STATE_MASK		= 0x0F, // #NO_SHOW
    SHOW_CHANGED	= 0x80	// #NO_SHOW flag to indicate what to show was changed, reconstruct!
  };
  
  enum HostType {
    HT_DIALOG,		// host/owner is an iDialog (legacy "Edit" behavior)
    HT_PANEL,		// host/owner is an EditPanel ("properties" panel)
    HT_CONTROL		// host/owner is a control -- we won't show menus or obj buttons
  };
  
#ifndef __MAKETA__
  enum CustomEventType {
    CET_RESHOW		= QEvent::User + 1,  // uses ReShowEvent
    CET_GET_IMAGE,
    CET_APPLY
  };
#endif

  static void	DeleteChildrenLater(QObject* obj); // convenience function -- deleteLater all children
  static void   MakeDarkBgColor(const iColor& bg, iColor& dk_bg); // for use by other users of stripe grids, to make the right dk bg color

  int		ctrl_size;	// a taiMisc::SizeSpec-compatible value (ie, from FontSpec) -- def is taiM->ctrl_size
  bool          read_only;	// cannot change data
  TypeDef*	typ;		// type of object (if relevant)
  void*		cur_base;	// current base pointer of object (if relevant)
  int		state;		// Dlg_State value -- indicates state of construction of dialog
  bool		modal;	// true if dialog is of the modal variety, always false for edit panels
  bool		no_ok_but;	// no ok button
  int		mouse_button;	// (Qt::ButtonState) the mouse button that pressed ok

  String	prompt_str;	// string that goes inside as a prompt or label
  String	win_str;	// string that goes on the window frame or similar area

  QVBoxLayout*	vblDialog;	// layout for the entire dialog -- stacked/nested as follows:
  QLabel*	prompt;		// informative message at top of dialog
  QWidget*	    body;	// parent for the body items
  QWidget*	widButtons; // box of buttons on the bottom of the dialog (unparented -- added to parent later
  QHBoxLayout*	layButtons;	
  HiLightButton*  okbut; // is HilightButton for the special mouse button handling
  HiLightButton*  canbut;// is HilightButton for the special mouse button handling
  HiLightButton*  apply_but;	// only use for dialogs that wait around
  HiLightButton*  revert_but;

  iColor		bgColor() const {return bg_color;}
  virtual void		setBgColor(const iColor& new_bg); // #SET_bg_color

  inline bool		isDialog() {return (host_type == HT_DIALOG);} 
    // 'true' when we will be been posted as a dialog
  inline bool		isPanel() {return (host_type == HT_PANEL);} 
    // 'true' when we will be shown in a panel
  inline bool		isControl() {return (host_type == HT_CONTROL);} 
    // 'true' when shown in a control
  QWidget* 	widget() {return mwidget;}

  taiDataHostBase(TypeDef* typ_ = NULL, bool read_only_ = false, 
    bool modal_ = false, QObject* parent = 0);
  virtual ~taiDataHostBase(); //

  void		ClearBody();	// prepare dialog for rebuilding Body to show new contents

  void  Constr(const char* prompt = "", const char* win_title = "",
    HostType host_type = HT_DIALOG, bool deferred = false);
    //NOTE: if built with as_panel=true, then must only be a panel, not dialog, and viceversa
  void  ConstrDeferred(); // finish deferred construction
  virtual int 		Edit(bool modal_ = false); // for dialogs -- creates iDialog
/*  virtual void		Iconify(bool value);	// for dialogs: iconify/deiconify
  virtual void 		ReConstr_Body(); // called when show has changed and body should be reconstructed -- this is a deferred call */
  virtual void  Unchanged();	// call when data has been saved or reverted
  virtual void		Refresh(); // does a GetImage or defered Reshow
/*  virtual void	Revert_force();	// forcibly (automatically) revert buffer (prompts)
  virtual void  SetRevert();	// set the revert button on
  virtual void  UnSetRevert();	// set the revert button off
  virtual bool		ReShow(bool force = false); // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
  virtual void		ReShow_Async(bool force = false); // reshow asynchronously; can be called multiple times before the reshow (only done once)*/
  virtual void		GetImage_Async(){} // refresh asynchronously; can be called multiple times (only done once)
  /*virtual void	Raise() {if (isDialog()) DoRaise_Dialog();}	// bring dialog or panel (in new tab) to the front*/
  virtual void 		ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL) {}
  virtual void		WidgetDeleting(); // lets us null the gui fields, and set state
  
public: // ITypedObject i/f (common to IDLC and IDH)
  void*		This() {return this;} // override
  TypeDef* 	GetTypeDef() const {return &TA_taiDataHostBase;} // override

public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
//  bool		ignoreDataChanged() const; we always accept, but respect hidden
  void		DataLinkDestroying(taDataLink* dl); 
  void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);


// virtuals for IDataHost i/f -- call back to these from taiDataHost
  virtual void		GetImage() {} // IDH brings this in too, we override in EDH
  virtual void		GetImage(bool force) {} // ugly hack
  virtual void		GetValue() {} // IDH brings this in too, we override in EDH
public slots:
  virtual void		Apply(); 
  virtual void  	Changed();	// override method call when data has changed

public slots:
  virtual void		Revert(); 
  virtual void 		Ok(); // for dialogs
  void 			Cancel(); // mostly for dialogs, but also used internally to close a Panel (ex. when obj deleted)

protected:
  ContextFlag		updating; // flag to indicate we are the instance that caused the update
  bool			modified;
  bool			warn_clobber; // was changed elsewhere while edited here; warn user before saving
  QWidget*		mwidget;	// outer container for all widgets
  iDialog*		dialog; // dialog, when using Edit, NULL otherwise
  HostType		host_type; // hint when constructed to tell us if we are a dialog or panel -- must be consistent with dialog/panel
  iColor 		bg_color; // background color of host -- set via setBgColor
  iColor		bg_color_dark;	// background color of dialog, darkened (calculated when bg_color set)
  bool			reshow_req; // these are set on async req, cleared when serviced
  bool			defer_reshow_req; 
  // deferred reshow -- used when hidden, or changed; when refresh comes, it reshows
  bool			get_image_req;
  bool			apply_req;
  

  virtual void	Constr_Strings(const char* prompt="", const char* win_title="");
  virtual void  Constr_Methods() {}
  virtual void 	Constr_RegNotifies() {} // register notify on taBase
  virtual void Constr_impl();
  // called in following order by Constr_impl
  virtual void	Constr_WinName();
  virtual void	Constr_Widget(); //create the widget(), then call this inherited member
  virtual void	Constr_Prompt();
  virtual void  Constr_Box() {} // impl in subclass
  virtual void	Constr_Body() {} // impl in subclass
  virtual void  Insert_Methods() {}
  virtual void	Constr_Buttons(); // note: Constr_impl creates the box/layout for the buttons
  virtual void	Constr_Final() {}

  virtual void	Cancel_impl();
  virtual void 	Ok_impl(); // for dialogs
  virtual void	Refresh_impl(bool reshow) {}
  
  virtual void 		DoConstr_Dialog(iDialog*& dlg); // common sub-code for constructing a dialog instance
  void 			DoDestr_Dialog(iDialog*& dlg); // common sub-code for destructing a dialog instance
  void			DoRaise_Dialog(); // what Raise() calls for dialogs
  
  
/*  override void		customEvent(QEvent* ev);
*/  
  virtual void		InitGuiFields(bool virt = true); // NULL the gui fields -- virt used for ctor 

};


class TA_API taiDataHost: public taiDataHostBase, virtual public IDataHost 
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiDataHostBase)
  Q_OBJECT
friend class iDialog;
public:
  static void		GetMembDesc(MemberDef* md, String& dsc_str, String indent);
  static void 		GetName(MemberDef* md, String& name, String& help_text); // returns one name, and optionally help_text
  static iLabel* MakeInitEditLabel(const String& name, QWidget* par, int ctrl_size,
    const String& desc, taiData* buddy = NULL, MemberDef* md = NULL,
    QObject* ctx_obj = NULL, const char* ctx_slot = NULL, int row = 0); 
    // helper used by AddName, and in ProgEditor (and elsewhere, ex inlines)
  
  int		row_height;	// height of edit rows, not including margins and spaces (= max_control_height(def_size)) -- set in Constr
  int		cur_row;	// #IGNORE marks row num of ctrl being added to grid or matrix (for groups) -- child can read this to determine its background color, by calling colorOfRow()


  QSplitter*	splBody;	// if not null when body created, then body is put into this splitter (used for list/group hosts)
  QScrollArea*	  scrBody;		// scrollbars for the body items
  QGridLayout* 	  layBody;	// layout for the body -- deleted/reconstructed when show changes
  QFrame*	frmMethButtons;	// method buttons -- in body for dialogs, in outer panel for panel
  iFlowLayout*	layMethButtons;	// method buttons


  virtual const iColor	colorOfRow(int row) const;	// background color for specified row (row need not exist); good for top and bottom areas
  inline bool		showMethButtons() const {return show_meth_buttons;} // true if any are created

  taiDataHost(TypeDef* typ_ = NULL, bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  virtual ~taiDataHost();

  void			ClearBody();	
   // prepare dialog for rebuilding Body to show new contents

  virtual void		Iconify(bool value);	// for dialogs: iconify/deiconify
  virtual void 		ReConstr_Body(); // called when show has changed and body should be reconstructed -- this is a deferred call
  virtual void		Revert_force();	
   // forcibly (automatically) revert buffer (prompts)
  virtual void  	SetRevert();	// set the revert button on
  virtual void  	UnSetRevert();	// set the revert button off
  virtual bool		ReShow(bool force = false); // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
  virtual void		ReShow_Async(bool force = false); // reshow asynchronously; can be called multiple times before the reshow (only done once)
  override void		GetImage_Async(); // refresh asynchronously; can be called multiple times (only done once)
  virtual void		Raise() {if (isDialog()) DoRaise_Dialog();}	// bring dialog or panel (in new tab) to the front
  override void		GetImage(bool force) {inherited::GetImage(force);} // scope ugh
  
public: // ITypedObject i/f (common to IDLC and IDH)
  void*		This() {return this;} // override
  TypeDef* 	GetTypeDef() const {return &TA_taiDataHost;} // override

public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
//  void		DataLinkDestroying(taDataLink* dl); 
  void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // IDataHost i/f
  const iColor	 colorOfCurRow() const {return colorOfRow(cur_row);} 
  taMisc::ShowMembs	show() const; // legacy -- just returns the app value
  bool  	HasChanged() {return modified;}	
  bool		isConstructed() {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  bool		isModal() {return modal;}
  bool		isReadOnly() {return read_only;} //
// iMainWindowViewer* viewerWindow() const; n/a here -- defined in taiEDH
  void*		Base() {return cur_base;} // base of the object
  TypeDef*	GetBaseTypeDef() {return typ;} // TypeDef on the base, for casting
  void		GetImage()	{GetImage(true);}
  void		GetValue()	{ }
public slots:
  void		Apply_Async();
  void  	Changed() {inherited::Changed();}


protected:
  bool			show_meth_buttons; // true if any are created
  MemberDef*		sel_item_md; // only used during handling of context menu for select edits
  bool			rebuild_body; // #IGNORE set for second and subsequent build of body (show change, and seledit rebuild)

  int 		AddSectionLabel(int row, QWidget* wid, const String& desc);
    // add a widget, usually a label or checkbox, that will span both columns (no data)
  int		AddName(int row, const String& name, const String& desc,
    taiData* buddy = NULL, MemberDef* md = NULL);
    // add a label item in first column; row<0 means "next row"; returns row
//  void		AddLabel(int index, QWidget* label); // add a label item in first column
  int		AddData(int row, QWidget* data, bool fill_hor = false);
    // add a data item in second column; row<0 means "next row"; returns row
  void		AddMultiRowName(iEditGrid* multi_body, int row, const String& name, const String& desc); // adds a label item in first column of multi data area -- we define here for source code mgt, since AddName etc. are similar
  void		AddMultiColName(iEditGrid* multi_body, int col, const String& name, const String& desc); // adds descriptive column text to top of a multi data item
  void		AddMultiData(iEditGrid* multi_body, int row, int col, QWidget* data); // add a data item in the multi-data area -- expands if necessary
  void		BodyCleared(); // called when show changed, and body has actually been cleared
  void		SetMultiSize(int rows, int cols) {} // implemented in gpiMultiEditHost
  virtual void	ClearBody_impl(); // #IGNORE prepare dialog for rebuilding Body to show new contents -- INHERITING CLASSES MUST CALL THIS LAST
  override void  Constr_Methods(); // creates the box for buttons
  virtual void	Constr_Methods_impl(); // actually makes methods -- stub this out to supress methods
  override void  Constr_Box();
  override void	Constr_Body();
  override void  Insert_Methods(); // insert the menu and methods, if made, and not owned elsewise
  override void	Constr_Final();
  virtual void	FillLabelContextMenu(iLabel* sender, QMenu* menu, int& last_id); // last_id enables access menu items
  override void	Cancel_impl();
  override void	Ok_impl();


protected:
  override void		customEvent(QEvent* ev);

  override void		InitGuiFields(bool virt = true); // NULL the gui fields -- virt used for ctor
  override void		Refresh_impl(bool reshow);
protected slots:
  virtual void	label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e);
};



class TA_API taiDialog_List : public taPtrList<taiDataHost> { // #IGNORE list of DataHosts that have been dialoged
protected:
  void	El_Done_(void* it)	{ delete (taiDataHost*)it; }

public:
  ~taiDialog_List()            { Reset(); }
};

class TA_API MembSet { // #IGNORE
public:
  Member_List		memb_el; // member elements (1:1 with data_el), empty in inline mode
  taiDataList 		data_el; // data elements (1:1 with memb_el WHEN section 
  bool			show; // flag to help by indicating whether to show or not
  
  MembSet() {show = false;}
private:
  MembSet(const MembSet& cp); // value semantics not allowed
  MembSet& operator=(const MembSet& cp);
};

class TA_API MembSet_List : public taPtrList<MembSet> { // #IGNORE 
public:
  int			def_size; // set to how many you want to use default processing
  void			SetMinSize(int n); // make sure there are at least n sets
  void			ResetItems(bool data_only = false); // calls Reset on all lists
  
  MembSet_List()  {def_size = 0;}
  ~MembSet_List() { Reset(); }
protected:
  void	El_Done_(void* it) { delete (MembSet*)it; }
};

class TA_API taiEditDataHost : public taiDataHost {
  // // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for classes -- default is to assume a EditDataPanel as the widget, but the Edit subclasses override that
  Q_OBJECT
INHERITED(taiDataHost)
friend class EditDataPanel;
public:
  enum DefMembSet { // keys for default members sets -- always created
    MS_NORM,	// normal members, always shown
    MS_EXPT,	// Expert members
    MS_HIDD,	// Hidden members
    
    MS_CNT	= 3 // number of default members
  };
  
  taiMenu_List		ta_menus;	// menu representations (from methods, non-menubuttons only)
  taiMenu_List		ta_menu_buttons;	// menu representations (from methods -- menubuttons only)
  taiActions*		cur_menu;	// current menu to add to (if not otherwise spec'd)
  taiActions*		cur_menu_but; // current menu button to add to (if not otherwise spec'd)
//temp  taiMenuBar*		menu;		// menu bar
  taiActions*		menu;		// menu bar
  
  MembSet_List		membs;
  QButtonGroup*		bgrp; // group used for set checkboxes
  taiDataList 		meth_el;	// method elements

  //NOTE: we provide indexed access to references here for convenience, but be careful!
  const bool&		show_set(int i) const // whether the set is shown 
    {return membs.FastEl(i)->show;}
  bool&			show_set(int i) // whether the set is shown 
    {return membs.FastEl(i)->show;}
  const Member_List&	memb_el(int i) const // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  Member_List&		memb_el(int i) // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  const taiDataList&	data_el(int i) const // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->data_el;}
  taiDataList&		data_el(int i) // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->data_el;}
  
  EditDataPanel*	dataPanel() {return panel;} // #IGNORE
  override void 	guiParentDestroying() {panel = NULL;}
  
  bool			SetShow(int value, bool no_refresh = false); // change show value; returns true if we rebuilt/reshowed dialog

  taiEditDataHost(void* base, TypeDef* typ_ = NULL, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  taiEditDataHost()		{ };
  ~taiEditDataHost();


  override int 		Edit(bool modal_ = false); 
    // for dialogs -- add to list of active_edit dialogs too
  EditDataPanel* 	EditPanel(taiDataLink* link); // for panels
  EditDataPanel* 	EditPanelDeferred(taiDataLink* link); // for panels
  void		 	ConstrEditControl(); 
    // for controls -- construct then edit 
  void			GetImage(bool force); //override
  void			GetValue(); //override
  virtual bool		ShowMember(MemberDef* md) const;
  virtual void		SetCurMenu(MethodDef* md); // sets or creates the cur_menu, for subsequent adding of items
  virtual void		SetCurMenuButton(MethodDef* md);
  override void		Raise() {if (isPanel()) DoRaise_Panel(); else taiDataHost::Raise();}
  override void 	ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
    // check for unsaved changes and prompt to save/discard; called by several places prior to closing tab window, closing dialog, shutting down app, etc.
  override void 	Cancel_impl();
public: // ITypedObject i/f (common to IDLC and IDH)
  override TypeDef* 	GetTypeDef() const {return &TA_taiEditDataHost;}
public slots:
// IDataHost i/f
  override iMainWindowViewer* viewerWindow() const;

protected:
  EditDataPanel* panel; //NOTE: not used when invoked by Edit()
  bool			inline_mode; // true when doing inline, set early in constr

  override void		InitGuiFields(bool virt = true);
  override void 	Constr_impl();
  virtual void		Enum_Members(); // called by Constr_impl to fill memb_el[]
  override void		Constr_Methods_impl();
  override void		ClearBody_impl();
  override void		Constr_Strings(const char* prompt, const char* win_title);
  override void		Constr_Body();    // construct the data of the dialog
  virtual void		Constr_Data_Labels(); // calls Data then Labels -- override to do your own
  virtual void 		Constr_Inline(); // called instead of Data/Labels when typ->requiresInline true
  virtual void		Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl);
  void			Constr_MethButtons();
  override void 	Constr_RegNotifies();
  override void		Constr_Final();
  override void		FillLabelContextMenu(iLabel* sender, QMenu* menu, int& last_id);
  virtual void		FillLabelContextMenu_SelEdit(iLabel* sender, QMenu* menu, int& last_id);
  virtual void		GetImage_Membs(); // for overridding
  virtual void		GetImage_Membs_def(); // calls GetImage_impl for all our lists
  virtual void		GetValue_Membs(); 
  virtual void		GetValue_Membs_def(); // calls GetValue_impl for all our lists
  virtual void		GetImage_impl(const Member_List* ms, const taiDataList& dl, void* base);
  virtual void		GetImageInline_impl(const void* base);
  virtual void		GetValue_impl(const Member_List* ms, const taiDataList& dl, void* base) const;
  virtual void		GetValueInline_impl(void* base) const;
  virtual void		GetButtonImage(bool force = true);
  void			AddMethButton(taiMethodData* mth_rep, const String& label = _nilString);
    // uses mth's label, if no label passed
  void			DoAddMethButton(QAbstractButton* but);
  void			DoRaise_Panel(); // what Raise() calls for panels
  override void 	DoConstr_Dialog(iDialog*& dlg);

protected slots:
  virtual void	DoSelectForEdit(int param); // param will be index of the SelectEdit; sel_data_index will hold the index of the data item
  virtual void		bgrp_buttonClicked(int id); // one of the section checkboxes
};


class TA_API taiStringDataHost: public taiDataHostBase 
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiDataHostBase)
  Q_OBJECT
public:
  QTextEdit*		edit;
  MemberDef*		mbr; // the member being edited (doesn't have to be String)
  
  taBase*		base() const;
  
  void 			Constr(const char* prompt = "", const char* win_title = "");
  override void		GetImage();
  override void		GetValue();
  override void		Constr_Buttons();
  
  taiStringDataHost(MemberDef* mbr, void* base, TypeDef* typ_ = NULL, 
   bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  ~taiStringDataHost();
  

protected slots:
  void		btnPrint_clicked();
  
protected:
  QPushButton*		btnPrint;

  void			DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void  	Constr_Box();
  override void 	Constr_RegNotifies();
  override void 	DoConstr_Dialog(iDialog*& dlg);
  override void 	ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
};

#endif // tai_dialog_h

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


// ta_qt_dialog.h: Qt-based data hosts and dialogs

#ifndef ta_qtdialog_h
#define ta_qtdialog_h 1

#include "ta_type.h"
#include "ta_qtdata.h" //for taiMenu_List
#include "ta_qtviewer.h"

#ifndef __MAKETA__
  #include <ilabel.h>
  #include <QDialog>
  #include <QObject>
  #include <QPushButton>
#endif

// externals
class taiDataLink;  //
class iDataPanel; //

// forwards
class taiDataHost;
class taiEditDataHost; //


//class	ivTopLevelWindow;		// #IGNORE

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


//////////////////////////////////
// 	iContextLabel		//
//////////////////////////////////

class TA_API iContextLabel: public iLabel {
  Q_OBJECT
INHERITED(iLabel)
public:
  int			index() {return mindex;}

  iContextLabel(QWidget* parent = NULL);
  iContextLabel(int index_, const String& text, QWidget* parent = NULL);
  ~iContextLabel()	{};

#ifndef __MAKETA__
signals:
  void			contextMenuInvoked(iContextLabel* sender, QContextMenuEvent* e);
#endif

protected:
  int			mindex;
  void 			contextMenuEvent (QContextMenuEvent* e); // override
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

class TA_API taiChoiceDialog : public QDialog {
  Q_OBJECT
public:
  // puts up a modal choice dialog.  returns value of Edit()
  static  int	ChoiceDialog(QWidget* win, const char* prompt="",
    const char* win_title="", bool no_cancel_ = false);

  static void ErrorDialog(QWidget* parent_, const char* msg, const char* win_title = "Error");

  bool		no_cancel;
  QVBoxLayout*	vblMain;
  QHBoxLayout*	hblButtons;
  QLabel*	txtMessage; // maybe should be a ro edit, so user can copy???
  Q3ButtonGroup* bgChoiceButtons;

  ~taiChoiceDialog()		{ }

protected:
  virtual void keyPressEvent(QKeyEvent* ev); // override

protected slots:
  virtual void accept(); // override -- it should return 0 for our purpose (not 1)
  virtual void reject(); // override -- ignore new no_cancel, also should return 1 for our purpose

private:
  taiChoiceDialog(QWidget* par = 0, const char* prompt = "",
    const char* win_title = "", bool no_cancel = false);
#ifndef __MAKETA__
  virtual bool	Constr_OneBut(String& lbl, int curId);
  // constructs one button, returns false when no more..
#endif
};

//////////////////////////
// 	iDialog		//
//////////////////////////

class TA_API iDialog : public QDialog { // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
  Q_OBJECT
friend class taiDataHost;
public:
  iDialog(taiDataHost* owner_, QWidget* parent = 0);
  ~iDialog();

  bool		post(bool modal); // simplified version of post_xxx routines, returns true if accepted or false (if modal) if cancelled
  void		dismiss(bool accept_);

  void		iconify();   // Iv compatibility routine
  void		deiconify(); // Iv compatibility routine
  void		setCentralWidget(QWidget* widg); // is put in a scroll area; dialog is limited to screen size
protected:
  taiDataHost* 	owner;
  QWidget* 	mcentralWidget;
  QScrollArea*	scr;
  override void closeEvent(QCloseEvent* ev);
};


class TA_API iTextEditDialog : public QDialog { // #NO_CSS
  Q_OBJECT
INHERITED(QDialog)
public:
  QTextEdit*	txtText;
  QPushButton*	btnOk; // read/write only
  QPushButton* 	btnCancel; // or close, if read only
  
  bool		isReadOnly() {return m_readOnly;}
  virtual void	setText(const String& value);
  
  iTextEditDialog(bool readOnly = false, QWidget* parent = 0);
  ~iTextEditDialog();
protected:
  bool		m_readOnly;
private:
  void 		init(bool readOnly);
};

//////////////////////////
// 	EditDataPanel	//
//////////////////////////

class TA_API EditDataPanel: public iDataPanelFrame { // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
friend class taiEditDataHost;
public:
  override String	panel_type() const; // this string is on the subpanel button for this panel
  override void		Closing(bool forced, bool& cancel);
  override const iColor* GetTabColor(bool selected) const; // special color for tab; NULL means use default
  override bool		HasChanged(); // 'true' if user has unsaved changes -- used to prevent browsing away

  EditDataPanel(taiEditDataHost* owner_, taiDataLink* dl_);
  ~EditDataPanel();

public: // IDataLinkClient interface
  override TypeDef*	GetTypeDef() const {return &TA_EditDataPanel;}

protected:
  taiEditDataHost* 	owner;
  override void		GetImage_impl(); // #IGNORE called when reshowing a panel, to insure latest data (except not called if HasChanged true)
};


class TA_API taiDataHost: public QObject, virtual public IDataLinkClient, 
  virtual public IDataHost 
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
  Q_OBJECT
friend class iDialog;
public:
  enum Dlg_State {
    EXISTS		= 0x01,
    CONSTRUCTED		= 0x02,
    ACTIVE		= 0x03,
    ACCEPTED		= 0x04,
    CANCELED		= 0x05,
    SHOW_CHANGED	= 0x80	// flag to indicate what to show was changed, reconstruct!
  };

  static void	DeleteChildrenLater(QObject* obj); // convenience function -- deleteLater all children

  bool          read_only;	// cannot change data
  bool		use_show; 	// true if can use the show menu/rebuild facility
  TypeDef*	typ;		// type of object (if relevant)
  void*		cur_base;	// current base pointer of object (if relevant)
  int		ctrl_size;	// a taiMisc::SizeSpec-compatible value (ie, from FontSpec) -- def is taiM->ctrl_size
  int		row_height;	// height of edit rows, not including margins and spaces (= max_control_height(def_size)) -- set in Constr
  int		cur_row;	// #IGNORE marks row num of ctrl being added to grid or matrix (for groups) -- child can read this to determine its background color, by calling colorOfRow()

  String	prompt_str;	// string that goes inside as a prompt or label
  String	win_str;	// string that goes on the window frame or similar area
  int		state;		// Dlg_State value -- indicates state of construction of dialog
  bool		modal;	// true if dialog is of the modal variety, always false for edit panels
  bool		no_ok_but;	// no ok button
  int		mouse_button;	// (Qt::ButtonState) the mouse button that pressed ok
  bool		no_revert_hilight; // flag to indicate we are the instance that caused the update
  bool		warn_clobber; // was changed elsewhere while edited here; warn user before saving


  QVBoxLayout*	vblDialog;	// layout for the entire dialog -- stacked/nested as follows:
    QLabel*	prompt;		// informative message at top of dialog
    QSplitter*	splBody;	// if not null when body created, then body is put into this splitter (used for list/group hosts)
      QScrollArea*	scrBody;		// scrollbars for the body items
      QGridLayout* 	layBody;	// layout for the body -- deleted/reconstructed when show changes
        QWidget*	body;		// parent for the body items (actually an iStripeWidget)
    QFrame*	frmMethButtons;	// method buttons
      iFlowLayout*	layMethButtons;	// method buttons
    QHBoxLayout*	hblButtons;	// box of buttons on the bottom of the dialog
      HiLightButton* 	okbut; // is HilightButton for the special mouse button handling
      HiLightButton* 	canbut;// is HilightButton for the special mouse button handling
      HiLightButton*	apply_but;	// only use for dialogs that wait around
      HiLightButton*	revert_but;


  iColor* 	bg_color;	// background color of host -- only set via setBgColor
  virtual bool	isDialog() {return dialog;} // 'true' when we will be been posted as a dialog
  virtual bool	isPanel() {return is_panel;} // 'true' when we will be shown in a panel
  virtual void	setBgColor(const iColor* new_bg); // #SET_bg_color
  virtual const iColor* 	colorOfRow(int row) const;	// background color for specified row (row need not exist); good for top and bottom areas
  QWidget* 	widget() {return mwidget;}

  taiDataHost(TypeDef* typ_ = NULL, bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  virtual ~taiDataHost();

  void		ClearBody();	// prepare dialog for rebuilding Body to show new contents

  void  Constr(const char* prompt = "", const char* win_title = "", const iColor* bgcol = NULL, bool as_panel = false);
    //NOTE: if built with as_panel=true, then must only be a panel, not dialog, and viceversa
  virtual int 		Edit(bool modal_ = false); // for dialogs -- creates iDialog
  virtual void		Iconify(bool value);	// for dialogs: iconify/deiconify
  virtual void 		ReConstr_Body(); // called when show has changed and body should be reconstructed -- this is a deferred call
  virtual void  Unchanged();	// call when data has been saved or reverted
  virtual void	Revert_force();	// forcibly (automatically) revert buffer (prompts)
  virtual void  SetRevert();	// set the revert button on
  virtual void  UnSetRevert();	// set the revert button off
  virtual void  NotifyChanged(); // called by our object when it has changed (by us, or other)
  virtual void		ReShow(); // rebuild; called on major obj change, or when new Show option
  virtual void	Raise() {if (isDialog()) DoRaise_Dialog();}	// bring dialog or panel (in new tab) to the front
  virtual void  Scroll(){}	// overload to scroll to field editor
  
public: // ITypedObject i/f (common to IDLC and IDH)
  void*		This() {return this;} // override
  TypeDef* 	GetTypeDef() const {return &TA_taiDataHost;} // override

public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
  void		DataLinkDestroying(taDataLink* dl); 
  void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // IDataHost i/f
  const iColor* colorOfCurRow() const {return colorOfRow(cur_row);} 
  bool  	HasChanged() {return modified;}	
  bool		isConstructed() {return state > EXISTS;}
  bool		isModal() {return modal;}
  bool		isReadOnly() {return read_only;}
  void*		Base() {return cur_base;} // base of the object
  TypeDef*	GetBaseTypeDef() {return typ;} // TypeDef on the base, for casting
  void		SetItemAsHandler(taiData* item, bool set_it = true) {} //
//  void		GetImage()	{ }
//  void		GetValue()	{ }
public slots:
  void  	Changed();	// override method call when data has changed

public slots:
  virtual void	Apply(); //override
  virtual void	Revert(); //override

protected:
  iColor*		bg_color_dark;	// background color of dialog, darkened (calculated when bg_color set)
  bool			modified;
  bool			showMethButtons; // true if any are created
  QWidget*		mwidget;	// outer container for all widgets
  iDialog*		dialog; // dialog, when using Edit, NULL otherwise
  bool			is_panel; // hint when constructed to tell us if we are a dialog or panel -- must be consistent with dialog/panel
  int 			sel_item_index; // only used during handling of context menu for select edits
  bool			rebuild_body; // #IGNORE set for second and subsequent build of body (show change, and seledit rebuild)
  DataChangeHelper 	dch; // helps track the state of datachanges

  int		AddName(int row, const String& name, const String& desc, taiData* buddy = NULL);
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
  virtual void	Constr_Strings(const char* prompt="", const char* win_title=""); // called by static Constr
  virtual void Constr_impl();
  // called in following order by Constr_impl
  virtual void	Constr_WinName();
  virtual void	Constr_Widget(); //create the widget(), then call this inherited member
  virtual void	Constr_Prompt();
  virtual void  Constr_Box();
  virtual void	Constr_Body();
  virtual void  Constr_Methods(); // creates the box for buttons
  virtual void	Constr_Buttons(); // note: Constr_impl creates the box/layout for the buttons
  virtual void	Constr_Final();
  virtual void	FillLabelContextMenu(iContextLabel* sender, QMenu* menu, int& last_id); // last_id enables access menu items

public slots:
  virtual void 	Ok(); // for dialogs
  virtual void 	Cancel(); // mostly for dialogs, but also used internally to close a Panel (ex. when obj deleted)

protected:

  void 			DoConstr_Dialog(iDialog*& dlg); // common sub-code for constructing a dialog instance
  void 			DoDestr_Dialog(iDialog*& dlg); // common sub-code for destructing a dialog instance
  void			DoRaise_Dialog(); // what Raise() calls for dialogs

protected slots:
  virtual void	label_contextMenuInvoked(iContextLabel* sender, QContextMenuEvent* e);
};



class TA_API taiDialog_List : public taPtrList<taiDataHost> { // #IGNORE list of DataHosts that have been dialoged
protected:
  void	El_Done_(void* it)	{ delete (taiDataHost*)it; }

public:
  ~taiDialog_List()            { Reset(); }
};

class TA_API Member_List : public taPtrList<MemberDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS list of members -- simpler than MemberSpace, no ownership
public:
  ~Member_List()            { Reset(); }
};

//////////////////////////////////
// 	taiEditDataHost		//
//////////////////////////////////

class TA_API taiEditDataHost : public taiDataHost {
  // // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for classes -- default is to assume a EditDataPanel as the widget, but the Edit subclasses override that
  Q_OBJECT
friend class EditDataPanel;
public:
  taMisc::ShowMembs	show;		// current setting for what to show
  taiMenu_List		ta_menus;	// menu representations (from methods, non-menubuttons only)
  taiMenu_List		ta_menu_buttons;	// menu representations (from methods -- menubuttons only)
  taiActions*		cur_menu;	// current menu to add to (if not otherwise spec'd)
  taiActions*		cur_menu_but; // current menu button to add to (if not otherwise spec'd)
//temp  taiMenuBar*		menu;		// menu bar
  taiActions*		menu;		// menu bar
  taiActions*		show_menu;	// Show menu bar
  Member_List		memb_el;	// member elements (1:1 with data_el), empty in inline mode
  taiDataList 		data_el;	// data elements (1:1 with memb_el), except in inline mode
  taiDataList 		meth_el;	// method elements

  EditDataPanel*	dataPanel() {return panel;} // #IGNORE
  virtual void		setShow(taMisc::ShowMembs value); // #SET_Show
  override void 	guiParentDestroying() {panel = NULL;}

  taiEditDataHost(void* base, TypeDef* typ_ = NULL, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  taiEditDataHost()		{ };
  ~taiEditDataHost();


  override int 		Edit(bool modal_); // for dialogs -- add to list of active_edit dialogs too
  EditDataPanel* 	EditPanel(taiDataLink* link); // for panels
  void			GetImage(); //override
  void			GetValue(); //override
  virtual bool		ShowMember(MemberDef* md) const;
  virtual void		GetMembDesc(MemberDef* md, String& dsc_str, String indent);
//obs  virtual QWidget* 	GetNameRep(MemberDef* md, QWidget* dataWidget);	// returns one name item (typically a label)
  virtual void 		GetName(MemberDef* md, String& name, String& help_text); // returns one name, and optionally help_text
  virtual void		SetCurMenu(MethodDef* md); // sets or creates the cur_menu, for subsequent adding of items
  virtual void		SetCurMenuButton(MethodDef* md);
  override void		SetItemAsHandler(taiData* item, bool set_it = true);
  override void		Raise() {if (isPanel()) DoRaise_Panel(); else taiDataHost::Raise();}
  virtual bool		ReShow(bool force = false); // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
public: // ITypedObject i/f (common to IDLC and IDH)
  override TypeDef* 	GetTypeDef() const {return &TA_taiEditDataHost;}
public slots:
  virtual void	ShowChange(taiAction* sender);	// when show/hide menu changes
  void Cancel(); // override

protected:
  EditDataPanel* panel; //NOTE: not used when invoked by Edit()

  void			setShowValues(taMisc::ShowMembs value);
  override void		ClearBody_impl();
  override void		Constr_Strings(const char* prompt, const char* win_title);
  override void		Constr_Body();    // construct the data of the dialog
  virtual void		Constr_Data(); // members, or equivalent in inherited classes, and labels
  virtual void		Constr_Labels(); // labels
  virtual void 		Constr_Inline(); // called instead of Data/Labels when typ->requiresInline true
  virtual void		Constr_Labels_impl(const MemberSpace& ms, taiDataList* dl = NULL); //dl non-null enables label-buddy linking
  virtual void		Constr_Data_impl(const MemberSpace& ms, taiDataList* dl);
  override void		Constr_Methods(); // construct the methods (buttons and menus)
  virtual void		Constr_Methods_impl(); // actually makes methods -- stub this out to supress methods
  void			Constr_MethButtons();
  virtual void		Constr_ShowMenu(); // make the show/hide menu
  override void		Constr_Final();
  override void		FillLabelContextMenu(iContextLabel* sender, QMenu* menu, int& last_id);
  virtual void		FillLabelContextMenu_SelEdit(iContextLabel* sender, QMenu* menu, int& last_id);
  virtual void		GetImage_impl(const MemberSpace& ms, const taiDataList& dl, void* base);
  virtual void		GetImageInline_impl(const void* base);
  virtual void		GetValue_impl(const MemberSpace& ms, const taiDataList& dl, void* base) const;
  virtual void		GetValueInline_impl(void* base) const;
  virtual void		GetButtonImage();
  void			AddMethButton(taiMethodData* mth_rep, const char* label = NULL);
    // uses mth's label, if no label passed
  void			DoAddMethButton(QPushButton* but);
  void			DoRaise_Panel(); // what Raise() calls for panels

protected slots:
  virtual void	DoSelectForEdit(int param); // param will be index of the SelectEdit; sel_data_index will hold the index of the data item
};


/* TODO: where are these actually used???
//////////////////////////////
//       taiDialogs        //
//////////////////////////////

class taiTokenDialog : public taiDialog {
  // automatic widget to select a token of a given type
public:
  TAPtr		itm;
  TAPtr		scope_ref;
  taiToken* 	itm_rep;

  taiTokenDialog(TypeDef* td, TAPtr scp_ref=NULL, TAPtr in_itm=NULL);
  ~taiTokenDialog();

  void		Constr_Box();
  void		GetValue();

  // this is the function you actually call..
  static TAPtr GetToken(TypeDef* td, const char* prompt="",
			TAPtr scp_ref=NULL, TAPtr init_itm = NULL, QDialog* win=NULL);
};

class taiTypeDialog : public taiDialog {
  // select a type
public:
  TypeDef*	base_typ;
  TypeDef*	sel_typ;
  taiTypeHier* typ_rep;

  taiTypeDialog(TypeDef* td, TypeDef* init_tp=NULL);
  ~taiTypeDialog();

  void		Constr_Box();
  void		GetValue();

  // this is the function you actually call..
  static TypeDef* GetType(TypeDef* td, const char* prompt="", TypeDef* init_tp=NULL,
			  QDialog* win=NULL);
};

class taiEnumDialog : public taiDialog {
  // select an enum
public:
  TypeDef*	enum_typ;
  int		sel_val;
  taiMenu* 	enm_rep;

  taiEnumDialog(TypeDef* td, int init_vl=0);
  ~taiEnumDialog();

  void		Constr_Box();
  void		GetValue();

  // this is the function you actually call..
  static int 	GetEnum(TypeDef* td, const char* prompt="", int init_vl=0,
			QDialog* win=NULL);
}; */

#endif // tai_dialog_h

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

#ifndef taiDataHostBase_h
#define taiDataHostBase_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QObject>
#endif
#include <ISigLinkClient>
#include <taPtrList>

// member includes:
#include <ContextFlag>
#include <iColor>
#include <taiMiscCore>
#include <TypeDef>
#ifndef __MAKETA__
#include <QWidget>
#endif

// declare all other types mentioned but not required to include:
class taiDataHostBase; //
class iHostDialog; //
class taBase;      // 
class HiLightButton; //
class QVBoxLayout; // 
class QScrollArea; //
class QHBoxLayout; // 
class QLabel; //

TypeDef_Of(taiDataHostBase_List);

class TA_API taiDataHostBase_List : public taPtrList<taiDataHostBase> {
  // list of data host base guys -- for async management -- no ownership of items
protected:
public:
  ~taiDataHostBase_List()            { Reset(); }
};


TypeDef_Of(taiDataHostBase);

class TA_API taiDataHostBase: public QObject, virtual public ISigLinkClient {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for managing the contents of an edit dialog
INHERITED(QObject)
  Q_OBJECT
friend class iHostDialog;
public:
  enum Dlg_State {
    EXISTS              = 0x01,
    DEFERRED1, // only the first part of entire construction
    CONSTRUCTED,
    ACTIVE,
    ACCEPTED,
    CANCELED,
    ZOMBIE, // for when gui stuff deleted before we did
    STATE_MASK          = 0x0F, // #NO_SHOW
    SHOW_CHANGED        = 0x80  // #NO_SHOW flag to indicate what to show was changed, reconstruct!
  };

  enum HostType {
    HT_DIALOG,          // host/owner is an iHostDialog (legacy "Edit" behavior)
    HT_PANEL,           // host/owner is an EditPanel ("properties" panel)
    HT_CONTROL          // host/owner is a control -- we won't show menus or obj buttons
  };

  static void   MakeDarkBgColor(const iColor& bg, iColor& dk_bg); // for use by other users of stripe grids, to make the right dk bg color

  TypeDef*      typ;            // type of object (if relevant)
  void*         root;   // current root pointer of object (if relevant)
  int           ctrl_size;      // a taiMisc::SizeSpec-compatible value (ie, from FontSpec) -- def is taiM->ctrl_size
  int           row_height;     // height of edit rows, not including margins and spaces (= max_control_height(def_size)) -- set in Constr_impl
  bool          read_only;      // cannot change data
  int           state;          // Dlg_State value -- indicates state of construction of dialog
  bool          modal;  // true if dialog is of the modal variety, always false for edit panels
  bool          no_ok_but;      // no ok button
  int           mouse_button;   // (Qt::ButtonState) the mouse button that pressed ok

  String        prompt_str;     // string that goes inside as a prompt or label
  String        win_str;        // string that goes on the window frame or similar area

  QVBoxLayout*  vblDialog;      // layout for the entire dialog -- stacked/nested as follows:
  QLabel*       prompt;         // informative message at top of dialog
  QScrollArea*  scrBody;        // scrollbars for the body items
  QWidget*      body;           // parent for the body items
  QWidget*      widButtons; // box of buttons on the bottom of the dialog (unparented -- added to parent later
  QHBoxLayout*  layButtons;
  HiLightButton*  okbut; // is HilightButton for the special mouse button handling
  HiLightButton*  canbut;// is HilightButton for the special mouse button handling
  HiLightButton*  apply_but;    // only use for dialogs that wait around
  HiLightButton*  revert_but;
  HiLightButton*  help_but;     // help button

  iColor                bgColor() const {return bg_color;}
  virtual void          setBgColor(const iColor& new_bg);

  inline bool           isDialog() {return (host_type == HT_DIALOG);}
    // 'true' when we will be been posted as a dialog
  bool                  isModified() const {return modified;}
  inline bool           isPanel() {return (host_type == HT_PANEL);}
    // 'true' when we will be shown in a panel
  inline bool           isControl() {return (host_type == HT_CONTROL);}
    // 'true' when shown in a control
  QWidget*      widget() {return mwidget;}
  void                  Updating(bool enter) {if (enter) ++updating; else --updating;}
  virtual void          StartEndLayout(bool start); // bracket the layout of ctrls; helps optimize

  taiDataHostBase(TypeDef* typ_ = NULL, bool read_only_ = false,
    bool modal_ = false, QObject* parent = 0);
  virtual ~taiDataHostBase(); //

  taBase*       Base_() const; // root of the object, if a taBase

  void  Constr(const char* prompt = "", const char* win_title = "",
    HostType host_type = HT_DIALOG, bool deferred = false);
    //NOTE: if built with as_panel=true, then must only be a panel, not dialog, and viceversa
  void  ConstrDeferred(); // finish deferred construction
  void                  ConstrEditControl();
  virtual int           Edit(bool modal_ = false, int min_width=-1, int min_height=-1);
  // for dialogs -- creates iHostDialog
  virtual void          Unchanged();    // call when data has been saved or reverted
  virtual void          Refresh(); // does a GetImage or defered Reshow
  virtual bool          ReShow(bool force = false) { return false; } // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
  virtual void          ReConstr_Body() { }; // called when show has changed and body should be reconstructed -- this is a deferred call
  virtual void          ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL) {}
  virtual void          WidgetDeleting(); // lets us null the gui fields, and set state

  virtual QWidget*      firstTabFocusWidget() { return NULL; } // first widget that accepts tab focus -- to set link between tab and contents of edit


  static bool   AsyncWaitProc();
  // process async apply, reshow, getimage requests -- called by overall wait proc system

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() {return this;} // override
  TypeDef*      GetTypeDef() const {return &TA_taiDataHostBase;} // override

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//  bool                ignoreDataChanged() const; we always accept, but respect hidden
  void          DataLinkDestroying(taSigLink* dl);
  void          DataDataChanged(taSigLink* dl, int dcr, void* op1, void* op2);


// virtuals for IDataHost i/f -- call back to these from taiDataHost
  virtual void          GetImage() {} // IDH brings this in too, we override in EDH
  virtual void          GetImage(bool force) {} // ugly hack
  virtual void          GetValue() {} // IDH brings this in too, we override in EDH
public slots:
  virtual void          Apply();
  virtual void          Changed();      // override method call when data has changed
  virtual void          Apply_Async();
  virtual void          ReShow_Async(bool force = false); // reshow asynchronously; can be called multiple times before the reshow (only done once)
  virtual void          ReConstr_Async();
  virtual void          GetImage_Async(); // refresh asynchronously; can be called multiple times (only done once)
  virtual void          DebugDestroy(QObject* obj); // todo: remove -- just for debugging

public slots:
  virtual void          Revert();
  virtual void          Ok(); // for dialogs
  void                  Cancel(); // mostly for dialogs, but also used internally to close a Panel (ex. when obj deleted)
  virtual void          Help();

protected:
  ContextFlag           updating; // flag to indicate we are the instance that caused the update
  bool                  modified;
  bool                  warn_clobber; // was changed elsewhere while edited here; warn user before saving
  QWidget*              mwidget;        // outer container for all widgets
  iHostDialog*          dialog; // dialog, when using Edit, NULL otherwise
  HostType              host_type; // hint when constructed to tell us if we are a dialog or panel -- must be consistent with dialog/panel
  iColor                bg_color; // background color of host -- set via setBgColor
  iColor                bg_color_dark;  // background color of dialog, darkened (calculated when bg_color set)
  bool                  reshow_req; // these are set on async req, cleared when serviced
  bool                  reshow_req_forced; // is reshow forced
  bool                  reconstr_req;
  bool                  defer_reshow_req;
  // deferred reshow -- used when hidden, or changed; when refresh comes, it reshows
  bool                  getimage_req;
  bool                  apply_req;
  bool                  reshow_on_apply; // default, so we rebuild on Apply for CONDSHOW etc., but no good for cssDialogs

  static taiDataHostBase_List async_apply_list;
  static taiDataHostBase_List async_reshow_list;
  static taiDataHostBase_List async_reconstr_list;
  static taiDataHostBase_List async_getimage_list;

  const String          def_prompt() const {return m_def_prompt;} // default prompt, provided at constr time
  const String          def_title() const {return m_def_title;}; // default title, provided at constr time

  virtual void  Constr_Strings();
  virtual void  Constr_Methods() {}
  virtual void  Constr_RegNotifies() {} // register notify on taBase
  virtual void Constr_impl();
  // called in following order by Constr_impl
  virtual void  Constr_Widget(); //create the widget(), then call this inherited member
  virtual void  Constr_Prompt();
  virtual void  Constr_Box() {} // impl in subclass
  virtual void  Constr_Body() {} // impl in subclass
  virtual void  Insert_Methods() {}
  virtual void  Constr_Buttons(); // note: Constr_impl creates the box/layout for the buttons
  virtual void  Constr_Final() {}

  virtual void  Cancel_impl();
  virtual void  Ok_impl(); // for dialogs
  virtual void  Refresh_impl(bool reshow) {}

  virtual void          DoConstr_Dialog(iHostDialog*& dlg); // common sub-code for constructing a dialog instance
  void                  DoDestr_Dialog(iHostDialog*& dlg); // common sub-code for destructing a dialog instance
  void                  DoRaise_Dialog(); // what Raise() calls for dialogs

  virtual void          InitGuiFields(bool virt = true); // NULL the gui fields -- virt used for ctor
  virtual void          GetImage_PromptTitle(); // updates the prompt and win title
private:
  String                m_def_prompt;
  String                m_def_title;
};

#endif // taiDataHostBase_h

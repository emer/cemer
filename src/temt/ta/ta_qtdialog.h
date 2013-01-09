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


// ta_qtdialog.h: Qt-based data hosts and dialogs

#ifndef TA_QTDIALOG_H
#define TA_QTDIALOG_H//

#include "ta_type.h"
#include "ta_qt.h"
#include "ta_qtdata.h" //for taiMenu_List
#include "ta_qtviewer.h"

#ifndef __MAKETA__
# include "ilabel.h"
# include <QAbstractItemModel>
# include <QEvent>
# include <QMessageBox>
# include <QObject>
# include <QPushButton>
#endif

// externals
class taiDataLink;  //
class iDataPanel; //
class taWizard; //
#ifndef __MAKETA__
class iSplitter;
class NumberedTextView;//
#endif

// forwards
class taiDataHostBase;
class taiDataHost_impl;
class taiDataHost;
class taiEditDataHost;
class iPluginEditor;
class taiStringDataHost; //


//////////////////////////////////
//      HiLightButton           //
//////////////////////////////////

class TA_API HiLightButton: public QPushButton {
  // ##IGNORE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS combines v3.2 Script Button and HiLightButton -- set script_ to script name, to enable ScriptButton functionality
public:
  int                   mouse_button;   // (Qt::Button) the mouse button that pressed the button (defaults to NoButton, when actuated by keyboard)

  bool                  hiLight() {return mhiLight;} // #GET_HiLight
  virtual void          setHiLight(bool value); // #SET_HiLight
  HiLightButton(QWidget* parent = 0, const char* script_ = 0);
  HiLightButton(const String& text, QWidget* parent, const char* script_ = 0);
  ~HiLightButton();

protected:
  iColor                mhiLight_color;
  bool                  mhiLight;
  String                mscript;

  void                  init(const char* script_); // #IGNORE
  void                  released(); // override
/*no more hacks!!!  override void               mousePressEvent(QMouseEvent* mev); //we hack these to accept right mouse click
  override void         mouseReleaseEvent(QMouseEvent* mev);
  override void         mouseMoveEvent(QMouseEvent*  mev); */
};


//////////////////////////
//      Script Button   //
//////////////////////////
/* TODO:
// this button generates script when pressed
class ScriptButton : public ivButton{ // #IGNORE
public:
  String script;
  ScriptButton(char* name, ivAction *a, char* srp);
  void release(const ivEvent&);
};
*/

class taiToken;
class taiTypeHier;

//////////////////////////////////
//      taiChoiceDialog         //
//////////////////////////////////

// Button 0 is the default if user presses Enter
// Button 1 is the Cancel, if user presses Esc (only if "no_cancel" option false)
// User can also press the number associated with the button, starting from 0

class TA_API taiChoiceDialog : public QMessageBox {
  // ##IGNORE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
  Q_OBJECT
  INHERITED(QMessageBox)

public:
  static const String delimiter;

  static int ChoiceDialog(
    QWidget* parent_,
    const String& msg,
    const String& but_list,
    const char* win_title = "");

  static bool ErrorDialog(
    QWidget* parent_,
    const char* msg,
    const char* win_title = "Error",
    bool copy_but = true,
    bool cancel_errs_but = true);

  static void ConfirmDialog(
    QWidget* parent_,
    const char* msg,
    const char* win_title = "",
    bool copy_but = true);

  QButtonGroup* bgChoiceButtons; //note: not a widget

  override void done(int r);

  // Note: QMessageBox::exec() is not virtual, so this is not an override.
  // Make it virtual from this point forward, in case subclasses need
  // to override for some reason.
  virtual int exec();

  ~taiChoiceDialog() { }

protected:
  int num_chs; // number of choices, we return last idx for Esc
  virtual void keyPressEvent(QKeyEvent* ev); // override

protected slots:
  void copyToClipboard(); // copy text of msg to clipboard

#ifndef __MAKETA__
private:
  taiChoiceDialog(
    Icon icon,
    const QString& title,
    const QString& text,
    String but_list,
    QWidget* parent,
    bool copy_but);

  virtual QAbstractButton* Constr_OneBut(String lbl, int id, ButtonRole role);
#endif
};

//////////////////////////
//      iHostDialog     //
//////////////////////////

class TA_API iHostDialog : public iDialog {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS dialog with taiDataHostBase owner
  INHERITED(iDialog)
  Q_OBJECT
friend class taiDataHostBase;
public:
  iHostDialog(taiDataHostBase* owner_, QWidget* parent = 0, int wflags = 0);
  ~iHostDialog();

  bool          post(bool modal); // simplified version of post_xxx routines, returns true if accepted or false (if modal) if cancelled
  void          dismiss(bool accept_);

  void          iconify();   // Iv compatibility routine
  void          deiconify(); // Iv compatibility routine
  void          setCentralWidget(QWidget* widg); // is put in a scroll area; dialog is limited to screen size
  void          setButtonsWidget(QWidget* widg); // is put at the bottom, not in a scroll
protected:
  taiDataHostBase*      owner;
  QVBoxLayout*  layOuter;
  QWidget*      mcentralWidget;
  QScrollArea*  scr;
  override void closeEvent(QCloseEvent* ev);
};


//////////////////////////
//      EditDataPanel   //
//////////////////////////

class TA_API EditDataPanel: public iDataPanelFrame {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for any edit dialog-like data panel
INHERITED(iDataPanelFrame)
friend class taiEditDataHost;
public:
  iColor*               bgcol; // temp holding spot
  taiEditDataHost*      editDataHost() {return owner;}
  override String       panel_type() const; // this string is on the subpanel button for this panel
  override void         Closing(CancelOp& cancel_op);
  override const iColor GetTabColor(bool selected, bool& ok) const; // special color for tab; NULL means use default
  override bool         HasChanged_impl(); // 'true' if user has unsaved changes -- used to prevent browsing away
  override void         UpdatePanel(); // always do it, even when hidden; the edit sorts it out
  override QWidget*     firstTabFocusWidget();

  EditDataPanel(taiEditDataHost* owner_, taiDataLink* dl_);
  ~EditDataPanel();

public: // IDataLinkClient interface
  override TypeDef*     GetTypeDef() const {return &TA_EditDataPanel;}

protected:
  taiEditDataHost*      owner;
  override void         UpdatePanel_impl(); // the refresh guy!
  override void         Render_impl();
  override void         ResolveChanges_impl(CancelOp& cancel_op);
  override void         showEvent(QShowEvent* ev);

};//


class TA_API iMethodButtonMgr: public QObject, virtual public IDataLinkClient
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS an that can be conveniently used anywhere to provide the meth buttons of an edit -- note: requires an IDataHost and gui objects
INHERITED(QObject)
  Q_OBJECT
public:
  inline QLayout*       lay() const {return m_lay;}

  bool                  show_meth_buttons; // set when we have any guys to show


  void                  setBase(taBase* value);

  void                  Constr(taBase* base,
    IDataHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void                  Constr(QWidget* widg, QLayout* lay, taBase* base,
    IDataHost* host = NULL); // #IGNORE -- note: host prob not needed, can be removed
  void                  AddMethButton(taiMethodData* mth_rep,
    const String& label = _nilString);
  void                  GetImage();
  void                  Reset();

  iMethodButtonMgr(QObject* parent = NULL);
  iMethodButtonMgr(QWidget* widg, QLayout* lay, QObject* parent = NULL);
  ~iMethodButtonMgr();

protected:
  QWidget*              widg; // the host widget
  QLayout*              m_lay; // usually an iFlowLayout or QHBoxLayout, margins/spacing set
  IDataHost*    host; // must have outer lifetime to us!
  taBase*               base; // the object that has the methods
  TypeDef*              typ;
  taiMenu_List          ta_menu_buttons; // menu representations (from methods -- menubuttons only)
  taiActions*           cur_menu_but; // current menu button to add to (if not otherwise
  taiDataList           meth_el;        // method elements

  void                  Constr_impl(taBase* base, IDataHost* host); // #IGNORE
  virtual void          Constr_Methods_impl(); // #IGNORE
  void                  DoAddMethButton(QWidget* but); // #IGNORE
  void                  SetCurMenuButton(MethodDef* md);

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() {return this;} // override
  TypeDef*      GetTypeDef() const {return &TA_QWidget;} // override
public: // IDataLinkClient i/f -- note: only registered though for taiEDH and later
  void          DataLinkDestroying(taDataLink* dl);
  void          DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

private:
  void          Init();
};

#include <taiDataHostBase>
#include <taiDataHost_impl>
#include <taiDataHost>


class TA_API MembSet_List : public taPtrList<MembSet> { // #IGNORE -- note that 1st list is always the default (no parent) -- leave it empty to have no root items
public:
  int                   def_size; // set to how many you want to use default processing
  void                  SetMinSize(int n); // make sure there are at least n sets
  void                  ResetItems(bool data_only = false); // calls Reset on all lists
  bool                  GetFlatDataItem(int idx, MemberDef** mbr, taiData** dat = NULL);
   // get the dat and/or mbr (both optional) from a flat idx
  int                   GetFlatDataIndex(taiData* dat);
  int                   GetFlatDataIndex(MemberDef* mbr, taBase* base);
   // get the flat idx from a mbr/dat, -1 if not found
  int                   GetDataSize() const; // # data items

  MembSet_List()  {def_size = 0;}
  ~MembSet_List();
protected:
  void  El_Done_(void* it) { delete (MembSet*)it; }
};


class TA_API taiEditDataHost : public taiDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for classes -- default is to assume a EditDataPanel as the widget, but the Edit subclasses override that
  Q_OBJECT
INHERITED(taiDataHost)
friend class EditDataPanel;
public:
  enum DefMembSet { // keys for default members sets -- always created
    MS_NORM,    // normal members, always shown
    MS_EXPT,    // Expert members
    MS_HIDD,    // Hidden members

    MS_CNT      = 3 // number of default members
  };

  taiMenu_List          ta_menus;       // menu representations (from methods, non-menubuttons only)
  taiMenu_List          ta_menu_buttons;        // menu representations (from methods -- menubuttons only)
  taiActions*           cur_menu;       // current menu to add to (if not otherwise spec'd)
  taiActions*           cur_menu_but; // current menu button to add to (if not otherwise spec'd)

#ifdef TA_OS_MAC
  // See bug 1518.
  taiActions*           menu; // menu bar
#else
  taiMenuBar*           menu; // menu bar
#endif

  MembSet_List          membs;
  QButtonGroup*         bgrp; // group used for set checkboxes
  taiDataList           meth_el;        // method elements

  //NOTE: we provide indexed access to references here for convenience, but be careful!
  const bool&           show_set(int i) const // whether the set is shown
    {return membs.FastEl(i)->show;}
  bool&                 show_set(int i) // whether the set is shown
    {return membs.FastEl(i)->show;}
  const Member_List&    memb_el(int i) const // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  Member_List&          memb_el(int i) // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  const taiDataList&    data_el(int i) const // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->data_el;}
  taiDataList&          data_el(int i) // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->data_el;}

  EditDataPanel*        dataPanel() {return panel;} // #IGNORE
  override void         guiParentDestroying() {panel = NULL;}

  bool                  SetShow(int value, bool no_refresh = false); // change show value; returns true if we rebuilt/reshowed dialog

  taiEditDataHost(void* base, TypeDef* typ_ = NULL, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditDataHost()             { };
  ~taiEditDataHost();


  override int          Edit(bool modal_ = false, int min_width=-1, int min_height=-1);
  // for dialogs -- add to list of active_edit dialogs too
  EditDataPanel*        EditPanel(taiDataLink* link); // for panels
  EditDataPanel*        EditPanelDeferred(taiDataLink* link); // for panels
  USING(inherited::GetImage)
  void                  GetImage(bool force); //override
  void                  GetValue(); //override
  virtual bool          ShowMember(MemberDef* md) const; // #OBS
  void                  SetCurMenu(MethodDef* md); // sets or creates the cur_menu, for subsequent adding of items
  virtual void          SetCurMenu_Name(String men_nm); // sets or creates the cur_menu -- leave blank for implicit ("Actions")
  virtual void          SetCurMenuButton(MethodDef* md);
  override void         Raise() {if (isPanel()) DoRaise_Panel(); else taiDataHost::Raise();}
  override void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
    // check for unsaved changes and prompt to save/discard; called by several places prior to closing tab window, closing dialog, shutting down app, etc.
  override void         Cancel_impl();
  virtual void          GetButtonImage(bool force = true);

public: // routines for the taiDelegate style of edit host
  virtual void          GetImage_Item(int row) {} // called from GetImage and ed->GetValue

public: // ITypedObject i/f (common to IDLC and IDH)
  override TypeDef*     GetTypeDef() const {return &TA_taiEditDataHost;}
public slots:
// IDataHost i/f
  override iMainWindowViewer* viewerWindow() const;

protected:
  EditDataPanel* panel; //NOTE: not used when invoked by Edit()
  bool                  inline_mode; // true when doing inline, set early in constr
  bool                  no_meth_menu; // for Seledit guys, don't use meth menus

  override void         InitGuiFields(bool virt = true);
  override void         Constr_impl();
  virtual void          Enum_Members(); // called by Constr_impl to fill memb_el[]
  override void         Constr_Methods_impl();
  override void         ClearBody_impl();
  override void         Constr_Strings();
  override void         Constr_Body();    // construct the data of the dialog
  virtual void          Constr_Data_Labels(); // calls Data then Labels -- override to do your own
  virtual void          Constr_Inline(); // called instead of Data/Labels when typ->requiresInline true
  virtual void          Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl);
  void                  Constr_MethButtons();
  override void         Constr_RegNotifies();
  override void         Constr_Final();
  virtual MemberDef*    GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc); // (use sel_item_idx) enables things like ProgCtrl to play

  override void         FillLabelContextMenu(QMenu* menu, int& last_id);
  virtual void          FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  virtual void          GetImage_Membs(); // for overridding
  virtual void          GetImage_Membs_def(); // calls GetImage_impl for all our lists
  virtual void          GetValue_Membs();
  virtual void          GetValue_Membs_def(); // calls GetValue_impl for all our lists
  virtual void          GetImage_impl(const Member_List* ms, const taiDataList& dl, void* base);
  virtual void          GetImageInline_impl(const void* base);
  virtual void          GetValue_impl(const Member_List* ms, const taiDataList& dl, void* base) const;
  virtual void          GetValueInline_impl(void* base) const;
  void                  AddMethButton(taiMethodData* mth_rep, const String& label = _nilString);
    // uses mth's label, if no label passed
  void                  DoAddMethButton(QWidget* but);
  void                  DoRaise_Panel(); // what Raise() calls for panels
  override void         DoConstr_Dialog(iHostDialog*& dlg);

  override bool         eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

protected slots:
  virtual void          DoSelectForEdit(QAction* act); // act.data will be index of the SelectEdit; sel_data_index will hold the index of the data item
  virtual void          bgrp_buttonClicked(int id); // one of the section checkboxes
};


class TA_API taiStringDataHost: public taiDataHostBase {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit dialog for displaying a single string value in an editor format
INHERITED(taiDataHostBase)
  Q_OBJECT
public:
  QTextEdit*            edit;
  MemberDef*            mbr; // the member being edited (doesn't have to be String)
  bool                  line_nos; // display line numbers in the editor view
  bool                  rich_text; // string has rich text, not plain

  void                  Constr(const char* prompt = "", const char* win_title = "");
  USING(inherited::GetImage)
  override void         GetImage();
  override void         GetValue();
  override void         Constr_Buttons();

  virtual void          SelectLines(int st_line, int end_line);
  // select a range of lines in the editor

  override bool         eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

  taiStringDataHost(MemberDef* mbr, void* base, TypeDef* typ_ = NULL,
                    bool read_only_ = false, bool modal_ = false, QObject* parent = 0,
                    bool line_nos_ = false, bool rich_text_ = false);
  ~taiStringDataHost();


protected slots:
  void          btnPrint_clicked();

protected:
  QPushButton*          btnPrint;

  void                  DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void         Constr_Strings();
  override void         Constr_Box();
  override void         Constr_RegNotifies();
  override void         DoConstr_Dialog(iHostDialog*& dlg);
  override void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
  override void         Ok_impl();
};

class TA_API taiWizardDataHost : public taiDataHost_impl {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for Wizards -- uses tab pages
  Q_OBJECT
INHERITED(taiDataHost_impl)
public:
  MembSet_List          membs; // one set per page

  QTabWidget*           tabs;

  Member_List&          memb_el(int i) // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}

//  override void               GetImage();
//  override void               GetValue();
  override void         Constr_Buttons();//

//  override bool       eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

  taiWizardDataHost(taWizard* base, TypeDef* typ_ = NULL,
    bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  ~taiWizardDataHost();

protected:
  String_PArray         page_names; // stores names, or blank if none
  override void         Constr_Strings();
  override void         Constr_Box();
  override void         Constr_Data_Labels();
  override void         Constr_Data_Labels_impl(int& idx, Member_List* ms,
     taiDataList* dl);
  override void         Constr_RegNotifies();
//  override void       DoConstr_Dialog(iHostDialog*& dlg);
  override void         Enum_Members(); // called by Constr_impl to fill memb_el[]
  override void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL);
  override void         Ok_impl();
};


class TA_API iPluginEditor: public QMainWindow {
  // editor window for plugins
INHERITED(QMainWindow)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjUrlRole = Qt::UserRole + 1, // Url stored in this
//    ObjCatRole  // for object category string, whether shown or not
  };
#endif

  String                dir_path;       // path to directory with files
  String                file_base;      // base name of files to edit

  static iPluginEditor* New(const String& dir, const String& file_bse);
  // main interface -- make a new editor

  virtual void          LoadFiles(); // load files into editors
  virtual void          SaveFiles(); // save files from editors
  virtual void          Compile(); // compile
  virtual void          ReBuild(); // full rebuild

#ifndef __MAKETA__
public:

  QWidget*              main_widg;
  QVBoxLayout*          main_vbox;
  iSplitter*            split;
  NumberedTextView*     hfile_view; // header file
  NumberedTextView*     cfile_view; // cpp file
  QHBoxLayout*          tool_box;
  QToolBar*             tool_bar;
  QAction*                actSave;
  QAction*                actCompile;
  QAction*                actReBuild;

protected:

  iPluginEditor();
  ~iPluginEditor();

  override void closeEvent(QCloseEvent* ev);
  override bool eventFilter(QObject *obj, QEvent *event);

protected slots:
  void                  save_clicked();
  void                  compile_clicked();
  void                  rebuild_clicked();

private:
  void          init(); // called by constructors
#endif // !__MAKETA__
};

#endif // tai_dialog_h

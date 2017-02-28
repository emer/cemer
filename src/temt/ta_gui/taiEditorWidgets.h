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

#ifndef taiEditorWidgets_h
#define taiEditorWidgets_h 1

// parent includes:
#include <taiEditor>
#include <IWidgetHost>

// member includes:
#include <TypeDef>

// declare all other types mentioned but not required to include:
class QFrame; //
class iFlowLayout; //
class iColor; //
class iLabel; //
class QMenu; //
class QContextMenuEvent; //


taTypeDef_Of(taiEditorWidgets);

class TA_API taiEditorWidgets: public taiEditor, virtual public IWidgetHost
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditor)
  Q_OBJECT
friend class iDialogEditor;
public:
  static void           DoFillLabelContextMenu_CtrlPanel
    (QMenu* menu, int& last_id, taBase* rbase, MemberDef* md, QWidget* menu_par,
     QObject* slot_obj, const char* add_slot, const char* rmv_slot,
     const char* add_short_slot = NULL);
    // helper used here and in ProgEditor and elsewhere to handle control panel context menus -- last slot if present is for a add with short name (no parent path)
  static void           GetName(MemberDef* md, String& name, String& help_text); // returns one name, and optionally help_text

  QFrame*       frmMethButtons; // method buttons -- in body for dialogs, in outer panel for panel
  iFlowLayout*  layMethButtons; // method buttons

  virtual int           curRow() const {return 0;}

  virtual const iColor  colorOfRow(int row) const;      // background color for specified row (row need not exist); good for top and bottom areas
  inline bool           showMethButtons() const {return show_meth_buttons;} // true if any are created

  virtual taBase*       GetMembBase_Flat(int idx); // these are overridden by seledit
  virtual taBase*       GetMethBase_Flat(int idx);

  taiEditorWidgets(TypeDef* typ_ = NULL, bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  virtual ~taiEditorWidgets();

  void          ClearBody(bool waitproc = true);
   // prepare dialog for rebuilding Body to show new contents

  virtual void  Iconify(bool value);    // for dialogs: iconify/deiconify
  void          ReConstr_Body() override; // called when show has changed and body should be reconstructed -- this is a deferred call

  virtual void  SetRevert();    // set the revert button on
  virtual void  UnSetRevert();  // set the revert button off
  bool          ReShow(bool force = false) override; // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
  virtual void  Raise() {if (isDialog()) DoRaise_Dialog();}     // bring dialog or panel (in new tab) to the front
  void          GetImage(bool force) override {inherited::GetImage(force);} // scope ugh

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() override {return this;}
  TypeDef*      GetTypeDef() const override {return &TA_taiEditorWidgets;}

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//  void         SigLinkDestroying(taSigLink* dl);
  void          SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) override;

public: // IWidgetHost i/f
  const iColor   colorOfCurRow() const override {return colorOfRow(curRow());}
  TypeItem::ShowMembs  show() const override; // legacy -- just returns the app value
  bool          HasChanged() override {return modified;}
  bool          isConstructed() override {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  bool          isModal() override {return modal;}
  bool          isReadOnly() override {return read_only;} //
// iMainWindowViewer* viewerWindow() const; n/a here -- defined in taiEDH
  void*         Root() const override {return root;} // root of the object
  taBase*       Base() const override {return Base_();} // root of the object, if a taBase
  TypeDef*      GetRootTypeDef() const override {return typ;} // TypeDef on the root, for casting
  void          GetImage()      override {GetImage(true);}
  void          GetValue()      override { }
public slots:
  void          Changed() override {inherited::Changed();}
  void          Apply_Async() override {inherited::Apply_Async(); }

protected:
  bool         show_meth_buttons; // true if any are created
  bool         ctrl_panel_mbrs; // support right-click for control panel of mbrs
  taiWidget*   ctrl_panel_dat; // ONLY used/valid in handling of context menu for control panels
  MemberDef*   ctrl_panel_mbr; // ONLY used/valid in handling of context menu for control panels
  taBase*      ctrl_panel_base; // ONLY used/valid in handling of context menu for control panels
  bool         rebuild_body; // #IGNORE set for second and subsequent build of body (show change, and seledit rebuild)

  virtual void ClearBody_impl(); // #IGNORE prepare dialog for rebuilding Body to show new contents -- INHERITING CLASSES MUST CALL THIS LAST
  void         Constr_Methods() override; // creates the box for buttons
  virtual void Constr_Methods_impl(); // actually makes methods -- stub this out to supress methods
  void         Insert_Methods() override; // insert the menu and methods, if made, and not owned elsewise
  //void       Constr_Final() override;
  virtual void FillLabelContextMenu(QMenu* menu, int& last_id); // last_id enables access menu items
  void         Cancel_impl() override;
  void         Ok_impl() override;

protected:
  void         InitGuiFields(bool virt = true) override; // NULL the gui fields -- virt used for ctor
  void         Refresh_impl(bool reshow) override;
protected slots:
  virtual void label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e);
  virtual void helpMenu_triggered();
};


#endif // taiEditorWidgets_h

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

#ifndef taiDataHost_impl_h
#define taiDataHost_impl_h 1

// parent includes:
#include <taiDataHostBase>
#include <IDataHost>

// member includes:
#include <TypeDef>

// declare all other types mentioned but not required to include:
class QFrame; //
class iFlowLayout; //
class iColor; //
class iLabel; //
class QMenu; //
class QContextMenuEvent; //


TypeDef_Of(taiDataHost_impl);

class TA_API taiDataHost_impl: public taiDataHostBase, virtual public IDataHost
{ // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiDataHostBase)
  Q_OBJECT
friend class iHostDialog;
public:
  static void           DoFillLabelContextMenu_SelEdit(QMenu* menu,
    int& last_id, taBase* rbase, MemberDef* md, QWidget* menu_par,
    QObject* slot_obj, const char* slot);
    // helper used here and in ProgEditor and elsewhere to handle Seledit context menus
  static void           GetName(MemberDef* md, String& name, String& help_text); // returns one name, and optionally help_text

  QFrame*       frmMethButtons; // method buttons -- in body for dialogs, in outer panel for panel
  iFlowLayout*  layMethButtons; // method buttons

  virtual int           curRow() const {return 0;}

  virtual const iColor  colorOfRow(int row) const;      // background color for specified row (row need not exist); good for top and bottom areas
  inline bool           showMethButtons() const {return show_meth_buttons;} // true if any are created

  virtual taBase*       GetMembBase_Flat(int idx); // these are overridden by seledit
  virtual taBase*       GetMethBase_Flat(int idx);

  taiDataHost_impl(TypeDef* typ_ = NULL, bool read_only_ = false, bool modal_ = false, QObject* parent = 0);
  virtual ~taiDataHost_impl();

  void                  ClearBody(bool waitproc = true);
   // prepare dialog for rebuilding Body to show new contents

  virtual void          Iconify(bool value);    // for dialogs: iconify/deiconify
  override void         ReConstr_Body(); // called when show has changed and body should be reconstructed -- this is a deferred call
  virtual void          Revert_force();
   // forcibly (automatically) revert buffer (prompts)
  virtual void          SetRevert();    // set the revert button on
  virtual void          UnSetRevert();  // set the revert button off
  override bool         ReShow(bool force = false); // rebuild the body; if changes and force=false then prompts user first; ret true if reshown
  virtual void          Raise() {if (isDialog()) DoRaise_Dialog();}     // bring dialog or panel (in new tab) to the front
  override void         GetImage(bool force) {inherited::GetImage(force);} // scope ugh

public: // ITypedObject i/f (common to IDLC and IDH)
  void*         This() {return this;} // override
  TypeDef*      GetTypeDef() const {return &TA_taiDataHost_impl;} // override

public: // ISigLinkClient i/f -- note: only registered though for taiEDH and later
//  void                DataLinkDestroying(taSigLink* dl);
  void          DataDataChanged(taSigLink* dl, int dcr, void* op1, void* op2);

public: // IDataHost i/f
  const iColor   colorOfCurRow() const {return colorOfRow(curRow());}
  TypeItem::ShowMembs  show() const; // legacy -- just returns the app value
  bool          HasChanged() {return modified;}
  bool          isConstructed() {int s = state & STATE_MASK;
    return ((s >= CONSTRUCTED) && (s < ZOMBIE));}
  bool          isModal() {return modal;}
  bool          isReadOnly() {return read_only;} //
// iMainWindowViewer* viewerWindow() const; n/a here -- defined in taiEDH
  void*         Root() const {return root;} // root of the object
  taBase*       Base() const {return Base_();} // root of the object, if a taBase
  TypeDef*      GetRootTypeDef() const {return typ;} // TypeDef on the root, for casting
  void          GetImage()      {GetImage(true);}
  void          GetValue()      { }
public slots:
  void          Changed() {inherited::Changed();}
  void          Apply_Async() {inherited::Apply_Async(); }

protected:
  bool                  show_meth_buttons; // true if any are created
  bool                  sel_edit_mbrs; // support right-click for seledit of mbrs
  taiData*              sel_item_dat; // ONLY used/valid in handling of context menu for select edits
  MemberDef*            sel_item_mbr; // ONLY used/valid in handling of context menu for select edits
  taBase*               sel_item_base; // ONLY used/valid in handling of context menu for select edits
  bool                  rebuild_body; // #IGNORE set for second and subsequent build of body (show change, and seledit rebuild)

  virtual void          ClearBody_impl(); // #IGNORE prepare dialog for rebuilding Body to show new contents -- INHERITING CLASSES MUST CALL THIS LAST
  override void         Constr_Methods(); // creates the box for buttons
  virtual void          Constr_Methods_impl(); // actually makes methods -- stub this out to supress methods
  override void         Insert_Methods(); // insert the menu and methods, if made, and not owned elsewise
  //override void       Constr_Final();
  virtual void          FillLabelContextMenu(QMenu* menu, int& last_id); // last_id enables access menu items
  override void         Cancel_impl();
  override void         Ok_impl();


protected:
  override void         InitGuiFields(bool virt = true); // NULL the gui fields -- virt used for ctor
  override void         Refresh_impl(bool reshow);
protected slots:
  virtual void  label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e);
  virtual void          helpMenu_triggered();
};


#endif // taiDataHost_impl_h

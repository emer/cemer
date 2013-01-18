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

#ifndef taiEditDataHost_h
#define taiEditDataHost_h 1

// parent includes:
#include <taiDataHost>
#include <taPtrList>

// member includes:
#ifndef __MAKETA__
#include <taiMenu_List>
#include <MembSet>
#include <taiDataList>
#endif

// declare all other types mentioned but not required to include:
class taiActions;
class QButtonGroup;             // #IGNORE
class EditDataPanel;
class taiMethodData;
class taiMenu_List;             // #IGNORE
class MembSet_List; // #IGNORE
class taiMenuBar; //

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

typedef taPtrList<taiEditDataHost> taiEditDataHost_List; // #IGNORE

#endif // taiEditDataHost_h

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

#ifndef taiEditorOfClass_h
#define taiEditorOfClass_h 1

// parent includes:
#include <taiEditorWidgetsMain>
#include <taPtrList>

// member includes:
#ifndef __MAKETA__
#include <taiWidgetActions_List>
#include <taiMemberWidgets>
#include <taiWidget_List>
#include <iWidget_List>
#else
class taiWidgetActions_List;
class taiMemberWidgets;
class taiWidget_List;
class iWidget_List;
#endif

// declare all other types mentioned but not required to include:
class taiWidgetActions; //
class QButtonGroup; //
class iPanelOfEditor; //
class taiWidgetMethod; //
class taiWidgetActions_List; //
class taiMemberWidgets_List; //
class taiWidgetMenuBar; //
class QAction; //

taTypeDef_Of(taiEditorOfClass);

class TA_API taiEditorOfClass : public taiEditorWidgetsMain {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit host for classes -- default is to assume a iPanelOfEditor as the widget, but the Edit subclasses override that
  Q_OBJECT
INHERITED(taiEditorWidgetsMain)
friend class iPanelOfEditor;
public:
  enum DeftaiMemberWidgets { // keys for default members sets -- always created
    MS_NORM,    // normal members, always shown
    MS_EXPT,    // Expert members
    MS_HIDD,    // Hidden members

    MS_CNT      = 3 // number of default members
  };

  taiWidgetActions_List ta_menus;       // menu representations (from methods, non-menubuttons only)
  taiWidgetActions_List ta_menu_buttons;        // menu representations (from methods -- menubuttons only)
  taiWidgetActions*     cur_menu;       // current menu to add to (if not otherwise spec'd)
  taiWidgetActions*     cur_menu_but; // current menu button to add to (if not otherwise spec'd)

#ifdef TA_OS_MAC
  // See bug 1518.
  taiWidgetActions*     menu; // menu bar
#else
  taiWidgetMenuBar*     menu; // menu bar
#endif

  taiMemberWidgets_List membs;
  QButtonGroup*         bgrp; // group used for set checkboxes
  taiWidget_List        meth_el;        // method elements
  iWidget_List          methbox_labels;    // labels in method box..
  MemberSpace           methbox_members;   // members that drive labels in method box

  //NOTE: we provide indexed access to references here for convenience, but be careful!
  const bool&           show_set(int i) const // whether the set is shown
    {return membs.FastEl(i)->show;}
  bool&                 show_set(int i) // whether the set is shown
    {return membs.FastEl(i)->show;}
  const Member_List&    memb_el(int i) const // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  Member_List&          memb_el(int i) // the member defs, typically enumerated once
    {return membs.FastEl(i)->memb_el;}
  const taiWidget_List&    widget_el(int i) const // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->widget_el;}
  taiWidget_List&          widget_el(int i) // data items, typically rebuilt each reshow
    {return membs.FastEl(i)->widget_el;}

  iPanelOfEditor*       dataPanel() {return panel;} // #IGNORE
  virtual  void         guiParentDestroying() {panel = NULL;}

  bool                  SetShow(int value, bool no_refresh = false); // change show value; returns true if we rebuilt/reshowed dialog

  taiEditorOfClass(void* base, TypeDef* typ_ = NULL, bool read_only_ = false,
        bool modal_ = false, QObject* parent = 0);
  taiEditorOfClass();
  ~taiEditorOfClass();


  int          Edit(bool modal_ = false, int min_width=-1, int min_height=-1) override;
  // for dialogs -- add to list of active_edit dialogs too
  iPanelOfEditor*        EditPanel(taiSigLink* link); // for panels
  iPanelOfEditor*        EditPanelDeferred(taiSigLink* link); // for panels
  using inherited::GetImage;
  void                  GetImage(bool force); //override
  void                  GetValue(); //override
  virtual bool          ShowMember(MemberDef* md) const; // #OBS
  void                  SetCurMenu(MethodDef* md); // sets or creates the cur_menu, for subsequent adding of items
  virtual void          SetCurMenu_Name(String men_nm); // sets or creates the cur_menu -- leave blank for implicit ("Actions")
  virtual void          SetCurMenuButton(MethodDef* md);
  void         Raise() override {if (isPanel()) DoRaise_Panel(); else taiEditorWidgetsMain::Raise();}
  void         ResolveChanges(CancelOp& cancel_op, bool* discarded = NULL) override;
    // check for unsaved changes and prompt to save/discard; called by several places prior to closing tab window, closing dialog, shutting down app, etc.
  void         Cancel_impl() override;
  virtual void          GetButtonImage(bool force = true);

public: // routines for the taiDelegate style of edit host
  virtual void          GetImage_Item(int row) {} // called from GetImage and ed->GetValue

public: // ITypedObject i/f (common to IDLC and IDH)
  TypeDef*     GetTypeDef() const override {return &TA_taiEditorOfClass;}
public slots:
// IWidgetHost i/f
  iMainWindowViewer* viewerWindow() const override;

protected:
  iPanelOfEditor* panel; //NOTE: not used when invoked by Edit()
  bool                  inline_mode; // true when doing inline, set early in constr
  bool                  no_meth_menu; // for Seledit guys, don't use meth menus

  void         InitGuiFields(bool virt = true) override;
  void         Constr_impl() override;
  virtual void          Enum_Members(); // called by Constr_impl to fill memb_el[]
  void         Constr_Methods_impl() override;
  void         ClearBody_impl() override;
  void         Constr_Strings() override;
  void         Constr_Body() override;    // construct the data of the dialog
  virtual void          Constr_Widget_Labels(); // calls Widget then Labels -- override to do your own
  virtual void          Constr_Inline(); // called instead of Widget/Labels when typ->requiresInline true
  virtual void          Constr_Widget_Labels_impl(int& idx, Member_List* ms,
     taiWidget_List* dl);
  void                  Constr_MethButtons();
  void         Constr_RegNotifies() override;
  void         Constr_Final() override;
  virtual MemberDef*    GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc); // (use sel_item_idx) enables things like ProgCtrl to play

  void         FillLabelContextMenu(QMenu* menu, int& last_id) override;
  virtual void          FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id);
  virtual void          GetImage_Membs(); // for overridding
  virtual void          GetImage_Membs_def(); // calls GetImage_impl for all our lists
  virtual void          GetValue_Membs();
  virtual void          GetValue_Membs_def(); // calls GetValue_impl for all our lists
  virtual void          GetImage_impl(const Member_List* ms, const taiWidget_List& dl, void* base);
  virtual void          GetImageInline_impl(const void* base);
  virtual void          GetValue_impl(const Member_List* ms, const taiWidget_List& dl, void* base) const;
  virtual void          GetValueInline_impl(void* base) const;
  void                  AddMethButton(taiWidgetMethod* mth_rep, const String& label = _nilString);
    // uses mth's label, if no label passed
  void                  DoAddMethButton(QWidget* but);
  void                  DoRaise_Panel(); // what Raise() calls for panels
  void         DoConstr_Dialog(iDialogEditor*& dlg) override;
  
  virtual void          Constr_Methbox_Labels();
  // construct #METHBOX_LABELS items
  virtual void          Update_Methbox_Labels();
  // update #METHBOX_LABELS items

  bool         eventFilter(QObject *obj, QEvent *event) override;
  // event filter to trigger apply button on Ctrl+Return

protected slots:
  virtual void          DoSelectForEdit(QAction* act); // act.data will be index of the SelectEdit; sel_data_index will hold the index of the data item
  virtual void          bgrp_buttonClicked(int id); // one of the section checkboxes
};

typedef taPtrList<taiEditorOfClass> taiEditorOfClass_List; // #IGNORE

#endif // taiEditorOfClass_h

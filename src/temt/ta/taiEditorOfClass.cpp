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

#include "taiEditorOfClass.h"
#include <taiWidgetMethod>
#include <iCheckBox>
#include <EditDataPanel>
#include <taiType>
#include <MethodDef>
#include <taiMember>
#include <taiMethod>
#include <iFlowLayout>
#include <taProject>
#include <iDialogEditor>
#include <iFormLayout>
#include <taiWidgetToolBar>
#include <iMainWindowViewer>
#include <taiWidgetMenuBar>

#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QButtonGroup>
#include <QAbstractButton>
#include <QLineEdit>
#include <QToolBar>
#include <QApplication>
#include <QMenuBar>
#include <QKeyEvent>


taiEditorOfClass::taiEditorOfClass(void* base, TypeDef* typ_, bool read_only_,
        bool modal_, QObject* parent)
:taiEditorWidgetsMain(typ_, read_only_, modal_, parent)
{
  root = base;
  // note: subclass might add more membs, and might set def_size to use them
  membs.SetMinSize(MS_CNT);
  membs.def_size = MS_CNT;
  show_set(MS_NORM) = true;

  for (int j = MS_EXPT; j <= MS_HIDD; ++j) {
    taiMemberWidgets* ms = membs.SafeEl(j);
    if (!ms) break; // shouldn't happen
    ms->modal = true;
    switch (j) {
    case MS_EXPT:
      ms->text = "Expert Items";
      ms->desc = "show member items that are usually only needed for advanced purposes";
      break;
    case MS_HIDD:
      ms->text = "Hidden Items";
      ms->desc = "show member items that are rarely needed by most users";
      break;
    default: continue; // shouldn't happen!
    }
  }


  inline_mode = false;
  no_meth_menu = false;
  bgrp = new QButtonGroup(this);
  bgrp->setExclusive(false);
  connect(bgrp, SIGNAL(buttonClicked(int)),
    this, SLOT(bgrp_buttonClicked(int)) );
  menu = NULL;
  InitGuiFields(false);
  ta_menu_buttons.own_items = true; // so they get destroyed
  //note: don't register for notification until constr starts
}

taiEditorOfClass::~taiEditorOfClass() {
  membs.Reset(); membs.def_size = 0;
  meth_el.Reset();
  taiMisc::active_edits.RemoveEl(this);
  taiMisc::css_active_edits.RemoveEl(this);
  // remove data client -- harmless if already done in Cancel
  taBase* rbase = Base();
  if  (rbase) {
    rbase->RemoveSigClient(this);
    root = NULL;
  }
  bgrp = NULL;
  if (menu) {
    delete menu;
    menu = NULL;
  }
}

// note: called non-virtually in our ctor, and virtually in WidgetDeleting
void taiEditorOfClass::InitGuiFields(bool virt) {
  if (virt) inherited::InitGuiFields();
  cur_menu = NULL;
  cur_menu_but = NULL;
  panel = NULL;
}


void taiEditorOfClass::AddMethButton(taiWidgetMethod* mth_rep, const String& label) {
  QWidget* but = mth_rep->GetButtonRep();
  DoAddMethButton(but);
  if(label.nonempty() && but->inherits("QAbstractButton")) {
    ((QAbstractButton*)but)->setText(label);
  }
}

void taiEditorOfClass::bgrp_buttonClicked(int id) {
  // id is an index of the membs
  iCheckBox* chk = qobject_cast<iCheckBox*>(bgrp->button(id));
  if (!chk) return; // shouldn't happen
  if ((id < 0) || (id >= membs.size)) return; // ditto
  show_set(id) = chk->isChecked();
  ReShow_Async();
}

void taiEditorOfClass::Cancel_impl() {
//NOTE: must be ok to call this if was still deferred
  // delete all methods and menu
  if (menu) {
    delete menu;
    menu = NULL;
  }
  taBase* rbase = Base();
  if  (rbase) {
    rbase->RemoveSigClient(this);
  }
  if (isPanel()) {
    if (panel != NULL)
      panel->ClosePanel();
  } else if (isControl()) {
    //TODO: need to verify what to do!
    ClearBody(false); // no event loop
  }
  inherited::Cancel_impl();
}

void taiEditorOfClass::ClearBody_impl() {
  // delete ALL the data items -- Qt will automatically disconnect the signals/slots
  for (int i = 0; i < membs.size; ++i)
    data_el(i).Reset();
  inherited::ClearBody_impl(); // deletes the body widgets, except structural ones
}

void taiEditorOfClass::Constr_impl() {
  inline_mode = (typ && typ->it->requiresInline());
  if (!inline_mode) {
    Enum_Members();
  }
  inherited::Constr_impl();
}

void taiEditorOfClass::Enum_Members() {
  if (membs.def_size <= 0) return; // not handling anything
  if (!typ) return; // class browser or such
  MemberSpace& ms = typ->members;
  for (int i = 0; i < ms.size; ++i) {
    MemberDef* md = ms.FastEl(i);
    if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
    if (md->ShowMember(~TypeItem::IS_NORMAL, TypeItem::SC_EDIT, TypeItem::IS_NORMAL)) {
      memb_el(MS_NORM).Add(md);
      continue;
    }
    if (membs.def_size <= MS_EXPT) continue;
    // set the show_set guys at this point to default to app values
    if (!(show() & TypeItem::NO_EXPERT))
      show_set(MS_EXPT) = true;
    if (md->ShowMember(0, TypeItem::SC_EDIT, TypeItem::IS_EXPERT)) {
      memb_el(MS_EXPT).Add(md);
      continue;
    }
    if (membs.def_size <= MS_HIDD) continue;
    if (!(show() & TypeItem::NO_HIDDEN))
      show_set(MS_HIDD) = true;
    if (md->ShowMember(~TypeItem::IS_HIDDEN & ~TypeItem::IS_NORMAL,
      TypeItem::SC_EDIT, TypeItem::IS_HIDDEN)) {
      memb_el(MS_HIDD).Add(md);
      continue;
    }
  }
}

void taiEditorOfClass::Constr_Body() {
  Constr_Body_impl();
  if (inline_mode) {
    dat_cnt = 0;
    Constr_Inline();
  } else {
    Constr_Data_Labels();
  }
}

void taiEditorOfClass::Constr_Data_Labels() {
  int idx = 0; // basically a row counter
  dat_cnt = 0; // NOT advanced for the section rows
  // Normal members
  if (MS_NORM >= membs.def_size) return; // don't do those
  if (show_set(MS_NORM) && (memb_el(MS_NORM).size > 0)) {
//    Constr_Data_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
    Constr_Data_Labels_impl(idx, &memb_el(MS_NORM), &data_el(MS_NORM));
  }
  for (int j = MS_EXPT; j <= MS_HIDD; ++j) {
    if (j >= membs.def_size) return; // don't do those
    taiMemberWidgets* ms = membs.SafeEl(j);
    if (!ms || ms->memb_el.size == 0) continue;
    String text = ms->text;
    String desc = ms->desc;
    iCheckBox* chk = new iCheckBox(text.chars(), body);
    AddSectionLabel(idx, chk, desc);
    ++idx;
    bgrp->addButton(chk, j);
    // if we are to show this section, then check the box, and build, else nothing else
    if (show_set(j)) {
      chk->setChecked(true);
//      Constr_Data_impl(idx, &memb_el(j), &data_el(j));
      Constr_Data_Labels_impl(idx, &memb_el(j), &data_el(j));
    }
  }
}

void taiEditorOfClass::Constr_Inline() {
  data_el(0).Reset(); // should already be clear
  // specify inline flag, just to be sure
  taiWidget* mb_dat = typ->it->GetDataRep(this, NULL, body, NULL, taiWidget::flgInline);
  data_el(0).Add(mb_dat);
  QWidget* rep = mb_dat->GetRep();
  bool fill_hor = mb_dat->fillHor();
  AddData(0, rep, fill_hor);
}

void taiEditorOfClass::Constr_Data_Labels_impl(int& idx, Member_List* ms,
  taiWidget_List* dl)
{
  String name;
  String desc;
  for (int i = 0; i < ms->size; ++i) {
    MemberDef* md = ms->FastEl(i);

    // Create data widget
    taiWidget* mb_dat = md->im->GetDataRep(this, NULL, body);
    dl->Add(mb_dat);
    QWidget* rep = mb_dat->GetRep();
    bool fill_hor = mb_dat->fillHor();
    //AddData(idx, rep, fill_hor);

    // create label
    name = "";
    desc = "";
    GetName(md, name, desc);
    AddNameData(idx, name, desc, rep, mb_dat, md, fill_hor);
    ++idx;
    ++dat_cnt;
  }
}

void taiEditorOfClass::Constr_Strings() {
//NOTE: this is INSANE!
  win_str = String(def_title());
  String desc;
  if (typ != NULL) {
    prompt_str = typ->name;
    taBase* rbase = Base();
    if (rbase) {
      desc = rbase->GetDesc(); // often empty -- use td if so
      if(rbase->GetOwner() != NULL)
        win_str += String(" ") + rbase->GetPathNames();
      if(rbase->GetName() != "") {
        win_str += String(" (") + rbase->GetName() + ")";
        prompt_str = rbase->GetName() + " (" + typ->name + ")";
      }
      else
        win_str += String(" (") + typ->name + ")";
    }
    if (desc.empty()) desc = typ->desc;
  }
  String sapr = def_prompt();
  if (!sapr.empty())
    prompt_str += ": " + sapr;
  else
    if (desc.nonempty()) prompt_str +=  ": " + desc;
}

void taiEditorOfClass::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  if ((typ == NULL) || no_meth_menu) return;

  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* md = typ->methods.FastEl(i);
    if ((md->im == NULL) || (md->name == "Edit")) // don't put edit on edit dialogs..
      continue;
    if (!md->ShowMethod()) continue;
    taiMethod* im = md->im;
//    taiWidgetMethod* mth_rep = md->im->GetMethodRep(root, this, NULL, frmMethButtons); //buttons are in the frame
    if (im == NULL)
      continue;

    taiWidgetMethod* mth_rep = NULL;
    if (md->HasOption("MENU_BUTTON")) {
      SetCurMenuButton(md);
      mth_rep = im->GetMenuMethodRep(root, this, NULL, NULL/*frmMethButtons*/);
      mth_rep->AddToMenu(cur_menu_but);
      meth_el.Add(mth_rep);
    }
    if (md->HasOption("MENU")) {
      SetCurMenu(md);
      mth_rep = im->GetMenuMethodRep(root, this, NULL, NULL/*frmMethButtons*/);
      mth_rep->AddToMenu(cur_menu);
      meth_el.Add(mth_rep);
    }
    if (md->HasOption("BUTTON")) {
      mth_rep = im->GetButtonMethodRep(root, this, NULL, frmMethButtons);
      AddMethButton(mth_rep);
      meth_el.Add(mth_rep);
    }
/*obs    // add to menu if a menu item
    if (mth_rep->is_menu_item) {
      if(md->HasOption("MENU_BUTTON")) {
        SetCurMenuButton(md);
        mth_rep->AddToMenu(cur_menu_but);
      } else {
        SetCurMenu(md);
        mth_rep->AddToMenu(cur_menu);
      }
    } else {
      AddMethButton(mth_rep);
    }*/
  }
}

void taiEditorOfClass::Constr_RegNotifies() {
  taBase* rbase = Base();
  if (rbase) {
    rbase->AddSigClient(this);
  }
}
//void taiEditorOfClass::Constr_ShowMenu() {
// moved to be by the setShow etc. calls, for clarity

void taiEditorOfClass::Constr_Final() {
  inherited::Constr_Final();
  if(body)
    body->installEventFilter(this); // hopefully everyone below body will get it too!
}

void taiEditorOfClass::DoAddMethButton(QWidget* but) {
  show_meth_buttons = true;
  // we use "medium" size for buttons
  but->setFont(taiM->buttonFont(taiMisc::fonMedium));
  but->setFixedHeight(taiM->button_height(taiMisc::sizMedium));
  if (but->parent() != frmMethButtons) {
     but->setParent(frmMethButtons);
  }
  layMethButtons->addWidget(but);
  but->show(); // needed when rebuilding
}

void taiEditorOfClass::DoRaise_Panel() {
  //TODO
}

void taiEditorOfClass::DoSelectForEdit(QAction* act){
//note: this routine is duplicated in the ProgEditor
  taProject* proj = dynamic_cast<taProject*>(((taBase*)root)->GetThisOrOwner(&TA_taProject));
  if (!proj) return;

  int param = act->data().toInt();
  SelectEdit* se = proj->edits.Leaf(param);

  if (!sel_item_base) return; // shouldn't happen!
  taBase* rbase = sel_item_base;
  MemberDef* md = sel_item_mbr;
  if (!md || !se || !rbase) return; //shouldn't happen...

  //NOTE: this handler adds if not on, or removes if already on
  int idx = se->FindMbrBase(rbase, md);
  if (idx >= 0) {
    se->RemoveField(idx);
  }
  else {
    se->SelectMember(rbase, md);
  }
}

MemberDef* taiEditorOfClass::GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc)
{
  MemberDef* md = NULL;
  if (!(membs.GetFlatDataItem(sel_idx, &md) && md))
    return NULL;
  taBase* rbase = Base();
  if (rbase) {
    if (base) *base = rbase;
    String tlbl = rbase->GetName().elidedTo(16);
    lbl = tlbl;
  }
  return md;
}

void taiEditorOfClass::DoConstr_Dialog(iDialogEditor*& dlg) {
  inherited::DoConstr_Dialog(dlg);
  if(!modal) {
#ifdef TA_OS_MAC
    // unfortunately, staysontop prevents any other dialog from opening..
    dlg->setWindowFlags(Qt::WindowMinimizeButtonHint);
#else
    dlg->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint);
#endif
  }
}

int taiEditorOfClass::Edit(bool modal_, int min_width, int min_height) {
  if (!modal_)
    taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  return inherited::Edit(modal_, min_width, min_height);
}

EditDataPanel* taiEditorOfClass::EditPanel(taiSigLink* link) {
  if (state != CONSTRUCTED)
    return NULL;
  if (panel == NULL)
    panel = new EditDataPanel(this, link); //TODO: make sure this conversion is always valid!!!
  panel->setCentralWidget(widget());
  panel->setButtonsWidget(widButtons);
  taiMisc::active_edits.Add(this); // add to the list of active edit dialogs
  state = ACTIVE;
  return panel;
}

EditDataPanel* taiEditorOfClass::EditPanelDeferred(taiSigLink* link) {
  panel = new EditDataPanel(this, link); //TODO: make sure this conversion is always valid!!!

  return panel;
}

void taiEditor::ConstrEditControl() {
  Constr("", "", HT_CONTROL);
//TEMP
//TODO: need to deal with the now wrongly based taiEDH stuff in taiMisc
  taiEditorOfClass* edh = dynamic_cast<taiEditorOfClass*>(this);
  if (edh) taiMisc::active_edits.Add(edh); // add to the list of active edit dialogs
  state = ACTIVE;
}

void taiEditorOfClass::FillLabelContextMenu(QMenu* menu, int& last_id) {
  inherited::FillLabelContextMenu(menu, last_id);
  if (sel_edit_mbrs) {
    FillLabelContextMenu_SelEdit(menu, last_id);
  }
}

void taiEditorOfClass::FillLabelContextMenu_SelEdit(QMenu* menu, int& last_id)
{
  DoFillLabelContextMenu_SelEdit(menu, last_id, sel_item_base, sel_item_mbr, body,
  this, SLOT(DoSelectForEdit(QAction*)));
}

void taiEditorOfClass::GetButtonImage(bool force) {
  // taMisc::DebugInfo("GetButtonImage", String(force));
  if(!typ || !mwidget || !frmMethButtons)  return;
  if(!force && !frmMethButtons->isVisible()) {
    // taMisc::DebugInfo("GetButtonImage", "not visible");
    return;
  }
  // taMisc::DebugInfo("GetButtonImage", "visible");

  for (int i = 0; i < meth_el.size; ++i) {
    taiWidgetMethod* mth_rep = (taiWidgetMethod*)meth_el.SafeEl(i);
    if ( !(mth_rep->hasButtonRep())) //note: construction forced creation of all buttons
      continue;

    mth_rep->UpdateButtonRep();
  }
}

void taiEditorOfClass::GetImage(bool force) {
  if ((host_type != HT_CONTROL) || (frmMethButtons != NULL))
    GetButtonImage(force); // does its own visible check
  if (!mwidget) return; // huh?
  //note: we could be invisible, so we only do what is visible
  if (!force && !mwidget->isVisible()) return;
  if ((typ == NULL) || (root == NULL)) return;
  if (state >= ACCEPTED ) return;
  ++updating;
  StartEndLayout(true);
  GetImage_PromptTitle();
  if (state > DEFERRED1) {
    GetImage_Membs();
  }
  Unchanged();
  StartEndLayout(false);
  --updating;
}

void taiEditorOfClass::GetImage_Membs() {
  cur_row = 0;
  if (inline_mode) {
    GetImageInline_impl(root);
  } else {
    GetImage_Membs_def();
  }

  // search through children to find first tab focus widget
  // skip over flags
  first_tab_foc = NULL;
  //  QList<QWidget*> list = qFindChildren<QWidget*>(body);
  QList<QWidget*> list = body->findChildren<QWidget*>();
  for (int i=0; i<list.size(); ++i) {
    QWidget* rep = list.at(i);
    if(// rep->isVisible() &&
       rep->isEnabled() &&
       (rep->focusPolicy() & Qt::TabFocus) &&
       !rep->inherits("QCheckBox")) {
      if(rep->inherits("QLineEdit")) {
        QLineEdit* qle = (QLineEdit*)rep;
        if(qle->isReadOnly()) continue;
      }
      first_tab_foc = rep;
      break;
    }
  }
}

void taiEditorOfClass::GetImageInline_impl(const void* base) {
  taiWidget* mb_dat = data_el(0).SafeEl(0);
  if (mb_dat)
    typ->it->GetImage(mb_dat, base);
}

void taiEditorOfClass::GetImage_Membs_def() {
  for (int i = 0; i < membs.def_size; ++i) {
    if (show_set(i) && (data_el(i).size > 0))
      GetImage_impl(&memb_el(i), data_el(i), root);
  }
}

void taiEditorOfClass::GetImage_impl(const Member_List* ms, const taiWidget_List& dl,
  void* base)
{
  for (int i = 0; i < dl.size; ++i) {
    MemberDef* md = ms->SafeEl(i);
    taiWidget* mb_dat = dl.SafeEl(i);
    if ((md == NULL) || (mb_dat == NULL))
      taMisc::Error("taiEditorOfClass::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i), "\n");
    else {
      if (typ && typ->IsActualTaBase())
        mb_dat->SetBase((taBase*)base); // used for things like Seledit context menu
      md->im->GetImage(mb_dat, base); // need to do this first, to affect visible
      // note: visibles are cached, so nothing happens if it hasn't changed
      layBody->setVisible(cur_row, mb_dat->visible());
      ++cur_row;
    }
  }
}

void taiEditorOfClass::GetValue() {
  if ((typ == NULL) || (root == NULL)) return;
  if (state >= ACCEPTED ) return;
  if(!mwidget->isVisible()) {
    taMisc::DebugInfo("taiEditorOfClass::GetValue attempt to GetValue with invisible widget!");
  }
  if (state > DEFERRED1) {
    GetValue_Membs();
  }
  GetButtonImage();
  Unchanged();
}

void taiEditorOfClass::GetValue_Membs() {
  taBase* rbase = Base();
  if(rbase && root) {
    taProject* proj = (taProject*)((taBase*)root)->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(rbase, "Edit", rbase);
    }
  }

  if (inline_mode) {
    GetValueInline_impl(root);
  } else {
    GetValue_Membs_def();
  }
  if (rbase) {
    rbase->UpdateAfterEdit();   // hook to update the contents after an edit..
    // for gui updates, always make sure that the names are unique within the list
    rbase->MakeNameUnique();
    taiMisc::Update(rbase);
  }
}

void taiEditorOfClass::GetValue_Membs_def() {
  for (int i = 0; i < membs.def_size; ++i) {
    if (show_set(i) && (data_el(i).size > 0))
      GetValue_impl(&memb_el(i), data_el(i), root);
  }
}

void taiEditorOfClass::GetValue_impl(const Member_List* ms, const taiWidget_List& dl,
  void* base) const
{
  taBase* rbase = Base();
  bool first_diff = true;
  for (int i = 0; i < ms->size; ++i) {
    MemberDef* md = ms->FastEl(i);
    taiWidget* mb_dat = dl.SafeEl(i);
    if (mb_dat == NULL)
      taMisc::Error("taiEditorOfClass::GetValue_impl(): unexpected dl=NULL at i ", String(i), "\n");
    else {
      md->im->GetMbrValue(mb_dat, base, first_diff);
      if(rbase) {
        rbase->MemberUpdateAfterEdit(md, true); // in edit dialog
      }
    }
  }
  if (!first_diff)
    taiMember::EndScript(base);
}

void taiEditorOfClass::GetValueInline_impl(void* base) const {
  taiWidget* mb_dat = data_el(0).SafeEl(0);
  if (mb_dat)
    typ->it->GetValue(mb_dat, base);
}

void taiEditorOfClass::ResolveChanges(CancelOp& cancel_op, bool* discarded) {
  // called by root on closing, dialog on closing, SLS_RESOLVE_NOW op, etc. etc.
  if (HasChanged()) {
    if(!mwidget->isVisible()) {
      taMisc::DebugInfo("taiEditorOfClass::ResolveChanges attempt to get value with invisible widget!");
    }
    GetValue();
  }
}

bool taiEditorOfClass::ShowMember(MemberDef* md) const {
  return (md->ShowMember(show(), TypeItem::SC_EDIT) && (md->im != NULL));
}

void taiEditorOfClass::SetCurMenu(MethodDef* md) {
  // note: men_nm will be blank if implicit (i.e. last one)
  // if no explicit name, and no menu yet, we use "Actions"
  String men_nm = md->OptionAfter("MENU_ON_");
  SetCurMenu_Name(men_nm);
}

void taiEditorOfClass::SetCurMenu_Name(String men_nm) {
  if (!menu) {
#ifdef TA_OS_MAC
    // This shouldn't be necessary but Mac still glitches occasionally.
    // See bug 1518.
    menu = new taiWidgetToolBar(widget(), taiMisc::fonSmall, NULL);
    vblDialog->insertWidget(0, menu->GetRep());
    vblDialog->insertSpacing(1, 2);
#else
    menu = new taiWidgetMenuBar(taiMisc::fonSmall, NULL, this, NULL, widget());
    QMenuBar *qmb = menu->rep_bar();
    vblDialog->setMenuBar(qmb);

    // This menubar should never be used as a native menubar.  Without
    // the following line, emergent doesn't work correctly on the Mac
    // or on Ubuntu with the Unity desktop, which both have a global
    // menubar.  Specifically, there is no way to access the normal
    // menubar (File, Edit, View) because this one (Object, SelectEdit)
    // is taking its place.
#if (QT_VERSION >= 0x040600)
    qmb->setNativeMenuBar(false);
#endif
#endif
  }

  if (men_nm.nonempty()) {
    cur_menu = ta_menus.FindName(men_nm);
    if (cur_menu != NULL) return;
  }

  if (cur_menu != NULL) return;

  if (men_nm.empty()) {
    men_nm = "Actions";
  }

  cur_menu = menu->AddSubMenu(men_nm);
  ta_menus.Add(cur_menu);
}


void taiEditorOfClass::SetCurMenuButton(MethodDef* md) {
  String men_nm = md->OptionAfter("MENU_ON_");
  if (men_nm != "") {
    cur_menu_but = ta_menu_buttons.FindName(men_nm);
    if (cur_menu_but != NULL)  return;
  }
  if (cur_menu_but != NULL)  return;

  if (men_nm == "")
    men_nm = "Misc"; //note: this description not great, but should be different from "Actions", esp. for
       // context menus in the browser (otherwise, there are 2 "Actions" menus); see also taSigLinkBase::FillContextMenu_impl
      // also, must work when it appears before the other label (ex "Misc", then "Actions" )
  cur_menu_but = taiWidgetActions::New(taiWidgetMenu::buttonmenu, taiWidgetMenu::normal, taiMisc::fonSmall,
            NULL, this, NULL, widget());
  cur_menu_but->setLabel(men_nm);
  DoAddMethButton(cur_menu_but->GetRep()); // rep is the button for buttonmenu
  ta_menu_buttons.Add(cur_menu_but);
}

iMainWindowViewer* taiEditorOfClass::viewerWindow() const {
  iMainWindowViewer* dv = NULL;
  if (panel) dv = panel->viewerWindow();
  return dv;
}

bool taiEditorOfClass::eventFilter(QObject* obj, QEvent* event) {
  if(event->type() != QEvent::KeyPress) {
    return inherited::eventFilter(obj, event);
  }

  QKeyEvent* e = static_cast<QKeyEvent *>(event);

  QCoreApplication* app = QCoreApplication::instance();
  iMainWindowViewer* mvw = viewerWindow();
  if(mvw) {
    mvw->FocusIsMiddlePanel();
    if(mvw->KeyEventFilterWindowNav(obj, e))
      return true;
  }

  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    if(modal)
      Ok();
    else
      Apply();
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  // for edit dialogs -- arrows = tabs..
  if(ctrl_pressed) {
    switch(e->key()) {
    case Qt::Key_N:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;              // we absorb this event
    case Qt::Key_P:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;              // we absorb this event
    case Qt::Key_V:
      if(taMisc::emacs_mode) {
        for(int i=0;i<5;i++)    // page up = 5
          app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
        return true;            // we absorb this event
      }
      return false;
    case Qt::Key_Down:
      for(int i=0;i<5;i++)
        app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;              // we absorb this event
    case Qt::Key_U:
    case Qt::Key_Up:
      for(int i=0;i<5;i++)
        app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;              // we absorb this event
    }
  }
  else {
    switch (e->key()) {
    case Qt::Key_Down:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;              // we absorb this event
    case Qt::Key_Up:
      app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;              // we absorb this event
    case Qt::Key_PageDown:
      for(int i=0;i<5;i++)
        app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
      return true;              // we absorb this event
    case Qt::Key_PageUp:
      for(int i=0;i<5;i++)
        app->postEvent(obj, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::ShiftModifier));
      return true;              // we absorb this event
    }
  }
  if(e->key() == Qt::Key_Escape) {
    if(modal)
      Cancel();
    else
      Revert();                 // do it!
    if(mvw)
      mvw->FocusCurTreeView(); // return focus back to current browser
    return true;
  }
  return QObject::eventFilter(obj, event);
}

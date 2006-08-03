// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.




#include "pdp_qtso.h"
#include "pdpshell.h"
#include "ta_qt.h"
#include "ta_qtdialog.h"
#include "netstru_qtso.h"
#include "program.h"

#include "icombobox.h"
#include "ilineedit.h"
#include "ispinbox.h"

#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QStackedWidget>

#include <Inventor/SoPath.h>
#include <Inventor/nodes/SoTransform.h>


//////////////////////////
//  taiProgVarType	//
//////////////////////////

int taiProgVarType::BidForType(TypeDef* td) {
   if (td->InheritsFrom(TA_ProgVar)) 
     return (inherited::BidForType(td) +1);
   else  return 0;
  return 0;
}

taiData* taiProgVarType::GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) 
{
  //note: we use a static New function because of funky construction-time virtual functions
  taiProgVar* rval = taiProgVar::New(typ, host_, par, gui_parent_, flags_);
  return rval;
}



//////////////////////////////////
//	taiSpecMember		//
//////////////////////////////////

int taiSpecMember::BidForMember(MemberDef* md, TypeDef* td) {
  if((td->InheritsFrom(TA_BaseSpec) || td->InheritsFrom(TA_BaseSubSpec))
     && !(md->HasOption("NO_INHERIT")))
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

// put some check here for if the spec base has no spec parent
//  (((TA_BaseSpec *) dlg->cur_base)->owner->GetOwner(dlg->typ) == NULL))

//   if((dlg != NULL) && (dlg->cur_base != NULL) && (dlg->typ != NULL) &&
//      dlg->typ->InheritsFrom(TA_BaseSpec) &&
//      (((BaseSpec*)dlg->cur_base)->GetOwner(&TA_BaseSpec) == NULL))
//   {

bool taiSpecMember::NoCheckBox(IDataHost* host_) const {
  void* base = host_->Base();
  TypeDef* typ = host_->GetBaseTypeDef();
  if((host_ == NULL) || (base == NULL) || (typ == NULL))
    return true;

  if(typ->InheritsFrom(TA_BaseSpec_Group))
    return false;		// always use a check box for these..

  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* bs = (BaseSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else if(typ->InheritsFrom(TA_BaseSubSpec)) {
    BaseSubSpec* bs = (BaseSubSpec*)base;
    if(bs->FindParent() != NULL)
      return false;		// owner has a parent spec, needs a box
    return true;		// no owner, no box..
  } else {
    TAPtr tap = (TAPtr)base;
    BaseSpec* bs = (BaseSpec*)tap->GetOwner(&TA_BaseSpec); // find an owner..
    if(bs == NULL)
      return true;		// no owner, no box..
    if(bs->FindParent() != NULL)
      return false;		// owner is owned by a spec, needs a box..
  }
  return true;			// default is to not have box...
}


taiData* taiSpecMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_) {
  if (NoCheckBox(host_)) {
    taiData* rdat;
    if (m_sub_types != NULL)
      rdat = sub_types()->GetDataRep(host_, par, gui_parent);
    else
      rdat = taiMember::GetDataRep_impl(host_, par, gui_parent, flags_);
    return rdat;
  } else {
    taiPlusToggle* rval = new taiPlusToggle(NULL, host_, par, gui_parent);
    rval->InitLayout();
    taiData* rdat;
    if (m_sub_types)
      rdat = sub_types()->GetDataRep(host_, rval, rval->GetRep(), NULL, flags_);
    else
      rdat = taiMember::GetDataRep_impl(host_, rval, rval->GetRep(), flags_);
    rval->data = rdat;
    rval->AddChildWidget(rdat->GetRep());
    rval->EndLayout();
    return rval;
  }
}

void taiSpecMember::GetImage_impl(taiData* dat, const void* base) {
  IDataHost* host_ = dat->host;
  if (NoCheckBox(host_)) {
    if (m_sub_types != NULL)
      sub_types()->GetImage(dat,base);
    else
      taiMember::GetImage_impl(dat, base);
  } else {
    taiPlusToggle* rval = (taiPlusToggle*)dat;
    if (m_sub_types != NULL)
      sub_types()->GetImage(rval->data,base);
    else
      taiMember::GetImage_impl(rval->data, base);
    bool uniq = false;
    if(typ->InheritsFrom(TA_BaseSpec)) {
      BaseSpec* es = (BaseSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    else {
      BaseSubSpec* es = (BaseSubSpec*)base;
      uniq = es->GetUnique(mbr->idx);
    }
    rval->GetImage(uniq);
    if(uniq)
      rval->orig_val = "true";
    else
      rval->orig_val = "false";
  }
}

void taiSpecMember::GetMbrValue(taiData* dat, void* base, bool& first_diff) {
  IDataHost* host_ = dat->host;
  if(NoCheckBox(host_)) {
    if (m_sub_types != NULL)
      sub_types()->GetMbrValue(dat, base, first_diff);
    else
      taiMember::GetMbrValue(dat, base, first_diff);
    return;
  }

  taiPlusToggle* rval = (taiPlusToggle*)dat;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    es->SetUnique(mbr->idx,rval->GetValue());
//    es->UpdateMbr(mbr->idx);
  }

  CmpOrigVal(dat, base, first_diff);
  if (m_sub_types != NULL)
    sub_types()->GetMbrValue(rval->data, base, first_diff);
  else
    taiMember::GetMbrValue(rval->data, base, first_diff);
}

void taiSpecMember::CmpOrigVal(taiData* dat, void* base, bool& first_diff) {
  if((taMisc::record_script == NULL) || !typ->InheritsFrom(TA_taBase))
    return;
  String new_val;
  bool uniq = false;
  if(typ->InheritsFrom(TA_BaseSpec)) {
    BaseSpec* es = (BaseSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  else {
    BaseSubSpec* es = (BaseSubSpec*)base;
    uniq = es->GetUnique(mbr->idx);
  }
  if(uniq)
    new_val = "true";
  else
    new_val = "false";
  if(dat->orig_val == new_val)
    return;
  if(first_diff)
    StartScript(base);
  first_diff = false;

  *taMisc::record_script << "  ths->SetUnique(\"" << mbr->name << "\", "
			   << new_val << ");" << endl;
}


//////////////////////////////////
//   taiProgVar		//
//////////////////////////////////

taiProgVar* taiProgVar::New(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
{
  taiProgVar* rval = new taiProgVar(typ_, host_, par, gui_parent_, flags);
  rval->Constr(gui_parent_);
  return rval;
}

taiProgVar::taiProgVar(TypeDef* typ_, IDataHost* host_, taiData* par, 
  QWidget* gui_parent_, int flags)
: inherited(typ_, host_, par, gui_parent_, flags)
{
  m_updating = 0;
}

taiProgVar::~taiProgVar() {
}

void taiProgVar::Constr(QWidget* gui_parent_) { 
  vt = 0;
  QWidget* rep_ = new QWidget(gui_parent_);
  SetRep(rep_);
  rep_->setMaximumHeight(taiM->max_control_height(defSize()));
  if (host != NULL) {
    SET_PALETTE_BACKGROUND_COLOR(rep_,*(host->colorOfCurRow()));
  }
  InitLayout();
  Constr_impl(gui_parent_, (mflags & flgReadOnly));
  EndLayout();
}

void taiProgVar::Constr_impl(QWidget* gui_parent_, bool read_only_) { 
  QWidget* rep_ = GetRep();
  QLabel* lbl = new QLabel("name", rep_);
  AddChildWidget(lbl, taiM->hsep_c);

  fldName = new taiField(&TA_taString, host, this, rep_, mflags & flgReadOnly);
  AddChildWidget(fldName->GetRep(), taiM->hsep_c);
  
  lbl = new QLabel("var type",rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  
  TypeDef* typ_var_enum = TA_ProgVar.sub_types.FindName("VarType");
  cmbVarType = new taiComboBox(true, typ_var_enum, host, this, rep_);
  
  AddChildWidget(cmbVarType->rep(), taiM->hsep_c);
  lbl->setBuddy(cmbVarType->rep());
  if (read_only_) {
    cmbVarType->rep()->setEnabled(false);
  } else {
    connect(cmbVarType, SIGNAL(itemChanged(int)), this, SLOT(cmbVarType_itemChanged(int)));
  }
  
  stack = new QStackedWidget(rep_);
  AddChildWidget(stack); // fill rest of space
  
  QWidget* sub_rep = NULL; //note: var reused in various sections below
  QHBoxLayout* hl = NULL; //note: var reused in various sections below
  
  // created in order of StackControls:
  // int
  incVal = new taiIncrField(typ, host, this, NULL, mflags & flgEditDialog);
  incVal->setMinimum(INT_MIN); //note: must be int
  incVal->setMaximum(INT_MAX); //note: must be int
  stack->addWidget(incVal->rep());
  
  // field
  fldVal = new taiField(typ, host, this, NULL, mflags & flgEditDialog);
  stack->addWidget(fldVal->rep());
  
  // toggle
  tglVal = new taiToggle(typ, host, this, NULL, mflags & flgEditDialog);
  stack->addWidget(tglVal->rep());
  
  // object, complicated!
  sub_rep = new QWidget();
  hl = new QHBoxLayout(sub_rep);
  hl->setMargin(0);
  lbl = new QLabel("min type", sub_rep);
  hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
  thValType = new taiTypeHier(taiActions::popupmenu, taiMisc::defFontSize, 
    &TA_taBase, host, this, sub_rep, (mflags & flgReadOnly));
  thValType->GetMenu();
  hl->addWidget(thValType->GetRep());  hl->addSpacing(taiM->hsep_c);
  lblObjectValue = new QLabel("value", sub_rep);
  hl->addWidget(lblObjectValue);  hl->addSpacing(taiM->hsep_c);
  tkObjectValue = new taiToken(taiActions::popupmenu, taiMisc::defFontSize, 
     thValType->typ, host, this, sub_rep, ((mflags & flgReadOnly) | flgNullOk));
  hl->addWidget(tkObjectValue->GetRep());  hl->addSpacing(taiM->hsep_c);
  
  stack->addWidget(sub_rep);
  
  // enum -- very complicated!
  sub_rep =  new QWidget();
  hl = new QHBoxLayout(sub_rep);
  hl->setMargin(0);
  
  lbl = new QLabel("enum type", sub_rep);
  hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
  thEnumType = new taiTypeHier(taiActions::popupmenu, taiMisc::defFontSize, 
    &TA_taBase, host, this, sub_rep, (mflags & flgReadOnly));
  thEnumType->enum_mode = true;
  thEnumType->GetMenu();
  hl->addWidget(thEnumType->GetRep()); hl->addSpacing(taiM->hsep_c);
  
  lbl = new QLabel("enum value", sub_rep);
  hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
  
  cboEnumValue = new taiComboBox(true, NULL, host, this, sub_rep, (mflags & flgReadOnly));
  hl->addWidget(cboEnumValue->GetRep());  hl->addSpacing(taiM->hsep_c);
  
  stack->addWidget(sub_rep);
  
  // DynEnum
  sub_rep =  new QWidget();
  hl = new QHBoxLayout(sub_rep);
  hl->setMargin(0);
  
  edDynEnum = taiEditButton::New(NULL, TA_DynEnum.ie, &TA_DynEnum, host, this,
      sub_rep, ((mflags & flgReadOnly) | flgEditOnly));
  hl->addWidget(edDynEnum->GetRep()); hl->addSpacing(taiM->hsep_c);
  
  lbl = new QLabel("enum value", sub_rep);
  hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
  
  cboDynEnumValue = new taiComboBox(true, NULL, host, this, sub_rep, (mflags & flgReadOnly));
  hl->addWidget(cboDynEnumValue->GetRep());  hl->addSpacing(taiM->hsep_c);
  
  stack->addWidget(sub_rep);
  
}

void taiProgVar::cmbVarType_itemChanged(int itm) {
  if (m_updating != 0) return;
  ++m_updating;
  // set combo box to right type
  int new_vt;
  cmbVarType->GetEnumValue(new_vt);
  SetVarType(new_vt);
  --m_updating;
}

void taiProgVar::DataChanged_impl(taiData* chld) {
  inherited::DataChanged_impl(chld);
  if (m_updating > 0) return;
  ++m_updating;
  if (chld == thEnumType) {
    cboEnumValue->SetEnumType(thEnumType->GetValue());
    //note: prev value of value may no longer be a valid enum value!
  } else  if (chld == thValType) {
    // previous token may no longer be in scope!
    tkObjectValue->typ = thValType->GetValue();
    tkObjectValue->GetUpdateMenu();
//    tkObjectValue->SetTypeScope(thValType->GetValue());
  }
  --m_updating;
}

void taiProgVar::GetImage(const ProgVar* var) {
  ++m_updating;
  fldName->GetImage(var->name);
  SetVarType(var->var_type);
  // we only transfer the value in use
  switch (varType()) {
  case ProgVar::T_Int:
    incVal->GetImage(var->int_val);
    break;
  case ProgVar::T_Real:
    fldVal->GetImageVar_(Variant(var->real_val)); 
    break;
  case ProgVar::T_Bool:
    tglVal->GetImage(var->bool_val); 
    break;
  case ProgVar::T_String:
    fldVal->GetImage(var->string_val); 
    break;
  case ProgVar::T_Object:
    thValType->GetImage(var->object_type);
    tkObjectValue->GetImage(var->object_val, var->object_type, NULL);// no scope
    break;
  case ProgVar::T_HardEnum:
    thEnumType->GetImage(var->hard_enum_type);
    cboEnumValue->SetEnumType(var->hard_enum_type);
    cboEnumValue->GetEnumImage(var->int_val);
    break;
  case ProgVar::T_DynEnum:
    edDynEnum->GetImage_(&(var->dyn_enum_val));
    UpdateDynEnumCombo(var);
    //note: dynenums use the index as the "value" here, but we don't care, and
    // treat that index as a "value"
    int dei = var->dyn_enum_val.value_idx;
    if (dei < 0) dei = 0;
    cboDynEnumValue->GetEnumImage(dei);
    break;
  }
  SetVarType(var->var_type);
  --m_updating;
}

void taiProgVar::GetValue(ProgVar* var) const {
  var->name = fldName->GetValue();
  var->var_type = (ProgVar::VarType)varType();
  // we only set the value for the type the user chose, and cleanup the rest
  switch (varType()) {
  case ProgVar::T_Int:
    var->int_val = incVal->GetValue();
    break;
  case ProgVar::T_Real:
    var->real_val = fldVal->GetValue().toDouble(); // note: we could check if ok...
    break;
  case ProgVar::T_Bool:
    var->bool_val = tglVal->GetValue(); 
    break;
  case ProgVar::T_String:
    var->string_val = fldVal->GetValue(); 
    break;
  case ProgVar::T_Object:
    var->object_type = thValType->GetValue();
    taBase::SetPointer(&var->object_val, tkObjectValue->GetValue());
    break;
  case ProgVar::T_HardEnum:
    var->hard_enum_type = thEnumType->GetValue();
    cboEnumValue->GetEnumValue(var->int_val);
    break;
  case ProgVar::T_DynEnum: // see notes in GetImage about what "value" is
    cboDynEnumValue->GetEnumValue(var->dyn_enum_val.value_idx);
    break;
  }
  // set all the unused values to blanks
  var->Cleanup();
}
  
void taiProgVar::SetVarType(int value) {
  ++m_updating;
  vt = value;
  cmbVarType->GetEnumImage(vt);
  switch (vt) {
  case ProgVar::T_Int:
    stack->setCurrentIndex(scInt);
    break;
  case ProgVar::T_Real:
  case ProgVar::T_String: 
    stack->setCurrentIndex(scField);
    break;
  case ProgVar::T_Bool:
    stack->setCurrentIndex(scToggle);
    break;
  case ProgVar::T_Object: 
    stack->setCurrentIndex(scBase);
//TODO ??   tabVal->GetImage(NULL); // obj, no scope
    break;
  case ProgVar::T_HardEnum: 
    stack->setCurrentIndex(scEnum);
    break;
  case ProgVar::T_DynEnum:
    stack->setCurrentIndex(scDynEnum);
    break;
  default: break ;
  }
  --m_updating;
}

void taiProgVar::UpdateDynEnumCombo(const ProgVar* var) {
  ++m_updating;
  cboDynEnumValue->Clear();
  const DynEnum& de = var->dyn_enum_val; // convenience
  for (int i = 0; i < de.size; ++i) {
    const DynEnumItem* dei = de.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cboDynEnumValue->AddItem(dei->name, i); //TODO: desc in status bar or such would be nice!
  }
  --m_updating;
}



//////////////////////////
//   iPdpDataBrowser	//
//////////////////////////

iPdpDataBrowser::iPdpDataBrowser(taBase* root_, MemberDef* md_, TypeDef* typ_, 
  PdpDataBrowser* browser_,  QWidget* parent)
:iDataBrowser(root_, md_, typ_, browser_,  parent)
{  
}

iPdpDataBrowser::~iPdpDataBrowser() {
}

void iPdpDataBrowser::Constr_Menu_impl() {
  inherited::Constr_Menu_impl();
  // customize the menu text
//TODO: add custom hints to the file items
}

ProjectBase* iPdpDataBrowser::curProject() {
  ProjectBase* rval = NULL;
  ISelectable* ci = curItem();
  if (ci) {
    taBase* ta = ci->taData(); // null if not tabase, but probably must be
    if (ta) {
      if (ta->InheritsFrom(&TA_ProjectBase))
        rval = (ProjectBase*)ta;
      else
        rval = (ProjectBase*)ta->GetOwner(&TA_ProjectBase);
    }
    // last resort is first project, if any
    if (!rval && pdpMisc::root)
      rval = pdpMisc::root->projects.SafeEl(0);
  }
  return rval;
}


void iPdpDataBrowser::fileNew() {
  if (!pdpMisc::root) return;
  pdpMisc::root->projects.New(); // let user choose type
}

void iPdpDataBrowser::fileOpen() {
  if (!pdpMisc::root) return;
  pdpMisc::root->projects.Load_File(&TA_ProjectBase);
}

void iPdpDataBrowser::fileSave() {
  ProjectBase* proj = curProject();
  if (!proj) return;
  proj->Save_File();
}

void iPdpDataBrowser::fileSaveAs() {
  ProjectBase* proj = curProject();
  if (!proj) return;
  proj->SaveAs_File();
}

void iPdpDataBrowser::fileSaveAll() {
  if (!pdpMisc::root) return;
  pdpMisc::root->SaveAll();
}

void iPdpDataBrowser::fileClose() {
  ProjectBase* proj = curProject();
  if (!proj) return;
//TODO: shouldn't we save, or confirm???
  proj->Close();
}


//////////////////////////
//   PdpDataBrowser	//
//////////////////////////

PdpDataBrowser::PdpDataBrowser(taBase* root_, MemberDef* md_, bool is_root_) {
  m_is_root = is_root_;
  root = root_;
  md = md_;
}

void PdpDataBrowser::Constr_Window_impl() {
  if (!root) return;
  m_window = new iPdpDataBrowser(root, md, root->GetTypeDef(), this);
}


//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

ipdpDataViewer::ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
  QWidget* parent)
:iT3DataViewer(root_, typ_, (T3DataViewer*)viewer_, parent)
{
}

/*void iNetworkViewer::SetActionsEnabled_impl() {
  //todo
} */


//////////////////////////
//	pdpDataViewer	//
//////////////////////////

pdpDataViewer* pdpDataViewer::New(ProjectBase* proj_) {
  pdpDataViewer* rval = new pdpDataViewer();
  proj_->AddDataView(rval);
  return rval;
}

void pdpDataViewer::Initialize() {
  data_base = &TA_ProjectBase;
}

void pdpDataViewer::Destroy() {
}

void pdpDataViewer::Constr_Window_impl() {
  m_window = new ipdpDataViewer(proj(), proj()->GetTypeDef(), this);
//note:multi is default  viewer_win()->sel_mode = iT3DataViewer::SM_MULTI;
}


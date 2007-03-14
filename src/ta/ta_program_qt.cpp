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

#include "ta_program_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "ta_TA_inst.h"

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "icombobox.h"
#include "ieditgrid.h"
#include "ilineedit.h"
#include "ispinbox.h"
#include "itreewidget.h"

//////////////////////////
//   taiProgVar		//
//////////////////////////

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
  init();
}

taiProgVar::~taiProgVar() {
}

void taiProgVar::init() {
  m_updating = 0;
  sc = scNone;
  vt = 0;
  stack = NULL;
  fldName = NULL;
  cmbVarType = NULL;
  incVal = NULL; // for: ints
  fldVal = NULL; // for: char, string, most numbers
  tglVal = NULL; // for: bool
  tglNCP = NULL;
  thEnumType = NULL;
  cboEnumValue = NULL;
  thValType = NULL;
  tkObjectValue = NULL;
  edDynEnum = NULL; // for invoking editor for values
  cboDynEnumValue = NULL;
}

void taiProgVar::Constr(QWidget* gui_parent_) { 
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
  QLabel* lbl = MakeLabel("name", rep_);
  AddChildWidget(lbl, taiM->hsep_c);

  fldName = new taiField(&TA_taString, host, this, rep_, mflags & flgReadOnly);
  AddChildWidget(fldName->GetRep(), taiM->hsep_c);
  
  lbl = MakeLabel("var type",rep_);
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

  lbl = MakeLabel("no ctrl panel", rep_);
  AddChildWidget(lbl, taiM->hsep_c);
  tglNCP = new taiToggle(typ, host, this, NULL, mflags & flgReadOnly);
  AddChildWidget(tglNCP->rep());
}

void taiProgVar::AssertControls(int value) {
  if (value == sc) return;
  // delete old
  switch (sc) {
//case scNone: // nothing 
  case scInt:
    incVal->Delete();
    incVal = NULL; 
    break;
  case scField:
    fldVal->Delete();
    fldVal = NULL; // for: char, string, most numbers
    break;
  case scToggle:
    tglVal->Delete();
    tglVal = NULL; // for: bool
    break;
  case scBase:
    thValType->Delete();
    tkObjectValue->Delete();
    thValType = NULL;
    tkObjectValue = NULL;
    break;
  case scEnum:
    thEnumType->Delete();
    cboEnumValue->Delete();
    thEnumType = NULL;
    cboEnumValue = NULL;
    break;
  case scDynEnum:
    edDynEnum->Delete();
    cboDynEnumValue->Delete();
    edDynEnum = NULL; // for invoking editor for values
    cboDynEnumValue = NULL;
    break;
  default: break; // compiler food
  }
  if (stack) {
    delete stack;
    stack = NULL;
  }
  
  QLabel* lbl = NULL; // used by many
  QHBoxLayout* hl = NULL;
  switch (value) {
//case scNone: // nothing
  case scInt: {
    incVal = new taiIncrField(typ, host, this, NULL, mflags & flgReadOnly);
    incVal->setMinimum(INT_MIN); //note: must be int
    incVal->setMaximum(INT_MAX); //note: must be int
    AddChildWidget(incVal->rep());
    } break;
  case scField: {
    fldVal = new taiField(typ, host, this, NULL, mflags & (flgReadOnly | flgEditDialog));
    AddChildWidget(fldVal->rep());
    } break;
  case scToggle: {
    tglVal = new taiToggle(typ, host, this, NULL, mflags & flgReadOnly);
    AddChildWidget(tglVal->rep());
    } break;
  case scBase: {
    stack = new QWidget();
    hl = new QHBoxLayout(stack);
    hl->setMargin(0);
    lbl = MakeLabel("min type", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    thValType = new taiTypeDefButton(&TA_taBase, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(thValType->GetRep());  hl->addSpacing(taiM->hsep_c);
    lbl = MakeLabel("value", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    tkObjectValue = new taiTokenPtrButton(thValType->typ, host, this, stack, 
      ((mflags & flgReadOnly) | flgNullOk | flgEditDialog));
    hl->addWidget(tkObjectValue->GetRep());  hl->addSpacing(taiM->hsep_c);
    
    AddChildWidget(stack);
    } break;
  case scEnum: {
    stack =  new QWidget();
    hl = new QHBoxLayout(stack);
    hl->setMargin(0);
    
    lbl = MakeLabel("enum type", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    thEnumType = new taiEnumTypeDefButton(&TA_taBase, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(thEnumType->GetRep()); hl->addSpacing(taiM->hsep_c);
    
    lbl = MakeLabel("enum value", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    
    cboEnumValue = new taiComboBox(true, NULL, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(cboEnumValue->GetRep());  hl->addSpacing(taiM->hsep_c);
    
    AddChildWidget(stack);
    } break;
  case scDynEnum: {
    stack =  new QWidget();
    hl = new QHBoxLayout(stack);
    hl->setMargin(0);
    
    edDynEnum = taiEditButton::New(NULL, TA_DynEnum.ie, &TA_DynEnum, host, this,
        stack, ((mflags & flgReadOnly) | flgEditOnly));
    hl->addWidget(edDynEnum->GetRep()); hl->addSpacing(taiM->hsep_c);
    
    lbl = MakeLabel("enum value", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    
    cboDynEnumValue = new taiComboBox(true, NULL, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(cboDynEnumValue->GetRep());  hl->addSpacing(taiM->hsep_c);
    
    AddChildWidget(stack);
    } break;
  default: break; // compiler food
  }
  sc = value;
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
    tkObjectValue->GetImage(tkObjectValue->token(), thValType->GetValue());
//    tkObjectValue->SetTypeScope(thValType->GetValue());
  }
  --m_updating;
}

void taiProgVar::GetImage(const ProgVar* var) {
  ++m_updating;
  fldName->GetImage(var->name);
  SetVarType(var->var_type); //asserts correct control type
  tglNCP->GetImage(var->HasVarFlag(ProgVar::NO_CTRL_PANEL)); 

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
  case ProgVar::T_Object: {
    thValType->GetImage(var->object_type, &TA_taBase);
    // get the host obj (usually Program) as scope obj
    taBase* host_obj = NULL;
    if (host && host->GetBaseTypeDef()->InheritsFrom(&TA_taBase)) {
      host_obj = (taBase*)host->Base();
    }
    tkObjectValue->GetImage(var->object_val.ptr(), var->object_type, host_obj);
    } break;
  case ProgVar::T_HardEnum:
    thEnumType->GetImage(var->hard_enum_type, &TA_taBase);
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
  --m_updating;
}

void taiProgVar::GetValue(ProgVar* var) const {
  var->name = fldName->GetValue();
  var->var_type = (ProgVar::VarType)varType();
  var->SetVarFlagState(ProgVar::NO_CTRL_PANEL, tglNCP->GetValue()); 
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
    var->object_val = tkObjectValue->GetValue();
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
    AssertControls(scInt);
    break;
  case ProgVar::T_Real:
  case ProgVar::T_String: 
    AssertControls(scField);
    break;
  case ProgVar::T_Bool:
    AssertControls(scToggle);
    break;
  case ProgVar::T_Object: 
    AssertControls(scBase);
//TODO ??   tabVal->GetImage(NULL); // obj, no scope
    break;
  case ProgVar::T_HardEnum: 
    AssertControls(scEnum);
    break;
  case ProgVar::T_DynEnum:
    AssertControls(scDynEnum);
    break;
  default: break ;
  }
  --m_updating;
}

void taiProgVar::UpdateDynEnumCombo(const ProgVar* var) {
  if (sc != scDynEnum) return;
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



//////////////////////////
// tabProgramViewType	//
//////////////////////////

int tabProgramViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_Program))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiDataLink* tabDataTableViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
} */

void tabProgramViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // control panel is default
  iProgramCtrlPanel* cp = new iProgramCtrlPanel(dl_);
  DataPanelCreated(cp);

  // then editor
  iProgramPanel* dp = new iProgramPanel(dl_);
  DataPanelCreated(dp);
  inherited::CreateDataPanel_impl(dl_);
}

//////////////////////////
// tabProgramGroupViewType	//
//////////////////////////

int tabProgramGroupViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_Program_Group))
    return (inherited::BidForView(td) +1);
  return 0;
}

/*taiDataLink* tabDataTableViewType::CreateDataLink_impl(taBase* data_) {
  return new tabListDataLink((taList_impl*)data_);
} */

void tabProgramGroupViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iProgramGroupPanel* dp = new iProgramGroupPanel(dl_);
  DataPanelCreated(dp);
  inherited::CreateDataPanel_impl(dl_);
}

//////////////////////////
//    iProgramEditor 	//
//////////////////////////

iProgramEditor::iProgramEditor(QWidget* parent)
:inherited(parent)
{
  Init();
}

iProgramEditor::~iProgramEditor() {
// just delete controls -- a Higher Power had to have saved before
//  setEditNode(NULL); // autosave here too
}

void iProgramEditor::Init() {
  m_changing = 0;
  read_only = false;
  m_modified = false;
  warn_clobber = false;
//  bg_color.set(TAI_Program->GetEditColor()); // always the same
  base = NULL;
  md_desc = NULL;
  row = 0;
  
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2);
  
  layEdit = new QVBoxLayout(layOuter);
  layEdit->setMargin(0);
  layEdit->addSpacing(taiM->vsep_c);
  
  body = new iEditGrid(false, false, 2, 1, 2, 1, this); //note: intrinsically sizes to 2 rows minimum
  body->setRowHeight(taiM->max_control_height(taiM->ctrl_size));
  setEditBgColor(NULL); // set defaults
  layEdit->addWidget(body, 1); // give all the space to this guy
  layEdit->addSpacing(taiM->vsep_c);
  
  QHBoxLayout* layButtons = new QHBoxLayout();
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  layButtons->addStretch();
  btnApply = new HiLightButton("&Apply", this);
  layButtons->addWidget(btnApply);
  layButtons->addSpacing(4);
  
  btnRevert = new HiLightButton("&Revert", this);
  layButtons->addWidget(btnRevert);
  layButtons->addSpacing(4);
  layEdit->addLayout(layButtons);
  
  items = new iTreeView(this, iTreeView::TV_AUTO_EXPAND);
  layOuter->addWidget(items, 1); // it gets the room
  items->setColumnCount(3);
  items->setSortingEnabled(false);// only 1 order possible
  items->setHeaderText(0, "Program Item");
  items->setHeaderText(1, "Item Detail");
  items->setColumnWidth(1, 160);
  items->setHeaderText(2, "Item Description");
  items->setColKey(1, taBase::key_disp_name); //note: ProgVars and Els have nice disp_name desc's
  items->setColFormat(1, iTreeView::CF_ELIDE_TO_FIRST_LINE);
  items->setColKey(2, taBase::key_desc); //note: ProgVars and Els have nice disp_name desc's
  items->setColFormat(2, iTreeView::CF_ELIDE_TO_FIRST_LINE);
  // adjunct data, tooltips, etc.
  items->AddColDataKey(1, taBase::key_disp_name, Qt::ToolTipRole); 
  items->AddColDataKey(2, taBase::key_desc, Qt::ToolTipRole);
  
  //enable dnd support
  items->setDragEnabled(true);
  items->setAcceptDrops(true);
  items->setDropIndicatorShown(true);
  items->setHighlightRows(true);
  
  connect(btnApply, SIGNAL(clicked()), this, SLOT(Apply()) );
  connect(btnRevert, SIGNAL(clicked()), this, SLOT(Revert()) );
  connect(items, SIGNAL(ItemSelected(iTreeViewItem*)),
    this, SLOT(items_ItemSelected(iTreeViewItem*)) );

  InternalSetModified(false);
}

void iProgramEditor::AddData(int row_, QWidget* data, QLayout* lay) {
  // just get the height right from the strip widget
//  layBody->setRowMinimumHeight(row, body->stripeHeight()); 
  row = row_;
  if (lay)
    body->setDataLayout(row, 0, lay);
  else {
    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->setMargin(0);
    hbl->addWidget(data, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
    hbl->addStretch();
    body->setDataLayout(row, 0, hbl);
  }
  data->show(); // needed for rebuilds, to make the widget show
}

void iProgramEditor::Base_Add() {
  base->AddDataClient(this);
  // we put the grid in a box so its rows don't expand
/*  QVBoxLayout* layGrid = new QVBoxLayout(body);
  layGrid->setSpacing(0);
  layGrid->setMargin(0);
  layBody = new QGridLayout(layGrid); 
  layBody->setSpacing(0);
  layBody->setMargin(0);
  // set row heights now, so only one row shows ok
//  layBody->setRowMinimumHeight(0, body->stripeHeight()); 
//  layBody->setRowMinimumHeight(1, body->stripeHeight()); 
  layGrid->addStretch(); */
  
  // get colors for type
  const iColor* bgc = base->GetEditColorInherit(); 
  setEditBgColor(bgc);

  int flags = taiData::flgInline | taiData::flgNoUAE;
  if (read_only) flags |= taiData::flgReadOnly;
  
  // add main inline controls in line 0, for whatever type
  TypeDef* typ = GetBaseTypeDef();
  taiData* mb_dat = typ->it->GetDataRep(this, NULL, body->dataGridWidget(), NULL, flags);
  data_el.Add(mb_dat);
  QWidget* rep = mb_dat->GetRep();
  AddData(0, rep/*, mb_dat->GetLayout()*/);
  
  // add desc control in line 1 for ProgEls (and any other type with a "desc" member
  md_desc = typ->members.FindName("desc");
  if (md_desc) {
    //NOTE: we don't make a poly guy because it seemed impossible to get field to stretch,
    // so we just get the field dude manually, and pack up in our own layout
    QHBoxLayout* hbl = new QHBoxLayout();
    hbl->setMargin(0);
    hbl->setSpacing(taiM->hsep_c);
    QLabel* lbl = taiM->NewLabel("desc", body->dataGridWidget());
    hbl->addWidget(lbl, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
    lbl->show();
    if (!read_only)
      flags |= taiData::flgEditDialog; // nice button for popup
    mb_dat = md_desc->im->GetDataRep(this, NULL, body->dataGridWidget(), NULL, flags);
    data_el.Add(mb_dat);
    rep = mb_dat->GetRep();
    hbl->addWidget(rep, 1,  (Qt::AlignVCenter)); // should consume all space
//    hbl->addStretch();
    AddData(1, rep, hbl);
    
    
    
  }
  
  // ok, get er!
  GetImage();
}

void iProgramEditor::Base_Remove() {
  base->RemoveDataClient(this);
  data_el.Reset(); // deletes the items
  md_desc = NULL; // for tidiness
  body->clearLater();
/*obs  // delete widgets in iStripe
  const QObjectList& ch = body->children();
  while (ch.count() > 0) {
    QObject* obj = ch.last();
    delete obj;
  } */
  base = NULL;
  taiMiscCore::RunPending(); // note: this is critical for the editgrid clear
}

void iProgramEditor::Apply() {
  if (warn_clobber) {
    int chs = taiChoiceDialog::ChoiceDialog
      (NULL, "Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?!Apply!Revert!Cancel!");
    if(chs == 1) {
      Revert();
      return;
    }
    if(chs == 2)
      return;
  }
  GetValue();
  GetImage();
  InternalSetModified(false); // superfulous??
}

const iColor* iProgramEditor::colorOfCurRow() const {
  if ((row % 2) == 0) {
    return &bg_color;
  } else {
    return &bg_color_dark;
  }
}

void iProgramEditor::DataLinkDestroying(taDataLink* dl) {
  setEditNode(NULL, false);
}
 
void iProgramEditor::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  if (m_changing > 0) return; // gets triggered when we do the GetValue on ctrl0
  if (dcr <= DCR_ITEM_UPDATED_ND) {
    // if it has been edited, (maybe??) warn user, else just silently update it
    if (m_modified) {
      warn_clobber = true; // no other visible sign, warned if save
    } else {
      GetImage();
      return;
    }
  }
}

void iProgramEditor::Changed() {
  if (m_changing > 0) return;
  InternalSetModified(true);
}

TypeDef* iProgramEditor::GetBaseTypeDef() {
  if (base) return base->GetTypeDef();
  else return &TA_void; // avoids null issues
}

void iProgramEditor::GetValue() {
  TypeDef* typ = GetBaseTypeDef();
  if (!typ) return; // shouldn't happen
  
  ++m_changing;
  InternalSetModified(false); // do it first, so signals/updates etc. see us nonmodified
  taiData* mb_dat = data_el.SafeEl(0);
  if (mb_dat) 
    typ->it->GetValue(mb_dat, base);
  mb_dat = data_el.SafeEl(1);
  if (mb_dat && md_desc) {
    bool first_diff = true;
    md_desc->im->GetMbrValue(mb_dat, base, first_diff);
  }
  if (typ->InheritsFrom(TA_taBase)) {
    base->UpdateAfterEdit();	// hook to update the contents after an edit..
//shouldn't be necessary    taiMisc::Update(base);
  }
  --m_changing;
}

void iProgramEditor::GetImage() {
  TypeDef* typ = GetBaseTypeDef();
  if (!typ) return; // shouldn't happen
  
  ++m_changing;
  taiData* mb_dat = data_el.SafeEl(0);
  if (mb_dat) 
    typ->it->GetImage(mb_dat, base);
  mb_dat = data_el.SafeEl(1);
  if (mb_dat && md_desc) {
    md_desc->im->GetImage(mb_dat, base);
  }
  InternalSetModified(false);
  --m_changing;
}

void iProgramEditor::ExpandAll() {
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::HasChildren);
  QTreeWidgetItem* item;
  while ((item = *it)) { 
    items->setItemExpanded(item, true);
    ++it;
  }
  // size first N-1 cols
  int cols = items->columnCount(); // cache
  // make columns nice sizes (not last)
  for (int i = 0; i < (cols - 1); ++i) {
    items->resizeColumnToContents(i);
  }
}

void iProgramEditor::InternalSetModified(bool value) {
  m_modified = value;
  if (!value) warn_clobber = false;
  UpdateButtons();
}

void iProgramEditor::items_ItemSelected(iTreeViewItem* item) {
  TAPtr new_base = NULL;
  if (item) {
    new_base = item->taData(); // NULL if not a taBase
  }
  setEditNode(new_base);
}

void iProgramEditor::Revert() {
  GetImage();
  InternalSetModified(false);
}

void iProgramEditor::Refresh() {
  //NOTE: for refresh, we just update the items (notify should always work for add/delete)
  items->Refresh();
  if (!m_modified) {
    GetImage();
  }
}


void iProgramEditor::setEditBgColor(const iColor* value) {
  // default colors, if not supplied
  if (value) // defaults
    bg_color = *value;
  else
    bg_color.set(QApplication::palette().color(QPalette::Active, QColorGroup::Background));
  taiDataHost::MakeDarkBgColor(bg_color, bg_color_dark);
  body->setColors(bg_color, bg_color_dark);
}

void iProgramEditor::setEditNode(TAPtr value, bool autosave) {
  if (base == value) return;
  if (base) {
    if (m_modified && autosave) {
      Apply();
    }
    Base_Remove();
  }
  base = value;
  if (base) Base_Add();
}

void iProgramEditor::UpdateButtons() {
  if (base && m_modified) {
    btnApply->setEnabled(true);
    btnRevert->setEnabled(true);
  } else {
    btnApply->setEnabled(false);
    btnRevert->setEnabled(false);
  }
}

iMainWindowViewer* iProgramEditor::window() const {
  return m_window;
}


//////////////////////////
//   iProgramPanelBase 	//
//////////////////////////

iProgramPanelBase::iProgramPanelBase(taiDataLink* dl_)
:inherited(dl_)
{
  pe = new iProgramEditor();
  setCentralWidget(pe); //sets parent
}

void iProgramPanelBase::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iProgramPanelBase::HasChanged() {
  return pe->HasChanged();
}

void iProgramPanelBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the tree up to the panel
  pe->items->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  pe->m_window = itv->viewerWindow();
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  //ProgramToolBar* ptb = 
  (ProgramToolBar*)mvw->FindToolBarByType(&TA_ProgramToolBar,
    "Program");
/*TODO: re-enable once the program toolbar is defined
  if (!ptb)
    ptb = (ProgramToolBar*)mvw->AddToolBarByType(&TA_ProgramToolBar,
    "Program");*/
}

void iProgramPanelBase::Refresh_impl() {
  pe->Refresh();
}

void iProgramPanelBase::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pe->HasChanged()) {
    pe->Apply();
  }
}


//////////////////////////
//   iProgramPanel 	//
//////////////////////////

iProgramPanel::iProgramPanel(taiDataLink* dl_)
:inherited(dl_)
{
  Program* prog_ = prog();
  if (prog_) {
    taiDataLink* dl = (taiDataLink*)prog_->GetDataLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->setDefaultExpandLevels(6); // shouldn't generally be more than this
  pe->items->setDecorateEnabled(true); //TODO: maybe make this an app option
  connect(pe->items, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(items_CustomExpandFilter(iTreeViewItem*, int, bool&)) );
}

void iProgramPanel::items_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand) 
{
  if (level < 1) return; // always expand root level
  // by default, expand code guys throughout, plus top-level args, vars and objs
  taiDataLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  if((level <= 1) && typ->InheritsFrom(&TA_ProgVar_List)) return; // only top guys: args, vars
  if(typ->DerivesFrom(&TA_ProgEl_List) || typ->DerivesFrom(&TA_ProgObjList))
    return;			// expand
  if(typ->InheritsFrom(&TA_ProgEl)) {
    String mbr = typ->OptionAfter("DEF_CHILD_");
    if(!mbr.empty()) {
      MemberDef* md = typ->members.FindName(mbr);
      if(md) {
	if(md->type->InheritsFrom(&TA_ProgEl_List)) return; // expand
      }
      expand = false;		// don't expand any non-progel def childs
    }
    else {
      return;			// ok to expand
    }
  }
  // otherwise, nada
  expand = false;
}

void iProgramPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  //ProgramToolBar* ptb = 
  (ProgramToolBar*)mvw->FindToolBarByType(&TA_ProgramToolBar,
    "Program");
/*TODO: re-enable once the program toolbar is defined
  if (!ptb)
    ptb = (ProgramToolBar*)mvw->AddToolBarByType(&TA_ProgramToolBar,
    "Program");*/
}


//////////////////////////
//   iProgramGroupPanel //
//////////////////////////

iProgramGroupPanel::iProgramGroupPanel(taiDataLink* dl_)
:inherited(dl_)
{
  pe->items->AddFilter("ProgGp");
  Program_Group* prog_ = progGroup();
  if (prog_) {
    taiDataLink* dl = (taiDataLink*)prog_->GetDataLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  }
  pe->items->setDefaultExpandLevels(2); 
  connect(pe->items, SIGNAL(CustomExpandFilter(iTreeViewItem*, int, bool&)),
    this, SLOT(items_CustomExpandFilter(iTreeViewItem*, int, bool&)) );
}

void iProgramGroupPanel::items_CustomExpandFilter(iTreeViewItem* item,
  int level, bool& expand) 
{
  if (level < 1) return; // always expand root level
  // by default, we don't expand code and objs,  but do expand
  // the args, and vars.
  taiDataLink* dl = item->link();
  TypeDef* typ = dl->GetDataTypeDef();
  if (typ->DerivesFrom(&TA_ProgEl_List) ||
    typ->DerivesFrom(&TA_ProgObjList)
  )  {
    expand = false;
  }
}



//////////////////////////
//   ProgramToolBoxProc	//
//////////////////////////

void ProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Program"); //note: need to keep it short
  // for vars
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("var",
    tabMisc::root->GetTemplateInstance(&TA_ProgVar)));
//   tb->AddClipToolWidget(sec, new iBaseClipToolWidget("arg",
//     tabMisc::root->GetTemplateInstance(&TA_ProgArg)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("loc vars",
    tabMisc::root->GetTemplateInstance(&TA_ProgVars)));

  // for els
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("block",
    tabMisc::root->GetTemplateInstance(&TA_CodeBlock)));

  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("script",
    tabMisc::root->GetTemplateInstance(&TA_UserScript)));
  
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("for",
    tabMisc::root->GetTemplateInstance(&TA_ForLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("do",
    tabMisc::root->GetTemplateInstance(&TA_DoLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("while",
    tabMisc::root->GetTemplateInstance(&TA_WhileLoop)));
  
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("if",
    tabMisc::root->GetTemplateInstance(&TA_IfElse)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("if.con",
    tabMisc::root->GetTemplateInstance(&TA_IfContinue)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("if.brk",
    tabMisc::root->GetTemplateInstance(&TA_IfBreak)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("if.ret",
    tabMisc::root->GetTemplateInstance(&TA_IfReturn)));
  
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("var=",
    tabMisc::root->GetTemplateInstance(&TA_AssignExpr)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("meth()",
    tabMisc::root->GetTemplateInstance(&TA_MethodCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("prog()",
    tabMisc::root->GetTemplateInstance(&TA_ProgramCall)));

  ////////////////////////////////////////////////////////////////////////////
  //		Prog2
  sec = tb->AssertSection("Prog2");
  //  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("static()",
    tabMisc::root->GetTemplateInstance(&TA_StaticMethodCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("math()",
    tabMisc::root->GetTemplateInstance(&TA_MathCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("random()",
    tabMisc::root->GetTemplateInstance(&TA_RandomCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("misc()",
    tabMisc::root->GetTemplateInstance(&TA_MiscCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("img proc()",
    tabMisc::root->GetTemplateInstance(&TA_ImageProcCall)));
  // add other spec meth calls here..

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("fun def",
    tabMisc::root->GetTemplateInstance(&TA_Function)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("fun()",
    tabMisc::root->GetTemplateInstance(&TA_FunctionCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("return",
    tabMisc::root->GetTemplateInstance(&TA_ReturnExpr)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("print",
    tabMisc::root->GetTemplateInstance(&TA_PrintVar)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("comment",
    tabMisc::root->GetTemplateInstance(&TA_Comment)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("stop/step",
    tabMisc::root->GetTemplateInstance(&TA_StopStepPoint)));

  ////////////////////////////////////////////////////////////////////////////
  //		Data processing
  sec = tb->AssertSection("Data Proc"); //note: need to keep it short
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("data proc()",
    tabMisc::root->GetTemplateInstance(&TA_DataProcCall)));
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("sort",
    tabMisc::root->GetTemplateInstance(&TA_DataSortProg)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("group",
    tabMisc::root->GetTemplateInstance(&TA_DataGroupProg)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("sel rows",
    tabMisc::root->GetTemplateInstance(&TA_DataSelectRowsProg)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("sel cols",
    tabMisc::root->GetTemplateInstance(&TA_DataSelectColsProg)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("join",
    tabMisc::root->GetTemplateInstance(&TA_DataJoinProg)));
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("calc",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("+dest row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcAddDestRow)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("=dest row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcSetDestRow)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("=src row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcSetSrcRow)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("cpy cols",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcCopyCommonCols)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("script",
    tabMisc::root->GetTemplateInstance(&TA_UserScript)));

  ////////////////////////////////////////////////////////////////////////////
  //		Data generation and analysis
  sec = tb->AssertSection("Data Misc"); //note: need to keep it short
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("data gen()",
    tabMisc::root->GetTemplateInstance(&TA_DataGenCall)));
  tb->AddClipToolWidget(sec, new iBaseClipToolWidget("data anal()",
    tabMisc::root->GetTemplateInstance(&TA_DataAnalCall)));

}

ToolBoxRegistrar ptb(ProgramToolBoxProc);


//////////////////////////
//    iProgramToolBar 	//
//////////////////////////

IDataViewWidget* ProgramToolBar::ConstrWidget_impl(QWidget* gui_parent) {
  return new iProgramToolBar(this, gui_parent); // usually parented later
}


void iProgramToolBar::Constr_post() {
//  iMainWindowViewer* win = viewerWindow(); //cache

//TODO: add the appropriate global actions
}


//////////////////////////////////
//	iProgramCtrl		//
//////////////////////////////////

iProgramCtrlDataHost::iProgramCtrlDataHost(Program* prog, bool read_only_,
					   bool modal_, QObject* parent)
: taiEditDataHost(prog, prog->GetTypeDef(), read_only_, modal_, parent)
{
  membs.SetMinSize(MS_CNT); // note: our own MS_CNT
  membs.def_size = 0; // we handle everything
  //  use_show = false;
  refs.setOwner(this); // these update frequently
  refs_struct.setOwner(this); // these are same for prog lifetime
  if (prog) { // better have a value!
    refs_struct.Add(prog);
    // note: we deftly solve the problem of reacting to new vars/args
    // by simply putting those lists on our ref list, which notifies us
    refs_struct.Add(&(prog->args));
    refs_struct.Add(&(prog->vars));
  }
  Program_Group* pg = GET_OWNER(prog, Program_Group);
  if (pg) { // better exist!
    refs_struct.Add(pg);
  }
}

iProgramCtrlDataHost::~iProgramCtrlDataHost() {
}

bool iProgramCtrlDataHost::ShowMember(MemberDef* md) const {
  return false;
}

void iProgramCtrlDataHost::Cancel_impl() {
  refs.setOwner(NULL);
  refs.Reset(); // release all the guys we are linked to
  refs_struct.setOwner(NULL);
  refs_struct.Reset(); // release all the guys we are linked to
  inherited::Cancel_impl();
}

void iProgramCtrlDataHost::Enum_Members() {
  // we show all sections
  for (int j = 0; j < MS_CNT; ++j) {
    show_set(j) = true;
  }
}


void iProgramCtrlDataHost::Constr_Data_Labels() {
  Program* prog = this->prog();
  refs.Reset();
  membs.ResetItems(); // all Meths and data
  if (!prog) return; // defensive
  String nm;
  String help_text;
  
  // Program guys (just a few key guys), no label
  {
    MemberSpace& ms = prog->GetTypeDef()->members;
    for (int i = 0; i < ms.size; ++i) {
      MemberDef* md = ms.FastEl(i);
      if (md->im == NULL) continue; // this puppy won't show nohow!set_grp
      // just have a fixed list of guys we show
      if ((md->name == "name") || (md->name == "desc")) {
        memb_el(MS_PROG).Add(md);
      } 
    } 
    int idx = 0;
    // we can just use the default worker bee routine
    Constr_Data_Labels_impl(idx, &memb_el(MS_PROG), &data_el(MS_PROG));
  }
  
  iLabel* lbl = new iLabel("Items from Program_Group", body);
  AddSectionLabel(-1, lbl,
    "useful items from the Program_Group to which this Program belongs");
  
  // ProgGroup guys
  if(!(prog->HasProgFlag(Program::NO_STOP) || prog->HasProgFlag(Program::NO_USER_RUN))) {
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    MemberDef* md = TA_Program_Group.members.FindName("step_prog");
    if(pg && md) {
      memb_el(MS_GP).Add(md);
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      data_el(MS_GP).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      int row = AddData(-1, data);
      nm = "step_prog";
      help_text = md->desc;
      AddName(row, nm, help_text, mb_dat); 
    }
  }
  // args and vars
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS:
      nm = "Program args";
      help_text = "the arguments to the program";
      pvl = &prog->args; 
      lbl = new iLabel(nm, body);
      AddSectionLabel(-1, lbl,help_text);
      break;
    case MS_VARS: {
      nm = "Program vars";
      help_text = "the variables used inside the program";
      pvl = &prog->vars; 
      lbl = new iLabel(nm, body);
      AddSectionLabel(-1, lbl, help_text);
      } break;
    default: continue; // shouldn't happen!
    }
    
    
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(pv->HasVarFlag(ProgVar::NO_CTRL_PANEL)) continue;
      MemberDef* md = pv->GetValMemberDef();
      memb_el(j).Add(md);
      taiData* mb_dat;
      if(pv->var_type == ProgVar::T_HardEnum) {
        mb_dat = new taiComboBox(true, NULL, this, NULL, body);
      }
      else if(pv->var_type == ProgVar::T_DynEnum) {
        mb_dat = new taiComboBox(true, NULL, this, NULL, body);
      }
      else {
        mb_dat = md->im->GetDataRep(this, NULL, body);
      }
      data_el(j).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      int row = AddData(-1, data);
      nm = pv->name;
      help_text = pv->desc;
      AddName(row, nm, help_text, mb_dat); 
      refs.Add(pv);
    }
  } // j == set
}

void iProgramCtrlDataHost::DataDestroying_Ref(taBase_RefList* ref, taBase* base) {
  // we need to rebuild...
  if (ref == &refs)
    ReShow_Async();
  // otherwise, pgp or prog are destroying, so don't bother
}

void iProgramCtrlDataHost::DataChanged_Ref(taBase_RefList* ref, taBase* base,
    int dcr, void* op1, void* op2) 
{
  Program* prog = this->prog(); //cache
  if (!prog) return;
  // ignore list delete msgs, since the obj itself should notify
  if (ref == &refs_struct) {
    if ((base == &(prog->args)) ||(base == &(prog->vars))) {
      if ((dcr <= DCR_LIST_INIT) ||  (dcr == DCR_LIST_ITEM_REMOVE) ||
        (dcr > DCR_LIST_SORTED)
      ) return;
    }
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    // for step, only interested in group-as-object item update
    if (base == pg) {
      if ((dcr > DCR_ITEM_UPDATED_ND))
        return;
    }
  } 
  
  //note: don't need to check for is_loading because we defer until after
  // we need to do a fullblown reshow, to handle things like name changes of vars, etc.
  ReShow_Async();
}


void iProgramCtrlDataHost::GetValue_Membs_def() {
  Program* prog = this->prog(); //cache
  if (!prog) return;
  
  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetValue_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }
  
  // group stuff
  if (show_set(MS_GP) && (data_el(MS_GP).size > 0)) {
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    GetValue_impl(&memb_el(MS_GP), data_el(MS_GP), pg);
  }
  
  bool first_diff = true;
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }
    
    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(pv->HasVarFlag(ProgVar::NO_CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
#ifdef DEBUG
        taMisc::Warning("iProgramCtrlDataHost:GetValue_impl: ran out of controls!");
#endif
        break;
      }
      if(pv->var_type == ProgVar::T_HardEnum) {
        ((taiComboBox*)mb_dat)->GetEnumValue(pv->int_val); // todo: not supporting first_diff
      }
      else if(pv->var_type == ProgVar::T_DynEnum) { // todo: not supporting first_diff
        ((taiComboBox*)mb_dat)->GetEnumValue(pv->dyn_enum_val.value_idx);
      }
      else {
        md->im->GetMbrValue(mb_dat, (void*)pv, first_diff);
      }
      if(!first_diff) {		// always reset!
        taiMember::EndScript((void*)pv);
        first_diff = true;
      }
    }
  }
}

void iProgramCtrlDataHost::UpdateDynEnumCombo(taiComboBox* cb, const ProgVar* var) {
  cb->Clear();
  const DynEnum& de = var->dyn_enum_val; // convenience
  for (int i = 0; i < de.size; ++i) {
    const DynEnumItem* dei = de.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cb->AddItem(dei->name, i); //TODO: desc in status bar or such would be nice!
  }
}

void iProgramCtrlDataHost::GetImage_Membs()
{
  Program* prog = this->prog(); //cache
  if (!prog) return;
  
  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetImage_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }
  
  // group stuff
  if (show_set(MS_GP) && (data_el(MS_GP).size > 0)) {
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    GetImage_impl(&memb_el(MS_GP), data_el(MS_GP), pg);
  }
  
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    if (!show_set(j)) continue;
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: pvl = &prog->args; break;
    case MS_VARS: pvl = &prog->vars; break;
    default: continue; // shouldn't happen!
    }
    
    int cnt = 0;
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(pv->HasVarFlag(ProgVar::NO_CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
#ifdef DEBUG
        taMisc::Warning("iProgramCtrlDataHost:GetImage_impl: ran out of controls!");
#endif
        break;
      }
      if(pv->var_type == ProgVar::T_HardEnum) {
        ((taiComboBox*)mb_dat)->SetEnumType(pv->hard_enum_type);
        ((taiComboBox*)mb_dat)->GetEnumImage(pv->int_val);
      }
      else if(pv->var_type == ProgVar::T_DynEnum) {
        UpdateDynEnumCombo(((taiComboBox*)mb_dat), pv);
        int dei = pv->dyn_enum_val.value_idx;
        if (dei < 0) dei = 0;
        ((taiComboBox*)mb_dat)->GetEnumImage(dei);
      }
      else {
        md->im->GetImage(mb_dat, (void*)pv);
      }
    }
  }
}

//////////////////////////
//   iProgramCtrlPanel 	//
//////////////////////////

iProgramCtrlPanel::iProgramCtrlPanel(taiDataLink* dl_)
:inherited(dl_)
{
  Program* prog_ = prog();
  pc = NULL;
  if (prog_) {
    pc = new iProgramCtrlDataHost(prog_);
    const iColor* bgcol = NULL;
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bgcol = prog_->GetEditColorInherit();
    }
    pc->ConstrEditControl(bgcol);
    setCentralWidget(pc->widget()); //sets parent
    setButtonsWidget(pc->widButtons);
  }
}

iProgramCtrlPanel::~iProgramCtrlPanel() {
  if (pc) {
    delete pc;
    pc = NULL;
  }
}

void iProgramCtrlPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
  // not in this case!
}

bool iProgramCtrlPanel::HasChanged() {
  if (pc) return pc->HasChanged();
  else return false;
}

void iProgramCtrlPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iProgramCtrlPanel::Refresh_impl() {
  if (pc) pc->ReShow_Async();
}

void iProgramCtrlPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pc && pc->HasChanged()) {
    pc->Apply();
  }
}

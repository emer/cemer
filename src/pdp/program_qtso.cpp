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

#include "program_qtso.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "pdp_TA_inst.h"

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
    lbl = new QLabel("min type", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    thValType = new taiTypeDefButton(&TA_taBase, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(thValType->GetRep());  hl->addSpacing(taiM->hsep_c);
    lbl = new QLabel("value", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    tkObjectValue = new taiTokenPtrButton(thValType->typ, host, this, stack, 
      ((mflags & flgReadOnly) | flgNullOk));
    hl->addWidget(tkObjectValue->GetRep());  hl->addSpacing(taiM->hsep_c);
    
    AddChildWidget(stack);
    } break;
  case scEnum: {
    stack =  new QWidget();
    hl = new QHBoxLayout(stack);
    hl->setMargin(0);
    
    lbl = new QLabel("enum type", stack);
    hl->addWidget(lbl);  hl->addSpacing(taiM->hsep_c);
    thEnumType = new taiEnumTypeDefButton(&TA_taBase, host, this, stack, (mflags & flgReadOnly));
    hl->addWidget(thEnumType->GetRep()); hl->addSpacing(taiM->hsep_c);
    
    lbl = new QLabel("enum value", stack);
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
    
    lbl = new QLabel("enum value", stack);
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
    thValType->GetImage(var->object_type, &TA_taBase);
    tkObjectValue->GetImage(var->object_val, var->object_type, NULL);// no scope
    break;
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
  inherited::CreateDataPanel_impl(dl_);
  iProgramPanel* dp = new iProgramPanel(dl_);
  DataPanelCreated(dp);
}

//////////////////////////
//    iProgramEditor 	//
//////////////////////////

iProgramEditor::iProgramEditor(QWidget* parent)
:inherited(parent)
{
  init();
}

void iProgramEditor::init() {
  outer_host = NULL; // MUST be set at some point!!!
  edh = NULL;
  read_only = false;
  modified = false;
  bg_color.set(TAI_Program->GetEditColor()); // always the same
  base = NULL;
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2);
  
  layEdit = new QHBoxLayout(layOuter);
  layEdit->setMargin(0);
  layEdit->addSpacing(taiM->hsep_c);
  
  body = new iStripeWidget(this); //note: intrinsically sizes to 2 rows minimum
  layBody = NULL; // dynamically created
  layEdit->addWidget(body, 1); // give all the space to this guy
  layEdit->addSpacing(taiM->hspc_c);
  
  QVBoxLayout* layButtons = new QVBoxLayout();
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  layButtons->addStretch();
  btnSave = new HiLightButton("Save", this);
  layButtons->addWidget(btnSave);
  layButtons->addStretch();
  
  btnRevert = new HiLightButton("Revert", this);
  layButtons->addWidget(btnRevert);
  layButtons->addStretch();
  layEdit->addLayout(layButtons);
  
  items = new iTreeView(this, this);
  layOuter->addWidget(items, 1); // it gets the room
  items->setColumnCount(2);
  items->setSortingEnabled(false);// only 1 order possible
  items->setHeaderText(0, "Program Item");
  items->setHeaderText(1, "Item Description");
  items->setColKey(1, taBase::key_disp_name); //note: ProgVars and Els have nice disp_name desc's
  
  
  connect(btnSave, SIGNAL(clicked()), this, SLOT(btnSave_clicked()) );
  connect(btnRevert, SIGNAL(clicked()), this, SLOT(btnRevert_clicked()) );
  connect(items, SIGNAL(ItemSelected(iTreeViewItem*)),
    this, SLOT(items_ItemSelected(iTreeViewItem*)) );

  InternalSetModified(false);
}

void iProgramEditor::AddData(int row, QWidget* data) {
  // just get the height right from the strip widget
  layBody->setRowMinimumHeight(row, body->stripeHeight()); 
//  QHBoxLayout* hbl = new QHBoxLayout();
//  layBody->addLayout(hbl, row, 1);
//  hbl->addWidget(data, 0);
  layBody->addWidget(data, row, 0);
  data->show(); // needed for rebuilds, to make the widget show
}

void iProgramEditor::btnSave_clicked() {
//TODO
}

void iProgramEditor::btnRevert_clicked() {
//TODO
}

void iProgramEditor::DataLinkDestroying(taDataLink* dl) {
  setEditNode(NULL, false);
}
 
void iProgramEditor::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
//TODO: do we need to do anything???
}

void iProgramEditor::Changed() {
  InternalSetModified(true);
  outer_host->Changed();
}

TypeDef* iProgramEditor::GetBaseTypeDef() {
  if (base) return base->GetTypeDef();
  else return &TA_void; // avoids null issues
}

void iProgramEditor::GetValue() {
//TODO
}

void iProgramEditor::GetImage() {
/* TODO
  taiData* mb_dat = data_el.SafeEl(0);
  if (mb_dat) 
    typ->it->GetValue(mb_dat, base);*/
}

void iProgramEditor::Base_Remove() {
  base->RemoveDataClient(this);
  data_el.Reset(); // deletes the items
  memb_el.Reset();
  // delete widgets in iStripe
  const QObjectList& ch = body->children();
  while (ch.count() > 0) {
    QObject* obj = ch.last();
    delete obj;
  }
  base = NULL;
}

void iProgramEditor::Base_Add() {
  base->AddDataClient(this);
  layBody = new QGridLayout(body); //note: vmargin passed for "spacing", applies to both dims
  layBody->setSpacing(0);
  layBody->setMargin(0);
  // set row heights now, so only one row shows ok
  layBody->setRowMinimumHeight(0, body->stripeHeight()); 
  layBody->setRowMinimumHeight(1, body->stripeHeight()); 

  // add desc controls for ProgEls
//TEST:
  TypeDef* typ = GetBaseTypeDef();
  taiData* mb_dat = typ->it->GetDataRep(this, NULL, body, NULL, taiData::flgInline);
  data_el.Add(mb_dat);
  QWidget* rep = mb_dat->GetRep();
  AddData(0, rep);
  
  // ok, get er!
  GetValue();
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
  btnSave->setEnabled(value);
  btnRevert->setEnabled(value); 
  modified = value;
}

bool iProgramEditor::ItemRemoving(ISelectable* /*item*/) {
// TODO: maybe don't need to do anything??? itemChanged probably enough
  return false;
}

void iProgramEditor::items_ItemSelected(iTreeViewItem* item) {
  TAPtr new_base = NULL;
  if (item) {
    new_base = item->taData(); // NULL if not a taBase
  }
  setEditNode(new_base);
}

void iProgramEditor::setEditNode(TAPtr value, bool autosave) {
  if (base == value) return;
  if (base) {
    if (autosave) {
      //todo: check if dirty, and do a save
    }
    Base_Remove();
  }
  base = value;
  if (base) Base_Add();
}


//////////////////////////
//    iProgramPanel 	//
//////////////////////////

iProgramPanel::iProgramPanel(taiDataLink* dl_)
:inherited(dl_)
{
  pe = new iProgramEditor();
  setCentralWidget(pe); //sets parent
  Program* prog_ = prog();
  if (prog_) {
    taiDataLink* dl = (taiDataLink*)prog_->GetDataLink();
    if (dl) {
      dl->CreateTreeDataNode(NULL, pe->items, NULL, dl->GetName());
    }
  //TODO: might need to be put into a one-shot after showEvent
//    pe->ExpandAll();
  }
  
/*  list->setSelectionMode(QListView::Extended);
  list->setShowSortIndicator(true);
  // set up number of cols, based on link
  list->addColumn("#");
  for (int i = 0; i < link()->NumListCols(); ++i) {
    list->addColumn(link()->GetColHeading(i));
  }
  connect(list, SIGNAL(contextMenuRequested(QListViewItem*, const QPoint &, int)),
      this, SLOT(list_contextMenuRequested(QListViewItem*, const QPoint &, int)) );
  connect(list, SIGNAL(selectionChanged()),
      this, SLOT(list_selectionChanged()) );
  FillList(); */
}

iProgramPanel::~iProgramPanel() {
}

void iProgramPanel::AddedToPanelSet() {
  inherited::AddedToPanelSet();
  pe->outer_host = (IDataHost*)viewer_win(); //note: only valid now, not earlier
}

void iProgramPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
//TODO: maybe we should do something less crude???
//  idt->updateConfig();
}

int iProgramPanel::EditAction(int ea) {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->EditAction_(sel_list, ea);
  }
  return rval;
}

int iProgramPanel::GetEditActions() {
  int rval = 0;

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  ISelectable* ci = sel_list.SafeEl(0);
  if (ci)  {
    rval = ci->GetEditActions_(sel_list);
    // certain things disallowed if more than one item selected
    if (sel_list.size > 1) {
      rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
    }
  }
  return rval;
}

void iProgramPanel::GetSelectedItems(ISelectable_PtrList& lst) {
/*TODO  QListViewItemIterator it(list, QListViewItemIterator::Selected);
  while (it.current()) {
    lst.Add((taiListDataNode*)it.current());
    ++it;
  } */
}

/*void iProgramPanel::idt_contextMenuRequested(QListViewItem* item, const QPoint & pos, int col ) {
  //TODO: 'item' will be whatever is under the mouse, but we could have a multi select!!!
  taiListDataNode* nd = (taiListDataNode*)item;
  if (nd == NULL) return; //TODO: could possibly be multi select

  taiMenu* menu = new taiMenu(this, taiMenu::popupmenu, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)

  ISelectable_PtrList sel_list;
  GetSelectedItems(sel_list);
  nd->FillContextMenu(sel_list, menu); // also calls link menu filler

  //TODO: any for us last (ex. delete)
  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iProgramPanel::list_selectionChanged() {
  viewer_win()->UpdateUi();
}*/


String iProgramPanel::panel_type() const {
  static String str("Edit Program");
  return str;
}


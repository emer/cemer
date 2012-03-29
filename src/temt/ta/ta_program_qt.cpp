// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_program_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "ta_TA_inst.h"
#include "css_machine.h"

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QRadioButton>
#include <QTextEdit>

#include "icombobox.h"
#include "ieditgrid.h"
#include "ilineedit.h"
#include "iscrollarea.h"
#include "ispinbox.h"
#include "itreewidget.h"

////////////////////////
//  taiDynEnumType    //
////////////////////////

void taiDynEnumMember::Initialize() {
  isBit = false;
}

int taiDynEnumMember::BidForMember(MemberDef* md, TypeDef* td){
  TypeDef* mtd = md->type;
  if(td->InheritsFrom(&TA_DynEnum) && mtd->InheritsFrom(&TA_int) &&
     md->OptionAfter("DYNENUM_ON_").nonempty())
    return taiMember::BidForMember(md,td)+1;
  return 0;
}

taiData* taiDynEnumMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  flags_ |= taiData::flgAutoApply; // always auto-apply
  isBit = false;		// oops -- we don't have base and can't find out!
  taiDataDeck* rval = new taiDataDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiBitBox* bit_rep = new taiBitBox(typ, host_, rval, gui_parent_, flags_);
  taiField*  field_rep = new taiField(typ, host_, rval, gui_parent_, flags_); // read only
  taiComboBox* combo_rep = new taiComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->data_el.Add(field_rep);
  rval->AddChildWidget(field_rep->rep());
  rval->data_el.Add(combo_rep);
  rval->AddChildWidget(combo_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiDynEnumMember::UpdateDynEnumCombo(taiComboBox* cb, DynEnum& de) {
  cb->Clear();
  if(!de.enum_type) return;
  for (int i = 0; i < de.enum_type->enums.size; ++i) {
    const DynEnumItem* dei = de.enum_type->enums.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cb->AddItem(dei->name, i); //TODO: desc in status bar or such would be nice!
  }
  if(de.value < 0) de.value = 0; // un-init -- init!
  cb->GetImage(de.value);
}

void taiDynEnumMember::UpdateDynEnumBits(taiBitBox* cb, DynEnum& de) {
  cb->Clear();
  if(!de.enum_type) return;
  for (int i = 0; i < de.enum_type->enums.size; ++i) {
    const DynEnumItem* dei = de.enum_type->enums.FastEl(i);
    //note: dynenums store the index of the value, not the value
    cb->AddBoolItem(true, dei->name, dei->value, dei->desc, false);
  }
  cb->lay->addStretch();
  if(de.value < 0) de.value = 0; // un-init -- init!
  cb->GetImage(de.value);
}

void taiDynEnumMember::GetImage_impl(taiData* dat, const void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiDataDeck* rval = (taiDataDeck*)dat;
  if(!isBit && dye->enum_type && dye->enum_type->bits) {
    isBit = true;
  }
  if(isBit) {
    rval->GetImage(0);
    taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(0));
    if(!bit_rep) return;
    UpdateDynEnumBits(bit_rep, *dye);
  }
  else if (isReadOnly(dat)) {
    rval->GetImage(1);
    taiField* field_rep = dynamic_cast<taiField*>(rval->data_el.SafeEl(1));
    if(!field_rep) return;
    String str = dye->NameVal();
    field_rep->GetImage(str);
  }
  else {
    rval->GetImage(2);
    taiComboBox* combo_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(2));
    if(!combo_rep) return;
    UpdateDynEnumCombo(combo_rep, *dye);
  }
}

void taiDynEnumMember::GetMbrValue_impl(taiData* dat, void* base) {
  DynEnum* dye = (DynEnum*)base;
  taiDataDeck* rval = (taiDataDeck*)dat;
  if(!isReadOnly(dat)) {
    if(isBit) {
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(0));
      if(!bit_rep) return;
      bit_rep->GetValue(dye->value);
    }
    else {
      taiComboBox* combo_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(2));
      if(!combo_rep) return;
      combo_rep->GetValue(dye->value);
    }
  }
}

////////////////////////
//  taiProgVarIntValType    //
////////////////////////

void taiProgVarIntValMember::Initialize() {
}

int taiProgVarIntValMember::BidForMember(MemberDef* md, TypeDef* td){
  if(td->InheritsFrom(&TA_ProgVar) && (md->name == "int_val"))
    return taiMember::BidForMember(md,td)+10;
  return 0;
}

taiData* taiProgVarIntValMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  taiDataDeck* rval = new taiDataDeck(NULL, host_, par, gui_parent_, flags_);
  rval->InitLayout();
  gui_parent_ = rval->GetRep();
  taiIncrField*	int_rep = new taiIncrField(typ, host_, rval, gui_parent_, flags_);
  int_rep->setMinimum(INT_MIN);
  taiComboBox*	enum_rep = new taiComboBox(true, NULL, host_, rval, gui_parent_, flags_);
  taiBitBox* bit_rep = new taiBitBox(typ, host_, rval, gui_parent_, flags_);
  rval->data_el.Add(int_rep);
  rval->AddChildWidget(int_rep->rep());
  rval->data_el.Add(enum_rep);
  rval->AddChildWidget(enum_rep->rep());
  rval->data_el.Add(bit_rep);
  rval->AddChildWidget(bit_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiProgVarIntValMember::GetImage_impl(taiData* dat, const void* base) {
  ProgVar* pv = (ProgVar*)base;
  int val =  *((int*)mbr->GetOff(base));
  taiDataDeck* rval = (taiDataDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      rval->GetImage(2);
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->SetEnumType(pv->hard_enum_type);
      bit_rep->GetImage(val);
    }
    else {
      rval->GetImage(1);
      taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->SetEnumType(pv->hard_enum_type);
      enum_rep->GetEnumImage(val);
    }
  }
  else {
    rval->GetImage(0);
    taiIncrField* int_rep = dynamic_cast<taiIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    int_rep->GetImage(val);
  }
}

void taiProgVarIntValMember::GetMbrValue_impl(taiData* dat, void* base) {
  ProgVar* pv = (ProgVar*)base;
  int& val =  *((int*)mbr->GetOff(base));
  taiDataDeck* rval = (taiDataDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    if(pv->hard_enum_type->HasOption("BITS")) {
      taiBitBox* bit_rep = dynamic_cast<taiBitBox*>(rval->data_el.SafeEl(2));
      if (!bit_rep) return; // shouldn't happen
      bit_rep->GetValue(val);
    }
    else {
      int itm_no = -1;
      taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
      if (!enum_rep) return; // shouldn't happen
      enum_rep->GetEnumValue(val);
    }
  }
  else {
    taiIncrField* int_rep = dynamic_cast<taiIncrField*>(rval->data_el.SafeEl(0));
    if (!int_rep) return; // shouldn't happen
    val = int_rep->GetValue();
  }
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

  // then view script
  iProgramViewScriptPanel* vp = new iProgramViewScriptPanel(dl_);
  DataPanelCreated(vp);
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
  // layout constants
  ln_sz = taiM->max_control_height(taiM->ctrl_size); 
  ln_vmargin = 1; 
  m_editLines = taMisc::program_editor_lines;

  m_changing = 0;
  read_only = false;
  m_modified = false;
  warn_clobber = false;
  apply_req = false;
//  bg_color.set(TAI_Program->GetEditColor()); // always the same
  base = NULL;
  row = 0;
  m_show = (taMisc::ShowMembs)(taMisc::show_gui & taMisc::SHOW_CHECK_MASK);
  sel_item_mbr = NULL;
  sel_item_base = NULL;
  
  brow_hist = new iBrowseHistory(this);
  
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(2);
  layOuter->setSpacing(taiM->vsep_c);
  
  scrBody = new QScrollArea(this);
  scrBody->setWidgetResizable(true); 
  scrBody->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  scrBody->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//  scrBody->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  body = new iStripeWidget; 
  scrBody->setWidget(body);
//  body = new iStripeWidget(this);
  body->installEventFilter(this);

  line_ht = ln_sz + (2 * ln_vmargin);
  body->setStripeHeight(line_ht);
  int body_ht = line_ht * editLines();
  scrBody->setMinimumHeight(body_ht + scrBody->horizontalScrollBar()->height() + 2);
  layOuter->addWidget(scrBody); 
//  layOuter->addWidget(body); 

  meth_but_mgr = new iMethodButtonMgr(this); 
  
  defEditBgColor(); 
  
  layButtons = new QHBoxLayout();
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  tb = new QToolBar(this);
  int icon_sz = taiM_->label_height(taiMisc::sizSmall) -4;
  tb->setIconSize(QSize(icon_sz, icon_sz));
  layButtons->addWidget(tb);
  layButtons->addSpacing(4);

  btnHelp = new HiLightButton("&Help", this);
  layButtons->addWidget(btnHelp);
  connect(btnHelp, SIGNAL(clicked()), this, SLOT(Help()) );

  layButtons->addStretch();
  btnApply = new HiLightButton("&Apply", this);
  layButtons->addWidget(btnApply);
  layButtons->addSpacing(4);
  
  btnRevert = new HiLightButton("&Revert", this);
  layButtons->addWidget(btnRevert);
  layButtons->addSpacing(4);
  layOuter->addLayout(layButtons);
  
  items = new iTreeView(this, iTreeView::TV_AUTO_EXPAND);
  layOuter->addWidget(items, 1); // it gets the room

  items->setColumnCount(2);
  items->setSortingEnabled(false);// only 1 order possible
  items->setSelectionMode(QAbstractItemView::ExtendedSelection);
  items->setHeaderText(0, "Program Item");
  items->setColumnWidth(0, 220);
  items->setHeaderText(1, "Item Description");
  items->setColKey(0, taBase::key_disp_name); //note: ProgVars and Els have nice disp_name desc's
  items->setColFormat(0, iTreeView::CF_ELIDE_TO_FIRST_LINE);
  items->setColKey(1, taBase::key_desc); //note: ProgVars and Els have nice disp_name desc's
  items->setColFormat(1, iTreeView::CF_ELIDE_TO_FIRST_LINE);
  // adjunct data, tooltips, etc.
  items->AddColDataKey(0, taBase::key_disp_name, Qt::ToolTipRole); 
  items->AddColDataKey(1, taBase::key_desc, Qt::ToolTipRole);
  
  //enable dnd support
  items->setDragEnabled(true);
  items->setAcceptDrops(true);
  items->setDropIndicatorShown(true);
  items->setHighlightRows(true);

  connect(btnApply, SIGNAL(clicked()), this, SLOT(Apply()) );

  connect(btnApply, SIGNAL(clicked()), this, SLOT(Apply()) );
  connect(btnRevert, SIGNAL(clicked()), this, SLOT(Revert()) );
  items->Connect_SelectableHostNotifySignal(this,
    SLOT(items_Notify(ISelectableHost*, int)) );

  // browse history
  historyBackAction = new taiAction("Back", QKeySequence(), "historyBackAction" );
  historyBackAction->setParent(this); // for shortcut functionality, and to delete
  connect(historyBackAction, SIGNAL(triggered()), brow_hist, SLOT(back()) );
  connect(brow_hist, SIGNAL(back_enabled(bool)), 
    historyBackAction, SLOT(setEnabled(bool)) );
  historyForwardAction = new taiAction("Forward", QKeySequence(), "historyForwardAction" );
  historyForwardAction->setParent(this); // for shortcut functionality, and to delete
  connect(historyForwardAction, SIGNAL(triggered()), brow_hist, SLOT(forward()) );
  connect(brow_hist, SIGNAL(forward_enabled(bool)), 
    historyForwardAction, SLOT(setEnabled(bool)) );
  items->Connect_SelectableHostNotifySignal(brow_hist,
    SLOT(SelectableHostNotifying(ISelectableHost*, int)) );
  connect(brow_hist, SIGNAL(select_item(taiDataLink*)),
    this, SLOT(slot_AssertBrowserItem(taiDataLink*)) );
  // no history, just manually disable
  historyBackAction->setEnabled(false);
  historyForwardAction->setEnabled(false);
  // toolbar
  historyBackAction->addTo(tb);
  historyForwardAction->addTo(tb);

  //TEMP 
  QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(historyBackAction));
  if (but) but->setArrowType(Qt::LeftArrow);
  but = qobject_cast<QToolButton*>(tb->widgetForAction(historyForwardAction));
  if (but) but->setArrowType(Qt::RightArrow);
  
  InternalSetModified(false);
}

bool iProgramEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() != QEvent::KeyPress) {
    return QWidget::eventFilter(obj, event);
  }

  QKeyEvent* e = static_cast<QKeyEvent *>(event);
  if((bool)m_window) {
    if(m_window->KeyEventFilterWindowNav(obj, e))
      return true;
  }
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed && ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))) {
    Apply();			// do it!
    items->setFocus();	// return to items!
    return true;
  }
  if(e->key() == Qt::Key_Escape) {
    Revert();			// do it!
    items->setFocus();	// return to items!
    return true;
  }
  return QWidget::eventFilter(obj, event);
}

QWidget* iProgramEditor::firstTabFocusWidget() {
  return items;
}

bool iProgramEditor::ShowMember(MemberDef* md) {
  return taiPolyData::ShowMemberStat(md, show());
}

void iProgramEditor::Base_Add() {
  base->AddDataClient(this);
  // get colors for type
  bool ok;
  const iColor bgc = base->GetEditColorInherit(ok); 
  if (ok) setEditBgColor(bgc);
  else defEditBgColor();
  Controls_Add();
}

void iProgramEditor::Controls_Add() {
  // metrics for laying out
  int lines = editLines(); // amount of space, in lines, available
  row = 0;
  
  // do methods first, so we know whether to show, and thus how many memb lines we have
  // this is just a dry run..
  QWidget* tmp_body = new QWidget();
  QHBoxLayout* layMeths = new QHBoxLayout; // def margins ok
  meth_but_mgr->Constr(tmp_body, layMeths, base);
  if (meth_but_mgr->show_meth_buttons) {
    --lines;
  }
  meth_but_mgr->Reset(); // deletes items and widgets (buts/menus)
  delete layMeths;
  layMeths = NULL;
  delete tmp_body;
  tmp_body = NULL;
  
  TypeDef* typ = GetRootTypeDef();
  // check for a desc guy, it will consume another line
  MemberDef* md_desc = typ->members.FindName("desc");
  if (md_desc) --lines;
  
  // make main layout
  QVBoxLayout* lay = new QVBoxLayout(body);
  lay->setMargin(0);
  lay->setSpacing(0);
  
  // ok, we know how much room we have, so allocate and divide
  membs.SetMinSize(lines);
  // first, enumerate all non-desc members, to get a count
  int mbr_cnt = 0;
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (ShowMember(md)) ++mbr_cnt;
  }
  if (md_desc) --mbr_cnt; // don't double count
  
  // apportion the members amongst the available lines
  // round up slightly so first N lines get the extra odd ones
  { 
  int cur_ln = 0; // current line binning into
//  int n_per_ln = (mbr_cnt + (lines - 1)) / lines; // number per line, rounded up
  int n_per_ln = mbr_cnt / lines; // probably under by 1, at least for first lines
  int n_rem = mbr_cnt % lines; // we'll need to use these up
  if (n_rem > 0) ++n_per_ln; // we'll reduce it when n_rem used up
  int tmbr_cnt = mbr_cnt; // temp, this loop
  for (int i = 0, i_ln = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md)) continue;
    if (md->name == "desc") continue; // on separate line at end
    membs.FastEl(cur_ln)->memb_el.Add(md);
    ++i_ln;
    --tmbr_cnt;
    // if we somehow used up all lines, tough!
    if (cur_ln >= (lines - 1)) continue;
    
    if (i_ln >= n_per_ln || md->HasOption("PROGEDIT_NEWLN")) {
      i_ln = 0;
      ++cur_ln;
      // redo metrics -- if we ever NEWLN we'll need to pack more in
      n_per_ln = tmbr_cnt / (lines - cur_ln); // probably under by 1, at least for first lines
      n_rem = tmbr_cnt % (lines - cur_ln); // we'll need to use these up
      if (n_rem > 0) ++n_per_ln; // we'll reduce it when n_rem used up
    }
  }
  }
  // don't forget the desc guy
  if (md_desc) {
     membs.SetMinSize(lines + 1);
     membs.FastEl(lines)->memb_el.Add(md_desc);
  }
  
  // add main inline controls
  int flags = taiData::flgInline ;
  if (read_only) flags |= taiData::flgReadOnly;
  const int ctrl_size = taiM->ctrl_size;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    if (ms->memb_el.size == 0) continue; // actually, is end
    QHBoxLayout* hbl = new QHBoxLayout();
    //hbl->setMargin(ln_vmargin);
    hbl->addItem(new QSpacerItem(0, line_ht, QSizePolicy::Minimum, QSizePolicy::Fixed));
    hbl->setSpacing(0);
    hbl->addSpacing(taiM->hsep_c);
    // if only 1 guy, give it all to him, else share
    int stretch = (ms->memb_el.size > 1) ? 0 : 1;
    for (int i = 0; i < ms->memb_el.size; ++i) {
      if (i > 0)
        hbl->addSpacing(taiM->hspc_c);
      MemberDef* md = ms->memb_el.FastEl(i);
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body, NULL, flags);
      ms->data_el.Add(mb_dat);
      
//obs      QLabel* lbl = taiM->NewLabel(, body);
      String name;
      String desc;
      taiDataHost::GetName(md, name, desc);
      iLabel* lbl = taiDataHost::MakeInitEditLabel(name, body,
        ctrl_size,  desc, mb_dat,   
        this, SLOT(label_contextMenuInvoked(iLabel*, QContextMenuEvent*)), row);
      
      hbl->addWidget(lbl, 0,  (Qt::AlignLeft | Qt::AlignVCenter));
      hbl->addSpacing(taiM->hsep_c);
      lbl->show(); //n???
      
      QWidget* rep = mb_dat->GetRep();
      hbl->addWidget(rep, stretch, (Qt::AlignVCenter)); 
      rep->show();
    }
    //if (stretch == 0)
      hbl->addStretch(); // so guys flush left
    lay->addLayout(hbl); 
    ++row;
  }

  layMeths = new QHBoxLayout; // def margins ok
  layMeths->setMargin(0); 
  layMeths->addSpacing(2); //no stretch: we left-justify
  layMeths->addItem(new QSpacerItem(0, ln_sz + (2 * ln_vmargin), 
				    QSizePolicy::Fixed, QSizePolicy::Fixed));
  meth_but_mgr->Constr(body, layMeths, base);
  if (meth_but_mgr->show_meth_buttons) {
    layMeths->addStretch();
  } else {
    delete layMeths;
    layMeths = NULL;
  }
  
  if (meth_but_mgr->show_meth_buttons) {
    lay->addLayout(meth_but_mgr->lay());
    row++;
    //AddData(cur_line++, NULL, meth_but_mgr->lay());
  }
  lay->addStretch();

  // ok, get er!
  GetImage();
}

void iProgramEditor::Base_Remove() {
  first_tab_foc = NULL;
  items->focus_next_widget = NULL; // clear
  base->RemoveDataClient(this);
  base = NULL;
  Controls_Remove();
}

void iProgramEditor::Controls_Remove() {
  membs.Reset();
  meth_but_mgr->Reset(); // deletes items and widgets (buts/menus)
  body->clearLater();
//nn and dangerous!  taiMiscCore::RunPending(); // note: this is critical for the editgrid clear
}

void iProgramEditor::customEvent(QEvent* ev_) {
  // we return early if we don't accept, otherwise fall through to accept
  switch ((int)ev_->type()) {
  case CET_APPLY: {
    if (apply_req) {
      Apply();
      apply_req = false;
    }
  } break;
  default: inherited::customEvent(ev_); 
    return; // don't accept
  }
  ev_->accept();
}

void iProgramEditor::Apply() {
  if (warn_clobber) {
    int chs = taMisc::Choice("Warning: this object has changed since you started editing -- if you apply now, you will overwrite those changes -- what do you want to do?",
			     "Apply", "Revert", "Cancel");
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

void iProgramEditor::Apply_Async() {
  if (apply_req) return; // already waiting
  QEvent* ev = new QEvent((QEvent::Type)CET_APPLY);
  apply_req = true;
  QCoreApplication::postEvent(this, ev);
}

void iProgramEditor::Help() {
  if(base)
    base->Help();
}

iTreeViewItem* iProgramEditor::AssertBrowserItem(taiDataLink* link)
{
  // note: waitproc is insulated against recurrent calls..
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = items->AssertItem(link);
  if (rval) {
    items->setFocus();
    items->clearExtSelection();
    items->scrollTo(rval);
    items->setCurrentItem(rval, 0, QItemSelectionModel::ClearAndSelect);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return rval;
}

const iColor iProgramEditor::colorOfCurRow() const {
  if ((row % 2) == 0) {
    return bg_color;
  } else {
    return bg_color_dark;
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

void iProgramEditor::setEditLines(int val) {
  if (m_editLines == val) return;
  m_editLines = val;
  // write back to global, because user likely wants this value hence
  taMisc::program_editor_lines = val;
  Controls_Remove();
  int body_ht = line_ht * val;
  scrBody->setMinimumHeight(body_ht + scrBody->horizontalScrollBar()->height() + 2);
  Controls_Add();
}

TypeDef* iProgramEditor::GetRootTypeDef() const {
  if (base) return base->GetTypeDef();
  else return &TA_void; // avoids null issues
}

void iProgramEditor::GetValue() {
  TypeDef* typ = GetRootTypeDef();
  if (!typ) return; // shouldn't happen

  if(typ->InheritsFrom(TA_taBase) && base) {
    taProject* proj = (taProject*)((taBase*)base)->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(base, "Edit", base);
    }
  }

  ++m_changing;
  InternalSetModified(false); // do it first, so signals/updates etc. see us nonmodified
  bool first_diff = true;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      MemberDef* md = ms->memb_el.SafeEl(i);
      taiData* mb_dat = ms->data_el.FastEl(i);
      if (md && mb_dat) {
        md->im->GetMbrValue(mb_dat, base, first_diff);
      }
    }
  }
  if (typ->InheritsFrom(TA_taBase)) {
    base->UpdateAfterEdit();	// hook to update the contents after an edit..
//shouldn't be necessary    taiMisc::Update(base);
  }
  --m_changing;
}

void iProgramEditor::GetImage() {
  TypeDef* typ = GetRootTypeDef();
  if (!typ) return; // shouldn't happen
  meth_but_mgr->GetImage();
  ++m_changing;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      MemberDef* md = ms->memb_el.SafeEl(i);
      taiData* mb_dat = ms->data_el.FastEl(i);
      if (md && mb_dat) {
        md->im->GetImage(mb_dat, base);
      }
    }
  }

  // search through children to find first tab focus widget
  // skip over flags
  first_tab_foc = NULL;
  QList<QWidget*> list = qFindChildren<QWidget*>(body);
  for (int i=0; i<list.size(); ++i) {
    QWidget* rep = list.at(i);
//     cerr << i << "\t" << rep->metaObject()->className() << endl;
    if(rep->isVisible() && rep->isEnabled() && (rep->focusPolicy() & Qt::TabFocus) &&
       !rep->inherits("QCheckBox")) {
      if(rep->inherits("QLineEdit")) {
	QLineEdit* qle = (QLineEdit*)rep;
	if(qle->isReadOnly()) continue;
      }
      first_tab_foc = rep;
      break;
    }
  }

  items->focus_next_widget = first_tab_foc; // set linkage
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

void iProgramEditor::items_Notify(ISelectableHost* src, int op) {
  switch (op) {
  //case ISelectableHost::OP_GOT_FOCUS: return;
  case ISelectableHost::OP_SELECTION_CHANGED: {
    taBase* new_base = NULL;
    ISelectable* si = src->curItem();
    if (si && si->link()) {
      new_base = si->link()->taData(); // NULL if not a taBase, shouldn't happen
    }
    setEditNode(new_base);
  } break;
  //case ISelectableHost::OP_DESTROYING: return;
  default: return;
  }
}

void iProgramEditor::label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e) {
  QMenu* menu = new QMenu(this);
  //note: don't use body for menu parent, because some context menu choices cause ReShow, which deletes body items!
  Q_CHECK_PTR(menu);
  int last_id = -1;
  taiData* sel_item_dat = (taiData*)qvariant_cast<ta_intptr_t>(sender->userData()); // pray!!!
  if (sel_item_dat) {
    sel_item_mbr = sel_item_dat->mbr;
    sel_item_base = sel_item_dat->Base();
    taiDataHost::DoFillLabelContextMenu_SelEdit(menu, last_id, 
      sel_item_base, sel_item_mbr, body,
    this, SLOT(DoSelectForEdit(QAction*)));
  }

  if (menu->actions().count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void iProgramEditor::DoSelectForEdit(QAction* act) {
//note: this routine is duplicated in the taiEditDataHost
  
  taProject* proj = (taProject*)(base->GetThisOrOwner(&TA_taProject));
  if (!proj) return;
  
  int param = act->data().toInt();
  SelectEdit* se = proj->edits.Leaf(param);
 
  taBase* rbase = sel_item_base;
  MemberDef* md = sel_item_mbr;
  if (!md || !se || !rbase) return; //shouldn't happen...
  
  //NOTE: this handler adds if not on, or removes if already on
  int idx;
  if ((idx = se->FindMbrBase(rbase, md)) >= 0)
    se->RemoveField(idx);
  else {
    se->SelectMember(rbase, md);
  }
}

void iProgramEditor::Revert() {
  GetImage();
  InternalSetModified(false);
}

void iProgramEditor::Refresh() {
  // to refresh, we just simulate a remove/add
  //NOTE: for refresh, we just update the items (notify should always work for add/delete)
  items->Refresh();
  if (!m_modified) {
    taBase* tbase = base;
    if (tbase) {
      setEditNode(NULL);
      setEditNode(tbase);
    }
  }
}


void iProgramEditor::setEditBgColor(const iColor& value) {
  bg_color = value;
  taiDataHost::MakeDarkBgColor(bg_color, bg_color_dark);
  body->setColors(bg_color, bg_color_dark);
}

void iProgramEditor::defEditBgColor() {
  setEditBgColor(QApplication::palette().color(QPalette::Active, QPalette::Background));
}

void iProgramEditor::setEditNode(taBase* value, bool autosave) {
  if (base == value) return;
  if (base) {
    if (m_modified && autosave) {
      Apply();
    }
    Base_Remove(); // nulls base
  }
  if (value) {
    base = value;
    Base_Add();
  }
}

void iProgramEditor::setShow(int value) {
  value = (value & taMisc::SHOW_CHECK_MASK);
  if (m_show == value) return;
  m_show = value;
  Refresh();
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
//    iProgramViewScriptPanel	//
//////////////////////////

iProgramViewScriptPanel::iProgramViewScriptPanel(taiDataLink* dl_)
:inherited(dl_)
{
  Program* prg_ = prog();
  vs = NULL;
  if (prg_) {
    prg_->ViewScriptUpdate();
    vs = new NumberedTextView(NULL, true); // enable icons
    vs->textEdit()->setReadOnly(true);
    vs->textEdit()->setHtml(prg_->view_script);
    setCentralWidget(vs);

    NumberBar* nb = vs->numberBar();
    connect(nb, SIGNAL(lineFlagsUpdated(int, int)), this,
	    SLOT(lineFlagsUpdated(int, int)));
    connect(nb, SIGNAL(viewSource(int)), this,
	    SLOT(viewSource(int)));
    connect(vs, SIGNAL(mouseHover(const QPoint&, int, const QString&)), this,
	    SLOT(mouseHover(const QPoint&, int, const QString&)));
  }
}

iProgramViewScriptPanel::~iProgramViewScriptPanel() {
  // qt should take care of it
}

QWidget* iProgramViewScriptPanel::firstTabFocusWidget() {
  return vs;
}

bool iProgramViewScriptPanel::ignoreDataChanged() const {
  return false; // don't ignore -- we do SmartButComplicatedIgnore(TM)
//  return !isVisible();
}

void iProgramViewScriptPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if(vs && vs->isVisible())
    UpdatePanel_impl();
//   if (dcr <= DCR_ITEM_UPDATED_ND) {
//     this->m_update_req = true; // so we update next time we show, if hidden
//   }
//   inherited::DataChanged_impl(dcr, op1_, op2_);
}

bool iProgramViewScriptPanel::HasChanged() {
  return false;
}

void iProgramViewScriptPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iProgramViewScriptPanel::UpdatePanel_impl() {
  if(!vs) return;
  inherited::UpdatePanel_impl(); // clears reg flag and updates tab
  Program* prg_ = prog();
  if(!prg_) return;

  NumberBar* nb = vs->numberBar();
  nb->clearAllLineFlags();
  for(int i=1;i<prg_->script_list.size;i++) {
    ProgLine* pl = prg_->script_list.FastEl(i);
    int pflg = NumberBar::LF_NONE;
    if(pl->HasPLineFlag(ProgLine::BREAKPOINT))
      pflg |= NumberBar::LF_BREAK;
    if(pl->HasPLineFlag(ProgLine::PROG_ERROR))
      pflg |= NumberBar::LF_ERROR;
    if(pl->HasPLineFlag(ProgLine::WARNING))
      pflg |= NumberBar::LF_WARNING;
    nb->setLineFlags(i, pflg);
  }
  
  if(prg_->script) {
    nb->setCurrentLine(prg_->script->CurRunSrcLn());
  }

  prg_->ViewScriptUpdate();
  int contentsY = vs->textEdit()->verticalScrollBar()->value();
  vs->textEdit()->setHtml(prg_->view_script);
  vs->textEdit()->verticalScrollBar()->setValue(contentsY);
}

void iProgramViewScriptPanel::lineFlagsUpdated(int lineno, int flags) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl) return;

  // this is the only flag that is user-settable
  if(flags & NumberBar::LF_BREAK)
    pl->SetBreakpoint();
  else
    pl->ClearBreakpoint();

  // basic update
  prg_->ViewScriptUpdate();
  int contentsY = vs->textEdit()->verticalScrollBar()->value();
  vs->textEdit()->setHtml(prg_->view_script);
  vs->textEdit()->verticalScrollBar()->setValue(contentsY);
}

void iProgramViewScriptPanel::viewSource(int lineno) {
  Program* prg_ = prog();
  if(!prg_) return;
  ProgLine* pl = prg_->script_list.SafeEl(lineno);
  if(!pl || !pl->prog_el) return;
  tabMisc::DelayedFunCall_gui(pl->prog_el, "BrowserSelectMe");
}

void iProgramViewScriptPanel::mouseHover(const QPoint &pos, int lineno, const QString& word) {
  Program* prg_ = prog();
  if(!prg_) return;
  String rval = prg_->GetProgCodeInfo(lineno, word);
  if(rval.nonempty())
    QToolTip::showText(pos, rval);
}

void iProgramViewScriptPanel::ResolveChanges_impl(CancelOp& cancel_op) {
}

//////////////////////////
//   iProgramPanelBase 	//
//////////////////////////

iProgramPanelBase::iProgramPanelBase(taiDataLink* dl_)
:inherited(dl_)
{
  pe = new iProgramEditor();
  setCentralWidget(pe); //sets parent
  
  // add view button(s)
  QLabel* lab = new QLabel;
  lab->setMaximumHeight(taiM->label_height(taiMisc::sizSmall));
  lab->setFont(taiM->nameFont(taiMisc::sizSmall));
  lab->setText("lines");
  lab->setToolTip("how many lines to show in the item editor");
  AddMinibarWidget(lab);
  QSpinBox* sp = new QSpinBox;
  sp->setFont(taiM->buttonFont(taiMisc::sizSmall));
  sp->setValue(pe->editLines()); // assume this is min as well
  sp->setMinimum(pe->editLines()); // assume this is min as well
  sp->setMaximum(20);// arbitrary
  sp->setToolTip(lab->toolTip());
  AddMinibarWidget(sp);
  connect(sp, SIGNAL(valueChanged(int)), this, SLOT(mb_Lines(int)) );
  
  // add view button(s)
  QCheckBox* but = new QCheckBox;
  but->setMaximumHeight(taiM->button_height(taiMisc::sizSmall));
  but->setFont(taiM->buttonFont(taiMisc::sizSmall));
  but->setText("expert");
  but->setToolTip("whether to show items marked 'expert' in the program editor");
  but->setChecked(!(taMisc::show_gui & taMisc::NO_EXPERT));
  AddMinibarWidget(but);
  connect(but, SIGNAL(clicked(bool)), this, SLOT(mb_Expert(bool)) );
}

void iProgramPanelBase::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if (dcr == DCR_RESOLVE_NOW)
  {
    if (pe->HasChanged()) {
      pe->GetValue();
      return;
    }
  }
  else
    inherited::DataChanged_impl(dcr, op1_, op2_);
    //NOTE: don't need to do anything because DataModel will handle it
}

bool iProgramPanelBase::HasChanged_impl() {
  return pe->HasChanged();
}

void iProgramPanelBase::mb_Expert(bool checked) {
  int show = pe->show();
  if (checked) {
    show = show & ~taMisc::NO_EXPERT;
  } else {
    show = show | taMisc::NO_EXPERT;
  }
  pe->setShow(show);
}

void iProgramPanelBase::mb_Lines(int val) {
  pe->setEditLines(val);
}

void iProgramPanelBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the tree up to the panel
  pe->items->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  pe->m_window = itv->viewerWindow();
  pe->items->main_window = pe->m_window;

  pe->items->installEventFilter(pe->m_window);

  // make sure the Program toolbar is created
  MainWindowViewer* mvw = itv->viewerWindow()->viewer();
  mvw->FindToolBarByType(&TA_ProgramToolBar,"Program");
}

void iProgramPanelBase::UpdatePanel_impl() {
  pe->Refresh();
}

void iProgramPanelBase::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pe->HasChanged()) {
    pe->Apply();
  }
}

QWidget* iProgramPanelBase::firstTabFocusWidget() {
  return pe->firstTabFocusWidget();
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
  pe->items->setDefaultExpandLevels(12); // shouldn't generally be more than this
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
  if((level <= 1) && (typ->InheritsFrom(&TA_ProgVar_List) ||
		      typ->InheritsFrom(&TA_ProgType_List))) return; // only top guys: args, vars
  if(typ->DerivesFrom(&TA_ProgEl_List) || typ->DerivesFrom(&TA_ProgObjList) 
     || typ->DerivesFrom(&TA_Function_List))
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

static void ptbp_deco_widget(QWidget* widg, taBase* obj) {
  if(!widg) return;
  String dec_key = obj->GetTypeDecoKey(); // nil if none
  if(dec_key.nonempty()) {
    ViewColor* vc = taMisc::view_colors->FindName(dec_key);
    iColor colr;
    if(vc) {
      if(vc->use_fg)
	colr = vc->fg_color.color();
      else if(vc->use_bg)
	colr = vc->bg_color.color();
      QPalette pal;
//       pal.setColor(QPalette::Button, Qt::white);
      pal.setColor(QPalette::ButtonText, colr);
      widg->setPalette(pal);
//       widg->setAutoFillBackground(true);
    }
  }
}

static void ptbp_add_widget(iToolBoxDockViewer* tb, int sec, TypeDef* td) {
  ProgEl* obj = (ProgEl*)tabMisc::root->GetTemplateInstance(td);
  if(td == &TA_ForLoop) {
    ((ForLoop*)obj)->init.expr = "_toolbox_tmp_"; // flag for auto-updating of for loop var
  }
  iBaseClipWidgetAction* act = new iBaseClipWidgetAction(obj->GetToolbarName(), obj);
  QWidget* widg = tb->AddClipToolWidget(sec, act);
  ptbp_deco_widget(widg, obj);
}

void ProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Ctrl"); //note: need to keep it short

  ptbp_add_widget(tb, sec, &TA_ForLoop);
  ptbp_add_widget(tb, sec, &TA_DoLoop);
  ptbp_add_widget(tb, sec, &TA_WhileLoop);
  
  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_IfElse);
  ptbp_add_widget(tb, sec, &TA_IfContinue);
  ptbp_add_widget(tb, sec, &TA_IfBreak);
  ptbp_add_widget(tb, sec, &TA_IfReturn);
  ptbp_add_widget(tb, sec, &TA_IfGuiPrompt);
  ptbp_add_widget(tb, sec, &TA_Switch);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_CodeBlock);
  ptbp_add_widget(tb, sec, &TA_UserScript);
  
  ////////////////////////////////////////////////////////////////////////////
  //		Var/Fun
  sec = tb->AssertSection("Var/Fun");
  QWidget* widg = tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var",
		       tabMisc::root->GetTemplateInstance(&TA_ProgVar)));
  ptbp_deco_widget(widg, tabMisc::root->GetTemplateInstance(&TA_ProgVar));
  ptbp_add_widget(tb, sec, &TA_LocalVars);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_AssignExpr);
  ptbp_add_widget(tb, sec, &TA_VarIncr);
  ptbp_add_widget(tb, sec, &TA_MemberAssign);
  ptbp_add_widget(tb, sec, &TA_MethodCall);
  ptbp_add_widget(tb, sec, &TA_MemberMethodCall);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_Function);
  ptbp_add_widget(tb, sec, &TA_FunctionCall);
  ptbp_add_widget(tb, sec, &TA_ReturnExpr);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_ProgramCall);
  ptbp_add_widget(tb, sec, &TA_ProgramCallVar);
  ptbp_add_widget(tb, sec, &TA_OtherProgramVar);

  ////////////////////////////////////////////////////////////////////////////
  //		Print/Misc
  sec = tb->AssertSection("Print/Args..");
  ptbp_add_widget(tb, sec, &TA_PrintExpr);
  ptbp_add_widget(tb, sec, &TA_PrintVar);
  ptbp_add_widget(tb, sec, &TA_Comment);
  ptbp_add_widget(tb, sec, &TA_StopStepPoint);

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_ProgVarFmArg);
  ptbp_add_widget(tb, sec, &TA_MemberFmArg);
  ptbp_add_widget(tb, sec, &TA_DataColsFmArgs);
  ptbp_add_widget(tb, sec, &TA_SelectEditsFmArgs);
  ptbp_add_widget(tb, sec, &TA_RegisterArgs);

  ////////////////////////////////////////////////////////////////////////////
  //		Misc Fun
  sec = tb->AssertSection("Misc Fun");
  ptbp_add_widget(tb, sec, &TA_StaticMethodCall);
  ptbp_add_widget(tb, sec, &TA_MathCall);
  ptbp_add_widget(tb, sec, &TA_RandomCall);
  ptbp_add_widget(tb, sec, &TA_MiscCall);
  // add other spec meth calls here..

  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_DataProcCall);
  ptbp_add_widget(tb, sec, &TA_DataAnalCall);
  ptbp_add_widget(tb, sec, &TA_DataGenCall);
  ptbp_add_widget(tb, sec, &TA_ImageProcCall);

  ////////////////////////////////////////////////////////////////////////////
  //		Data processing
  sec = tb->AssertSection("Data"); //note: need to keep it short
  ptbp_add_widget(tb, sec, &TA_DataLoop);
  ptbp_add_widget(tb, sec, &TA_ResetDataRows);
  ptbp_add_widget(tb, sec, &TA_AddNewDataRow);
  ptbp_add_widget(tb, sec, &TA_DoneWritingDataRow);
  ptbp_add_widget(tb, sec, &TA_DataVarProg);
  ptbp_add_widget(tb, sec, &TA_DataVarProgMatrix);

  ////////////////////////////////////////////////////////////////////////////
  //		Data processing
  sec = tb->AssertSection("Data Proc"); //note: need to keep it short
  ptbp_add_widget(tb, sec, &TA_DataSortProg);
  ptbp_add_widget(tb, sec, &TA_DataGroupProg);
  ptbp_add_widget(tb, sec, &TA_DataSelectRowsProg);
  ptbp_add_widget(tb, sec, &TA_DataSelectColsProg);
  ptbp_add_widget(tb, sec, &TA_DataJoinProg);
  tb->AddSeparator(sec);
  ptbp_add_widget(tb, sec, &TA_DataCalcLoop);
  ptbp_add_widget(tb, sec, &TA_DataCalcAddDestRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcSetDestRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcSetSrcRow);
  ptbp_add_widget(tb, sec, &TA_DataCalcCopyCommonCols);
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
  sel_edit_mbrs = true; 
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
  dat_cnt = 0; 
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
      if ((md->name == "name") || (md->name == "desc") || (md->name == "short_nm")
	  || (md->name == "flags")) {
        memb_el(MS_PROG).Add(md);
      } 
    } 
    int idx = 0;
    // we can just use the default worker bee routine
    Constr_Data_Labels_impl(idx, &memb_el(MS_PROG), &data_el(MS_PROG));
  }
  
//   MembSet* ms = membs.SafeEl(MS_GP); 
//   ms->text = "Items from Program_Group";
//   ms->desc = "useful items from the Program_Group to which this Program belongs";
//   iLabel* lbl = new iLabel(ms->text.chars(), body);
//   AddSectionLabel(-1, lbl,ms->desc.chars());

  MembSet* ms = NULL;
  iLabel* lbl = NULL;
  // args and vars
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    ms = membs.SafeEl(j);
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS: {
      ms->text = "Program args";
      ms->desc = "the arguments to the program";
      pvl = &prog->args; 
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl,ms->desc.chars());
      break;
    }
    case MS_VARS: {
      ms->text = "Program vars";
      ms->desc = "the variables used inside the program";
      pvl = &prog->vars; 
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl, ms->desc.chars());
      } break;
    default: continue; // shouldn't happen!
    }
    
    
    for (int i = 0; i < pvl->size; ++i) {
      ProgVar* pv = pvl->FastEl(i);
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = pv->GetValMemberDef();
      memb_el(j).Add(md);
      taiData* mb_dat;
      int flags_ = 0;
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY))
	flags_ |= taiData::flgReadOnly;
      if((pv->var_type == ProgVar::T_HardEnum) || (pv->var_type == ProgVar::T_DynEnum)) {
	if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
	  mb_dat = new taiField(NULL, this, NULL, body, flags_);
	}
	else if((pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type &&
		 pv->hard_enum_type->HasOption("BITS")) ||
		(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits)) {
	  mb_dat = new taiBitBox(NULL, this, NULL, body, flags_ | taiData::flgAutoApply);
	}
	else {
	  mb_dat = new taiComboBox(true, NULL, this, NULL, body, flags_ | taiData::flgAutoApply);
	}
      }
      else if (pv->var_type == ProgVar::T_Int) {
        taiIncrField* int_rep = new taiIncrField(NULL, this, NULL, body, flags_);
        int_rep->setMinimum(INT_MIN);
        mb_dat = int_rep;
      }
      else {
        mb_dat = md->im->GetDataRep(this, NULL, body, NULL, flags_);
      }
      // we need to manually set the md into the dat...
      // need to check for enums, because md is the type, not the val
      if (pv->var_type == ProgVar::T_HardEnum) 
        md = pv->FindMember("int_val");
      else if (pv->var_type == ProgVar::T_DynEnum) {
        // special case -- we will be setting the base to the DynEnum, not pv
        // and herein need to set the md for the nested dyn_val, which 
        // conceivably may not even exist, so we do this via the instance
        md = TAI_DynEnum->FindMember("value");
      }
      mb_dat->SetMemberDef(md); // usually done by im, but we are manual here...
      
      data_el(j).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);
      nm = pv->name;
      help_text = pv->desc;
      AddNameData(-1, nm, help_text, data, mb_dat/*, md*/); 
      refs.Add(pv);
      ++dat_cnt;
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
  if (ignoreDataChanged()) return; // not visible, so ignore!
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
  if(!apply_req)		// already doing delayed apply
    ReShow_Async();
}

MemberDef* iProgramCtrlDataHost::GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc)
{
  return NULL;
/*
  MemberDef* md = NULL;
  if (!(membs.GetFlatDataItem(sel_idx, &md) && md))
    return NULL;
  if (base) *base = cur_base;
  String tlbl = ((taBase*)cur_base)->GetName().elidedTo(16);
  lbl = tlbl;
  return md;*/
}


void iProgramCtrlDataHost::GetValue_Membs_def() {
  Program* prog = this->prog(); //cache
  if (!prog) return;
  
  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetValue_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }
  
  // group stuff
//   if (show_set(MS_GP) && (data_el(MS_GP).size > 0)) {
//     Program_Group* pg = GET_OWNER(prog, Program_Group);
//     GetValue_impl(&memb_el(MS_GP), data_el(MS_GP), pg);
//   }
  
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
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL))
	continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      //note: code below is "risky" ex if visiblity update ctrl changes etc.
      // then the type values can be wrong -- so we strongly cast
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) continue; // do this after the cnt++!
      if (!md || !mb_dat) {
        taMisc::DebugInfo("iProgramCtrlDataHost:GetValue_impl: ran out of controls!");
        break;
      }
      if(pv->var_type == ProgVar::T_HardEnum) {
	if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
	  taiBitBox* tmb_dat = dynamic_cast<taiBitBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiBitBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetValue(pv->int_val);
	}
	else {
	  taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
	  //note: use of pv for tests is just a hook, pv not really germane
	  if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetEnumValue(pv->int_val); // todo: not supporting first_diff
	}
      }
      else if(pv->var_type == ProgVar::T_DynEnum) { // todo: not supporting first_diff
	if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
	  taiBitBox* tmb_dat = dynamic_cast<taiBitBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiBitBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetValue(pv->dyn_enum_val.value);
	}
	else {
	  taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetValue(pv->dyn_enum_val.value);
	}
      }
      else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
        taiIncrField* tmb_dat = dynamic_cast<taiIncrField*>(mb_dat);
        if (pv->TestError(!tmb_dat, "expected taiIncrField, not: ", 
          mb_dat->metaObject()->className())) continue;
        pv->int_val = tmb_dat->GetValue();
      }
      else {
        md->im->GetMbrValue(mb_dat, (void*)pv, first_diff);
      }
      pv->UpdateAfterEdit();
      if(!first_diff) {		// always reset!
        taiMember::EndScript((void*)pv);
        first_diff = true;
      }
    }
  }
}

void iProgramCtrlDataHost::GetImage_Membs()
{
  Program* prog = this->prog(); //cache
  if (!prog) return;
  
// #ifdef DEBUG
//   cerr << "ctrl panel get image on: " << prog->name << endl;
// #endif

  // prog stuff
  if (show_set(MS_PROG) && (data_el(MS_PROG).size > 0)) {
    GetImage_impl(&memb_el(MS_PROG), data_el(MS_PROG), prog);
  }
  
  // group stuff
//   if (show_set(MS_GP) && (data_el(MS_GP).size > 0)) {
//     Program_Group* pg = GET_OWNER(prog, Program_Group);
//     GetImage_impl(&memb_el(MS_GP), data_el(MS_GP), pg);
//   }
  
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
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
        taMisc::DebugInfo("iProgramCtrlDataHost:GetImage_impl: ran out of controls!");
        break;
      }
      // set base, for ctxt menu, so it won't try to use the Program (which is not the base)
      mb_dat->SetBase(pv); // for all, except HardEnum which is nested again
      if(pv->var_type == ProgVar::T_HardEnum) {
	if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
	  taiField* tmb_dat = dynamic_cast<taiField*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiField, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetImage(pv->GenCssInitVal());
	}
	else if(pv->hard_enum_type && pv->hard_enum_type->HasOption("BITS")) {
	  taiBitBox* tmb_dat = dynamic_cast<taiBitBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiBitBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->SetEnumType(pv->hard_enum_type);
	  tmb_dat->GetImage(pv->int_val);
	}
	else {
	  taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->SetEnumType(pv->hard_enum_type);
	  tmb_dat->GetEnumImage(pv->int_val);
	}
      }
      else if(pv->var_type == ProgVar::T_DynEnum) {
	mb_dat->SetBase(&pv->dyn_enum_val);
	if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) {
	  taiField* tmb_dat = dynamic_cast<taiField*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiField, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  tmb_dat->GetImage(pv->GenCssInitVal());
	}
	else if(pv->dyn_enum_val.enum_type && pv->dyn_enum_val.enum_type->bits) {
	  taiBitBox* tmb_dat = dynamic_cast<taiBitBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiBitBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  taiDynEnumMember::UpdateDynEnumBits(tmb_dat, pv->dyn_enum_val);
	}
	else {
	  taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  taiDynEnumMember::UpdateDynEnumCombo(tmb_dat, pv->dyn_enum_val);
	}
      }
      else if(pv->var_type == ProgVar::T_Int) { // todo: not supporting first_diff
        taiIncrField* tmb_dat = dynamic_cast<taiIncrField*>(mb_dat);
        if (pv->TestError(!tmb_dat, "expected taiIncrField, not: ", 
          mb_dat->metaObject()->className())) continue;
        tmb_dat->GetImage(pv->int_val);
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
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = prog_->GetEditColorInherit(ok);
      if (ok) pc->setBgColor(bgcol);
    }
    pc->ConstrEditControl();
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

bool iProgramCtrlPanel::HasChanged_impl() {
  if (pc) return pc->HasChanged();
  else return false;
}

void iProgramCtrlPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
}

void iProgramCtrlPanel::UpdatePanel_impl() {
// #ifdef DEBUG
//   cerr << "update panel on: " << prog()->name << endl;
// #endif
  if (pc) pc->ReShow_Async();
}

void iProgramCtrlPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (pc && pc->HasChanged()) {
    pc->Apply();
  }
}

///////////////////////////////////////////////////////////////////////
// 	Program specific browser guys!

iProgramPanel* Program::FindMyProgramPanel() {
  if(!taMisc::gui_active) return NULL;
  taDataLink* link = data_link();
  if(!link) return NULL;
  taDataLinkItr itr;
  iProgramPanel* el;
  FOR_DLC_EL_OF_TYPE(iProgramPanel, el, link, itr) {
    if (el->prog() == this) {
      // bad: this is what causes all the movement -- can't select program b/c that will do that
      // in the program tree browser -- need to direct this to the other guy somehow and
      // prevent program itself from selecting in prog editor!
      BrowserSelectMe();	// select my program
      iDataPanelSet* dps = el->data_panel_set();
      if(dps) {
	dps->setCurrentPanelId(1); // this is the editor
      }
      return el;
    }
  }
  return NULL;
}

bool Program::BrowserSelectMe_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;
  taiDataLink* link = (taiDataLink*)itm->GetDataLink();
  if(!link) return false;

  iProgramPanel* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserSelectMe();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->setFocus();
    itv->clearExtSelection();
    itv->scrollTo(iti);
    itv->setCurrentItem(iti, 0, QItemSelectionModel::ClearAndSelect);
    // make sure our operations are finished
    taiMiscCore::ProcessEvents();
    // tab into ProgCode but not other ProgEls, and into all other items
    if(itm->InheritsFrom(&TA_ProgCode) || !itm->InheritsFrom(&TA_ProgEl))
      QCoreApplication::postEvent(itv, new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier));
  }
  return (bool)iti;
}

bool Program::BrowserExpandAll_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;
  taiDataLink* link = (taiDataLink*)itm->GetDataLink();
  if(!link) return false;

  iProgramPanel* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserExpandAll();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->ExpandAllUnder(iti);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return (bool)iti;
}

bool Program::BrowserCollapseAll_ProgItem(taOBase* itm) {
  if(!taMisc::gui_active) return false;
  taiDataLink* link = (taiDataLink*)itm->GetDataLink();
  if(!link) return false;

  iProgramPanel* mwv = FindMyProgramPanel();
  if(!mwv || !mwv->pe) return itm->taBase::BrowserCollapseAll();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->CollapseAllUnder(iti);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
  return (bool)iti;
}

iDataPanelSet* Program::FindMyDataPanelSet() {
  if(!taMisc::gui_active) return NULL;
  taDataLink* link = data_link();
  if(!link) return NULL;
  taDataLinkItr itr;
  iDataPanelSet* el;
  FOR_DLC_EL_OF_TYPE(iDataPanelSet, el, link, itr) {
//     if (el->data() == this) {
      return el;
//     }
  }
  return NULL;
}

bool Program::ViewCtrlPanel() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(0);
  return true;
}

bool Program::ViewProgEditor() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(1);
  return true;
}

bool Program::ViewCssScript() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  return true;
}

bool Program::ViewProperties() {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(3);
  return true;
}

bool Program::ViewScriptEl(taBase* pel) {
  iDataPanelSet* dps = FindMyDataPanelSet();
  if(!dps) return false;
  dps->setCurrentPanelId(2);
  iProgramViewScriptPanel* pnl = dynamic_cast<iProgramViewScriptPanel*>(dps->panels.SafeEl(2));
  if(!pnl || !pnl->vs) return false;
  int start_ln, end_ln;
  if(!ScriptLinesEl(pel, start_ln, end_ln))
    return false;
  
  pnl->vs->setHighlightLines(start_ln, (end_ln - start_ln)+1); 
  return true;
}

////////////////////////////////////////////////////////
//  Special ProgLib browser support


taiProgLibElsButton::taiProgLibElsButton(TypeDef* typ_, IDataHost* host, taiData* par,
					 QWidget* gui_parent_, int flags_)
 :inherited(typ_, host, par, gui_parent_, flags_)
{
  // nop
}

int taiProgLibElsButton::columnCount(int view) const {
  switch (view) {
  case 0: return 6;
  default: return 0; // not supposed to happen
  }
}

const String taiProgLibElsButton::headerText(int index, int view) const {
  switch (view) {
  case 0: switch (index) {
    case 0: return "Name"; 
    case 1: return "Type"; 
    case 2: return "Tags"; 
    case 3: return "Description"; 
    case 4: return "Date Modified"; 
    case 5: return "URL/filename"; 
    } break; 
  default: break; // compiler food
  }
  return _nilString; // shouldn't happen
}

const String taiProgLibElsButton::titleText() {
  return "Please choose a program from the following in the available libraries";
}

void taiProgLibElsButton::BuildCategories_impl() {
  if (cats) cats->Reset();
  else cats = new String_Array;

  if(!list) return;		// shouldn't happen

  ProgLib* plib = (ProgLib*)list;
  for(int i=0;i<plib->size;i++) {
    ProgLibEl* pel = plib->FastEl(i);
    for(int j=0;j<pel->tags_array.size;j++) {
      cats->AddUnique(pel->tags_array[j]);
    }
  }
  cats->Sort(); // empty, if any, should sort to top
}

int taiProgLibElsButton::BuildChooser_0(taiItemChooser* ic, taList_impl* top_lst, 
					QTreeWidgetItem* top_item) 
{
  int rval = 0;

  ic->multi_cats = true;	// multiple categories
  
  ProgLib* plib = (ProgLib*)top_lst;
  for (int i = 0; i < plib->size; ++i) {
    ProgLibEl* pel = plib->FastEl(i);
    QTreeWidgetItem* item = ic->AddItem(pel->tags, pel->GetDisplayName(),
					top_item, pel); 
    item->setText(1, pel->lib_name); // GetColText(taBase::key_type));
    item->setText(2, pel->tags);
    item->setText(3, pel->desc); // GetColText(taBase::key_desc));
    item->setText(4, pel->date);
    if(pel->URL.nonempty())
      item->setText(5, pel->URL);
    else
      item->setText(5, pel->filename);
    ++rval;
  }
  return rval;
}

//////////////////////////////////////
//        taiProgLibElArgType       //
//////////////////////////////////////

int taiProgLibElArgType::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if ((argt->ptr != 1) || !argt->DerivesFrom(TA_ProgLibEl))
    return 0;
  return gpiFromGpArgType::BidForArgType(aidx,argt,md,td)+1;
}

taiData* taiProgLibElArgType::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_)
{
  MemberDef* from_md = GetFromMd();
  if(from_md == NULL)	return NULL;
  int new_flags = flags_;
  if (GetHasOption("NULL_OK"))
    new_flags |= taiData::flgNullOk;
  if (GetHasOption("EDIT_OK"))
    new_flags |= taiData::flgEditOk;

  if (GetHasOption("NO_GROUP_OPT"))
    new_flags |= taiData::flgNoGroup; //aka flagNoList

  return new taiProgLibElsButton(typ, host_, par, gui_parent_, new_flags);
}

void taiProgLibElArgType::GetImage_impl(taiData* dat, const void* base) {
  if (arg_base == NULL)  return;
  if (GetHasOption("ARG_VAL_FM_FUN")) {
    Variant val = ((taBase*)base)->GetGuiArgVal(meth->name, arg_idx);
    if(val != _nilVariant) {
      taBase::SetPointer((taBase**)arg_base, val.toBase());
    }
  }
  MemberDef* from_md = GetFromMd();
  if (from_md == NULL)	return;
  taList_impl* lst = GetList(from_md, base);
  taiProgLibElsButton* els = (taiProgLibElsButton*)dat;
  els->GetImage((taList_impl*)lst, *((taBase**)arg_base));
}

void taiProgLibElArgType::GetValue_impl(taiData* dat, void*) {
  if (arg_base == NULL)
    return;
  taiProgLibElsButton* els = (taiProgLibElsButton*)dat;
  // must use set pointer because cssTA_Base now does refcounts on pointer!
  taBase::SetPointer((taBase**)arg_base, (taBase*)els->GetValue());
}


//////////////////////////////////////
//        StringFieldLookupFun      //
//////////////////////////////////////

static ProgExprBase* expr_lookup_cur_base = NULL;

bool ProgExprBase::ExprLookupVarFilter(void* base_, void* var_) {
  if(!base_) return true;
  Program* prog = dynamic_cast<Program*>(static_cast<taBase*>(base_));
  if(!prog) return true;
  ProgVar* var = dynamic_cast<ProgVar*>(static_cast<taBase*>(var_));
  if(!var || !var->HasVarFlag(ProgVar::LOCAL_VAR)) return true; // definitely all globals
  Function* varfun = GET_OWNER(var, Function);
  if(!varfun) return true;	// not within a function, always go -- can't really tell scoping very well at this level -- could actually do it but it would be recursive and hairy
  if(!expr_lookup_cur_base) return true; // no filter possible
  Function* basefun = GET_OWNER(expr_lookup_cur_base, Function);
  if(basefun != varfun) return false; // different function scope
  return true;
}


String ProgExprBase::ExprLookupFun(const String& cur_txt, int cur_pos, int& new_pos,
				   taBase*& path_own_obj, TypeDef*& path_own_typ,
				   MemberDef*& path_md, ProgExprBase* expr_base,
				   Program* own_prg, Function* own_fun,
				   taBase* path_base, TypeDef* path_base_typ) {
  path_own_obj = NULL;
  path_own_typ = NULL;
  path_md = NULL;

  String txt = cur_txt.before(cur_pos);
  String extra_txt = cur_txt.from(cur_pos);
  String append_at_end;
  String prepend_before;

  String base_path;		// path to base element(s) if present
  String lookup_seed;		// start of text to seed lookup process
  String rval = _nilString;

  int lookup_type = -1; // 1 = var name (no path, delim), 2 = obj memb/meth,
  // 3 = type scoped, 4 = array index
  
  int_Array delim_pos;
  int delims_used = 0;
  int expr_start = 0;
  int c = '\0';
  for(int i=cur_pos-1;i>= 0; i--) {
    c = txt[i];
    if(isalpha(c) || isdigit(c) || (c == '_')) continue;
    if(c == ']' || c == '[' || c == '.' || c == '>' || c == '-' || c == ':') {
      delim_pos.Add(i);
      continue;
    }
    expr_start = i+1;		// anything else is a bust
    break;
  }

  int xtra_st = extra_txt.length();
  for(int i=0;i<extra_txt.length(); i++) {
    c = extra_txt[i];
    if(isalpha(c) || isdigit(c) || (c == '_')) continue;
    xtra_st = i;
    break;
  }
  if(xtra_st < extra_txt.length()) {
    append_at_end = extra_txt.from(xtra_st);
  }

  if(delim_pos.size > 0) {
    if(txt[delim_pos[0]] == '.') { // path sep = .
      base_path = txt.at(expr_start, delim_pos[0]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 2;
      delims_used = 1;
    }
    else if(txt[delim_pos[0]] == '>' && delim_pos.size > 1 && txt[delim_pos[1]] == '-'
	    && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ->
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 2;
      delims_used = 2;
    }
    else if(txt[delim_pos[0]] == ':' && delim_pos.size > 1 && txt[delim_pos[1]] == ':'
	    && (delim_pos[0] == delim_pos[1] + 1)) { // path sep = ::
      base_path = txt.at(expr_start, delim_pos[1]-expr_start);
      prepend_before = txt.before(expr_start);
      lookup_seed = txt.after(delim_pos[0]);
      lookup_type = 3;
      delims_used = 2;
    }
    // todo: []
  }
  else {
    if(path_base || path_base_typ) {
      lookup_type = 2;
    }
    else {
      lookup_type = 1;
    }
    lookup_seed = txt.from(expr_start);
    prepend_before = txt.before(expr_start);
  }

  String path_prepend_before;	// for path operations
  if(delim_pos.size > 0) {
    path_prepend_before = txt.through(delim_pos[0]);
  }

  switch(lookup_type) {
  case 1: {// lookup variables
//     cerr << "base_path empty: lookup a var, seed: " << lookup_seed << endl;
    taiTokenPtrMultiTypeButton* varlkup =  new taiTokenPtrMultiTypeButton
      (&TA_ProgVar, NULL, NULL,	NULL, 0, lookup_seed);
    varlkup->setNewObj1(&(own_prg->vars), " New Global Var");
    if(expr_base) {
      ProgEl* pel = GET_OWNER(expr_base, ProgEl);
      if(pel) {
	LocalVars* pvs = pel->FindLocalVarList();
	if(pvs) {
	  varlkup->setNewObj2(&(pvs->local_vars), " New Local Var");
	}
      }
    }
    varlkup->item_filter = (item_filter_fun)ProgExprBase::ExprLookupVarFilter;
    expr_lookup_cur_base = expr_base;
    varlkup->type_list.Link(&TA_ProgVar);
    varlkup->type_list.Link(&TA_DynEnumItem);
    varlkup->GetImageScoped(NULL, &TA_ProgVar, own_prg, &TA_Program);
    bool okc = varlkup->OpenChooser();
    if(okc && varlkup->token()) {
      rval = prepend_before + varlkup->token()->GetName();
      new_pos = rval.length();
      rval += append_at_end;
    }
    delete varlkup;
    expr_lookup_cur_base = NULL;
    break;
  }
  case 2: {			// members/methods
//     cerr << "lookup a memb/meth from path: " << base_path << " seed: " << lookup_seed << endl;
    String path_var, path_rest;
    TypeDef* lookup_td = NULL;
    taList_impl* tal = NULL;
    taBase* base_base = NULL;
    TypeDef* own_td = NULL;
    if(path_base) {
      base_base = path_base;
      path_rest = base_path;
      own_td = path_base_typ;
    }
    else if(path_base_typ) {
      own_td = path_base_typ;
      path_rest = base_path;
    }
    else {
      if(delim_pos.size > delims_used) {
	// note: any ref to base path needs to subtract expr_start relative to delim_pos!
	path_var = base_path.before(delim_pos.SafeEl(-1)-expr_start); // use last one = first in list
	if(delim_pos.size > delims_used+1 && delim_pos.SafeEl(-2) == delim_pos.SafeEl(-1)+1)
	  path_rest = base_path.after(delim_pos.SafeEl(-2)-expr_start);
	else
	  path_rest = base_path.after(delim_pos.SafeEl(-1)-expr_start);
      }
      else {
	path_var = base_path;
      }
      ProgVar* st_var = NULL;
      if(own_fun)
	st_var = own_fun->FindVarName(path_var);
      if(!st_var)
	st_var = own_prg->FindVarName(path_var);
      if(st_var) {
	if(st_var->var_type == ProgVar::T_Object) {
	  if(!st_var->object_type) {
	    taMisc::Info("Var lookup: cannot lookup anything about variable:", path_var,
			 "because it is an Object* but has no type set yet!");
	  }
	  else {
	    own_td = st_var->object_type;
	    if(path_rest.empty()) {
	      lookup_td = st_var->object_type;
	    }
	    else {
	      base_base = st_var->object_val;
	    }
	  }
	}
	else if(st_var->var_type == ProgVar::T_String) {
	  lookup_td = &TA_taString;
	}
      }
      else {
	taMisc::Info("Var lookup: cannot find variable:", path_var,
		     "as start of lookup path:", base_path);
      }
    }
    if(base_base && !lookup_td) {
      MemberDef* md = NULL;
      taBase* mb_tab = base_base->FindFromPath(path_rest, md);
      if(mb_tab) {
	lookup_td = mb_tab->GetTypeDef();
	if(lookup_td->InheritsFrom(&TA_taList_impl))
	  tal = (taList_impl*)mb_tab;
      }
      else {
	if(md) lookup_td = md->type;
      }
    }
    if(!lookup_td && own_td) {
      int net_base_off=0;
      ta_memb_ptr net_mbr_off=0;
      MemberDef* md = TypeDef::FindMemberPathStatic(own_td, net_base_off, 
						    net_mbr_off, path_rest, false);
      // no warn
      if(md) lookup_td = md->type;
    }
    if(!lookup_td) {
      taMisc::Info("Var lookup: cannot find path:", path_rest, "in variable:",
		   path_var);
    }
    if(tal) {
      if(tal->InheritsFrom(&TA_taGroup_impl)) {
	taiGroupElsButton* lilkup = new taiGroupElsButton(lookup_td, NULL, NULL, NULL,
							  0, lookup_seed);
	lilkup->GetImage((taGroup_impl*)tal, NULL);
	bool okc = lilkup->OpenChooser();
	if(okc && lilkup->item()) {
	  path_own_obj = lilkup->item();
	  path_own_typ = path_own_obj->GetTypeDef();
	  rval = path_prepend_before + path_own_obj->GetName();
	  new_pos = rval.length();
	  rval += append_at_end;
	}
	delete lilkup;
      }
      else {
	taiListElsButton* lilkup = new taiListElsButton(lookup_td, NULL, NULL, NULL,
							0, lookup_seed);
	lilkup->GetImage(tal, NULL);
	bool okc = lilkup->OpenChooser();
	if(okc && lilkup->item()) {
	  path_own_obj = lilkup->item();
	  path_own_typ = path_own_obj->GetTypeDef();
	  rval = path_prepend_before + path_own_obj->GetName();
	  new_pos = rval.length();
	  rval += append_at_end;
	}
	delete lilkup;
      }
    }
    else if(lookup_td) {
//       cerr << "lookup from type: " << lookup_td->name << endl;
      TypeItem* lookup_md = NULL;
      if(path_base || path_base_typ) {		// can only lookup members, not methods
	taiMemberDefButton* mdlkup = new taiMemberDefButton(lookup_td, NULL, NULL,
							    NULL, 0, lookup_seed);
	mdlkup->GetImage((MemberDef*)NULL, lookup_td);
	bool okc = mdlkup->OpenChooser();
	if(okc && mdlkup->md()) {
	  lookup_md = mdlkup->md();
	}
	delete mdlkup;
      }
      else {
	taiMemberMethodDefButton* mdlkup =  new taiMemberMethodDefButton(lookup_td, NULL, NULL,
									 NULL, 0, lookup_seed);
	mdlkup->GetImage((MemberDef*)NULL, lookup_td);
	bool okc = mdlkup->OpenChooser();
	if(okc && mdlkup->md()) {
	  lookup_md = mdlkup->md();
	}
	delete mdlkup;
      }
      if(lookup_md) {
	rval = path_prepend_before + lookup_md->name;
	if(lookup_md->typeInfoKind() == taMisc::TIK_METHOD)
	  rval += "()";
	new_pos = rval.length();
	rval += append_at_end;
	path_own_typ = lookup_td;
	if(lookup_md->typeInfoKind() == taMisc::TIK_MEMBER) {
	  path_md = (MemberDef*)lookup_md;
	}
      }
    }
    break;
  }
  case 3: {
//     cerr << "lookup a type/static guy from path: " << base_path << " seed: "
// 	 << lookup_seed << endl;
    TypeDef* lookup_td = taMisc::types.FindName(base_path);
    if(lookup_td) {
//       cerr << "lookup from type: " << lookup_td->name << endl;
      taiEnumStaticButton* eslkup =  new taiEnumStaticButton(lookup_td, NULL, NULL,
							     NULL, 0, lookup_seed);
      eslkup->GetImage((MemberDef*)NULL, lookup_td);
      bool okc = eslkup->OpenChooser();
      if(okc && eslkup->md()) {
	rval = path_prepend_before + eslkup->md()->name;
	if(eslkup->md()->typeInfoKind() == taMisc::TIK_METHOD)
	  rval += "()";
	new_pos = rval.length();
	rval += append_at_end;
      }
      delete eslkup;
    }
    else {			// now try for local enums
      ProgType* pt = own_prg->types.FindName(base_path);
      if(pt && pt->InheritsFrom(&TA_DynEnumType)) {
	taiTokenPtrButton* varlkup =  new taiTokenPtrButton(&TA_DynEnumItem, NULL, NULL,
							    NULL, 0, lookup_seed);
	varlkup->GetImageScoped(NULL, &TA_DynEnumItem, pt, &TA_DynEnumType); // scope to this guy
	bool okc = varlkup->OpenChooser();
	if(okc && varlkup->token()) {
	  rval = prepend_before + varlkup->token()->GetName();
	  new_pos = rval.length();
	  rval += append_at_end;
	}
	delete varlkup;
      }
    }
    break;
  }
  case 4: {
    cerr << "lookup an array index from path: " << base_path << " seed: " << lookup_seed << endl;
    break;
  }
  }

  return rval;
}

String ProgExprBase::StringFieldLookupFun(const String& cur_txt, int cur_pos,
					  const String& mbr_name, int& new_pos) {

  Program* own_prg = GET_MY_OWNER(Program);
  if(!own_prg) return _nilString;
  Function* own_fun = GET_MY_OWNER(Function);
  taBase* path_own_obj = NULL;
  TypeDef* path_own_typ = NULL;
  MemberDef* path_md = NULL;
  return ProgExprBase::ExprLookupFun(cur_txt, cur_pos, new_pos,
				     path_own_obj, path_own_typ, path_md,
				     this, own_prg, own_fun);
}

String MemberProgEl::StringFieldLookupFun(const String& cur_txt, int cur_pos,
					  const String& mbr_name, int& new_pos) {

  String rval = _nilString;
  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return rval;
  }
  TypeDef* path_base_typ = obj->act_object_type();
  taBase* path_base = NULL;
  if((bool)obj->object_val) {
    path_base = obj->object_val;
  }

  Program* own_prg = GET_MY_OWNER(Program);
  if(!own_prg) return _nilString;
  Function* own_fun = GET_MY_OWNER(Function);
  taBase* path_own_obj = NULL;
  TypeDef* path_own_typ = NULL;
  MemberDef* path_md = NULL;
  rval = ProgExprBase::ExprLookupFun(cur_txt, cur_pos, new_pos,
				     path_own_obj, path_own_typ, path_md,
				     NULL, own_prg, own_fun, path_base, path_base_typ);

  if(path_own_typ) {
    obj_type = path_own_typ;
  }

  return rval;
}

/////////////////////////////
//   taiStepButtonMethod   //
/////////////////////////////

int taiStepButtonMethod::BidForMethod(MethodDef* md, TypeDef* td) {
  if (md->HasOption("STEP_BUTTON"))
    return (inherited::BidForMethod(md,td) + 1);
  return 0;
}

taiMethodData* taiStepButtonMethod::GetButtonMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiProgStepButton* rval = new taiProgStepButton(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}

taiMethodData* taiStepButtonMethod::GetMenuMethodRep_impl(void* base, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_) {
  taiMethMenu* rval = new taiMethMenu(base, meth, typ, host_, par, gui_parent_, flags_);
  return rval;
}


/////////////////////////////
// 	taiProgStepButton  //
/////////////////////////////

taiProgStepButton::taiProgStepButton(void* bs, MethodDef* md, TypeDef* typ_, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_)
: taiMethodData(bs, md, typ_, host_, par, gui_parent_, flags_)
{
  is_menu_item = false;
  tool_bar = NULL;
  new_step_n = -1;
  last_step_n = 1;
  step10_val = 10;
  n_step_progs = 0;
  last_step = NULL;		// reset when switching
}

void taiProgStepButton::CallFunList(void* itm) {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;

  Program* trg = (Program*)itm;

  QPointer<taiProgStepButton> ths = this; // to detect us being deleted
  ApplyBefore();
  // note: this is not a great situation, whereby applying deletes us, but
  // we warn user (and should probably do something, ie a directive that you 
  // have to save before)
  if (!ths) {
    taMisc::Error("This menu item or button action apparently cannot be invoked when you have not applied changes. Please try the operation again. (The developers would appreciate hearing about this situation.");
    return;
  }

  taProject* proj = (taProject*)prg->GetOwner(&TA_taProject);
  if(proj) {
    proj->undo_mgr.SaveUndo(prg, "Call Method: " + meth->name, NULL, true);
    // true = force project-level save
  }

  if(trg && trg->owner) {
    if(new_step_n > 0)
      trg->step_n = new_step_n;
    last_step_n = trg->step_n;
  }
  
  prg->Step_Gui(trg);	// that was simple!

  last_step = trg;
  new_step_n = -1;
}

void taiProgStepButton::Step1(bool on) {
  if(!on) {
    stp1->setChecked(true);	// can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = 1;
  stp5->setChecked(false);
  stp10->setChecked(false);
}

void taiProgStepButton::Step5(bool on) {
  if(!on) {
    stp5->setChecked(true);	// can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = 5;
  stp1->setChecked(false);
  stp10->setChecked(false);
}


void taiProgStepButton::Step10(bool on) {
  if(!on) {
    stp10->setChecked(true);	// can't click off!
    return;
  }
  Program* prg = (Program*)base; // definitively this
  if(!prg) return;
  new_step_n = step10_val;
  stp1->setChecked(false);
  stp5->setChecked(false);
}


QWidget* taiProgStepButton::GetButtonRep() {
  Program* prg = (Program*)base; // definitively this
  if(!prg) return buttonRep;

  // we are all up-to-date and this is expensive so just bail!!
  if(buttonRep && !prg->sub_progs_updtd && n_step_progs == prg->sub_progs_step.size)
    return buttonRep;

  if(!buttonRep) {
    buttonRep = new QStackedWidget(gui_parent);
    SetRep(buttonRep);
    if(prg->step_prog)
      last_step_n = prg->step_prog->step_n;
  }

  QToolBar* newbar = new QToolBar();
  newbar->setFont(taiM->menuFont(defSize()));
  QLabel* lbl = new QLabel("Step:");
  lbl->setToolTip("Run N Step(s) of a program -- select N (1,5,10) in adjacent buttons, and then click on the name of the program shown after N's to Step that given program (this Step label is not a button and does not do anything)");
  lbl->setFont(taiM->menuFont(defSize()));
  newbar->addWidget(lbl);

  QWidget* intstak = new QWidget();
  QGridLayout* glay = new QGridLayout(intstak);
  glay->setMargin(0); glay->setSpacing(0);

  stp1 = new QRadioButton(intstak);
  stp1->setToolTip("step by single (1) steps");
//   QSize sz = stp1->minimumSizeHint();
  // this size is useless!
//   taMisc::Info("radio sz:", String(sz.width()), String(sz.height()));
  int radio_width = 16;
  int radio_height = 16;
  stp1->setMaximumSize(radio_width, radio_height);
  connect(stp1, SIGNAL(clicked(bool)), this, SLOT(Step1(bool)) );
  glay->addWidget(stp1, 0, 0, Qt::AlignHCenter);

  stp5 = new QRadioButton(intstak);
  stp5->setToolTip("step by 5 steps per step click");
  stp5->setMaximumSize(radio_width, radio_height);
  connect(stp5, SIGNAL(clicked(bool)), this, SLOT(Step5(bool)) );
  glay->addWidget(stp5, 0, 1, Qt::AlignHCenter);

  stp10 = new QRadioButton(intstak);
  stp10->setToolTip("step by 10 steps per step click, or amount shown if different from 10 (if program step_n != {1,5,10}");
  stp10->setMaximumSize(radio_width, radio_height);
  connect(stp10, SIGNAL(clicked(bool)), this, SLOT(Step10(bool)) );
  glay->addWidget(stp10, 0, 2, Qt::AlignHCenter);

  step10_val = 10;
  int stp_n = last_step_n;
  if(new_step_n > 0)
    stp_n = new_step_n;

  if(stp_n == 1)
    stp1->setChecked(true);
  else if(stp_n == 5)
    stp5->setChecked(true);
  else if(stp_n == 10)
    stp10->setChecked(true);
  else {
    step10_val = stp_n;
    stp10->setChecked(true);
  }

  int steps[3] = {1,5,step10_val};

  for(int i=0;i<3;i++) {
    lbl = new QLabel(String(steps[i]));
    lbl->setFont(taiM->menuFont(taiMisc::sizSmall));
    glay->addWidget(lbl, 1, i, Qt::AlignHCenter);
  }

  newbar->addWidget(intstak);

  int but_marg_minus = 10;

  QWidget* stpwidg = new QWidget();
  QHBoxLayout* hbl = new QHBoxLayout(stpwidg);
  hbl->setMargin(0); hbl->setSpacing(0);
  for(int i=0;i<prg->sub_progs_step.size; i++) {
    Program* sp = (Program*)prg->sub_progs_step[i];
    QToolButton* tbut = new QToolButton(stpwidg);
    taiAction* act = new taiAction(taiActions::normal, sp->short_nm);
    act->usr_data = (void*)sp;
    act->connect(taiAction::ptr_act, this, SLOT(CallFunList(void*)));
    act->setToolTip(sp->name);
    act->setFont(taiM->menuFont(defSize()));
    tbut->setDefaultAction(act);
    if(i == prg->sub_progs_step.size-1) {
      tbut->setShortcut(QKeySequence("F10"));
    }
    QSize sz = tbut->minimumSizeHint();
    tbut->setMaximumWidth(sz.width() - but_marg_minus);
    hbl->addWidget(tbut);
  }
  newbar->addWidget(stpwidg);
 n_step_progs = prg->sub_progs_step.size;
  
  if(tool_bar) {
    ((QStackedWidget*)buttonRep.data())->removeWidget(tool_bar);
  }
  tool_bar = newbar;
  ((QStackedWidget*)buttonRep.data())->addWidget(tool_bar);
//   newbar->show();
  return buttonRep;
}

bool taiProgStepButton::UpdateButtonRep() {
  if(!base || !buttonRep) return false;
  GetButtonRep();
  taiMethodData::UpdateButtonRep();
  return true;
}

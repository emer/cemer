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

#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QTreeWidget>
#include <QVBoxLayout>

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
     (md->OptionAfter("DYNENUM_ON_") != ""))
    return taiMember::BidForMember(md,td)+1;
  return 0;
}

taiData* taiDynEnumMember::GetDataRep_impl(IDataHost* host_, taiData* par,
  QWidget* gui_parent_, int flags_, MemberDef* mbr_) {
  isBit = false;		// oops -- we don't have base and can't find out!
  if (isBit) {
    return new taiBitBox(true, typ, host_, par, gui_parent_, flags_);
  } else if (flags_ & taiData::flgReadOnly) {
    return new taiField(typ, host_, par, gui_parent_, flags_);
  } else {
    taiComboBox* rval = new taiComboBox(true, NULL, host_, par, gui_parent_, flags_);
    return rval;
  }
}

void taiDynEnumMember::GetImage_impl(taiData* dat, const void* base) {
  DynEnum* dye = (DynEnum*)base;
  if (isBit) {
    taiBitBox* rval = (taiBitBox*)dat;
    rval->GetImage(dye->value);
  } else if (isReadOnly(dat)) {
    taiField* rval = (taiField*)(dat);
    String str = dye->NameVal();
    rval->GetImage(str);
  } else {
    taiComboBox* rval = (taiComboBox*)dat;
    rval->Clear();
    if(dye->enum_type) {
      for (int i = 0; i < dye->enum_type->enums.size; ++i) {
	const DynEnumItem* dei = dye->enum_type->enums.FastEl(i);
	rval->AddItem(dei->name, i); //TODO: desc in status bar or such would be nice!
      }
    }
    int dei = dye->value;
    if (dei < 0) dei = 0;
    rval->GetImage(dei);
  }
}

void taiDynEnumMember::GetMbrValue_impl(taiData* dat, void* base) {
  DynEnum* dye = (DynEnum*)base;
  if (isBit) {
    taiBitBox* rval = (taiBitBox*)dat;
    rval->GetValue(dye->value);
  } else if (!isReadOnly(dat)) {
    taiComboBox* rval = (taiComboBox*)dat;
    rval->GetValue(dye->value);
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
  rval->data_el.Add(int_rep);
  rval->AddChildWidget(int_rep->rep());
  rval->data_el.Add(enum_rep);
  rval->AddChildWidget(enum_rep->rep());
  rval->EndLayout();
  return rval;
}

void taiProgVarIntValMember::GetImage_impl(taiData* dat, const void* base) {
  ProgVar* pv = (ProgVar*)base;
  int val =  *((int*)mbr->GetOff(base));
  taiDataDeck* rval = (taiDataDeck*)dat;

  if(pv->var_type == ProgVar::T_HardEnum && pv->hard_enum_type) {
    rval->GetImage(1);
    taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
    if (!enum_rep) return; // shouldn't happen
    enum_rep->SetEnumType(pv->hard_enum_type);
    EnumDef* td = pv->hard_enum_type->enum_vals.FindNo(val);
    if(td != NULL)
      enum_rep->GetImage(td->idx);
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
    int itm_no = -1;
    taiComboBox* enum_rep = dynamic_cast<taiComboBox*>(rval->data_el.SafeEl(1));
    if (!enum_rep) return; // shouldn't happen
    enum_rep->GetValue(itm_no);
    EnumDef* td = NULL;
    if ((itm_no >= 0) && (itm_no < pv->hard_enum_type->enum_vals.size))
      td = pv->hard_enum_type->enum_vals.FastEl(itm_no);
    if (td != NULL)
      val = td->enum_no;
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
  body = new iStripeWidget; 
  scrBody->setWidget(body);

  body->installEventFilter(this);

  line_ht = ln_sz + (2 * ln_vmargin);
  body->setStripeHeight(line_ht);
  int body_ht = line_ht * editLines();
  scrBody->setMinimumHeight(body_ht + scrBody->horizontalScrollBar()->height() + 2);
  layOuter->addWidget(scrBody); 

  meth_but_mgr = new iMethodButtonMgr(this); 
  
  defEditBgColor(); 
  
  layButtons = new QHBoxLayout();
  layButtons->setMargin(0);
  layButtons->setSpacing(0);
  tb = new QToolBar(this);
  int icon_sz = taiM_->label_height(taiMisc::sizSmall) -4;
  tb->setIconSize(QSize(icon_sz, icon_sz));
  layButtons->addWidget(tb);
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

bool iProgramEditor::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::KeyPress) {
    QKeyEvent* e = static_cast<QKeyEvent *>(event);
    bool ctrl_pressed = false;
    if(e->modifiers() & Qt::ControlModifier)
      ctrl_pressed = true;
#ifdef TA_OS_MAC
    // ctrl = meta on apple
    if(e->modifiers() & Qt::MetaModifier)
      ctrl_pressed = true;
#endif
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
  }
  return QWidget::eventFilter(obj, event);
}

void iProgramEditor::Apply_Async() {
  if (apply_req) return; // already waiting
  QEvent* ev = new QEvent((QEvent::Type)CET_APPLY);
  apply_req = true;
  QCoreApplication::postEvent(this, ev);
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
  // first, enumerate all members, to get a count
  int mbr_cnt = 0;
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (ShowMember(md)) ++mbr_cnt;
  }
  
  // apportion the members amongst the available lines
  // round up slightly so first N lines get the extra odd ones
  { 
  int cur_ln = 0; // current line binning into
  int n_per_ln = (mbr_cnt + (lines - 1)) / lines; // number per line, rounded up
  for (int i = 0, i_ln = 0; i < typ->members.size; ++i) {
    MemberDef* md = typ->members.FastEl(i);
    if (!ShowMember(md)) continue;
    if (md->name == "desc") continue; // on separate line at end
    membs.FastEl(cur_ln)->memb_el.Add(md);
    ++i_ln;
    if (i_ln >= n_per_ln || md->HasOption("PROGEDIT_NEWLN")) {
      i_ln = 0;
      ++cur_ln;
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
  membs.Reset();
  meth_but_mgr->Reset(); // deletes items and widgets (buts/menus)
  body->clearLater();
//nn and dangerous!  taiMiscCore::RunPending(); // note: this is critical for the editgrid clear
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

iTreeViewItem* iProgramEditor::AssertBrowserItem(taiDataLink* link)
{
  // note: waitproc is insulated against recurrent calls..
  taiMiscCore::ProcessEvents();
  iTreeViewItem* rval = items->AssertItem(link);
  if (rval) {
    items->scrollTo(rval);
    items->setCurrentItem(rval);
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

int iProgramEditor::editLines() const {
  return 4;
}

TypeDef* iProgramEditor::GetRootTypeDef() const {
  if (base) return base->GetTypeDef();
  else return &TA_void; // avoids null issues
}

void iProgramEditor::GetValue() {
  TypeDef* typ = GetRootTypeDef();
  if (!typ) return; // shouldn't happen
  
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
    TAPtr new_base = NULL;
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

void iProgramEditor::setEditNode(TAPtr value, bool autosave) {
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
//   iProgramPanelBase 	//
//////////////////////////

iProgramPanelBase::iProgramPanelBase(taiDataLink* dl_)
:inherited(dl_)
{
  pe = new iProgramEditor();
  setCentralWidget(pe); //sets parent
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

void iProgramPanelBase::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  // connect the tree up to the panel
  pe->items->Connect_SelectableHostNotifySignal(itv,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
  pe->m_window = itv->viewerWindow();
  pe->items->main_window = pe->m_window;
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

void ProgramToolBoxProc(iToolBoxDockViewer* tb) {
  int sec = tb->AssertSection("Ctrl"); //note: need to keep it short

  ForLoop* forguy = (ForLoop*)tabMisc::root->GetTemplateInstance(&TA_ForLoop);
  forguy->init.expr = "_toolbox_tmp_"; // flag for auto-updating of for loop var

  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("for",
    forguy));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("do",
    tabMisc::root->GetTemplateInstance(&TA_DoLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("while",
    tabMisc::root->GetTemplateInstance(&TA_WhileLoop)));
  
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if",
    tabMisc::root->GetTemplateInstance(&TA_If)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if.else",
    tabMisc::root->GetTemplateInstance(&TA_IfElse)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if.cont",
    tabMisc::root->GetTemplateInstance(&TA_IfContinue)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if.break",
    tabMisc::root->GetTemplateInstance(&TA_IfBreak)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if.return",
    tabMisc::root->GetTemplateInstance(&TA_IfReturn)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("if gui prmt",
    tabMisc::root->GetTemplateInstance(&TA_IfGuiPrompt)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("switch",
    tabMisc::root->GetTemplateInstance(&TA_Switch)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("block",
    tabMisc::root->GetTemplateInstance(&TA_CodeBlock)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("script",
    tabMisc::root->GetTemplateInstance(&TA_UserScript)));
  
  ////////////////////////////////////////////////////////////////////////////
  //		Var/Fun
  sec = tb->AssertSection("Var/Fun");
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var",
    tabMisc::root->GetTemplateInstance(&TA_ProgVar)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("loc vars",
    tabMisc::root->GetTemplateInstance(&TA_ProgVars)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var=",
    tabMisc::root->GetTemplateInstance(&TA_AssignExpr)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var+=",
    tabMisc::root->GetTemplateInstance(&TA_VarIncr)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("meth()",
    tabMisc::root->GetTemplateInstance(&TA_MethodCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("memb=",
    tabMisc::root->GetTemplateInstance(&TA_MemberAssign)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("memb()",
    tabMisc::root->GetTemplateInstance(&TA_MemberMethodCall)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("fun def",
    tabMisc::root->GetTemplateInstance(&TA_Function)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("fun()",
    tabMisc::root->GetTemplateInstance(&TA_FunctionCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("return",
    tabMisc::root->GetTemplateInstance(&TA_ReturnExpr)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("prog()",
    tabMisc::root->GetTemplateInstance(&TA_ProgramCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("oth prg var",
    tabMisc::root->GetTemplateInstance(&TA_OtherProgramVar)));

  ////////////////////////////////////////////////////////////////////////////
  //		Print/Misc
  sec = tb->AssertSection("Print/Args..");
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("print",
    tabMisc::root->GetTemplateInstance(&TA_PrintExpr)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("print var",
    tabMisc::root->GetTemplateInstance(&TA_PrintVar)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("comment",
    tabMisc::root->GetTemplateInstance(&TA_Comment)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("stop/step",
    tabMisc::root->GetTemplateInstance(&TA_StopStepPoint)));

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("var=arg",
    tabMisc::root->GetTemplateInstance(&TA_ProgVarFmArg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("memb=arg",
    tabMisc::root->GetTemplateInstance(&TA_MemberFmArg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data=args",
    tabMisc::root->GetTemplateInstance(&TA_DataColsFmArgs)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("reg args",
    tabMisc::root->GetTemplateInstance(&TA_RegisterArgs)));

  ////////////////////////////////////////////////////////////////////////////
  //		Misc Fun
  sec = tb->AssertSection("Misc Fun");
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("static()",
    tabMisc::root->GetTemplateInstance(&TA_StaticMethodCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("math()",
    tabMisc::root->GetTemplateInstance(&TA_MathCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("random()",
    tabMisc::root->GetTemplateInstance(&TA_RandomCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("misc()",
    tabMisc::root->GetTemplateInstance(&TA_MiscCall)));
  // add other spec meth calls here..

  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data proc()",
    tabMisc::root->GetTemplateInstance(&TA_DataProcCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data anal()",
    tabMisc::root->GetTemplateInstance(&TA_DataAnalCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data gen()",
    tabMisc::root->GetTemplateInstance(&TA_DataGenCall)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("img proc()",
    tabMisc::root->GetTemplateInstance(&TA_ImageProcCall)));

  ////////////////////////////////////////////////////////////////////////////
  //		Data processing
  sec = tb->AssertSection("Data"); //note: need to keep it short
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data loop",
    tabMisc::root->GetTemplateInstance(&TA_DataLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("reset rows",
    tabMisc::root->GetTemplateInstance(&TA_ResetDataRows)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("new row",
    tabMisc::root->GetTemplateInstance(&TA_AddNewDataRow)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("row done",
    tabMisc::root->GetTemplateInstance(&TA_DoneWritingDataRow)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("data vars",
    tabMisc::root->GetTemplateInstance(&TA_DataVarProg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("dat var mat",
    tabMisc::root->GetTemplateInstance(&TA_DataVarProgMatrix)));

  ////////////////////////////////////////////////////////////////////////////
  //		Data processing
  sec = tb->AssertSection("Data Proc"); //note: need to keep it short
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("sort",
    tabMisc::root->GetTemplateInstance(&TA_DataSortProg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("group",
    tabMisc::root->GetTemplateInstance(&TA_DataGroupProg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("sel rows",
    tabMisc::root->GetTemplateInstance(&TA_DataSelectRowsProg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("sel cols",
    tabMisc::root->GetTemplateInstance(&TA_DataSelectColsProg)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("join",
    tabMisc::root->GetTemplateInstance(&TA_DataJoinProg)));
  tb->AddSeparator(sec);
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("calc loop",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcLoop)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("+dest row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcAddDestRow)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("=dest row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcSetDestRow)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("=src row",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcSetSrcRow)));
  tb->AddClipToolWidget(sec, new iBaseClipWidgetAction("cpy cols",
    tabMisc::root->GetTemplateInstance(&TA_DataCalcCopyCommonCols)));
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
      if ((md->name == "name") || (md->name == "desc")) {
        memb_el(MS_PROG).Add(md);
      } 
    } 
    int idx = 0;
    // we can just use the default worker bee routine
    Constr_Data_Labels_impl(idx, &memb_el(MS_PROG), &data_el(MS_PROG));
  }
  
  MembSet* ms = membs.SafeEl(MS_GP); 
  ms->text = "Items from Program_Group";
  ms->desc = "useful items from the Program_Group to which this Program belongs";
  iLabel* lbl = new iLabel(ms->text.chars(), body);
  AddSectionLabel(-1, lbl,ms->desc.chars());
  
  // ProgGroup guys
  if(!(prog->HasProgFlag(Program::NO_STOP) || !prog->HasProgFlag(Program::SHOW_STEP))) {
    Program_Group* pg = GET_OWNER(prog, Program_Group);
    MemberDef* md = TA_Program_Group.members.FindName("step_prog");
    if(pg && md) {
      memb_el(MS_GP).Add(md);
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      data_el(MS_GP).Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);
      nm = "step_prog";
      help_text = md->desc;
      AddNameData(-1, nm, help_text, data, mb_dat/*, md*/); 
      ++dat_cnt;
    }
  }
  // args and vars
  for (int j = MS_ARGS; j <= MS_VARS; ++j) {
    ms = membs.SafeEl(j);
    ProgVar_List* pvl = NULL;
    switch (j) {
    case MS_ARGS:
      ms->text = "Program args";
      ms->desc = "the arguments to the program";
      pvl = &prog->args; 
      lbl = new iLabel(ms->text.chars(), body);
      AddSectionLabel(-1, lbl,ms->desc.chars());
      break;
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
      if ((pv->var_type == ProgVar::T_HardEnum) ||
        (pv->var_type == ProgVar::T_DynEnum)) 
      {
	if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY))
	  mb_dat = new taiField(NULL, this, NULL, body, flags_);
	else
	  mb_dat = new taiComboBox(true, NULL, this, NULL, body, flags_);
      } else if (pv->var_type == ProgVar::T_Int) {
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
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL))
	continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      //note: code below is "risky" ex if visiblity update ctrl changes etc.
      // then the type values can be wrong -- so we strongly cast
      if(pv->HasVarFlag(ProgVar::CTRL_READ_ONLY)) continue; // do this after the cnt++!
      if (!md || !mb_dat) {
#ifdef DEBUG
        taMisc::Warning("iProgramCtrlDataHost:GetValue_impl: ran out of controls!");
#endif
        break;
      }
      if(pv->var_type == ProgVar::T_HardEnum) {
        taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
        //note: use of pv for tests is just a hook, pv not really germane
        if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
          mb_dat->metaObject()->className())) continue;
        tmb_dat->GetEnumValue(pv->int_val); // todo: not supporting first_diff
      }
      else if(pv->var_type == ProgVar::T_DynEnum) { // todo: not supporting first_diff
        taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
        if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
          mb_dat->metaObject()->className())) continue;
        tmb_dat->GetValue(pv->dyn_enum_val.value);
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
  if(!de.enum_type) return;
  for (int i = 0; i < de.enum_type->enums.size; ++i) {
    const DynEnumItem* dei = de.enum_type->enums.FastEl(i);
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
      if(!pv->HasVarFlag(ProgVar::CTRL_PANEL)) continue;
      MemberDef* md = memb_el(j).SafeEl(cnt);
      taiData* mb_dat = data_el(j).SafeEl(cnt++);
      if (!md || !mb_dat) {
#ifdef DEBUG
        taMisc::Warning("iProgramCtrlDataHost:GetImage_impl: ran out of controls!");
#endif
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
	else {
	  taiComboBox* tmb_dat = dynamic_cast<taiComboBox*>(mb_dat);
	  if (pv->TestError(!tmb_dat, "expected taiComboBox, not: ", 
			    mb_dat->metaObject()->className())) continue;
	  UpdateDynEnumCombo((tmb_dat), pv);
	  int dei = pv->dyn_enum_val.value;
	  if (dei < 0) dei = 0;
	  tmb_dat->GetImage(dei);
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
  if(!mwv || !mwv->pe) return itm->taBase::BrowserExpandAll();

  iTreeView* itv = mwv->pe->items;
  iTreeViewItem* iti = itv->AssertItem(link);
  if(iti) {
    itv->scrollTo(iti);
    itv->setCurrentItem(iti);
  }
  // make sure our operations are finished
  taiMiscCore::ProcessEvents();
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
  TABLPtr lst = GetList(from_md, base);
  taiProgLibElsButton* els = (taiProgLibElsButton*)dat;
  els->GetImage((TABLPtr)lst, *((TAPtr*)arg_base));
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

String ProgExprBase::StringFieldLookupFun(const String& cur_txt, int cur_pos,
					  const String& mbr_name, int& new_pos) {

  Program* prg = GET_MY_OWNER(Program);
  if(!prg) return _nilString;

  Function* fun = GET_MY_OWNER(Function); // first look inside any function we might live within

//   cerr << "lookup for text: " << cur_txt << " pos: " << cur_pos << " mbr: " << mbr_name
//        << endl;

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
    lookup_type = 1;
    lookup_seed = txt.from(expr_start);
    prepend_before = txt.before(expr_start);
  }

  switch(lookup_type) {
  case 1: {// lookup variables
//     cerr << "base_path empty: lookup a var, seed: " << lookup_seed << endl;
    taiTokenPtrMultiTypeButton* varlkup =  new taiTokenPtrMultiTypeButton
      (&TA_ProgVar, NULL, NULL,	NULL, 0, lookup_seed);
    varlkup->item_filter = (item_filter_fun)ProgExpr::StdProgVarFilter;
    varlkup->type_list.Link(&TA_ProgVar);
    varlkup->type_list.Link(&TA_DynEnumItem);
    varlkup->GetImage(NULL, &TA_ProgVar, this, &TA_Program); // scope_token is this!
    bool okc = varlkup->OpenChooser();
    if(okc && varlkup->token()) {
      rval = prepend_before + varlkup->token()->GetName();
      new_pos = rval.length();
      rval += append_at_end;
    }
    delete varlkup;
    break;
  }
  case 2: {			// methods
//     cerr << "lookup a memb/meth from path: " << base_path << " seed: " << lookup_seed << endl;
    String path_var, path_rest;
    ProgVar* st_var = NULL;
    TypeDef* lookup_td = NULL;
    taList_impl* tal = NULL;
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
    if(fun)
      st_var = fun->FindVarName(path_var);
    if(!st_var)
      st_var = prg->FindVarName(path_var);
    if(st_var) {
      if(st_var->var_type == ProgVar::T_Object) {
	if(!st_var->object_type) {
	  taMisc::Info("Var lookup: cannot lookup anything about variable:", path_var,
		       "because it is an Object* but has no type set yet!");
	}
	else {
	  if(path_rest.empty()) {
	    lookup_td = st_var->object_type;
	  }
	  else {
	    if((bool)st_var->object_val) {
	      MemberDef* md = NULL;
	      taBase* mb_tab = st_var->object_val->FindFromPath(path_rest, md);
	      if(mb_tab) {
		lookup_td = mb_tab->GetTypeDef();
		if(lookup_td->InheritsFrom(&TA_taList_impl))
		  tal = (taList_impl*)mb_tab;
	      }
	      else {
		if(md) lookup_td = md->type;
	      }
	    }
	    if(!lookup_td) {
	      TypeDef* own_td = st_var->object_type;
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
    if(tal) {
      if(tal->InheritsFrom(&TA_taGroup_impl)) {
	taiGroupElsButton* lilkup = new taiGroupElsButton(lookup_td, NULL, NULL, NULL,
							  0, lookup_seed);
	lilkup->GetImage((taGroup_impl*)tal, NULL);
	bool okc = lilkup->OpenChooser();
	if(okc && lilkup->item()) {
	  rval = txt.through(delim_pos[0]) + lilkup->item()->GetName();
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
	  rval = txt.through(delim_pos[0]) + lilkup->item()->GetName();
	  new_pos = rval.length();
	  rval += append_at_end;
	}
	delete lilkup;
      }
    }
    else if(lookup_td) {
//       cerr << "lookup from type: " << lookup_td->name << endl;
      taiMemberMethodDefButton* mdlkup =  new taiMemberMethodDefButton(lookup_td, NULL, NULL,
								       NULL, 0, lookup_seed);
      mdlkup->GetImage(NULL, lookup_td);
      bool okc = mdlkup->OpenChooser();
      if(okc && mdlkup->md()) {
	rval = txt.through(delim_pos[0]) + mdlkup->md()->name;
	new_pos = rval.length();
	rval += append_at_end;
      }
      delete mdlkup;
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
      eslkup->GetImage(NULL, lookup_td);
      bool okc = eslkup->OpenChooser();
      if(okc && eslkup->md()) {
	rval = txt.through(delim_pos[0]) + eslkup->md()->name;
	new_pos = rval.length();
	rval += append_at_end;
      }
      delete eslkup;
    }
    else {			// now try for local enums
      ProgType* pt = prg->types.FindName(base_path);
      if(pt && pt->InheritsFrom(&TA_DynEnumType)) {
	taiTokenPtrButton* varlkup =  new taiTokenPtrButton(&TA_DynEnumItem, NULL, NULL,
							    NULL, 0, lookup_seed);
	varlkup->GetImage(NULL, &TA_DynEnumItem, pt, &TA_DynEnumType); // scope to this guy
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

String MemberProgEl::StringFieldLookupFun(const String& cur_txt, int cur_pos,
					  const String& mbr_name, int& new_pos) {
  String txt = cur_txt.before(cur_pos);
  String extra_txt = cur_txt.from(cur_pos);

  String rval = _nilString;

  if(!obj) {
    obj_type = &TA_taBase; // placeholder
    return rval;
  }

  String base_path;
  String seed_path;

  if(!txt.empty()) {
    if(txt.contains('.')) {
      base_path = txt.through('.',-1);
      seed_path = txt.after('.', -1);
    }
    else {
      base_path = "";
      seed_path = txt;
    }
  }

  TypeDef* lookup_td = obj->act_object_type();
  if(base_path.nonempty()) {
    int net_base_off = 0;
    ta_memb_ptr net_mbr_off = 0;
    MemberDef* md = 
      TypeDef::FindMemberPathStatic(lookup_td, net_base_off, net_mbr_off,
				    base_path, false);
    if(md) lookup_td = md->type;
    if(!lookup_td) {
      taMisc::Info("Var lookup: cannot find path:", base_path, "in object of type:",
		   obj->act_object_type()->name);
      return rval;
    }
  }

  taiMemberDefButton* mdlkup = new taiMemberDefButton(lookup_td, NULL, NULL,
						      NULL, 0, seed_path);
  mdlkup->GetImage(NULL, lookup_td);
  bool okc = mdlkup->OpenChooser();
  MemberDef* md = mdlkup->md();
  if(okc && md) {
    rval = base_path + md->name;
    if(md->type->InheritsFormal(&TA_class) && !md->type->InheritsFrom(&TA_taString))
      rval += ".";
    new_pos = rval.length();
    rval += extra_txt;
  }
  delete mdlkup;

  return rval;
}

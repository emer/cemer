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

#include "iProgramEditor.h"
#include <iBrowseHistory>
#include <iStripeWidget>
#include <iMethodButtonMgr>
#include <HiLightButton>
#include <iTreeView>
#include <iTreeViewItem>
#include <iTreeSearch>
#include <iMainWindowViewer>
#include <taiMember>
#include <iLabel>
#include <taiPolyData>
#include <taProject>


#include <SigLinkSignal>
#include <taMisc>
#include <taiMisc>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QToolBar>
#include <QToolButton>
#include <QApplication>
#include <QLineEdit>
#include <QKeyEvent>


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
  m_show = (TypeItem::ShowMembs)(taMisc::show_gui & TypeItem::SHOW_CHECK_MASK);
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

  search = new iTreeSearch(items, this);
  layOuter->addWidget(search);

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
  connect(brow_hist, SIGNAL(select_item(taiSigLink*)),
    this, SLOT(slot_AssertBrowserItem(taiSigLink*)) );
  // no history, just manually disable
  historyBackAction->setEnabled(false);
  historyForwardAction->setEnabled(false);
  // toolbar
  tb->addAction(historyBackAction);
  tb->addAction(historyForwardAction);

  //TEMP
  if (QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(historyBackAction))) {
    but->setArrowType(Qt::LeftArrow);
  }
  if (QToolButton* but = qobject_cast<QToolButton*>(tb->widgetForAction(historyForwardAction))) {
    but->setArrowType(Qt::RightArrow);
  }

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
    Apply();                    // do it!
    items->setFocus();  // return to items!
    return true;
  }
  if(e->key() == Qt::Key_Escape) {
    Revert();                   // do it!
    items->setFocus();  // return to items!
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
  base->AddSigClient(this);
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
      taiEditorWidgetsMain::GetName(md, name, desc);
      iLabel* lbl = taiEditorWidgetsMain::MakeInitEditLabel(name, body,
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
  base->RemoveSigClient(this);
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

iTreeViewItem* iProgramEditor::AssertBrowserItem(taiSigLink* link)
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

void iProgramEditor::SigLinkDestroying(taSigLink* dl) {
  setEditNode(NULL, false);
}

void iProgramEditor::SigLinkRecv(taSigLink* dl, int sls, void* op1, void* op2) {
  if (m_changing > 0) return; // gets triggered when we do the GetValue on ctrl0
  if (sls <= SLS_ITEM_UPDATED_ND) {
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

  if(typ->IsActualTaBase() && base) {
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
  if (typ->IsActualTaBase()) {
    base->UpdateAfterEdit();    // hook to update the contents after an edit..
    base->MakeNameUnique();
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
  QList<QWidget*> list = body->findChildren<QWidget*>();
  for (int i=0; i<list.size(); ++i) {
    QWidget* rep = list.at(i);
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
    taiEditorWidgetsMain::DoFillLabelContextMenu_SelEdit(menu, last_id,
      sel_item_base, sel_item_mbr, body,
    this, SLOT(DoSelectForEdit(QAction*)));
  }

  if (menu->actions().count() > 0)
    menu->exec(sender->mapToGlobal(e->pos()));
  delete menu;
}

void iProgramEditor::DoSelectForEdit(QAction* act) {
//note: this routine is duplicated in the taiEditorWidgetsOfClass

  taProject* proj = (taProject*)(base->GetThisOrOwner(&TA_taProject));
  if (!proj) return;

  int param = act->data().toInt();
  SelectEdit* se = proj->edits.Leaf(param);

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
  taiEditorWidgetsMain::MakeDarkBgColor(bg_color, bg_color_dark);
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
  value = (value & TypeItem::SHOW_CHECK_MASK);
  if (m_show == value) return;
  m_show = (TypeItem::ShowMembs)value;
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


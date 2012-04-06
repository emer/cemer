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

#include "ta_seledit_qt.h"

#include "ta_qt.h"
#include "ta_qtdialog.h" // for Hilight button
#include "ta_TA_inst.h"

#include <QColor>
#include <QHeaderView>
#include <QLayout>
#include <QTextEdit>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "iflowlayout.h"

//////////////////////////////////
//  tabSelectEditViewType       //
/////////////////////////////////

int tabSelectEditViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_SelectEdit))
    return (inherited::BidForView(td) +1);
  return 0;
}

void tabSelectEditViewType::CreateDataPanel_impl(taiDataLink* dl_)
{
  // we create ours first, because it should be the default
  iSelectEditPanel* bldp = new iSelectEditPanel(dl_);
  DataPanelCreated(bldp);
  inherited::CreateDataPanel_impl(dl_);
}


//////////////////////////////////
//  iSelectEditDataHostBase     //
//////////////////////////////////

iSelectEditDataHostBase::iSelectEditDataHostBase(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, false, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHostBase::~iSelectEditDataHostBase() {
}

void iSelectEditDataHostBase::Initialize()
{
  no_meth_menu = true; // only show them on outer menu, by way of Propertiesguy
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
}

void iSelectEditDataHostBase::Constr_Body() {
  if (rebuild_body) {
    meth_el.Reset();
  }
  inherited::Constr_Body();
  // we deleted the normally not-deleted methods, so redo them here
  if (rebuild_body) {
    Constr_Methods();
    frmMethButtons->setHidden(!showMethButtons());
  }
}

void iSelectEditDataHostBase::Constr_Methods_impl() {
  inherited::Constr_Methods_impl();
  Insert_Methods();

  FOREACH_SUBGROUP(EditMthItem_Group, grp, sele->mths) {
    //note: root group uses only buttons (hard wired)
    EditMthItem_Group::MthGroupType group_type = grp->group_type;

    // make a menu or button group if needed
    String men_nm = grp->GetDisplayName(); // blank for default
    switch (group_type) {
    case EditMthItem_Group::GT_MENU: {
      SetCurMenu_Name(men_nm); // default is "Actions"
    } break;
    case EditMthItem_Group::GT_MENU_BUTTON: {
      if (men_nm.empty()) // shouldn't happen
        men_nm = "Actions";
      cur_menu_but = ta_menu_buttons.FindName(men_nm);
      if (cur_menu_but == NULL) {
        cur_menu_but = taiActions::New
          (taiMenu::buttonmenu, taiMenu::normal, taiMisc::fonSmall,
                  NULL, this, NULL, widget());
        cur_menu_but->setLabel(men_nm);
        DoAddMethButton((QPushButton*)cur_menu_but->GetRep()); // rep is the button for buttonmenu
        ta_menu_buttons.Add(cur_menu_but);
      }
    } break;
    default: break; // nothing for butts
    } // switch group_type

    for (int i = 0; i < grp->size; ++i) {
      EditMthItem* item = grp->FastEl(i);
      MethodDef* md = item->mth;
      taBase* base = item->base;
      if (!md || (md->im == NULL) || (base == NULL)) continue;
      taiMethod* im = md->im;
      if (im == NULL) continue;

      //NOTE: for seledit functions, we never place them on the last menu or button, because that may
      // make no sense -- the label specifies the place, or Actions if no label
      String mth_cap = item->caption();
      String statustip = item->desc;
      taiMethodData* mth_rep = NULL;
      switch (group_type) {
      case EditMthItem_Group::GT_BUTTONS:  {
        mth_rep = im->GetButtonMethodRep(base, this, NULL, frmMethButtons);
        AddMethButton(mth_rep, mth_cap);
      } break;
      case EditMthItem_Group::GT_MENU: {
        mth_rep = im->GetMenuMethodRep(base, this, NULL, NULL/*frmMethButtons*/);
//        mth_rep->AddToMenu(cur_menu);
        taiAction* act = cur_menu->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      case EditMthItem_Group::GT_MENU_BUTTON: {
        mth_rep = im->GetMenuMethodRep(base, this, NULL, NULL/*frmMethButtons*/);
//        mth_rep->AddToMenu(cur_menu_but);
        taiAction* act = cur_menu_but->AddItem(mth_cap, taiMenu::use_default,
              taiAction::action, mth_rep, SLOT(CallFun()) );
        if (statustip.nonempty())
          act->setStatusTip(statustip);
      } break;
      } // switch group_type
      if (mth_rep)
        meth_el.Add(mth_rep);
    }
  } // groups
}

taBase* iSelectEditDataHostBase::GetMembBase_Flat(int idx) {
  return sele->mbrs.GetBase_Flat(idx);
}

taBase* iSelectEditDataHostBase::GetMethBase_Flat(int idx) {
  return sele->mths.GetBase_Flat(idx);
}


void iSelectEditDataHostBase::mnuRemoveMember_select(int idx) {
  sele->RemoveField(idx);
}

void iSelectEditDataHostBase::mnuRemoveMethod_select(int idx) {
  sele->RemoveFun(idx);
}



//////////////////////////
//  iSelectEditDataHost //
//////////////////////////

iSelectEditDataHost::iSelectEditDataHost(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
}

iSelectEditDataHost::~iSelectEditDataHost() {
}

void iSelectEditDataHost::Initialize()
{
}

void iSelectEditDataHost::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  if(frmMethButtons) {
    if(layMethButtons && (layMethButtons != (iFlowLayout*)frmMethButtons->layout())) {
      delete layMethButtons;
    }
    if(frmMethButtons->layout()) {
      delete frmMethButtons->layout();
    }
    layMethButtons = NULL;
    taiMisc::DeleteChildrenLater(frmMethButtons);
  }
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
  inherited::ClearBody_impl();
}

void iSelectEditDataHost::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  dat_cnt = 0;
  // mark place
  String nm;
  String help_text;
  MembSet* memb_set = NULL;

  int set_idx = 0;
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(EditMbrItem_Group, grp, sele->mbrs) {
    bool def_grp = (grp == &(sele->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    memb_set = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      iLabel* lbl = new iLabel(grp->GetName(), body);
      AddSectionLabel(-1, lbl,
        "");
    }
    for (int i = 0; i < grp->size; ++i) {
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL)) continue; // should only happen if created manually (Bad!)
      taiData* mb_dat = md->im->GetDataRep(this, NULL, body);
      memb_set->memb_el.Add(md);
      memb_set->data_el.Add(mb_dat);
      QWidget* data = mb_dat->GetRep();
      //int row = AddData(-1, data);

      help_text = item->GetDesc();
      String new_lbl = item->caption();
      //obs GetName(md, nm, help_text); //note: we just call this to get the help text
      AddNameData(-1, new_lbl, help_text, data, mb_dat, md);
      ++dat_cnt;
    }
    def_grp = false;
    ++set_idx;
  }
}

void iSelectEditDataHost::DoRemoveSelEdit() {
   // removes the sel_item_index item
  int sel_item_index = membs.GetFlatDataIndex(sel_item_dat);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
  else
    taMisc::DebugInfo("iSelectEditDataHost::DoRemoveSelEdit: could not find item");
}

void iSelectEditDataHost::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = membs.GetFlatDataIndex(sel_item_mbr, sel_item_base);
  if (sel_item_index < 0) return;
  //QAction* act =
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void iSelectEditDataHost::GetImage_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL)) {
        taMisc::DebugInfo("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        md->im->GetImage(mb_dat, item->base); // need to do this first, to affect visible
      }
      ++itm_idx;
    }
  }
}

void iSelectEditDataHost::GetValue_Membs_def() {
  int itm_idx = 0;
  for (int j = 0; j < membs.size; ++j) {
    MembSet* ms = membs.FastEl(j);
    for (int i = 0; i < ms->data_el.size; ++i) {
      taiData* mb_dat = ms->data_el.FastEl(i);
      MemberDef* md = ms->memb_el.SafeEl(i);
      EditMbrItem* item = sele->mbrs.Leaf(itm_idx);
      if ((item == NULL) || (item->base == NULL) || (md == NULL) || (mb_dat == NULL)) {
        taMisc::DebugInfo("iSelectEditDataHost::GetImage_impl(): unexpected md or mb_dat=NULL at i ", String(i));
      }
      else {
        bool first_diff = true;
        md->im->GetMbrValue(mb_dat, item->base, first_diff);
        if (!first_diff)
          taiMember::EndScript(item->base);
        item->base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
      }
      ++itm_idx;
    }
  }
}


//////////////////////////
//   taiDataDelegate    //
//////////////////////////


taiDataDelegate::taiDataDelegate(taiEditDataHost* edh_)
{
  edh = edh_;
  m_dat_row = -1;
  connect(this, SIGNAL(closeEditor(QWidget*,
    QAbstractItemDelegate::EndEditHint)),
    this, SLOT(this_closeEditor(QWidget*,
    QAbstractItemDelegate::EndEditHint)) );
}

QWidget* taiDataDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  MemberDef* md = NULL;
  taBase* base = NULL;
  if (IndexToMembBase(index, md, base)) {
    if (md->im == NULL) goto exit; // shouldn't happen
    // we create a wrap widget for many of these guys, mostly so that smaller
    // guys like Combo don't try to be stretched the whole way
    bool wrap = true;           // NOTE: wrap is always true!!
    QWidget* rep_par = (wrap) ?  new QWidget(parent) : parent;

    dat = md->im->GetDataRep(edh, NULL, rep_par);
    m_dat_row = index.row();
    dat->SetBase(base);
    dat->SetMemberDef(md);
    rep = dat->GetRep(); // note: rep may get replaced by rep_par
    // color stuff
    // by default, the table color shines through widget, which is weird
    // so we will shut that off if it isn't merely a container widget
    String cn(rep->metaObject()->className());
//TODO: FIGURE THIS OUT!!! wasn't any of these:
    if (cn != "QWidget") {
      //rep->setAutoFillBackground(false);
      //rep->setAttribute(Qt::WA_NoSystemBackground);
      //rep->setAttribute(Qt::WA_OpaquePaintEvent);
    }
    if (wrap) {
      hbl = new QHBoxLayout(rep_par);
      hbl->setMargin(0);
      hbl->setSpacing(0);
      hbl->addWidget(rep);
      // some controls do better without stretch
      if (!(dynamic_cast<taiField*>((taiData*)dat)))
        hbl->addStretch();
      rep = rep_par;
    }
    connect(rep, SIGNAL(destroyed(QObject*)),
      dat, SLOT(deleteLater()) );

    EditorCreated(parent, rep, option, index);
    return rep;
  }
exit:
  return inherited::createEditor(parent, option, index);
}

void taiDataDelegate::EditorCreated(QWidget* parent, QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  // resize the column to accommodate the controls
  // (this seems the only place that works -- DataDelegate::sizeHint()
  //  gets called *before* createEditor() so we don't know its
  //  size yet at that point; therefore, we hackily do it here)
  //  sh = editor->sizeHint();
  sh = editor->minimumSizeHint(); // use min b/c sometimes it gets too big..
  QTableView* tv = NULL;
  QWidget* tv_candidate = parent;
  while (tv_candidate) {
    tv = qobject_cast<QTableView*>(tv_candidate);
    if (tv) break;
    tv_candidate = tv_candidate->parentWidget();
  }
  if (tv) {
    if (tv->columnWidth(1) < sh.width())
      tv->setColumnWidth(1, sh.width());
  }
}

bool taiDataDelegate::eventFilter(QObject *object, QEvent *event)
{
  QWidget *editor = qobject_cast<QWidget*>(object);
  if (!editor)
      return false;
  if (event->type() == QEvent::KeyPress) {
    switch (static_cast<QKeyEvent *>(event)->key()) {
/*  case Qt::Key_Tab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditNextItem);
      return true;
    case Qt::Key_Backtab:
      emit commitData(editor);
      emit closeEditor(editor, QAbstractItemDelegate::EditPreviousItem);
      return true;*/
    case Qt::Key_Enter:
    case Qt::Key_Return:
      if (qobject_cast<QTextEdit*>(editor))
          return false; // don't filter enter key events for QTextEdit
      // We want the editor to be able to process the key press
      // before committing the data (e.g. so it can do
      // validation/fixup of the input).
      if (QLineEdit* e = qobject_cast<QLineEdit*>(editor))
        if (!e->hasAcceptableInput())
          return false;
      QMetaObject::invokeMethod(this, "_q_commitDataAndCloseEditor",
        Qt::QueuedConnection, Q_ARG(QWidget*, editor));
      return false;
    case Qt::Key_Escape:
      // don't commit data
      emit closeEditor(editor, QAbstractItemDelegate::RevertModelCache);
      edh->Unchanged();
      break;
    default:
      return false;
    }
    if (editor->parentWidget())
        editor->parentWidget()->setFocus();
    return true;
  }
/*  else if (event->type() == QEvent::FocusOut) {
    if (!editor->isActiveWindow() || (QApplication::focusWidget() != editor)) {
      QWidget *w = QApplication::focusWidget();
      while (w) { // don't worry about focus changes internally in the editor
        if (w == editor)
            return false;
        w = w->parentWidget();
      }
      // The window may lose focus during an drag operation.
      // i.e when dragging involves the taskbar on Windows.
      if (QDragManager::self() && QDragManager::self()->object != 0)
        return false;
      // Opening a modal dialog will start a new eventloop
      // that will process the deleteLater event.
      if (QApplication::activeModalWidget()
        && !QApplication::activeModalWidget()->isAncestorOf(editor)
        && qobject_cast<QDialog*>(QApplication::activeModalWidget()))
        return false;
      emit commitData(editor);
      emit closeEditor(editor, NoHint);
    }
  } */
  else if (event->type() == QEvent::ShortcutOverride) {
    if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
      event->accept();
      return true;
    }
  }
  return false;
}

void taiDataDelegate::GetImage() const {
  if (!dat) return;
  edh->Updating(true);
  if (dat->mbr) { // has member (typical case)
    dat->mbr->im->GetImage(dat, dat->Base());
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetImage_(dat->Base());
  }
  edh->Updating(false);
  edh->Unchanged();
}

void taiDataDelegate::GetValue() const {
  if (!dat) return;
  taBase* base = dat->Base(); // cache
  if (dat->mbr) { // has member (typical case)
    bool first_diff = true;
    dat->mbr->im->GetMbrValue(dat, base, first_diff);
    if (!first_diff)
      taiMember::EndScript(base);
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetValue_(base);
  }
  base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
  // update text of the cell, otherwise it usually ends up stale!
  edh->GetImage_Item(m_dat_row);
  edh->Unchanged();
}

void taiDataDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
  // skip, and just paint background if an editor is in place
  if (dat && (m_dat_row == index.row())) {
    drawBackground(painter, option, index);
  } else { // normal, which also means interpret rich text!
    // this stuff all from qitemdelegate.cpp
    QStyleOptionViewItemV4 opt = setOptions(index, option); // or V2 for Qt4.2
    const QStyleOptionViewItemV2 *v2 = qstyleoption_cast<const QStyleOptionViewItemV2 *>
      (&option);
    opt.features = v2 ? v2->features
      : QStyleOptionViewItemV2::ViewItemFeatures(QStyleOptionViewItemV2::None);
    const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3 *>
      (&option);
    opt.locale = v3 ? v3->locale : QLocale();
    opt.widget = v3 ? v3->widget : 0;

    // draw rich text:
    painter->save();

    drawBackground(painter, option, index);

    QRect disp_rect = option.rect;

    // this does the rich text interp
    QTextDocument doc;
    doc.setHtml( index.data().toString() );
    painter->setClipRect(disp_rect);
    painter->translate(disp_rect.x(), disp_rect.y()+2); // add a little border
    doc.drawContents(painter);
    painter->restore();

    drawFocus(painter, opt, disp_rect);
  }
}

void taiDataDelegate::rep_destroyed(QObject* rep) {
}

QSize taiDataDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const
{
  QSize rval = inherited::sizeHint(option, index);
  if (dat) {
    rval = rval.expandedTo(sh);
  }
  return rval;
}

void taiDataDelegate::setEditorData(QWidget* editor,
    const QModelIndex& index) const
{
  if (!dat) return;
  GetImage();
}

void taiDataDelegate::setModelData(QWidget* editor,
  QAbstractItemModel* model, const QModelIndex& index) const
{
  if (!dat) return;
  GetValue();
//note: -- testing has indicated that this call only happens once
// so we invalidate it now, so the cell paints properly
  dat = NULL;
  m_dat_row = -1;
}

void taiDataDelegate::this_closeEditor(QWidget* /*editor*/,
    QAbstractItemDelegate::EndEditHint /*hint*/)
{
  // get rid of dat info, so we paint
  dat = NULL;
  m_dat_row = -1;
}

//////////////////////////
//   SelectEditDelegate //
//////////////////////////

QTableWidgetItem* /*SelectEditDelegate::*/ItemFromIndex(QTableWidget* tw,
  const QModelIndex& index)
{
  QTableWidgetItem* rval = tw->item(index.row(), index.column());
  return rval;
}

SelectEditDelegate::SelectEditDelegate(SelectEdit* sele_,
  iSelectEditDataHost2* sedh_)
:inherited(sedh_)
{
  sele = sele_;
  sedh = sedh_;
}

QWidget* SelectEditDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const
{
  QWidget* rval = inherited::createEditor(parent, option, index);

  if(emi && emi->is_numeric) {
    MemberDef* psmd = TA_EditMbrItem.members.FindName("param_search");
    if(psmd) {
      ps_dat = psmd->im->GetDataRep(edh, NULL, rep);
      ps_dat->SetBase(emi);
      ps_dat->SetMemberDef(psmd);
      ps_rep = ps_dat->GetRep(); // note: rep may get replaced by rep_par

      hbl->addWidget(ps_rep);
      // some controls do better without stretch
      if (!(dynamic_cast<taiField*>((taiData*)ps_dat)))
        hbl->addStretch();
      connect(ps_rep, SIGNAL(destroyed(QObject*)),
              ps_dat, SLOT(deleteLater()) );
    }
  }

  QSize sz(rval->size());
  if (sz.width() > sedh->tw->columnWidth(1)) {
    sedh->tw->setColumnWidth(1,sz.width());
  }
  return rval;
}

void SelectEditDelegate::GetImage() const {
  if (!dat) return;
  edh->Updating(true);
  if (dat->mbr) { // has member (typical case)
    dat->mbr->im->GetImage(dat, dat->Base());
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetImage_(dat->Base());
  }
  if(ps_dat) {
    if (ps_dat->mbr) { // has member (typical case)
      ps_dat->mbr->im->GetImage(ps_dat, ps_dat->Base());
    } else { // no mbr, typically an inline taBase, esp for userdata
      ps_dat->GetImage_(ps_dat->Base());
    }
  }
  edh->Updating(false);
  edh->Unchanged();
}

void SelectEditDelegate::GetValue() const {
  if (!dat) return;
  taBase* base = dat->Base(); // cache
  if (dat->mbr) { // has member (typical case)
    bool first_diff = true;
    dat->mbr->im->GetMbrValue(dat, base, first_diff);
    if (!first_diff)
      taiMember::EndScript(base);
  } else { // no mbr, typically an inline taBase, esp for userdata
    dat->GetValue_(base);
  }
  if(ps_dat) {
    taBase* ps_base = ps_dat->Base(); // cache
    if (ps_dat->mbr) { // has member (typical case)
      bool first_diff = true;
      ps_dat->mbr->im->GetMbrValue(ps_dat, ps_base, first_diff);
      if (!first_diff)
        taiMember::EndScript(ps_base);
    } else { // no mbr, typically an inline taBase, esp for userdata
      ps_dat->GetValue_(ps_base);
    }
  }
  base->UpdateAfterEdit(); // call UAE on item bases because won't happen elsewise!
  // update text of the cell, otherwise it usually ends up stale!
  edh->GetImage_Item(m_dat_row);
  edh->Unchanged();
}

bool SelectEditDelegate::IndexToMembBase(const QModelIndex& index,
    MemberDef*& mbr, taBase*& base) const
{
  QTableWidgetItem* twi = sedh->tw->item(index.row(), index.column());
  if (twi) {
    emi = dynamic_cast<EditMbrItem*>((taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
    if (emi) {
      mbr = emi->mbr;
      base = emi->base;
      return true;
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////
//              taiEditTableWidget

taiEditTableWidget::taiEditTableWidget(QWidget* parent)
:inherited(parent)
{
}

void taiEditTableWidget::keyPressEvent(QKeyEvent* e) {
  if((e->key() == Qt::Key_Tab) || (e->key() == Qt::Key_Backtab)) {
    e->ignore();                        // tell that we don't want this -- send to others
    return;
  }
  bool ctrl_pressed = taiMisc::KeyEventCtrlPressed(e);
  if(ctrl_pressed) {
    QPersistentModelIndex newCurrent;
    switch (e->key()) {
    case Qt::Key_N:
      newCurrent = moveCursor(MoveDown, e->modifiers());
      break;
    case Qt::Key_P:
      newCurrent = moveCursor(MoveUp, e->modifiers());
      break;
    case Qt::Key_U:
      newCurrent = moveCursor(MovePageUp, e->modifiers());
      break;
#ifdef TA_OS_MAC
      // this is a conflict with paste -- only works on mac where cmd and ctrl are diff!
    case Qt::Key_V:
      newCurrent = moveCursor(MovePageDown, e->modifiers());
      break;
#endif
    case Qt::Key_F:
      newCurrent = moveCursor(MoveRight, e->modifiers());
      break;
    case Qt::Key_B:
      newCurrent = moveCursor(MoveLeft, e->modifiers());
      break;
    }
    // from qabstractitemview.cpp
    QPersistentModelIndex oldCurrent = currentIndex();
    if (newCurrent != oldCurrent && newCurrent.isValid()) {
      QItemSelectionModel::SelectionFlags command = selectionCommand(newCurrent, e);
      if (command != QItemSelectionModel::NoUpdate
          || style()->styleHint(QStyle::SH_ItemView_MovementWithoutUpdatingSelection, 0, this)) {
        if (command & QItemSelectionModel::Current) {
          selectionModel()->setCurrentIndex(newCurrent, QItemSelectionModel::NoUpdate);
        } else {
          selectionModel()->setCurrentIndex(newCurrent, QItemSelectionModel::ClearAndSelect);
        }
        return;
      }
    }
  }
  inherited::keyPressEvent(e);
}


//////////////////////////
//  iSelectEditDataHost2        //
//////////////////////////

iSelectEditDataHost2::iSelectEditDataHost2(void* base, TypeDef* td,
  bool read_only_, QObject* parent)
:inherited(base, td, read_only_, parent)
{
  Initialize();
  sele = (SelectEdit*)base;
}

iSelectEditDataHost2::~iSelectEditDataHost2() {
  delete sed; // hope it's ref is deleted too!
  sed = NULL;
}

void iSelectEditDataHost2::Initialize()
{
  tw = NULL;
  sele = NULL;
  sel_edit_mbrs = true; // note: we don't actually select members, just for removal
  sed = new SelectEditDelegate(sele, this);
}

void iSelectEditDataHost2::Constr_Body_impl() {
}

void iSelectEditDataHost2::Constr_Box() {
  row_height = taiM->max_control_height(ctrl_size); // 3 if using line between; 2 if
  if (tw) return;
  tw = new taiEditTableWidget(widget());
  tw->setColumnCount(2);
  tw->horizontalHeader()->setVisible(false);
  tw->horizontalHeader()->setStretchLastSection(true); // note: works if header invis
  tw->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
  tw->verticalHeader()->setVisible(false);
  tw->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  tw->setSortingEnabled(false);
  // don't try to scroll by item, it looks ugly, just scroll smoothly
  tw->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
  // colors
  QPalette palette(tw->palette());
  palette.setColor(QPalette::Base, bg_color);
  palette.setColor(QPalette::AlternateBase, bg_color_dark);
  tw->setPalette(palette);
  tw->setAlternatingRowColors(true);
  tw->setContextMenuPolicy(Qt::CustomContextMenu);

  tw->setItemDelegateForColumn(1, sed);
  vblDialog->addWidget(tw, 1);
  connect(tw, SIGNAL(currentCellChanged(int, int, int, int)),
    this, SLOT(tw_currentCellChanged(int, int, int, int)) );
  connect(tw, SIGNAL(customContextMenuRequested(const QPoint&)),
    this, SLOT(tw_customContextMenuRequested(const QPoint&)) );
  body = tw;
}


void iSelectEditDataHost2::ClearBody_impl() {
  // we also clear all the methods, and then rebuild them
  ta_menus.Reset();
  ta_menu_buttons.Reset();
//  meth_el.Reset(); // must defer deletion of these, because the MethodData objects are used in menu calls, so can't be
  // somehow separate..
  if(frmMethButtons) {
    if(layMethButtons && (layMethButtons != (iFlowLayout*)frmMethButtons->layout())) {
      delete layMethButtons;
    }
    if(frmMethButtons->layout()) {
      delete frmMethButtons->layout();
    }
    layMethButtons = NULL;
    taiMisc::DeleteChildrenLater(frmMethButtons);
  }
  show_meth_buttons = false;

  // note: no show menu in this class
  cur_menu = NULL;
  cur_menu_but = NULL;
  if (menu) {
    menu->Reset();
  }
//??  inherited::ClearBody_impl();
}

void iSelectEditDataHost2::Constr_Data_Labels() {
  // delete all previous sele members
  membs.ResetItems();
  tw->clearSpans();
  tw->clear();
  // mark place
  String nm;
  String help_text;
  MembSet* ms = NULL;

  int set_idx = 0;
  int row = 0;
  // note: iterates non-empty groups only
  FOREACH_SUBGROUP(EditMbrItem_Group, grp, sele->mbrs) {
    bool def_grp = (grp == &(sele->mbrs));// root group
    membs.SetMinSize(set_idx + 1);
    ms = membs.FastEl(set_idx);
    // make a group header
    if (!def_grp) {
      tw->setRowCount(row+1);
      int item_flags = Qt::ItemIsEnabled; //Qt::ItemIsUserCheckable;
      ms->text = grp->GetName();
      ms->show = true;
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(ms->text);
      twi->setFlags((Qt::ItemFlags)item_flags);
      //TODO: Bold
      QFont f(tw->font());
      f.setBold(true);
      twi->setFont(f);
      tw->setItem(row, 0, twi);
      tw->setSpan(row, 0, 1, 2);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    tw->setRowCount(row + grp->size);
    for (int i = 0; i < grp->size; ++i) {
      int item_flags = 0;
      EditMbrItem* item = grp->FastEl(i);
      MemberDef* md = item->mbr;
      if (!md || (md->im == NULL)) continue; // shouldn't happen

      ms->memb_el.Add(md);
      // force span, in case this row had a group before
      // this is now giving an error in 4.6.0 beta -- unnec?
      // but it needs the call in 4.5.x -- otherwise row is blank.
#if (QT_VERSION < 0x040600)
      tw->setSpan(row, 0, 1, 1);
#endif
      // label item
      QTableWidgetItem* twi = new QTableWidgetItem;
      twi->setText(item->caption());
      twi->setStatusTip(item->GetDesc());
      twi->setToolTip(item->GetDesc());
      item_flags = Qt::ItemIsEnabled;
      twi->setFlags((Qt::ItemFlags)item_flags);
      tw->setItem(row, 0, twi);

      // data item
      twi = new QTableWidgetItem;
      item_flags = Qt::ItemIsEnabled; // |Qt::ItemIsSelectable;
      //TODO: check for READONLY
      bool ro = false;
      if (!ro)
        item_flags |= Qt::ItemIsEditable;
      twi->setFlags((Qt::ItemFlags)item_flags);
      // set EditMbrItem into 1.data
      twi->setData(Qt::UserRole, QVariant((ta_intptr_t)item));

      tw->setItem(row, 1, twi);
      tw->setRowHeight(row, row_height);
      ++row;
    }
    def_grp = false;
    ++set_idx;
  }
  tw->resizeColumnToContents(0); // note: don't do 1 here, do it in GetImage
}

void iSelectEditDataHost2::GetImage_Item(int row) {
  if (row < 0) return;
  QTableWidgetItem* it = tw->item(row, 1);
  QTableWidgetItem* lbl = tw->item(row, 0); // not always needed
  if (!it) return;
  EditMbrItem* item = dynamic_cast<EditMbrItem*>(
    (taBase*)(it->data(Qt::UserRole).value<ta_intptr_t>()));
  if ((item == NULL) || (item->base == NULL) ||  (item->mbr == NULL)) return;
  if(!item->base->InheritsFrom((TypeDef*)item->mbr->owner->owner)) {
    taMisc::Warning("type mismatch in select edit", item->label, "-- should be removed asap",
                    ((TypeDef*)item->mbr->owner->owner)->name, "!=",
                    item->base->GetTypeDef()->name);
    return;
  }
  void* off = item->mbr->GetOff(item->base);
  String txt = item->mbr->type->GetValStr(off, item->base,
                                          item->mbr, TypeDef::SC_DISPLAY, true);
  if(item->is_numeric) {
    txt += "&nbsp;&nbsp;|&nbsp;&nbsp;" + TA_EditParamSearch.GetValStr(&(item->param_search), NULL, NULL, TypeDef::SC_DISPLAY, true);
  }

  // true = force inline
  // augment plain non-class vals with bg color
  if(!txt.contains("<font style=\"background-color:")) {
    if(item->mbr->type->DerivesFormal(TA_enum) || item->mbr->type->DerivesFrom(TA_taSmartPtr)
        || item->mbr->type->DerivesFrom(TA_taSmartRef) || item->mbr->type->ptr > 0)
      txt = "<font style=\"background-color: LightGrey\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
    else
      txt = "<font style=\"background-color: white\">&nbsp;&nbsp;" + txt + "&nbsp;&nbsp;</font>";
  }

  it->setText(txt);
  it->setToolTip(txt); // for when over

  // default highlighting
  switch (item->mbr->GetDefaultStatus(item->base)) {
  case MemberDef::NOT_DEF:
    lbl->setData(Qt::BackgroundRole, QColor(Qt::yellow));
    break;
  case MemberDef::EQU_DEF:
    //note: setting nil Variant will force it to ignore and use bg
    lbl->setData(Qt::BackgroundRole, QVariant());
    break;
  default: break; // compiler food
  }
}

void iSelectEditDataHost2::GetImage_Membs_def() {
  for (int row = 0; row < tw->rowCount(); ++row) {
    GetImage_Item(row);
  }
  sed->GetImage(); // if a ctrl is active
  // note: this is taking the bulk of the compute time for refreshing during running:
  // it seems fine to leave it off for now -- once you click on something it resizes
  // and that seems good enough..
  //  tw->resizeColumnToContents(1);
}

void iSelectEditDataHost2::DoRemoveSelEdit() {
   // removes the sel_item_index item
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index >= 0) {
    sele->RemoveField(sel_item_index);
  }
  else
    taMisc::DebugInfo("iSelectEditDataHost::DoRemoveSelEdit: could not find item");
}

void iSelectEditDataHost2::FillLabelContextMenu_SelEdit(QMenu* menu,
  int& last_id)
{
  int sel_item_index = -1;
  sele->mbrs.FindItemBase(sel_item_base, sel_item_mbr, sel_item_index);
  if (sel_item_index < 0) return;
  //QAction* act =
  menu->addAction("Remove from SelectEdit", this, SLOT(DoRemoveSelEdit()));
}

void iSelectEditDataHost2::GetValue_Membs_def() {
  sed->GetValue();
}

void iSelectEditDataHost2::tw_currentCellChanged(int row,
    int col, int previousRow, int previousColumn)
{
  if ((row < 0) || (col < 1)) return;
  QTableWidgetItem* twi = tw->item(row, col);
  if (!twi) return;
  tw->editItem(twi);
}

void iSelectEditDataHost2::tw_customContextMenuRequested(const QPoint& pos)
{
  int row = tw->rowAt(pos.y());
  int col = tw->columnAt(pos.x());
  if ((row < 0) || (col != 0)) return;
  // we want the data item for the label, to get its goodies...
  QTableWidgetItem* twi = tw->item(row, 1);
  if (!twi) return; // ex. right clicking on a section

  EditMbrItem* item = dynamic_cast<EditMbrItem*>(
    (taBase*)(twi->data(Qt::UserRole).value<ta_intptr_t>()));
  if ((item == NULL) || (item->base == NULL)) return;

  sel_item_mbr = item->mbr;
  sel_item_base = item->base;
  QMenu* menu = new QMenu(widget());
  int last_id = -1;
  FillLabelContextMenu(menu, last_id);
  if (menu->actions().count() > 0)
    menu->exec(tw->mapToGlobal(pos));
  delete menu;

}



//////////////////////////
//   iSelectEditPanel   //
//////////////////////////

iSelectEditPanel::iSelectEditPanel(taiDataLink* dl_)
:inherited(dl_)
{
  SelectEdit* se_ = sele();
  se = NULL;
  if (se_) {
    switch (taMisc::select_edit_style) {
    case taMisc::ES_ALL_CONTROLS:
      se = new iSelectEditDataHost(se_, se_->GetTypeDef());
      break;
    case taMisc::ES_ACTIVE_CONTROL:
      se = new iSelectEditDataHost2(se_, se_->GetTypeDef());
      break;
    }
    if (taMisc::color_hints & taMisc::CH_EDITS) {
      bool ok;
      iColor bgcol = se_->GetEditColorInherit(ok);
      if (ok) se->setBgColor(bgcol);
    }
  }
}

iSelectEditPanel::~iSelectEditPanel() {
  if (se) {
    delete se;
    se = NULL;
  }
}

void iSelectEditPanel::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  //NOTE: don't need to do anything because DataModel will handle it
}

bool iSelectEditPanel::HasChanged() {
  return se->HasChanged();
}

bool iSelectEditPanel::ignoreDataChanged() const {
  return !isVisible();
}

void iSelectEditPanel::OnWindowBind_impl(iTabViewer* itv) {
  inherited::OnWindowBind_impl(itv);
  se->ConstrEditControl();
  setCentralWidget(se->widget()); //sets parent
  setButtonsWidget(se->widButtons);
}

void iSelectEditPanel::UpdatePanel_impl() {
  if (se) se->ReShow_Async();
}

void iSelectEditPanel::ResolveChanges_impl(CancelOp& cancel_op) {
 // per semantics elsewhere, we just blindly apply changes
  if (se && se->HasChanged()) {
    se->Apply();
  }
}

void iSelectEditPanel::showEvent(QShowEvent* ev) {
  inherited::showEvent(ev);
  if(se && se->state >= taiDataHost::CONSTRUCTED)  {
    se->GetButtonImage();		// update buttons whenver we show!
  }
}


//////////////////////////////////
//   taiObjDiffBrowser          //
//////////////////////////////////

taiObjDiffBrowser* taiObjDiffBrowser::New(taObjDiff_List& diffs,
                                          int font_type, QWidget* par_window_) {
  String caption = "DiffCompare_" + diffs.tab_obj_a->GetDisplayName() + "_" +
    diffs.tab_obj_b->GetDisplayName();

  taiObjDiffBrowser* rval = new taiObjDiffBrowser(caption, par_window_);
  rval->setFont(taiM->dialogFont(font_type));
  rval->odl = &diffs;
  rval->filter_dlg = NULL;
  rval->Constr();
  return rval;
}

taiObjDiffBrowser::taiObjDiffBrowser(const String& caption_, QWidget* par_window_)
:inherited(par_window_)
{
  caption = caption_;
  setModal(true);
  setWindowTitle(caption);
  resize(taiM->dialogSize(taiMisc::hdlg_b));
}

taiObjDiffBrowser::~taiObjDiffBrowser() {
  if(filter_dlg)
    delete filter_dlg;
  filter_dlg = NULL;
}

void taiObjDiffBrowser::accept() {
  inherited::accept();
}

void taiObjDiffBrowser::reject() {
  inherited::reject();
}

bool taiObjDiffBrowser::Browse() {
  return (exec() == iDialog::Accepted);
}

Q_DECLARE_METATYPE(taObjDiffRec*);

void taiObjDiffBrowser::Constr() {
  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  String a_path = odl->tab_obj_a->GetPathNames();
  String b_path = odl->tab_obj_b->GetPathNames();
  String lb_txt = "Differences between object A and object B, shown as changes needed to make A into B\nA is: " + a_path + "\nB is: " + b_path +
    "\nClick actions to actually perform edits on objects";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  //  layOuter->addSpacing(taiM->vsep_c);

  QHBoxLayout* lay = new QHBoxLayout();
  btnAllA = new QPushButton("Toggle All &A's", this);
  btnAllA->setToolTip("Toggle selection of all the selectable actions for the A item -- i.e., make the A equivalent to the B");
  btnAllA->setDefault(false);
  btnAllA->setAutoDefault(false);
  lay->addWidget(btnAllA);

  btnFiltA = new QPushButton("Set Filtered A's", this);
  btnFiltA->setToolTip("Set actions on or off for filtered subset of the selectable actions for the A item");
  btnFiltA->setDefault(false);
  btnFiltA->setAutoDefault(false);
  lay->addWidget(btnFiltA);
  lay->addStretch();

  btnAllB = new QPushButton("Toggle All &B's", this);
  btnAllB->setToolTip("Toggle selection all the selectable actions for the B item -- i.e., make the B equivalent to the A");
  btnAllB->setDefault(false);
  btnAllB->setAutoDefault(false);
  lay->addWidget(btnAllB);

  btnFiltB = new QPushButton("Set Filtered B's", this);
  btnFiltB->setToolTip("Set actions on or off for filtered subset of the selectable actions for the B item");
  btnFiltB->setDefault(false);
  btnFiltB->setAutoDefault(false);
  lay->addWidget(btnFiltB);
  lay->addStretch();

  layOuter->addLayout(lay);

  items = new QTreeWidget(this);
  layOuter->addWidget(items, 1); // list is item to expand in host

  items->setColumnCount(COL_N);
  items->setSortingEnabled(false);// only 1 order possible
  items->setEditTriggers(QAbstractItemView::DoubleClicked);
  items->headerItem()->setText(COL_NEST, "Nesting");
  items->headerItem()->setToolTip(COL_NEST, "Nesting level below the original A or B object");
  items->headerItem()->setText(COL_A_FLG, "A Action");
  items->headerItem()->setToolTip(COL_A_FLG, "Edit action to perform on the A object -- these actions, if selected, will transform A into B");
  items->headerItem()->setText(COL_A_NM, "A Name");
  items->headerItem()->setToolTip(COL_A_NM, "Name of the item in A -- member name or sub-object name");
  items->headerItem()->setText(COL_A_VAL, "A Value");
  items->headerItem()->setToolTip(COL_A_VAL, "Value of the item in B");

  items->headerItem()->setText(COL_SEP, " | ");

  items->headerItem()->setText(COL_B_FLG, "B Action");
  items->headerItem()->setToolTip(COL_B_FLG, "Edit action to perform on the B object -- these actions, if selected, will transform B into A (i.e., the opposite of the default 'diff' direction)");
  items->headerItem()->setText(COL_B_NM, "B Name");
  items->headerItem()->setToolTip(COL_B_NM, "Name of the item in B -- member name or sub-object name");
  items->headerItem()->setText(COL_B_VAL, "B Value");
  items->headerItem()->setToolTip(COL_B_VAL, "Value of the item in B");
  items->setUniformRowHeights(true);
  items->setIndentation(taMisc::tree_indent);

  lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );

  connect(btnAllA, SIGNAL(clicked()), this, SLOT(toggleAllA()) );
  connect(btnAllB, SIGNAL(clicked()), this, SLOT(toggleAllB()) );

  connect(btnFiltA, SIGNAL(clicked()), this, SLOT(setFilteredA()) );
  connect(btnFiltB, SIGNAL(clicked()), this, SLOT(setFilteredB()) );

  connect(items, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this,
          SLOT(itemClicked(QTreeWidgetItem*,int)));

  AddItems();

  filter_dlg = taiObjDiffBrowserFilter::New(taiMisc::defFontSize, this);
}

void taiObjDiffBrowser::AddItems() {
  if(! odl || odl->size == 0) return;

  QBrush add_color(Qt::green);
  QBrush del_color(Qt::red);
  QBrush chg_color(Qt::yellow);
  int max_width = 40;

  voidptr_PArray        nest_pars;

  int init_nest = odl->FastEl(0)->nest_level; // should be 0..

  for(int i=0;i<odl->size; i++) {
    taObjDiffRec* rec = odl->FastEl(i);
    String lbl_a;
    bool chk_a = rec->GetCurAction(0, lbl_a);
    String lbl_b;
    bool chk_b = rec->GetCurAction(1, lbl_b);

    QTreeWidgetItem* witm;
    QTreeWidgetItem* parw = NULL;
    int par_nest = rec->nest_level-1;
    if(par_nest > init_nest) {
      if(par_nest < nest_pars.size) {
        parw = (QTreeWidgetItem*)nest_pars[par_nest];
        if(!parw) {
	  taMisc::Info("par widg null:", rec->name, "=", rec->value);
        }
      }
      else {
	taMisc::Info("par nest:", String(par_nest), ">= nest pars:",
		     String(nest_pars.size));
      }
      if(!parw) {
        if(rec->par_odr)
          parw = (QTreeWidgetItem*)rec->par_odr->widget;
      }
    }
    if(parw)
      witm = new QTreeWidgetItem(parw);
    else
      witm = new QTreeWidgetItem(items);
    rec->widget = witm;

    QVariant val = qVariantFromValue( rec );
    witm->setData(0, Qt::UserRole+1, val);

    int cur_nest = rec->nest_level;
    nest_pars.SetSize(cur_nest+1);
    nest_pars[cur_nest] = witm;

    witm->setText(COL_NEST, String(rec->nest_level));
    witm->setText(COL_SEP, " | ");

    if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL)) {
      witm->setText(COL_A_NM, rec->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_A_VAL, rec->value.elidedTo(max_width));
      witm->setToolTip(COL_A_VAL, rec->value);
      if(chk_a) witm->setBackground(COL_A_FLG, del_color);
      witm->setBackground(COL_A_VAL, del_color);
      witm->setBackground(COL_A_NM, del_color);
      witm->setText(COL_B_NM, "");
      witm->setText(COL_B_VAL, "");
      if(chk_b) witm->setBackground(COL_B_FLG, add_color);
      witm->setBackground(COL_B_VAL, add_color);
      witm->setBackground(COL_B_NM, add_color);
      witm->setExpanded(false); // never expand a del -- only applies to parents anyway..
      if(!rec->HasDiffFlag(taObjDiffRec::SUB_NO_ACT)) {
        // only ta base items really feasible here..
        if(rec->type->InheritsFrom(&TA_taBase)) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_A_FLG, Qt::Unchecked);
          witm->setCheckState(COL_B_FLG, Qt::Unchecked);
          witm->setText(COL_A_FLG, lbl_a);
          witm->setText(COL_B_FLG, lbl_b);
        }
      }
    }
    else if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD)) {
      witm->setText(COL_A_NM, "");
      witm->setText(COL_A_VAL, "");
      if(chk_a) witm->setBackground(COL_A_FLG, add_color);
      witm->setBackground(COL_A_VAL, add_color);
      witm->setBackground(COL_A_NM, add_color);
      witm->setText(COL_B_NM, rec->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_B_VAL, rec->value.elidedTo(max_width));
      witm->setToolTip(COL_B_VAL, rec->value);
      if(chk_b) witm->setBackground(COL_B_FLG, del_color);
      witm->setBackground(COL_B_VAL, del_color);
      witm->setBackground(COL_B_NM, del_color);
      witm->setExpanded(false);
      if(!rec->HasDiffFlag(taObjDiffRec::SUB_NO_ACT)) {
        // only ta base items really feasible here..
        if(rec->type->InheritsFrom(&TA_taBase)) {
          witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
          witm->setCheckState(COL_A_FLG, Qt::Unchecked);
          witm->setCheckState(COL_B_FLG, Qt::Unchecked);
          witm->setText(COL_A_FLG, lbl_a);
          witm->setText(COL_B_FLG, lbl_b);
        }
      }
    }
    else if(rec->HasDiffFlag(taObjDiffRec::DIFF_CHG)) {
      witm->setText(COL_A_NM, rec->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_A_VAL, rec->value.elidedTo(max_width));
      witm->setToolTip(COL_A_VAL, rec->value);
      if(chk_a) witm->setBackground(COL_A_FLG, chg_color);
      witm->setBackground(COL_A_VAL, chg_color);
      witm->setBackground(COL_A_NM, chg_color);
      witm->setText(COL_B_NM, rec->diff_odr->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_B_VAL, rec->diff_odr->value.elidedTo(max_width));
      witm->setToolTip(COL_B_VAL, rec->diff_odr->value);
      if(chk_b) witm->setBackground(COL_B_FLG, chg_color);
      witm->setBackground(COL_B_VAL, chg_color);
      witm->setBackground(COL_B_NM, chg_color);

      witm->setFlags(witm->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
      witm->setCheckState(COL_A_FLG, Qt::Unchecked);
      witm->setCheckState(COL_B_FLG, Qt::Unchecked);
      witm->setText(COL_A_FLG, lbl_a);
      witm->setText(COL_B_FLG, lbl_b);
    }
    else if(rec->HasDiffFlag(taObjDiffRec::DIFF_PAR_A)) {
      witm->setText(COL_A_NM, rec->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_A_VAL, rec->value.elidedTo(max_width));
      witm->setExpanded(true);
      // not editable or checkable
    }
    else if(rec->HasDiffFlag(taObjDiffRec::DIFF_PAR_B)) {
      witm->setText(COL_B_NM, rec->GetDisplayName().elidedTo(max_width));
      witm->setText(COL_B_VAL, rec->value.elidedTo(max_width));
      witm->setExpanded(true);
      // not editable or checkable
    }
  }

  items->resizeColumnToContents(COL_NEST);
  items->resizeColumnToContents(COL_A_FLG);
  items->resizeColumnToContents(COL_A_NM);
  items->resizeColumnToContents(COL_A_VAL);
  items->resizeColumnToContents(COL_SEP);
  items->resizeColumnToContents(COL_B_FLG);
  items->resizeColumnToContents(COL_B_NM);
  items->resizeColumnToContents(COL_B_VAL);
}

void taiObjDiffBrowser::itemClicked(QTreeWidgetItem* itm, int column) {
  if(column != COL_A_FLG && column != COL_B_FLG) return;

  QVariant val = itm->data(0, Qt::UserRole+1);
  taObjDiffRec* rec = val.value<taObjDiffRec*>();
  if(!rec || !rec->ActionAllowed()) return;

  int a_or_b = 0;
  if(column == COL_B_FLG) a_or_b = 1;
  Qt::CheckState chkst = itm->checkState(column);
  bool on = (chkst == Qt::Checked);

  rec->SetCurAction(a_or_b, on);

  UpdateItemDisp(itm, rec, a_or_b);
}

void taiObjDiffBrowser::UpdateItemDisp(QTreeWidgetItem* itm, taObjDiffRec* rec,  int a_or_b) {
  QBrush add_color(Qt::green);
  QBrush del_color(Qt::red);
  QBrush chg_color(Qt::yellow);
  QBrush no_color;

  String lbl;
  bool chk = rec->GetCurAction(a_or_b, lbl);
  int column;
  if(a_or_b == 0) column = COL_A_FLG;
  else column = COL_B_FLG;

  if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL))
    itm->setBackground(column, chk ? (a_or_b ? add_color : del_color) : no_color);
  else if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD))
    itm->setBackground(column, chk ? (a_or_b ? del_color : add_color) : no_color);
  else if(rec->HasDiffFlag(taObjDiffRec::DIFF_CHG))
    itm->setBackground(column, chk ? chg_color : no_color);

  itm->setCheckState(column, chk ? Qt::Checked : Qt::Unchecked);
}

void taiObjDiffBrowser::ToggleAll(int a_or_b) {
  for(int i=0;i<odl->size; i++) {
    taObjDiffRec* rec = odl->FastEl(i);

    if(!rec->ActionAllowed()) continue;

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    String lbl;
    bool chk = rec->GetCurAction(a_or_b, lbl);
    rec->SetCurAction(a_or_b, !chk);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void taiObjDiffBrowser::toggleAllA() {
  ToggleAll(0);
}

void taiObjDiffBrowser::toggleAllB() {
  ToggleAll(1);
}

void taiObjDiffBrowser::SetFiltered(int a_or_b, bool on, bool add, bool del, bool chg,
                                    bool nm_not, String nm_contains, bool val_not,
                                    String val_contains) {
  for(int i=0;i<odl->size; i++) {
    taObjDiffRec* rec = odl->FastEl(i);

    if(!rec->ActionAllowed()) continue;

    if(a_or_b == 0) {
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD) && !add) continue;
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL) && !del) continue;
    }
    else {                      // switched for b
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_DEL) && !add) continue;
      if(rec->HasDiffFlag(taObjDiffRec::DIFF_ADD) && !del) continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::DIFF_CHG) && !chg) continue;
    if(nm_contains.nonempty()) {
      if(nm_not) {
        if(rec->name.contains(nm_contains)) continue;
      }
      else {
        if(!rec->name.contains(nm_contains)) continue;
      }
    }
    if(val_contains.nonempty()) {
      if(val_not) {
        if(rec->value.contains(val_contains)) continue;
      }
      else {
        if(!rec->value.contains(val_contains)) continue;
      }
    }

    QTreeWidgetItem* witm = (QTreeWidgetItem*)rec->widget;
    if(!witm) continue;         // shouldn't happen

    rec->SetCurAction(a_or_b, on);
    UpdateItemDisp(witm, rec, a_or_b);
  }
}

void taiObjDiffBrowser::setFilteredA() {
  if(filter_dlg->Browse()) {
    SetFiltered(0, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}

void taiObjDiffBrowser::setFilteredB() {
  if(filter_dlg->Browse()) {
    SetFiltered(1, filter_dlg->action_on, filter_dlg->add, filter_dlg->del,
                filter_dlg->chg, filter_dlg->nm_not, filter_dlg->nm_contains,
                filter_dlg->val_not, filter_dlg->val_contains);
  }
}

/////////////////////////////////////////////////////////////
//      taiObjDiffBrowserFilter

taiObjDiffBrowserFilter* taiObjDiffBrowserFilter::New(int font_type,
                                                      QWidget* par_window_) {
  taiObjDiffBrowserFilter* rval = new taiObjDiffBrowserFilter(par_window_);
  rval->setFont(taiM->dialogFont(font_type));
  rval->Constr();
  return rval;
}

taiObjDiffBrowserFilter::taiObjDiffBrowserFilter(QWidget* par_window_)
:inherited(par_window_)
{
  setModal(true);
  setWindowTitle("Set Diff Actions Filter Dialog");
  resize(taiM->dialogSize(taiMisc::hdlg_s));
}

taiObjDiffBrowserFilter::~taiObjDiffBrowserFilter() {
}

void taiObjDiffBrowserFilter::accept() {
  action_on = chkActionOn->isChecked();
  add = chkAdd->isChecked();
  del = chkDel->isChecked();
  chg = chkChg->isChecked();
  nm_not = chkNmNot->isChecked();
  val_not = chkValNot->isChecked();
  nm_contains = editNm->text();
  val_contains = editVal->text();
  inherited::accept();
}

void taiObjDiffBrowserFilter::reject() {
  inherited::reject();
}

bool taiObjDiffBrowserFilter::Browse() {
  return (exec() == iDialog::Accepted);
}

void taiObjDiffBrowserFilter::Constr() {
  action_on = true;
  add = del = chg = true;
  nm_not = val_not = false;

  layOuter = new QVBoxLayout(this);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);

  String lb_txt = "Select types of diffs records to change, and value to set\n";

  QLabel* lbl = new QLabel(lb_txt);
  layOuter->addWidget(lbl);
  layOuter->addSpacing(taiM->vsep_c);
  layOuter->addSpacing(taiM->vsep_c);

  chkActionOn = new QCheckBox("Action &On", this);
  chkActionOn->setToolTip("State to set for the action flag for diff items -- on or off");
  chkActionOn->setCheckState(Qt::Checked);
  layOuter->addWidget(chkActionOn);

  QHBoxLayout* lay = new QHBoxLayout();

  lbl = new QLabel("Diff types: ");
  lay->addWidget(lbl);

  chkAdd = new QCheckBox("&Add", this);
  chkAdd->setToolTip("Include Add Diffs");
  chkAdd->setCheckState(Qt::Checked);
  lay->addWidget(chkAdd);

  chkDel = new QCheckBox("&Del", this);
  chkDel->setToolTip("Include Del Diffs");
  chkDel->setCheckState(Qt::Checked);
  lay->addWidget(chkDel);

  chkChg = new QCheckBox("&Chg", this);
  chkChg->setToolTip("Include Chg Diffs");
  chkChg->setCheckState(Qt::Checked);
  lay->addWidget(chkChg);
  lay->addStretch();

  layOuter->addLayout(lay);

  lay = new QHBoxLayout();

  lbl = new QLabel("name contains:");
  lbl->setToolTip("Text that the name of diff object must contain (or not)");
  lay->addWidget(lbl);
  editNm = new iLineEdit(this);
  lay->addWidget(editNm);

  chkNmNot = new QCheckBox("not!", this);
  chkNmNot->setToolTip("changes name contains text to NOT contains");
  lay->addWidget(chkNmNot);

  layOuter->addLayout(lay);

  lay = new QHBoxLayout();

  lbl = new QLabel("value contains:");
  lbl->setToolTip("Text that the value of diff object must contain (or not)");
  lay->addWidget(lbl);
  editVal = new iLineEdit(this);
  lay->addWidget(editVal);

  chkValNot = new QCheckBox("not!", this);
  chkValNot->setToolTip("changes value contains text to NOT contains");
  lay->addWidget(chkValNot);

  layOuter->addLayout(lay);

  layOuter->addStretch();

  lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", this);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", this);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
}

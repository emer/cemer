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

#include "iDialogItemChooser.h"
#include <taiWidgetItemChooser>
#include <iLineEdit>
#include <iTreeWidget>

#include <taMisc>
#include <taiMisc>

#include <QPushButton>
#include <QLineEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QFrame>

#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
// defined in mac_objc_code.mm objective C file:
// per bug ticket: https://bugreports.qt-project.org/browse/QTBUG-38815
extern void TurnOffTouchEventsForWindow(QWindow* qtWindow);
#endif

const String iDialogItemChooser::cat_none("(none)");
int iDialogItemChooser::filt_delay = 500;

iDialogItemChooser* iDialogItemChooser::New(const String& caption_, taiWidgetItemChooser* client_,
  int ft, QWidget* par_window_)
{
/*no, let qt choose  if (par_window_ == NULL)
    par_window_ = taiMisc::main_window;*/
  iDialogItemChooser* rval = new iDialogItemChooser(caption_, par_window_);
  rval->setFont(taiM->dialogFont(ft));
  rval->Constr(client_);
  return rval;
}

iDialogItemChooser::iDialogItemChooser(const String& caption_, QWidget* par_window_)
:inherited(par_window_)
{
  init(caption_);
}

void iDialogItemChooser::init(const String& caption_) {
  init_filter = "^";
  m_changing = 0;
  m_fully_up = false;
  caption = caption_;
  multi_cats = false;
  m_selObj = NULL;
  m_selItem = NULL;
  m_client = NULL;
  m_view = -1; // until set to valid value
  m_cat_filter = 0; // default is all
  is_dialog = true; // default
  setModal(true);
  setWindowTitle(caption);
//  setFont(taiM->dialogFont(taiMisc::fonSmall));
  resize(taiM->dialogSize(taiMisc::hdlg_m));
}

void iDialogItemChooser::accept() {
  QString text = filter->text(); // apply current filter
  QTreeWidgetItem* itm = NULL;
  if(!text.isEmpty() && text != last_filter && !(last_filter.isEmpty() && text == "^")) {
    last_filter = text;
    QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::All);
    QTreeWidgetItem* item;
    while ((item = *it++)) {
      bool show = ShowItem(item);
      if(!show) continue;
      itm = item;
      break;
    }
  }
  else {
    itm = items->currentItem();
  }
  m_selItem = itm;

  if(is_dialog) {
    inherited::accept();  // Do this BEFORE the new choose dialog!!! jar 10/27/13
  }

  if(itm) {
    bool new_fun = itm->data(0, NewFunRole).toBool();
    if(new_fun) {
      taBase* new_base = (taBase*)QVARIANT_TO_INTPTR(itm->data(0,ObjDataRole));
      if(new_base) {
        taBase* nw = NULL;
        if(itm->text(0).contains(" CREATE")) { // special chooser guy
            nw = new_base->ChooseNew(m_client->Base());
        }
        else {
          nw = new_base->New(1);
        }
        if(nw) {
          m_selObj = nw;
        }
      }
    }
    else {
      m_selObj = (void*)QVARIANT_TO_INTPTR(itm->data(0,ObjDataRole));
    }
  }
  m_client = NULL;

  if(!is_dialog) {
    inherited::accept();        // this signals accepted() and we want it at the end..
  }
}

QTreeWidgetItem* iDialogItemChooser::AddItem(const QString& itm_cat,
                                             const QString& itm_txt,
                                             QTreeWidgetItem* parent, const void* data_,
                                             const String& desc, int desc_idx)
{
  QTreeWidgetItem* rval = AddItem(itm_txt, parent, data_, desc, desc_idx);
  if (!itm_cat.isEmpty())
    rval->setData(0, ObjCatRole, itm_cat);
  return rval;
}

QTreeWidgetItem* iDialogItemChooser::AddItem(const QString& itm_txt,
                                             QTreeWidgetItem* parent,
                                             const void* data_,
                                             const String& desc, int desc_idx)
{
  QTreeWidgetItem* rval;
  if (parent)
    rval = new QTreeWidgetItem(parent);
  else
    rval = new QTreeWidgetItem(items);
  // set standard item text
  rval->setText(0, itm_txt);
  // set the object, which is an extended attribute
  //note: use the ta version because Qt uses longs on some plats
  if (data_)
    rval->setData(0, ObjDataRole, QVariant((ta_intptr_t)data_));
  if(!desc.empty()) {
    rval->setText(desc_idx, desc);
    int ttwidth = taiM->scrn_chars.w / 2;
    rval->setData(desc_idx, Qt::ToolTipRole, desc.wrap(ttwidth));
  }
  return rval;
}

bool iDialogItemChooser::SetInitView(void* sel_val, const String& filt_str) {
  if(sel_val == NULL && filt_str.empty()) {
    setView(0, true);           // nothing..
    return false;
  }
  else if(filt_str.nonempty()) {
    last_filter = init_filter.chars();
    for(int vw = 0; vw < m_client->viewCount(); vw++) {
      setView(vw, true);
//       taMisc::ProcessEvents();
      int n_items = ApplyFiltering();
      if(n_items > 0) {
        return true;            // stop when get any hits
      }
    }
  }
  else if(sel_val != NULL) {
    for(int vw = 0; vw < m_client->viewCount(); vw++) {
      setView(vw, true);
//       taMisc::ProcessEvents();
      if(SetCurrentItemByData(sel_val)) {
        return true;            // stop when get any hits
      }
    }
  }
  setView(0, true);             // go back to start..
  ApplyFiltering();
  return false;
}

int iDialogItemChooser::ApplyFiltering() {
  ++m_changing;
  taMisc::Busy();
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item;
  QString s;
  QTreeWidgetItem* first_item = NULL;
  int n_items = 0;
  while ((item = *it)) {
    bool show = ShowItem(item);
    if(!show) item->setSelected(false); // maybe help with selection problems
    items->setItemHidden(item, !show);
    if(show) {
      if(n_items == 0)
        first_item = item;
      n_items++;
    }
    ++it;
  }
  taMisc::DoneBusy();
  if(first_item)
    SelectItem(first_item, true); // auto-select first item always
  --m_changing;
  return n_items;
}

bool iDialogItemChooser::Choose(taiWidgetItemChooser* client_) {
  // NOTE: current semantics is simple: always rebuild each show
  // more complex caching would require flags etc. to track whether the
  // inputs to the item changed -- cpus are so fast now, this may not be worth it
  Activate(client_);
  is_dialog = true;             // yep we're a dialog now..
  return (exec() == iDialog::Accepted);
}

void iDialogItemChooser::Activate(taiWidgetItemChooser* client_) {
  m_client = client_;
  if(client_->filter_start_txt.nonempty())
    init_filter = String("^") + client_->filter_start_txt;
  else
    init_filter = "^";
  SetInitView(m_client->sel(), client_->filter_start_txt);
  setSelObj(m_client->sel());
  is_dialog = false;            // if we only get this far, then it is not a dialog
}

void iDialogItemChooser::Clear() {
  timFilter->stop();
  items->clear();
  m_selObj = NULL;
  m_selItem = NULL;
}

void iDialogItemChooser::ClearFilter() {
  ++m_changing;
  taMisc::Busy();
  last_filter.clear();
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::Hidden);
  QTreeWidgetItem* item;
  while ((item = *it)) {
    items->setItemHidden(item, false);
    ++it;
  }
  taMisc::DoneBusy();
  --m_changing;
}

void iDialogItemChooser::cmbView_currentIndexChanged(int index) {
  if (m_changing > 0) return;
  setView(index);
}

void iDialogItemChooser::cmbCat_currentIndexChanged(int index) {
  if (m_changing > 0) return;
  setCatFilter(index);
}

void iDialogItemChooser::Constr(taiWidgetItemChooser* client_) {
  m_fully_up = false;
  m_client = client_; // revoked at end

  QVBoxLayout* blay = new QVBoxLayout(this);
  blay->setMargin(0);
  blay->setSpacing(0);
  body = new QFrame(this);
  body->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);
  blay->addWidget(body);

  layOuter = new QVBoxLayout(body);
  layOuter->setMargin(taiM->vsep_c);
  layOuter->setSpacing(taiM->vspc_c);
  QHBoxLayout* layFilter = NULL; // only if needed
  QLabel* lbl = NULL;

  // we only put a cats selector if there are cats
  if (client_->catCount() > 0) {
    if (!layFilter) {
      layFilter = new QHBoxLayout(); layFilter->setMargin(0); // sp ok
    }
    lbl = new QLabel("category", body);
    layFilter->addWidget(lbl);
    cmbCat = new QComboBox(body);
    cmbCat->addItem("(all)");
    String s;
    for (int i = 0; i < client_->catCount(); ++i) {
      s = client_->catText(i);
      if (s.empty()) s = cat_none;
      cmbCat->addItem(s);
    }
    layFilter->addWidget(cmbCat, 1);
    connect(cmbCat, SIGNAL(currentIndexChanged(int)),
      this, SLOT(cmbCat_currentIndexChanged(int)) );
  } else cmbCat = NULL;

  // we only put up a view selector if more than 1 view supported
  if (client_->viewCount() > 1) {
    if (!layFilter) {
      layFilter = new QHBoxLayout(); layFilter->setMargin(0); // sp ok
    }
    lbl = new QLabel("view", body);
    layFilter->addWidget(lbl);
    cmbView = new QComboBox(body);
    for (int i = 0; i < client_->viewCount(); ++i) {
      cmbView->addItem(client_->viewText(i));
    }
    layFilter->addWidget(cmbView, 1);
    connect(cmbView, SIGNAL(currentIndexChanged(int)),
      this, SLOT(cmbView_currentIndexChanged(int)) );
  } else cmbView = NULL;
  if (layFilter) layOuter->addLayout(layFilter);

  QHBoxLayout* lay = new QHBoxLayout();
  lay->addSpacing(taiM->hspc_c);
  lbl = new QLabel("search", body);
  lbl->setToolTip("Search for items that contain this text, showing only them -- if starts with a ^ then only look for items in the first column that start with the text after the ^");
  lay->addWidget(lbl);
  lay->addSpacing(taiM->vsep_c);
  filter = new iLineEdit(body);
  filter->setToolTip(lbl->toolTip());
  lay->addWidget(filter, 1);
  lay->addSpacing(taiM->hspc_c);
  layOuter->addLayout(lay);

  items = new iTreeWidget(body);
  // items->setSortingEnabled(true);
  items->setAllColumnsShowFocus(true);
  items->setSelectionMode(QAbstractItemView::SingleSelection);
  items->setSelectionBehavior(QAbstractItemView::SelectRows);
  layOuter->addWidget(items, 1); // list is item to expand in host

  lay = new QHBoxLayout();
  lay->addStretch();
  btnOk = new QPushButton("&Ok", body);
  btnOk->setDefault(true);
  lay->addWidget(btnOk);
  lay->addSpacing(taiM->vsep_c);
  btnCancel = new QPushButton("&Cancel", body);
  lay->addWidget(btnCancel);
  layOuter->addLayout(lay);

  timFilter = new QTimer(this);
  timFilter->setSingleShot(true);
  timFilter->setInterval(filt_delay);

  connect(btnOk, SIGNAL(clicked()), this, SLOT(accept()) );
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()) );
  connect(items, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
      this, SLOT(items_itemDoubleClicked(QTreeWidgetItem*, int)) );
  connect(filter, SIGNAL(textChanged(const QString&)),
    this, SLOT(filter_textChanged(const QString&)) );
  connect(timFilter, SIGNAL(timeout()), this, SLOT(timFilter_timeout()) );

  m_client = NULL;
  filter->setFocus();
}

void iDialogItemChooser::items_itemDoubleClicked(QTreeWidgetItem* itm, int col) {
  accept();
}

void iDialogItemChooser::filter_textChanged(const QString& /*text*/) {
  // following either starts timer, or restarts it
  timFilter->start();
}

void iDialogItemChooser::Refresh() {
  ++m_changing;
  // delete items
  Clear();
  if (m_client) { // note: should be valid
    // redo header
    int cols = m_client->columnCount(m_view); // cache
    items->setColumnCount(cols);
    QTreeWidgetItem* hi = items->headerItem();
    for (int i = 0; i < cols; ++i) {
      hi->setData(i, Qt::DisplayRole, m_client->headerText(i, m_view));
    }
    // call client to fill items
    m_client->BuildChooser(this, m_view);
    // make columns nice sizes (not last)
    for (int i = 0; i < (cols - 1); ++i) {
      items->resizeColumnToContents(i);
    }
    // set current item; if NO NULL and none, then first item by default
    void* tsel = m_client->sel();
    if (!tsel) {
      if(items->topLevelItemCount() > 0) {  // if NULL ok - first choice when no current selection
        if(m_client->HasFlag(taiWidget::flgNullOk))
          SetCurrentItemByData(NULL);
      }
      else if(items->topLevelItemCount() > 1) {
        tsel = (void*)QVARIANT_TO_INTPTR(items->topLevelItem(1)->data(0, ObjDataRole));
      }
      else if(items->topLevelItemCount() > 0) {
        if(!m_client->HasFlag(taiWidget::flgNullOk)) {
          tsel = (void*)QVARIANT_TO_INTPTR(items->topLevelItem(0)->data(0, ObjDataRole));
        }
      }
    }
    m_client->GetImage(tsel, NULL); // no targ type
  }
  --m_changing;
}

void iDialogItemChooser::reject() {
//TODO: maybe shouldn't nuke sel, caller should check true/false
  m_selObj = NULL;
  m_client = NULL;
  inherited::reject();
}

void iDialogItemChooser::setCatFilter(int value, bool force) {
  if ((m_cat_filter == value) && !force) return;
  m_cat_filter = value; //so its valid for any subcalls, etc.
  ApplyFiltering();
}

bool iDialogItemChooser::SetCurrentItemByData(void* value) {
  QTreeWidgetItemIterator it(items, QTreeWidgetItemIterator::Selectable);
  QTreeWidgetItem* item;
  bool is_first = true;
  while ((item = *it)) {
    void* data = (void*)QVARIANT_TO_INTPTR(item->data(0, ObjDataRole));
    if (value == data) {  // also works for NULL choice
      SelectItem(item, is_first);
      m_selItem = item; // cache for showEvent
      return true;
    }
    is_first = false;
    ++it;
  }
  // not found
  items->selectionModel()->clear();
  m_selItem = NULL;
  return false;
}

bool iDialogItemChooser::ShowItem(const QTreeWidgetItem* item) const {
  // we show the item unless it either doesn't meet filter criteria, or not in cat

  // category filter
  if (m_cat_filter != 0) {
    String act_cat = item->data(0, ObjCatRole).toString(); //s/b blank if none set
    String cat_txt = client()->catText(m_cat_filter - 1);// subtract 1 for 'all' item
    if(multi_cats && act_cat.contains(", ")) {
      bool any_match = false;
      while(true) {
        String cur_cat = act_cat.before(", ");
        act_cat = act_cat.after(", ");
        if(cur_cat == cat_txt) {
          any_match = true; break;
        }
        if(act_cat.contains(", ")) continue;
        if(act_cat == cat_txt) {
          any_match = true;
        }
        break;
      }
      if(!any_match) return false;
    }
    else {
      if (act_cat != cat_txt)
        return false;
    }
  }

  // filter text filter
  if (!last_filter.isEmpty()) {
    bool hide = true;
    if(last_filter.startsWith("^")) { // special start of string search (on by default)
      QString flt = last_filter.mid(1); // skip ^
      if(flt.isEmpty()) return true;    // nothing after it!
      QString s = item->text(0);        // only 1st col
      if(s.startsWith(flt, Qt::CaseInsensitive)) {
        hide = false;
      }
      if (hide) return false;
    }
    else {
      QString s;
      int cols = items->columnCount();
      for (int i = 0; i < cols; ++i) {
        s = item->text(i);
        if (s.contains(last_filter, Qt::CaseInsensitive)) {
          hide = false;
          break;
        }
      }
      if (hide) return false;
    }
  }
  return true;
}

void iDialogItemChooser::SetFilter(const QString& filt) {
  last_filter = filt;
  ApplyFiltering();
}

void iDialogItemChooser::setSelObj(void* value, bool force) {
  if ((m_selObj == value) && !force) return;
  m_selObj = value;
  SetCurrentItemByData(value);
}

void iDialogItemChooser::setView(int value, bool force) {
  if ((m_view == value) && !force) return;
  m_view = value; //so its valid for any subcalls, etc.
  if (cmbView) {
    cmbView->setCurrentIndex(value);
  }
  Refresh();
  QString text = filter->text();
  if (!text.isEmpty()) SetFilter(text);
}

void iDialogItemChooser::SelectItem(QTreeWidgetItem* itm, bool is_first) {
  // some kind of completely crazy hack needed to get this thing to select
  // totally pulling my hair out about this!!!!
  // problem seems to be when items are hidden -- only happens when the filter
  // is in place -- oh well, couldn't find a better workaround..
  items->selectItem(itm);
  QModelIndex idx = items->indexFromItem(itm);
  QCoreApplication* app = QCoreApplication::instance();
  app->postEvent(items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
  if(!is_first) {
    app->postEvent(items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
                                        Qt::NoModifier));
  }
  items->scrollToItem(itm);
}


void iDialogItemChooser::showEvent(QShowEvent* event) {
  inherited::showEvent(event);
  if(init_filter != "^") {
    filter->insert(init_filter);
    items->setFocus();
    if(m_selItem) {
      SelectItem(m_selItem, true); // is first in this case
    }
  }
  QTimer::singleShot(150, this, SLOT(show_timeout()) );
  m_fully_up = true;
}

void iDialogItemChooser::show_timeout() {
#if defined(TA_OS_MAC) && (QT_VERSION >= 0x050200)
  // needs to be after window is fully up and running..
  TurnOffTouchEventsForWindow(windowHandle());
#endif
  if(m_selItem) {
    items->scrollToItem(m_selItem);
  }
}

void iDialogItemChooser::timFilter_timeout() {
  // if nothing has changed in text, do nothing
  QString text = filter->text();
  if (last_filter == text || (last_filter.isEmpty() && text == "^")) return;
  // if we are already filtering, then don't reenter, but just try again
  if (m_changing > 0) {
    timFilter->start();
    return;
  }
  if (text.isEmpty()) ClearFilter();
  else SetFilter(text);
}


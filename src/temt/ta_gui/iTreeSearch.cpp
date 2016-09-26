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

#include "iTreeSearch.h"
#include <taString>
#include <taBase_PtrList>
#include <taiSigLink>
#include <String_Array>
#include <taProject>
#include <iTreeView>
#include <iTreeViewItem>
#include <iLineEdit>
#include <iMenuButton>

#include <taMisc>
#include <taiMisc>

#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QToolButton>

void iTreeSearch::Constr() {
  search_mode = TEXT;  //  default
  taiMisc::SizeSpec font_size = taiMisc::sizSmall;
  
  QHBoxLayout* lay = new QHBoxLayout(this);
  srch_bar = new QToolBar(this);
  lay->addWidget(srch_bar);
  lay->setMargin(0);
  lay->setSpacing(0);
  
  srch_mode_button = new QToolButton();
  srch_mode_button->setPopupMode(QToolButton::MenuButtonPopup);
  srch_bar->addWidget(srch_mode_button);
  
  find_action = new QAction("Find/Replace", this);
  find_deep_action = new QAction("Find Deep", this);
  
  srch_mode_menu = new QMenu(this);
  srch_mode_button->setMenu(srch_mode_menu);
  srch_mode_button->setDefaultAction(find_action);
  srch_mode_button->setFont(taiM->nameFont(font_size));
  srch_mode_button->setToolTip(taiMisc::ToolTipPreProcess("Find/Replace or Find Deep - the first mode only searches the text of the tree items - the deep search also looks at the object name, value, description, members and so on. In most cases the former is probably what you want. Find is case sensitive if the search string has any uppercase letter, otherwise case insensitive. If multiple strings are entered find only matches if all strings are found. To replace strings as you step through the found items use \"R>\" - does not work for Find Deep"));
  
  srch_text = new iLineEdit();
  srch_bar->addWidget(srch_text);
  repl_text = new iLineEdit();
  srch_bar->addWidget(repl_text);
  
  srch_nfound = new QLabel(" 0");
  srch_nfound->setFont(taiM->nameFont(font_size));
  srch_nfound->setToolTip(taiMisc::ToolTipPreProcess("Number of items found"));
  srch_bar->addWidget(srch_nfound);
  
  srch_clear = srch_bar->addAction("x");
  srch_clear->setToolTip(taiMisc::ToolTipPreProcess("Clear search text and highlighting"));
  srch_prev = srch_bar->addAction("<");
  srch_prev->setToolTip(taiMisc::ToolTipPreProcess("Find previous occurrence of find text within browser"));
  srch_next = srch_bar->addAction(">");
  srch_next->setToolTip(taiMisc::ToolTipPreProcess("Find next occurrence of find text within browser"));
  repl_next = srch_bar->addAction("R>");
  repl_next->setToolTip(taiMisc::ToolTipPreProcess("Replace current selection, then find next occurrence"));
  
  srch_mode_menu->addAction(find_action);
  srch_mode_menu->addAction(find_deep_action);
  
  connect(srch_clear, SIGNAL(triggered()), this, SLOT(srch_clear_clicked()) );
  connect(srch_prev, SIGNAL(triggered()), this, SLOT(srch_prev_clicked()) );
  connect(srch_next, SIGNAL(triggered()), this, SLOT(srch_next_clicked()) );
  connect(repl_next, SIGNAL(triggered()), this, SLOT(repl_next_clicked()) );
  connect(srch_text, SIGNAL(returnPressed()), this, SLOT(srch_text_entered()) );
  connect(repl_text, SIGNAL(returnPressed()), this, SLOT(srch_text_entered()) );
  connect(tree_view, SIGNAL(TreeStructToUpdate()), this, SLOT(treeview_to_updt()) );
  
  connect(find_action, SIGNAL(triggered()), this, SLOT(TextFindSelected()));
  connect(find_deep_action, SIGNAL(triggered()), this, SLOT(DeepFindSelected()));
}

iTreeSearch::iTreeSearch(QWidget* parent) : QWidget(parent) {
  tree_view = NULL;
  Constr();
}

iTreeSearch::iTreeSearch(iTreeView* tree_view_, QWidget* parent)  : QWidget(parent) {
  tree_view = tree_view_;
  Constr();
}

iTreeSearch::~iTreeSearch() {
  
}

void iTreeSearch::Search(iTreeSearch::SearchMode mode) {
  unHighlightFound();
  srch_nfound->setText("0");
  srch_found.clear();
  found_items.Reset();
  if(!tree_view) {
    return;
  }
  String ftxt = srch_text->text();
  ftxt.trim();
  if (ftxt.empty()) {
    return; // nothing to search for - bye!
  }
  bool case_sens = ftxt.HasUpper();
  if(!case_sens) {
    ftxt.downcase();
  }
  String_Array srch;
  srch.Split(ftxt, " ");
  
  bool text_only = (mode == iTreeSearch::TEXT);
  QTreeWidgetItemIterator it(tree_view, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item_;
  taBase_PtrList sub_srch;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if(!item) { ++it; continue; }
    taiSigLink* dl = item->link();
    if(!dl || !dl->isBase()) { ++it; continue; }
    taBase* tab = (taBase*)dl->data();
    MemberDef* md = item->md();
    if(md) {
      if(md->HasOption("READ_ONLY") || md->HasOption("HIDDEN") || md->HasOption("HIDDEN_TREE")
         || md->HasOption("NO_FIND") || md->is_static || md->HasOption("EXPERT")) {
        ++it; continue;
      }
    }
    if(!tree_view->isItemExpanded(item) && !item->ChildrenCreated()) {
      // do full recursive search if not already expanded
      sub_srch.Reset();
      tab->Search(ftxt, sub_srch, NULL, text_only, true, case_sens); // go with defaults for now
      for(int k=0; k<sub_srch.size; k++) { // transfer to our ref list
        taBase* fnd = sub_srch.FastEl(k);
        found_items.Add(fnd);
      }
      sub_srch.Reset();
    }
    else if(tab->SearchTestItem_impl(srch, text_only, true, case_sens)) {       // otherwise just test this one item
      srch_found.append(item);
      found_items.Add(tab);
    }
    ++it;
  }
  
  // only highlight for small number of hits
  if(found_items.size < 25 && found_items.size > srch_found.count()) {
    for(int i=0;i<found_items.size; i++) {
      taBase* fnd = found_items.FastEl(i);
      taiSigLink* lnk = (taiSigLink*)fnd->GetSigLink();
      if(!lnk) continue;
      iTreeViewItem* fitm = tree_view->AssertItem(lnk);
      if(!fitm) continue;
      if(srch_found.indexOf(fitm) < 0) {
        srch_found.append(fitm);
      }
    }
  }
  
  highlightFound();
  cur_item = 0;
  selectCurrent();
}

void iTreeSearch::highlightFound() {
  srch_nfound->setText(String((int)found_items.size));
  for(int i=0; i< srch_found.count(); i++) {
    iTreeViewItem* itm = srch_found.at(i);
    itm->setBackgroundColor(Qt::yellow);
  }
}

void iTreeSearch::unHighlightFound() {
  for(int i=0; i< srch_found.count(); i++) {
    iTreeViewItem* itm = srch_found.at(i);
    itm->resetBackgroundColor();
  }
}

void iTreeSearch::selectCurrent(bool replace) {
  if(!tree_view)
    return;
  if(cur_item < 0 || cur_item >= found_items.size)
    return;
  
  srch_nfound->setText(String((int)found_items.size));
  
  taBase* fnd = found_items.FastEl(cur_item);
  if(!fnd) return;
  taiSigLink* lnk = (taiSigLink*)fnd->GetSigLink();
  if(!lnk) return;
  iTreeViewItem* fitm = tree_view->AssertItem(lnk);
  if(!fitm) return;             // todo: do something more sensible?
  if(srch_found.indexOf(fitm) < 0) {
    srch_found.append(fitm);
  }
  
  fitm->setBackgroundColor(Qt::yellow);
  tree_view->setFocus();
  tree_view->clearExtSelection();
  tree_view->scrollTo(fitm);
  tree_view->setCurrentItem(fitm, 0, QItemSelectionModel::ClearAndSelect);
  
}

void iTreeSearch::srch_text_entered() {
  Search(search_mode);
}

void iTreeSearch::srch_clear_clicked() {
  unHighlightFound();
  srch_nfound->setText("0");
  srch_found.clear();
  found_items.Reset();
  srch_text->setText("");
  repl_text->setText("");
}

void iTreeSearch::treeview_to_updt() {
  srch_found.clear();           // keep found items, just clear the tree view guys
}

void iTreeSearch::srch_next_clicked() {
  cur_item++;
  if(cur_item >= found_items.size)
    cur_item = found_items.size-1;
  selectCurrent();
}

void iTreeSearch::repl_next_clicked() {
  if (found_items.size > 0) {
    taBase* fnd = found_items.FastEl(cur_item);
    if(fnd) {
      taiSigLink* dl = (taiSigLink*)fnd->GetSigLink();
      if (dl) {
        taProject* proj = tree_view->curProject();
        if(proj) {
          proj->undo_mgr.SaveUndo(dl->taData(), "Replace", NULL, false, proj);
        }
        String srch_string = static_cast<String>(srch_text->text());
        String repl_string = static_cast<String>(repl_text->text());
        bool replace_deep = false;
        dl->taData()->ReplaceValStr(srch_string, repl_string, "", NULL, NULL, NULL, TypeDef::SC_DEFAULT, replace_deep);
      }
    }
  }
  srch_next_clicked();
}

void iTreeSearch::srch_prev_clicked() {
  cur_item--;
  if(cur_item < 0)
    cur_item = 0;
  selectCurrent();
}

void iTreeSearch::TextFindSelected() {
  search_mode = TEXT;
  repl_text->setEnabled(true);
  repl_next->setEnabled(true);  // R>
}

void iTreeSearch::DeepFindSelected() {
  search_mode = DEEP;
  repl_text->setEnabled(false);
  repl_next->setEnabled(false);  // R>
}




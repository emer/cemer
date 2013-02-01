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
#include <iLineEdit>
#include <taString>
#include <iTreeView>
#include <iTreeViewItem>
#include <taBase_PtrList>
#include <taiSigLink>


#include <QHBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>


void iTreeSearch::Constr() {
  QHBoxLayout* lay = new QHBoxLayout(this);
  srch_bar = new QToolBar(this);
  lay->addWidget(srch_bar);
  lay->setMargin(0);
  lay->setSpacing(0);

  srch_label = new QLabel("find:");
  srch_label->setToolTip("Find text within the above browser");
  srch_bar->addWidget(srch_label);
  srch_text = new iLineEdit();
  // srch_text->setCharWidth(16);
  srch_bar->addWidget(srch_text);
  srch_nfound = new QLabel(" 0");
  srch_nfound->setToolTip("number of items found");
  srch_bar->addWidget(srch_nfound);
  srch_clear = srch_bar->addAction("x");
  srch_clear->setToolTip("Clear srch text and reset any prior highlighting");
  srch_prev = srch_bar->addAction("<");
  srch_prev->setToolTip("Find previous occurrence of find text within browser");
  srch_next = srch_bar->addAction(">");
  srch_next->setToolTip("Srch next occurrence of find text within browser");

  connect(srch_clear, SIGNAL(triggered()), this, SLOT(srch_clear_clicked()) );
  connect(srch_next, SIGNAL(triggered()), this, SLOT(srch_next_clicked()) );
  connect(srch_prev, SIGNAL(triggered()), this, SLOT(srch_prev_clicked()) );
  connect(srch_text, SIGNAL(returnPressed()), this, SLOT(srch_text_entered()) );
  connect(tree_view, SIGNAL(TreeStructToUpdate()), this, SLOT(treeview_to_updt()) );
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

void iTreeSearch::search() {
  unHighlightFound();
  srch_found.clear();
  if(!tree_view) return;
  String ftxt = srch_text->text();
  QTreeWidgetItemIterator it(tree_view, QTreeWidgetItemIterator::All);
  QTreeWidgetItem* item_;
  taBase_PtrList sub_srch;
  taBase_PtrList sub_srch_own;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      int cols = item->columnCount();
      for(int i=0;i<cols;i++) {
        String str = item->text(i);
        if(str.contains_ci(ftxt)) {
          srch_found.append(item);
          break;                // out of for -- all done
        }
      }
      if(!tree_view->isItemExpanded(item)) {
        taiSigLink* dl = item->link();
        if(dl->isBase()) {
          taBase* tab = (taBase*)dl->data();
          tab->Search(ftxt, sub_srch, &sub_srch_own); // go with defaults for now
        }
      }
    }
    ++it;
  }

  // expand afterward because otherwise it messes up iterator
  for(int i=0;i<sub_srch.size; i++) {
    taBase* fnd = sub_srch.FastEl(i);
    taiSigLink* lnk = (taiSigLink*)fnd->GetSigLink();
    if(!lnk) continue;
    iTreeViewItem* fitm = tree_view->AssertItem(lnk);
    if(fitm) {
      srch_found.append(fitm);
    }
  }

  highlightFound();
  cur_item = 0;
  selectCurrent();
}

void iTreeSearch::highlightFound() {
  srch_nfound->setText(String((int)srch_found.count()));
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

void iTreeSearch::selectCurrent() {
  if(!tree_view) return;
  if(cur_item < 0 || cur_item >= srch_found.count())
    return;
  iTreeViewItem* itm = srch_found.at(cur_item);
  tree_view->setFocus();
  tree_view->clearExtSelection();
  tree_view->scrollTo(itm);
  tree_view->setCurrentItem(itm, 0, QItemSelectionModel::ClearAndSelect);
}

void iTreeSearch::srch_text_entered() {
  search();
}

void iTreeSearch::srch_clear_clicked() {
  unHighlightFound();
  srch_nfound->setText("0");
  srch_found.clear();
  srch_text->setText("");
}

void iTreeSearch::treeview_to_updt() {
  unHighlightFound();
  srch_nfound->setText("0");
  srch_found.clear();
  //  srch_text->setText(""); -- keep this in case people want to re-search
}

void iTreeSearch::srch_next_clicked() {
  cur_item++;
  if(cur_item >= srch_found.count())
    cur_item = srch_found.count()-1;
  selectCurrent();
}

void iTreeSearch::srch_prev_clicked() {
  cur_item--;
  if(cur_item < 0)
    cur_item = 0;
  selectCurrent();
}



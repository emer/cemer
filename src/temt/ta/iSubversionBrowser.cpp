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

#include "iSubversionBrowser.h"

#include <iSvnFileListModel>
#include <iLineEdit>
#include <iTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <iMainWindowViewer>
#include <QToolBar>
#include <iSpinBox>
#include <QFrame>
#include <QSortFilterProxyModel>

#include <taMisc>
#include <taiMisc>

iSubversionBrowser::iSubversionBrowser(QWidget* parent)
:inherited(taiMisc::main_window)
{
  int font_spec = taiMisc::fonMedium;
  this->setWindowTitle("Subversion Browser");

  svn_file_model = new iSvnFileListModel(this);

  QFrame* body = new QFrame(this);
  body->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  QVBoxLayout* lay_body = new QVBoxLayout(body);
  lay_body->setMargin(0);

  QToolBar* tool_bar = new QToolBar(body);
  lay_body->addWidget(tool_bar);
  
  QLabel* lbl = new QLabel("svn repo url:");
  lbl->setToolTip("repository url");
  tool_bar->addWidget(lbl);

  url_text = new iLineEdit(body);
  tool_bar->addWidget(url_text);

  tool_bar = new QToolBar(body);
  lay_body->addWidget(tool_bar);

  lbl = new QLabel(" working copy:");
  lbl->setToolTip("path to the working copy where repository is checked out");
  tool_bar->addWidget(lbl);

  wc_text = new iLineEdit(body);
  tool_bar->addWidget(wc_text);

  tool_bar = new QToolBar(body);
  lay_body->addWidget(tool_bar);

  lbl = new QLabel(" subdir:");
  lbl->setToolTip("current subdirectory path within repository");
  tool_bar->addWidget(lbl);

  subdir_text = new iLineEdit(body);
  tool_bar->addWidget(subdir_text);

  tool_bar = new QToolBar(body);
  lay_body->addWidget(tool_bar);

  lbl = new QLabel(" rev:");
  lbl->setToolTip("current revision to operate on -- use -1 for head (current)");
  tool_bar->addWidget(lbl);

  rev_box = new iSpinBox(body);
  rev_box->setMinimum(-10000);
  rev_box->setValue(-1);
  tool_bar->addWidget(rev_box);

  actGo = tool_bar->addAction("Go");

  lay_body->addSpacing(taiM->hsep_c);

  file_table = new iTableView(body);

  svn_file_sort = new QSortFilterProxyModel(this);
  svn_file_sort->setSourceModel(svn_file_model);

  file_table->setModel(svn_file_sort);
  lay_body->addWidget(file_table);

  QHeaderView *header = file_table->horizontalHeader();

  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);
  header->setSortIndicator(0, Qt::AscendingOrder);

  // Resize the index column to fit the contents.
  for(int i=0; i<5; i++) {
#if (QT_VERSION >= 0x050000)
    header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
    header->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
  }
  file_table->setSortingEnabled(true);

  // Disallow complex selections.
  file_table->setSelectionMode(QAbstractItemView::SingleSelection);

  // Keep the scrollbar visible at all times since the combo-boxes are
  // sized assuming it is present.  Hopefully this works on all platforms.
  file_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  // Don't need to see the vertical headers since we have an index column.
  file_table->verticalHeader()->setVisible(false);
  setCentralWidget(body);

  connect(url_text, SIGNAL(returnPressed()), this, SLOT(goClicked()) );
  connect(wc_text, SIGNAL(returnPressed()), this, SLOT(goClicked()) );
  connect(subdir_text, SIGNAL(returnPressed()), this, SLOT(goClicked()) );
  connect(rev_box, SIGNAL(valueChanged(int)), this, SLOT(goClicked()) );
  connect(actGo, SIGNAL(triggered()), this, SLOT(goClicked()) );
  connect(file_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(fileCellDoubleClicked(const QModelIndex&)));


  iSize sz = taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer);
  resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
}

iSubversionBrowser::~iSubversionBrowser() {
  
}

void iSubversionBrowser::updateView() {
  file_table->resizeColumnsToContents();
}

void iSubversionBrowser::setUrl(const String& url) {
  url_text->setText(url);
  svn_file_model->setUrl(url);
  updateView();
}

void iSubversionBrowser::setWCPath(const String& wc_path) {
  wc_text->setText(wc_path);
  svn_file_model->setWCPath(wc_path);
  updateView();
}

void iSubversionBrowser::setSubDir(const String& path) {
  subdir_text->setText(path);
  svn_file_model->setSubDir(path);
  updateView();
}

void iSubversionBrowser::setRev(int rev) {
  rev_box->setValue(rev);
  svn_file_model->setRev(rev);
  updateView();
}

void iSubversionBrowser::setUrlWCPath(const String& url, const String& wc_path, int rev) {
  url_text->setText(url);
  wc_text->setText(wc_path);
  rev_box->setValue(rev);
  svn_file_model->setUrlWCPath(url, wc_path, rev);
  updateView();
}

void iSubversionBrowser::setUrlWCPathSubDir(const String& url, const String& wc_path,
      const String& subdir, int rev) {
  url_text->setText(url);
  wc_text->setText(wc_path);
  subdir_text->setText(subdir);
  rev_box->setValue(rev);
  svn_file_model->setUrlWCPath(url, wc_path, rev);
  svn_file_model->setSubDir(subdir); // should have one fun for this..
  updateView();
}

void iSubversionBrowser::goClicked() {
  String urltxt = url_text->text();
  if(urltxt.empty()) {
    taMisc::Error("url is empty -- must specify a valid url");
    return;
  }
  String wctxt = wc_text->text();
  String subtxt = subdir_text->text();
  int rev = rev_box->value();
  setUrlWCPathSubDir(urltxt, wctxt, subtxt, rev);
}

void iSubversionBrowser::fileCellDoubleClicked(const QModelIndex& index) {
  QVariant qfn = svn_file_sort->data(index);
  String fnm = qfn.toString();
  if(fnm.empty() || fnm == ".") return;
  String subtxt = subdir_text->text();
  if(fnm == "..") {
    if(subtxt.empty()) return;
    subtxt = taMisc::GetDirFmPath(subtxt);
  }
  else {
    if(subtxt.nonempty())
      subtxt += "/";
    subtxt += fnm;
  }
  subtxt = taMisc::NoFinalPathSep(subtxt);
  setSubDir(subtxt);
}

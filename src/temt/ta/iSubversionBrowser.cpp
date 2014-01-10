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

#include <iSubversionModel>
#include <iLineEdit>
#include <iTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <iMainWindowViewer>
#include <QToolBar>
#include <iSpinBox>
#include <QFrame>

#include <taMisc>
#include <taiMisc>

iSubversionBrowser::iSubversionBrowser(QWidget* parent)
:inherited(taiMisc::main_window)
{
  int font_spec = taiMisc::fonMedium;
  this->setWindowTitle("Subversion Browser");

  svn_model = new iSubversionModel(this);

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
  file_table->setModel(svn_model);
  file_table->setSortingEnabled(true);
  lay_body->addWidget(file_table);

  QHeaderView *header = file_table->horizontalHeader();

  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);

  // Resize the index column to fit the contents.
  for(int i=0; i<5; i++) {
#if (QT_VERSION >= 0x050000)
    header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
    header->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
  }
  // Disallow complex selections.
  file_table->setSelectionMode(QAbstractItemView::SingleSelection);

  // Keep the scrollbar visible at all times since the combo-boxes are
  // sized assuming it is present.  Hopefully this works on all platforms.
  file_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  // Don't need to see the vertical headers since we have an index column.
  file_table->verticalHeader()->setVisible(false);
  setCentralWidget(body);

  connect(url_text, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(wc_text, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(subdir_text, SIGNAL(returnPressed()), this, SLOT(go_clicked()) );
  connect(rev_box, SIGNAL(valueChanged(int)), this, SLOT(go_clicked()) );
  connect(actGo, SIGNAL(triggered()), this, SLOT(go_clicked()) );

  iSize sz = taiM->dialogSize(taiMisc::dlgMedium | taiMisc::dlgVer);
  resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
}

iSubversionBrowser::~iSubversionBrowser() {
  
}

void iSubversionBrowser::setUrl(const String& url) {
  url_text->setText(url);
  svn_model->setUrl(url);
}

void iSubversionBrowser::setWCPath(const String& wc_path) {
  wc_text->setText(wc_path);
  svn_model->setWCPath(wc_path);
}

void iSubversionBrowser::setSubDir(const String& path) {
  subdir_text->setText(path);
  svn_model->setSubDir(path);
}

void iSubversionBrowser::setRev(int rev) {
  rev_box->setValue(rev);
  svn_model->setRev(rev);
}

void iSubversionBrowser::setUrlWCPath(const String& url, const String& wc_path, int rev) {
  url_text->setText(url);
  wc_text->setText(wc_path);
  rev_box->setValue(rev);
  svn_model->setUrlWCPath(url, wc_path, rev);
}

void iSubversionBrowser::setUrlWCPathSubDir(const String& url, const String& wc_path,
      const String& subdir, int rev) {
  url_text->setText(url);
  wc_text->setText(wc_path);
  subdir_text->setText(subdir);
  rev_box->setValue(rev);
  svn_model->setUrlWCPath(url, wc_path, rev);
  svn_model->setSubDir(subdir); // should have one fun for this..
}

void iSubversionBrowser::go_clicked() {
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


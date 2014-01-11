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
#include <iSvnRevLogModel>
#include <QFileSystemModel>
#include <iLineEdit>
#include <iTableView>
#include <QVBoxLayout>
#include <QHeaderView>
#include <iMainWindowViewer>
#include <QToolBar>
#include <iSpinBox>
#include <QFrame>
#include <QSortFilterProxyModel>
#include <iSplitter>

#include <taMisc>
#include <taiMisc>

iSubversionBrowser::iSubversionBrowser(QWidget* parent)
:inherited(taiMisc::main_window)
{
  int font_spec = taiMisc::fonMedium;
  this->setWindowTitle("Subversion Browser");

  svn_log_model = new iSvnRevLogModel(this);
  svn_file_model = new iSvnFileListModel(this);
  svn_wc_model = new QFileSystemModel(this);

  split = new iSplitter;
  setCentralWidget(split);

  QLabel* lbl = NULL;
  QHeaderView* header = NULL;
  QToolBar* tool_bar = NULL;

  ////////////////////////////////////////////////////////////////////////
  // first is revision browser

  QFrame* lbrow = new QFrame(this);
  lbrow->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_lb = new QVBoxLayout(lbrow);
  lay_lb->setMargin(0);

  QToolBar* lb_tb = new QToolBar(lbrow);
  lay_lb->addWidget(lb_tb);
  
  lbl = new QLabel("end rev:");
  lbl->setToolTip("ending revision to list log of commits for -- use -1 for the most recent (head)");
  lb_tb->addWidget(lbl);

  end_rev_box = new iSpinBox(lbrow);
  end_rev_box->setMinimum(-10000);
  end_rev_box->setValue(svn_log_model->end_rev());
  lb_tb->addWidget(end_rev_box);

  lbl = new QLabel(" n revs:");
  lbl->setToolTip("number of revisions to get log data for");
  lb_tb->addWidget(lbl);

  n_entries_box = new iSpinBox(lbrow);
  n_entries_box->setMinimum(-10000);
  n_entries_box->setValue(svn_log_model->n_entries());
  lb_tb->addWidget(n_entries_box);

  lb_act_go = lb_tb->addAction("Go");

  log_table = new iTableView(lbrow);
  svn_log_sort = new QSortFilterProxyModel(this);
  svn_log_sort->setSourceModel(svn_log_model);

  log_table->setModel(svn_log_sort);
  lay_lb->addWidget(log_table);

  header = log_table->horizontalHeader();
  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);
  header->setSortIndicator(0, Qt::DescendingOrder);
  for(int i=0; i<svn_log_model->columnCount(); i++) {
    if(i != 2 && i != 4) {
#if (QT_VERSION >= 0x050000)
      header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
      header->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
    }
  }
  log_table->setSortingEnabled(true);
  log_table->setWordWrap(true);

  // Disallow complex selections.
  log_table->setSelectionMode(QAbstractItemView::SingleSelection);

  // Keep the scrollbar visible at all times since the combo-boxes are
  // sized assuming it is present.  Hopefully this works on all platforms.
  log_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  // Don't need to see the vertical headers since we have an index column.
  log_table->verticalHeader()->setVisible(false);

  split->addWidget(lbrow);


  ////////////////////////////////////////////////////////////////////////
  // next is svn file browser

  QFrame* fbrow = new QFrame(this);
  fbrow->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_fb = new QVBoxLayout(fbrow);
  lay_fb->setMargin(0);

  QToolBar* fb_tb = new QToolBar(fbrow);
  lay_fb->addWidget(fb_tb);
  
  lbl = new QLabel("svn url:");
  lbl->setToolTip("subversion repository url -- typically http:// or https:// url of server hosting the repository");
  fb_tb->addWidget(lbl);

  url_text = new iLineEdit(fbrow);
  fb_tb->addWidget(url_text);

  tool_bar = new QToolBar(fbrow);
  lay_fb->addWidget(tool_bar);

  lbl = new QLabel("subdir:");
  lbl->setToolTip("current subdirectory path within repository");
  tool_bar->addWidget(lbl);

  subdir_text = new iLineEdit(fbrow);
  tool_bar->addWidget(subdir_text);

  lbl = new QLabel(" rev:");
  lbl->setToolTip("current revision to operate on -- use -1 for head (current)");
  tool_bar->addWidget(lbl);

  rev_box = new iSpinBox(fbrow);
  rev_box->setMinimum(-10000);
  rev_box->setValue(svn_file_model->rev());
  tool_bar->addWidget(rev_box);

  fb_act_go = tool_bar->addAction("Go");

  file_table = new iTableView(fbrow);
  svn_file_sort = new QSortFilterProxyModel(this);
  svn_file_sort->setSourceModel(svn_file_model);

  file_table->setModel(svn_file_sort);
  lay_fb->addWidget(file_table);

  header = file_table->horizontalHeader();
  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);
  header->setSortIndicator(0, Qt::AscendingOrder);
  for(int i=0; i<svn_file_model->columnCount(); i++) {
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

  split->addWidget(fbrow);


  ////////////////////////////////////////////////////////////////////////
  // finally third panel is working copy standard file browser

  QFrame* wbrow = new QFrame(this);
  wbrow->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_wb = new QVBoxLayout(wbrow);
  lay_wb->setMargin(0);

  tool_bar = new QToolBar(wbrow);
  lay_wb->addWidget(tool_bar);

  lbl = new QLabel("working copy:");
  lbl->setToolTip("path to the working copy where repository is checked out");
  tool_bar->addWidget(lbl);

  wc_text = new iLineEdit(wbrow);
  tool_bar->addWidget(wc_text);

  wb_act_go = tool_bar->addAction("Go");

  wc_table = new iTableView(wbrow);
  svn_wc_sort = new QSortFilterProxyModel(this);
  svn_wc_sort->setSourceModel(svn_wc_model);

  wc_table->setModel(svn_wc_sort);
  lay_wb->addWidget(wc_table);

  header = wc_table->horizontalHeader();
  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);
  header->setSortIndicator(0, Qt::AscendingOrder);
  for(int i=0; i<svn_wc_model->columnCount(); i++) {
#if (QT_VERSION >= 0x050000)
    header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
    header->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
  }
  wc_table->setSortingEnabled(true);

  // Disallow complex selections.
  wc_table->setSelectionMode(QAbstractItemView::SingleSelection);

  // Keep the scrollbar visible at all times since the combo-boxes are
  // sized assuming it is present.  Hopefully this works on all platforms.
  wc_table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

  // Don't need to see the vertical headers since we have an index column.
  wc_table->verticalHeader()->setVisible(false);

  split->addWidget(wbrow);

  // connect it all up!
  // these are evil -- use only the go box for this!
  // connect(end_rev_box, SIGNAL(editingFinished()), this, SLOT(lBrowGoClicked()) );
  // connect(n_entries_box, SIGNAL(editingFinished()), this, SLOT(lBrowGoClicked()) );
  connect(lb_act_go, SIGNAL(triggered()), this, SLOT(lBrowGoClicked()) );
  connect(log_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(logCellDoubleClicked(const QModelIndex&)));

  connect(url_text, SIGNAL(returnPressed()), this, SLOT(fBrowGoClicked()) );
  connect(subdir_text, SIGNAL(returnPressed()), this, SLOT(fBrowGoClicked()) );
  connect(rev_box, SIGNAL(editingFinished()), this, SLOT(fBrowGoClicked()) );
  connect(fb_act_go, SIGNAL(triggered()), this, SLOT(fBrowGoClicked()) );
  connect(file_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(fileCellDoubleClicked(const QModelIndex&)));

  connect(wc_text, SIGNAL(returnPressed()), this, SLOT(wBrowGoClicked()) );
  connect(wb_act_go, SIGNAL(triggered()), this, SLOT(wBrowGoClicked()) );
  connect(wc_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(wcCellDoubleClicked(const QModelIndex&)));

  iSize sz = taiM->dialogSize(taiMisc::dlgBig);
  resize(sz.width(), (int)(1.2f * (float)sz.height())); // a bit bigger than .6h
}

iSubversionBrowser::~iSubversionBrowser() {
  
}

void iSubversionBrowser::updateView() {
//  log_table->resizeColumnsToContents();
  file_table->resizeColumnsToContents();
  wc_table->resizeColumnsToContents();
}

void iSubversionBrowser::setUrl(const String& url) {
  url_text->setText(url);
  svn_file_model->setUrl(url);
  if(svn_log_model->end_rev() < 0 && svn_file_model->file_revs.size > 0) {
    int end_rev = svn_file_model->file_revs[0]; // should be the last commit at top level
    end_rev_box->setValue(end_rev);
    n_entries_box->setValue(svn_log_model->n_entries());
    svn_log_model->setUrl(url, end_rev, svn_log_model->n_entries());
  }
  else {
    svn_log_model->setUrl(url);
  }
  updateView();
}

void iSubversionBrowser::setWCPath(const String& wc_path) {
  wc_text->setText(wc_path);
  svn_wc_model->setRootPath(wc_path);
  updateView();
}

void iSubversionBrowser::setSubDir(const String& path) {
  subdir_text->setText(path);
  svn_file_model->setSubDir(path);
  wc_table->setRootIndex(svn_wc_model->index(path));
  updateView();
}

void iSubversionBrowser::setRev(int rev) {
  rev_box->setValue(rev);
  svn_file_model->setRev(rev);
  updateView();
}

void iSubversionBrowser::setEndRev(int end_rev, int n_entries) {
  end_rev_box->setValue(end_rev);
  n_entries_box->setValue(n_entries);
  svn_log_model->setRev(end_rev, n_entries);
  updateView();
}

void iSubversionBrowser::setUrlWCPath(const String& url, const String& wc_path, int rev) {
  setUrl(url);
  wc_text->setText(wc_path);
  svn_wc_model->setRootPath(wc_path);
  rev_box->setValue(rev);
  svn_file_model->setWCPath(wc_path);
  svn_file_model->setRev(rev);
  updateView();
}

void iSubversionBrowser::setUrlWCPathSubDir(const String& url, const String& wc_path,
      const String& subdir, int rev) {
  setUrl(url);
  wc_text->setText(wc_path);
  svn_wc_model->setRootPath(wc_path);
  subdir_text->setText(subdir);
  rev_box->setValue(rev);
  svn_file_model->setWCPath(wc_path);
  svn_file_model->setRev(rev);
  svn_file_model->setSubDir(subdir); // should have one fun for this..
  updateView();
}

void iSubversionBrowser::lBrowGoClicked() {
  String urltxt = url_text->text();
  if(urltxt.empty()) {
    taMisc::Error("url is empty -- must specify a valid url");
    return;
  }
  int end_rev = end_rev_box->value();
  int n_entries = n_entries_box->value();
  setEndRev(end_rev, n_entries);
}

void iSubversionBrowser::fBrowGoClicked() {
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

void iSubversionBrowser::wBrowGoClicked() {
  String wctxt = wc_text->text();
  setWCPath(wctxt);
}

void iSubversionBrowser::logCellDoubleClicked(const QModelIndex& index) {
  QModelIndex rw0 = index.child(index.row(), 0);
  QVariant qrev = svn_log_sort->data(rw0);
  int rev = qrev.toInt();
  setRev(rev);
  // todo: want to filter browser to show only the files in log rev!
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

void iSubversionBrowser::wcCellDoubleClicked(const QModelIndex& index) {
}

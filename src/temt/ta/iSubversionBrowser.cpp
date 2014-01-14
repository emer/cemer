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
#include <iCheckBox>
#include <taiEditorOfString>
#include <SubversionClient>
#include <MethodDef>

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

  QWidget* body = new QWidget;
  setCentralWidget(body);
  
  QVBoxLayout* lay_bd = new QVBoxLayout(body);
  lay_bd->setMargin(0);

  ////////////////////////////////////////////////////////////////////////
  // top-level toolbar

  main_tb = new QToolBar;
  lay_bd->addWidget(main_tb);

  a_view_file = main_tb->addAction("View File");
  connect(a_view_file, SIGNAL(triggered()), this, SLOT(a_view_file_do()));
  a_view_diff = main_tb->addAction("View Diff");
  connect(a_view_diff, SIGNAL(triggered()), this, SLOT(a_view_diff_do()));
  a_save_file = main_tb->addAction("Save File");
  connect(a_save_file, SIGNAL(triggered()), this, SLOT(a_save_file_do()));
  a_add_file =  main_tb->addAction("Add File");
  connect(a_save_file, SIGNAL(triggered()), this, SLOT(a_save_file_do()));
  a_rm_file =   main_tb->addAction("Del File");
  connect(a_rm_file, SIGNAL(triggered()), this, SLOT(a_rm_file_do()));

  main_tb->addSeparator();
  a_update    = main_tb->addAction("Update");
  connect(a_update, SIGNAL(triggered()), this, SLOT(a_update_do()));
  a_commit    = main_tb->addAction("Commit");
  connect(a_commit, SIGNAL(triggered()), this, SLOT(a_commit_do()));
  a_checkout  = main_tb->addAction("Checkout");
  connect(a_checkout, SIGNAL(triggered()), this, SLOT(a_checkout_do()));

  main_tb->addSeparator();
  a_list_mod  = main_tb->addAction("Show Modified");
  connect(a_list_mod, SIGNAL(triggered()), this, SLOT(a_list_mod_do()));

  split = new iSplitter;
  lay_bd->addWidget(split);

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

  end_rev_pgup = lb_tb->addAction("^^");
  end_rev_pgup->setToolTip("page-up on end_rev -- increase end_rev by n_entries");
  end_rev_pgdn = lb_tb->addAction("vv");
  end_rev_pgdn->setToolTip("page-down on end_rev -- decrease end_rev by n_entries");
  connect(end_rev_pgup, SIGNAL(triggered()), this, SLOT(endRevPgUp()));
  connect(end_rev_pgdn, SIGNAL(triggered()), this, SLOT(endRevPgDn()));

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
  header->setDefaultSectionSize(200);
  for(int i=0; i<svn_log_model->columnCount(); i++) {
#if (QT_VERSION >= 0x050000)
    if(i == 2) {
      header->setSectionResizeMode(i, QHeaderView::Interactive); // takes default
    }
    else {
      header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
#else
    if(i == 2) {
      header->setResizeMode(i, QHeaderView::Interactive);
    }
    else {
      header->setResizeMode(i, QHeaderView::ResizeToContents);
    }
#endif
  }
#if (QT_VERSION >= 0x050000)
  log_table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
#else
  log_table->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
#endif
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

  sd_up = tool_bar->addAction("Up..");

  lbl = new QLabel(" rev:");
  lbl->setToolTip("current revision to operate on -- use -1 for head (current)");
  tool_bar->addWidget(lbl);

  rev_box = new iSpinBox(fbrow);
  rev_box->setMinimum(-10000);
  rev_box->setValue(svn_file_model->rev());
  tool_bar->addWidget(rev_box);

  lbl = new QLabel(" only:");
  lbl->setToolTip("only show files from specified revision");
  tool_bar->addWidget(lbl);

  rev_only = new iCheckBox(fbrow);
  tool_bar->addWidget(rev_only);

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
  connect(sd_up, SIGNAL(triggered()), this, SLOT(subDirUp()) );
  connect(rev_box, SIGNAL(editingFinished()), this, SLOT(fBrowGoClicked()) );
  connect(rev_only, SIGNAL(clicked(bool)), this, SLOT(fBrowGoClicked()) );
  connect(fb_act_go, SIGNAL(triggered()), this, SLOT(fBrowGoClicked()) );
  connect(file_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(fileCellDoubleClicked(const QModelIndex&)));

  connect(wc_text, SIGNAL(returnPressed()), this, SLOT(wBrowGoClicked()) );
  connect(wb_act_go, SIGNAL(triggered()), this, SLOT(wBrowGoClicked()) );
  connect(wc_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(wcCellDoubleClicked(const QModelIndex&)));

  // almost full screen:
  iSize sz = taiM->scrn_s;
  sz.set((int)(.9f * sz.width()), (int)(0.9f * (float)sz.height()));
  resize(sz.w, sz.h);

  QList<int> cur_sz = split->sizes();
  if(cur_sz.count() == 3) {     // should be
    cur_sz[0] = (int)(.4f * sz.w);
    cur_sz[1] = (int)(.3f * sz.w);
    cur_sz[2] = (int)(.3f * sz.w);
    split->setSizes(cur_sz);
  }
}

iSubversionBrowser::~iSubversionBrowser() {
  
}

void iSubversionBrowser::updateView() {
  bool filt_rev = rev_only->isChecked();
  if(filt_rev) {
    int rev = rev_box->value();
    svn_file_sort->setFilterKeyColumn(2);    
    svn_file_sort->setFilterRegExp(QRegExp(QString::number(rev), Qt::CaseInsensitive,
                                            QRegExp::FixedString));
  }
  else {
    svn_file_sort->setFilterKeyColumn(2);    
    svn_file_sort->setFilterRegExp(QRegExp());
  }

  log_table->resizeRowsToContents();
  file_table->resizeColumnsToContents();
  wc_table->resizeColumnsToContents();
}

void iSubversionBrowser::setUrl(const String& url) {
  svn_file_model->setUrl(url);
  String ur = svn_file_model->url();
  url_text->setText(ur);
  if(svn_log_model->end_rev() < 0 && svn_file_model->svn_head_rev >= 0) {
    int end_rev = svn_file_model->svn_head_rev;
    end_rev_box->setValue(end_rev);
    n_entries_box->setValue(svn_log_model->n_entries());
    svn_log_model->setUrl(url, end_rev, svn_log_model->n_entries());
  }
  else {
    if(svn_log_model->url() != url)
      svn_log_model->setUrl(url);
  }
  updateView();
}

void iSubversionBrowser::setWCPath(const String& wc_path) {
  svn_file_model->setWCPath(wc_path);
  String wc = svn_file_model->wc_path();
  wc_text->setText(wc);
  svn_wc_model->setRootPath(wc);
  wc_table->setRootIndex(svn_wc_sort->mapFromSource(svn_wc_model->index(wc)));
  updateView();
}

void iSubversionBrowser::setWCView(const String& wc_path) {
  wc_table->setRootIndex(svn_wc_sort->mapFromSource(svn_wc_model->index(wc_path)));
  updateView();
}

void iSubversionBrowser::setSubDir(const String& path) {
  subdir_text->setText(path);
  svn_file_model->setSubDir(path);
  String wc = svn_file_model->wc_path_full();
  setWCView(wc);
  updateView();
}

void iSubversionBrowser::setRev(int rev) {
  rev_box->setValue(rev);
  svn_file_model->setRev(rev);
  updateView();
}

void iSubversionBrowser::setEndRev(int end_rev, int n_entries) {
  end_rev_box->setValue(end_rev);
  if(n_entries > 0)
    n_entries_box->setValue(n_entries);
  else
    n_entries = n_entries_box->value();
  svn_log_model->setRev(end_rev, n_entries);
  updateView();
}

void iSubversionBrowser::setUrlWCPath(const String& url, const String& wc_path, int rev) {
  setUrl(url);
  setWCPath(wc_path);
  setRev(rev);
}

void iSubversionBrowser::setUrlWCPathSubDir(const String& url, const String& wc_path,
      const String& subdir, int rev) {
  setUrl(url);
  setWCPath(wc_path);
  setSubDir(subdir);
  setRev(rev);
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
  if(urltxt != svn_file_model->url()) {
    setUrl(urltxt);
  }
  int rev = rev_box->value();
  if(rev != svn_file_model->rev()) {
    setRev(rev);
  }
  String subtxt = subdir_text->text();
  setSubDir(subtxt);
}

void iSubversionBrowser::wBrowGoClicked() {
  String wctxt = wc_text->text();
  setWCPath(wctxt);
}

void iSubversionBrowser::logCellDoubleClicked(const QModelIndex& index) {
  QModelIndex rw0 = index.child(index.row(), 0);
  QVariant qrev = svn_log_sort->data(rw0);
  int rev = qrev.toInt();
  rev_only->setChecked(true);   // filter
  setRev(rev);
}

void iSubversionBrowser::fileCellDoubleClicked(const QModelIndex& index) {
  int col = index.column();
  if(col < 2) {
    QModelIndex rw0 = index.child(index.row(), 0);
    QModelIndex fidx = svn_file_sort->mapToSource(rw0);
    String fnm = svn_file_model->fileName(fidx);
    if(fnm.empty() || fnm == ".") return;
    if(fnm == "..") {
      subDirUp();
      return;
    }
    bool is_dir = svn_file_model->isDir(fidx);
    if(is_dir) {
      String subtxt = subdir_text->text();
      if(subtxt.nonempty())
        subtxt += "/";
      subtxt += fnm;
      subtxt = taMisc::NoFinalPathSep(subtxt);
      setSubDir(subtxt);
    }
    else {
      if(svn_file_model->fileToString(fnm, view_svn_file)) {
        // todo: if only then do diff instead
        viewSvnFile(fnm);
      }
    }
  }
  else {
    QModelIndex rw2 = index.child(index.row(), 2);
    QModelIndex fidx = svn_file_sort->mapToSource(rw2);
    int rev = svn_file_model->fileRev(fidx);
    setEndRev(rev);
  }
}

void iSubversionBrowser::endRevPgUp() {
  int end_rev = end_rev_box->value();
  int n_entries = n_entries_box->value();
  end_rev += n_entries;
  if(svn_file_model->svn_head_rev >= 0) {
    end_rev = MIN(end_rev, svn_file_model->svn_head_rev);
  }
  setEndRev(end_rev);
}

void iSubversionBrowser::endRevPgDn() {
  int end_rev = end_rev_box->value();
  int n_entries = n_entries_box->value();
  end_rev -= n_entries;
  end_rev = MAX(end_rev, n_entries);
  setEndRev(end_rev);
}

void iSubversionBrowser::subDirUp() {
  String subtxt = subdir_text->text();
  if(subtxt.empty()) return;
  subtxt = taMisc::GetDirFmPath(subtxt);
  subtxt = taMisc::NoFinalPathSep(subtxt);
  setSubDir(subtxt);
}

void iSubversionBrowser::wcCellDoubleClicked(const QModelIndex& index) {
  QModelIndex fidx = svn_wc_sort->mapToSource(index);
  String fnm = svn_wc_model->fileName(fidx);
  if(fnm.empty()) return;
  String subtxt = subdir_text->text();
  if(subtxt.nonempty())
    subtxt += "/";
  subtxt += fnm;
  subtxt = taMisc::NoFinalPathSep(subtxt);
  setSubDir(subtxt);
}

String iSubversionBrowser::selSvnFile() {
  QModelIndexList sels = file_table->selectionModel()->selectedIndexes();
  if(sels.count() == 0) {
    return "";
  }
  QModelIndex idx = sels.at(0); // only support single sel
  QModelIndex fidx = svn_file_sort->mapToSource(idx);
  String fnm = svn_file_model->fileName(fidx);
  return fnm;
}

String iSubversionBrowser::selWcFile() {
  QModelIndexList sels = wc_table->selectionModel()->selectedIndexes();
  if(sels.count() == 0) {
    return "";
  }
  QModelIndex idx = sels.at(0); // only support single sel
  QModelIndex fidx = svn_wc_sort->mapToSource(idx);
  String fnm = svn_wc_model->fileName(fidx);
  return fnm;
}

void iSubversionBrowser::viewSvnFile(const String& fnm) {
  TypeDef* td = &TA_iSubversionBrowser;
  MemberDef* md = td->members.FindName("view_svn_file");
  taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, false);
  // args are: read_only, modal, parent, line_nos, rich_text
  host_->Constr("Selected Svn File: " + fnm);
  host_->Edit(false);
}

void iSubversionBrowser::a_view_file_do() {
  String fnm = selSvnFile();
  if(fnm.nonempty()) {
    if(svn_file_model->fileToString(fnm, view_svn_file)) {
      viewSvnFile(fnm);
    }
  }
  else {
    fnm = selWcFile();
    if(fnm.nonempty()) {
      if(svn_file_model->fileToString(fnm, view_svn_file)) {
        viewSvnFile(fnm);
      }
    }
  }
}

void iSubversionBrowser::a_view_diff_do() {

}

void iSubversionBrowser::a_save_file_do() {
  String fnm = selSvnFile();
  if(fnm.nonempty()) {
    // if(svn_file_model->foileToString(fnm, view_svn_file)) {
    //   viewSvnFile(fnm);
    // }
  }
}

void iSubversionBrowser::a_add_file_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    // svn_file_model->addFile(fnm);
  }
}

void iSubversionBrowser::a_rm_file_do() {

}

void iSubversionBrowser::a_update_do() {
  int rev = svn_file_model->svn_client->Update();
  if(rev > svn_file_model->svn_head_rev) {
    setEndRev(rev);
    rev_only->setChecked(true);   // filter
    setRev(rev);
    svn_file_model->svn_head_rev = rev;
  }
}

void iSubversionBrowser::a_commit_do() {
  svn_file_model->svn_client->Checkin(""); // will prompt
}

void iSubversionBrowser::a_checkout_do() {

}

void iSubversionBrowser::a_list_mod_do() {

}


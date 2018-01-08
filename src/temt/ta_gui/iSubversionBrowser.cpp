// Copyright 2014-2017, Regents of the University of Colorado,
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
#include <iFileSystemModel>
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
#include <taiWidgetMenu>
#include <QPoint>
#include <taRootBase>
#include <taProject>

#include <taMisc>
#include <taiMisc>
#include <tabMisc>

iSubversionBrowser* iSubversionBrowser::OpenBrowser(const String& url,
                                                    const String& wc_path) {
  iSubversionBrowser* svb = new iSubversionBrowser;
  if(url.nonempty() && wc_path.nonempty()) {
    svb->setUrlWcPath(url, wc_path);
  }
  else if(url.nonempty()) {
    svb->setUrl(url);
  }
  else if(wc_path.nonempty()) {
    svb->setWcPath(wc_path);
    String wc_url;
    if(svb->svn_file_model->getUrlFromPath(wc_url, wc_path))
      svb->setUrl(wc_url);
  }
  svb->show();
  svb->raise();
  return svb;
}

iSubversionBrowser::iSubversionBrowser(QWidget* parent)
:inherited(taiMisc::main_window)
{
  int font_spec = taiMisc::fonMedium;
  this->setWindowTitle("Subversion Browser");

  log_subdir = false;
  
  svn_log_model = new iSvnRevLogModel(this);
  svn_file_model = new iSvnFileListModel(this);
  svn_wc_model = new iFileSystemModel(this);
  svn_wc_model->setReadOnly(false);

  QWidget* body = new QWidget;
  setCentralWidget(body);
  
  QVBoxLayout* lay_bd = new QVBoxLayout(body);
  lay_bd->setMargin(0);

  ////////////////////////////////////////////////////////////////////////
  // top-level toolbar

  main_tb = new QToolBar;
  lay_bd->addWidget(main_tb);

  a_log_file =  main_tb->addAction("Log");
  a_log_file->setToolTip(taiMisc::ToolTipPreProcess("Use currently-selected file as target to display in Svn Revision Log panel -- only shows svn commits that affected this file"));
  connect(a_log_file, SIGNAL(triggered()), this, SLOT(a_log_file_do()));

  a_view_file = main_tb->addAction("View File");
  a_view_file->setToolTip(taiMisc::ToolTipPreProcess("Display currently-selected file contents in a simple text editor"));
  connect(a_view_file, SIGNAL(triggered()), this, SLOT(a_view_file_do()));
  
  a_view_diff = main_tb->addAction("View Diff");
  a_view_diff->setToolTip(taiMisc::ToolTipPreProcess("Display diffs for currently-selected file relative to prior revision"));
  connect(a_view_diff, SIGNAL(triggered()), this, SLOT(a_view_diff_do()));

  a_save_file = main_tb->addAction("Save");
  a_save_file->setToolTip(taiMisc::ToolTipPreProcess("Save currently-selected file to a new file name in local working copy -- prompts for new file name"));
  connect(a_save_file, SIGNAL(triggered()), this, SLOT(a_save_file_do()));
  
  a_add_file =  main_tb->addAction("Add");
  a_add_file->setToolTip(taiMisc::ToolTipPreProcess("Add currently-selected Working Copy file to SVN version control management"));
  connect(a_add_file, SIGNAL(triggered()), this, SLOT(a_add_file_wc_do()));
  
  a_rm_file =   main_tb->addAction("Delete");
  a_rm_file->setToolTip(taiMisc::ToolTipPreProcess("Delete currently-selected file"));
  connect(a_rm_file, SIGNAL(triggered()), this, SLOT(a_rm_file_do()));
  
  a_cp_file =   main_tb->addAction("Copy");
  a_cp_file->setToolTip(taiMisc::ToolTipPreProcess("Copy currently-selected file to a new file name -- prompets for new file name"));
  connect(a_cp_file, SIGNAL(triggered()), this, SLOT(a_cp_file_do()));
  
  a_mv_file =   main_tb->addAction("Rename");
  a_mv_file->setToolTip(taiMisc::ToolTipPreProcess("Move currently-selected file to a new file name -- prompts for a new file name"));
  connect(a_mv_file, SIGNAL(triggered()), this, SLOT(a_mv_file_do()));
  
  a_rev_file =   main_tb->addAction("Revert");
  a_rev_file->setToolTip(taiMisc::ToolTipPreProcess("Revert currently-selected Working copy file to the last-saved revision in svn repository"));
  connect(a_rev_file, SIGNAL(triggered()), this, SLOT(a_rev_file_do()));
  
  main_tb->addSeparator();
  
  a_diffcmp =   main_tb->addAction("DiffCompare Proj");
  a_diffcmp->setToolTip(taiMisc::ToolTipPreProcess("Perform a Diff Compare on currently-selected repository .proj project file, at currently-selected revision, against a current project -- saves the file to a file_rev.proj filename, loads it, and runs DiffCompare"));
  connect(a_diffcmp, SIGNAL(triggered()), this, SLOT(a_diffcmp_do()));

  main_tb->addSeparator();
  a_update    = main_tb->addAction("Update");
  a_update->setToolTip(taiMisc::ToolTipPreProcess("Update Working Copy files based on current repository"));
  connect(a_update, SIGNAL(triggered()), this, SLOT(a_update_do()));
  
  a_commit    = main_tb->addAction("Commit");
  a_commit->setToolTip(taiMisc::ToolTipPreProcess("Commit current changes in Working Copy to repository"));
  connect(a_commit, SIGNAL(triggered()), this, SLOT(a_commit_do()));
  
  a_checkout  = main_tb->addAction("Checkout");
  a_checkout->setToolTip(taiMisc::ToolTipPreProcess("Check out repository to Working Copy -- prompts for a location for the new working copy files"));
  connect(a_checkout, SIGNAL(triggered()), this, SLOT(a_checkout_do()));
  
  a_cleanup  = main_tb->addAction("Cleanup");
  a_cleanup->setToolTip(taiMisc::ToolTipPreProcess("Cleanup the current Working Copy SVN metadata etc -- often needed after a failed action"));
  connect(a_cleanup, SIGNAL(triggered()), this, SLOT(a_cleanup_do()));

  // main_tb->addSeparator();
  // a_list_mod  = main_tb->addAction("Show Modified");
  // connect(a_list_mod, SIGNAL(triggered()), this, SLOT(a_list_mod_do()));
  a_list_mod = NULL;

  split = new iSplitter;
  lay_bd->addWidget(split);

  QLabel* lbl = NULL;
  QHeaderView* header = NULL;
  QToolBar* tool_bar = NULL;
  QHBoxLayout* hb = NULL;

  ////////////////////////////////////////////////////////////////////////
  // first is revision browser

  QFrame* lbrow = new QFrame(this);
  lbrow->setFrameStyle(QFrame::Panel); //  | QFrame::Sunken);

  QVBoxLayout* lay_lb = new QVBoxLayout(lbrow);
  lay_lb->setMargin(0); lay_lb->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Svn Revision Log</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("shows the log of revisions for files for current repository url (shown in middle panel) -- double click on a revision to view it in the repository file viewer"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_lb->addLayout(hb);

  QToolBar* lb_tb = new QToolBar(lbrow);
  lay_lb->addWidget(lb_tb);
  
  lbl = new QLabel("end rev:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("ending revision to list log of commits for -- use -1 for the most recent (head)"));
  lb_tb->addWidget(lbl);

  end_rev_box = new iSpinBox(lbrow);
  end_rev_box->setMinimum(-10000);
  end_rev_box->setValue(svn_log_model->end_rev());
  lb_tb->addWidget(end_rev_box);

  end_rev_pgup = lb_tb->addAction("^^");
  end_rev_pgup->setToolTip(taiMisc::ToolTipPreProcess("page-up on end_rev -- increase end_rev by n_entries"));
  end_rev_pgdn = lb_tb->addAction("vv");
  end_rev_pgdn->setToolTip(taiMisc::ToolTipPreProcess("page-down on end_rev -- decrease end_rev by n_entries"));
  connect(end_rev_pgup, SIGNAL(triggered()), this, SLOT(endRevPgUp()));
  connect(end_rev_pgdn, SIGNAL(triggered()), this, SLOT(endRevPgDn()));

  lbl = new QLabel(" n revs:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("number of revisions to get log data for"));
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
  lay_fb->setMargin(0);  lay_fb->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Svn Repository Files</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("shows the files checked into the repository at current url, and subdirectory -- double click on a file to view the file or diff if 'only' flag is set (so only given revision is being shown), or a directory to open it up -- keeps working copy and url coordinated"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_fb->addLayout(hb);

  QToolBar* fb_tb = new QToolBar(fbrow);
  lay_fb->addWidget(fb_tb);
  
  lbl = new QLabel("svn url:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("subversion repository url -- typically http:// or https:// url of server hosting the repository"));
  fb_tb->addWidget(lbl);

  url_text = new iLineEdit(fbrow);
  fb_tb->addWidget(url_text);

  tool_bar = new QToolBar(fbrow);
  lay_fb->addWidget(tool_bar);

  lbl = new QLabel("subdir:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("current subdirectory path within repository"));
  tool_bar->addWidget(lbl);

  subdir_text = new iLineEdit(fbrow);
  tool_bar->addWidget(subdir_text);

  sd_up = tool_bar->addAction("Up..");

  lbl = new QLabel(" rev:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("current revision to operate on -- use -1 for head (current)"));
  tool_bar->addWidget(lbl);

  rev_box = new iSpinBox(fbrow);
  rev_box->setMinimum(-10000);
  rev_box->setValue(svn_file_model->rev());
  tool_bar->addWidget(rev_box);

  rev_only = new iCheckBox(" only", fbrow);
  rev_only->setToolTip(taiMisc::ToolTipPreProcess("only show files from specified revision"));
  tool_bar->addWidget(rev_only);

  log_me = new iCheckBox(" log", fbrow);
  log_me->setToolTip(taiMisc::ToolTipPreProcess("show change log only for files in this directory"));
  tool_bar->addWidget(log_me);
  
  fb_act_go = tool_bar->addAction("Go");

  file_table = new iTableView(fbrow);
  svn_file_sort = new QSortFilterProxyModel(this);
  svn_file_sort->setSortRole(Qt::EditRole);
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
  lay_wb->setMargin(0);  lay_wb->setSpacing(2);

  hb = new QHBoxLayout;
  hb->setMargin(0);
  lbl = new QLabel("<b>Working Copy Files</b>");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("shows the files in the currently checked-out working copy, and subdirectory -- double click on a file to view the file, or a directory to open it up -- keeps working copy and url coordinated"));
  hb->addStretch();
  hb->addWidget(lbl);
  hb->addStretch();
  lay_wb->addLayout(hb);

  tool_bar = new QToolBar(wbrow);
  lay_wb->addWidget(tool_bar);

  lbl = new QLabel("working copy:");
  lbl->setToolTip(taiMisc::ToolTipPreProcess("path to the working copy where repository is checked out"));
  tool_bar->addWidget(lbl);

  wc_text = new iLineEdit(wbrow);
  tool_bar->addWidget(wc_text);

  wc_updt = new iCheckBox("view", wbrow);
  wc_updt->setToolTip(taiMisc::ToolTipPreProcess("view the working copy files -- this can be somewhat slow and memory intensive for large directories"));
  tool_bar->addWidget(wc_updt);

  wb_act_go = tool_bar->addAction("Go");

  wc_table = new iTableView(wbrow);
  svn_wc_sort = new QSortFilterProxyModel(this);
  svn_wc_sort->setSortRole(Qt::EditRole);
  svn_wc_sort->setSourceModel(svn_wc_model);

  wc_table->setModel(svn_wc_sort);
  lay_wb->addWidget(wc_table);
  wc_table->setDragEnabled(true);
  wc_table->setAcceptDrops(true);
  wc_table->setDropIndicatorShown(true);

  header = wc_table->horizontalHeader();
  // Don't highlight the header cells when a selection is made (looks dumb).
  header->setHighlightSections(false);
  header->setSortIndicator(0, Qt::AscendingOrder);
  wBrowResizeCols();            // this doesn't do anything unfortunately
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
#if (QT_VERSION >= 0x040700)
  connect(svn_wc_model, SIGNAL(directoryLoaded(const QString&)),
          this, SLOT(wBrowResizeCols()));
#endif
  connect(lb_act_go, SIGNAL(triggered()), this, SLOT(lBrowGoClicked()) );
  connect(log_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(logCellDoubleClicked(const QModelIndex&)));

  connect(url_text, SIGNAL(returnPressed()), this, SLOT(fBrowGoClicked()) );
  connect(subdir_text, SIGNAL(returnPressed()), this, SLOT(fBrowGoClicked()) );
  connect(sd_up, SIGNAL(triggered()), this, SLOT(subDirUp()) );
  connect(rev_box, SIGNAL(editingFinished()), this, SLOT(fBrowGoClicked()) );
  connect(rev_only, SIGNAL(clicked(bool)), this, SLOT(fBrowGoClicked()) );
  connect(log_me, SIGNAL(clicked(bool)), this, SLOT(logMeClicked()) );
  connect(fb_act_go, SIGNAL(triggered()), this, SLOT(fBrowGoClicked()) );
  connect(file_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(fileCellDoubleClicked(const QModelIndex&)));

  connect(file_table, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(file_table_customContextMenuRequested(const QPoint&)) );

  connect(wc_text, SIGNAL(returnPressed()), this, SLOT(wBrowGoClicked()) );
  connect(wc_updt, SIGNAL(clicked(bool)), this, SLOT(wBrowGoClicked()) );
  connect(wb_act_go, SIGNAL(triggered()), this, SLOT(wBrowGoClicked()) );
  connect(wc_table, SIGNAL(doubleClicked(const QModelIndex &)), this, 
          SLOT(wcCellDoubleClicked(const QModelIndex&)));

  connect(wc_table, SIGNAL(customContextMenuRequested(const QPoint&)), this,
          SLOT(wc_table_customContextMenuRequested(const QPoint&)) );

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
  String org_url = svn_file_model->url();
  String org_wc = svn_file_model->wc_path();

  setUrl_impl(url);

  if(org_url.empty()) {
    updateView();
    return;
  }
  String new_url = svn_file_model->url();
  // update the working copy to match new url -- hopefully some kind of mod of current path
  // need to find the common path between org_url and ur
  String com_ur = common_prefix(org_url, new_url, 0);
  if(com_ur.empty()) {
    taMisc::Warning("A completely different svn_url has just been set -- working copy cannot be updated automatically -- please update manually!");
    updateView();
    return;
  }
  if(com_ur == org_url) { // new is an addition to the original
    String new_sufx = new_url.after(com_ur);
    String new_wc = org_wc + new_sufx;
    setWcPath_impl(new_wc);
  }
  else {
    String org_sufx = org_url.after(com_ur); // unique suffix after common path
    String com_wc = org_wc.before(org_sufx,-1); // assuming this is there!
    if(com_wc.empty()) {
      taMisc::Warning("unable to update working copy based on svn url change -- update it manually!");
      updateView();
      return;
    }
    String new_sufx = new_url.after(com_ur);
    String new_wc = com_wc + new_sufx;
    setWcPath_impl(new_wc);
  }
  updateView();
}

void iSubversionBrowser::setUrl_impl(const String& url) {
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
    int end_rev = end_rev_box->value();
    int n_entries = n_entries_box->value();
    if(svn_log_model->url() != url) {
      svn_log_model->setUrl(url, end_rev, n_entries);
    }
  }
}

void iSubversionBrowser::setWcPath(const String& wc_path) {
  String org_url = svn_file_model->url();
  String org_wc = svn_file_model->wc_path();

  setWcPath_impl(wc_path);
  
  if(org_wc.empty()) {
    updateView();
    return;
  }
  String new_wc = svn_file_model->wc_path();
  // update the working copy to match new url -- hopefully some kind of mod of current path
  // need to find the common path between org_url and ur
  String com_wc = common_prefix(org_wc, new_wc, 0);
  if(com_wc.empty()) {
    taMisc::Warning("A completely different working copy path has just been set -- url path cannot be updated automatically -- please update manually!");
    updateView();
    return;
  }
  if(com_wc == org_wc) { // new is an addition to the original
    String new_sufx = new_wc.after(com_wc);
    String new_url = org_url + new_sufx;
    setUrl_impl(new_url);
  }
  else {
    String org_sufx = org_wc.after(com_wc); // unique suffix after common path
    String com_ur = org_url.before(org_sufx,-1); // assuming this is there!
    if(com_ur.empty()) {
      taMisc::Warning("unable to update url path based on working copy change -- update it manually!");
      updateView();
      return;
    }
    String new_sufx = new_wc.after(com_wc);
    String new_url = com_ur + new_sufx;
    setUrl_impl(new_url);
  }
  updateView();
}

void iSubversionBrowser::setWcPath_impl(const String& wc_path) {
  svn_file_model->setWcPath(wc_path);
  String wc = svn_file_model->wc_path();
  wc_text->setText(wc);
  bool updt = wc_updt->isChecked();
  if(updt) {
    svn_wc_model->setRootPath(wc);
    wc_table->setRootIndex(svn_wc_sort->mapFromSource(svn_wc_model->index(wc)));
  }
}

void iSubversionBrowser::setWcView(const String& wc_path) {
  bool updt = wc_updt->isChecked();
  if(updt) {
    wc_table->setRootIndex(svn_wc_sort->mapFromSource(svn_wc_model->index(wc_path)));
    updateView();
  }
}

void iSubversionBrowser::setSubDir(const String& path) {
  subdir_text->setText(path);
  svn_file_model->setSubDir(path);
  String wc = svn_file_model->wc_path_full();
  setWcView(wc);
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

void iSubversionBrowser::setUrlWcPath(const String& url, const String& wc_path, int rev) {
  setUrl(url);
  setWcPath(wc_path);
  setRev(rev);
}

void iSubversionBrowser::setUrlWcPathSubDir(const String& url, const String& wc_path,
      const String& subdir, int rev) {
  setUrl(url);
  setWcPath(wc_path);
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
  setWcPath(wctxt);
  bool updt = wc_updt->isChecked();
  if(updt) {
    String subtxt = subdir_text->text();
    setSubDir(subtxt);
  }
}

void iSubversionBrowser::logMeClicked() {
  bool log_st = log_me->isChecked();
  if(log_subdir != log_st) {
    log_subdir = log_st;
    if(!log_subdir) {
      log_file_name = "";       // clear
    }
  }

  String log_url = svn_file_model->url();
  if(log_subdir || log_file_name.nonempty()) {
    log_url = svn_file_model->url_full();
    if(log_file_name.nonempty()) {
      log_url += "/" + log_file_name;
    }
  }
  taMisc::Info("Logging:", log_url);
  svn_log_model->setUrl(log_url, svn_log_model->end_rev(), svn_log_model->n_entries());
}

void iSubversionBrowser::wBrowResizeCols() {
  QHeaderView* header = wc_table->horizontalHeader();
  for(int i=0; i<svn_wc_model->columnCount(); i++) {
#if (QT_VERSION >= 0x050000)
    header->setSectionResizeMode(i, QHeaderView::ResizeToContents);
#else
    header->setResizeMode(i, QHeaderView::ResizeToContents);
#endif
  }
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
    QModelIndex fidx = svn_file_sort->mapToSource(index);
    String fnm = svn_file_model->fileName(fidx);
    int rev = svn_file_model->fileRev(fidx);
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
      bool filt_rev = rev_only->isChecked();
      if(filt_rev) {
        if(svn_file_model->diffToString(fnm, view_svn_diffs, rev)) {
          viewSvnDiffs(fnm);
        }
      }
      else {
        if(svn_file_model->fileToString(fnm, view_svn_file, rev)) {
          viewSvnFile(fnm);
        }
      }
    }
  }
  else {
    QModelIndex fidx = svn_file_sort->mapToSource(index);
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
  if(subtxt.empty()) {
    String ur = url_text->text();
    String wc = wc_text->text();
    ur = taMisc::GetDirFmPath(ur);
    ur = taMisc::NoFinalPathSep(ur);
    wc = taMisc::GetDirFmPath(wc);
    wc = taMisc::NoFinalPathSep(wc);
    int rev = rev_box->value();
    setUrlWcPath(ur, wc, rev);
    // move up on the paths
  }
  else {
    subtxt = taMisc::GetDirFmPath(subtxt);
    subtxt = taMisc::NoFinalPathSep(subtxt);
    setSubDir(subtxt);
  }
}

void iSubversionBrowser::wcCellDoubleClicked(const QModelIndex& index) {
  QModelIndex fidx = svn_wc_sort->mapToSource(index);
  String fnm = svn_wc_model->fileName(fidx);
  if(fnm.empty()) return;
  bool is_dir = svn_wc_model->isDir(fidx);
  if(is_dir) {
    String subtxt = subdir_text->text();
    if(subtxt.nonempty())
      subtxt += "/";
    subtxt += fnm;
    subtxt = taMisc::NoFinalPathSep(subtxt);
    setSubDir(subtxt);
  }
  else {
    if(svn_file_model->diffToStringWc(fnm, view_wc_diffs)) {
      viewWcDiffs(fnm);
    }
  }
}

void iSubversionBrowser::file_table_customContextMenuRequested(const QPoint& pos) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  iAction* act = NULL;
  act = menu->AddItem("&Log of File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_log_file_do()), 1);
  act = menu->AddItem("View &File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_view_file_do()), 1);
  act = menu->AddItem("View &Diffs", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_view_diff_do()), 1);
  act = menu->AddItem("&Save File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_save_file_do()), 1);
  act = menu->AddItem("&Del File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_rm_file_do()), 1);
  act = menu->AddItem("&Copy File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_cp_file_do()), 1);
  act = menu->AddItem("&Rename File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_mv_file_do()), 1);
  act = menu->AddItem("DiffCompare Project", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_diffcmp_do()), 1);

  menu->exec(file_table->mapToGlobal(pos));
  delete menu;
}

void iSubversionBrowser::wc_table_customContextMenuRequested(const QPoint& pos) {
  taiWidgetMenu* menu = new taiWidgetMenu(this, taiWidgetMenu::normal, taiMisc::fonSmall);
  iAction* act = NULL;
  act = menu->AddItem("&Log of File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_log_file_wc_do()), 1);
  act = menu->AddItem("View &File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_view_file_wc_do()), 1);
  act = menu->AddItem("&Edit File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_edit_file_wc_do()), 1);
  act = menu->AddItem("View D&iffs", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_view_diff_wc_do()), 1);
  menu->insertSeparator();
  act = menu->AddItem("Svn &Add File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_add_file_wc_do()), 1);
  act = menu->AddItem("Svn &Delete File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_rm_file_wc_do()), 1);
  act = menu->AddItem("Svn &Revert File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_rev_file_wc_do()), 1);
  act = menu->AddItem("Svn &Copy File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_cp_file_wc_do()), 1);
  act = menu->AddItem("&Svn Rename File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_mv_file_wc_do()), 1);
  act = menu->AddItem("Svn Re&name File (force)", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_mv_file_wc_force_do()), 1);
  
  menu->insertSeparator();
  act = menu->AddItem("Local Delete File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_rm_file_loc_do()), 1);
  act = menu->AddItem("Local Copy File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_cp_file_loc_do()), 1);
  act = menu->AddItem("Local Rename File", taiWidgetMenu::normal,
                      iAction::int_act, this, SLOT(a_mv_file_loc_do()), 1);

  menu->exec(wc_table->mapToGlobal(pos));
  delete menu;
}

String iSubversionBrowser::selSvnFile(int& rev) {
  QModelIndexList sels = file_table->selectionModel()->selectedIndexes();
  if(sels.count() == 0) {
    return "";
  }
  QModelIndex idx = sels.at(0); // only support single sel
  QModelIndex fidx = svn_file_sort->mapToSource(idx);
  String fnm = svn_file_model->fileName(fidx);
  rev = svn_file_model->fileRev(fidx);
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

void iSubversionBrowser::viewWcFile(const String& fnm) {
  TypeDef* td = &TA_iSubversionBrowser;
  MemberDef* md = td->members.FindName("view_wc_file");
  taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, false);
  // args are: read_only, modal, parent, line_nos, rich_text
  host_->Constr("Selected Working Copy File: " + fnm);
  host_->Edit(false);
}

void iSubversionBrowser::viewSvnDiffs(const String& fnm) {
  view_svn_diffs.xml_esc();      // esc for rich text
  TypeDef* td = &TA_iSubversionBrowser;
  MemberDef* md = td->members.FindName("view_svn_diffs");
  taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, false, true);
  // args are: read_only, modal, parent, line_nos, rich_text, diffs
  host_->Constr("Selected Svn Diffs: " + fnm);
  host_->Edit(false);
}

void iSubversionBrowser::viewWcDiffs(const String& fnm) {
  view_wc_diffs.xml_esc();      // esc for rich text
  TypeDef* td = &TA_iSubversionBrowser;
  MemberDef* md = td->members.FindName("view_wc_diffs");
  taiEditorOfString* host_ = new taiEditorOfString(md, this, td, true, false, NULL, true, false, true);
  // args are: read_only, modal, parent, line_nos, rich_text, diffs
  host_->Constr("Selected Working Copy Diffs: " + fnm);
  host_->Edit(false);
}

void iSubversionBrowser::a_log_file_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    log_file_name = fnm;
    log_me->setChecked(true);
    logMeClicked();
  }
  else {
    a_log_file_wc_do();
  }
}

void iSubversionBrowser::a_log_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    log_file_name = fnm;
    log_me->setChecked(true);
    logMeClicked();
  }
}

void iSubversionBrowser::a_view_file_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    if(svn_file_model->fileToString(fnm, view_svn_file, rev)) {
      viewSvnFile(fnm);
    }
  }
  else {
    a_view_file_wc_do();
  }
}

void iSubversionBrowser::a_view_diff_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    if(svn_file_model->diffToString(fnm, view_svn_diffs, rev)) {
      viewSvnDiffs(fnm);
    }
  }
  else {
    a_view_diff_wc_do();
  }
}

void iSubversionBrowser::a_view_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    if(svn_file_model->fileToStringWc(fnm, view_wc_file)) {
      viewWcFile(fnm);
    }
  }
}

void iSubversionBrowser::a_edit_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    svn_file_model->editFileWc(fnm);
  }
}

void iSubversionBrowser::a_view_diff_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    if(svn_file_model->diffToStringWc(fnm, view_wc_diffs)) {
      viewWcDiffs(fnm);
    }
  }
}

void iSubversionBrowser::a_save_file_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    svn_file_model->saveFile(fnm, "", rev); // prompt for where to save to
  }
}

void iSubversionBrowser::a_add_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    svn_file_model->addFile(fnm);
  }
}

void iSubversionBrowser::a_rm_file_do() {
  int rev;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    int chs = taMisc::Choice(String("Are you sure you want to delete file from svn repository: ") + fnm,
                             "Ok", "Cancel");
    if(chs == 0) {
      svn_file_model->delFile(fnm, false, false); // filling in options here..
    }
  }
  else {
    a_rm_file_wc_do();
  }
}

void iSubversionBrowser::a_rm_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    int chs = taMisc::Choice(String("Are you sure you want to delete file from svn repository: ") + fnm,
                             "Ok", "Cancel");
    if(chs == 0) {
      svn_file_model->delFile(fnm, false, false); // filling in options here..
    }
  }
}

void iSubversionBrowser::a_rm_file_loc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    int chs = taMisc::Choice(String("Are you sure you want to delete file locally: ") + fnm,
                             "Ok", "Cancel");
    if(chs == 0) {
      svn_file_model->delFileLocal(fnm);
    }
  }
}

void iSubversionBrowser::a_rev_file_do() {
  int rev;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    int chs = taMisc::Choice(String("Are you sure you want to revert local changes in  file: ") + fnm + " -- all local modifictions will be lost!",
                             "Ok", "Cancel");
    if(chs == 0) {
      svn_file_model->revertFile(fnm);
    }
  }
  else {
    a_rev_file_wc_do();
  }
}

void iSubversionBrowser::a_rev_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    int chs = taMisc::Choice(String("Are you sure you want to revert local changes in file: ") + fnm  + " -- all local modifictions will be lost!",
                             "Ok", "Cancel");
    if(chs == 0) {
      svn_file_model->revertFile(fnm);
    }
  }
}

void iSubversionBrowser::a_cp_file_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Copy subversion file to new name and/or path:",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->copyFile(org_fnm, fnm);
    }
  }
}

void iSubversionBrowser::a_mv_file_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Rename (move) subversion file to new name and/or path (will NOT overwrite existing):",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->moveFile(org_fnm, fnm, false); // not force
    }
  }
}

void iSubversionBrowser::a_cp_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Copy subversion file to new name and/or path:",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->copyFile(org_fnm, fnm);
    }
  }
}

void iSubversionBrowser::a_mv_file_wc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Rename (move) subversion file to new name and/or path (will NOT overwrite existing):",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->moveFile(org_fnm, fnm, false); // not force
    }
  }
}

void iSubversionBrowser::a_mv_file_wc_force_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Rename (move) subversion file to new name and/or path (will overwrite existing):",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->moveFile(org_fnm, fnm, true);
    }
  }
}

void iSubversionBrowser::a_cp_file_loc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Copy local file to new name and/or path:",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->copyFileLocal(org_fnm, fnm);
    }
  }
}

void iSubversionBrowser::a_mv_file_loc_do() {
  String fnm = selWcFile();
  if(fnm.nonempty()) {
    String org_fnm = fnm;
    bool ok = taMisc::StringPrompt(fnm, "Rename (move) local file to new name and/or path:",
                             "Ok", "Cancel");
    if(ok) {
      svn_file_model->moveFileLocal(org_fnm, fnm);
    }
  }
}

void iSubversionBrowser::a_update_do() {
  bool updt = svn_file_model->update();
  if(updt) {
    int rev = svn_file_model->svn_head_rev;
    setEndRev(rev);
    rev_only->setChecked(true);   // filter
    // wc_updt->setChecked(false);  // no updt
    setRev(rev);
  }
}

void iSubversionBrowser::a_commit_do() {
  svn_file_model->commit(""); // will prompt
}

void iSubversionBrowser::a_checkout_do() {
  int rev = rev_box->value();
  String new_wc;
  if(svn_file_model->checkout(new_wc, rev)) { // will prompt, and fill in new_wc
    String new_url = svn_file_model->url_full(); // change to full url
    setUrlWcPath(new_url, new_wc);
  }
}

void iSubversionBrowser::a_cleanup_do() {
  svn_file_model->cleanup();
}

void iSubversionBrowser::a_list_mod_do() {

}

void iSubversionBrowser::a_diffcmp_do() {
  int rev = 0;
  String fnm = selSvnFile(rev);
  if(fnm.empty()) {
    taMisc::Error("No file selected for diff compare");
    return;
  }
  if(!fnm.endsWith(".proj")) {
    taMisc::Error("DiffCompare only works on .proj files");
    return;
  }

  String new_fnm = fnm.before(".proj", -1) + "_" + String(rev) + ".proj";
  svn_file_model->saveFile(fnm, new_fnm, rev);
  int pidx = tabMisc::root->projects.size;
  tabMisc::root->projects.Load(new_fnm);
  taProject* proj = tabMisc::root->projects.SafeEl(pidx);
  if(proj) {
    proj->CallFun("DiffCompare");
  }
}


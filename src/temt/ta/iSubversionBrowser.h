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

#ifndef iSubversionBrowser_h
#define iSubversionBrowser_h 1

// parent includes:
#include "ta_def.h"
#include <taString>
#ifndef __MAKETA__
#include <QMainWindow>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class iSvnFileListModel; //
class iSvnRevLogModel; //
class QSortFilterProxyModel; //
class QFileSystemModel; //
class iLineEdit; //
class iTableView; //
class iSpinBox; //
class QAction; //
class QModelIndex; //
class iSplitter; //
class iCheckBox; //
class QToolBar; //
class QPoint; //

taTypeDef_Of(iSubversionBrowser);

class TA_API iSubversionBrowser : public QMainWindow {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##NO_CSS #CAT_Gui browser for subversion repositories
INHERITED(QMainWindow)
  Q_OBJECT
public:

  static iSubversionBrowser*   OpenBrowser(const String& url, const String& wc_path);
  // open a new browser for given paths -- if only wc_path is specified, then it tries to find url from that working copy path

  String                 view_svn_file; // subversion file for viewing
  String                 view_wc_file;  // working copy file for viewing
  String                 view_svn_diffs; // subversion diffs for viewing
  String                 view_wc_diffs;  // working copy diffs for viewing

  virtual void  setUrl(const String& url);
  // set the url for the repository and update display to that
  virtual void  setWcPath(const String& wc_path);
  // set the url for the repository and update display to that
  virtual void  setUrlWcPath(const String& url, const String& wc_path, int rev = -1);
  // set the url and working copy path for the repository and update display to that
  virtual void  setUrlWcPathSubDir(const String& url, const String& wc_path, 
                                   const String& subdir, int rev = -1);
  // set the url and working copy path and subdir for the repository and update display to that
  virtual void  setSubDir(const String& path);
  // set the current subdirectory within repository
  virtual void  setRev(int rev);
  // set the revision
  virtual void  setEndRev(int end_rev, int n_entries = -1);
  // set the ending revision and n_entries for the log browser

  virtual void  setWcView(const String& wc_path);
  // set only what is being viewed in the working copy guy -- nothing else

  virtual void  updateView();
  // shouldn't need this, but need to call it to get it to resize to contents

  virtual String selSvnFile(int& rev);
  // get currently selected svn browser file name (empty if none)
  virtual String selWcFile();
  // get currently selected working copy browser file name (empty if none)
  virtual void   viewSvnFile(const String& fnm);
  // view the view_svn_file file
  virtual void   viewWcFile(const String& fnm);
  // view the view_wc_file file
  virtual void   viewSvnDiffs(const String& fnm);
  // view the view_svn_diffs
  virtual void   viewWcDiffs(const String& fnm);
  // view the view_wc_diffs

  iSubversionBrowser(QWidget* parent = NULL);
  ~iSubversionBrowser();

protected slots:
  void    lBrowGoClicked();
  void    fBrowGoClicked();
  void    wBrowGoClicked();

  void    endRevPgUp();
  void    endRevPgDn();

  void    subDirUp();
  void    fileCellDoubleClicked(const QModelIndex& index);
  void    logCellDoubleClicked(const QModelIndex& index);
  void    wcCellDoubleClicked(const QModelIndex& index);

  void    file_table_customContextMenuRequested(const QPoint& pos);
  void    wc_table_customContextMenuRequested(const QPoint& pos);

  void    a_view_file_do();
  void    a_view_diff_do();
  void    a_view_file_wc_do();
  void    a_view_diff_wc_do();
  void    a_save_file_do();
  void    a_add_file_do();
  void    a_rm_file_do();
  void    a_rm_file_wc_do();
          
  void    a_update_do();
  void    a_commit_do();
  void    a_checkout_do();

  void    a_list_mod_do();

  void    a_edit_file_wc_do();

protected:
  iSvnRevLogModel*       svn_log_model;
  iSvnFileListModel*     svn_file_model;
  QFileSystemModel*      svn_wc_model;
  QSortFilterProxyModel* svn_log_sort;
  QSortFilterProxyModel* svn_file_sort;
  QSortFilterProxyModel* svn_wc_sort;

  QToolBar*             main_tb;
  QAction*              a_view_file;
  QAction*              a_view_diff;
  QAction*              a_save_file;
  QAction*              a_add_file;
  QAction*              a_rm_file;

  QAction*              a_update;
  QAction*              a_commit;
  QAction*              a_checkout;

  QAction*              a_list_mod;

  iSplitter*            split;

  iSpinBox*             end_rev_box;
  QAction*              end_rev_pgup;
  QAction*              end_rev_pgdn;
  iSpinBox*             n_entries_box;
  QAction*              lb_act_go;
  iTableView*           log_table;

  iLineEdit*            url_text;
  iLineEdit*            subdir_text;
  QAction*              sd_up;
  iSpinBox*             rev_box;
  iCheckBox*            rev_only;
  QAction*              fb_act_go;
  iTableView*           file_table;

  iLineEdit*            wc_text;
  iCheckBox*            wc_updt;
  QAction*              wb_act_go;
  iTableView*           wc_table;
};

#endif // iSubversionBrowser_h

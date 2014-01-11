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
#include <QMainWindow>

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

//taTypeDef_Of(iSubversionBrowser);

class TA_API iSubversionBrowser : public QMainWindow {
  // browser for subversion repositories
INHERITED(QMainWindow)
  Q_OBJECT
public:
  iSvnRevLogModel*       svn_log_model;
  iSvnFileListModel*     svn_file_model;
  QFileSystemModel*      svn_wc_model;
  QSortFilterProxyModel* svn_log_sort;
  QSortFilterProxyModel* svn_file_sort;
  QSortFilterProxyModel* svn_wc_sort;

  iSplitter*            split;

  iSpinBox*             end_rev_box;
  iSpinBox*             n_entries_box;
  QAction*              lb_act_go;
  iTableView*           log_table;

  iLineEdit*            url_text;
  iLineEdit*            subdir_text;
  iSpinBox*             rev_box;
  QAction*              fb_act_go;
  iTableView*           file_table;

  iLineEdit*            wc_text;
  QAction*              wb_act_go;
  iTableView*           wc_table;

  virtual void  setUrl(const String& url);
  // set the url for the repository and update display to that
  virtual void  setWCPath(const String& wc_path);
  // set the url for the repository and update display to that
  virtual void  setUrlWCPath(const String& url, const String& wc_path, int rev = -1);
  // set the url and working copy path for the repository and update display to that
  virtual void  setUrlWCPathSubDir(const String& url, const String& wc_path, 
                                   const String& subdir, int rev = -1);
  // set the url and working copy path and subdir for the repository and update display to that
  virtual void  setSubDir(const String& path);
  // set the current subdirectory within repository
  virtual void  setRev(int rev);
  // set the revision
  virtual void  setEndRev(int end_rev, int n_entries);
  // set the ending revision and n_entries for the log browser

  virtual void  updateView();
  // shouldn't need this, but need to call it to get it to resize to contents

  iSubversionBrowser(QWidget* parent = NULL);
  ~iSubversionBrowser();

protected slots:
  void          lBrowGoClicked();
  void          fBrowGoClicked();
  void          wBrowGoClicked();
  void          fileCellDoubleClicked(const QModelIndex& index);
  void          logCellDoubleClicked(const QModelIndex& index);
  void          wcCellDoubleClicked(const QModelIndex& index);
};

#endif // iSubversionBrowser_h

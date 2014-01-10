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
class iLineEdit; //
class iTableView; //
class iSubversionModel; //
class iSpinBox; //
class QAction; //

//taTypeDef_Of(iSubversionBrowser);

class TA_API iSubversionBrowser : public QMainWindow {
  // browser for subversion repositories
INHERITED(QMainWindow)
  Q_OBJECT
public:
  iSubversionModel*     svn_model;

  iLineEdit*            url_text;
  iLineEdit*            wc_text;
  iLineEdit*            subdir_text;
  iSpinBox*             rev_box;
  QAction*              actGo;
  iTableView*           file_table;

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

  iSubversionBrowser(QWidget* parent = NULL);
  ~iSubversionBrowser();

protected slots:
  void                  go_clicked(); // or return in url_text

};

#endif // iSubversionBrowser_h

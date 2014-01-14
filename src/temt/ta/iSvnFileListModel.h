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

#ifndef iSvnFileListModel_h
#define iSvnFileListModel_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QAbstractItemModel>
// todo: this crashes maketa!
#include <stdexcept>
#endif

// member includes:
#include <String_PArray>
#include <int_PArray>

// declare all other types mentioned but not required to include:
class SubversionClient; //
class QFileIconProvider; //

taTypeDef_Of(iSvnFileListModel);

class TA_API iSvnFileListModel : public QAbstractItemModel {
  // #NO_INSTANCE #NO_CSS Qt model for subversion client list of files in repository
INHERITED(QAbstractItemModel)
  Q_OBJECT
public:

  iSvnFileListModel(QObject *parent = 0);
  ~iSvnFileListModel();

  // one of the following set functions must be called before data will be avail
  virtual bool  setUrlWCPath(const QString& url, const QString& wc_path, int rev = -1);
  // init the svn client, set the url, wc_path and read the data from that url
  virtual bool  setWCPath(const QString& wc_path);
  // init the svn client, set the working copy path -- resets subdir to empty
  virtual bool  setUrl(const QString& url, int rev = -1);
  // init the svn client, set the url and read the data from that url  -- resets subdir to empty
  virtual bool  setSubDir(const QString& path);
  // set the current subdirectory within repository
  virtual bool  setRev(int rev);
  // set the revision
  virtual bool  refresh();
  // read the data from the currently set url and subdir

  const QString& url() const { return svn_url; } 
  int            rev() const { return svn_rev; } 
  const QString& wc_path() const { return svn_wc_path; } 
  const QString& subdir() const { return svn_subdir; } 
  const QString& url_full() const { return svn_url_full; } 
  const QString& wc_path_full() const { return svn_wc_path_full; } 

  virtual QString fileName(const QModelIndex& index);
  // get file name at index
  virtual int     fileSize(const QModelIndex& index);
  // get author at index
  virtual int     fileRev(const QModelIndex& index);
  // get revsion at index
  virtual int     fileTime(const QModelIndex& index);
  // get time at index
  virtual int     fileKind(const QModelIndex& index);
  // get file kind at index -- should be svn_node_file or svn_node_dir
  virtual bool    isDir(const QModelIndex& index);
  // return true if this is a directory at index
  virtual QString fileAuthor(const QModelIndex& index);
  // get author at index

  virtual bool    fileToString(const String& fnm, String& to_file);
  // get file to string -- file is relative to current url and subdir

public: // required model implementations
#ifndef __MAKETA__
  override int          columnCount(const QModelIndex& parent = QModelIndex()) const;
  override QVariant     data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  override Qt::ItemFlags flags(const QModelIndex& index) const;
  override QVariant     headerData(int section, Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const;
  override int          rowCount(const QModelIndex& parent = QModelIndex()) const;
  override bool         setData(const QModelIndex& index, const QVariant& value,
                                int role = Qt::EditRole);
  override QModelIndex  index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const;
  override QModelIndex  parent(const QModelIndex &child) const;

  int           svn_head_rev;
  // head (current) revision -- obtained when setUrl is called -- gets it from the first entry in directory 

  // this is how we store the raw data -- interface with SubversionClient
  String_PArray file_names;
  String_PArray file_paths;
  int_PArray    file_sizes;
  int_PArray    file_revs;
  int_PArray    file_times;
  int_PArray    file_kinds;
  String_PArray file_authors;

  SubversionClient*     svn_client; // our client
  QFileIconProvider*    file_icon_provider;

protected:
  // This exception class only used internally.
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const char *msg);
  };

  QString               svn_url;    // current url
  int                   svn_rev;    // svn revision number
  QString               svn_wc_path; // current working copy path
  QString               svn_subdir;  // current subdirectory within
  QString               svn_url_full; // current full url = url + subdir
  QString               svn_wc_path_full; // current full wc_path = wc_path + subdir

  virtual bool  initSvnClient();

  virtual bool  validateIndex(const QModelIndex& index) const;
#endif
};

#endif // iSvnFileListModel_h

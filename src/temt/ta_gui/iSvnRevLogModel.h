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

#ifndef iSvnRevLogModel_h
#define iSvnRevLogModel_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QAbstractItemModel>
#endif

#include <stdexcept>

// member includes:
#include <String_PArray>
#include <int_PArray>

// declare all other types mentioned but not required to include:
class SubversionClient; //

taTypeDef_Of(iSvnRevLogModel);

class TA_API iSvnRevLogModel : public QAbstractItemModel {
  // #NO_INSTANCE #NO_CSS Qt model for subversion client log of revisions to repository
INHERITED(QAbstractItemModel)
  Q_OBJECT
public:

  iSvnRevLogModel(QObject *parent = 0);
  ~iSvnRevLogModel();

  // one of the following set functions must be called before data will be avail
  virtual bool  setUrl(const QString& url, int end_rev = -1, int n_entries = 50);
  // init the svn client, set the url and read the data from that url, for given ending revision and (maximum) number of entries
  virtual bool  setRev(int end_rev, int n_entries);
  // set the ending revision and (maximum) number of entries to get log for -- gets data
  virtual bool  refresh();
  // read the data from the currently set url and revision

  const QString& url() const { return svn_url; } 
  int            end_rev() const { return svn_end_rev; } 
  int            n_entries() const { return svn_n_entries; } 

public: // required model implementations
#ifndef __MAKETA__
  int          columnCount(const QModelIndex& parent = QModelIndex()) const override;
  QVariant     data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant     headerData(int section, Qt::Orientation orientation,
                                   int role = Qt::DisplayRole) const override;
  int          rowCount(const QModelIndex& parent = QModelIndex()) const override;
  bool         setData(const QModelIndex& index, const QVariant& value,
                                int role = Qt::EditRole) override;
  QModelIndex  index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex  parent(const QModelIndex &child) const override;

  // this is how we store the raw data -- interface with SubversionClient
  int_PArray    revs;            // one per rev -- revision number
  String_PArray commit_msgs;     // one per rev -- the commit message
  String_PArray authors;         // one per rev -- author of rev
  int_PArray    times;           // one per rev -- time as secs since 1970
  int_PArray    files_start_idx; // one per rev -- starting index in files/actions
  int_PArray    files_n;    // one per rev -- number of files in files/actions
  String_PArray files;      // raw list of all files for all logs
  String_PArray actions; // one-to-one with files, mod action for each file

protected:
  // This exception class only used internally.
  class Exception : public std::runtime_error {
  public:
    explicit Exception(const char *msg);
  };

  SubversionClient*     svn_client; // our client
  QString               svn_url;    // current url
  int                   svn_end_rev; // svn ending revision number
  int                   svn_n_entries; // svn number of entries to get

  virtual bool  initSvnClient();

  virtual bool  validateIndex(const QModelIndex& index) const;
#endif
};

#endif // iSvnRevLogModel_h

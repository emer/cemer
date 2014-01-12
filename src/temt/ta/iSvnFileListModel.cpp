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

#include "iSvnFileListModel.h"

#include <SubversionClient>
#include <QDateTime>
#include <QColor>

#include <taMisc>

#define SVN_N_COLS 5

iSvnFileListModel::iSvnFileListModel(QObject* parent)
  : inherited(parent)
  , svn_client(0)
  , svn_rev(-1)
{

}

iSvnFileListModel::~iSvnFileListModel() {
  if(svn_client)
    delete svn_client;
  svn_client = NULL;
}

bool iSvnFileListModel::initSvnClient() {
  if(!svn_client) {
    try {
      svn_client = new SubversionClient;
    }
    catch (const iSvnFileListModel::Exception &ex) {
      taMisc::Error("Error creating SubversionClient.\n", ex.what());
      return false;
    }
  }
  return true;
}

bool iSvnFileListModel::setUrl(const QString& url, int rev) {
  if(!initSvnClient())
    return false;
  svn_url = url;
  svn_rev = rev;
  svn_subdir = "";
  return refresh();
}

bool iSvnFileListModel::setWCPath(const QString& wc_path) {
  if(!initSvnClient())
    return false;
  String wp = taMisc::ExpandFilePath(wc_path);
  svn_client->SetWorkingCopyPath(wp);
  svn_wc_path = svn_client->GetWorkingCopyPath().c_str();
  svn_subdir = "";
  svn_wc_path_full = svn_wc_path;
  return true;
}

bool iSvnFileListModel::setUrlWCPath(const QString& url, const QString& wc_path, int rev) {
  if(!setWCPath(url)) return false;
  return setUrl(url, rev);
}

bool iSvnFileListModel::setSubDir(const QString& subdir) {
  if(!svn_client) {
    taMisc::Error("iSvnFileListModel::setSubDir -- svn_client has not been initialized with setUrl or setWCPath");
    return false;
  }
  svn_subdir = subdir;
  return refresh();
}

bool iSvnFileListModel::setRev(int rev) {
  if(!svn_client) {
    taMisc::Error("iSvnFileListModel::setRev -- svn_client has not been initialized with setUrl or setWCPath");
    return false;
  }
  svn_rev = rev;
  return refresh();
}

QString iSvnFileListModel::fileName(const QModelIndex& index) {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return static_cast<const char*>(file_names[idx]);
  }
  return QString();
}

int iSvnFileListModel::fileSize(const QModelIndex& index) {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_sizes[idx];
  }
  return 0;
}

int iSvnFileListModel::fileRev(const QModelIndex& index) {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_revs[idx];
  }
  return 0;
}

int iSvnFileListModel::fileTime(const QModelIndex& index) {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_times[idx];
  }
  return 0;
}

QString iSvnFileListModel::fileAuthor(const QModelIndex& index) {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return static_cast<const char*>(file_authors[idx]);
  }
  return QString();
}

bool iSvnFileListModel::refresh() {
  if(!svn_client) {
    emit layoutAboutToBeChanged();
    taMisc::Error("iSvnFileListModel::refresh -- svn_client has not been initialized with setUrl or setWCPath");
    emit layoutChanged();
    return false;
  }

  emit layoutAboutToBeChanged();

  svn_url = static_cast<const char*>(taMisc::NoFinalPathSep(svn_url));
  svn_url_full = svn_url;
  if(!svn_subdir.isEmpty()) {
    svn_url_full += QString("/") + svn_subdir;
  }
  svn_url_full = static_cast<const char*>(taMisc::NoFinalPathSep(svn_url_full));

  svn_wc_path = static_cast<const char*>(taMisc::NoFinalPathSep(svn_wc_path));
  svn_wc_path_full = svn_wc_path;
  if(!svn_subdir.isEmpty()) {
    svn_wc_path_full += QString("/") + svn_subdir;
  }
  svn_wc_path_full = static_cast<const char*>(taMisc::NoFinalPathSep(svn_wc_path_full));

  file_names.Reset();
  file_paths.Reset();
  file_sizes.Reset();
  file_revs.Reset();
  file_times.Reset();
  file_authors.Reset();

  try {
    svn_client->List(file_names, file_paths, file_sizes, file_revs, file_times,
                     file_authors, svn_url_full.toLatin1(), svn_rev, false); // no recurse
  }
  catch (const iSvnFileListModel::Exception &ex) {
    taMisc::Error("Error doing List in SubversionClient.\n", ex.what());
    emit layoutChanged();
    return false;
  }

  // taMisc::Info("svn got:", String(file_names.size), "new names");

  emit layoutChanged();
  return true;
}

int iSvnFileListModel::columnCount(const QModelIndex& parent) const {
  return SVN_N_COLS;
}

bool iSvnFileListModel::validateIndex(const QModelIndex& index) const {
  if (!svn_client) return false;
  int idx = index.row();
  if(idx > file_names.size) return false; // +1
  int col = index.column();
  if(col >= SVN_N_COLS) return false;
  return true;
}

QVariant iSvnFileListModel::data(const QModelIndex& index, int role) const {
  if (!validateIndex(index)) return QVariant();
  int idx = index.row() - 1;    // +1 for ..
  int col = index.column();

  int sz = 0;
  if(idx >= 0) sz = file_sizes[idx];

  switch (role) {
  case Qt::DisplayRole: 
  case Qt::EditRole: {
    switch(col) {
    case 0: {
      if(idx < 0)
        return QString("..");
      if(idx == 0)
        return QString(".");
      QString nm = static_cast<const char *>(file_names[idx]);
      if(sz == 0)
        return nm + "/";
      return nm;
      break;
    }
    case 1: {
      if(sz == 0)
        return QString("--");
      QString szstr = static_cast<const char *>
        (taMisc::GetSizeString(sz, 3, true)); // 3 prec, power of 2
      return szstr;
      break;
    }
    case 2: {
      if(idx < 0) return QVariant();
      return file_revs[idx];
      break;
    }
    case 3: {
      if(idx < 0) return QVariant();
      QDateTime dm = QDateTime::fromTime_t(file_times[idx]);
      QString dmstr = dm.toString("yyyy MM/dd hh:mm:ss");
      return dmstr;
      break;
    }
    case 4: {
      if(idx < 0) return QVariant();
      return static_cast<const char *>(file_authors[idx]);
      break;
    }
    }
    break;
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
    // case Qt::SizeHintRole: {
    //   break;
    // }
//Qt::FontRole--  QFont: font for the text
  // case Qt::TextAlignmentRole:
  //   return m_mat->defAlignment();
  //   break;
    case Qt::BackgroundRole: {
      if(idx % 2 == 0)
        return QColor(240, 240, 240); // light grey
      return QVariant();
      break;
    }
  }
/*Qt::TextColorRole
  QColor: color of text
Qt::CheckStateRole*/
  default: break;
  }
  return QVariant();
}

Qt::ItemFlags iSvnFileListModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags rval = 0;
  if (validateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  // not generally editable
  //    rval |= Qt::ItemIsEditable;
  return rval; 
}

QVariant iSvnFileListModel::headerData(int section, 
  Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal) {
    if (role != Qt::DisplayRole)
      return QVariant();

    switch(section) {
    case 0: return tr("Name");
    case 1: return tr("Size");
    case 2: return tr("Rev");
    case 3: return tr("Modified");
    case 4: return tr("Author");
    default: return QVariant();
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int iSvnFileListModel::rowCount(const QModelIndex& parent) const {
  if(!svn_client) return 0;
  return file_names.size+1;
}

bool iSvnFileListModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  return false;
}

QModelIndex iSvnFileListModel::index(int row, int column, const QModelIndex &parent) const {
  if(!svn_client) return QModelIndex();
  if (column < 0 || column >= SVN_N_COLS || row < 0 || row > file_names.size)
    return QModelIndex();
  return createIndex(row, column);
}

QModelIndex iSvnFileListModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}


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

#include "iSvnRevLogModel.h"

#include <SubversionClient>
#include <QDateTime>
#include <QColor>

#include <taMisc>

#define SVN_N_COLS 5

iSvnRevLogModel::iSvnRevLogModel(QObject* parent)
  : inherited(parent)
  , svn_client(0)
  , svn_end_rev(-1)
  , svn_n_entries(20)
{

}

iSvnRevLogModel::~iSvnRevLogModel() {
  if(svn_client)
    delete svn_client;
  svn_client = NULL;
}

bool iSvnRevLogModel::initSvnClient() {
  if(!svn_client) {
    try {
      svn_client = new SubversionClient;
    }
    catch (const iSvnRevLogModel::Exception &ex) {
      taMisc::Error("Error creating SubversionClient.\n", ex.what());
      return false;
    }
  }
  return true;
}

bool iSvnRevLogModel::setUrl(const QString& url, int end_rev, int n_entries) {
  if(!initSvnClient())
    return false;
  svn_url = url;
  svn_end_rev = end_rev;
  svn_n_entries = n_entries;
  return refresh();
}

bool iSvnRevLogModel::setRev(int end_rev, int n_entries) {
  if(!svn_client) {
    taMisc::Error("iSvnRevLogModel::setRev -- svn_client has not been initialized with setUrl");
    return false;
  }
  svn_end_rev = end_rev;
  svn_n_entries = n_entries;
  return refresh();
}

bool iSvnRevLogModel::refresh() {
  if(!svn_client) {
    emit layoutAboutToBeChanged();
    taMisc::Error("iSvnRevLogModel::refresh -- svn_client has not been initialized with setUrl or setWCPath");
    emit layoutChanged();
    return false;
  }

  emit layoutAboutToBeChanged();
  revs.Reset();
  commit_msgs.Reset();
  authors.Reset();
  times.Reset();
  files_start_idx.Reset();
  files_n.Reset();
  files.Reset();
  actions.Reset();

  try {
    svn_client->GetLogs(revs, commit_msgs, authors, times, files_start_idx, files_n,
                       files, actions, svn_url.toLatin1(), svn_end_rev, svn_n_entries);
  }
  catch (const iSvnRevLogModel::Exception &ex) {
    taMisc::Error("Error doing GetLog in SubversionClient.\n", ex.what());
    emit layoutChanged();
    return false;
  }

  emit layoutChanged();
  return true;
}

int iSvnRevLogModel::columnCount(const QModelIndex& parent) const {
  return SVN_N_COLS;
}

bool iSvnRevLogModel::validateIndex(const QModelIndex& index) const {
  if (!svn_client) return false;
  int idx = index.row();
  if(idx >= revs.size) return false;
  int col = index.column();
  if(col >= SVN_N_COLS) return false;
  return true;
}

QVariant iSvnRevLogModel::data(const QModelIndex& index, int role) const {
  if (!validateIndex(index)) return QVariant();
  int idx = index.row();
  int col = index.column();

  switch (role) {
  case Qt::DisplayRole: 
  case Qt::EditRole: {
    switch(col) {
    case 0: {
      return revs[idx];
      break;
    }
    case 1: {
      if(idx < 0) return QVariant();
      QDateTime dm = QDateTime::fromTime_t(times[idx]);
      QString dmstr = dm.toString("yyyy_MM_dd_hh_mm_ss");
      return dmstr;
      break;
    }
    case 2: {
      return static_cast<const char *>(commit_msgs[idx]);
      break;
    }
    case 3: {
      return static_cast<const char *>(authors[idx]);
      break;
    }
    case 4: {
      String rval;
      int sti = files_start_idx[idx];
      int ni = files_n[idx];
      for(int i = 0; i<ni; i++) {
        rval += files[sti + i] + " (" + actions[sti + i] + "); ";
      }
      return static_cast<const char *>(rval);
      break;
    }
    }
    break;
//Qt::DecorationRole
//Qt::ToolTipRole
//Qt::StatusTipRole
//Qt::WhatsThisRole
//Qt::SizeHintRole -- QSize
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
    //-- QColor
  }
/*Qt::TextColorRole
  QColor: color of text
Qt::CheckStateRole*/
  default: break;
  }
  return QVariant();
}

Qt::ItemFlags iSvnRevLogModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags rval = 0;
  if (validateIndex(index)) {
    rval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  }
  // not generally editable
  //    rval |= Qt::ItemIsEditable;
  return rval; 
}

QVariant iSvnRevLogModel::headerData(int section, 
  Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal) {
    if (role != Qt::DisplayRole)
      return QVariant();

    switch(section) {
    case 0: return tr("Rev");
    case 1: return tr("Time");
    case 2: return tr("Message");
    case 3: return tr("Author");
    case 4: return tr("Paths");
    default: return QVariant();
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int iSvnRevLogModel::rowCount(const QModelIndex& parent) const {
  if(!svn_client) return 0;
  return revs.size;
}

bool iSvnRevLogModel::setData(const QModelIndex& index, const QVariant & value, int role) {
  return false;
}

QModelIndex iSvnRevLogModel::index(int row, int column, const QModelIndex &parent) const {
  if(!svn_client) return QModelIndex();
  if (column < 0 || column >= SVN_N_COLS || row < 0 || row >= revs.size)
    return QModelIndex();
  return createIndex(row, column);
}

QModelIndex iSvnRevLogModel::parent(const QModelIndex &child) const {
  return QModelIndex();
}


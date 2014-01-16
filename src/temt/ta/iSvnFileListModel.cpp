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
#include <QIcon>
#include <QFileIconProvider>
#include <QFileDialog>

#include <taMisc>
#include <taFiler>

#include <svn_types.h>

#define SVN_N_COLS 5

bool iSvnFileListModel::CommitFile(const String& file_path, const String& msg) {
  iSvnFileListModel* svn_file_model = new iSvnFileListModel();
  String path = taMisc::GetDirFmPath(file_path);
  svn_file_model->setWcPath(path);
  String wc_url;
  bool rval = false;
  if(svn_file_model->getUrlFromPath(wc_url, path)) {
    svn_file_model->setUrl(wc_url);
    svn_file_model->commit(msg);
    rval = true;
  }
  delete svn_file_model;
  return rval;
}

iSvnFileListModel::iSvnFileListModel(QObject* parent)
  : inherited(parent)
  , svn_client(0)
  , svn_rev(-1)
  , svn_head_rev(-1)
{
  file_icon_provider = new QFileIconProvider;
}

iSvnFileListModel::~iSvnFileListModel() {
  if(svn_client)
    delete svn_client;
  svn_client = NULL;
  if(file_icon_provider)
    delete file_icon_provider;
  file_icon_provider = NULL;
}

bool iSvnFileListModel::initSvnClient() {
  if(!svn_client) {
    try {
      svn_client = new SubversionClient;
    }
    catch (const SubversionClient::Exception &ex) {
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
  bool rval = refresh();
  if(rval && file_revs.size > 0) {
    svn_head_rev = file_revs[0];
  }
  return rval;
}

bool iSvnFileListModel::setWcPath(const QString& wc_path) {
  if(!initSvnClient())
    return false;
  String wp = taMisc::ExpandFilePath(wc_path);
  try {
    svn_client->SetWorkingCopyPath(wp);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in setWcPath\n", ex.what());
    return false;
  }
  svn_wc_path = svn_client->GetWorkingCopyPath().c_str();
  svn_subdir = "";
  svn_wc_path_full = svn_wc_path;
  return true;
}

bool iSvnFileListModel::setUrlWcPath(const QString& url, const QString& wc_path, int rev) {
  if(!setWcPath(url)) return false;
  return setUrl(url, rev);
}

bool iSvnFileListModel::setSubDir(const QString& subdir) {
  if(!svn_client) {
    taMisc::Error("iSvnFileListModel::setSubDir -- svn_client has not been initialized with setUrl or setWcPath");
    return false;
  }
  svn_subdir = subdir;
  return refresh();
}

bool iSvnFileListModel::setRev(int rev) {
  if(!svn_client) {
    taMisc::Error("iSvnFileListModel::setRev -- svn_client has not been initialized with setUrl or setWcPath");
    return false;
  }
  svn_rev = rev;
  return refresh();
}

QString iSvnFileListModel::fileName(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return static_cast<const char*>(file_names[idx]);
    return "..";
  }
  return QString();
}

int iSvnFileListModel::fileSize(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_sizes[idx];
  }
  return 0;
}

int iSvnFileListModel::fileRev(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_revs[idx];
  }
  return 0;
}

int iSvnFileListModel::fileTime(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_times[idx];
  }
  return 0;
}

int iSvnFileListModel::fileKind(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return file_kinds[idx];
    return svn_node_dir;        // -1 is dir
  }
  return svn_node_none;
}

bool iSvnFileListModel::isDir(const QModelIndex& index) const {
  int kind = fileKind(index);
  if(kind == svn_node_file)
    return false;
  if(kind == svn_node_dir)
    return true;
  int sz = fileSize(index);
  if(sz == 0) return true;      // fall back on size if we don't get full info
  return false;
}

QString iSvnFileListModel::fileAuthor(const QModelIndex& index) const {
  if(validateIndex(index)) {
    int idx = index.row() - 1;    // +1 for ..
    if(idx >= 0)
      return static_cast<const char*>(file_authors[idx]);
  }
  return QString();
}

bool iSvnFileListModel::fileToString(const String& fnm, String& to_file, int rv) {
  if(!svn_client)
    return false;
  String path = url_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  try {
    svn_client->GetFile(path, to_file, rv);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in fileToString\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::diffToString(const String& fnm, String& to_file, int rv) {
  if(!svn_client)
    return false;
  String path = url_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  if(rv < 0)
    rv = svn_head_rev;
  try {
    svn_client->GetDiffToPrev(path, to_file, rv); // must be real rev
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in diffToString\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::fileToStringWc(const String& fnm, String& to_file) {
  if(!svn_client)
    return false;
  String path = wc_path_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  to_file.LoadFromFile(path);
  return true;
}

bool iSvnFileListModel::diffToStringWc(const String& fnm, String& to_file) {
  if(!svn_client)
    return false;
  String path = wc_path_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  try {
    svn_client->GetDiffWc(path, to_file);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in diffToStringWc\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::addFile(const String& fnm) {
  if(!svn_client)
    return false;
  if(wc_path().isEmpty()) {
    taMisc::Error("working copy path is empty -- can only add from working copy");
    return false;
  }
  String path = wc_path_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  try {
    svn_client->Add(path);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in addFile\n", ex.what());
    return false;
  }

  taMisc::Info("subversion added path:", path);
  return true;
}

bool iSvnFileListModel::delFile(const String& fnm, bool force, bool keep_local) {
  if(!svn_client)
    return false;
  if(wc_path().isEmpty()) {
    taMisc::Error("working copy path is empty -- can only delete from working copy");
    return false;
  }
  String path = wc_path_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  String_PArray files;
  files.Add(path);
  try {
    svn_client->Delete(files, force, keep_local);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::RemoveFile(path);   // if svn fails, then try locally
  }
  taMisc::Info("subversion deleted path:", path);
  return true;
}

bool iSvnFileListModel::saveFile(const String& fnm, const String& to_fnm, int rv) {
  if(!svn_client)
    return false;
  String out_fnm = to_fnm;
  if(out_fnm.empty()) {
    taFiler* flr = taFiler::New();
    taRefN::Ref(flr);
    String wc_path = wc_path_full();
    wc_path = taMisc::FinalPathSep(wc_path);
    flr->SetFileName(wc_path); // filer etc. does auto extension
    flr->SaveAs(false, false);
    out_fnm = flr->FileName();
    flr->Close();
    taMisc::RemoveFile(out_fnm); // now nuke it
    taRefN::unRefDone(flr);
    if(out_fnm.empty()) {
      taMisc::Error("saveFile -- save to file is still empty -- need to choose one");
      return false;
    }
  }
  String path = url_full();
  path = taMisc::FinalPathSep(path);
  path += fnm;
  try {
    svn_client->SaveFile(path, out_fnm, rv);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in SaveFile\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::update() {
  if(!svn_client)
    return false;
  if(wc_path().isEmpty()) {
    taMisc::Error("working copy path is empty -- can only update with working copy");
    return false;
  }
  int nw_rv = -1;
  try {
    nw_rv = svn_client->Update();
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in update\n", ex.what());
    return false;
  }
  if(nw_rv > svn_head_rev) {
    svn_head_rev = nw_rv;
    return true;
  }
  return false;                 // not updated!
}

bool iSvnFileListModel::commit(const String& msg) {
  if(!svn_client)
    return false;
  if(wc_path().isEmpty()) {
    taMisc::Error("working copy path is empty -- can only commit with working copy");
    return false;
  }
  int nw_rv = -1;
  try {
    nw_rv = svn_client->Checkin(msg);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in commit\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::checkout(String& to_path, int rv) {
  if(!svn_client)
    return false;
  if(to_path.empty()) {
    String wc_path = wc_path_full();
    wc_path = taMisc::FinalPathSep(wc_path);
    QString op = QFileDialog::getExistingDirectory
      (NULL, "select directory to checkout into", wc_path,
       QFileDialog::ShowDirsOnly);
    if(op.isEmpty()) {
      taMisc::Error("checkout -- no directory to check out into -- choose one");
      return false;
    }
    to_path = op;
  }
  String path = url_full();
  try {
    svn_client->Checkout(path, to_path, rv);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in checkout\n", ex.what());
    return false;
  }
  taMisc::Info("Subversion url:", path, "checked out to:", to_path);
  return true;
}

bool iSvnFileListModel::getUrlFromPath(String& url, const String& path) {
  if(!svn_client)
    return false;
  try {
    svn_client->GetUrlFromPath(url, path);
  }
  catch (const SubversionClient::Exception &ex) {
    taMisc::Error("Subversion client error in GetUrlFromPath\n", ex.what());
    return false;
  }
  return true;
}

bool iSvnFileListModel::refresh() {
  if(!svn_client) {
    emit layoutAboutToBeChanged();
    taMisc::Error("iSvnFileListModel::refresh -- svn_client has not been initialized with setUrl or setWcPath");
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
  file_kinds.Reset();
  file_authors.Reset();

  try {
    svn_client->List(file_names, file_paths, file_sizes, file_revs, file_times,
                     file_kinds, file_authors, svn_url_full.toLatin1(), svn_rev, false); // no recurse
  }
  catch (const SubversionClient::Exception &ex) {
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
  bool isdir = isDir(index);

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
      if(isdir)
        return nm + "/";
      return nm;
      break;
    }
    case 1: {
      if(idx < 0 || isdir)
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
  }
  case Qt::DecorationRole: {
    if(col == 0) {
      // from: /usr/local/Trolltech/qt-everywhere-opensource-src-4.8.2/src/gui/dialogs/
      // qfilesystemmodel.cpp
      if(idx < 0) 
        return file_icon_provider->icon(QFileIconProvider::Folder);
      else if(isdir)
        return file_icon_provider->icon(QFileIconProvider::Folder);
      else
        return file_icon_provider->icon(QFileIconProvider::File);
    }
    break;
  }
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


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

#include "DirectoryCatalog.h"


void DirectoryCatalog::Initialize() {
  m_dir = NULL;
  options = CO_Files;
  directory = "./";
}

void DirectoryCatalog::Destroy() {
  ReadClose();
}

void DirectoryCatalog::Copy_(const DirectoryCatalog& cp) {
  ReadClose();
  directory = cp.directory;
}

QDir& DirectoryCatalog::dir() {
  if (!m_dir)
    m_dir = new QDir();
  return *m_dir;
}

const Variant DirectoryCatalog::GetData_impl(int chan) {
  switch (chan) {
  case 0: return dir()[rd_itr]; // blank if out of range
  default: return _nilVariant;
  }
}

int DirectoryCatalog::ItemCount() const {
  return m_dir->count();
}

void DirectoryCatalog::ReadOpen_impl(bool& ok) {
  inherited::ReadOpen_impl(ok);
  if (!ok) return;
  QDir& dir = this->dir(); // cache
  dir.setPath(directory);
  QDir::Filters flags = QDir::NoDotAndDotDot;
  if (options & CO_Files) flags |= QDir::Files;
  if (options & CO_Dirs) flags |= QDir::AllDirs;
  dir.setFilter(flags);
  if (filters.nonempty()) {
    QStringList sl((filters.toQString()).split(";"));
    dir.setNameFilters(sl);
  }
  ReadItrInit();
}

void DirectoryCatalog::ReadClose_impl() {
  if (m_dir) {
    delete m_dir;
    m_dir = NULL;
  }
  inherited::ReadClose_impl();
}

const String DirectoryCatalog::SourceChannelName(int chan) const {
  static String chan_files("files");
  switch (chan) {
  case 0: return chan_files;
  default: return _nilString;
  }
}


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

#include "taXmlStreamReader.h"

#include <QFile>

#include <taMisc>
#include <taString>

TA_BASEFUNS_CTORS_DEFN(taXmlStreamReader);

bool taXmlStreamReader::SetFile(const String& filename) {
  
#if (QT_VERSION >= 0x040600)
  QFile file(filename);
  if(!file.open(QFile::ReadOnly | QFile::Text)){
    String msg("ImageNetUtils::OpenReader -- could not open file " + filename);
    taMisc::Error(msg);
    return false;
  }
  
  reader.setDevice(&file);
  return true;
  
#endif
}

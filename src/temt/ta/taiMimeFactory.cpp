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

#include "taiMimeFactory.h"

const String taiMimeFactory::text_plain("text/plain");

QByteArray taiMimeFactory::StrToByteArray(const String& str) {
  return QByteArray(str.chars());
}

QByteArray taiMimeFactory::StrToByteArray(const QString& str) {
  return str.toLatin1();
}

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

#include "taiMatrixDataMimeItem.h"


taiMimeItem* taiMatrixDataMimeItem::Extract(taiMimeSource* ms,
    const String& subkey)
{
  if (!ms->hasFormat(taiTabularDataMimeFactory::tacss_matrixdesc))
    return NULL;
  taiMatrixDataMimeItem* rval = new taiMatrixDataMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

void taiMatrixDataMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiTabularDataMimeFactory::tacss_matrixdesc, arg);

  return ExtractGeom(arg, m_flat_geom);
}

void  taiMatrixDataMimeItem::DecodeData_impl() {
//note: maybe nothing!
}

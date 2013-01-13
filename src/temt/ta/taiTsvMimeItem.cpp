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

#include "taiTsvMimeItem.h"
#include <taiMimeSource>
#include <taiTabularDataMimeItem>
#include <taiTabularDataMimeFactory>

#include <QTextStream>


taiMimeItem* taiTsvMimeItem::Extract(taiMimeSource* ms,
    const String& subkey)
{
  // note: virtually everything has text/plain, so we just grab and examine
  // note: tabs are separators, not terminators
  // note: some formats (ex Excel) use eol as a sep, others (ex OO.org) as a terminator
  QByteArray ba = ms->data(taiTabularDataMimeFactory::text_plain);
  if (ba.size() == 0) return NULL;
  // first, look at the first line -- we will pull tent cols from this, then
  // double check against the remainder
  QTextStream ts(ba, QIODevice::ReadOnly); // ro stream
  QString str = ts.readLine();
  int cols = str.count('\t', Qt::CaseInsensitive) + 1;
  // now, if it is really a table, it should have total metrics consistent

  // see if it ends in an eol, adjust if necessary
  int tot_rows = 1;
  if (ba.endsWith('\n')) // note: works for Unix or Windows
    tot_rows = 0;

  tot_rows = ba.count('\n') + tot_rows; // note: works for Unix or Windows
  int tot_tabs = ba.count('\t');
  if (((cols - 1) * tot_rows) != tot_tabs)
    return NULL;

  taiTsvMimeItem* rval = new taiTsvMimeItem;
  rval->Constr(ms, subkey); //note: doesn't do anything
  rval->m_flat_geom.set(cols, tot_rows);
  return rval;
}

void taiTsvMimeItem::Initialize() {
}


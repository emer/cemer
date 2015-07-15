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

#include "taPluginBase_List.h"
#include <iClipData>

TA_BASEFUNS_CTORS_DEFN(taPluginBase_List);


taPluginBase* taPluginBase_List::FindUniqueId(const String& value) {
  for (int i = 0; i < size; ++i) {
    taPluginBase* rval = FastEl(i);
    if (!rval) continue;
    if (rval->unique_id == value) return rval;
  }
  return NULL;
}

void taPluginBase_List::QueryEditActions_impl(const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  allowed = iClipData::EA_COPY;
  forbidden = ~iClipData::EA_COPY;
}

void taPluginBase_List::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
    const taiMimeSource* ms, int& allowed, int& forbidden)
{
  allowed = iClipData::EA_COPY;
  forbidden = ~iClipData::EA_COPY;
}

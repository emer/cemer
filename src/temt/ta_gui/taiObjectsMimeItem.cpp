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

#include "taiObjectsMimeItem.h"
#include <taiMimeSource>
#include <taiObjectMimeFactory>

TA_BASEFUNS_CTORS_DEFN(taiObjectsMimeItem);



void taiObjectsMimeItem::Initialize() {
  m_all_base = 0;
  items.SetBaseType(&TA_taiObjectMimeItem);
}

taiMimeItem* taiObjectsMimeItem::Extract(taiMimeSource* ms, 
    const String& subkey)
{
  if (!ms->hasFormat(taiObjectMimeFactory::tacss_objectdesc))
    return NULL;
    
  taiObjectsMimeItem* rval = new taiObjectsMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

bool taiObjectsMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiObjectMimeFactory::tacss_objectdesc, arg);
  
  // get header and class info
  String str;
  str = arg.before(';');
  bool ok;
  int itm_cnt = str.toInt(&ok);
  if (!ok) goto fail;
  arg = arg.after(";\n");
  
  for (int i = 0; i < itm_cnt; ++i) {
    // decode type and get the typedef obj
    String typeName = arg.before(';');
    if (typeName.length() == 0) goto fail;
    arg = arg.after(';');

    // decode path -- could be empty for non-taBase object
    String path = arg.before(';');
    arg = arg.after(';');
    //TODO: skip extension data
    taiObjectMimeItem* msd = (taiObjectMimeItem*) items.New(1);
    msd->m_type_name = typeName;
    msd->m_path = path;
    msd->Constr(ms());
    arg = arg.after('\n');
  }
  return true;

fail:
  items.Reset();
  return false;
}

bool taiObjectsMimeItem::allBase() const {
  if (m_all_base == 0) 
    CommonSubtype(); // asserts it
  return (m_all_base == 1);
}

TypeDef* taiObjectsMimeItem::CommonSubtype() const {
  m_all_base = -1; // assume not
  if (count() == 0) return NULL;
  TypeDef* rval = item(0)->td();
  for (int i = 1; (rval && (i < count())); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, 
      ((taiObjectMimeItem*)item(i))->td());
  }
  if (rval && rval->IsActualTaBase())
    m_all_base = 1;
  return rval;
}


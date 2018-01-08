// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#include "taiObjectMimeItem.h"
#include <taiMimeSource>
#include <taiObjectMimeFactory>

#include <tabMisc>
#include <taRootBase>

TA_BASEFUNS_CTORS_DEFN(taiObjectMimeItem);


using namespace std;

void taiObjectMimeItem::Initialize() {
  m_td = NULL;
  m_obj = NULL;
}

bool taiObjectMimeItem::Constr_impl(const String&) {
  m_td = TypeDef::FindGlobalTypeName(m_type_name, false);
  // note: could be NULL if, ex. decoding leabra object in instance of bp
  return (m_td);
}

void taiObjectMimeItem::DecodeData_impl() {
  MemberDef* md;
  if (isThisProcess() && (!m_obj) && tabMisc::root) {
    m_obj = tabMisc::RootFindFromPath(m_path, md);
  }
}

int taiObjectMimeItem::GetObjectData(istringstream& result) {
  AssertData();
  if (m_ms) {
    QString fmt = taiObjectMimeFactory::tacss_objectdata + ";index=" + String(GetIndex());
    return m_ms->data(fmt, result);
  } else {
    return 0;
  }
}


taBase* taiObjectMimeItem::obj() {  // note: only called when we are InProcess
  AssertData();
  return m_obj;
}


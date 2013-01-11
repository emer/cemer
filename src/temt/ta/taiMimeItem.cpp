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

#include "taiMimeItem.h"


int taiMimeItem::data(const QMimeData* md, const QString& mimeType,
  taString& result) 
{
  QByteArray ba = md->data(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

taiMimeItem* taiMimeItem::ExtractByType(TypeDef* td, taiMimeSource* ms, 
    const String& mimetype)
{
  if (!td->DerivesFrom(&TA_taiMimeItem)) return NULL;
  taiMimeItem* inst = (taiMimeItem*)td->GetInstance();
  if (!inst) return NULL;
  return inst->Extract(ms, mimetype);
}

void taiMimeItem::Initialize() {
  m_index = -1;
  m_flags = 0;
  m_ms = NULL;
}

void taiMimeItem::AssertData() {
//TODO: not if zombie!
  if (!isDecoded()) {
    DecodeData_impl();
    m_flags |= MIF_DECODED;
  }
}

bool taiMimeItem::Constr(taiMimeSource* ms_, const String& mimetype) {
  m_ms = ms_;
  bool rval = Constr_impl(mimetype); // NOTE: mt is often nil (default)
  if (!rval)
    m_flags |= MIF_ZOMBIE;
  return rval;
}

QByteArray taiMimeItem::data(const QString& mimeType) const {
  if (m_ms) return m_ms->data(mimeType);
  else return QByteArray();
}
/*
int taiMimeItem::data(const QString& mimeType, taString& result) const {
  QByteArray ba = data(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

int taiMimeItem::data(const QString& mimeType, istringstream& result) const {
  QByteArray ba = data(mimeType);
  result.str(string(ba.data(), ba.size()));
  return ba.size();
}
*/
bool taiMimeItem::isThisProcess() const {
  if (m_ms) return m_ms->isThisProcess();
  else return false;
}

const QMimeData* taiMimeItem::mimeData() const {
 if (m_ms) return m_ms->mimeData();
  else return NULL;
}

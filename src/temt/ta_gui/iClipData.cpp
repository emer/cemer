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

#include "iClipData.h"
#include <taiMimeFactory>

#include <taMisc>
#include <taiMisc>



const String iClipData::tacss_common("tacss/common");

iClipData::EditAction iClipData::ClipOpToSrcCode(int ea) {
  switch (ea & EA_OP_MASK) {
  case EA_CUT:  return EA_SRC_CUT;
  case EA_COPY: return EA_SRC_COPY;
  case EA_DRAG: return EA_SRC_DRAG;
  default:      return (EditAction)0;
  }
}

iClipData::iClipData(int src_edit_action_)
:inherited()
{
  src_edit_action = src_edit_action_;
  setData(tacss_common, taiMimeFactory::StrToByteArray(GetCommonDescHeader()));
}


const QString iClipData::GetCommonDescHeader() const {
  QString rval =  QString::number(src_edit_action) + ';' +
        QString::number(taMisc::ProcessId()) + ";";
  return rval;
}

void iClipData::setTextData(const QString& mime_type, const QString& data_) {
  setData(mime_type, taiMimeFactory::StrToByteArray(data_));
}

void iClipData::setTextFromStr(const String& str) {
  // sets text without doubly converting from String->QString->ByteArray
  setData(taiMimeFactory::text_plain, taiMimeFactory::StrToByteArray(str));
}


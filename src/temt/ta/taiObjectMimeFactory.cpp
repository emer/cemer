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

#include "taiObjectMimeFactory.h"


#include <sstream>

using namespace std;

const String taiObjectMimeFactory::tacss_objectdesc("tacss/objectdesc");
const String taiObjectMimeFactory::tacss_objectdata("tacss/objectdata");

void taiObjectMimeFactory::AddSingleObject(QMimeData* md, taBase* obj) {
  if (!obj) return;
  QString str;
  InitHeader(1, str);
  AddHeaderDesc(obj, str);
  md->setData(tacss_objectdesc, StrToByteArray(str));
  AddObjectData(md, obj, 0);
}

void taiObjectMimeFactory::AddMultiObjects(QMimeData* md,
   taPtrList_impl* obj_list)
{
  if (!obj_list) return;
  QString str;
  InitHeader(obj_list->size, str);
  // note: prob not necessary, but we iterate twice so header precedes data
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddHeaderDesc(obj, str);
  }
  md->setData(tacss_objectdesc, StrToByteArray(str));
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddObjectData(md, obj, i);
  }
}

void taiObjectMimeFactory::AddObjectData(QMimeData* md, taBase* obj, int idx) {
    ostringstream ost;
    obj->Save_strm(ost);
    String str = tacss_objectdata + ";index=" + String(idx);
    md->setData(str, QByteArray(ost.str().c_str()));
}


void taiObjectMimeFactory::AddHeaderDesc(taBase* obj, QString& str) {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  str = str + obj->GetTypeDef()->name.toQString() + ";" +
        obj->GetPath().toQString() + ";\n"; // we put ; at end to facilitate parsing, and for }
}

void taiObjectMimeFactory::InitHeader(int cnt, QString& str) {
  str = QString::number(cnt) + ";\n";
}


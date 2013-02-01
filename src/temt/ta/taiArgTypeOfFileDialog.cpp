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

#include "taiArgTypeOfFileDialog.h"
#include <taiWidgetFieldFile>

#include <css_basic_types.h>


void taiArgTypeOfFileDialog::Initialize() {
}

void taiArgTypeOfFileDialog::Destroy() {
}

int taiArgTypeOfFileDialog::BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td) {
  if(argt->IsString() && GetOptionAfter("FILE_DIALOG_", md, aidx) != "")
    return taiArgType::BidForArgType(aidx,argt,md,td)+1;
  return 0;
}

cssEl* taiArgTypeOfFileDialog::GetElFromArg(const char* nm, void*) {
  arg_val = new cssString("", nm);
  arg_base = (void*)&(((cssString*)arg_val)->val);
  return arg_val;
}

taiWidget* taiArgTypeOfFileDialog::GetDataRep_impl(IWidgetHost* host_, taiWidget* par,
                                               QWidget* gui_parent_, int flags_, MemberDef*) {
  String file_act = GetOptionAfter("FILE_DIALOG_");
  taiWidgetFieldFile::FileActionType fact = taiWidgetFieldFile::FA_LOAD;
  if(file_act == "SAVE")
    fact = taiWidgetFieldFile::FA_SAVE;
  else if(file_act == "APPEND")
    fact = taiWidgetFieldFile::FA_APPEND;

  String fext = String(".") + GetOptionAfter("EXT_");
  String ftyp = GetOptionAfter("FILETYPE_");
  int cmpr = GetHasOption("COMPRESS")? 1 : -1;

  return new taiWidgetFieldFile(meth->type, host_, par, gui_parent_, flags_, fact, fext, ftyp, cmpr);
}

void taiArgTypeOfFileDialog::GetImage_impl(taiWidget* dat, const void* base){
  taiWidgetFieldFile* rval = (taiWidgetFieldFile*)dat;
  rval->base_obj = (taBase*)base;
  rval->GetImage(*((String*)arg_base));
}

void taiArgTypeOfFileDialog::GetValue_impl(taiWidget* dat, void*) {
  taiWidgetFieldFile* rval = (taiWidgetFieldFile*)dat;
  *((String*)arg_base) = rval->GetValue();
}

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

#include "taiMemberOfFileDialog.h"
#include <taiWidgetFieldFile>


int taiMemberOfFileDialog::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->type->IsString() && md->OptionAfter("FILE_DIALOG_").nonempty())
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiWidget* taiMemberOfFileDialog::GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  String file_act = mbr->OptionAfter("FILE_DIALOG_");
  taiWidgetFieldFile::FileActionType fact = taiWidgetFieldFile::FA_LOAD;
  if(file_act == "SAVE")
    fact = taiWidgetFieldFile::FA_SAVE;
  else if(file_act == "APPEND")
    fact = taiWidgetFieldFile::FA_APPEND;

  String fext = String(".") + mbr->OptionAfter("EXT_");
  String ftyp = mbr->OptionAfter("FILETYPE_");
  int cmpr = mbr->HasOption("COMPRESS") ? 1 : -1;

  return new taiWidgetFieldFile(mbr->type, host_, par, gui_parent_, flags_, fact, fext, ftyp, cmpr);
}

void taiMemberOfFileDialog::GetImage_impl(taiWidget* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiWidgetFieldFile* rval = (taiWidgetFieldFile*)dat;
  rval->GetImage(*((String*)new_base));
}

void taiMemberOfFileDialog::GetMbrValue_impl(taiWidget* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiWidgetFieldFile* rval = (taiWidgetFieldFile*)dat;
  *((String*)new_base) = rval->GetValue();
}


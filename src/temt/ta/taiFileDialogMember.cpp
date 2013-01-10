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

#include "taiFileDialogMember.h"

int taiFileDialogMember::BidForMember(MemberDef* md, TypeDef* td) {
  if (md->type->InheritsFrom(&TA_taString) && md->OptionAfter("FILE_DIALOG_").nonempty())
    return (taiMember::BidForMember(md,td) + 1);
  return 0;
}

taiData* taiFileDialogMember::GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_, MemberDef*) {
  String file_act = mbr->OptionAfter("FILE_DIALOG_");
  taiFileDialogField::FileActionType fact = taiFileDialogField::FA_LOAD;
  if(file_act == "SAVE")
    fact = taiFileDialogField::FA_SAVE;
  else if(file_act == "APPEND")
    fact = taiFileDialogField::FA_APPEND;

  String fext = String(".") + mbr->OptionAfter("EXT_");
  String ftyp = mbr->OptionAfter("FILETYPE_");
  int cmpr = mbr->HasOption("COMPRESS") ? 1 : -1;

  return new taiFileDialogField(mbr->type, host_, par, gui_parent_, flags_, fact, fext, ftyp, cmpr);
}

void taiFileDialogMember::GetImage_impl(taiData* dat, const void* base){
  void* new_base = mbr->GetOff(base);
  taiFileDialogField* rval = (taiFileDialogField*)dat;
  rval->GetImage(*((String*)new_base));
}

void taiFileDialogMember::GetMbrValue_impl(taiData* dat, void* base) {
  void* new_base = mbr->GetOff(base);
  taiFileDialogField* rval = (taiFileDialogField*)dat;
  *((String*)new_base) = rval->GetValue();
}


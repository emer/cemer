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

#include "taiFileDialogField.h"

#include <iLineEdit>
#include <taFiler>


taiFileDialogField::taiFileDialogField(TypeDef* typ_, IWidgetHost* host_, taiData* par,
                                       QWidget* gui_parent_, int flags_, FileActionType fact,
                                       const String& fext, const String& ftyp, int fcmprs)
  : taiText(typ_, host_, par, gui_parent_, flags_, true,
            "Open a file chooser dialog to select a file name.")
  , file_act(fact)
  , file_ext(fext)
  , file_type(ftyp)
  , file_cmprs(fcmprs)
  , base_obj(0)
{
  setMinCharWidth(40);          // file names are longer in general..
}

void taiFileDialogField::btnEdit_clicked(bool) {
  lookupKeyPressed();           // all the code is there
}

void taiFileDialogField::lookupKeyPressed() {
  taFiler* flr = NULL;
  if(base_obj) {
    String desc = file_type;
    if(desc.empty()) desc = base_obj->GetTypeDef()->name;
    if(file_act == FA_SAVE) {
      flr = base_obj->GetSaveFiler("", file_ext, file_cmprs, desc);
      if (flr->ostrm) {
        rep()->setText(flr->FileName());
      }
    }
    else if(file_act == FA_LOAD) {
      flr = base_obj->GetLoadFiler("", file_ext, file_cmprs, desc);
      if (flr->istrm) {
        rep()->setText(flr->FileName());
      }
    }
    else if(file_act == FA_APPEND) {
      flr = base_obj->GetAppendFiler("", file_ext, file_cmprs, desc);
      if (flr->ostrm) {
        rep()->setText(flr->FileName());
      }
    }
    flr->Close();
    taRefN::unRefDone(flr);
  }
  else {                        // do a static dialog
    taFiler* flr = taBase::StatGetFiler(NULL, file_ext, file_cmprs, file_type);
    taRefN::Ref(flr);
    if(file_act == FA_LOAD) {
      flr->Open();
      if (flr->istrm) {
        rep()->setText(flr->FileName());
      }
    }
    else if(file_act == FA_SAVE) {
      flr->SaveAs();
      if (flr->ostrm) {
        rep()->setText(flr->FileName());
      }
    }
    else if(file_act == FA_APPEND) {
      flr->Append();
      if (flr->ostrm) {
        rep()->setText(flr->FileName());
      }
    }
    flr->Close();
    taRefN::unRefDone(flr);
  }
#ifdef TA_OS_MAC
  // per this bug with 2.8.x on mac, we need to regain focus:  https://bugreports.qt-project.org/browse/QTBUG-22911
  rep()->window()->setFocus();
  rep()->setFocus();
#endif
}

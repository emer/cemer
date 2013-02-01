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

#include "taiFileButton.h"
#include <taiMenu>
#include <taFiler>

#include <taMisc>
#include <taiMisc>

taiFileButton::taiFileButton(TypeDef* typ_, IWidgetHost* host_, taiData* par,
                             QWidget* gui_parent_, int flags_, bool rd_only, bool wrt_only)
  : taiButtonMenu(taiMenu::normal, taiMisc::fonSmall, typ_, host_, par, gui_parent_, flags_)
{
  gf = NULL;
  read_only = rd_only;
  write_only = wrt_only;
  setLabel("------No File-----");
}

taiFileButton::~taiFileButton() {
  SetFiler(NULL);
}

void taiFileButton::SetFiler(taFiler* gf_) {
  if (gf != gf_) {
    if (gf != NULL)
      taRefN::unRefDone(gf);
    gf = gf_;
    if (gf != NULL)
      taRefN::Ref(gf);
  }
}

void taiFileButton::GetImage() {
  if (items.size == 0) {
    if (!write_only)
      AddItem("Open", taiMenu::use_default,
        taiAction::action, this, SLOT(Open()) );
    if(!read_only && ((gf == NULL) || !gf->select_only)) {
      AddItem("Save", taiMenu::use_default,
        taiAction::action, this, SLOT(Save()) );
      AddItem("SaveAs", taiMenu::use_default,
        taiAction::action, this, SLOT(SaveAs()) );
      AddItem("Append", taiMenu::use_default,
        taiAction::action, this, SLOT(Append()) );
    }
    AddItem("Close", taiMenu::use_default,
        taiAction::action, this, SLOT(Close()) );
    AddItem("Edit", taiMenu::use_default,
        taiAction::action, this, SLOT(Edit()) );
  }

  if ((gf == NULL) || (!gf->select_only && !gf->open_file) || gf->FileName().empty() )
    setLabel("------No File-----");
  else
    setLabel(gf->FileName());
}

void taiFileButton::GetGetFile() {
  if (!gf) {
    SetFiler(taFiler::New());
  }
}

void taiFileButton::Open() {
  GetGetFile();
  if ((gf->Open() != NULL) || (gf->select_only)) {
    GetImage();
  }
}

void taiFileButton::Append() {
  GetGetFile();
  if(gf->Append()) {
    GetImage();
  }
}

void taiFileButton::Save() {
  GetGetFile();
  if (gf->Save() != NULL) {
    GetImage();
  }
}

void taiFileButton::SaveAs() {
  GetGetFile();
  if (gf->SaveAs()) {
    GetImage();
  }
}

void taiFileButton::Close() {
  GetGetFile();
  gf->Close();
  if(gf->select_only)
    gf->SetFname("");           // reset file name on close
  GetImage();
}

void taiFileButton::Edit() {
  GetGetFile();
  taMisc::EditFile(gf->FileName());
}

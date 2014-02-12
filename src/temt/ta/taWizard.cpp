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

#include "taWizard.h"

TA_BASEFUNS_CTORS_DEFN(taWizard);

void taWizard::Initialize() {
  auto_open = true;
  SetUserData("NO_CLIP", true);
  SetBaseFlag(NAME_READONLY);
}

void taWizard::InitLinks() {
  inherited::InitLinks();
  wiz_doc.SetName(name);        // same name as us..
  RenderWizDoc();
}

void taWizard::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  wiz_doc.SetName(name);        // same name as us..
}

void taWizard::RenderWizDoc() {
  RenderWizDoc_header();
  RenderWizDoc_impl();
  RenderWizDoc_footer();
  wiz_doc.UpdateText();
}

void taWizard::RenderWizDoc_header() {
  wiz_doc.text = "<html>\n<head></head>\n<body>\n";
}

void taWizard::RenderWizDoc_footer() {
  wiz_doc.text += "</body>\n</html>\n";
}

void taWizard::RenderWizDoc_impl() {
  wiz_doc.text +=
"= taWizard =\n\
this is a virtual base wizard -- not the real thing -- shouldn't see this!\n";
}


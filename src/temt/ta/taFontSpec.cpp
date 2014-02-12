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

#include "taFontSpec.h"
#include <taMisc>

#include <QFontDialog>

TA_BASEFUNS_CTORS_DEFN(taFontSpec);

void taFontSpec::Initialize() {
}

void taFontSpec::InitLinks() {
  inherited::InitLinks();
  if (!taMisc::gui_active) return;
}

void taFontSpec::CutLinks() {
/*obs  if(text_g != NULL) {
    ivResource::unref(text_g);
    text_g = NULL;
  } */
  inherited::CutLinks();
}

bool taFontSpec::Equals(const taFontSpec& b) {
  bool rval = iFont::equals(b);
  return rval;
}

void taFontSpec::SetFont(char* fn) {
  pattern = fn;
  UpdateAfterEdit();
}

void taFontSpec::SetFontSize(int sz) {
  pointSize = sz;
}

void taFontSpec::UpdateAfterEdit(){
  if (!taMisc::gui_active) return;
  if (!pattern.empty()) {
    //legacy value -- from load
#if (defined(TA_OS_WIN))
    if(pattern.contains("-Helvetica-medium-r"))
      pattern.gsub("Helvetica-medium-r", "Arial*medium*");
    if(pattern.contains("-helvetica-medium-r"))
      pattern.gsub("helvetica-medium-r", "Arial*medium*");
    if(pattern.contains("Arial*medium*")) {
      fontName = "Arial";
      String szs = pattern.after("Arial*medium*");
      String nwsz;
      int i;
      for(i=0;i<(int)szs.length();i++) {
        if(!isdigit(szs[i])) continue;
        nwsz += szs[i];
      }
      if(nwsz.empty()) pointSize = 12;
      else if((int)nwsz < 9) pointSize = 9;
      else pointSize = (int)nwsz;
    }
#else
  //TODO: following is just a temporary hack:
    this->setRawName(pattern);
    pattern = _nilString;
#endif
  }
  inherited::UpdateAfterEdit();
}

void taFontSpec::SelectFont() {
  QFont fnt;
  copyTo(fnt);
  bool ok;
  QFontDialog::getFont(&ok, fnt, NULL, NULL);
  if (ok) {
    this->copyFrom(fnt);
    UpdateAfterEdit();
  }
}

void taFontSpec::Copy_(const taFontSpec& cp) {
  pattern = cp.pattern;
  iFont::copy((iFont)cp);
}

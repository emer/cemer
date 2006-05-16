// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "fontspec.h"
#include "ta_qt.h"

#include <qfontdialog.h>

void FontSpec::Initialize() {
}

void FontSpec::InitLinks() {
  inherited::InitLinks();
  if (!taMisc::gui_active) return;
/*obs
#ifndef WINDOWS
  pattern = "*" + (String) fnt->name + "-*";
//TODO???  if (fnt != (iFont*)iFont::lookup(pattern)) {
//    pattern = "*-" + (String) fnt->name() + "-medium-r*";
//  }
#else
  pattern = "*" + (String) fnt->name + "*medium*--10*";
#endif
  prv_pat = pattern; */
}

void FontSpec::CutLinks() {
/*obs  if(text_g != NULL) {
    ivResource::unref(text_g);
    text_g = NULL;
  } */
  inherited::CutLinks();
}

void FontSpec::SetFont(char* fn) {
  pattern = fn;
  UpdateAfterEdit();
}

void FontSpec::SetFontSize(int sz) {
  pointSize = sz;
/* obs
  String szstr = String(sz);
  int len = pattern.length();
  int szpos;
  for(szpos=0;szpos<len;szpos++) {
    if(isdigit(pattern[szpos])) break;
  }
  if(szpos < len) {
    pattern = pattern.before(szpos) + szstr + "*";
  }
  else {
    pattern += szstr + "*";
  }
  UpdateAfterEdit(); */
}

void FontSpec::UpdateAfterEdit(){
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
  /*code was:  const iFont* f = iFont::lookup(pattern);
    if((f==NULL) && pattern.contains("Arial*medium*")) {
      String szs = pattern.after("Arial*medium*");
      String nwsz;
      int i;
      for(i=0;i<(int)szs.length();i++) {
        if(!isdigit(szs[i])) continue;
        nwsz += szs[i];
      }
      if(nwsz.empty()) nwsz = "10";
      else if((int)nwsz <= 9) nwsz = "8";
      pattern = "*-Helvetica-medium-r*" + nwsz + "*";
      f = iFont::lookup(pattern);
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-medium-r*"));
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-r*"));
    }
    if(f==NULL){
      f = iFont::lookup(String( "*-" + pattern + "-*"));
    }
    if(f==NULL){
      taMisc::Error("Cannot find font: ", pattern);
      return;
    }
    prv_pat = pattern;
    if(fnt == f) return;
    ivResource::unref(fnt);
    fnt = (iFont*)f;
    ivResource::ref(fnt);
  */
  //TODO: following is just a temporary hack:
    this->setRawName(pattern);
    pattern = _nilString;
#endif
  }
  inherited::UpdateAfterEdit();
}

void FontSpec::SelectFont() {
  QFont fnt;
  copyTo(fnt);
  bool ok;
  QFontDialog::getFont(&ok, fnt, NULL, NULL);
  if (ok) {
    this->copyFrom(fnt);
    UpdateAfterEdit();
  }
}

void FontSpec::Copy_(const FontSpec& cp) {
  pattern = cp.pattern;
  iFont::copy((iFont)cp);
}


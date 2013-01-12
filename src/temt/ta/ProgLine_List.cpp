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

#include "ProgLine_List.h"
#include <ViewColor_List>
#include <iColor>
#include <taMisc>


void ProgLine_List::Initialize() {
  SetBaseType(&TA_ProgLine);
}

bool ProgLine_List::AddLine(taBase* prog_el, int indent, const String& code,
                            int pline_flags) {
  ProgLine* pl = (ProgLine*)New(1);
  pl->indent = indent;
  pl->code = code;
  pl->prog_el = prog_el;
  pl->flags = (ProgLine::PLineFlags)pline_flags;
  return true;
}

void ProgLine_List::FullListing(String& script_code) {
  for(int i=1; i<size; i++) {
    ProgLine* pl = FastEl(i);
    const String ci = pl->CodeIndented();
    script_code.cat(ci).cat('\n');
  }
}

void ProgLine_List::FullListingHTML(String& script_code) {
  for(int i=1; i<size; i++) {
    ProgLine* pl = FastEl(i);
    String ci = "<p style=\"text-indent:" + String(pl->indent*10) + "px";
    if(pl->prog_el) {
      String dec_key = pl->prog_el->GetTypeDecoKey(); // nil if none
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        if(vc->use_fg) {
          String hex_color = vc->fg_color.color().toString();
          ci += "; color:#" + hex_color;
        }
      }
      dec_key = pl->prog_el->GetStateDecoKey(); // nil if none
      vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        if(vc->use_bg) {
          String hex_color = vc->bg_color.color().toString();
          ci += "; background-color:#" + hex_color;
        }
      }
    }
    String cd = pl->code;
    cd.xml_esc();
    ci += "\">" + cd + "</p>\n";
    script_code.cat(ci);
  }
}

int ProgLine_List::FindProgEl(taBase* prog_el, bool reverse) {
  if(reverse) {
    for(int i=size-1; i>=1; i--) {
      ProgLine* pl = FastEl(i);
      if(pl->prog_el == prog_el && !pl->HasPLineFlag(ProgLine::COMMENT)) return i;
    }
  }
  else {
    for(int i=1; i<size; i++) {
      ProgLine* pl = FastEl(i);
      if(pl->prog_el == prog_el && !pl->HasPLineFlag(ProgLine::COMMENT)) return i;
    }
  }
  return -1;
}

int ProgLine_List::FindMainLine(taBase* prog_el) {
  for(int i=1; i<size; i++) {
    ProgLine* pl = FastEl(i);
    if(pl->prog_el == prog_el && pl->IsMainLine()) return i;
  }
  return -1;
}

bool ProgLine_List::SetBreakpoint(int line_no) {
  if(!InRange(line_no)) return false;
  ProgLine* pl = FastEl(line_no);
  pl->SetBreakpoint();
  return true;
}

void ProgLine_List::ClearAllBreakpoints() {
  for(int i=1; i<size; i++) {
    ProgLine* pl = FastEl(i);
    pl->ClearBreakpoint();
  }
}

void ProgLine_List::ClearAllErrors() {
  for(int i=1; i<size; i++) {
    ProgLine* pl = FastEl(i);
    pl->ClearError();
    pl->ClearWarning();
  }
}

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

#include "TokenSpace.h"
#include <TypeDef>
#include <taMisc>

#ifndef NO_TA_BASE
#include <taBase>
#include <taSigLink>
#endif

String TokenSpace::tmp_el_name;

void TokenSpace::Initialize() {
  owner = NULL;
  keep = false;
  sub_tokens = 0;
#ifndef NO_TA_BASE
  sig_link = NULL;
#endif
}

TokenSpace::~TokenSpace() {
//  Reset(); //note: TokenSpace never had a Reset, but maybe it should...
#ifndef NO_TA_BASE
  if (sig_link) {
    sig_link->SigDestroying(); // link NULLs our pointer
  }
#endif
}

String TokenSpace::El_GetName_(void* it) const {
#ifndef NO_TA_BASE
  if((owner == NULL) || !(owner->IsTaBase())) {
#else
  if(owner == NULL) {
#endif
    tmp_el_name = String((ta_intptr_t)it);
    return tmp_el_name;
  }
#ifndef NO_TA_BASE
  taBase* tmp = (taBase*)it;
  return tmp->GetName();
#else
  return _nilString;
#endif
}

String& TokenSpace::Print(String& strm, int indent) const {
  if(owner == NULL) return strm;

  taMisc::IndentString(strm, indent);
  strm << "Tokens of type: " << owner->name << " (" << size
       << "), sub-tokens: " << sub_tokens;
  if(keep)
    strm << "\n";
  else
    strm << " (not keeping tokens)\n";

  return taPtrList<void>::Print(strm, indent);
}


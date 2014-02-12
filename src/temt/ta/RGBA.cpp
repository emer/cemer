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

#include "RGBA.h"
#include <taColor>
#include <taMisc>

TA_BASEFUNS_CTORS_LITE_DEFN(RGBA);


RGBA::RGBA(float rd, float gr, float bl, float al) {
  Initialize();
  r = rd; g = gr; b = bl; a = al;
}

void RGBA::Initialize() {
  name = _nilString;
  r = g = b = 0.0f;
  a = 1.0f;
}

void RGBA::Destroy() {
}

void RGBA::UpdateAfterEdit_impl(){
  taOBase::UpdateAfterEdit_impl(); // skip nBase guy
  //  if(!taMisc::gui_active)    return;
  if (!name.empty()) {
    if(name.contains("RGBA_")) {
      name = _nilString; // not legal name
    }
    else {
      if(!(iColor::find((char*)name,r,g,b))){
	taMisc::Error("Color: " , name , " not found for this display");
      }
    }
  }
}

void RGBA::Copy_(const RGBA& cp) {
  name = cp.name;		// copy name because it goes with r,g,b
  r = cp.r;
  g = cp.g;
  b = cp.b;
  a = cp.a;
}

void RGBA::CanCopyCustom_impl(bool to, const taBase* cp,
    bool quiet, bool& allowed, bool& forbidden) const
{
  // we handle both dirs, so don't check
  if (cp->InheritsFrom(&TA_taColor)) {
    allowed = true;
  } else 
    inherited::CanCopyCustom_impl(to, cp, quiet, allowed, forbidden);
}

void RGBA::CopyFromCustom_impl(const taBase* cp_)
{
  const taColor* cp = dynamic_cast<const taColor*>(cp_);
  if (cp) {
    r = cp->r;
    g = cp->g;
    b = cp->b;
    a = cp->a;
  } else
    inherited::CopyFromCustom_impl(cp_);
}

void RGBA::CopyToCustom_impl(taBase* to_) const
{
  taColor* to = dynamic_cast<taColor*>(to_);
  if (to) {
    to->r = r;
    to->g = g;
    to->b = b;
    to->a = a;
  } else
    inherited::CopyToCustom_impl(to_);
}


const iColor RGBA::color() const {
  return iColor(r, g, b, a);
}

void RGBA::setColor(const iColor& cp) {
  Set(cp.redf(), cp.greenf(), cp.bluef(), cp.alphaf());
}

void RGBA::setColorName(const String& nm) {
  name = nm;
  UpdateAfterEdit();
}

String RGBA::ToString_RGBA() const {
  return String("R:") + String(r) +
    String("; G:") + String(g) +
    String("; B:") + String(b) +
    String("; A:") + String(a);
}

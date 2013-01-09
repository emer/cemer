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

#include "T3NodeLeaf.h"

SO_NODE_SOURCE(T3NodeLeaf);

void T3NodeLeaf::initClass()
{
  SO_NODE_INIT_CLASS(T3NodeLeaf, T3Node, "");
}

T3NodeLeaf::T3NodeLeaf(T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NodeLeaf);

  initCommon();
}

T3NodeLeaf::~T3NodeLeaf()
{
}

SoFont* T3NodeLeaf::captionFont(bool auto_create) {
  SoSeparator* cs = captionSeparator(auto_create);
  if (!cs) return NULL;
  SoFont* rval = (SoFont*)getNodeByName(cs, "captionFont");;
  if (!rval && auto_create) {
    rval = new SoFont();
    rval->setName("captionFont");
    rval->name = (const char*)taMisc::t3d_font_name;
    cs->insertChild(rval, 0);
  }
  return rval;
}

SoSeparator* T3NodeLeaf::captionSeparator(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoSeparator* rval = (SoSeparator*)getNodeByName(ts, "captionSeparator");
  if (!rval && auto_create) {
    rval = new SoSeparator;
    rval->setName("captionSeparator");
    insertChildAfter(ts, rval, transform());
  }
  return rval;
}

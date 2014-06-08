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

#include "T3NodeParent.h"

#include <taMisc>

#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoTransform.h>


SO_NODE_SOURCE(T3NodeParent);

void T3NodeParent::initClass()
{
  SO_NODE_INIT_CLASS(T3NodeParent, T3Node, "");
}

T3NodeParent::T3NodeParent(T3DataView* dataView_)
  : inherited(dataView_)
{
  SO_NODE_CONSTRUCTOR(T3NodeParent);

  childNodes_ = NULL;

  shapeSeparator_ = new SoSeparator;
  shapeSeparator_->setName("shapeSeparator");
  topSeparator()->addChild(shapeSeparator_);

  initCommon();

}

T3NodeParent::~T3NodeParent()
{
  shapeSeparator_ = NULL;
  childNodes_ = NULL;
}

void T3NodeParent::addRemoveChildNode(SoNode* node, bool adding) {
  if (adding)
    childNodes()->addChild(node);
  else {
    int idx = childNodes()->findChild(node);
    if(idx >= 0) {
      childNodes()->removeChild(idx);
    }
  }
}

SoSeparator* T3NodeParent::captionSeparator(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoSeparator* rval = (SoSeparator*)getNodeByName(ts, "captionSeparator");
  if (!rval && auto_create) {
    rval = new SoSeparator;
    rval->setName("captionSeparator");
    // node goes after font, if any, else after shape s
    SoNode* after = getNodeByName(ts, "captionFont");
    if (!after)
      after = shapeSeparator();
    insertChildAfter(ts, rval, after);
  }
  return rval;
}

SoSeparator* T3NodeParent::childNodes() {
  if (!childNodes_) {
    childNodes_ = new SoSeparator;
    childNodes_->setName("childNodes");
    topSeparator()->addChild(childNodes_);
  }
  return childNodes_;
}

SoFont* T3NodeParent::captionFont(bool auto_create) {
  SoSeparator* ts = topSeparator();
  SoFont* rval = (SoFont*)getNodeByName(ts, "captionFont");;
  if (!rval && auto_create) {
    rval = new SoFont();
    rval->setName("captionFont");
    rval->name = (const char*)taMisc::t3d_font_name;
    insertChildAfter(ts, rval, shapeSeparator());
  }
  return rval;
}


// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

#include "imisc_so.h"

#include <Inventor/misc/SoBase.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/nodes/SoTransform.h>

//////////////////////////
//   T3RenderHelper	//
//////////////////////////

void T3RenderHelper::moveTo(float x, float y, float z) {
  checkIssueTransform(x, y, z);
}

void T3RenderHelper::render(SoNode* node) {
  par->addChild(node);
}

void T3RenderHelper::render(SoNode* node, float x, float y, float z) {
  checkIssueTransform(x, y, z);
  render(node);
}

void T3RenderHelper::checkIssueTransform(float x, float y, float z) {
  if (cur.isEqual(x, y, z)) return;
  SoTransform* xf = new SoTransform();
  xf->translation.setValue(x - cur.x, y - cur.y, z - cur.z);
  par->addChild(xf);
  cur.setValue(x, y, z);
}

void T3RenderHelper::setOrigin(float x, float y, float z) {
  cur.setValue(x, y, z);
}


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

#include "BrowseViewer.h"

void BrowseViewer::Initialize() {
  root_typ = NULL;
  root_md = NULL;
}

void BrowseViewer::Copy_(const BrowseViewer& cp) {
  root_typ = cp.root_typ;
  root_md = cp.root_md;
}

void BrowseViewer::Render_pre() {
  widget()->ApplyRoot();
}

/* nn void BrowseViewer::TreeNodeDestroying(taiTreeDataNode* item) {
  if (!m_window) return;
  browser_win()->TreeNodeDestroying(item);
} */



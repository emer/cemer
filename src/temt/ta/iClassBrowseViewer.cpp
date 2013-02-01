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

#include "iClassBrowseViewer.h"
#include <taiTreeDataNode>
#include <taSigLinkClass>
#include <MainWindowViewer>



iClassBrowseViewer::iClassBrowseViewer(ClassBrowseViewer* browser_, QWidget* parent)
:inherited((BrowseViewer*)browser_, parent)
{
}

iClassBrowseViewer::~iClassBrowseViewer()
{
}

void iClassBrowseViewer::mnuNewBrowser(iAction* mel) {
  taiTreeDataNode* node = (taiTreeDataNode*)(mel->usr_data.toPtr());
  taSigLinkClass* dl = static_cast<taSigLinkClass*>(node->link());
  MainWindowViewer* brows = MainWindowViewer::NewClassBrowser(dl->data(),
     dl->GetDataTypeDef(), dl->GetDataMemberDef());
  if (!brows) return;
  brows->ViewWindow();
}


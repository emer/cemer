// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "nvDataView.h"

TypeDef_Of(NetView);

NetView* nvDataView::getNetView()
{
  if (!m_nv) {
    // Cache the value on the first request.
    m_nv = GET_MY_OWNER(NetView);
  }
  return m_nv;
}

void nvDataView::Initialize()
{
  m_nv = NULL;
}

void nvDataView::CutLinks()
{
  m_nv = NULL;
  inherited::CutLinks();
}


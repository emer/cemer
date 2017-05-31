// Copyright 2017, Regents of the University of Colorado,
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

#include "taSmartRef.h"
#include <taSigLink>

void taSmartRef::SigLinkRecv(taSigLink*, int sls, void* op1, void* op2) {
  if (m_own) {
    m_own->SmartRef_SigEmit(this, m_ptr, sls, op1, op2);
  }
}
void taSmartRef::SigLinkDestroying(taSigLink* dl) {
  if (m_own) {
    taBase* tmp_ptr = m_ptr;
    m_ptr = NULL;
    //send a changing ref, in case it only monitors for setting/clearing (not destroying)
    m_own->SmartRef_SigChanging(this, NULL, false);
    m_own->SmartRef_SigDestroying(this, tmp_ptr);
    //NO MORE CODE HERE -- object may have destroyed itself
  }
  else {
    m_ptr = NULL;
  }
}

void taSmartRef::SigChanging(taBase* obj, bool setting) {
  if (m_own) {
    m_own->SmartRef_SigChanging(this, obj, setting);
  }
}


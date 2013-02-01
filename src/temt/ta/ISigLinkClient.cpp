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

#include "ISigLinkClient.h"
#include <taSigLink>
#include <taMisc>

ISigLinkClient::~ISigLinkClient() {
  if (m_link) {
    //NOTE: since this is destructor, the 'this' we pass is our *own* virtual v-table
    // version, therefore, RemoveDataClient may NOT use any of our virtual or pure-virtual methods
    m_link->RemoveDataClient(this); //nulls our ref
  }
}

bool ISigLinkClient::AddDataLink(taSigLink* dl) {
  if (m_link && (m_link != dl)) {
    taMisc::DebugInfo("ISigLinkClient::AddDataLink: a link has already been set!");
  }
  bool r = (0 == m_link);
  m_link = dl;
  return r;
}

bool ISigLinkClient::RemoveDataLink(taSigLink* dl) {
  bool r = (0 != m_link);
  m_link = NULL;
  return r;
}


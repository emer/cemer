// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#include "ta_data.h"

#include <QDir>

/* prob nn
/////////////////////////
//  SequenceMaster     //
/////////////////////////

// Note: this singleton uses the safe singleton pattern

SequenceMaster& SequenceMaster::in
/* prob nn
/////////////////////////
//  SequenceMaster     //
/////////////////////////

// Note: this singleton uses the safe singleton pattern

SequenceMaster& SequenceMaster::instance() {
  static SequenceMaster	s_instance; // actually initialized first time through here
  return s_instance;
}
  
SequenceMaster::SequenceMaster()
:m_cycle(1)
{
}stance() {
  static SequenceMaster	s_instance; // actually initialized first time through here
  return s_instance;
}
  
SequenceMaster::SequenceMaster()
:m_cycle(1)
{
}

SequenceMaster::~SequenceMaster()
{
}

void SequenceMaster::Next() {
  ++m_cycle;
}

bool SequenceMaster::UpdateClient(int64_t& client_cycle, int64_t* diff) {
  // updates a client's own cycle count, returning 'true' if it was out of date, and optionally the (+ve) number of cycles by which it was stale
  if (m_cycle <= client_cycle) return false;
  if (diff != NULL) *diff = m_cycle - client_cycle;
  client_cycle = m_cycle;
  return true;
}
*/



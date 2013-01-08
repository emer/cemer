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

#ifndef taSubGroup_h
#define taSubGroup_h 1

// parent includes:
#include <taList_taGroup_impl_>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 


class   TA_API taSubGroup : public TALOG {
  // #INSTANCE ##NO_TOKENS ##NO_UPDATE_AFTER has the sub-groups for a group
INHERITED(TALOG)
public:
  override  void DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // forward LIST events as GROUP events to owner

  bool  Transfer(taBase* item);

  TA_BASEFUNS_NOCOPY(taSubGroup);
private:
  void  Initialize()    { };
  void  Destroy()       { };
};

#endif // taSubGroup_h

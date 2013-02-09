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
#include "ta_def.h"
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class taGroup_impl; //

taTypeDef_Of(taGroup_List);

class TA_API taGroup_List : public taList<taGroup_impl> {
  // #INSTANCE ##NO_TOKENS ##NO_UPDATE_AFTER a list of groups
  INHERITED(taList<taGroup_impl>)
public:
  TA_BASEFUNS_NOCOPY(taGroup_List);
private:
  void  Initialize()    { };
  void  Destroy()       { };
};

taTypeDef_Of(taSubGroup);

class TA_API taSubGroup : public taGroup_List {
  // #INSTANCE ##NO_TOKENS ##NO_UPDATE_AFTER has the sub-groups for a group
  INHERITED(taGroup_List)
public:
  override  void SigEmit(int sls, void* op1 = NULL, void* op2 = NULL);
  // forward LIST events as GROUP events to owner

  bool  Transfer(taBase* item);

  TA_BASEFUNS_NOCOPY(taSubGroup);
private:
  void  Initialize()    { };
  void  Destroy()       { };
};

#endif // taSubGroup_h

// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef NetMonItem_List_h
#define NetMonItem_List_h 1

// parent includes:
#include "network_def.h"
#include <taList>
#include <NetMonItem>

// member includes:

// declare all other types mentioned but not required to include:

eTypeDef_Of(NetMonItem_List);

class E_API NetMonItem_List: public taList<NetMonItem> { 
  // ##CAT_Network list of network monitor items
INHERITED(taList<NetMonItem>)
public:
  bool              ignore_sig;
  int                 NumListCols() const override {return 3;}
  const KeyString   GetListColKey(int col) const override;
  String            GetColHeading(const KeyString&) const override; // header text for the indicated column

  void              SigEmit(int sls, void* op1 = NULL, void* op2 = NULL) override;

  TA_BASEFUNS_NOCOPY(NetMonItem_List);
  
private:
  void          Initialize() { SetBaseType(&TA_NetMonItem); ignore_sig = false; }
  void          Destroy() {}
};

#endif // NetMonItem_List_h

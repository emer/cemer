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

#ifndef KwtaSortBuff_List_h
#define KwtaSortBuff_List_h 1

// parent includes:
#include <KwtaSortBuff>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class taHashTable; // 


eTypeDef_Of(KwtaSortBuff_List);

class E_API KwtaSortBuff_List: public taList<KwtaSortBuff> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Network ##NO_EXPAND_ALL list of kwta sort buffs -- a specific number are defined as standard per the enum
INHERITED(taList<KwtaSortBuff>)
public:
  enum StdSortBuffs {
    ACTIVE,			// standard kwta active list
    INACT,			// standard kwta inactive list
    ACTIVE_2K,			// kv2k active list
    INACT_2K,			// kv2k inactive list
    N_BUFFS,			// total number of standard buffers
  };

  void		ResetAllBuffs();
  // reset all the sort buffers on the list
  void		AllocAllBuffs(int nunits, int ngps);
  // allocate storage to given number of units and groups, and initialize all sizes to 0

  void		CreateStdBuffs();
  // allocate N_BUFFS items on this list

  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "Layer"; }

  NOCOPY(KwtaSortBuff_List)
  TA_BASEFUNS(KwtaSortBuff_List);
private:
  void	Initialize() 		{ SetBaseType(&TA_KwtaSortBuff); }
  void 	Destroy()		{ };
};

#endif // KwtaSortBuff_List_h

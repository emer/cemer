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

#ifndef NetMonItem_List_h
#define NetMonItem_List_h 1

// parent includes:
#include <taList>
#include <NetMonItem>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(NetMonItem_List);

class EMERGENT_API NetMonItem_List: public taList<NetMonItem> { 
  // ##CAT_Network list of network monitor items
INHERITED(taList<NetMonItem>)
public:

  int	NumListCols() const {return 3;} 
  const KeyString GetListColKey(int col) const;
  String GetColHeading(const KeyString&) const; // header text for the indicated column
  TA_BASEFUNS_NOCOPY(NetMonItem_List);
  
private:
  void		Initialize() {SetBaseType(&TA_NetMonItem);}
  void		Destroy() {}
};

#endif // NetMonItem_List_h

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

#ifndef ParamSetItem_Group_h
#define ParamSetItem_Group_h 1

// parent includes:
#include <ParamSetItem>
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ParamSetItem_Group);

class TA_API ParamSetItem_Group : public taGroup<ParamSetItem> {
  // ##CAT_Display group of param set items
INHERITED(taGroup<ParamSetItem>)
public:
  
#ifdef __MAKETA__ // dummy to supress New
  taBase*       New_gui(int n_objs=1, TypeDef* typ = NULL); // #NO_SHOW
#endif

  TA_BASEFUNS_NOCOPY(ParamSetItem_Group);
private:
  void  Initialize()  { SetBaseType(&TA_ParamSetItem);}
  void  Destroy()     { };
};

#endif // ParamSetItem_Group_h

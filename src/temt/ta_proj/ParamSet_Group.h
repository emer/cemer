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

#ifndef ParamSet_Group_h
#define ParamSet_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class ParamSet; //

taTypeDef_Of(ParamSet);
taTypeDef_Of(ParamSet_Group);

class TA_API ParamSet_Group : public taGroup<ParamSet> {
  // ##CAT_Display ##EXPAND_DEF_1 a group of param sets
  INHERITED(taGroup<ParamSet>)
public:
  virtual void  RestorePanels();
  // if the panel was pinned when the program was saved redisplay it on project open
  virtual bool  LoadParamSet(const String& set_name);

  String       GetTypeDecoKey() const override { return "ParamSet"; }

TA_BASEFUNS(ParamSet_Group);
private:
NOCOPY(ParamSet_Group)
void  Initialize() { SetBaseType(&TA_ParamSet);}
void  Destroy()               { };
};

#endif // ParamSet_Group_h

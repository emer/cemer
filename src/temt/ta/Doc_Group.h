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

#ifndef Doc_Group_h
#define Doc_Group_h 1

// parent includes:
#include <taGroup>

// member includes:

// declare all other types mentioned but not required to include:
class taDoc; //

taTypeDef_Of(taDoc); 
taTypeDef_Of(Doc_Group);

class TA_API Doc_Group : public taGroup<taDoc> {
  // ##CAT_Docs ##EXPAND_DEF_1 group of doc objects
INHERITED(taGroup<taDoc>)
public:
  virtual void          AutoEdit();

  String       GetTypeDecoKey() const override { return "Doc"; }
  TA_BASEFUNS(Doc_Group);
private:
  NOCOPY(Doc_Group)
  void  Initialize()            { SetBaseType(&TA_taDoc); }
  void  Destroy()               { };
};

#endif // Doc_Group_h

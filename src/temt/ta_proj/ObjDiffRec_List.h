// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef ObjDiffRec_List_h
#define ObjDiffRec_List_h 1

// parent includes:
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class ObjDiffRec; //

taTypeDef_Of(ObjDiffRec_List);

class TA_API ObjDiffRec_List: public taList<ObjDiffRec> {
  // #CAT_ObjDiff list of object difference records
INHERITED(taList<ObjDiffRec>)
public:

  void          SetAllFlag(int flg);
  // set ObjDiffRec::DiffFlag flag value for all items on the list
 
  TA_BASEFUNS_NOCOPY(ObjDiffRec_List);
private:
  void  Initialize();
  void  Destroy()               { };
};

#endif // ObjDiffRec_List_h

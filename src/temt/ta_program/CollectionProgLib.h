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

#ifndef CollectionProgLib_h
#define CollectionProgLib_h 1

// parent includes:
#include <taNBase>
#include <ProgLib>
#include <NameVar>
#include <NameVar_PArray>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; //
class ProgLibEl; //
class Program_Group; //
class Program; //


taTypeDef_Of(CollectionProgLib);

class TA_API CollectionProgLib : public ProgLib {
  // #INSTANCE #INLINE #CAT_Program the program library as a collection of sub ProgLibs, such as system, user, wiki prog lib into a combined prog lib
INHERITED(ProgLib)
public:
  ProgLib_List          sub_libs; // #NO_SAVE ProgLib libraries within this collection 
  
  void  FindPrograms() override;
  bool  SaveProgToProgLib(Program* prg, ProgLibs library) override;
  bool  SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) override;

  virtual ProgLib*      GetSubLib(ProgLibs library);
  // get program library based on enum type
  virtual void          CreateSubLibs();
  // #IGNORE make the sub_libs for this collection -- automtically called in FindPrograms
  
  TA_SIMPLE_BASEFUNS(CollectionProgLib);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // CollectionProgLib_h

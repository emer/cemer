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

#ifndef FileProgLib_h
#define FileProgLib_h 1

// parent includes:
#include <ProgLib>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class ProgLibEl; // 
class Program_Group; // 
class Program; //

taTypeDef_Of(FileProgLib);

class TA_API FileProgLib: public ProgLib {
  // program library for programs located in file system
INHERITED(ProgLib)
public:
  String  path;                 // path to search for programs
 
  void  FindPrograms() override;
  bool  SaveProgToProgLib(Program* prg, ProgLibs library) override;
  bool  SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) override;

  FileProgLib(const String& path, const String& lib_name);
  TA_SIMPLE_BASEFUNS(FileProgLib);
private:
  void  Initialize();
  void  Destroy() { CutLinks(); }
};

#endif // FileProgLib_h

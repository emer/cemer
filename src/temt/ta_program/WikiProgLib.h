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

#ifndef WikiProgLib_h
#define WikiProgLib_h 1

// parent includes:
#include <taNBase>
#include <ProgLib>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(WikiProgLib);

class TA_API WikiProgLib : public ProgLib {
  // program library for programs located on a wiki
INHERITED(ProgLib)
public:
  String   wiki_name;            // name of wiki -- looked up in taMisc::wiki_url's

  void  FindPrograms() override;
  bool  SaveProgToProgLib(Program* prg, ProgLibs library) override;
  bool  SaveProgGrpToProgLib(Program_Group* prg_grp, ProgLibs library) override;

  virtual String        GetLocalCacheDir();
  // get the local file cache dir for programs downloaded from this wiki
  
  WikiProgLib(const String& wiki_name, const String& lib_name);
  TA_SIMPLE_BASEFUNS(WikiProgLib);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // WikiProgLib_h

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

#ifndef Program_List_h
#define Program_List_h 1

// parent includes:
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:
class Program; //

class TA_API Program_List : public taList<Program> {
  // ##CAT_Program a list of programs
  INHERITED(taList<Program>)
public:

  override String       GetTypeDecoKey() const { return "Program"; }
  TA_BASEFUNS_NOCOPY(Program_List);
private:
  void  Initialize();
  void  Destroy()               {Reset(); }; //
}; //

#endif // Program_List_h

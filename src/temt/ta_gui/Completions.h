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

#ifndef Completions_h
#define Completions_h 1

// parent includes:
#include <taNBase>
#include <String_Array>
#include <taBase_List>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(Completions);

class TA_API Completions : public taNBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taNBase)
public:
  String_Array            string_completions;
  taBase_List             object_completions;
    TA_SIMPLE_BASEFUNS(Completions);
  
protected:

private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // Completions_h

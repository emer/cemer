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

#ifndef ProgBrkPt_List_h
#define ProgBrkPt_List_h 1

// parent includes:
#include <ProgBrkPt>
#include <taList>

// member includes:
#include <ProgEl>

// declare all other types mentioned but not required to include:



taTypeDef_Of(ProgBrkPt_List);

class TA_API ProgBrkPt_List : public taList<ProgBrkPt> {
  // A list of breakpoints across all project programs to support viewing and enabling from one window
INHERITED(taList<ProgBrkPt> )
public:
  void            AddBrkPt(ProgEl* prog_el);
  void            DeleteBrkPt(ProgEl* prog_el);
  ProgBrkPt*      FindBrkPt(ProgEl* prog_el) const;


  TA_SIMPLE_BASEFUNS(ProgBrkPt_List);
private:
  void Initialize();
  void Destroy()     { };
};

#endif // ProgBrkPt_List_h

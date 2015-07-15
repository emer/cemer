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

#ifndef T3Annotation_List_h
#define T3Annotation_List_h 1

// parent includes:
#include <taNBase>
#include <taList>
#include <T3Annotation>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(T3Annotation_List);

class TA_API T3Annotation_List : public taList<T3Annotation> {
  // ##CAT_Gui list of gui annotation objects
INHERITED(taList<T3Annotation>)
public:

  TA_SIMPLE_BASEFUNS(T3Annotation_List);
private:
  void Initialize()  { SetBaseType(&TA_T3Annotation); }
  void Destroy()     { };
};

#endif // T3Annotation_List_h

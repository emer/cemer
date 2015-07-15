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

#ifndef ControlItemNote_h
#define ControlItemNote_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ControlItemNote);

class TA_API ControlItemNote : public taOBase {
  // #INLINE required by mashup to be a separate class even though it is a single String

  INHERITED(taOBase)
public:
  String            notes;  // #EDIT_DIALOG #EDIT_WIDTH_40 a place to comment on the effect of parameter or any other comment

  TA_SIMPLE_BASEFUNS(ControlItemNote);
private:
  void Initialize()  { };
  void Destroy()     { };
};

#endif // ControlItemNote_h

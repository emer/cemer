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

#ifndef ParamSetItem_h
#define ParamSetItem_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(ParamSetItem);

class TA_API ParamSetItem : public taOBase {
// #INLINE a saved value for a control panel item
  INHERITED(taOBase)
public:
  String            saved_value;  // #READ_ONLY #SHOW the value for this parameter when this param set is invoked
  
  TA_SIMPLE_BASEFUNS(ParamSetItem);
protected:
  void              UpdateAfterEdit_impl();

private:
  void  Initialize()  { };
  void  Destroy()     { };
};

#endif // ParamSetItem_h

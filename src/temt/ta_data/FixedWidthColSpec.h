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

#ifndef FixedWidthColSpec_h
#define FixedWidthColSpec_h 1

// parent includes:
#include <taNBase>

// smartptr, ref includes
#include <taSmartRefT>
#include <taSmartPtrT>

// member includes:

// declare all other types mentioned but not required to include:
class DataCol;


taTypeDef_Of(FixedWidthColSpec);

class TA_API FixedWidthColSpec : public taNBase {
  // ##CAT_Data #STEM_BASE single column spec for fixed width loading of scalar cols
INHERITED(taNBase)
friend class FixedWidthSpec;
public:
  int           start_col; // #MIN_1 (1-based) starting column of the field
  int           col_width; // width of the column -- use -1 for "rest of line"
  TA_BASEFUNS(FixedWidthColSpec);
protected:
  DataCol*      col; // #IGNORE only valid during the load
  virtual void  WriteData(const String& val); //writes to the col

private:
  SIMPLE_COPY(FixedWidthColSpec)
  void  Initialize();
  void  Destroy()               {}
};

TA_SMART_PTRS(TA_API, FixedWidthColSpec); //

#endif // FixedWidthColSpec_h

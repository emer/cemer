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

#ifndef taiStreamArgType_h
#define taiStreamArgType_h 1

// parent includes:
#include <taiArgType>

// member includes:

// declare all other types mentioned but not required to include:
class taFiler; //


TypeDef_Of(taiStreamArgType);

class TA_API taiStreamArgType : public taiArgType {
  // for ios derived args (uses a file-requestor)
  TAI_ARGTYPE_SUBCLASS(taiStreamArgType, taiArgType);
public:
  taFiler*      gf;

  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);
  virtual void  GetValueFromGF(); // actually get the value from the getfile

private:
  void Initialize();
  void Destroy();
};

#endif // taiStreamArgType_h

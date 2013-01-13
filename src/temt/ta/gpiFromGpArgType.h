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

#ifndef gpiFromGpArgType_h
#define gpiFromGpArgType_h 1

// parent includes:
#include <taiTokenPtrArgType>

// member includes:

// declare all other types mentioned but not required to include:
class taList_impl; //


class TA_API gpiFromGpArgType : public taiTokenPtrArgType {
  // for taBase pointers with FROM_GROUP_xxx
  TAI_ARGTYPE_SUBCLASS(gpiFromGpArgType, taiTokenPtrArgType);
public:
  int           BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  cssEl*        GetElFromArg(const char* arg_nm, void* base);
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiData* dat, const void* base);
  void          GetValue_impl(taiData* dat, void* base);

  virtual MemberDef*    GetFromMd();
  virtual taList_impl*       GetList(MemberDef* from_md, const void* base);

private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // gpiFromGpArgType_h

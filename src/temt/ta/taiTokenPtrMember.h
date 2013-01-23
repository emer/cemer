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

#ifndef taiTokenPtrMember_h
#define taiTokenPtrMember_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; //


TypeDef_Of(taiTokenPtrMember);

class TA_API taiTokenPtrMember : public taiMember {
  // for taBase pointer members (allows scoping by owner obj)
  TAI_MEMBER_SUBCLASS(taiTokenPtrMember, taiMember);
public:
  enum Mode {
    MD_BASE,            // taBase pointer
    MD_SMART_PTR,       // taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF        // taSmartRef
  };

  override bool handlesReadOnly() const { return true; }
  TypeDef*      GetMinType(const void* base);
  taBase*       GetTokenPtr(const void* base) const; // depends on mode
  int           BidForMember(MemberDef* md, TypeDef* td);
protected:
  Mode          mode; // set during first GetDataRep (is garbage until then)
  taiData*      GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  override void GetImage_impl(taiData* dat, const void* base);
  override void GetMbrValue_impl(taiData* dat, void* base);
private:
  void          Initialize() {}
  void          Destroy() {}
};

#endif // taiTokenPtrMember_h

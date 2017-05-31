// Copyright 2017, Regents of the University of Colorado,
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

#ifndef taiMemberOfTokenPtr_h
#define taiMemberOfTokenPtr_h 1

// parent includes:
#include <taiMember>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; //


taTypeDef_Of(taiMemberOfTokenPtr);

class TA_API taiMemberOfTokenPtr : public taiMember {
  // for taBase pointer members (allows scoping by owner obj)
  TAI_MEMBER_SUBCLASS(taiMemberOfTokenPtr, taiMember);
public:
  enum Mode {
    MD_BASE,            // taBase pointer
    MD_SMART_PTR,       // taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF        // taSmartRef
  };

  bool handlesReadOnly() const override { return true; }
  TypeDef*      GetMinType(const void* base);
  taBase*       GetTokenPtr(const void* base) const; // depends on mode
  int           BidForMember(MemberDef* md, TypeDef* td) override;
protected:
  Mode          mode; // set during first GetWidgetRep (is garbage until then)
  taiWidget*    GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr) override;
  void          GetImage_impl(taiWidget* dat, const void* base) override;
  void          GetMbrValue_impl(taiWidget* dat, void* base) override;
private:
  void          Initialize() { mode = MD_BASE; }
  void          Destroy() {}
};

#endif // taiMemberOfTokenPtr_h

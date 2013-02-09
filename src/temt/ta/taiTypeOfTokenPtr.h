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

#ifndef taiTypeOfTokenPtr_h
#define taiTypeOfTokenPtr_h 1

// parent includes:
#include <taiType>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; //

taTypeDef_Of(taiTypeOfTokenPtr);

class TA_API taiTypeOfTokenPtr : public taiType {
  TAI_TYPEBASE_SUBCLASS(taiTypeOfTokenPtr, taiType);
public:
  enum Mode {
    MD_BASE,            // taBase pointer
    MD_SMART_PTR,       // taSmartPtr -- acts almost identical to taBase*
    MD_SMART_REF        // taSmartRef
  };
  override bool handlesReadOnly() const { return true; } // uses a RO tokenptr button
  taBase*       GetTokenPtr(const void* base) const; // depends on mode
  TypeDef*      GetMinType(const void* base);
  int           BidForType(TypeDef* td);
  void          Initialize() { mode = MD_BASE; }
protected:
  taiWidget*      GetWidgetRep_impl(IWidgetHost* host_, taiWidget* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr);
  void          GetImage_impl(taiWidget* dat, const void* base);
  void          GetValue_impl(taiWidget* dat, void* base);

  Mode          mode; // set during first GetWidgetRep (is garbage until then)
};

#endif // taiTypeOfTokenPtr_h

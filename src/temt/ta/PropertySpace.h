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

#ifndef PropertySpace_h
#define PropertySpace_h 1

// parent includes:
#include <MemberDefBase_List>

// member includes:

// declare all other types mentioned but not required to include:
class PropertyDef; //


taTypeDef_Of(PropertySpace);

class TA_API PropertySpace: public MemberDefBase_List {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of properties -- note: will hold PropertyDef (owned) and MemberDef (linked) objects
INHERITED(MemberDefBase_List)
public:
  PropertySpace()                               { }
  PropertySpace(const PropertySpace& cp)        { Borrow(cp); }

  void operator=(const PropertySpace& cp)       { Borrow(cp); }

  int                   FindNameOrType(const char* nm) const;
  // checks name and type name in 2 passes
  int                   FindTypeName(const char* nm) const;
  // find by name of type
  MemberDefBase*                FindNameR(const char* nm) const;
  // recursive find of name (or type name)
#ifdef NO_TA_BASE
  PropertyDef*          AssertProperty(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr)
    {return AssertProperty_impl(nm, is_new, get_nset, mbr, NULL);}
  // assert property, supplying accessor
  PropertyDef*          AssertProperty(const char* nm, bool& is_new,
    bool get_nset, MethodDef* mth)
    {return AssertProperty_impl(nm, is_new, get_nset, NULL, mth);}
  // assert property, supplying accessor
protected:
  PropertyDef*          AssertProperty_impl(const char* nm, bool& is_new,
    bool get_nset, MemberDef* mbr, MethodDef* mth);
#endif
};

#endif // PropertySpace_h

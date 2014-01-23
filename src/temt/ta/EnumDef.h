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

#ifndef EnumDef_h
#define EnumDef_h 1

// parent includes:
#include <TypeItem>

// member includes:

// declare all other types mentioned but not required to include:
class EnumSpace; //

taTypeDef_Of(EnumDef);

class TA_API EnumDef : public TypeItem { //  defines an enum member
INHERITED(TypeItem)
public:
  EnumSpace*    owner;          // the owner of this one

  int           enum_no;        // number (value) of the enum

  void*        This() CPP11_OVERRIDE {return this;}
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_EnumDef;}
  TypeInfoKinds TypeInfoKind() const CPP11_OVERRIDE {return TIK_ENUM;}

  void          Copy(const EnumDef& cp);

  EnumDef();
  EnumDef(const char* nm);
  EnumDef(const char* nm, const char* dsc, int eno, const char* op, const char* lis);
  EnumDef(const EnumDef& cp);
  EnumDef*      Clone()         { return new EnumDef(*this); }
  EnumDef*      MakeToken()     { return new EnumDef(); }

  TypeDef*  GetOwnerType() const CPP11_OVERRIDE;
  bool          CheckList(const String_PArray& lst) const;
  // check if enum has a list in common with given one
private:
  void          Initialize();
  void          Copy_(const EnumDef& cp);
};

#endif // EnumDef_h

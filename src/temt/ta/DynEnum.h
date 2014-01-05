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

#ifndef DynEnum_h
#define DynEnum_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <DynEnumBase>

// declare all other types mentioned but not required to include:

// NOTE: prior to 12/11 the value for a non-bits enum was encoded as the index into
// the list of enums.  this is typically the same as the actual int value, but
// not always.  it has now been changed to the more conventional storage of the
// actual numerical value in all cases..

taTypeDef_Of(DynEnum);

class TA_API DynEnum : public taOBase {
  // #STEM_BASE #NO_TOKENS #NO_UPDATE_AFTER ##EDIT_INLINE ##CAT_Program ##SCOPE_Program dynamic enumerated value -- represents one item from a list of enumerated alternative labeled values
INHERITED(taOBase)
public:
  DynEnumBaseRef        enum_type; // enum type information (list of enum labels)
  int                   value;     // #DYNENUM_ON_enum_type current integer value

  inline bool   IsSet() const { return (bool)enum_type; }
  // has the type been set for this enum?

  inline int   NumVal() const { return value; }
  // current numerical (integer) value of enum (-1 = no value set)
  virtual const String NameVal() const;
  // current name (string) value of enum ("" = no value set)

  inline void  SetNumVal(int val)  { value = val; }
  // set current enum value by numerical value
  bool  SetNameVal(const String& nm);
  // set current enum value by name -- for bits this clears any existing bits (use css |= to set bits without clearing)

  override String       GetDisplayName() const;

  TA_SIMPLE_BASEFUNS_UPDT_PTR_PAR(DynEnum, Program);
protected:
  override void CheckThisConfig_impl(bool quiet, bool& rval);

private:
  void  Initialize();
  void  Destroy();
};

#endif // DynEnum_h

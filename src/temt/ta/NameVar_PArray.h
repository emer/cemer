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

#ifndef NameVar_PArray_h
#define NameVar_PArray_h 1

// parent includes:
#include <NameVar>
#include <taPlainArray>

// member includes:

// declare all other types mentioned but not required to include:
class String_PArray; // 


TypeDef_Of(NameVar_PArray);

class TA_API NameVar_PArray : public taPlainArray<NameVar> {
  // #NO_TOKENS a plain-array of name value (variant) items
INHERITED(taPlainArray<NameVar>)
public:
  static const String   def_sep; // ", "

  int   FindName(const String& nm, int start=0) const;
  // find by name  (start < 0 = from end)
  int   FindNameContains(const String& nm, int start=0) const;
  // find by name containing nm (start < 0 = from end)
  int   FindValue(const Variant& var, int start=0) const;
  // find by value (start < 0 = from end)
  int   FindValueContains(const String& vl, int start=0) const;
  // find by value.toString() containing vl (start < 0 = from end)

  Variant       GetVal(const String& nm);
  // get value from name; isNull if not found
  Variant       GetValDef(const String& nm, const Variant& def);
  // get value from name; def if not found
  bool          GetAllVals(const String& nm, String_PArray& vals);
  // get all values having given name (converts to strings)
  bool          SetVal(const String& nm, const Variant& val);
  // set value by name; if name already on list, it is updated (rval = true); else new item added

  const String  AsString(const String& sep = def_sep) const;
  void  operator=(const NameVar_PArray& cp)     { Copy_Duplicate(cp); }
  NameVar_PArray()                              { };
  NameVar_PArray(const NameVar_PArray& cp)      { Copy_Duplicate(cp); }
protected:
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((NameVar*)a)->value > ((NameVar*)b)->value) rval=1; else if(((NameVar*)a)->value == ((NameVar*)b)->value) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (((NameVar*)a)->value == ((NameVar*)b)->value); }
  String        El_GetStr_(const void* it) const { return ((NameVar*)it)->GetStr(); }
  void          El_SetFmStr_(void* it, const String& val) { ((NameVar*)it)->SetFmStr(val); }
};

#endif // NameVar_PArray_h

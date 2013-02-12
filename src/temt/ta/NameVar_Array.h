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

#ifndef NameVar_Array_h
#define NameVar_Array_h 1

// parent includes:
#include <NameVar>
#include <taArray>

// member includes:

// declare all other types mentioned but not required to include:
class String_Array; // 


taTypeDef_Of(NameVar_Array);

class TA_API NameVar_Array : public taArray<NameVar> {
  // #NO_TOKENS an array of name value (variant) items
INHERITED(taArray<NameVar>)
public:
  static const String   def_sep; // ", " default separator
  static const NameVar blank; // #HIDDEN #READ_ONLY

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
  bool          GetAllVals(const String& nm, String_Array& vals);
  // get all values having given name (converts to strings)
  bool          SetVal(const String& nm, const Variant& vl);
  // set value by name; if name already on list, it is updated (rval = true); else new item added
  TA_BASEFUNS_NOCOPY(NameVar_Array);
  TA_ARRAY_FUNS(NameVar_Array, NameVar)
protected:
  override Variant      El_GetVar_(const void* itm) const
    { return (Variant)((NameVar*)itm)->value; }
  int           El_Compare_(const void* a, const void* b) const
  { int rval=-1; if(((NameVar*)a)->value > ((NameVar*)b)->value) rval=1; else if(((NameVar*)a)->value == ((NameVar*)b)->value) rval=0; return rval; }
  bool          El_Equal_(const void* a, const void* b) const
  { return (((NameVar*)a)->value == ((NameVar*)b)->value); }
  String        El_GetStr_(const void* it) const { return ((NameVar*)it)->GetStr(); }
  void          El_SetFmStr_(void* it, const String& val) { ((NameVar*)it)->SetFmStr(val); }
private:
  void Initialize()     { };
  void Destroy()        { };
};

TA_ARRAY_OPS(NameVar_Array)

#endif // NameVar_Array_h

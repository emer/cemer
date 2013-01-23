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

#ifndef EnumSpace_h
#define EnumSpace_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class EnumDef; //
#ifndef NO_TA_BASE
class taDataLink; //
#endif

TypeDef_Of(EnumSpace);

class TA_API EnumSpace : public taPtrList<EnumDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of enums
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  taPtrList_impl* El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it);
  void*         El_Copy_(void* trg, void* src);

public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
#ifndef NO_TA_BASE
  taDataLink*   data_link;
#endif

  void          Initialize();
  EnumSpace()                   { Initialize(); }
  EnumSpace(const EnumSpace& cp) { Initialize(); Borrow(cp); }
  ~EnumSpace();

  void operator=(const EnumSpace& cp)   { Borrow(cp); }

  // adding manages the values of the enum-values
  void                  Add(EnumDef* it);
  virtual EnumDef*      Add(const char* nm, const char* dsc="", const char* op="",
                            int eno=0);

  virtual EnumDef*      FindNo(int eno) const;
  // finds for a given enum_no

  virtual String&      PrintType(String& strm, int indent = 1) const;
};

#endif // EnumSpace_h

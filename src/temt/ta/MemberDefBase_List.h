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

#ifndef MemberDefBase_List_h
#define MemberDefBase_List_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class MemberDefBase; //
#ifndef NO_TA_BASE
class taDataLink; //
#endif

TypeDef_Of(MemberDefBase_List);

class TA_API MemberDefBase_List: public taPtrList<MemberDefBase> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS common subtype for Member and PropertySpace
public:
  String        name;           // of the space
  TypeDef*      owner;          // owner is a typedef
#ifndef NO_TA_BASE
  taDataLink*   data_link;
#endif

  MemberDefBase_List() {Initialize();}
  ~MemberDefBase_List();
protected:
  String        GetListName_() const            { return name; }
  String        El_GetName_(void* it) const;
  taPtrList_impl* El_GetOwnerList_(void* it) const;
  void*         El_SetOwner_(void* it);
  void          El_SetIndex_(void* it, int i);

  void*         El_Ref_(void* it);
  void*         El_unRef_(void* it);
  void          El_Done_(void* it);
  void*         El_MakeToken_(void* it); // makes a proper token of the concrete type
  void*         El_Copy_(void* trg, void* src); // note: copies like guys correctly

private:
  void          Initialize();
};

#endif // MemberDefBase_List_h

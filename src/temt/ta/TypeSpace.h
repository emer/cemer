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

#ifndef TypeSpace_h
#define TypeSpace_h 1

// parent includes:
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; //
#ifndef NO_TA_BASE
class taSigLink; //
#endif

TypeDef_Of(TypeSpace);

class TA_API TypeSpace: public taPtrList<TypeDef> {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS space of types; uses default string-based hashing
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
  taSigLink*   sig_link;
#endif

  void          Initialize();

  TypeSpace()                           { Initialize(); }
  TypeSpace(const char* nm)             { Initialize(); name = nm; }
  TypeSpace(const char* nm, int hash_sz) { Initialize(); name = nm; BuildHashTable(hash_sz); }
  TypeSpace(const TypeSpace& cp)        { Initialize(); Borrow(cp); }
  ~TypeSpace();

  void operator=(const TypeSpace& cp)   { Borrow(cp); }

  TypeDef*      FindTypeR(const String& fqname) const; // find a possibly nested subtype based on :: name
  virtual bool  ReplaceLinkAll(TypeDef* ol, TypeDef* nw);
  virtual bool  ReplaceParents(const TypeSpace& ol, const TypeSpace& nw);
  // replace any parents on the old list with those on the new for all types

  String&       PrintAllTokens(String& strm) const;
  // print count for all types that are keeping tokens
};

#endif // TypeSpace_h

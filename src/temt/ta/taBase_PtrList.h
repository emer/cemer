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

#ifndef taBase_PtrList_h
#define taBase_PtrList_h 1

// parent includes:
#include <taBase>
#include <taPtrList>

// member includes:

// declare all other types mentioned but not required to include:


class TA_API taBase_PtrList: public taPtrList<taBase> { // a primitive taBase list type, used for global lists that manage taBase objects
public:
  TypeDef*      El_GetType_(void* it) const
    {return ((taBase*)it)->GetTypeDef();} // #IGNORE
protected:
  String        El_GetName_(void* it) const { return ((taBase*)it)->GetName(); }

  void*         El_Ref_(void* it)       { taBase::Ref((taBase*)it); return it; }
  void*         El_unRef_(void* it)     { taBase::unRef((taBase*)it); return it; }
  void          El_Done_(void* it)      { taBase::Done((taBase*)it); }
};

#endif // taBase_PtrList_h

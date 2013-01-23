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

#ifndef taiDataList_h
#define taiDataList_h 1

// parent includes:
#include <taPtrList>
#include <taiData>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(taiDataList);

class TA_API taiDataList : public taPtrList<taiData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS taiData list, OBJECTS ARE DELETED ON REMOVAL
INHERITED(taPtrList<taiData>)
public:
#ifndef __MAKETA__
  USING(inherited::Add)
  template<class T>
  T*                    Add(T* it) {Add_((void*)it); return it;}
    // convenience method, returns strongly typed guy that it adds
#endif
  ~taiDataList();
protected:
//  void*               El_Ref_(void* it)       { taRefN::Ref((taiData*)it); return it; }
//  void*       El_unRef_(void* it)     { taRefN::unRef((taiData*)it); return it; }
  void          El_Done_(void* it);
};

#endif // taiDataList_h

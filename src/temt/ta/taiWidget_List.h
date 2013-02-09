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

#ifndef taiWidget_List_h
#define taiWidget_List_h 1

// parent includes:
#include <taPtrList>
#include <taiWidget>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taiWidget_List);

class TA_API taiWidget_List : public taPtrList<taiWidget> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS taiWidget list, OBJECTS ARE DELETED ON REMOVAL
INHERITED(taPtrList<taiWidget>)
public:
#ifndef __MAKETA__
  USING(inherited::Add)
  template<class T>
  T*                    Add(T* it) {Add_((void*)it); return it;}
    // convenience method, returns strongly typed guy that it adds
#endif
  ~taiWidget_List();
protected:
//  void*               El_Ref_(void* it)       { taRefN::Ref((taiWidget*)it); return it; }
//  void*       El_unRef_(void* it)     { taRefN::unRef((taiWidget*)it); return it; }
  void          El_Done_(void* it);
};

#endif // taiWidget_List_h

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

#ifndef IRefListClient_h
#define IRefListClient_h 1

// parent includes:
#include <ITypedObject>

// member includes:

// declare all other types mentioned but not required to include:
class taBase; // 
class taBase_RefList; //

TypeDef_Of(IRefListClient);

class TA_API IRefListClient: virtual public ITypedObject { // #NO_CSS
// optional interface so disparate classes can use RefList to get notifies
public:
  virtual void          SigDestroying_Ref(taBase_RefList* src, taBase* ta) = 0;
    // note: item will already have been removed from list
  virtual void          SigEmit_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2) = 0;

};

#endif // IRefListClient_h

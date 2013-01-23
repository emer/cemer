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

#ifndef ITypedObject_h
#define ITypedObject_h 1

// parent includes:
#include "ta_def.h"
#include "ta_TA_type_WRAPPER.h"

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(ITypedObject);

class TA_API ITypedObject {
  //#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE this is the interface available to the host data object
public:
  virtual void*         This() = 0; // reference to the 'this' pointer of the client object
  virtual TypeDef*      GetTypeDef() const = 0; // typedef of the dlc
  virtual ~ITypedObject() {}
};

#define TYPED_OBJECT(T) \
  void* This() {return this;} \
  TypeDef* GetTypeDef() const {return &TA_##T;}

#endif // ITypedObject_h

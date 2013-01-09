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

#ifndef IDataLinkProxy_h
#define IDataLinkProxy_h 1

// parent includes:
#include <ITypedObject>

// member includes:

// declare all other types mentioned but not required to include:
class taiDataLink; //


class TA_API IDataLinkProxy : public virtual ITypedObject {//#NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE
public:
#ifndef TA_NO_GUI
  virtual taiDataLink*  link() const = 0;
#else
  virtual taDataLink*   link() const = 0;
#endif
  virtual TypeDef*      GetDataTypeDef() const; // convenience function, default gets data type from link
};

#endif // IDataLinkProxy_h

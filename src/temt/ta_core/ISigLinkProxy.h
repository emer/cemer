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

#ifndef ISigLinkProxy_h
#define ISigLinkProxy_h 1

// parent includes:
#include <ITypedObject>

// member includes:

// declare all other types mentioned but not required to include:
class taiSigLink; //


taTypeDef_Of(ISigLinkProxy);

class TA_API ISigLinkProxy : public virtual ITypedObject {
  // #NO_INSTANCE #NO_TOKENS #NO_CSS #NO_MEMBERS #VIRT_BASE most basic interface for anyone interfacing with SigLink's
public:
#ifndef TA_NO_GUI
  virtual taiSigLink*  link() const = 0;
#else
  virtual taSigLink*   link() const = 0;
#endif
  virtual TypeDef*      GetDataTypeDef() const; // convenience function, default gets data type from link
};

#endif // ISigLinkProxy_h

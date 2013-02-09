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

#ifndef taNBase_h
#define taNBase_h 1

// parent includes:
#include <taOBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taNBase);

class TA_API taNBase : public taOBase { // #NO_TOKENS Named, owned base class of taBase
INHERITED(taOBase)
public:
  String                name; // #CONDEDIT_OFF_base_flags:NAME_READONLY #CAT_taBase name of the object

  override bool HasName() const { return true; }
  override bool SetName(const String& nm);
  override String GetName() const  { return name; }
  override void SetDefaultName();
  override void MakeNameUnique();

  TA_BASEFUNS(taNBase);
protected:
  override void         UpdateAfterEdit_impl();

private:
  NOCOPY(taNBase); //note: we don't copy name, because it causes too many issues
  void  Initialize()    { }
  void  Destroy()       { }
};

#endif // taNBase_h

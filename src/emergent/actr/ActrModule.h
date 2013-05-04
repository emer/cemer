// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef ActrModule_h
#define ActrModule_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ActrBuffer>

// declare all other types mentioned but not required to include:
class ActrModel; //

eTypeDef_Of(ActrModule);

class E_API ActrModule : public taNBase {
  // ##INSTANCE ##CAT_ActR base class for ACT-R modules
INHERITED(taNBase)
public:
  String                desc;  // #EDIT_DIALOG #HIDDEN_INLINE description of this module
  ActrBufferRef         buffer; // the buffer for this module

  virtual void  InitBuffer(ActrModel& model) { };
  // ensure we have our appropriate buffer in model, and set our buffer pointer

  virtual void  ProcessRequests(ActrModel& model) { };
  // process any active requests from our buffer

  override String       GetDesc() const {return desc;}
  override String       GetTypeDecoKey() const { return "Program"; }

  TA_SIMPLE_BASEFUNS(ActrModule);
private:
  void Initialize();
  void Destroy()     { CutLinks(); }
};

SmartRef_Of(ActrModule); // ActrModuleRef

#endif // ActrModule_h

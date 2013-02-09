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

#ifndef taFBase_h
#define taFBase_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(taFBase);

class TA_API taFBase: public taNBase {
  // #NO_TOKENS #NO_UPDATE_AFTER named/owned base class of taBase, with filename
public:
  String                desc;      // #EDIT_DIALOG #CAT_taBase description of this object: what does it do, how should it be used, etc
  String                file_name; // #READ_ONLY #NO_SAVE #EXPERT #CAT_taBase The most recent file saved or loaded in association with this object.

  override String       GetDesc() const { return desc; }

  override bool         SetFileName(const String& val); // #IGNORE note: we canonicalize name first
  override String       GetFileName() const { return file_name; } // #IGNORE
  TA_BASEFUNS2(taFBase, taNBase)
private:
  void                  Copy_(const taFBase& cp) { desc = cp.desc; file_name = cp.file_name; }
  void                  Initialize() {}
  void                  Destroy() {}
};

#endif // taFBase_h

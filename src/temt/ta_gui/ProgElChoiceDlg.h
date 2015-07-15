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

#ifndef ProgElChoiceDlg_h
#define ProgElChoiceDlg_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ProgVar>
#include <taGuiDialog>

// declare all other types mentioned but not required to include:

taTypeDef_Of(ProgElChoiceDlg);

class TA_API ProgElChoiceDlg : public taNBase {
  // #CAT_GUI choose where to create a new variable and what type it is
  INHERITED(taNBase)
public:
  taGuiDialog  dlg;

  enum LocalGlobalOption {
      LOCAL,        // force local only choice
      GLOBAL,       // force global only choice
      LOCALGLOBAL   // local or global plus ignore
    };

  virtual int  GetLocalGlobalChoice(String& var_nm, int& local_global_choice,
                   ProgVar::VarType& var_type_choice, LocalGlobalOption = LOCALGLOBAL,
                                    bool make_new_instr = false);
  // #IGNORE

  TA_SIMPLE_BASEFUNS(ProgElChoiceDlg);

 private:
  void  Initialize() {}
  void  Destroy() {}
};

#endif // ProgElChoiceDlg_h

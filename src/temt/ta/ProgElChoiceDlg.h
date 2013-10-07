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
#include <Program>
#include <ProgVar>

// declare all other types mentioned but not required to include:
class taGuiDialog; //

taTypeDef_Of(ProgElChoiceDlg);

class TA_API ProgElChoiceDlg : public taNBase {
  // <describe here in full detail in one extended line comment>
INHERITED(taNBase)
public:
      taGuiDialog  dlg;

      virtual int  GetLocalGlobalChoice(Program* prg, String& var_nm, int& local_global_choice, ProgVar::VarType& var_type_choice);

      TA_SIMPLE_BASEFUNS(ProgElChoiceDlg);

private:
      void  Initialize() {}
      void  Destroy() {}
};

#endif // ProgElChoiceDlg_h

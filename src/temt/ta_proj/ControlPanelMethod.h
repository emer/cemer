// Co2018ght 2013-2017, Regents of the University of Colorado,
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

#ifndef ControlPanelMethod_h
#define ControlPanelMethod_h 1

// parent includes:
#include <ControlPanelItem>

// member includes:
#include <MethodDef>

// rename to ControlPanelMethod

// declare all other types mentioned but not required to include:

taTypeDef_Of(ControlPanelMethod);

class TA_API ControlPanelMethod: public ControlPanelItem {
  // #AKA_EditMthItem a method control panel item -- allows access via menu or button to just one method from a class
  INHERITED(ControlPanelItem)
public:
  MethodDef*            mth; // #READ_ONLY #SHOW the mbr type

  TypeItem*    typeItem() const override {return mth;} // the mbr or mth

//  String     GetColText(const KeyString& key, int itm_idx = -1) const override;
  TA_BASEFUNS(ControlPanelMethod);
protected:
  void         UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const ControlPanelMethod& cp);
};

#endif // ControlPanelMethod_h

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

#ifndef EditMbrItem_h
#define EditMbrItem_h 1

// parent includes:
#include <ControlPanelItem>

// member includes:
#include <EditParamSearch>
#include <ParamSetItem>
#include <ControlItemNote>
#include <MemberDef>

// declare all other types mentioned but not required to include:

taTypeDef_Of(EditMbrItem);

class TA_API EditMbrItem: public ControlPanelItem {
  // a member control panel item -- allows editing of just one member from a class
  INHERITED(ControlPanelItem)
public:
  MemberDef*            mbr; // #READ_ONLY #SHOW the mbr type
  bool                  is_numeric;   // #READ_ONLY is this a single numeric type (float, double, int)
  bool                  is_single;   // #READ_ONLY is this a single item, not a composite item
  EditParamSearch       param_search; // #CONDSHOW_ON_is_single specifications for parameter searching, only for numeric items
  ParamSetItem          param_set_value;  // if the panel is a param set panel display the saved value
  ControlItemNote       notes;            // you can record notes here about this parameter
    
  TypeItem*    typeItem() const override {return mbr;} // the mbr or mth

  virtual String        CurValAsString();
  // #CAT_ParamSearch get current value of item, as a string -- not subject to validity for parameter searching

  String       GetColText(const KeyString& key, int itm_idx = -1) const override;
  TA_BASEFUNS(EditMbrItem);
  void  InitLinks() override;
protected:
  void  UpdateAfterEdit_impl() override;
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const EditMbrItem& cp);
};

#endif // EditMbrItem_h

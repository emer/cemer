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

#ifndef EditMthItem_h
#define EditMthItem_h 1

// parent includes:
#include <SelectEditItem>

// member includes:
#include <MethodDef>

// declare all other types mentioned but not required to include:

taTypeDef_Of(EditMthItem);

class TA_API EditMthItem: public SelectEditItem {
  // a method select edit item -- allows access via menu or button to just one method from a class
  INHERITED(SelectEditItem)
public:
  MethodDef*            mth; // #READ_ONLY #SHOW the mbr type

  TypeItem*    typeItem() const CPP11_OVERRIDE {return mth;} // the mbr or mth

//  String     GetColText(const KeyString& key, int itm_idx = -1) const CPP11_OVERRIDE;
  TA_BASEFUNS(EditMthItem);
protected:
  void                  UpdateAfterEdit_impl();
private:
  void  Initialize();
  void  Destroy();
  void  Copy_(const EditMthItem& cp);
};

#endif // EditMthItem_h

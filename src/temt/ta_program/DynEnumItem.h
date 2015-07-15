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

#ifndef DynEnumItem_h
#define DynEnumItem_h 1

// parent includes:
#include <taNBase>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(DynEnumItem);

class TA_API DynEnumItem : public taNBase {
  // ##EDIT_INLINE ##CAT_Program ##SCOPE_Program dynamic enumerated type value (name and numerical int value)
INHERITED(taNBase)
public:
  int           value;          // numerical (integer) value of this enum -- automatically set to be sequential or orthogonal bits if using bits mode -- order must be increasing in list order
  String        desc;           // #EDIT_DIALOG description of item

  String       GetDisplayName() const override;
  String       GetDesc() const override { return desc; }

  bool         BrowserSelectMe() override;
  bool         BrowserExpandAll() override;
  bool         BrowserCollapseAll() override;

  bool                  BrowserEditSet(const String& code, int move_after = 0) override;
  virtual  String       CodeGetDesc(const String& code);
  // #IGNORE get description (comment) from the code string -- returns code without the comment

  inline void   Initialize()                    { value = 0; }
  inline void   Destroy()                       { };
  inline void   Copy_(const DynEnumItem& cp)    { value = cp.value; desc = cp.desc; }
  TA_BASEFUNS(DynEnumItem);
protected:
  void         UpdateAfterEdit_impl() override;
  void         CheckThisConfig_impl(bool quiet, bool& rval) override;
};

#endif // DynEnumItem_h

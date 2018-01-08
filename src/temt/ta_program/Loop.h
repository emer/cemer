// Copyright 2013-2017, Regents of the University of Colorado,
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

#ifndef Loop_h
#define Loop_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 
class TypeDef; // 


taTypeDef_Of(Loop);

class TA_API Loop: public ProgEl {
  // ##DEF_CHILD_loop_code #VIRT_BASE base class for loops
INHERITED(ProgEl)
public:
  ProgEl_List           loop_code; // the items to execute in the loop

  int          ProgElChildrenCount() const override { return loop_code.size; }

  virtual ProgEl*        AddLoopCode(TypeDef* el_type)
  { return (ProgEl*)loop_code.New(1, el_type); }
  // #BUTTON #TYPE_ProgEl add a new loop code element

  taList_impl*	children_() override { return &loop_code; }
  bool          HasSubCode() override { return true; }

  ProgVar*     FindVarName(const String& var_nm) const override;
  String       GetTypeDecoKey() const override { return "ProgCtrl"; }
  bool         BrowserEditTest() override;

  SIMPLE_COPY(Loop);
  SIMPLE_LINKS(Loop);
  TA_ABSTRACT_BASEFUNS(Loop) //

protected:
  void         CheckChildConfig_impl(bool quiet, bool& rval) override;
  void         PreGenChildren_impl(int& item_id) override;
  bool         GenCssBody_impl(Program* prog) override;
  const String GenListing_children(int indent_level) const override;

private:
  void  Initialize();
  void  Destroy()       {CutLinks();}
};

#endif // Loop_h

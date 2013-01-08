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

#ifndef SelectEditsFmArgs_h
#define SelectEditsFmArgs_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVarRef>

// declare all other types mentioned but not required to include:
class SelectEdit; // 


class TA_API SelectEditsFmArgs: public ProgEl { 
  // sets SelectEdit value(s) based on startup arguments of the name of the select edit item -- items must be single value numeric items
INHERITED(ProgEl)
public:
  ProgVarRef		sel_edit_var;	// #ITEM_FILTER_ObjProgVarFilter program variable pointing to select edit that contains the items to access with startup arguments

  virtual SelectEdit* 	GetSelectEdit() const;
  // get actual select edit from variable

  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  override String	GetToolbarName() const { return "sele=args"; }

  void	GenRegArgs(Program* prog);
  // #IGNORE generate RegisterArgs code

  PROGEL_SIMPLE_BASEFUNS(SelectEditsFmArgs);
protected:
  override void UpdateAfterEdit_impl();
  override void CheckThisConfig_impl(bool quiet, bool& rval);
  override void	GenCssBody_impl(Program* prog);

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // SelectEditsFmArgs_h

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

#ifndef RegisterArgs_h
#define RegisterArgs_h 1

// parent includes:
#include <ProgEl>

// member includes:

// declare all other types mentioned but not required to include:
class ProgEl_List;


taTypeDef_Of(RegisterArgs);

class TA_API RegisterArgs: public ProgEl { 
  // register command-line arguments for any MemberFmArg, ProgVarFmArg DataColsFmArgs program elements contained in the prog_code of the program that this item appears in.  calls taMisc::UpdateArgs(), so any any other taMisc::AddArgName MiscCall's placed before this will also be processed
INHERITED(ProgEl)
public:
  String	GetDisplayName() const override;
  String 	GetTypeDecoKey() const override { return "ProgVar"; }
  String	GetToolbarName() const override { return "reg args"; }

  PROGEL_SIMPLE_BASEFUNS(RegisterArgs);
protected:
  void	GenCssBody_impl(Program* prog) override;

  virtual void		AddArgsFmCode(Program* prog, ProgEl_List& progs);
  // main function: iterates recursively through progs, adding any that add args to gen_code

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // RegisterArgs_h

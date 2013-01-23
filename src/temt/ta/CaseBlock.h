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

#ifndef CaseBlock_h
#define CaseBlock_h 1

// parent includes:
#include <CodeBlock>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:
class ProgEl; // 


TypeDef_Of(CaseBlock);

class TA_API CaseBlock: public CodeBlock { 
  // one case element of a switch: if switch variable is equal to case_val, then this chunk of code is run
  INHERITED(CodeBlock)
public:
  ProgExpr		case_val; // value of the switch variable -- if switch_var is equal to this, then this code is run (must use literal expression here) -- if case_val is empty, then this represents the default case (run when no other case matches)

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);
  override bool		IsCtrlProgEl() 	{ return true; }

  override String	GetDisplayName() const;
  override String	GetToolbarName() const { return "case"; }

  PROGEL_SIMPLE_BASEFUNS(CaseBlock);
protected:
  override void		CheckThisConfig_impl(bool quiet, bool& rval);
  override void		GenCssPre_impl(Program* prog); 
  override void		GenCssBody_impl(Program* prog);
  override void		GenCssPost_impl(Program* prog); 

private:
  void	Initialize();
  void	Destroy()	{ CutLinks(); }
};

#endif // CaseBlock_h

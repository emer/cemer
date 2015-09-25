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


taTypeDef_Of(CaseBlock);

class TA_API CaseBlock: public CodeBlock { 
  // one case element of a switch: if switch variable is equal to case_val, then this chunk of code is run
  INHERITED(CodeBlock)
public:
  ProgExpr	case_val; // #BROWSER_EDIT_LOOKUP value of the switch variable -- if the switch condition value is equal to this, then this code is run (must use a concrete, literal expression here -- no variables or functions)
  
  bool          is_default;
  // make this a default case which is selected if none of the other cases match the switch condition
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String	GetDisplayName() const override;
  String	GetToolbarName() const override { return "case"; }

  bool    ChooseMe() override;
  // #IGNORE code to run when object drop from toolbar

  PROGEL_SIMPLE_BASEFUNS(CaseBlock);
protected:
  void    UpdateAfterEdit_impl();
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssBody_impl(Program* prog) override;
  void		GenCssPost_impl(Program* prog) override; 

private:
  void	Initialize();
  void	Destroy();
};

#endif // CaseBlock_h

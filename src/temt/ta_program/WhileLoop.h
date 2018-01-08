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

#ifndef WhileLoop_h
#define WhileLoop_h 1

// parent includes:
#include <Loop>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:
class ProgEl; // 


taTypeDef_Of(WhileLoop);

class TA_API WhileLoop: public Loop { 
  // #PROGEL_COMPLETION #ADD_PARENS Repeat loop_code while loop_test expression is true (test first): while(loop_test) do loop_code
INHERITED(Loop)
public:
  ProgExpr		test; // #BROWSER_EDIT_LOOKUP a test expression for whether to continue looping (e.g., 'i < max')
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool		CvtFmCode(const String& code) override;
  bool		IsCtrlProgEl() 	override { return true; }

  String	GetDisplayName() const override;
  String	GetToolbarName() const override { return "while"; }

  PROGEL_SIMPLE_BASEFUNS(WhileLoop);
protected:
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		GenCssPre_impl(Program* prog) override; 
  void		GenCssPost_impl(Program* prog) override; 

private:
  void	Initialize() {}
  void	Destroy()	{}
};

#endif // WhileLoop_h

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

#ifndef MemberAssign_h
#define MemberAssign_h 1

// parent includes:
#include <MemberProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:
class ProgEl; // 


taTypeDef_Of(MemberAssign);

class TA_API MemberAssign: public MemberProgEl { 
  // set a member (attribute) on an object to a value given by an expression
INHERITED(MemberProgEl)
public:
  ProgExpr		expr; // the expression to compute and assign to the member
  bool			update_after; // call UpdateAfterEdit after setting the member: useful for updating displays and triggering other computations based on changed value, but this comes at a performance cost 
  
  bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const CPP11_OVERRIDE;
  bool		CvtFmCode(const String& code) CPP11_OVERRIDE;

  String	GetDisplayName() const CPP11_OVERRIDE;
  String 	GetTypeDecoKey() const CPP11_OVERRIDE { return "ProgVar"; }
  String	GetToolbarName() const CPP11_OVERRIDE { return "memb="; }

  PROGEL_SIMPLE_BASEFUNS(MemberAssign);
protected:
  void		UpdateAfterEdit_impl() CPP11_OVERRIDE;
  void		CheckChildConfig_impl(bool quiet, bool& rval) CPP11_OVERRIDE;
  void		GenCssBody_impl(Program* prog) CPP11_OVERRIDE;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();}
}; 

#endif // MemberAssign_h

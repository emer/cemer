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

#ifndef LocalVars_h
#define LocalVars_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgVar_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(LocalVars);

class TA_API LocalVars: public ProgEl {
  // ##DEF_CHILD_local_vars #AKA_ProgVars local program variables -- these variables do NOT use or update the values that are shown -- they exist only as script variables (unlike global args and vars)
INHERITED(ProgEl)
public:
  ProgVar_List		local_vars;	// the list of variables -- these variables do NOT update their values as shown here -- they exist only as script variables (unlike global args and vars)
  
  virtual ProgVar*	AddVar();
  // #BUTTON add a new variable
  virtual ProgVar*	AddFloatMatrix();
  // #BUTTON add a new float_Matrix* variable -- these have to be in local vars so are frequently created..
  virtual ProgVar*	AddDoubleMatrix();
  // #BUTTON add a new double_Matrix* variable -- these have to be in local vars so are frequently created..
  virtual ProgVar*	AddIntMatrix();
  // #BUTTON add a new int_Matrix* variable -- these have to be in local vars so are frequently created..
  virtual ProgVar*	AddStringMatrix();
  // #BUTTON add a new String_Matrix* variable -- these have to be in local vars so are frequently created..
  virtual ProgVar*	AddVarMatrix();
  // #BUTTON add a new Variant_Matrix* variable -- these have to be in local vars so are frequently created..

  override ProgVar*	FindVarName(const String& var_nm) const;

  override bool		CanCvtFmCode(const String& code, ProgEl* scope_el) const;
  override bool		CvtFmCode(const String& code);

  override taList_impl*	children_() {return &local_vars;}
  override String	GetDisplayName() const;
  override String 	GetTypeDecoKey() const { return "ProgVar"; }
  override String	GetToolbarName() const { return "loc vars"; }

  PROGEL_SIMPLE_BASEFUNS(LocalVars);
protected:
  override void		CheckChildConfig_impl(bool quiet, bool& rval);
  override void		GenCssBody_impl(Program* prog);
  override const String	GenListing_children(int indent_level);

private:
  void	Initialize();
  void	Destroy();
};

#endif // LocalVars_h

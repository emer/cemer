// Copyright 2014-2018, Regents of the University of Colorado,
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

#ifndef Else_h
#define Else_h 1

// parent includes:
#include <CondBase>

// member includes:
#include <ProgExpr>
#include <ProgEl_List>

// declare all other types mentioned but not required to include:
class ProgVar; // 


taTypeDef_Of(Else);

class TA_API Else: public CondBase {
  // the other half of a conditional If test element: must come after an If or an ElseIf element, and is run if the condition there is false
INHERITED(CondBase)
public:
  bool          CanCvtFmCode(const String& code, ProgEl* scope_el) const override;
  bool          CvtFmCode(const String& code) override;
  virtual CondBase*  FindPriorIf(bool err_msgs = true) const;
  // find the preceding If or ElseIf for this Else

  String        GenProgName() const override;
  String        GetDisplayName() const override;
  String        GetTypeDecoKey() const override { return "ProgCtrl"; }
  ProgVar*      FindVarName(const String& var_nm) const override;
  String        GetToolbarName() const override { return "else"; }

  bool          BrowserEditTest() override;
  
  PROGEL_SIMPLE_BASEFUNS(Else);
protected:
  void		UpdateAfterEdit_impl() override;
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;
  void		CheckChildConfig_impl(bool quiet, bool& rval) override;
  void		PreGenChildren_impl(int& item_id) override;
  void		GenCssPre_impl(Program* prog) override; 
  bool		GenCssBody_impl(Program* prog) override; //replaces If
  void		GenCssPost_impl(Program* prog) override; 
  const String	GenListing_children(int indent_level) const override;

private:
  void	Initialize();
  void	Destroy()	{CutLinks();} //
};

#endif // Else_h

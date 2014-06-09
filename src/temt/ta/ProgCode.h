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

#ifndef ProgCode_h
#define ProgCode_h 1

// parent includes:
#include <ProgEl>

// member includes:
#include <ProgExpr>

// declare all other types mentioned but not required to include:
class ProgEl_List; //


taTypeDef_Of(ProgCode);

class TA_API ProgCode: public ProgEl {
  // generic program code where you can enter an expression in text and it will auto-convert to a known program element
INHERITED(ProgEl)
public:
  ProgExpr     code;   // #BROWSER_EDIT_LOOKUP program code statement that will be converted into an appropriate program element if possible

  void         SetProgExprFlags() override;
  String       GetDisplayName() const override;
  String       GetToolbarName() const override { return "code"; }
  String       GetTypeDecoKey() const override { return "ProgCode"; }

  bool          BrowserEditSet(const String& code, int move_after=0) override;

  virtual void ConvertToProgEl();
  // overall entry point for converting the code to an appropriate program element -- called in a delayed gui callback routine -- manages whole process
  virtual ProgEl* CvtCodeToProgEl();
  // convert current code string to a program element -- NULL if cannot be converted -- just does the conversion process to a new prog el, doesn't do anything else
  virtual bool  CvtCodeToVar(String& code_str);
  // attempt to convert the code to a new variable declaration -- prompts for var location -- if true, then it was interpreted as a var decl, and var decl is removed from code -- e.g., if input is "int i = 20" then remaining code will be "i = 20" -- if nothing but a decl (e.g., "int i"), then code is empty, and nothing left to do
  virtual void  CvtCodeCheckType(ProgEl_List& candidates, TypeDef* td,
                                 const String& code_str);
  // #IGNORE

  PROGEL_SIMPLE_BASEFUNS(ProgCode);
protected:
  void          UpdateAfterEdit_impl() override;
  void		CheckThisConfig_impl(bool quiet, bool& rval) override;

private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // ProgCode_h

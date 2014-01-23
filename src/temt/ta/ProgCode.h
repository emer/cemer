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
  ProgExpr              code;   // program code statement that will be converted into an appropriate program element if possible

  void         SetProgExprFlags() CPP11_OVERRIDE;
  String       GetDisplayName() const CPP11_OVERRIDE;
  String       GetToolbarName() const CPP11_OVERRIDE { return "code"; }
  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Comment"; }

  virtual void          ConvertToProgEl();
  // converts the code to an appropriate program element -- called in a delayed gui callback routine

  static void           CvtCodeCheckType(ProgEl_List& candidates, TypeDef* td,
                                         const String& code, ProgEl* scope_el);
  // #IGNORE
  static ProgEl*        CvtCodeToProgEl(const String& code, ProgEl* scope_el);
  // convert code string to a program element -- NULL if cannot be converted

  PROGEL_SIMPLE_BASEFUNS(ProgCode);
protected:
  void UpdateAfterEdit_impl() CPP11_OVERRIDE;

private:
  void  Initialize();
  void  Destroy()       {}
};

#endif // ProgCode_h

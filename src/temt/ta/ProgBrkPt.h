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

#ifndef ProgBrkPt_h
#define ProgBrkPt_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <ProgEl>

// declare all other types mentioned but not required to include:
class taSigLink; //

taTypeDef_Of(ProgBrkPt);

class TA_API ProgBrkPt : public taNBase {
  // This class tracks program breakpoints and is the data for the breakpoint UI
INHERITED(taNBase)
public:
  ProgElRef     prog_el;
  // #HIDDEN #SHOW_TREE the program element on which the break is set
  bool          enabled;
  // #SHOW breakpoints can persist and be enabled or disabled
  String                desc;
  // #EDIT_DIALOG #HIDDEN_INLINE optional brief description of element's function; included as comment in script

  void          Enable();
  // #MENU #DYN1 #GHOST_OFF_enabled enable existing disabled breakpoint
  void          Disable();
  // #MENU #DYN1 #GHOST_ON_enabled disable existing breakpoint but don't delete the breakpoint

  override String       GetDesc() const {return desc;}
  override String       GetStateDecoKey() const;
  override void         UpdateAfterEdit_impl();


  void          InitLinks();
  void          CutLinks();
  TA_BASEFUNS_NOCOPY(ProgBrkPt);

protected:

private:
  void Initialize();
  void Destroy()     { };
};

SmartRef_Of(ProgBrkPt);

#endif // ProgBrkPt_h

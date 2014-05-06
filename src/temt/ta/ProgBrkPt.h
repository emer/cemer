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
#include <ProgramRef>

// declare all other types mentioned but not required to include:
class taSigLink; //

taTypeDef_Of(ProgBrkPt);

class TA_API ProgBrkPt : public taNBase {
  // represents a break point in a Program -- program will stop at this point if enabled
INHERITED(taNBase)
public:
  bool          enabled;
  // #SHOW is this break point enabled?  using this flag makes it easy to toggle breakpoints on and off as needed, without having to find the line of code again
  ProgramRef    program;
  // the program where the break point is set
  ProgElRef     prog_el;
  // #HIDDEN #SHOW_TREE the program element on which the break is set
  String        desc;
  // #READ_ONLY #SHOW describes the line of code where the breakpoint is set

  void          Enable();
  // #MENU #DYN1 #GHOST_OFF_enabled enable existing disabled breakpoint
  void          Disable();
  // #MENU #DYN1 #GHOST_ON_enabled disable existing breakpoint but don't delete the breakpoint

  String       GetDesc() const override {return desc;}
  String       GetStateDecoKey() const override;
  void         UpdateAfterEdit_impl() override;
  String       GetDisplayName() const override;


  void          InitLinks();
  void          CutLinks();
  TA_BASEFUNS_NOCOPY(ProgBrkPt);

protected:

private:
  void Initialize();
  void Destroy()     { };
};

SMARTREF_OF(TA_API, ProgBrkPt);

#endif // ProgBrkPt_h

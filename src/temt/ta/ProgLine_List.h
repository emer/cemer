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

#ifndef ProgLine_List_h
#define ProgLine_List_h 1

// parent includes:
#include <ProgLine>
#include <taList>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ProgLine_List);

class TA_API ProgLine_List : public taList<ProgLine> {
  // ##NO_TOKENS ##NO_UPDATE_AFTER ##CAT_Program a list of program listing elements -- always add one blank entry at the start so line numbers start at 1
INHERITED(taList<ProgLine>)
public:

  bool          AddLine(taBase* prog_el, int indent, const String& code,
                        int pline_flags = ProgLine::PL_NONE);
  // add one line of code to the program listing -- this is the official interface for adding to program -- code should be pure code without any indent,
  void          FullListing(String& script_code);
  // generate full listing of code from stored program lines into given string
  void          FullListingHTML(String& script_code);
  // generate full listing of code from stored program lines into given string -- with HTML color coding formating

  bool          SetBreakpoint(int line_no);
  // set breakpoint at given line
  void          ClearAllBreakpoints();
  // clear all the breakpoints
  void          ClearAllErrors();
  // clear all the error and warning flags
  int           FindProgEl(taBase* prog_el, bool reverse = false);
  // find program element in the listing, optionally going in reverse (safer b/c some prog els generate init code that may not be what is desired) -- returns line_no (index) of ProgLine that has this element or -1 if not found
  int           FindMainLine(taBase* prog_el);
  // find the main line associated with a given program element -- the one that should be logged etc

  String       GetTypeDecoKey() const CPP11_OVERRIDE { return "Program"; }
  TA_BASEFUNS_NOCOPY(ProgLine_List);
private:
  void  Initialize();
  void  Destroy()               { Reset(); } //
};

#endif // ProgLine_List_h

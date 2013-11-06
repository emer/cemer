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

#ifndef ProgLine_h
#define ProgLine_h 1

// parent includes:
#include <taOBase>

// for smart ptrs need these:
#include <taSmartRefT>

// member includes:

// declare all other types mentioned but not required to include:


taTypeDef_Of(ProgLine);

class TA_API ProgLine: public taOBase {
  // ##NO_TOKENS ##EDIT_INLINE ##SCOPE_Program ##CAT_Program represents one line of the generated program css code -- manages stuff for one line of code
INHERITED(taOBase)
public:
  enum PLineFlags { // #BITS flags for program listing elements
    PL_NONE             = 0, // #NO_BIT
    BREAKPOINT          = 0x0001, // a breakpoint has been set at this line
    PROG_ERROR          = 0x0002, // there was an error at this point
    WARNING             = 0x0004, // there was a warning at this point
    MAIN_LINE           = 0x0008, // this is the primary line associated with the given program element -- should be displayed for verbose logging etc
    COMMENT             = 0x0010, // this is a comment line -- does not contain actual code -- not all comments are marked as such -- just used as a heuristic for positioning verbose debugging -- never mark a MAIN_LINE as also a COMMENT..
    PROG_DEBUG          = 0x0020, // this is a program debugging line, e.g. from AddVerboseLine
  };

  PLineFlags    flags;          // flags for this line of code
  int           line_no;        // line in listing -- starts at 0, is index for ProgLine_List
  int           indent;         // how far indented is this item
  String        code;           // code associated with this line
  taBaseRef     prog_el;        // program element associated with this line (could be something besides ProgEl too, e.g., progvar or something)

  inline void           SetPLineFlag(PLineFlags flg)   { flags = (PLineFlags)(flags | flg); }
  // set flag state on
  inline void           ClearPLineFlag(PLineFlags flg) { flags = (PLineFlags)(flags & ~flg); }
  // clear flag state (set off)
  inline bool           HasPLineFlag(PLineFlags flg) const { return (flags & flg); }
  // check if flag is set
  inline void           SetPLineFlagState(PLineFlags flg, bool on)
  { if(on) SetPLineFlag(flg); else ClearPLineFlag(flg); }
  // set flag state according to on bool (if true, set flag, if false, clear it)
  inline void           TogglePLineFlag(PLineFlags flg) { SetPLineFlagState(flg, !HasPLineFlag(flg)); }
  // toggle program flag

  inline bool           IsMainLine() { return HasPLineFlag(MAIN_LINE); }
  // is this the main line for a given program element?
  inline bool           IsComment() { return HasPLineFlag(COMMENT); }
  // is this a comment non-coding line?

  const String          Indent();
  // generate string with indent spacing
  const String          CodeIndented() { return Indent() + code; }
  // get the code with indentation
  const String          CodeLineNo();
  // get the code with line numbers and indentation

  void                  SetBreakpoint();        // set breakpoint here and in associated prog_el
  void                  ClearBreakpoint();      // clear breakpoint here and in associated prog_el
  void                  SetError();             // set error here and in associated prog_el
  void                  ClearError();           // clear error here and in associated prog_el

  void                  SetWarning();           // set warning here and in associated prog_el
  void                  ClearWarning();         // clear warning here and in associated prog_el

  override int          GetIndex() const { return line_no; }
  override void         SetIndex(int value) { line_no = value; }

//    TA_SIMPLE_BASEFUNS(ProgLine);
    void  InitLinks();
    void  CutLinks();
    TA_BASEFUNS_NOCOPY(ProgLine);

private:
  void  Initialize();
  void  Destroy();
};

#endif // ProgLine_h

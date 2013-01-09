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

#ifndef taFontSpec_h
#define taFontSpec_h 1

// parent includes:
#include <taOBase>
#include <iFont>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API taFontSpec : public taOBase, public iFont {
 // #INLINE #INLINE_DUMP ##CAT_Display a specification of a font (for Unix - X logical font description)
INHERITED(taOBase)
public:
  String                pattern; // #READ_ONLY #NO_SAVE DEPRECATED the name of the font

  virtual void  SelectFont();
  // #BUTTON opens font selection dialog
  virtual void  SetFont(char* fn);
  virtual void  SetFontSize(int point_size = 10);
  // #BUTTON set font to given point size

  bool  Equals(const taFontSpec& b);
  void  UpdateAfterEdit();
  void  CutLinks();
  void  InitLinks();
  void  Copy_(const taFontSpec& cp);
  TA_BASEFUNS(taFontSpec);
private:
  void  Initialize();
  void  Destroy() {CutLinks();}
};

#endif // taFontSpec_h

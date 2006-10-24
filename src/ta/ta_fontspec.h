// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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



#ifndef TA_FONTSPEC_H
#define TA_FONTSPEC_H

#include "ta_base.h"
#include "ifont.h"

#include "ta_TA_type.h"

class TA_API FontSpec : public taOBase, public iFont {
 // #INLINE #INLINE_DUMP a specification of a font (for Unix - X logical font description)
INHERITED(taOBase)
public:
  String		pattern; // #READ_ONLY #NO_SAVE DEPRECATED the name of the font
//  String		prv_pat; // #READ_ONLY #NO_SAVE the previous font that was successfully obtained

  virtual void	SelectFont();
  // #BUTTON opens font selection dialog
  virtual void 	SetFont(char* fn);
  virtual void	SetFontSize(int point_size = 10);
  // #BUTTON set font to given point size

  void	UpdateAfterEdit();
  void	CutLinks();
  void	InitLinks();
  void	Copy_(const FontSpec& cp);
  COPY_FUNS(FontSpec,taOBase);
  TA_BASEFUNS(FontSpec);
private:
  void	Initialize();
  void	Destroy() {CutLinks();}
};


#endif // TA_FONTSPEC_H

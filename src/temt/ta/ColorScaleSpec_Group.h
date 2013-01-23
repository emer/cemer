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

#ifndef ColorScaleSpec_Group_h
#define ColorScaleSpec_Group_h 1

// parent includes:
#include <taGroup>
#include <ColorScaleSpec>

// member includes:

// declare all other types mentioned but not required to include:


TypeDef_Of(ColorScaleSpec_Group);

class TA_API ColorScaleSpec_Group : public taGroup<ColorScaleSpec> {
  // ##CAT_Display group of color scale specs
INHERITED(taGroup<ColorScaleSpec>)
public:
  virtual void          NewDefaults();  // create a set of default colors
  virtual void          SetDefaultColor();// set the default color based on gui

  override int          NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String       GetColHeading(const KeyString& key) const; // header text for the indicated column

  TA_BASEFUNS_NOCOPY(ColorScaleSpec_Group);
private:
  void  Initialize()    {SetBaseType(&TA_ColorScaleSpec);};
  void  Destroy()       { };
};


#endif // ColorScaleSpec_Group_h

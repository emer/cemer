// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef nvhDataView_h
#define nvhDataView_h 1

// parent includes:
#include <nvDataView>

// member includes:
#include <T3Color>

// declare all other types mentioned but not required to include:

TypeDef_Of(nvhDataView);

class EMERGENT_API nvhDataView: public nvDataView { // #VIRT_BASE highlightable guys
INHERITED(nvDataView)
public:
  void                  setDefaultColor() {DoHighlightColor(false);}
    // restore to its default color
  void                  setHighlightColor(const T3Color& color);
    // used for highlighting in gui, esp lay/prjn

  TA_BASEFUNS(nvhDataView)
protected:
  T3Color               m_hcolor; // hilight color (saved for rebuilds)

  virtual void          DoHighlightColor(bool apply) {}

private:
  void Copy_(const nvhDataView& cp);
  void                  Initialize() {}
  void                  Destroy() {}
};

#endif // nvhDataView_h

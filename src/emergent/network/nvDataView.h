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

#ifndef nvDataView_h
#define nvDataView_h 1

// parent includes:
#include "network_def.h"
#include <T3DataViewPar>

// member includes:

// declare all other types mentioned but not required to include:
class NetView; //
class T3Color; //


eTypeDef_Of(nvDataView);

class E_API nvDataView: public T3DataViewPar { // #VIRT_BASE most children of NetView
INHERITED(T3DataViewPar)
public:
  NetView*              getNetView();

  virtual void          setDefaultColor() {} // restore to its default color
  virtual void          setHighlightColor(const T3Color& color) {}
    // used for highlighting in gui, esp lay/prjn

  override void         CutLinks();

  TA_BASEFUNS(nvDataView)

private:
  void Copy_(const nvDataView& cp) {m_nv = NULL;}
  void                  Initialize();
  void                  Destroy() {}

  NetView*              m_nv; // cache
};

#endif // nvDataView_h

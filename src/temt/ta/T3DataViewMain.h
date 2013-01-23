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

#ifndef T3DataViewMain_h
#define T3DataViewMain_h 1

// parent includes:
#include <T3DataViewPar>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(T3DataViewMain);

class TA_API T3DataViewMain: public T3DataViewPar {
  // base class for major self-contained view objects, which can be independently positioned etc (main classes under T3DataViewRoot)
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
public:
  taTransform           main_xform;
  // this is the overall transform (position, scale, rotation) for this view object (typically can be adjusted by view's transform dragbox)

  override bool         isTopLevelView() const {return true;}

  virtual void          CopyFromViewFrame(T3DataViewPar* cp);
  // copy key view frame parameters from other view object

  virtual void          setInteractionModeOn(bool on_off, bool re_render = true);
  // set the interaction mode for the underlying examiner viewer -- does update and re-render unless specified as false

  override void         InitLinks();
  T3_DATAVIEWFUNS(T3DataViewMain, T3DataViewPar) //
private:
  void  Copy_(const T3DataViewMain& cp);
  void  Initialize() {}
  void  Destroy() {}
};

#endif // T3DataViewMain_h

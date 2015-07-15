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

#ifndef T3DataViewPar_h
#define T3DataViewPar_h 1

// parent includes:
#include <T3DataView>
#include <T3DataView_List>

// member includes:

// declare all other types mentioned but not required to include:

taTypeDef_Of(T3DataViewPar);

class TA_API T3DataViewPar: public T3DataView {
  // #VIRT_BASE #NO_TOKENS T3DataView that has child T3DataView's
#ifndef __MAKETA__
typedef T3DataView inherited;
#endif
public:
  T3DataView_List       children; // #SHOW #READ_ONLY #SHOW_TREE child view objects
  bool         hasChildren() const override {return (children.size > 0);}

  void         OnWindowBind(iT3Panel* vw) override;
  void         CloseChild(taDataView* child) override;

  void         InitLinks() override;
  void         CutLinks() override;
  T3_DATAVIEWFUNS(T3DataViewPar, T3DataView)

protected:
  void         DoActionChildren_impl(DataViewAction acts) override;
  void         ReInit_impl() override;
  //note: does a depth-first calls to children, before self

private:
  void  Copy_(const T3DataViewPar& cp);
  void                  Initialize() {}
  void                  Destroy() {CutLinks();}
};

#endif // T3DataViewPar_h

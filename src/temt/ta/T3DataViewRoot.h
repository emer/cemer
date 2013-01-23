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

#ifndef T3DataViewRoot_h
#define T3DataViewRoot_h 1

// parent includes:
#include <T3DataViewPar>

// member includes:

// declare all other types mentioned but not required to include:

TypeDef_Of(T3DataViewRoot);

class TA_API T3DataViewRoot: public T3DataViewPar {
  // #NO_TOKENS Root item for a viewwidget type
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class iT3DataViewFrame;
friend class T3DataViewFrame;
public:
  ISelectableHost*      host; // ss/b set by owner

//note: typically never has a non-null parent, because it is rooted in non-T3DataView
  override T3DataViewRoot* root() {return this;}
  override bool         isRootLevelView() const {return true;}

  T3_DATAVIEWFUNS(T3DataViewRoot, T3DataViewPar)

// ISelectable i/f
  override GuiContext   shType() const {return GC_DUAL_DEF_VIEW;} // the default for mains

protected:
  override void         Constr_Node_impl();

private:
  NOCOPY(T3DataViewRoot)
  void                  Initialize();
  void                  Destroy() {}
};


#endif // T3DataViewRoot_h

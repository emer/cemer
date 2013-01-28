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

#ifndef UnitView_h
#define UnitView_h 1

// parent includes:
#include "network_def.h"
#include <T3DataView>

// member includes:

// declare all other types mentioned but not required to include:
class LayerView; //
TypeDef_Of(LayerView);
class Unit; //
class NetView; //
class T3UnitNode; //

TypeDef_Of(UnitView);

class EMERGENT_API UnitView: public T3DataView {
  // control the display of a single unit -- this is only for non-optimized unit display types (UDM_BLOCK is optimized and renders directly, without these sub-objects)
INHERITED(T3DataView)
  friend class LayerView;
public:
#ifndef __MAKETA__
  int           picked : 1;     // #IGNORE set when unit is picked for display (note: not same as selected)
#endif
  Unit*                 unit() const {return (Unit*)data();}
  LayerView*            lay() {return GET_MY_OWNER(LayerView);}
  T3UnitNode*           node_so() const {return (T3UnitNode*)inherited::node_so();}
  NetView*              getNetView();

  override void         CutLinks();
  T3_DATAVIEWFUNS(UnitView, T3DataView)
protected:
  override void         Render_pre(); //
  // note: _impl is done by the LayerView
private:
  void Copy_(const UnitView& cp) {m_nv = NULL;}
  void                  Initialize();
  void                  Destroy() {CutLinks();}

  NetView*              m_nv; // cache
};

#endif // UnitView_h

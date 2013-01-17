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

#ifndef LayerGroupView_h
#define LayerGroupView_h 1

// parent includes:
#include <nvhDataView>

// member includes:

// declare all other types mentioned but not required to include:
class Layer_Group; //
class T3LayerGroupNode; //
class BaseSpec; //

TypeDef_Of(LayerGroupView);

class EMERGENT_API LayerGroupView: public nvhDataView {
INHERITED(nvhDataView)
public:
  bool                  root_laygp; // true if this is network.layers root layer group

  Layer_Group*          layer_group() const {return (Layer_Group*)data();}
  T3LayerGroupNode*     node_so() const {return (T3LayerGroupNode*)inherited::node_so();}

  override void         BuildAll(); // creates fully populated subviews
  virtual void          UpdateUnitValues(); // *only* updates unit values
  virtual void          InitDisplay();

  virtual void          UpdateAutoScale(bool& updated);
  virtual void          SetHighlightSpec(BaseSpec* spec);

  override DumpQueryResult Dump_QuerySaveMember(MemberDef* md); // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerGroupView, nvhDataView)
protected:
  override void         DoHighlightColor(bool apply);
  override void         DataUpdateAfterEdit_impl(); // also invoke for the connected prjns
  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE
private:
  NOCOPY(LayerGroupView)
  void                  Initialize();
  void                  Destroy();
};

#endif // LayerGroupView_h

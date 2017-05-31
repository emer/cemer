// Copyright 2017, Regents of the University of Colorado,
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

#ifndef LayerView_h
#define LayerView_h 1

// parent includes:
#include <nvhDataView>

// member includes:

// declare all other types mentioned but not required to include:
class T3LayerNode; //
class T3DataViewMain; //
class Layer; //
class BaseSpec; //

eTypeDef_Of(LayerView);

class E_API LayerView: public nvhDataView {
INHERITED(nvhDataView)
public:
  enum DispMode {
    DISP_UNITS,                         // display units (standard)
    DISP_OUTPUT_NAME,                   // display output_name field (what layer has generated)
    DISP_FRAME,                         // only display outer frame -- useful when using another viewer for the layer data (e.g., a grid view)
  };

  DispMode              disp_mode; // how to display layer information


  void         BuildAll() override; // creates fully populated subviews
  virtual void          InitDisplay();
  virtual void          UpdateUnitValues(); // *only* updates unit values

  Layer*                layer() const {return (Layer*)data();}
  T3LayerNode*          node_so() const {return (T3LayerNode*)inherited::node_so();}

  virtual void  UpdateNetLayDispMode();
  // update network's storing of the layer display mode value

  virtual void  DispUnits();
  // #BUTTON #VIEWMENU display standard representation of unit values
  virtual void  DispOutputName();
  // #BUTTON #VIEWMENU display contents of output_name on layer instead of unit values
  virtual void  UseViewer(T3DataViewMain* viewer);
  // #BUTTON #VIEWMENU #SCOPE_T3Panel replace usual unit view display with display from viewer (only displays frame of layer, and aligns given viewer with layer)

  virtual void          UpdateAutoScale(bool& updated);
  virtual void          SetHighlightSpec(BaseSpec* spec);

  bool         hasViewProperties() const override { return true; }

  DumpQueryResult Dump_QuerySaveMember(MemberDef* md) override; // don't save ugs and lower
  T3_DATAVIEWFUNS(LayerView, nvhDataView)
protected:
  void         UpdateAfterEdit_impl() override;
  void         DoHighlightColor(bool apply) override;
  void         SigRecvUpdateAfterEdit_impl() override; // also invoke for the connected prjns
  void         Render_pre() override; // #IGNORE
  void         Render_impl() override; // #IGNORE
private:
  NOCOPY(LayerView)
  void                  Initialize();
  void                  Destroy();
};

#endif // LayerView_h

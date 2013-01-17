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

#ifndef UnitGroupView_h
#define UnitGroupView_h 1

// parent includes:
#include "network_def.h"
#include <nvDataView>

// member includes:
#include <voidptr_Matrix>
#include <float_Matrix>
#include <CircMatrix>

// declare all other types mentioned but not required to include:
class Unit; //
class Layer; //
class LayerView; //
class T3UnitGroupNode; //


TypeDef_Of(UnitGroupView);

class EMERGENT_API UnitGroupView: public nvDataView {
  // does all the rendering of unit values, either direct optimized 3D_BLOCK rendering or managing -- there is ONLY ONE of these objects per layer, and it manages all the units regardless of whether there are sub unit groups
INHERITED(nvDataView)
  friend class LayerView;
public:
  static void           ValToDispText(float val, String& str); // renders the display text, typ 6 chars max

  voidptr_Matrix        uvd_bases; // #IGNORE [x][y][nv->membs.size] void* base pointers to unit values -- computed during Init -- note that bases for all members are encoded, so switching members does not require recompute, and this also speeds history saving
  float_Matrix          uvd_hist; // #IGNORE [x][y][nv->membs.size][hist_max] buffer of history of previous value data -- last (frame) dimension uses circ buffer system for efficient storing and retrieval
  CircMatrix            uvd_hist_idx; // #IGNORE circular buffer indexing of uvd_hist -- last (frame) dimension uses circ buffer system for efficient storing and retrieval

  Layer*                layer() const {return (Layer*)data();}
  T3UnitGroupNode*      node_so() const {return (T3UnitGroupNode*)inherited::node_so();}

  LayerView*            lv() const { return m_lv; }
  void                  SetLayerView(LayerView* l) { m_lv = l; }

  void                  AllocUnitViewData(); // make sure we have correct space in uvd storage
  override void         BuildAll(); // creates fully populated subviews
  virtual void          InitDisplay();

  float                 GetUnitDisplayVal(const taVector2i& co, void*& base);
  // get raw floating point value to display according to current nv settings, at given *logical* coordinate within the layer -- fills in base for this value as well (NULL if not set) -- uses history values if nv hist_idx > 0
  float                 GetUnitDisplayVal_Idx(const taVector2i& co, int midx, void*& base);
  // get raw floating point value to display at given member index (< membs.size), at given *logical* coordinate -- fills in base for this value as well (NULL if not set) -- does NOT use history vals ever
  void                  UpdateUnitViewBases(Unit* src_u);
  // update base void* for all current nv->membs, src_u only used for s./r. values
  virtual void          UpdateUnitValues();
  // *only* updates unit values
  virtual void          UpdateUnitValues_blocks();
  // *only* updates unit values: optimized blocks mode
  virtual void          UpdateUnitValues_outnm();
  // output name mode update
  virtual void          UpdateUnitValues_snap_bord();
  // snap border
  virtual void          UpdateAutoScale(bool& updated);
  // update autoscale values
  virtual void          SaveHist();
  // save current values to usv_hist history -- saves all the possible variables, not just currently displayed one (expensive but worth it..)

  override void         InitLinks();
  override void         CutLinks();
  T3_DATAVIEWFUNS(UnitGroupView, nvDataView)
protected:
  LayerView*            m_lv;
  void          UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md); // for unit members
  void          UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md); // for unit submembers
  void          UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u);
  // for cons
  void          UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md);
  // for bias vals
  override void         DoActionChildren_impl(DataViewAction acts);
  virtual void          Render_impl_children(); // #IGNORE we trap this in DoActionChildren
  virtual void          Render_impl_blocks(); // optimized blocks
  virtual void          Render_impl_outnm(); // output name
  virtual void          Render_impl_snap_bord(); // snap border

  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE
  override void         Reset_impl(); // #IGNORE
private:
  NOCOPY(UnitGroupView)
  void                  Initialize();
  void                  Destroy();
};

#endif // UnitGroupView_h

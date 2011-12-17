// Copyright, 1995-2011, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
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

#ifndef BRAIN_VOLUME_VIEW_H
#define BRAIN_VOLUME_VIEW_H

#include "netstru_qtso.h" // overkill #include
class T3BrainNode;
class BrainView;

class EMERGENT_API BrainVolumeView: public T3DataViewPar {
  // does all the rendering of unit values, either direct optimized 3D_BLOCK rendering or managing -- there is ONLY ONE of these objects per layer, and it manages all the units regardless of whether there are sub unit groups
INHERITED(T3DataViewPar)
public:

  voidptr_Matrix        uvd_bases; // #IGNORE [x][y][nv->membs.size] void* base pointers to unit values -- computed during Init -- note that bases for all members are encoded, so switching members does not require recompute
  float_Matrix          cur_disp_vals;  // current display values to render --- extract from units and then use this to render to volume

  Network*              net() const;
  T3BrainNode*  node_so() const;
  BrainView*    bv();

  void          AllocUnitViewData(); // make sure we have correct space in uvd storage
  override void BuildAll(); // creates fully populated subviews
  virtual void  InitDisplay();
  void          UpdateSlices(); // #IGNORE
  float                 GetUnitDisplayVal(const Unit* u, void*& base);
  float                 GetUnitDisplayVal(const TwoDCoord& co, void*& base);
  // get raw floating point value to display according to current nv settings, at given *logical* coordinate within the layer -- fills in base for this value as well (NULL if not set)
  float                 GetUnitDisplayVal_Idx(const TwoDCoord& co, int midx, void*& base);
  // get raw floating point value to display at given member index (< membs.size), at given *logical* coordinate -- fills in base for this value as well (NULL if not set)
  void                  UpdateUnitViewBases(Unit* src_u);
  // update base void* for all current nv->membs, src_u only used for s./r. values
  virtual void          UpdateUnitValues();
  // *only* updates unit values
  virtual void          UpdateUnitValues_blocks();
  // *only* updates unit values: optimized blocks mode
  virtual void          UpdateAutoScale(bool& updated);
  // update autoscale values

  Layer*            layer() const {return (Layer*)data();}
  override void         InitLinks();
  override void         CutLinks();
  T3_DATAVIEWFUNS(BrainVolumeView, T3DataViewPar)


protected:
  void          UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md); // for unit members
  void          UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md); // for unit submembers
  void          UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u);
  // for cons
  void          UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md);
  // for bias vals  
  override void         DoActionChildren_impl(DataViewAction acts);
  virtual void          Render_impl_children(); // #IGNORE we trap this in DoActionChildren
  virtual void          Render_impl_blocks(); // optimized blocks

  override void         Render_pre(); // #IGNORE
  override void         Render_impl(); // #IGNORE
  override void         Reset_impl(); // #IGNORE
  override void         DataUpdateView_impl(); // #IGNORE

private:
  NOCOPY(BrainVolumeView)
  void              Initialize();
  void              Destroy();
  void              CreateFaceSets(); // #IGNORE
  void              RenderBrain();    // #IGNORE

  NiftiReader*                   brain_data_;// #IGNORE
#ifndef __MAKETA__
  QMultiMap<unsigned int, Unit*> depth_map_; //#IGNORE
  QMap<const Unit*, FloatTDCoord>      unit_map_; //#IGNORE 
  QMap<const Unit*, unsigned int>      uvd_bases_map_; //#IGNORE
#endif
};

#endif // BRAIN_VOLUME_VIEW_H

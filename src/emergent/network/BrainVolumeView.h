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

#ifndef BrainVolumeView_h
#define BrainVolumeView_h 1

// parent includes:
#include "network_def.h"
#include <T3DataViewPar>

// member includes:
#include <voidptr_Matrix>
#include <float_Matrix>
#include <BrainView>

#ifndef __MAKETA__
#include <QMultiMap>
#include <QMap>
#include <QList>
#endif

// declare all other types mentioned but not required to include:
class Network; //
class Voxel; //
class Unit; //
class T3BrainNode; // #IGNORE
class NiftiReader; // #IGNORE

TypeDef_Of(BrainVolumeView);

class EMERGENT_API BrainVolumeView: public T3DataViewPar {
  // does all the rendering of unit values, either direct optimized 3D_BLOCK rendering or managing -- there is ONLY ONE of these objects per layer, and it manages all the units regardless of whether there are sub unit groups
INHERITED(T3DataViewPar)
public:
  voidptr_Matrix uvd_bases; // #IGNORE [x][y][nv->membs.size] void* base pointers to unit values -- computed during Init -- note that bases for all members are encoded, so switching members does not require recompute
  float_Matrix  cur_disp_vals;  // current display values to render --- extract from units and then use this to render to volume

  Network*      net() const;
  T3BrainNode*  node_so() const;
  BrainView*    getBrainView();

  void          AllocUnitViewData(); // make sure we have correct space in uvd storage
  override void BuildAll(); // creates fully populated subviews
  virtual void  InitDisplay();
  void          UpdateSlices(); // #IGNORE
  float         GetUnitDisplayVal(const Unit* u, void*& base);
  // get raw floating point value to display at given member index (< membs.size), at given *logical* coordinate -- fills in base for this value as well (NULL if not set)
  void          UpdateUnitViewBases(Unit* src_u);
  // update base void* for all current nv->membs, src_u only used for s./r. values
  virtual void  UpdateUnitValues();
  // *only* updates unit values
  virtual void  UpdateUnitValues_blocks();
  // *only* updates unit values: optimized blocks mode
  virtual void  UpdateAutoScale(bool& updated);
  // update autoscale values

  override void InitLinks();
  override void CutLinks();
  T3_DATAVIEWFUNS(BrainVolumeView, T3DataViewPar)


protected:
  void          UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md); // for unit members
  void          UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md); // for unit submembers
  void          UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u);
  // for cons
  void          UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md);
  // for bias vals
  override void  DoActionChildren_impl(DataViewAction acts);
  virtual void   Render_impl_children(); // #IGNORE we trap this in DoActionChildren
  virtual void   Render_impl_blocks();  // optimized blocks

  override void  Render_pre();          // #IGNORE
  override void  Render_impl();         // #IGNORE
  override void  Reset_impl();          // #IGNORE
  override void  DataUpdateView_impl(); // #IGNORE

private:
  NOCOPY(BrainVolumeView)
  void           Initialize();
  void           Destroy();
  bool           ColorBrain();           // #IGNORE
  void           RenderBrain();          // #IGNORE
  void           CreateReferenceWidget( SoSeparator* widg, BrainView::AnatomicalPlane p, float max_dim, float scale=0.0f ); // #IGNORE
  void           SliceAsTexture( BrainView::AnatomicalPlane p, int index, unsigned char* data, NiftiReader* brain_data );       // #IGNORE
  void           SliceAsColorTexture( BrainView::AnatomicalPlane p, int index, unsigned char* data, NiftiReader* brain_data, NiftiReader* atlas_data ); // #IGNORE
  void           CreateFaceSets();        // #IGNORE
  void           CreateAtlasFaceSets(String brain_area, T3Color area_color); // #IGNORE
  void           UpdateAtlasFaceValues(float alpha);

  NiftiReader*   m_brain_data;// #IGNORE
  NiftiReader*   m_atlas_data;// #IGNORE

#ifndef __MAKETA__
  QMultiMap<unsigned int, taVector3f> m_atlas_depth_map; //#IGNORE
  QMultiMap<unsigned int, Voxel*>       m_units_depth_map; //#IGNORE
  QMap<const Voxel*, taVector3f>      m_voxel_map; //#IGNORE
  QMap<const Unit*, unsigned int>       m_uvd_bases_map; //#IGNORE
  QList<QColor>                         m_atlasColors; //#IGNORE
#endif
};

#endif // BrainVolumeView_h

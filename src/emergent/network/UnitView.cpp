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

#include "UnitView.h"
#include <NetView>
#include <Network>
#include <LayerView>
#include <iColor>

#include <T3UnitNode_Circle>
#include <T3UnitNode_Rect>
#include <T3UnitNode_Block>
#include <T3UnitNode_Cylinder>

#include <Inventor/nodes/SoTransform.h>

TA_BASEFUNS_CTORS_DEFN(UnitView);

void UnitView::Initialize()
{
  m_nv = NULL;
  picked = 0;
}

void UnitView::CutLinks()
{
  m_nv = NULL;
  inherited::CutLinks();
}

NetView* UnitView::getNetView()
{
  if (!m_nv) {
    if (LayerView* layv = lay()) {
      // Cache the value on the first request.
      m_nv = layv->getNetView();
    }
  }
  return m_nv;
}

void UnitView::Render_pre()
{
  NetView* nv = getNetView();
  float max_x = nv->eff_max_size.x;
  float max_y = nv->eff_max_size.y;
  float max_z = nv->eff_max_size.z;
  float un_spc= nv->view_params.unit_spacing;

  Unit* unit = this->unit(); //cache
  Layer* lay = unit->own_lay();
  float disp_scale = lay->disp_scale;
  taVector2i upos;  unit->LayerDispPos(upos);

#ifdef TA_QT3D

  switch (nv->unit_disp_mode) {
  case NetView::UDM_CIRCLE:
    setNode(new T3UnitNode_Circle(NULL, this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_RECT:
    setNode(new T3UnitNode_Rect(NULL, this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_BLOCK:
    setNode(new T3UnitNode_Block(NULL, this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_CYLINDER:
    setNode(new T3UnitNode_Cylinder(NULL, this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  }

  node_so()->Translate(disp_scale * ((float)(upos.x + 0.5f) / max_x), 0.0f,
                       -disp_scale * (((float)(upos.y + 0.5f) / max_y)));
  
#else // TA_QT3D

  switch (nv->unit_disp_mode) {
  case NetView::UDM_CIRCLE:
    setNode(new T3UnitNode_Circle(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_RECT:
    setNode(new T3UnitNode_Rect(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_BLOCK:
    setNode(new T3UnitNode_Block(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  case NetView::UDM_CYLINDER:
    setNode(new T3UnitNode_Cylinder(this, max_x, max_y, max_z, un_spc, disp_scale));
    break;
  }
  node_so()->transform()->translation.setValue
    (disp_scale * ((float)(upos.x + 0.5f) / max_x), 0.0f,
     -disp_scale * (((float)(upos.y + 0.5f) / max_y)));
  
#endif // TA_QT3D

  inherited::Render_pre();
}


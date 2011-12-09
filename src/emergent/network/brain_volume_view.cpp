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

#include "brain_view.h"
#include "t3brain_node.h"
#include "brain_volume_view.h"
#include "nifti_reader.h"

#include <Inventor/SoEventManager.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoShapeHints.h>

Network* BrainVolumeView::net() const
{
  return (Network*) data();
}

T3BrainNode* BrainVolumeView::node_so() const
{
  return (T3BrainNode*) inherited::node_so();
}

BrainView* BrainVolumeView::bv()
{
 return GET_MY_OWNER(BrainView);
}

void BrainVolumeView::Initialize()
{
  data_base = &TA_Network;
  brain_data_ = NULL;
}

void BrainVolumeView::InitLinks()
{
  inherited::InitLinks();
}

void BrainVolumeView::CutLinks()
{
  inherited::CutLinks();
}

void BrainVolumeView::Destroy()
{
  Reset();
}

void BrainVolumeView::AllocUnitViewData() {}

void BrainVolumeView::BuildAll()
{
  BrainView* bv = this->bv();
  Reset(); // in case where we are rebuilding
  UpdateUnitViewBases(bv->unit_src);
}

void BrainVolumeView::InitDisplay()
{
  BrainView* bv = this->bv();
  UpdateUnitViewBases(bv->unit_src);
}

float BrainVolumeView::GetUnitDisplayVal(const Unit* u)
{
  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  if(!u) return val;
  val = u->act;
  return val;
}

float BrainVolumeView::GetUnitDisplayVal(const TwoDCoord& co, void*& base)
{
  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  if(bv->unit_disp_idx < 0) return val;
  base = uvd_bases.SafeEl(co.x, co.y, bv->unit_disp_idx);
  if (!base) return val;

  switch (bv->unit_md_flags) {
    case BrainView::MD_FLOAT:
      val = *((float*)base); break;
    case BrainView::MD_DOUBLE:
      val = *((double*)base); break;
    case BrainView::MD_INT:
      val = *((int*)base); break;
    default:
      val = 0.0f; break;
  }
  return val;
}

float BrainVolumeView::GetUnitDisplayVal_Idx(const TwoDCoord& co, int midx, void*& base)
{
  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  base = uvd_bases.SafeEl(co.x, co.y, midx);
  MemberDef* md = bv->membs.SafeEl(midx);
  if(!base) {
    return val;
  }
  if (md) {
    if(md->type->InheritsFrom(&TA_float))
      val = *((float*)base);
    else if(md->type->InheritsFrom(&TA_double))
      val = *((double*)base);
    else if(md->type->InheritsFrom(&TA_int))
      val = *((int*)base);
  }
  return val;
}

void BrainVolumeView::UpdateUnitViewBases(Unit* src_u)
{
//  BrainView* bv = this->bv();
//  AllocUnitViewData();
//  for(int midx=0;midx<bv->membs.size;midx++) {
//    MemberDef* disp_md = bv->membs[midx];
//    String nm = disp_md->name.before(".");
//    if(nm.empty()) { // direct unit member
//      UpdateUnitViewBase_Unit_impl(midx, disp_md);
//    } else if ((nm=="s") || (nm == "r")) {
//      UpdateUnitViewBase_Con_impl(midx, (nm=="s"), disp_md->name.after('.'), src_u);
//    } else if (nm=="bias") {
//      UpdateUnitViewBase_Bias_impl(midx, disp_md);
//    } else { // sub-member of unit
//      UpdateUnitViewBase_Sub_impl(midx, disp_md);
//    }
//  }
}

void BrainVolumeView::UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u) {}
void BrainVolumeView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {}
void BrainVolumeView::UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md) {}
void BrainVolumeView::UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md) {}
void BrainVolumeView::UpdateAutoScale(bool& updated) {}

void BrainVolumeView::Render_pre()
{
  BrainViewState& bvs = this->bv()->bv_state;
  brain_data_ = new NiftiReader(bvs.DataName());
  if (true == brain_data_->isValid()) {
    setNode(new T3BrainNode(this));
    RenderBrain();
  }
  inherited::Render_pre();
}

void BrainVolumeView::RenderBrain()
{
  //Creates the nodes that render the reference brain
  BrainViewState& bvs = this->bv()->bv_state;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());

  SoSeparator* ss = node.shapeSeparator();
  node.brain_group = new SoSeparator();

  SoTransform* global_xform = new SoTransform;
  node.brain_group->addChild(global_xform);
  global_xform->translation.setValue(SbVec3f(0.0,0.0,0.0)); 
  global_xform->scaleFactor.setValue(SbVec3f(0.0075,0.0075,0.0075)); //arbitrary scale seems to work

  // set the "origin"
  TDCoord dims(brain_data_->xyzDimensions());
  SoTransform* b0 = new SoTransform;
  int d1(0),d2(0),d3(0);
  if (bvs.ViewPlane() == BrainViewState::CORONAL){
    d1 = dims.x;
    d2 = dims.z;
    d3 = dims.y;
    b0->translation.setValue(SbVec3f(0.0, dims.z/2, -dims.y/2.0f));
  }
  else if (bvs.ViewPlane() == BrainViewState::SAGITTAL){
    d1 = dims.y;
    d2 = dims.z;
    d3 = dims.x;
    b0->translation.setValue(SbVec3f(0.0f, dims.z/2.0f, -dims.x/2.0f));
  }
  else { //BrainViewState::AXIAL)
    d1 = dims.x;
    d2 = dims.y;
    d3 = dims.z;
    //this used to be offset by dims.y/2.0f, but not important now that we're 
    //drawing everything from same coordinate transform space...so we make it 
    //close/similar to same offset as with other viewplanes
    b0->translation.setValue(SbVec3f(0.0f, dims.z/2.0f, -dims.z/2.0f));
  }
  node.brain_group->addChild(b0);

  // allocate texture and face set for each "slice" in 3rd dimension
  // we will render one brain slice and one set of activations per "slice"
  // in 3rd dimensions...to preserve transparency hierarchy when using blend render mode
  node.brain_tex_mat_array = new SoMaterial* [d3];
  node.voxel_face_set_array = new SoIndexedFaceSet* [d3];
  node.voxel_vrtx_prop_array = new SoVertexProperty* [d3];

  //if revert to CopyPolicy::Copy then do allocation out of loop and reuse
  //unsigned char* tex = new unsigned char[d1*d2*2];
  unsigned char* tex(0);
  float transparency(bvs.SliceTransparency()/100.0f);
  for (int i=0; i<d3; i++) {
    tex = static_cast<unsigned char*>(malloc(d1*d2*2));
    brain_data_->sliceAsTexture((NiftiReader::AnatomicalPlane)bvs.ViewPlane(), 1+i, tex);

    node.brain_tex_mat_array[i] = new SoMaterial;
    node.brain_tex_mat_array[i]->transparency = transparency;
    node.brain_tex_mat_array[i]->diffuseColor.setValue(SbVec3f(1.0f,1.0f,1.0f));

    node.voxel_face_set_array[i] = new SoIndexedFaceSet;
    node.voxel_vrtx_prop_array[i] = new SoVertexProperty;
    node.voxel_face_set_array[i]->vertexProperty.setValue(node.voxel_vrtx_prop_array[i]);
    
    SoSeparator* b = new SoSeparator;
    // shape hint needed for sorted blend rendering (when used)
    SoShapeHints* sh = new SoShapeHints;
    sh->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    sh->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    SoTexture2* texture = new SoTexture2;
    //using a CopyPolicy may cause intermittent crashes on Windows??
    //http://doc.coin3d.org/Coin-3.1/classSoSFImage.html
    texture->image.setValue(SbVec2s(d1, d2),2,tex,SoSFImage::NO_COPY_AND_FREE);
    texture->wrapS = SoTexture2::CLAMP;
    texture->wrapT = SoTexture2::CLAMP;

    // slice face
    SoTextureCoordinate2* tcoords = new SoTextureCoordinate2;
    tcoords->point.set1Value(0, SbVec2f(0.0f, 1.0f));
    tcoords->point.set1Value(1, SbVec2f(0.0f, 0.0f));
    tcoords->point.set1Value(2, SbVec2f(1.0f, 0.0f));
    tcoords->point.set1Value(3, SbVec2f(1.0f, 1.0f));
        
    SoCoordinate3* coords = new SoCoordinate3;
    coords->point.set1Value(0, SbVec3f(-d1/2.0f,  d2/2.0f, 0));
    coords->point.set1Value(1, SbVec3f(-d1/2.0f, -d2/2.0f, 0));
    coords->point.set1Value(2, SbVec3f( d1/2.0f, -d2/2.0f, 0));
    coords->point.set1Value(3, SbVec3f( d1/2.0f,  d2/2.0f, 0));
    
    SoFaceSet* fs = new SoFaceSet;
    fs->numVertices.set1Value(0,4);
    
    b->addChild(sh);
    b->addChild(node.brain_tex_mat_array[i]);
    b->addChild(texture);
    b->addChild(tcoords);
    b->addChild(coords);
    b->addChild(fs);

    SoSeparator* s = new SoSeparator; //necessary to separate texmap
    s->addChild(node.voxel_face_set_array[i]);

    node.brain_group->addChild(b);
    node.brain_group->addChild(s);

    node.spacing_xform = new SoTransform;
    node.brain_group->addChild(node.spacing_xform);
    node.spacing_xform->translation.setValue(SbVec3f(0.0,0.0,bvs.SliceSpacing()));
  }
  //if revert to CopyPolicy::Copy then delete allocated tex
  //delete [] tex;

  ss->addChild(node.brain_group);
}

void BrainVolumeView::UpdateSlices()
{
  // Hides slices not currently in stack defined by start,end
  BrainViewState& bvs = bv()->bv_state;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so()); // cache

  // now update slices
  float transparency(bvs.SliceTransparency()/100.0f);
  for (int i=0; i<bvs.MaxSlices(); i++) {
    if ( ((bvs.SliceStart() - 1) <= i) && (i <= (bvs.SliceEnd() - 1)) ) {
      node.brain_tex_mat_array[i]->transparency = transparency;
    }
    else {
      node.brain_tex_mat_array[i]->transparency = 1.0f;
    }
  }
  UpdateUnitValues_blocks();
}

void BrainVolumeView::Render_impl()
{
  inherited::Render_impl();
}

void BrainVolumeView::DoActionChildren_impl(DataViewAction acts)
{
  if (acts & RENDER_IMPL) {
    acts = (DataViewAction)( acts & ~RENDER_IMPL); // note: only supposed to be one, but don't assume
    Render_impl_children();
    if (!acts) return;
  }
  inherited::DoActionChildren_impl(acts);
}

void BrainVolumeView::Render_impl_children()
{
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  T3BrainNode* node_so = this->node_so(); // cache
  if(!node_so) return;

  Render_impl_blocks();
}

void BrainVolumeView::Render_impl_blocks()
{
  // create unit/voxel face sets & set values
  CreateFaceSets();
  UpdateSlices(); // will call UpdateUnitValues_blocks ...
}

void BrainVolumeView::CreateFaceSets()
{
  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());
  BrainViewState& bvs = this->bv()->bv_state;

  BrainViewState::AnatomicalPlane view_plane = bvs.ViewPlane();
  FloatTDCoord dims(bvs.Dimensions());
  FloatTDCoord halfDims;
  halfDims.SetXYZ(dims.x/2.0f, dims.y/2.0f, dims.z/2.0f);

  // clear the old maps
  depth_map_.clear();
  unit_map_.clear();

  //  // need to iterate once to get number of mapped units so we
  //  // can allocate number of vertices, etc.
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if (lay->lesioned() || lay->Iconified() || lay->brain_area.empty()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      //if (u->voxel == FloatTDCoord(-1,-1,-1)) continue;
      FloatTDCoord talCoord(u->voxel);
      FloatTDCoord mniCoord(TalairachAtlas::Tal2Mni(talCoord));
      if ( (view_plane == BrainViewState::AXIAL) || (view_plane == BrainViewState::SAGITTAL)) {
        mniCoord.x = mniCoord.x * -1; // reverse coordinates for Inventor directions
      }
      FloatTDCoord ijkCoord(brain_data_->XyzToIjk(mniCoord));

      if (view_plane == BrainViewState::AXIAL) {
        depth_map_.insert((unsigned int)ijkCoord.z, u);
      }
      else if (view_plane == BrainViewState::SAGITTAL) {
        depth_map_.insert((unsigned int)ijkCoord.x, u);
      }
      else { //CORONAL
        depth_map_.insert((unsigned int)ijkCoord.y, u);
      }
      unit_map_.insert(u, ijkCoord);
    }
  }

  FloatTDCoord unit_coord;
  for (int s=0; s<bvs.MaxSlices(); s++) {
    QList<Unit*> units = depth_map_.values(s);
    if (0 == units.size()) continue;
    SoIndexedFaceSet* ifs = node.voxel_face_set_array[s];
    SoVertexProperty* vtx_prop = node.voxel_vrtx_prop_array[s];
    if (!ifs || !vtx_prop) return; // something wrong..
    ifs->vertexProperty.setValue(vtx_prop);

    SoMFVec3f& vertex = vtx_prop->vertex;
    SoMFVec3f& normal = vtx_prop->normal;
    SoMFUInt32& color = vtx_prop->orderedRGBA;

    vtx_prop->normalBinding.setValue(SoNormalBinding::OVERALL);
    vtx_prop->materialBinding.setValue(SoMaterialBinding::PER_FACE_INDEXED);

    normal.setNum(1);
    SbVec3f* normal_dat = normal.startEditing();
    int idx=0;
    normal_dat[idx++].setValue(0.0f, 0.0f, 1.0f);
    normal.finishEditing();

    int n_per_vtx = 4;
    int tot_vtx = units.size() * n_per_vtx;

    vertex.setNum(tot_vtx);
    color.setNum(units.size());
    uint32_t* color_dat = color.startEditing();
    SbVec3f* vertex_dat = vertex.startEditing();
    int v_idx = 0;
    int c_idx = 0;

    foreach (Unit* u, units) {
      unit_coord = unit_map_.value(u);
      float ri      = unit_coord.x;
      float rj      = unit_coord.y;
      float rk      = unit_coord.z;
      float extent  = u->voxel_size/2.0f; //half voxel size

      //@TODO add voxel size in 3rd dimension...(by adding more faces at different depth levels)
      if ( view_plane == BrainViewState::AXIAL){
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rj-extent-halfDims.y, rk-s); // 00_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rj-extent-halfDims.y, rk-s );// 10_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rj+extent-halfDims.y, rk-s); // 11_0 = 0
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rj+extent-halfDims.y, rk-s); // 01_0 = 0
      }
      else if ( view_plane == BrainViewState::SAGITTAL ){
        vertex_dat[v_idx++].setValue(rj-extent-halfDims.y, rk-extent-halfDims.z, ri-s); // 00_0 = 0
        vertex_dat[v_idx++].setValue(rj+extent-halfDims.y, rk-extent-halfDims.z, ri-s); // 10_0 = 0
        vertex_dat[v_idx++].setValue(rj+extent-halfDims.y, rk+extent-halfDims.z, ri-s); // 11_0 = 0
        vertex_dat[v_idx++].setValue(rj-extent-halfDims.y, rk+extent-halfDims.z, ri-s); // 01_0 = 0
      }
      else{ //CORONAL
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rk-extent-halfDims.z, rj-s); // 00_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rk-extent-halfDims.z, rj-s); // 10_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rk+extent-halfDims.z, rj-s); // 11_0 = 0
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rk+extent-halfDims.z, rj-s); // 01_0 = 0
      }
      color_dat[c_idx++] = T3Color::makePackedRGBA(1.0f, 0.0f, 0.0f, 0.5f);
    }
    vertex.finishEditing();
    color.finishEditing();

    SoMFInt32& coords = ifs->coordIndex;
    SoMFInt32& norms = ifs->normalIndex;
    SoMFInt32& mats = ifs->materialIndex;
    int nc_per_idx = 5;          // number of coords per index
    int nn_per_idx = 1;          // number of norms per index
    int nm_per_idx = 1;           // number of mats per index
    coords.setNum(units.size() * nc_per_idx);
    norms.setNum(units.size() * nn_per_idx);
    mats.setNum(units.size() * nm_per_idx);

    int32_t* coords_dat = coords.startEditing();
    int32_t* norms_dat  = norms.startEditing();
    int32_t* mats_dat   = mats.startEditing();

    int cidx = 0;
    int nidx = 0;
    int midx = 0;
    int uidx = 0;
    foreach (Unit* u, units) {
      int c00_0 = uidx * n_per_vtx;
      int c10_0 = c00_0 + 1;
      int c01_0 = c00_0 + 2;
      int c11_0 = c00_0 + 3;

      coords_dat[cidx++] = (c00_0); // 0
      coords_dat[cidx++] = (c10_0); // 1
      coords_dat[cidx++] = (c01_0); // 2
      coords_dat[cidx++] = (c11_0); // 3
      coords_dat[cidx++] = (-1); // -1  -- 5 total

      norms_dat[nidx++] = (0);
      mats_dat[midx++] = (uidx);
      uidx++;
    }
    coords.finishEditing();
    norms.finishEditing();
    mats.finishEditing();
  }
}

void BrainVolumeView::UpdateUnitValues_blocks()
{
  // Update voxel face colors with Unit values
  BrainView* bv = this->bv(); //cache
  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());
  BrainViewState& bvs = this->bv()->bv_state;

  BrainViewState::AnatomicalPlane view_plane = bvs.ViewPlane();

  FloatTDCoord unit_coord;
  for (int s=0; s<bvs.MaxSlices(); s++) {
    SoVertexProperty* vp  = node.voxel_vrtx_prop_array[s];
    SoMFUInt32& color = vp->orderedRGBA;

    bool makeTransparent(false);
    float val, sc_val,alpha,trans = bv->view_params.unit_trans;
    int idx(0);
    T3Color col;
    Unit* u(0);

    uint32_t* color_dat = color.startEditing();
    QList<Unit*> units = depth_map_.values(s);
    if (0 == units.size()) continue;

    foreach (u,units) {
      unit_coord = unit_map_.value(u);
      if ( view_plane == BrainViewState::AXIAL) {
        if ( (unit_coord.z > bvs.SliceEnd()) || (unit_coord.z < bvs.SliceStart()) ) {
          makeTransparent = true;
        }
      }
      else if ( view_plane == BrainViewState::SAGITTAL ) {
        if ( (unit_coord.x > bvs.SliceEnd()) || (unit_coord.x < bvs.SliceStart()) ) {
          makeTransparent = true;
        }
      }
      else { //CORONAL
        if ( (unit_coord.y > bvs.SliceEnd()) || (unit_coord.y < bvs.SliceStart()) ) {
          makeTransparent = true;
        }
      }
      bv->GetUnitDisplayVals(this, u, val, col, sc_val);

      if (true == makeTransparent) {
        alpha = 0.0f;
      }
      else {
        alpha = 1.0f - ((1.0f - fabsf(sc_val)) * trans);
      }
      color_dat[idx++] = T3Color::makePackedRGBA(col.r, col.g, col.b, alpha);
    }

    color.finishEditing();
  }
}

void BrainVolumeView::UpdateUnitValues()
{
  UpdateUnitValues_blocks();
}

void BrainVolumeView::DataUpdateView_impl()
{
//do nothing...we don't need to re-render when Network data changes
}

void BrainVolumeView::Reset_impl()
{
  if (NULL != brain_data_) {
    delete brain_data_;
    brain_data_ = NULL;
  }
  inherited::Reset_impl();
  uvd_bases.Reset();
}


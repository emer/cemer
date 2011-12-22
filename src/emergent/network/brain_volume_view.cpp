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
#include "ta_math.h"

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
  depth_map_.clear();
  voxel_map_.clear();
  uvd_bases_map_.clear();
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

void BrainVolumeView::AllocUnitViewData()
{
  BrainView* bv = this->bv();
  Network* net = this->net();
  if (!net) return;

  int i=0;
  // need to determine number of units that are mapped...
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if (lay->lesioned() || lay->Iconified() || lay->brain_area.empty()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      if (u->voxels.size == 0) continue;
      if (u->lesioned()) continue;
      i++;
    }
  }
  if (i == 0) return;

  int mbs_sz = MAX(bv->membs.size, 1);
  MatrixGeom nwgm1(2, i, mbs_sz);
  if (uvd_bases.geom != nwgm1) {
    uvd_bases.SetGeomN(nwgm1);
  }

//  MatrixGeom nwgm2(3, unit_map_.size(), mbs_sz, bv->hist_max);
//  bool reset_idx = bv->hist_reset_req; // if requested somewhere, reset us!
//  if (uvd_hist.geom != nwgm2) {
//    if (!uvd_hist.SetGeomN(nwgm2)) {
//      taMisc::Warning("Forcing nv->hist_max to 1");
//      bv->hist_max = 1;
//      nwgm2.Set(2, bv->hist_max);
//      uvd_hist.SetGeomN(nwgm2); // still might fail, but it's the best we can do.
//    }
//    reset_idx = true;
//    bv->hist_reset_req = true;  // tell main netview history guy to reset and reset everyone
//  }
//  if (reset_idx) {
//    uvd_hist_idx.Reset();
//  }
}

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

float BrainVolumeView::GetUnitDisplayVal(const Unit* u, void*& base)
{

  BrainView* bv = this->bv();
  float val = bv->scale.zero;
  if(bv->unit_disp_idx < 0) return val;

  base = uvd_bases.SafeEl(uvd_bases_map_.value(u), bv->unit_disp_idx);
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

void BrainVolumeView::UpdateUnitViewBases(Unit* src_u)
{
  BrainView* bv = this->bv();
  AllocUnitViewData();

  for(int midx=0;midx<bv->membs.size;midx++) {
    MemberDef* disp_md = bv->membs[midx];
    String nm = disp_md->name.before(".");
    if(nm.empty()) { // direct unit member
      UpdateUnitViewBase_Unit_impl(midx, disp_md);
    } else if ((nm=="s") || (nm == "r")) {
      UpdateUnitViewBase_Con_impl(midx, (nm=="s"), disp_md->name.after('.'), src_u);
    } else if (nm=="bias") {
      UpdateUnitViewBase_Bias_impl(midx, disp_md);
    } else { // sub-member of unit
      UpdateUnitViewBase_Sub_impl(midx, disp_md);
    }
  }
}

void BrainVolumeView::UpdateUnitViewBase_Con_impl(int midx, bool is_send, String nm, Unit* src_u) {}
void BrainVolumeView::UpdateUnitViewBase_Bias_impl(int midx, MemberDef* disp_md) {}
void BrainVolumeView::UpdateUnitViewBase_Sub_impl(int midx, MemberDef* disp_md) {}
void BrainVolumeView::UpdateUnitViewBase_Unit_impl(int midx, MemberDef* disp_md)
{
  if (uvd_bases_map_.size() == 0) return; //we don't have a list of units yet

  foreach (const Unit* u, uvd_bases_map_.keys()) {
    uvd_bases.Set(disp_md->GetOff(u), uvd_bases_map_.value(u), midx);
  }
}

void BrainVolumeView::UpdateAutoScale(bool& updated) {}

void BrainVolumeView::Render_pre()
{
  brain_data_ = new NiftiReader(bv()->DataName());
  if (true == brain_data_->isValid()) {
    setNode(new T3BrainNode(this));
    RenderBrain();
  }
  inherited::Render_pre();
}

void BrainVolumeView::RenderBrain()
{
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  
  //Creates the nodes that render the reference brain
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());

  // Get the dimensions of the brain
  TDCoord dims(brain_data_->xyzDimensions());
  float max_dim = taMath_float::max(dims.z, taMath_float::max(dims.x, dims.y));
  
  SoSeparator* ss = node.shapeSeparator();
  node.brain_group = new SoSeparator();

  SoTransform* global_xform = new SoTransform;
  node.brain_group->addChild(global_xform);
  global_xform->translation.setValue(SbVec3f(0.0,0.0,0.0));
  global_xform->scaleFactor.setValue(SbVec3f(1/max_dim, 1/max_dim, 1/max_dim)); //normalize by max brain dimension
  
  // set the "origin"
  SoTransform* b0 = new SoTransform;
  int d1(0),d2(0),d3(0);
  if (bv->ViewPlane() == BrainView::CORONAL){
    d1 = dims.x;
    d2 = dims.z;
    d3 = dims.y;
    b0->translation.setValue(SbVec3f(dims.x/2.0f, dims.z/2.0f, -dims.y));
  }
  else if (bv->ViewPlane() == BrainView::SAGITTAL){
    d1 = dims.y;
    d2 = dims.z;
    d3 = dims.x;
    b0->translation.setValue(SbVec3f(dims.y/2.0f, dims.z/2.0f, -dims.x));
  }
  else { //BrainView::AXIAL)
    d1 = dims.x;
    d2 = dims.y;
    d3 = dims.z;
    b0->translation.setValue(SbVec3f(dims.x/2.0f, dims.y/2.0f, -dims.z));
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
  float transparency(bv->SliceTransparencyXformed());
  for (int i=0; i<d3; i++) {
    tex = static_cast<unsigned char*>(malloc(d1*d2*2));
    brain_data_->sliceAsTexture((NiftiReader::AnatomicalPlane)bv->ViewPlane(), 1+i, tex);

    node.brain_tex_mat_array[i] = new SoMaterial;
    node.brain_tex_mat_array[i]->transparency = transparency;
    node.brain_tex_mat_array[i]->diffuseColor.setValue(SbVec3f(0.0f,0.0f,0.0f));
    node.brain_tex_mat_array[i]->emissiveColor.setValue(SbVec3f(1.0f,1.0f,1.0f));

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
    node.spacing_xform->translation.setValue(SbVec3f(0.0,0.0,bv->SliceSpacing()));
  }
  //if revert to CopyPolicy::Copy then delete allocated tex
  //delete [] tex;

  ss->addChild(node.brain_group);
}

void BrainVolumeView::UpdateSlices()
{
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  
  // Hides slices not currently in stack defined by start,end
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so()); // cache

  // now update slices
  float transparency(bv->SliceTransparencyXformed());
  for (int i=0; i<bv->MaxSlices(); i++) {
    if ( ((bv->SliceStart() - 1) <= i) && (i <= (bv->SliceEnd() - 1)) ) {
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
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  T3BrainNode& node = *(this->node_so());

  BrainView::AnatomicalPlane view_plane = bv->ViewPlane();
  FloatTDCoord dims(bv->Dimensions());
  FloatTDCoord halfDims;
  halfDims.SetXYZ(dims.x/2.0f, dims.y/2.0f, dims.z/2.0f);

  // clear the old maps
  depth_map_.clear();
  voxel_map_.clear();
  uvd_bases_map_.clear();

  
  // create the maps
  int i=0;
  FOREACH_ELEM_IN_GROUP(Layer, lay, net->layers) {
    if (lay->lesioned() || lay->Iconified() || lay->brain_area.empty()) continue;
    FOREACH_ELEM_IN_GROUP(Unit, u, lay->units) {
      if (u->lesioned()) continue;
      if (u->voxels.size == 0) continue;
      // TODO: for now, assumes only one voxel per unit.  Update to handle multiple.
      FOREACH_ELEM_IN_LIST(Voxel, v, u->voxels) {
        if (v->size == 0) continue;
      
        FloatTDCoord talCoord(v->coord);
        FloatTDCoord mniCoord(TalairachAtlas::Tal2Mni(talCoord));
        FloatTDCoord ijkCoord(brain_data_->XyzToIjk(mniCoord));
        if ( (view_plane == BrainView::AXIAL) || (view_plane == BrainView::SAGITTAL) ) {
          // reverse x coordinates...since we draw X in opposite direction in Inventor
          ijkCoord.x = taMath_float::fabs(ijkCoord.x - (dims.x - 1));
        }
        
        if (view_plane == BrainView::AXIAL) {
          depth_map_.insert((unsigned int)ijkCoord.z, v);
        }
        else if (view_plane == BrainView::SAGITTAL) {
          depth_map_.insert((unsigned int)ijkCoord.x, v);
        }
        else { //CORONAL
          depth_map_.insert((unsigned int)ijkCoord.y, v);
        }
        voxel_map_.insert(v, ijkCoord);
        uvd_bases_map_.insert(u,i); //map unit* to index, so we can index into uvd_bases
        i++;
      }
    }
  }
  
  // iterate over all slices, and for each voxel at that slice depth, create face in face set
  FloatTDCoord voxel_coord;
  for (int s=0; s<bv->MaxSlices(); s++) {
    QList<Voxel*> voxels = depth_map_.values(s);
    if (0 == voxels.size()) continue;
    
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
    int tot_vtx = voxels.size() * n_per_vtx;

    vertex.setNum(tot_vtx);
    color.setNum(voxels.size());
    uint32_t* color_dat = color.startEditing();
    SbVec3f* vertex_dat = vertex.startEditing();
    int v_idx = 0;
    int c_idx = 0;

    // for each voxel at current slice depth, create face vertices in face set
    foreach (Voxel* v, voxels) {
      voxel_coord = voxel_map_.value(v);
      float ri      = voxel_coord.x;
      float rj      = voxel_coord.y;
      float rk      = voxel_coord.z;

      float extent  = v->size/2.0f; //half voxel size

      //@TODO add voxel size in 3rd dimension...(by adding more faces at different depth levels)
      if ( view_plane == BrainView::AXIAL){
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rj-extent-halfDims.y, rk-s); // 00_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rj-extent-halfDims.y, rk-s );// 10_0 = 0
        vertex_dat[v_idx++].setValue(ri+extent-halfDims.x, rj+extent-halfDims.y, rk-s); // 11_0 = 0
        vertex_dat[v_idx++].setValue(ri-extent-halfDims.x, rj+extent-halfDims.y, rk-s); // 01_0 = 0
      }
      else if ( view_plane == BrainView::SAGITTAL ){
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

    // for each voxel at current slice depth, create indices in face set
    SoMFInt32& coords = ifs->coordIndex;
    SoMFInt32& norms = ifs->normalIndex;
    SoMFInt32& mats = ifs->materialIndex;
    int nc_per_idx = 5;          // number of coords per index
    int nn_per_idx = 1;          // number of norms per index
    int nm_per_idx = 1;           // number of mats per index
    coords.setNum(voxels.size() * nc_per_idx);
    norms.setNum(voxels.size() * nn_per_idx);
    mats.setNum(voxels.size() * nm_per_idx);

    int32_t* coords_dat = coords.startEditing();
    int32_t* norms_dat  = norms.startEditing();
    int32_t* mats_dat   = mats.startEditing();

    int cidx = 0;
    int nidx = 0;
    int midx = 0;
    int uidx = 0;
    foreach (Voxel* v, voxels) {
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
  BrainView* bv = this->bv(); //cache
  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());

  BrainView::AnatomicalPlane view_plane = bv->ViewPlane();

  // iterate over all slices, and for each voxel at that slice depth, 
  // determine the face color/transparency from unit value
  FloatTDCoord voxel_coord;
  for (int s=0; s<bv->MaxSlices(); s++) {
    QList<Voxel*> voxels = depth_map_.values(s);
    if (0 == voxels.size()) continue;
    
    SoVertexProperty* vp  = node.voxel_vrtx_prop_array[s];
    SoMFUInt32& color = vp->orderedRGBA;

    bool makeTransparent(false);
    float val, sc_val,alpha,trans = bv->view_params.unit_trans;
    int idx(0);
    T3Color col;
    Voxel* v(0);

    uint32_t* color_dat = color.startEditing();
   
    foreach (v,voxels) {
      voxel_coord = voxel_map_.value(v);
      if ( view_plane == BrainView::AXIAL) {
        if ( (voxel_coord.z > bv->SliceEnd()) || (voxel_coord.z < bv->SliceStart()) ) {
          makeTransparent = true;
        }
      }
      else if ( view_plane == BrainView::SAGITTAL ) {
        if ( (voxel_coord.x > bv->SliceEnd()) || (voxel_coord.x < bv->SliceStart()) ) {
          makeTransparent = true;
        }
      }
      else { //CORONAL
        if ( (voxel_coord.y > bv->SliceEnd()) || (voxel_coord.y < bv->SliceStart()) ) {
          makeTransparent = true;
        }
      }
      Unit* u = (Unit*)((taOBase*)v->owner)->owner;
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


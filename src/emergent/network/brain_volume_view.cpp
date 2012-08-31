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

// THIS IS THE GOOD ONE
#include "brain_view.h"
#include "t3brain_node.h"
#include "brain_volume_view.h"
#include "brainstru.h"
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
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoText2.h>
#include <Inventor/nodes/SoShapeHints.h>

#include <QList>

namespace { // anonymous
  // Set to 0 to turn off debug output.
  // Set to 1 to enable debug output and test brain areas painting.
  const int DEBUG_LEVEL = 0;
}

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
  m_brain_data = NULL;
  m_atlas_data = NULL;
  m_units_depth_map.clear();
  m_voxel_map.clear();
  m_uvd_bases_map.clear();
  m_atlas_depth_map.clear();
  m_atlasColors.clear();

}

void BrainVolumeView::InitLinks()
{
  inherited::InitLinks();
}

void BrainVolumeView::CutLinks()
{
  delete m_atlas_data;
  m_atlas_data = 0;
  delete m_brain_data;
  m_brain_data = 0;

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
  if (bv->unit_disp_idx < 0) return val;

  base = uvd_bases.SafeEl(m_uvd_bases_map.value(u), bv->unit_disp_idx);
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
  if (m_uvd_bases_map.size() == 0) return; //we don't have a list of units yet

  foreach (const Unit* u, m_uvd_bases_map.keys()) {
    uvd_bases.Set(disp_md->GetOff(u), m_uvd_bases_map.value(u), midx);
  }
}

void BrainVolumeView::UpdateAutoScale(bool& updated) {}

bool BrainVolumeView::ColorBrain()
{
  // this is a convenience method that encapsulates
  // dependency checking on externals

  BrainView* bv = this->bv(); //cache
  if(!bv) return false;

  if (bv->net()->brain_atlas.ptr() && bv->ColorBrain()) {
    return true;
  }
  else {
    return false;
  }
}

void BrainVolumeView::Render_pre()
{

  // Create structural brain data object (high-res brain data for rendering)
  m_brain_data = new NiftiReader(bv()->DataName());

  if (m_brain_data->IsValid()) {
    setNode(new T3BrainNode(this));
    if (ColorBrain()) {
      // Get the atlas colors and change those which DO NOT MATCH
      // the regexp areas to white (effectively not coloring those areas)
      BrainAtlas& atlas = bv()->net()->brain_atlas.ptr()->Atlas();
      m_atlasColors.clear();
      m_atlasColors = BrainAtlasUtils::Colors(atlas);
      const QColor WHITE("#ffffff");
      QSet<int> indexes = BrainAtlasUtils::Indexes(atlas, bv()->ColorBrainRegexp());
      for (int i=0; i<m_atlasColors.size();++i) {
        if (indexes.contains(i)) {
          continue;
        }
        else {
          m_atlasColors[i] = WHITE;
        }
      }
    }

    RenderBrain();
  }

  inherited::Render_pre();
}

void BrainVolumeView::RenderBrain()
{
  BrainView* bv = this->bv(); //cache
  if(!bv) return;

  // If colorizing the structural brain, we need access
  // to the atlas image data...
  taBrainAtlas* atlas(bv->net()->brain_atlas.ptr());
  if (ColorBrain()) {
    m_atlas_data = new NiftiReader(atlas->image_filepath);
    if (!m_atlas_data->IsValid()) {
      delete m_atlas_data;
      m_atlas_data = 0;
      return;
    }
  }

  //Creates the nodes that render the reference brain
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());

  // Get the dimensions of the brain
  TDCoord dims(m_brain_data->XyzDimensions());
  float max_dim = taMath_float::max(dims.z, taMath_float::max(dims.x, dims.y));

  SoSeparator* ss = node.shapeSeparator();
  node.brain_group = new SoSeparator();

  SoTransform* global_xform = new SoTransform;
  node.brain_group->addChild(global_xform);
  global_xform->translation.setValue(SbVec3f(0.0,0.0,0.0));
  global_xform->scaleFactor.setValue(SbVec3f(1/max_dim, 1/max_dim, 1/max_dim)); //normalize by max brain dimension

  // Create reference widget
  SoSeparator* ref_widg = new SoSeparator;
  CreateReferenceWidget(ref_widg, bv->ViewPlane(), max_dim, 0.2f);
  node.brain_group->addChild(ref_widg);

  // set the "origin"
  SoTransform* b0 = new SoTransform;
  int d1(0),d2(0),d3(0);
  if (bv->ViewPlane() == BrainView::CORONAL) {
    d1 = dims.x;
    d2 = dims.z;
    d3 = dims.y;
    b0->translation.setValue(SbVec3f(dims.x/2.0f, dims.z/2.0f, -dims.y));
  }
  else if (bv->ViewPlane() == BrainView::SAGITTAL) {
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
  node.atlas_face_set_array = new SoIndexedFaceSet* [d3];
  node.voxel_vrtx_prop_array = new SoVertexProperty* [d3];
  node.atlas_vrtx_prop_array = new SoVertexProperty* [d3];

  //if revert to CopyPolicy::Copy then do allocation out of loop and reuse
  //unsigned char* tex = new unsigned char[d1*d2*2];
  unsigned char* tex(0);
  float transparency(bv->SliceTransparencyXformed());
  for (int i=0; i<d3; i++) {

    if (ColorBrain()) {
      // texture is four bytes/pixel: RGBA
      tex = static_cast<unsigned char*>(malloc(d1*d2*4));
      SliceAsColorTexture(bv->ViewPlane(), 1+i, tex, m_brain_data, m_atlas_data);
    }
    else {
      // texture is two bytes/pixel (intensity,alpha)
      tex = static_cast<unsigned char*>(malloc(d1*d2*2));
      SliceAsTexture(bv->ViewPlane(), 1+i, tex, m_brain_data);
    }

    node.brain_tex_mat_array[i] = new SoMaterial;
    node.brain_tex_mat_array[i]->transparency = transparency;
    node.brain_tex_mat_array[i]->diffuseColor.setValue(SbVec3f(0.0f,0.0f,0.0f));
    node.brain_tex_mat_array[i]->emissiveColor.setValue(SbVec3f(1.0f,1.0f,1.0f));

    node.atlas_face_set_array[i] = new SoIndexedFaceSet;
    node.atlas_vrtx_prop_array[i] = new SoVertexProperty;
    node.atlas_face_set_array[i]->vertexProperty.setValue(node.atlas_vrtx_prop_array[i]);

    node.voxel_face_set_array[i] = new SoIndexedFaceSet;
    node.voxel_vrtx_prop_array[i] = new SoVertexProperty;
    node.voxel_face_set_array[i]->vertexProperty.setValue(node.voxel_vrtx_prop_array[i]);

    SoSeparator* b = new SoSeparator;
    // shape hint needed for sorted blend rendering (when used)
    SoShapeHints* sh = new SoShapeHints;
    sh->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    sh->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;

    SoTexture2* texture = new SoTexture2;
    // NOTE: Using a CopyPolicy may cause intermittent crashes on Windows
    // http://doc.coin3d.org/Coin-3.1/classSoSFImage.html
    if (ColorBrain()) {
      texture->image.setValue(SbVec2s(d1, d2),4,tex,SoSFImage::NO_COPY_AND_FREE);
    }
    else {
      texture->image.setValue(SbVec2s(d1, d2),2,tex,SoSFImage::NO_COPY_AND_FREE);
    }
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
    // THE FOLLOWING CAUSES CRASH ON CENTOS5 UNDER VBOX AND SOME
    // REMOTE INSTANCES (WINXP) BUT I CAN'T EXPLAIN WHY...
    //fs->numVertices.set1Value(0,4);

    b->addChild(sh);
    b->addChild(node.brain_tex_mat_array[i]);
    b->addChild(texture);
    b->addChild(tcoords);
    b->addChild(coords);
    b->addChild(fs);

    SoSeparator* s = new SoSeparator; //necessary to separate texmap
    s->addChild(node.atlas_face_set_array[i]);
    s->addChild(node.voxel_face_set_array[i]);

    node.brain_group->addChild(b);
    node.brain_group->addChild(s);

    node.spacing_xform = new SoTransform;
    node.brain_group->addChild(node.spacing_xform);
    node.spacing_xform->translation.setValue(SbVec3f(0.0,0.0,bv->SliceSpacing()));
  }
  // NOTE: If revert to CopyPolicy::Copy (see above) then delete allocated tex
  // delete [] tex;

  ss->addChild(node.brain_group);

  // done with atlas data
  delete m_atlas_data;
  m_atlas_data = 0;
}

void BrainVolumeView::SliceAsTexture(BrainView::AnatomicalPlane p, int index, unsigned char* data, NiftiReader* brain_data)
{
  TDCoord size(brain_data->XyzDimensions());
  int plane_size(0);
  if (p == BrainView::AXIAL) {
    plane_size = size.x*size.y;
  }
  else if (p == BrainView::CORONAL) {
    plane_size = size.x*size.z;
  }
  else { //BrainView::SAGITTAL
    plane_size = size.y*size.z;
  }
  unsigned short* s = new unsigned short[plane_size];
  brain_data->Slice(static_cast<NiftiReader::AnatomicalPlane>(p), index, s);

  // scale the data by the monitor calibrations
  // and set paired transpaency/alpha bytes
  int pmax((int)brain_data->CalMax());
  int pmin((int)brain_data->CalMin());
  int prange((int)brain_data->CalRange());
  int j(0);
  unsigned int pixel(0);
  unsigned char pixel8(0);
  for (int i=0;i<plane_size; i++) {

    // skip zero-valued pixels to speed things up
    if (s[i] == 0) {
      data[j] = 0;
      data[j+1] = 0; // zero-valued pixels are fully transparent
      continue;
    }

    if (s[i] > pmax) {
      pixel = pmax;
    }
    else if (s[i] < pmin) {
      pixel = pmin;
    }
    else {
      pixel = s[i];
    }
    pixel8 = static_cast<unsigned char>(((pixel-pmin) * 255) / prange);
    j = i * 2; // data is 2 bytes/pixel: intensity,alpha

    data[j] = pixel8;
    data[j+1] = 255; // non-zero pixels are fully opaque
  }

  delete [] s;
  s = 0;
}

void BrainVolumeView::SliceAsColorTexture(BrainView::AnatomicalPlane p, int index, unsigned char* data, NiftiReader* brain_data, NiftiReader* atlas_data)
{
  TDCoord size(brain_data->XyzDimensions());
  int plane_size(0), width(0), height(0), depth(0);
  if (p == BrainView::AXIAL) {
    plane_size = size.x*size.y;
    width=size.x;
    height=size.y;
    depth = size.z;
  }
  else if (p == BrainView::CORONAL) {
    plane_size = size.x*size.z;
    width=size.x;
    height=size.z;
    depth = size.y;
  }
  else { //BrainView::SAGITTAL
    plane_size = size.y*size.z;
    width=size.y;
    height=size.z;
    depth = size.x;
  }
  unsigned short* s = new unsigned short[plane_size];
  brain_data->Slice(static_cast<NiftiReader::AnatomicalPlane>(p), index, s);

  // Extract the same plane of data from the atlas, to use to get label indices
  // for each pixel.
  //
  // To summarize, for each I,J,K brain pixel, we need to get the corresponding I,J,K
  // atlas pixel's label index to use as the index into the color LUT, so we can
  // color the pixel
  unsigned short* ci = new unsigned short[plane_size];
  atlas_data->Slice(static_cast<NiftiReader::AnatomicalPlane>(p), index, ci);

  // scale the data by the monitor calibrations
  // and set paired transpaency/alpha bytes
  int pmax((int)brain_data->CalMax());
  int pmin((int)brain_data->CalMin());
  int prange((int)brain_data->CalRange());
  int j(0);
  unsigned int pixel(0);
  unsigned char pixel8(0);

  const QColor WHITE(255,255,255);
  for (int h=0; h<height; h++) {
    for (int w=0; w<width; w++) {
      int i=(width*h) + w;

      // skip zero-valued pixels to speed things up
      if (s[i] == 0) {
        data[j] = 0;
        data[j+1] = 0;
        data[j+2] = 0;
        data[j+3] = 0; // zero-valued pixels are fully transparent
        continue;
      }

      if (s[i] > pmax) {
        pixel = pmax;
      }
      else if (s[i] < pmin) {
        pixel = pmin;
      }
      else {
        pixel = s[i];
      }
      pixel8 = static_cast<unsigned char>(((pixel-pmin) * 255) / prange);
      j = i * 4; // data is 4 bytes/pixel: RGBA

      T3Color color(1.0f,1.0f,1.0f);
      QColor col = m_atlasColors.value(ci[i],WHITE); //default to white if not found
      color.r = col.redF();
      color.g = col.greenF();
      color.b = col.blueF();

      if(col != WHITE) {
        data[j] = (unsigned char)(255 * color.r);
        data[j+1] = (unsigned char)(255 * color.g);
        data[j+2] = (unsigned char)(255 * color.b);
        data[j+3] = pixel8;     // colors modulate as transparency
      }
      else {                    // white modultes as value
        data[j] = (unsigned char)(pixel8 * color.r);
        data[j+1] = (unsigned char)(pixel8 * color.g);
        data[j+2] = (unsigned char)(pixel8 * color.b);
        data[j+3] = 128;        // white is half-transparent
      }
    }
  }

  delete [] s;
  s = 0;
  delete [] ci;
  ci = 0;
}

void BrainVolumeView::CreateReferenceWidget(SoSeparator* widg, BrainView::AnatomicalPlane p, float max_dim, float scale)
{
  // This creates a three (3) line, 3d widget which serves as an reference for
  // brain volume view by indicating which axes correspond to which anatomical
  // directions (Left-Right, Anterior-Posterior, Superior-Inferior)

  float sc = scale;
  float len = max_dim*sc; //length of lines
  float pos = len + (len*sc); // position of letter at end of line (with padding)

  SoFont* font = new SoFont;
  font->name.setValue("Times-Roman");
  font->size.setValue(12);
  widg->addChild(font);

  SoMaterialBinding* lines_mat_bind = new SoMaterialBinding;
  lines_mat_bind->value = SoMaterialBinding::PER_PART;
  widg->addChild(lines_mat_bind);

  // Establish coordinates for three reference lines
  SoCoordinate3* lines_coords = new SoCoordinate3;
  lines_coords->point.set1Value(0, SbVec3f(0,0,0));  // draw line in screen X direction
  lines_coords->point.set1Value(1, SbVec3f(len,0,0)); //
  lines_coords->point.set1Value(2, SbVec3f(0,0,0));  // draw line in screen Y direction
  lines_coords->point.set1Value(3, SbVec3f(0,len,0)); //
  lines_coords->point.set1Value(4, SbVec3f(0,0,0));  // draw line in screen Z direction
  lines_coords->point.set1Value(5, SbVec3f(0,0,-len)); //


  // Define useful color variables
  SbColor red(1,0,0), green(0,1,0), blue(0,0,1);

  // For setting the line colors, we choose to always use:
  //  RED:      for Left-Right reference line
  //  GREEN:    for Anterior-Posterior line
  //  BLUE:     for Superior-Inferior line
  //
  // Because the orientation of these lines will change
  // depending on the user's view (view-plane) we must
  // set the colors in the proper order to match the order
  // in which we draw the lines (code above)
  //
  SoMaterial* lines_mat = new SoMaterial;
  if (p == BrainView::AXIAL) {
    lines_mat->diffuseColor.set1Value(0,red);   // L-R line
    lines_mat->diffuseColor.set1Value(1,green); // P-A line
    lines_mat->diffuseColor.set1Value(2,blue);  // S-I line

  }
  else if (p == BrainView::SAGITTAL) {
    lines_mat->diffuseColor.set1Value(0,green); // P-A line
    lines_mat->diffuseColor.set1Value(1,blue);  // S-I line
    lines_mat->diffuseColor.set1Value(2,red);   // L-R line
  }
  else { // BrainView::CORONAL
    lines_mat->diffuseColor.set1Value(0,red);   // L-R line
    lines_mat->diffuseColor.set1Value(1,blue);  // S-I line
    lines_mat->diffuseColor.set1Value(2,green); // A-P line
  }
  widg->addChild(lines_mat);
  widg->addChild(lines_coords);

  // Create the lines
  SoLineSet* lines = new SoLineSet;
  lines->numVertices.set1Value(0, 2);
  lines->numVertices.set1Value(1, 2);
  lines->numVertices.set1Value(2, 2);
  widg->addChild(lines);

  // Add letters to end of lines indicating:
  //  (L)eft or (R)ight,
  //  (A)nterior or (P)osterior
  //  (S)uperior or (I)nferior
  //
  // Note that the direction of translation will
  // vary based on current view-plane
  //

  // Add Left-Right text
  SoSeparator* lr_sep = new SoSeparator;
  SoMaterial* lr_mat = new SoMaterial;
  SoTransform* lr_xform = new SoTransform;
  SoText2* lr_text = new SoText2;
  lr_mat->diffuseColor.set1Value(0, red);
  if (p == BrainView::AXIAL) {
    lr_xform->translation.setValue(SbVec3f(pos,0,0));
    lr_text->string = "R";
  }
  else if (p == BrainView::SAGITTAL) {
    lr_xform->translation.setValue(SbVec3f(0,0,-pos));
    lr_text->string = "L";
  }
  else { // BrainView::CORONAL
    lr_xform->translation.setValue(SbVec3f(pos,0,0));
    lr_text->string = "R";
  }
  lr_sep->addChild(lr_mat);
  lr_sep->addChild(lr_xform);
  lr_sep->addChild(lr_text);

  // Add Superior-Inferior text
  SoSeparator* si_sep = new SoSeparator;
  SoMaterial* si_mat = new SoMaterial;
  SoTransform* si_xform = new SoTransform;
  SoText2* si_text = new SoText2;
  si_mat->diffuseColor.set1Value(0, blue);
  if (p == BrainView::AXIAL) {
    si_xform->translation.setValue(SbVec3f(0,0,-pos));
    si_text->string = "I";
  }
  else if (p == BrainView::SAGITTAL) {
    si_xform->translation.setValue(SbVec3f(0,pos,0));
    si_text->string = "S";
  }
  else { // BrainView::CORONAL
    si_xform->translation.setValue(SbVec3f(0,pos,0));
    si_text->string = "S";
  }
  si_sep->addChild(si_mat);
  si_sep->addChild(si_xform);
  si_sep->addChild(si_text);

  // Add Anterior-Posterior text
  SoSeparator* ap_sep = new SoSeparator;
  SoMaterial* ap_mat = new SoMaterial;
  SoTransform* ap_xform = new SoTransform;
  SoText2* ap_text = new SoText2;
  ap_mat->diffuseColor.set1Value(0, green);
  if (p == BrainView::AXIAL) {
    ap_xform->translation.setValue(SbVec3f(0,pos,0));
    ap_text->string = "A";
  }
  else if (p == BrainView::SAGITTAL) {
    ap_xform->translation.setValue(SbVec3f(pos,0,0));
    ap_text->string = "A";
  }
  else { // BrainView::CORONAL
    ap_xform->translation.setValue(SbVec3f(0,0,-pos));
    ap_text->string = "P";
  }
  ap_sep->addChild(ap_mat);
  ap_sep->addChild(ap_xform);
  ap_sep->addChild(ap_text);

  // add text node groups
  widg->addChild(lr_sep);
  widg->addChild(si_sep);
  widg->addChild(ap_sep);
}

void BrainVolumeView::Render_impl()
{
  inherited::Render_impl();
}

void BrainVolumeView::DoActionChildren_impl(DataViewAction acts)
{
  if (acts & RENDER_IMPL) {
    acts = (DataViewAction)(acts & ~RENDER_IMPL); // note: only supposed to be one, but don't assume
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
  BrainView* bv = this->bv(); //cache
  if(!bv) return;

  // Create atlas face sets if requested
  if (bv->show_atlas && bv->net()->brain_atlas.ptr() != NULL) {
    // use the first matching label's color for face sets (or white if not found)
    BrainAtlas& atlas = bv->net()->brain_atlas->Atlas();
    QSet<int> match_idxs = BrainAtlasUtils::Indexes(atlas, bv->brain_area_regexp);
    int color_idx(*match_idxs.constBegin());
    QList<QColor> colors = BrainAtlasUtils::Colors(atlas);
    const QColor WHITE(255,255,255);
    T3Color color(colors.value(color_idx,WHITE));
    CreateAtlasFaceSets(bv->brain_area_regexp, color);
  }

  // create unit/voxel face sets & set values
  CreateFaceSets();
  UpdateSlices(); // will call UpdateUnitValues_blocks ...
}

void BrainVolumeView::CreateFaceSets()
{
  // @TODO - refactor this and CreateAtlasFaceSets remove redundant code

  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  T3BrainNode& node = *(this->node_so());

  BrainView::AnatomicalPlane view_plane = bv->ViewPlane();
  FloatTDCoord dims(bv->Dimensions());
  FloatTDCoord half_dims;
  half_dims.SetXYZ(dims.x/2.0f, dims.y/2.0f, dims.z/2.0f);

  // clear the old maps
  m_units_depth_map.clear();
  m_voxel_map.clear();
  m_uvd_bases_map.clear();

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

        FloatTDCoord mniCoord(v->coord);
        FloatTDCoord ijkCoord(m_brain_data->XyzToIjk(mniCoord));
        if ((view_plane == BrainView::AXIAL) || (view_plane == BrainView::SAGITTAL)) {
          // reverse x coordinates:
          // we must do this for these views since we don't render the brain in neurological
          // convention (which would be looking from the feet in Axial) - thus our X and
          // how X in the data is stored are reversed
            ijkCoord.x = taMath_float::fabs(ijkCoord.x - (dims.x - 1));
        }

        if (view_plane == BrainView::AXIAL) {
          m_units_depth_map.insert((unsigned int)ijkCoord.z, v);
        }
        else if (view_plane == BrainView::SAGITTAL) {
          m_units_depth_map.insert((unsigned int)ijkCoord.x, v);
        }
        else { //CORONAL
          m_units_depth_map.insert((unsigned int)ijkCoord.y, v);
        }
        m_voxel_map.insert(v, ijkCoord);
        m_uvd_bases_map.insert(u,i); //map unit* to index, so we can index into uvd_bases
      }
      i++;
    }
  }

  // iterate over all slices, and for each voxel at that slice depth, create face in face set
  FloatTDCoord voxel_coord;
  for (int s=0; s<bv->MaxSlices(); s++) {
    QList<Voxel*> voxels = m_units_depth_map.values(s);
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

    // when rendering using blend transparency mode, we need to keep face sets "above"
    // the texture maps, otherwise rendering gets wonky due to ovelapping triangles
    // we'll add a shim value to make sure faces are "above" texture maps
    const float shim  = 0.05f;

    // for each voxel at current slice depth, create face vertices in face set
    foreach (Voxel* v, voxels) {
      voxel_coord = m_voxel_map.value(v);
      float ri      = voxel_coord.x;
      float rj      = voxel_coord.y;
      float rk      = voxel_coord.z;
      float half_voxel  = v->size*0.5f;

      //@TODO add voxel size in 3rd dimension (true 3D voxel, not a sheet)?
      // would need to do this by adding more faces at different depth levels
      // i.e. stack of sheets
      if (view_plane == BrainView::AXIAL) {
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 00_0 = 0
                                     rj-half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 10_0 = 0
                                     rj-half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 11_0 = 0
                                     rj+half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 01_0 = 0
                                     rj+half_voxel-half_dims.y,
                                     rk-s+shim);
      }
      else if (view_plane == BrainView::SAGITTAL) {
        vertex_dat[v_idx++].setValue(rj-half_voxel-half_dims.y, // 00_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj+half_voxel-half_dims.y, // 10_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj+half_voxel-half_dims.y, // 11_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj-half_voxel-half_dims.y, // 01_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     ri-s+shim);
      }
      else { //view_plane == BrainView::CORONAL
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 00_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 10_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 11_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 01_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     rj-s+shim);
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

void BrainVolumeView::CreateAtlasFaceSets(String brain_area, T3Color area_color)
{
  // @TODO - refactor this and CreateFaceSets remove redundant code

  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  BrainView* bv = this->bv(); //cache
  if(!bv) return;
  T3BrainNode& node = *(this->node_so());

  BrainView::AnatomicalPlane view_plane = bv->ViewPlane();
  FloatTDCoord dims(bv->Dimensions());
  FloatTDCoord half_dims;
  half_dims.SetXYZ(dims.x/2.0f, dims.y/2.0f, dims.z/2.0f);

  // return if we don't have a network atlas chosen
  if (bv->net()->brain_atlas.ptr() == NULL) return;

  BrainAtlas& atlas = bv->net()->brain_atlas.ptr()->Atlas();

  // Get and assign voxel coordinates and sizes to all units.
  QList<FloatTDCoord> voxels = atlas.VoxelCoordinates(brain_area);

  // clear the old map
  m_atlas_depth_map.clear();

  if(voxels.size() == 0) return;

  // create the depth map
  foreach (FloatTDCoord v, voxels) {
    FloatTDCoord ijkCoord(m_brain_data->XyzToIjk(v));
    if ((view_plane == BrainView::AXIAL) || (view_plane == BrainView::SAGITTAL)) {
      // reverse x coordinates:
      // we must do this for these views since we don't render the brain in neurological
      // convention (which would be looking from the feet in Axial) - thus our X and
      // how X in the data is stored are reversed
       ijkCoord.x = taMath_float::fabs(ijkCoord.x - (dims.x - 1));
        if (DEBUG_LEVEL) {
          std::ostringstream os;
          if (ijkCoord.x > 90) {
            os << "MNI_X: "<< v.x << ", MNI_Y: " << v.y << ", MNI_Z: " << v.z << std::endl;
            taMisc::Warning(os.str().c_str());
            os << "IJK_X: "<< ijkCoord.x << ", IJK_Y: " << ijkCoord.y << ", IJK_Z: " << ijkCoord.z << std::endl;
            taMisc::Warning(os.str().c_str());
          }
        }
    }

    if (view_plane == BrainView::AXIAL) {
      m_atlas_depth_map.insert((unsigned int)ijkCoord.z, ijkCoord);
    }
    else if (view_plane == BrainView::SAGITTAL) {
      m_atlas_depth_map.insert((unsigned int)ijkCoord.x, ijkCoord);
    }
    else { //CORONAL
      m_atlas_depth_map.insert((unsigned int)ijkCoord.y, ijkCoord);
    }
  }

  if(m_atlas_depth_map.empty()) return;
  int maxslice = bv->MaxSlices();

  // iterate over all slices
  // for each voxel at a slice depth, create face in the face set
  for (int s=0; s<maxslice; s++) {
    QList<FloatTDCoord> voxels = m_atlas_depth_map.values(s);

    SoIndexedFaceSet* ifs = node.atlas_face_set_array[s];
    SoVertexProperty* vtx_prop = node.atlas_vrtx_prop_array[s];
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

    // we arbitrarily define the size of atlas faces as 1.0,
    // so the extent, or half-size is 0.5f
    const float half_voxel  = 0.5f;

    // when rendering using blend transparency mode, we need to keep face sets "above"
    // the texture maps, otherwise rendering gets wonky due to ovelapping triangles
    // we'll add a shim value to make sure faces are "above" texture maps
    const float shim  = 0.05f;

    // for each voxel at current slice depth, create face vertices in face set
    foreach (FloatTDCoord voxel_coord, voxels) {
      float ri      = voxel_coord.x;
      float rj      = voxel_coord.y;
      float rk      = voxel_coord.z;

      //@TODO add voxel size in 3rd dimension (true 3D voxel, not a sheet)?
      // would need to do this by adding more faces at different depth levels
      // i.e. stack of sheets
      if (view_plane == BrainView::AXIAL) {
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 00_0 = 0
                                     rj-half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 10_0 = 0
                                     rj-half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 11_0 = 0
                                     rj+half_voxel-half_dims.y,
                                     rk-s+shim);
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 01_0 = 0
                                     rj+half_voxel-half_dims.y,
                                     rk-s+shim);
      }
      else if (view_plane == BrainView::SAGITTAL) {
        vertex_dat[v_idx++].setValue(rj-half_voxel-half_dims.y, // 00_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj+half_voxel-half_dims.y, // 10_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj+half_voxel-half_dims.y, // 11_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     ri-s+shim);
        vertex_dat[v_idx++].setValue(rj-half_voxel-half_dims.y, // 01_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     ri-s+shim);
      }
      else { //CORONAL
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 00_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 10_0 = 0
                                     rk-half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri+half_voxel-half_dims.x, // 11_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     rj-s+shim);
        vertex_dat[v_idx++].setValue(ri-half_voxel-half_dims.x, // 01_0 = 0
                                     rk+half_voxel-half_dims.z,
                                     rj-s+shim);
      }
      color_dat[c_idx++] = T3Color::makePackedRGBA(area_color.r, area_color.g, area_color.b, 0.8f);
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
    foreach (FloatTDCoord v, voxels) {
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
    QList<Voxel*> voxels = m_units_depth_map.values(i);
    if (0 == voxels.size()) continue;

    if (((bv->SliceStart() - 1) <= i) && (i <= (bv->SliceEnd() - 1))) {
      node.brain_tex_mat_array[i]->transparency = transparency;
    }
    else {
      node.brain_tex_mat_array[i]->transparency = 1.0f;
    }
  }
  UpdateUnitValues_blocks();
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
    QList<Voxel*> voxels = m_units_depth_map.values(s);
    if (0 == voxels.size()) continue;

    SoVertexProperty* vp  = node.voxel_vrtx_prop_array[s];
    SoMFUInt32& color = vp->orderedRGBA;

    bool make_transparent = false;
    float val, sc_val,alpha,trans = bv->view_params.unit_trans;
    int idx(0);
    T3Color col;
    Voxel* v(0);

    uint32_t* color_dat = color.startEditing();

    foreach (v,voxels) {
      voxel_coord = m_voxel_map.value(v);
      if (view_plane == BrainView::AXIAL) {
        if ((voxel_coord.z > bv->SliceEnd()) || (voxel_coord.z < bv->SliceStart())) {
          make_transparent = true;
        }
      }
      else if (view_plane == BrainView::SAGITTAL) {
        if ((voxel_coord.x > bv->SliceEnd()) || (voxel_coord.x < bv->SliceStart())) {
          make_transparent = true;
        }
      }
      else { //CORONAL
        if ((voxel_coord.y > bv->SliceEnd()) || (voxel_coord.y < bv->SliceStart())) {
          make_transparent = true;
        }
      }
      Unit* u = (Unit*)((taOBase*)v->owner)->owner;
      bv->GetUnitDisplayVals(this, u, val, col, sc_val);

      if (make_transparent) {
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

void BrainVolumeView::UpdateAtlasFaceValues(float alpha)
{
  // this method will control the transparency of the atlas face sets...and
  // should be called with a transparency value sepaarte and unique from that
  // which controls the brain itself...

  BrainView* bv = this->bv(); //cache
  Network* net = this->net(); //cache
  if (NULL == net) return;
  if (NULL == this->node_so()) return;
  T3BrainNode& node = *(this->node_so());

  BrainView::AnatomicalPlane view_plane = bv->ViewPlane();

  // iterate over all slices, and for each voxel at that slice depth,
  // determine the face color/transparency from unit value
  for (int s=0; s<bv->MaxSlices(); s++) {
    QList<FloatTDCoord> voxels = m_atlas_depth_map.values(s);
    if (0 == voxels.size()) continue;

    SoVertexProperty* vp  = node.atlas_vrtx_prop_array[s];
    SoMFUInt32& color = vp->orderedRGBA;

    int idx(0);
    bool make_transparent = false;
    uint32_t* color_dat = color.startEditing();
    foreach (FloatTDCoord voxel_coord,voxels) {
      if (view_plane == BrainView::AXIAL) {
        if ((voxel_coord.z > bv->SliceEnd()) || (voxel_coord.z < bv->SliceStart())) {
          make_transparent = true;
        }
      }
      else if (view_plane == BrainView::SAGITTAL) {
        if ((voxel_coord.x > bv->SliceEnd()) || (voxel_coord.x < bv->SliceStart())) {
          make_transparent = true;
        }
      }
      else { //CORONAL
        if ((voxel_coord.y > bv->SliceEnd()) || (voxel_coord.y < bv->SliceStart())) {
          make_transparent = true;
        }
      }

      if (make_transparent) {
        color_dat[idx] = (color_dat[idx] & 0xfff0) | 0x0000;
      }
      else {
        color_dat[idx] = (color_dat[idx] & 0xfff0) | 0x000f;
      }
      idx++;
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
  delete m_brain_data;
  m_brain_data = 0;

  inherited::Reset_impl();
  uvd_bases.Reset();
}

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

#include "taBrainAtlas.h"
#include <BrainAtlasInfo>
#include <BrainAtlasFactory>
#include <BrainAtlas>
#include <BrainAtlasUtils>
#include <BrainAtlasRegexpPopulator>
#include <iBrainViewEditDialog>

#include <taColor_List>
#include <String_Matrix>
#include <int_Matrix>
#include <float_Matrix>
#include <ColorScale>
#include <taVector3f>

#include <QColor>


taBrainAtlas::taBrainAtlas(const BrainAtlasInfo& info) : taNBase()
  , filepath(info.atlas_filename)
  , description(info.description)
  , image_filepath(info.image_filename)
  , m_have_atlas_instance(false)
  , m_atlas(NULL)
{
  name = info.name;
}

void taBrainAtlas::Initialize()
{  
  m_have_atlas_instance = false;
  m_atlas = NULL;
}

BrainAtlas& taBrainAtlas::Atlas()
{
  // if we don't already have an BrainAtlas
  // instance we're proxying, create one...
  if (m_have_atlas_instance == false) {
    m_atlas = BrainAtlasFactory::CreateInstance(filepath);
    m_have_atlas_instance = true;
  }
  return *m_atlas;
}

void taBrainAtlas::Destroy()
{
  if (m_have_atlas_instance) { 
    delete m_atlas; 
    m_atlas = NULL;
    m_have_atlas_instance = false;
  }
}

String taBrainAtlas::Label(int index) {
  return BrainAtlasUtils::Label(Atlas(), index);
}

int taBrainAtlas::Index(const String& label) {  
  return BrainAtlasUtils::Index(Atlas(), label);
}

iColor taBrainAtlas::ColorForIndex(int index) {  
  return BrainAtlasUtils::Color(Atlas(), index);
}

iColor taBrainAtlas::ColorForLabel(const String& label) {  
  return BrainAtlasUtils::Color(Atlas(), Index(label));
}

void taBrainAtlas::Colors(taColor_List& colors, const String& labels_regexp) {
  QList<QColor> qclr = BrainAtlasUtils::Colors(Atlas(), (QString)labels_regexp.chars());
  colors.Reset();
  for(int i=0; i< qclr.size(); i++) {
    taColor* tac = (taColor*)colors.New(1);
    tac->setColor((iColor)qclr[i]);
  }
}

void taBrainAtlas::Labels(String_Matrix& labels, const String& labels_regexp) {
  QStringList qstr = BrainAtlasUtils::Labels(Atlas(), (QString)labels_regexp.chars());
  labels.SetGeom(1, qstr.size());
  for(int i=0; i< qstr.size(); i++) {
    labels.Set(qstr[i], i);
  }
}

void taBrainAtlas::Indexes(int_Matrix& indexes, const String& labels_regexp) {
  QList<int> qidx = BrainAtlasUtils::IndexList(Atlas(), (QString)labels_regexp.chars());
  indexes.SetGeom(1, qidx.size());
  for(int i=0; i< qidx.size(); i++) {
    indexes.Set(qidx.at(i), i);
  }
}

void taBrainAtlas::VoxelCoordinates(float_Matrix& voxels, const String& label_regexp) {
  QList<taVector3f> qcrd = Atlas().VoxelCoordinates((QString)label_regexp.chars());
  voxels.SetGeom(2, 3, qcrd.size());
  for(int i=0; i< qcrd.size(); i++) {
    taVector3f td = qcrd[i];
    voxels.Set(td.x, 0, i);
    voxels.Set(td.y, 1, i);
    voxels.Set(td.z, 2, i);
  }
}

void taBrainAtlas::SetColor(const String& labels_regexp, iColor color) {
  QList<int> qidx = BrainAtlasUtils::IndexList(Atlas(), (QString)labels_regexp.chars());
  QColor clr = (QColor)color;
  for(int i=0; i< qidx.size(); i++) {
    BrainAtlasUtils::SetColor(Atlas(), clr, qidx.at(i));
  }
}

void taBrainAtlas::SetColorString(const String& labels_regexp, const String& color) {
  QList<int> qidx = BrainAtlasUtils::IndexList(Atlas(), (QString)labels_regexp.chars());
  QColor clr((QString)color.chars());
  for(int i=0; i< qidx.size(); i++) {
    BrainAtlasUtils::SetColor(Atlas(), clr, qidx.at(i));
  }
}

void taBrainAtlas::SetColorValue(const String& labels_regexp, float val,
				   ColorScale* color_scale) {
  QList<int> qidx = BrainAtlasUtils::IndexList(Atlas(), (QString)labels_regexp.chars());
  float sc_val;
  QColor clr(color_scale->GetColor(val, sc_val));
  for(int i=0; i< qidx.size(); i++) {
    BrainAtlasUtils::SetColor(Atlas(), clr, qidx.at(i));
  }
}

void taBrainAtlas::SaveAtlas() {
  Atlas().Save(filepath);
}

void taBrainAtlas::SaveAtlasAs(const String& filename) {
  filepath = filename;
  Atlas().Save(filepath);
}

void taBrainAtlas::RevertAtlas() {
  delete m_atlas;
  m_atlas = BrainAtlasFactory::CreateInstance(filepath);
  m_have_atlas_instance = (bool)m_atlas;
}

void taBrainAtlas::LoadAtlas(const String& filename) {
  delete m_atlas;
  m_atlas = BrainAtlasFactory::CreateInstance(filename);
  m_have_atlas_instance = (bool)m_atlas;
}

void taBrainAtlas::EditAtlas() {
  BrainAtlasRegexpPopulator* atlas_regexp_pop = new BrainAtlasRegexpPopulator();

  // taiRegexpField* rfield = new taiRegexpField(&TA_taString, NULL, NULL, NULL, 0,
  // 					      atlas_regexp_pop);
  // atlas_regexp_pop->setSource(this);
  // rfield->SetFieldOwner(rfield);
  iBrainViewEditDialog* red = new iBrainViewEditDialog(NULL, name, atlas_regexp_pop,
						       (void*)this, false, true);
  bool rval = red->exec();
  delete red;
  delete atlas_regexp_pop;
}

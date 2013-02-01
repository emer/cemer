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

#include "BrainAtlasRegexpPopulator.h"
#include <BrainAtlasUtils>
#include <taBrainAtlas>
#include <BrainAtlasFactory>
#include <BrainAtlas>
#include <Network>

#include <taMisc>


BrainAtlasRegexpPopulator::BrainAtlasRegexpPopulator()
: iDialogRegexpPopulator()
, m_labels()
, m_headings()
, m_filepath("")
, m_colors()
{
  m_atlas = NULL;
}

QStringList BrainAtlasRegexpPopulator::getHeadings(bool editor_mode, int& extra_cols) const
{
  extra_cols = 0;
  if (m_headings.empty()) {
    m_headings << "Hemisphere" << "Lobe" << "Gyrus" << "Tissue Type"
      << "Cell Type";
    if(editor_mode) {
      extra_cols += 1;
      m_headings << "Color";
    }
  }
  return m_headings;
}

QStringList BrainAtlasRegexpPopulator::getLabels() const
{  
  if (m_labels.empty()) {
    if(m_atlas) {
      m_labels = BrainAtlasUtils::Labels(m_atlas->Atlas());
    }
    else {
      if (m_filepath == "") {
	m_filepath = taMisc::app_dir.toQString() + "/data/atlases/Talairach.xml";
      }
      BrainAtlas* atlas = BrainAtlasFactory::CreateInstance(m_filepath);
      m_labels = BrainAtlasUtils::Labels(*atlas);
      delete atlas;
    }
  }
  return m_labels;
}

void BrainAtlasRegexpPopulator::setLabels(const QStringList& labels) {  
  if(!m_atlas) return;
  BrainAtlasUtils::SetLabels(m_atlas->Atlas(), labels);
}

void BrainAtlasRegexpPopulator::setColors(const QList<QColor>& colors) {  
  if(!m_atlas) return;
  BrainAtlasUtils::SetColors(m_atlas->Atlas(), colors);
}

QList<QColor> BrainAtlasRegexpPopulator::getColors() const
{
  // Returns a list of colors associated with the brain atlas labels
  
  if (m_colors.empty()) {
    if(m_atlas) {
      m_colors = BrainAtlasUtils::Colors(m_atlas->Atlas());
    }
    else {
      if (m_filepath == "") {
	m_filepath = taMisc::app_dir.toQString() + "/data/atlases/Talairach.xml";
      }
      BrainAtlas* atlas = BrainAtlasFactory::CreateInstance(m_filepath);
      m_colors = BrainAtlasUtils::Colors(*atlas);
      delete atlas;
    }
  }
  return m_colors;
}

QString BrainAtlasRegexpPopulator::getSeparator() const
{
  return "/";
}

void BrainAtlasRegexpPopulator::setSource(const void *fieldOwner)
{
  // Sets the source (brain atlas filepath) for the populator 
  // to the network's currently selected atlas
  
  // We know the owner of the regexp field is some type of taBase, so cast it.
  const taBase *base = reinterpret_cast<const taBase *>(fieldOwner);
  if(!base) return;

  if(base->InheritsFrom(&TA_taBrainAtlas)) {
    taBrainAtlas* bap = (taBrainAtlas*)base;
    if (!m_labels.empty()){
      m_labels.clear();
    }
    m_atlas = bap;
  }
  else {
    Network* net = NULL;
    if(base->InheritsFrom(&TA_Network)) {
      net = (Network*)base;
    }
    else if(base->InheritsFrom(&TA_Layer)) { 
      Layer* layer = (Layer*)base;
      net = layer->own_net;
    }
    if(net) {
      if (!m_labels.empty()){
	m_labels.clear();
      }
      m_atlas = net->brain_atlas;
    }
  }
}

void BrainAtlasRegexpPopulator::adjustTitle(QString &title, const void *fieldOwner) const
{
  // We know the owner of the regexp field is some type of taBase, so cast it.
  const taBase *base = reinterpret_cast<const taBase *>(fieldOwner);
  if(!base) return;
  // Then dynamic cast to verify it's a Layer.
  if(base->InheritsFrom(&TA_taBrainAtlas)) {
    taBrainAtlas* bap = (taBrainAtlas*)base;
    title.append(" atlas \"").append(bap->name.chars()).append("\"");
  }
  else if(base->InheritsFrom(&TA_Network)) {
    Network* net = (Network*)base;
    title.append(" in network \"").append(net->name.chars()).append("\"");
    if(net->brain_atlas) {
      title.append(" using atlas \"").append(net->brain_atlas->name.chars()).append("\"");
    }
  }
  else if(base->InheritsFrom(&TA_Layer)) { 
    Layer* layer = (Layer*)base;
    title.append(" in layer \"").append(layer->name.chars()).append("\"");
    if (const Network* net = layer->own_net) {
      title.append(" in network \"").append(net->name.chars()).append("\"");
      if(net->brain_atlas) {
	title.append(" using atlas \"").append(net->brain_atlas->name.chars()).append("\"");
      }
    }
  }
}


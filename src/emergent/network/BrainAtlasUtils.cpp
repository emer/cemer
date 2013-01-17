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

#include "BrainAtlasUtils.h"
#include <BrainAtlasLabel>
#include <BrainAtlas>
#include <BrainAtlasInfo>
#include <BrainAtlasFileParser>
#include <BrainAtlasFileParserFactory>

#include <QColor>
#include <QStringList>
#include <QDir>


QList<QColor> BrainAtlasUtils::Colors(const BrainAtlas& atlas, const QString& labels_regex)
{
  QList<QColor> colors;
  QList<BrainAtlasLabel> labels = atlas.Labels(labels_regex);
  foreach( BrainAtlasLabel label, labels){
    colors.append(label.Color());
  }
  return colors;
}

QStringList BrainAtlasUtils::Labels(const BrainAtlas& atlas, const QString& labels_regex)
{
  QStringList list;
  QList<BrainAtlasLabel> labels = atlas.Labels(labels_regex);
  foreach(BrainAtlasLabel label, labels) {
    list.append(label.Text());
  }
  return list;
}

QSet<int> BrainAtlasUtils::Indexes(const BrainAtlas& atlas, const QString& labelsRegexp) 
{
  QSet<int> indexes;
  
  QList<BrainAtlasLabel> labels = atlas.Labels(labelsRegexp);
  foreach( BrainAtlasLabel label, labels){
    indexes << label.Index();
  }
  return indexes;
}  

QList<int> BrainAtlasUtils::IndexList(const BrainAtlas& atlas, const QString& labelsRegexp) 
{
  QList<int> indexes;
  
  QList<BrainAtlasLabel> labels = atlas.Labels(labelsRegexp);
  foreach( BrainAtlasLabel label, labels){
    indexes.append(label.Index());
  }
  return indexes;
}  

QString BrainAtlasUtils::Label(const BrainAtlas& atlas, unsigned int index)
{
  QString text("");
  QList<BrainAtlasLabel> labels = atlas.Labels();
  foreach( BrainAtlasLabel label, labels){
    if (label.Index() == index){
      text = label.Text();
    }
  }
  return text;
}

unsigned int BrainAtlasUtils::Index(const BrainAtlas& atlas, const QString& label)
{
  unsigned int index(0);
  QList<BrainAtlasLabel> labels = atlas.Labels();
  foreach( BrainAtlasLabel l, labels){
    if (l.Text() == label){
      index = l.Index();
    }
  }
  return index;
}

QColor BrainAtlasUtils::Color(const BrainAtlas& atlas, unsigned int index)
{
  QColor clr;
  QList<BrainAtlasLabel> labels = atlas.Labels();
  foreach( BrainAtlasLabel label, labels){
    if (label.Index() == index){
      clr = label.Color();
    }
  }
  return clr;
}

QStringList BrainAtlasUtils::AtlasesAvailable(const QString& atlas_dir)
{
  QStringList atlas_list;
  
  QDir dir(atlas_dir);
  QStringList filters("*.xml");
  dir.setNameFilters(filters);
  
  QStringList files = dir.entryList();
  foreach(QString file, files) {
    BrainAtlasInfo info(BrainAtlasUtils::ParseAtlasHeader(dir.filePath(file)));
    if (info.atlas_filename != "" && info.image_filename != "") {
      atlas_list.append(dir.filePath(file));      
    }
  }
  return atlas_list;
}

BrainAtlasInfo BrainAtlasUtils::ParseAtlasHeader(const QString& filename)
{
  BrainAtlasFileParser* parser = BrainAtlasFileParserFactory::CreateInstance(filename);
  BrainAtlasInfo info(parser->ParseHeader());
  delete parser;
  return info;
}

void BrainAtlasUtils::SetLabels(BrainAtlas& atlas, const QStringList& strings)
{
  QList<BrainAtlasLabel> labels = atlas.Labels();

  if (labels.size() != strings.size()) return;

  int index(0);
  foreach(BrainAtlasLabel label, labels){
    label.SetText(strings.at(index));
    labels.replace(index, label);
    index++;
  }
  atlas.SetLabels(labels);
}

void BrainAtlasUtils::SetColors(BrainAtlas& atlas, const QList<QColor>& colors)
{
  QList<BrainAtlasLabel> labels = atlas.Labels();
  
  // must have the same number of colors to change
  if (labels.size() != colors.size()) return;
  
  int index(0);
  foreach(BrainAtlasLabel label, labels){
    label.SetColor(colors.at(index));
    labels.replace(index, label);
    index++;
  }
  atlas.SetLabels(labels);
}

void BrainAtlasUtils::SetColor(BrainAtlas& atlas, QColor clr, unsigned int index)
{
  QList<BrainAtlasLabel> labels = atlas.Labels();
  int idx(0);
  foreach( BrainAtlasLabel label, labels){
    if (label.Index() == index){
      label.SetColor(clr);
      labels.replace(idx, label);
      break;
    }
    idx++;
  }
  atlas.SetLabels(labels);
}

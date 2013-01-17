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

#include "FSLBrainAtlasFileParser.h"
#include <BrainAtlasInfo>
#include <BrainAtlasLabel>

#include <QDomDocument>
#include <QColor>
#include <QFileInfo>


FSLBrainAtlasFileParser::FSLBrainAtlasFileParser(const QString& filename)
: BrainAtlasFileParser(filename)
, m_filename(filename)
, m_atlas_dom(NULL)
{
  QFile file(m_filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
  
  m_atlas_dom = new QDomDocument;
  if (!m_atlas_dom->setContent(&file)) {
    file.close();
    delete m_atlas_dom;
    m_atlas_dom = NULL;
    return;
  }

}

FSLBrainAtlasFileParser::~FSLBrainAtlasFileParser() 
{
  if (m_atlas_dom != NULL ){
    delete m_atlas_dom;
    m_atlas_dom = NULL;
  }
}


BrainAtlasInfo FSLBrainAtlasFileParser::ParseHeader()
{
  BrainAtlasInfo info = {"", "", "", ""};
  if (m_atlas_dom == NULL) return info;
  
  // FSL atlas files (XML) have a <header> with the following structure:
  //    <header>
  //      <name>Talairach Daemon Labels</name>
  //      <description>Contains Talairach Daemon Labels</description>
  //      <type>Label</type>
  //      <images>
  //        <imagefile>/Talairach/Talairach-labels-2mm</imagefile>
  //        <summaryimagefile>/Talairach/Talairach-labels-2mm</summaryimagefile>
  //      </images>
  //      ...
  //    </header>
  
  QDomElement root = m_atlas_dom->documentElement();
  QDomNodeList elements;
  
  //  atlas filename
  info.atlas_filename = m_filename;
  
  // find name
  elements = root.elementsByTagName("name");
  if (!elements.isEmpty()){
    QDomElement element = elements.item(0).toElement();
    info.name = element.text();
  }
  
  // find description
  elements = root.elementsByTagName("description");
  if (!elements.isEmpty()){
    QDomElement element = elements.item(0).toElement();
    info.description = element.text();
  }
  
  // find imagefiles
  elements = root.elementsByTagName("imagefile");
  if (!elements.isEmpty()){
    for (int i=0; i<elements.size(); i++) {
      QDomElement element = elements.item(i).toElement();
      QString image_path(element.text());
    
      // FSL atlases often contain both 1mm^3 and 2mm^3 image files.
      // For Emergent, we are only using 1mm^3 currently, so we ignore
      // any image files that DO NOT contain "1mm" in the filename.
      if (image_path.contains("1mm") ) {
        //info.image_filename = image_path;
        QFileInfo file_info(info.atlas_filename);
        info.image_filename = file_info.absolutePath() + image_path;
      }  
    }     
  }
  return info;
}

QList<BrainAtlasLabel> FSLBrainAtlasFileParser::ParseLabels()
{
  QList<BrainAtlasLabel> labels;
  if (m_atlas_dom == NULL) return labels;

  // FSL atlas files (XML) have the following structure:
  //  <atlas version="1.0">
  //    <header>
  //      ...
  //    </header>
  //    <data>
  //      <label index="0" x="0" y="0" z="0" color="#ffffff">*.*.*.*.*</label>
  //      ...
  //    </data>
  //  </atlas>
  
  QString label_text("");
  QColor label_color(255,255,255);
  unsigned int label_index(0);
  taVector3i label_center(0,0,0);
  
  QDomElement root = m_atlas_dom->documentElement();
  QDomNodeList elements = root.elementsByTagName("label");
  if (!elements.isEmpty()){
    for (int i=0; i<elements.size(); i++) {
      QDomElement element = elements.item(i).toElement();
      label_index = element.attribute("index","0").toInt();
      label_center.SetXYZ(element.attribute("x","0").toInt(),
                         element.attribute("y","0").toInt(),
                         element.attribute("z","0").toInt());
      label_color = QColor(element.attribute("color","#ffffff"));
      label_text = element.text();
      
      BrainAtlasLabel label(label_text, label_index, label_color, label_center);
      labels.append(label);      
    }
  }
  return labels;
}


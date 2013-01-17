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

#include "FSLBrainAtlasFileComposer.h"
#include <BrainAtlasInfo>
#include <BrainAtlasLabel>

#include <QTextStream>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>

FSLBrainAtlasFileComposer::FSLBrainAtlasFileComposer(const QString& filename)
: BrainAtlasFileComposer(filename)
, m_filename(filename)
, m_atlas_dom(new QDomDocument) {}

FSLBrainAtlasFileComposer::~FSLBrainAtlasFileComposer() 
{
  if (m_atlas_dom != NULL ){
    delete m_atlas_dom;
    m_atlas_dom = NULL;
  }
}

void FSLBrainAtlasFileComposer::ComposeHeader(const BrainAtlasInfo& info)
{
  if (m_atlas_dom == NULL) return;
  QDomProcessingInstruction instr = m_atlas_dom->createProcessingInstruction( "xml", "version='1.0' encoding='UTF-8'");
  m_atlas_dom->appendChild(instr);
  
  // FSL atlas files (XML) have a <header> with the following structure:
  //    <header>
  //      <name>Talairach Daemon Labels</name>
  //      <description>Contains Talairach Daemon Labels</description>
  //      <type>Label</type>
  //      <images>
  //        <imagefile>/usr/local/share/Emergent/data/atlases/Talairach/Talairach-labels-2mm</imagefile>
  //        <summaryimagefile>/usr/local/share/Emergent/data/atlases/Talairach/Talairach-labels-2mm</summaryimagefile>
  //      </images>
  //      ...
  //    </header>
  
  QDomElement root = m_atlas_dom->createElement("atlas");
  root.setAttribute("version", "1.0");
  m_atlas_dom->appendChild(root);
  QDomElement header = addElement(*m_atlas_dom, root, "header");
  addElement(*m_atlas_dom, header, "name", info.name);
  addElement(*m_atlas_dom, header, "description", info.description);
  addElement(*m_atlas_dom, header, "type", "Label");
  QDomElement images = addElement(*m_atlas_dom, header, "images");
  addElement(*m_atlas_dom, images, "imagefile", info.image_filename);
  addElement(*m_atlas_dom, images, "summaryimagefile", info.image_filename);
}

void FSLBrainAtlasFileComposer::ComposeLabels(const QList<BrainAtlasLabel>& labels)
{
  if (m_atlas_dom == NULL) return;
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
  QDomElement root = m_atlas_dom->documentElement();
  QDomElement data = addElement(*m_atlas_dom, root, "data");
  foreach(BrainAtlasLabel label, labels){
    QDomElement label_element = addElement(*m_atlas_dom, data, "label", label.Text());
    taVector3i center = label.Center();
    label_element.setAttribute("x", center.x);
    label_element.setAttribute("y", center.y);
    label_element.setAttribute("z", center.z);
    label_element.setAttribute("index", label.Index());
    label_element.setAttribute("color", label.Color().name());
  }
}

bool FSLBrainAtlasFileComposer::Save() 
{
  if (m_atlas_dom == NULL) return false;

  QFile file(m_filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;   
  
  QTextStream stream(&file);
  stream << m_atlas_dom->toString();
  file.close();
  return true;
}

QDomElement FSLBrainAtlasFileComposer::addElement(QDomDocument& document, QDomNode& node, const QString& tag, const QString& text)
{
  QDomElement element = document.createElement( tag );
  node.appendChild( element );
  if ( !text.isNull() ) {
    QDomText text_node = document.createTextNode( text );
    element.appendChild( text_node );
  }
  return element;
}

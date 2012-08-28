    // Copyright, 1995-2007, Regents of the University of Colorado,
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

#include "brainstru.h"
#include "nifti_reader.h"

#include <QString>
#include <QStringList>
#include <QSet>
#include <QList>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QTextStream>

namespace { // anonymous
            // Set to 0 to turn off debug output.
            // Set to 1 to enable debug output
  const int DEBUG_LEVEL = 1 ;
}

////////////////////////////////////////////////////
//   BrainAtlasLabel
////////////////////////////////////////////////////
BrainAtlasLabel::BrainAtlasLabel() :
  m_text(""),
  m_index(0),
  m_color(255,255,255),
  m_center_coordinate(0,0,0)
{}
 
BrainAtlasLabel::BrainAtlasLabel(const QString& text, 
                                 unsigned int index, 
                                 const QColor& color,
                                 const TDCoord& center) :
  m_text(text),
  m_index(index),
  m_color(color),
  m_center_coordinate(center)
{}

BrainAtlasLabel::~BrainAtlasLabel()
{}

QString BrainAtlasLabel::Text() const
{
  return m_text;
}

unsigned int BrainAtlasLabel::Index() const
{
  return m_index;
}

QColor BrainAtlasLabel::Color() const
{
  return m_color;
}

TDCoord BrainAtlasLabel::Center() const
{
  return m_center_coordinate;
}

void BrainAtlasLabel::SetText(const QString& text)
{
  m_text = text;
}

void BrainAtlasLabel::SetColor(const QColor& color)
{
  m_color = color;
}

void BrainAtlasLabel::SetCenter(const TDCoord& center)
{
  m_center_coordinate = center;
}

////////////////////////////////////////////////////
//   BrainUtils
////////////////////////////////////////////////////

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

////////////////////////////////////////////////////
//   BrainAtlas
////////////////////////////////////////////////////
BrainAtlas::BrainAtlas(){}
BrainAtlas::~BrainAtlas(){}


////////////////////////////////////////////////////
//   BrainAtlasFactory
////////////////////////////////////////////////////
BrainAtlas* BrainAtlasFactory::CreateInstance(const QString& filename)
{
  if (filename.endsWith(".xml")) {
    return new FSLBrainAtlas(filename);
  }
  
  return NULL;
}

////////////////////////////////////////////////////
//   FSLBrainAtlas
////////////////////////////////////////////////////
FSLBrainAtlas::FSLBrainAtlas(const QString& filename) : BrainAtlas(),
  m_name(""),
  m_filename(""),
  m_description(""),
  m_image_filename("")
{
  m_labels.clear();
  
  // attempt to parse file header
  FSLBrainAtlasFileParser parser(filename);
  BrainAtlasInfo info(parser.ParseHeader());
  m_name = info.name;
  m_filename = info.atlas_filename;
  m_description = info.description;
  m_image_filename = info.image_filename;
  
  // parse labels and populate lists
  m_labels = parser.ParseLabels();
}

FSLBrainAtlas::~FSLBrainAtlas()
{
    m_labels.clear();
}
  
QString FSLBrainAtlas::Name() const
{
  return m_name;
}
QString FSLBrainAtlas::AtlasFilename() const
{
  return m_filename;
}

QString FSLBrainAtlas::Description() const
{
  return m_description;
}

QString FSLBrainAtlas::ImageFilename() const
{
  return m_image_filename;
}

QList<BrainAtlasLabel> FSLBrainAtlas::Labels(const QString& labels_regexp) const
{
  // Labels are fully populated during constructor's parsing
  // of file. This method returns a subset of those labels.
  QList<BrainAtlasLabel> list; 
  QRegExp regexp(labels_regexp);

  foreach(BrainAtlasLabel label, m_labels){
    if (label.Text().contains(regexp)){
      list.append(label);
    }
  }
  return list;
}

QList<FloatTDCoord> FSLBrainAtlas::VoxelCoordinates(const QString& labels_regexp) const
{
  // Extract all voxel coordinates which match the label regexp.
  // If the regexp does not match, voxel list is empty
  
  QList<TDCoord> index_coords;
  QList<FloatTDCoord> voxel_coords;
  
  // We don't currently support 2mm atlas image data 
  // files since we render to a 1mm brain 
  const QRegExp regexp("2mm");
  assert(!m_image_filename.contains(regexp));
  
  // get labels that match the regexp
  QList<BrainAtlasLabel> labels = Labels(labels_regexp);

  // now get the indexes for the matching labels
  QSet<int> match_indexes; 
  foreach( BrainAtlasLabel label, labels){
    match_indexes << label.Index();
  }

  // We need a NiftiReader to read the atlas image data
  NiftiReader img(m_image_filename);
  if (img.IsValid()) {
    
    ///////////////////////////////////////////////////////////////
    // @TODO Add support for 4D probabilistic atlases?
    //       Can use img.NumVolumes() to get number of volumes
    ///////////////////////////////////////////////////////////////
    
    // Get the dimensions of the data and a pointer to the data.
    // The data has already been byte-swapped for this platform.
    TDCoord size(img.XyzDimensions());
    const short *data = reinterpret_cast<const short *>(img.RawData());
    // i,j,k are the array indexes in the x,y,z dimensions, respectively.
    for (int k = 0; k < size.z; ++k) {
      for (int j = 0; j < size.y; ++j) {
        for (int i = 0; i < size.x; ++i) {
          short label_index = *data++;
          bool match = match_indexes.contains(label_index);
          if (match) {
            index_coords << TDCoord(i, j, k);
          }
        }
      }
    }  
  }
  
  // Transform each ijk-based (index) coordinate to an xyz-based (voxel) coordinate.
  foreach(const TDCoord &ijk_coord, index_coords) {
    voxel_coords << img.IjkToXyz(ijk_coord);
  }
  return voxel_coords;
}

void FSLBrainAtlas::SetLabels(const QList<BrainAtlasLabel>& labels)
{
  if (labels.size() != 0){
    m_labels.clear();
    m_labels = labels;  
  }
}

bool FSLBrainAtlas::Save(const QString& filename)
{
  // attempt to compose the file header
  FSLBrainAtlasFileComposer composer(filename);
  String fnm = "/" + taMisc::GetFileFmPath(m_image_filename);
  BrainAtlasInfo info = {m_name, m_description, m_filename, fnm};
  composer.ComposeHeader(info);
   
  // compose labels
  composer.ComposeLabels(m_labels); 

  // save to file
  return composer.Save();
}


////////////////////////////////////////////////////
//   BrainAtlasFileParser
////////////////////////////////////////////////////
BrainAtlasFileParser::BrainAtlasFileParser(const QString& filename) {}
BrainAtlasFileParser::~BrainAtlasFileParser() {}


////////////////////////////////////////////////////
//   BrainAtlasFileParserFactory
////////////////////////////////////////////////////
BrainAtlasFileParser* BrainAtlasFileParserFactory::CreateInstance(const QString& filename)
{
  if (filename.endsWith(".xml")) {
    return new FSLBrainAtlasFileParser(filename);
  }
  return NULL; 
}

////////////////////////////////////////////////////
//   FSLBrainAtlasFileParser
////////////////////////////////////////////////////
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
  TDCoord label_center(0,0,0);
  
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

////////////////////////////////////////////////////
//   BrainAtlasFileComposer
////////////////////////////////////////////////////
BrainAtlasFileComposer::BrainAtlasFileComposer(const QString& filename) {}
BrainAtlasFileComposer::~BrainAtlasFileComposer() {}


////////////////////////////////////////////////////
//   BrainAtlasFileComposerFactory
////////////////////////////////////////////////////
BrainAtlasFileComposer* BrainAtlasFileComposerFactory::CreateInstance(const QString& filename)
{
  if (filename.endsWith(".xml")) {
    return new FSLBrainAtlasFileComposer(filename);
  }
  return NULL; 
}

////////////////////////////////////////////////////
//   FSLBrainAtlasFileComposer
////////////////////////////////////////////////////
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
    TDCoord center = label.Center();
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

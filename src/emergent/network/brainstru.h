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

#ifndef brainstru_h
#define brainstru_h

#include "ta_geometry.h"
#include "emergent_base.h"

#ifndef __MAKETA__
#include <QColor>
#endif

class QDomDocument; // #IGNORE
class QDomNode;	    // #IGNORE
class QDomElement;  // #IGNORE

class BrainAtlas;
class BrainAtlasInfo;

///////////////////////////////////////////////////
//   BrainAtlasInfo
////////////////////////////////////////////////////
class BrainAtlasInfo
{
public:
  QString name;
  QString description;
  QString atlas_filename;
  QString image_filename;
};

////////////////////////////////////////////////////
//   BrainAtlasLabel
////////////////////////////////////////////////////
class BrainAtlasLabel
{
public:
  BrainAtlasLabel(); 
  BrainAtlasLabel(const QString& text, unsigned int index, const QColor& color = QColor(255,255,255), const taVector3i& center = taVector3i(0,0,0) );
  ~BrainAtlasLabel();
  
  QString Text() const;
  unsigned int Index() const;
  QColor Color() const;
  taVector3i Center() const;
  
  
  void SetText(const QString& text);
  void SetColor(const QColor& color);
  void SetCenter(const taVector3i& center);
  
private:
  QString m_text;
  unsigned int m_index;
  QColor m_color;
  taVector3i m_center_coordinate;
};

////////////////////////////////////////////////////
//   BrainAtlasUtils
////////////////////////////////////////////////////
class BrainAtlasUtils
{
  //    contains static functions useful for manipulating a BrainAtlas
public:
  
  static QList<QColor>  Colors(const BrainAtlas& atlas, const QString& labels_regex=".*");
  static QStringList    Labels(const BrainAtlas& atlas, const QString& labels_regex=".*");
#ifndef __MAKETA__
  static QSet<int>      Indexes(const BrainAtlas& atlas, const QString& labelsRegexp);
  static QList<int>     IndexList(const BrainAtlas& atlas, const QString& labelsRegexp);
#endif 
  static QString        Label(const BrainAtlas& atlas, unsigned int index);
  static unsigned int   Index(const BrainAtlas& atlas, const QString& label);
  static QColor   	Color(const BrainAtlas& atlas, unsigned int index);

  static QStringList    AtlasesAvailable(const QString& atlas_dir);
  static BrainAtlasInfo ParseAtlasHeader(const QString& filename);
  
  static void           SetLabels(BrainAtlas& atlas, const QStringList& labels);
  static void           SetColors(BrainAtlas& atlas, const QList<QColor>& colors);
  static void   	SetColor(BrainAtlas& atlas, QColor clr, unsigned int index);
};  

  
////////////////////////////////////////////////////
//   BrainAtlas
////////////////////////////////////////////////////
class BrainAtlas
{
public:
  BrainAtlas();
  virtual ~BrainAtlas();
  
  virtual QString   Name() const = 0;
  virtual QString   AtlasFilename() const = 0;
  virtual QString   Description() const = 0;
  virtual QString   ImageFilename() const = 0;
  virtual QList<BrainAtlasLabel>  Labels(const QString& labels_regexp=QString(".*")) const = 0;
  virtual QList<taVector3f>  VoxelCoordinates(const QString& labels_regexp=QString(".*")) const = 0;
  virtual void SetLabels(const QList<BrainAtlasLabel>& labels ) = 0;
  virtual bool Save(const QString& filename) = 0;
};

////////////////////////////////////////////////////
//   BrainAtlasFactory
////////////////////////////////////////////////////
class BrainAtlasFactory
{
public:
  static BrainAtlas* CreateInstance(const QString& filename);
};

////////////////////////////////////////////////////
//   FSLBrainAtlas
////////////////////////////////////////////////////
class FSLBrainAtlas : public BrainAtlas
{
public:
  FSLBrainAtlas(const QString& filename);
  ~FSLBrainAtlas();
  
  QString   Name() const;
  QString   AtlasFilename() const;  
  QString   Description() const;
  QString   ImageFilename() const;
  QList<BrainAtlasLabel>  Labels(const QString& labels_regexp=QString(".*")) const;
  QList<taVector3f>  VoxelCoordinates(const QString& labels_regexp=QString(".*")) const;
  void      SetLabels(const QList<BrainAtlasLabel>& labels);
  bool      Save(const QString& filename);

private:
  FSLBrainAtlas();
  FSLBrainAtlas(const FSLBrainAtlas&);
  FSLBrainAtlas operator=(const FSLBrainAtlas&);
  
  QString m_name;
  QString m_filename;
  QString m_description;
  QString m_image_filename;
  QList<BrainAtlasLabel> m_labels;
};

////////////////////////////////////////////////////
//   BrainAtlasFileParser
////////////////////////////////////////////////////
class EMERGENT_API BrainAtlasFileParser
{
public:
  BrainAtlasFileParser(const QString& filename);
  virtual ~BrainAtlasFileParser();
  virtual BrainAtlasInfo ParseHeader() = 0;
  virtual QList<BrainAtlasLabel> ParseLabels() = 0;
};

////////////////////////////////////////////////////
//   BrainAtlasFileParserFactory
////////////////////////////////////////////////////
class EMERGENT_API BrainAtlasFileParserFactory
{
public:
  static BrainAtlasFileParser* CreateInstance(const QString& filename);
};

////////////////////////////////////////////////////
//   FSLBrainAtlasFileParser
////////////////////////////////////////////////////
class EMERGENT_API FSLBrainAtlasFileParser : public BrainAtlasFileParser
{
public:
  FSLBrainAtlasFileParser(const QString& filename);
  virtual ~FSLBrainAtlasFileParser();
  
  BrainAtlasInfo ParseHeader();
  QList<BrainAtlasLabel> ParseLabels();
  
private: 
  QString m_filename;
  QDomDocument* m_atlas_dom;
};

////////////////////////////////////////////////////
//   BrainAtlasFileComposer
////////////////////////////////////////////////////
class EMERGENT_API BrainAtlasFileComposer
{
public:
  BrainAtlasFileComposer(const QString& filename);
  virtual ~BrainAtlasFileComposer();
  virtual void ComposeHeader(const BrainAtlasInfo& info) = 0;
  virtual void ComposeLabels(const QList<BrainAtlasLabel>& labels) = 0;
  virtual bool Save() = 0;
};

////////////////////////////////////////////////////
//   BrainAtlasFileComposerFactory
////////////////////////////////////////////////////
class EMERGENT_API BrainAtlasFileComposerFactory
{
public:
  static BrainAtlasFileComposer* CreateInstance(const QString& filename);
};

////////////////////////////////////////////////////
//   FSLBrainAtlasFileComposer
////////////////////////////////////////////////////
class EMERGENT_API FSLBrainAtlasFileComposer : public BrainAtlasFileComposer
{
public:
  FSLBrainAtlasFileComposer(const QString& filename);
  virtual ~FSLBrainAtlasFileComposer();
  
  void ComposeHeader(const BrainAtlasInfo& info);
  void ComposeLabels(const QList<BrainAtlasLabel>& labels);
  bool Save();

private:
  QDomElement addElement(QDomDocument& document, QDomNode& node, const QString& tag, const QString& text=QString());
  
  QString m_filename;
  QDomDocument* m_atlas_dom;
};


#endif

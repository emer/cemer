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

#ifndef BrainAtlasLabel_h
#define BrainAtlasLabel_h 1

// parent includes:
#include "network_def.h"

// member includes:
#include <taVector3i>
#include <QString>
#include <QColor>


// declare all other types mentioned but not required to include:

class EMERGENT_API BrainAtlasLabel {
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

#endif // BrainAtlasLabel_h

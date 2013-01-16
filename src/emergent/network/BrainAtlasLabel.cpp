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

#include "BrainAtlasLabel.h"

BrainAtlasLabel::BrainAtlasLabel() :
  m_text(""),
  m_index(0),
  m_color(255,255,255),
  m_center_coordinate(0,0,0)
{}
 
BrainAtlasLabel::BrainAtlasLabel(const QString& text, 
                                 unsigned int index, 
                                 const QColor& color,
                                 const taVector3i& center) :
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

taVector3i BrainAtlasLabel::Center() const
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

void BrainAtlasLabel::SetCenter(const taVector3i& center)
{
  m_center_coordinate = center;
}

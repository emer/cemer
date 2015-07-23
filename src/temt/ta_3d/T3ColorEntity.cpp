// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#include "T3ColorEntity.h"
#include <Qt3DRenderer/QPhongMaterial>

T3ColorEntity::T3ColorEntity(Qt3DNode* parent)
  : inherited(parent)
{
  color_type = PHONG;
  prev_color_type = 0;          // trigger update at first
  color = Qt::green;
  ambient = 0.2f;
  specular = 0.95f;
  shininess = 150.0f;

  phong = new Qt3D::QPhongMaterial();
  texture = new T3Texture;

  updateColor();
}

T3ColorEntity::~T3ColorEntity() {
  // todo: delete texture!
  removeComponent(phong);
  removeComponent(texture);
  delete phong;
  delete texture;
}

void T3ColorEntity::setColor(const QColor& clr, float amb,
                             float spec, float shin) {
  color_type = PHONG;
  color = clr;
  ambient = MAX(amb, 0.0f);
  specular = spec;
  shininess = shin;
  updateColor();
}

void T3ColorEntity::setTexture(const QUrl& source) {
  texture->setSource(source);
  updateColor();
}

void T3ColorEntity::updateColor() {
  texture->specular = specular;
  texture->shininess = shininess;
  
  if(color_type != prev_color_type) {
    if(prev_color_type == PHONG)
      removeMaterial(phong);
    else if(prev_color_type == TEXTURE)
      removeMaterial(texture);
    if(color_type == PHONG)
      addMaterial(phong);
    else if(color_type == TEXTURE)
      addMaterial(texture);
    prev_color_type = color_type;
  }
  phong->setDiffuse(color);
  if(ambient <= 0.0f) {
    phong->setAmbient(QColor(0,0,0));
  }
  else {
    phong->setAmbient(color.darker((int)(100.0f / ambient)));
  }
  phong->setSpecular(QColor::fromRgbF(specular, specular, specular, color.alphaF()));
  phong->setShininess(shininess);
}

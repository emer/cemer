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
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <T3PerVertexTransMaterial>
#include <T3AmbientNoCullMaterial>

#include <taMisc>

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DInput;
using namespace Qt3DExtras;

T3ColorEntity::T3ColorEntity(Qt3DNode* parent)
  : inherited(parent)
{
  color_type = NO_COLOR;               // don't start with anything
  cur_color_type = NO_COLOR;
  color = Qt::green;
  ambient = 0.2f;
  specular = 0.1f;
  shininess = 150.0f;

  updateColor();
}

T3ColorEntity::~T3ColorEntity() {
}

void T3ColorEntity::setColor(const QColor& clr, float amb,
                             float spec, float shin) {
  if(color_type == NO_COLOR) {
    if(clr.alpha() < 255)
      color_type = TRANS;
    else
      color_type = PHONG;
  }
  color = clr;
  ambient = MAX(amb, 0.0f);
  specular = spec;
  shininess = shin;
  updateColor();
}

void T3ColorEntity::setTexture(const QUrl& source) {
  color_type = TEXTURE;
  texture_src = source;
  updateColor();
}

void T3ColorEntity::setTexture(const QString& source) {
  color_type = TEXTURE;
  texture_src = source;
  updateColor();
}

void T3ColorEntity::removeAllBut(ColorType typ) {
  if(cur_color_type == typ || cur_color_type == NO_COLOR) {
    cur_color_type = typ;
    return;
  }
  QMaterial* mat = material;
  removeMaterial(mat);
  delete mat;                   // todo: not sure about whether we need to do this or not!
  cur_color_type = typ;
}

void T3ColorEntity::updateColor() {
  bool add = false;
  switch(color_type) {
  case NO_COLOR: {
    removeAllBut(NO_COLOR);
    break;
  }
  case PHONG: {
    removeAllBut(PHONG);
    QPhongMaterial* phong = NULL;
    if(material) {
      phong = dynamic_cast<QPhongMaterial*>(material);
    }
    if(!phong) {
      phong = new QPhongMaterial();
      add = true;
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
    if(add) {
      addMaterial(phong);
    }
    break;
  }
  case TRANS: {
    removeAllBut(TRANS);
    QPhongAlphaMaterial* trans = NULL;
    if(material) {
      trans = dynamic_cast<QPhongAlphaMaterial*>(material);
    }
    if(!trans) {
      trans = new QPhongAlphaMaterial();
      add = true;
    }
    trans->setDiffuse(color);
    trans->setAlpha(color.alphaF());
    if(ambient <= 0.0f) {
      trans->setAmbient(QColor(0,0,0));
    }
    else {
      trans->setAmbient(color.darker((int)(100.0f / ambient)));
    }
    trans->setSpecular(QColor::fromRgbF(specular, specular, specular, color.alphaF()));
    trans->setShininess(shininess);
    if(add) {
      addMaterial(trans);
    }
    break;
  }
  case TEXTURE: {
    removeAllBut(TEXTURE);
    T3Texture* texture = NULL;
    if(material) {
      texture = dynamic_cast<T3Texture*>(material);
    }
    if(!texture) {
      texture = new T3Texture();
      add = true;
    }
    texture->specular = specular;
    texture->shininess = shininess;
    texture->setSource(texture_src);
    if(add) {
      addMaterial(texture);
    }
    break;
  }
  case PER_VERTEX: {
    removeAllBut(PER_VERTEX);
    QPerVertexColorMaterial* per_vertex = NULL;
    if(material) {
      per_vertex = dynamic_cast<QPerVertexColorMaterial*>(material);
    }
    if(!per_vertex) {
      per_vertex = new QPerVertexColorMaterial;
      addMaterial(per_vertex);
    }
    break;
  }
  case PER_VERTEX_TRANS: {
    removeAllBut(PER_VERTEX_TRANS);
    T3PerVertexTransMaterial* per_vertex_trans = NULL;
    if(material) {
      per_vertex_trans = dynamic_cast<T3PerVertexTransMaterial*>(material);
    }
    if(!per_vertex_trans) {
      per_vertex_trans = new T3PerVertexTransMaterial;
      add = true;
    }
    per_vertex_trans->setSpecular
      (QColor::fromRgbF(specular, specular, specular, color.alphaF()));
    taMisc::Info("trans spec:", String(specular));
    per_vertex_trans->setShininess(shininess);
    if(add) {
      addMaterial(per_vertex_trans);
    }
    break;
  }
  case AMBIENT_NO_CULL: {
    removeAllBut(AMBIENT_NO_CULL);
    T3AmbientNoCullMaterial* amb_no_cull = NULL;
    if(material) {
      amb_no_cull = dynamic_cast<T3AmbientNoCullMaterial*>(material);
    }
    if(!amb_no_cull) {
      amb_no_cull = new T3AmbientNoCullMaterial;
      add = true;
    }
    amb_no_cull->setAmbient(color);
    if(add) {
      addMaterial(amb_no_cull);
    }
    break;
  }
  }
}

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

#ifndef T3ColorEntity_h
#define T3ColorEntity_h 1

// parent includes:
#include <T3Entity>

// member includes:
#include <T3Texture>

// declare all other types mentioned but not required to include:
class T3TransparentMaterial;
class T3PerVertexTransMaterial;

namespace Qt3D {
  class QPerVertexColorMaterial;
}

class TA_API T3ColorEntity : public T3Entity {
  // manages color for a T3Entity, e.g., using a Phong Material or a texture
  Q_OBJECT
  INHERITED(T3Entity)
public:
  enum ColorType {
    NO_COLOR,
    PHONG,                      // solid, phong
    TRANS,                      // transparent
    TEXTURE,                    // texture
    PER_VERTEX,                 // color specified per vertex
    PER_VERTEX_TRANS,           // color specified per vertex, transparency supported
  };

  ColorType     color_type;     // where does the color come from?
  QColor        color;          // basic color of the object
  float         ambient;        // how much to diminish the color values to create ambient color -- always based off of color -- a value of 0 means ambient is black
  float         specular;       // how bright is the specular (shiny reflection) component (always white)
  float         shininess;      // how shiny is the surface -- larger values create smaller specular highlight, and vice-versa
  QUrl          texture_src;    // location of texture
  
  virtual void  setColor(const QColor& color, float ambient = 0.2f,
                         float specular = 0.95f, float shininess = 150.0f);
  // set the color parameters and update display -- selects color_type = PHONG

  virtual void  setTexture(const QUrl& source);
  // set texture and update display -- selects color_type = TEXTURE
  virtual void  setTexture(const QString& source);
  // set texture and update display -- selects color_type = TEXTURE
  
  T3ColorEntity(Qt3DNode* parent = 0);
  ~T3ColorEntity();

public slots:
  virtual void  updateColor(); // update to new color
protected:

  void removeAllBut(ColorType typ);
  Qt3DRender::QPhongMaterial* phong;  // phong material -- null if using texture mode
  T3TransparentMaterial* trans;
  T3Texture*    texture;        // texture -- null if using phong mode
  Qt3DRender::QPerVertexColorMaterial* per_vertex;
  T3PerVertexTransMaterial* per_vertex_trans;
};

#endif // T3ColorEntity_h

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

#ifndef T3Texture_h
#define T3Texture_h 1

// parent includes:
#include <T3Entity>
#include <Qt3DRenderer/QDiffuseMapMaterial>

// member includes:
#include <Qt3DRenderer/QTextureImage>

// declare all other types mentioned but not required to include:

class TA_API T3Texture : public Qt3D::QDiffuseMapMaterial {
  // a texture material
  Q_OBJECT
  INHERITED(Qt3D::QDiffuseMapMaterial)
public:
  QUrl          source;         // path to texture image
  float         specular;       // how bright is the specular (shiny reflection) component (always white)
  float         shininess;      // how shiny is the surface -- larger values create smaller specular highlight, and vice-versa
  Qt3D::QTextureImage* texture;
  
  virtual void  setSource(const QUrl& src);

  T3Texture(Qt3DNode* parent = 0);
  ~T3Texture();
public slots:
  virtual void  updateSource(); // update to new source
};

#endif // T3Texture_h
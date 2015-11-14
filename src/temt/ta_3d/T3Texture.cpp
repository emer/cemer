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

#include "T3Texture.h"

T3Texture::T3Texture(Qt3DNode* parent)
  : inherited(parent)
  , specular(0.2f)
  , shininess(150.0f)
  , texture(new Qt3DRender::QTextureImage(this))
{
  diffuse()->addTextureImage(texture);
}

T3Texture::~T3Texture() {
  
}

void T3Texture::setSource(const QUrl& src) {
  source = src;
  updateSource();
}

void T3Texture::updateSource() {
  texture->setSource(source);
  setSpecular(QColor::fromRgbF(specular, specular, specular, 1.0f));
  setShininess(shininess);
}



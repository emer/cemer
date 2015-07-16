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

#include "T3TwoDText.h"

#include <Qt3DRenderer/QDiffuseMapMaterial>
#include <Qt3DRenderer/QPlaneMesh>

T3TwoDText::T3TwoDText(Qt3DNode* parent)
  : T3Entity(parent)
{
  addMesh(new Qt3D::QPlaneMesh());
  Qt3D::QDiffuseMapMaterial* mat = new Qt3D::QDiffuseMapMaterial();
  mat->setSpecular(QColor::fromRgbF(0.2f, 0.2f, 0.2f, 1.0f));
  mat->setShininess(2.0f);
  mat->diffuse()->addTextureImage(&texture);
  addMaterial(mat);
}

T3TwoDText::~T3TwoDText() {
}

void T3TwoDText::setText(const QString& txt) {
  label.setText(txt);
  updateRender();
}

void T3TwoDText::updateRender() {
  texture.renderLabel(label);
  QSize sz = label.size();
  Qt3D::QPlaneMesh* pmesh = dynamic_cast<Qt3D::QPlaneMesh*>(mesh);
  if(pmesh->width() != sz.width())
    pmesh->setWidth(sz.width());
  if(pmesh->height() != sz.height())
    pmesh->setHeight(sz.height());
  // todo: trigger screen update??
}


///////////////////////


T3TwoDTexture::T3TwoDTexture(Qt3DNode* parent)
  : Qt3D::QAbstractTextureImage(parent)
{
  image = NULL;
}

T3TwoDTexture::~T3TwoDTexture() {
  // if(image)
  //   delete image;
}

void T3TwoDTexture::renderLabel(QLabel& label) {
  if(!image) {
    image = new QImage(label.size(), QImage::Format_ARGB32);
  }
  else {
    if(image->size() != label.size()) {
      delete image;
    }
    image = new QImage(label.size(), QImage::Format_ARGB32);
  }
  image->fill(Qt::transparent);
  QPainter painter(image);
  label.render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
}

class T3TwoDTextureDataFunctor : public Qt3D::QTextureDataFunctor {
public:
  QImage* image;
  
  T3TwoDTextureDataFunctor(QImage* img)
    : Qt3D::QTextureDataFunctor()
    , image(img)
  {}

  // Will be executed from within a QAspectJob
  Qt3D::TexImageDataPtr operator ()() Q_DECL_FINAL
  {
    Qt3D::TexImageDataPtr dataPtr;
    dataPtr.reset(new Qt3D::TexImageData());
    if(image)
      dataPtr->setImage(*image);
    return dataPtr;
  }

  bool operator ==(const Qt3D::QTextureDataFunctor &other) const Q_DECL_FINAL
  {
    const T3TwoDTextureDataFunctor *otherFunctor = dynamic_cast<const T3TwoDTextureDataFunctor*>(&other);
    return (otherFunctor != Q_NULLPTR && otherFunctor->image == image);
  }
};

Qt3D::QTextureDataFunctorPtr T3TwoDTexture::dataFunctor() const {
  return Qt3D::QTextureDataFunctorPtr(new T3TwoDTextureDataFunctor(image));
}

void T3TwoDTexture::copy(const Qt3DNode *ref) {
  // inherited:copy(ref);
  const T3TwoDTexture* img = static_cast<const T3TwoDTexture *>(ref);
  image = img->image;
}


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

#include <T3DiffuseTransMapMaterial>
#include <Qt3DRenderer/QPlaneMesh>
#include <QPalette>
#include <QPainter>

// #include <taMisc>

T3TwoDText::T3TwoDText(Qt3DNode* parent)
  : T3Entity(parent)
  , align(T3_ALIGN_CENTER)
  , v_pos(T3_VPOS_CENTER)
{
  // bg_color = Qt::white;
  bg_color = Qt::transparent;
  QFont fnt("Arial", 24);       // 24 gives decent resolution for rendering
  label.setFont(fnt);
  texture = new T3TwoDTexture();
  plane = new T3Entity(this);
  plane->addMesh(new Qt3D::QPlaneMesh());
  T3DiffuseTransMapMaterial* mat = new T3DiffuseTransMapMaterial;
  mat->setSpecular(QColor::fromRgbF(0.2f, 0.2f, 0.2f, 1.0f));
  mat->setShininess(10000.0f);
  mat->diffuse()->addTextureImage(texture);
  plane->addMaterial(mat);
  plane->RotateDeg(1.0f, 0.0f, 0.0f, 90.0f); // flip up by default
}

T3TwoDText::~T3TwoDText() {
}

void T3TwoDText::setText(const QString& txt) {
  label.setText(txt);
  updateRender();
}

void T3TwoDText::setTextColor(const QColor& fg_color) {
  // can't get this to not clip the text!
  
  // QPalette pal = label.palette();
  // pal.setColor(QPalette::Active, QPalette::WindowText, fg_color);
  // pal.setColor(QPalette::Active, QPalette::Window, bg_color);
  // label.setAutoFillBackground(true);
  // label.setPalette(pal);

  // label.setStyleSheet("QLabel { color: " + fg_color.name() + " background-color: rgba(0,0,0,0) }");
  // updateRender();
}

void T3TwoDText::setFont(const QString& family, int pointSize, int weight,
                         bool italic) {
  QFont fnt(family, pointSize, weight, italic);
  label.setFont(fnt);
  updateRender();
}


void T3TwoDText::updateRender() {
  QSize sz = label.sizeHint();
  if(sz.width() < 1) sz.setWidth(1);
  if(sz.height() < 1) sz.setHeight(1);
  Qt3D::QPlaneMesh* pmesh = dynamic_cast<Qt3D::QPlaneMesh*>(plane->mesh);
  float wd = (float)sz.width() / (float)sz.height();
  // width in height units -- norm ht to 1
  size.setX(wd); size.setY(1.0f); size.setZ(1.0e-06f);
  if(pmesh->height() != 1.0f)
    pmesh->setHeight(1.0f);
  if(pmesh->width() != wd)
    pmesh->setWidth(wd);
  float scx = scale->scale3D().x();
  float scy = scale->scale3D().y();
  float xoff = 0.0f;
  float yoff = 0.0f;
  switch(align) {
  case T3_ALIGN_LEFT:
    xoff = 0.5f * wd;
    break;
  case T3_ALIGN_RIGHT:
    xoff = -0.5f * wd;
    break;
  default:
    break;
  }
  switch(v_pos) {
  case T3_VPOS_BOTTOM:
    yoff = 0.5f;
    break;
  case T3_VPOS_TOP:
    yoff = -0.5f;
    break;
  default:
    break;
  }
  plane->Translate(xoff, yoff, 0.0f);
  texture->renderLabel(*this);
}

///////////////////////


T3TwoDTexture::T3TwoDTexture(Qt3DNode* parent)
  : Qt3D::QAbstractTextureImage(parent)
{
  image = NULL;
}

T3TwoDTexture::~T3TwoDTexture() {
  Qt3D::QNode::cleanup();
  // no!
  // if(image)
  //   delete image;
}

void T3TwoDTexture::renderLabel(T3TwoDText& txt) {
  QSize sz = txt.label.sizeHint();
  if(sz.width() < 1) sz.setWidth(1);
  if(sz.height() < 1) sz.setHeight(1);
  if(!image) {
    image = new QImage(sz, QImage::Format_ARGB32);
  }
  else {
    if(image->size() != sz) {
      delete image;
      image = new QImage(sz, QImage::Format_ARGB32);
    }
  }
  image->fill(txt.bg_color); 
  QPainter painter(image);
  txt.label.render(&painter, QPoint(), QRegion(), QWidget::DrawChildren);
  update();
}

class T3TwoDTextureDataFunctor : public Qt3D::QTextureDataFunctor {
public:
  QImage image;
  
  T3TwoDTextureDataFunctor(QImage* img)
    : Qt3D::QTextureDataFunctor()
    , image(*img)
  {}

  // Will be executed from within a QAspectJob
  Qt3D::TexImageDataPtr operator ()() Q_DECL_FINAL
  {
    Qt3D::TexImageDataPtr dataPtr;
    dataPtr.reset(new Qt3D::TexImageData());
    dataPtr->setImage(image);
    return dataPtr;
  }

  bool operator ==(const Qt3D::QTextureDataFunctor &other) const Q_DECL_FINAL
  {
    const T3TwoDTextureDataFunctor *otherFunctor = dynamic_cast<const T3TwoDTextureDataFunctor*>(&other);
    return (otherFunctor != Q_NULLPTR && otherFunctor->image == image);
  }

  QT3D_FUNCTOR(T3TwoDTextureDataFunctor)
};

Qt3D::QTextureDataFunctorPtr T3TwoDTexture::dataFunctor() const {
  return Qt3D::QTextureDataFunctorPtr(new T3TwoDTextureDataFunctor(image));
}

void T3TwoDTexture::copy(const Qt3DNode *ref) {
  // inherited:copy(ref);
  const T3TwoDTexture* img = static_cast<const T3TwoDTexture *>(ref);
  image = img->image;
}


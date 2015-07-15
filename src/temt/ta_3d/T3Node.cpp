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

#include "T3Node.h"
#include <T3DataView>
#include <iColor>
#include <T3Panel>

#ifdef TA_QT3D

#include <Qt3DRenderer/QPlaneMesh>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QScaleTransform>
#include <Qt3DCore/QTranslateTransform>
#include <Qt3DRenderer/QPhongMaterial>
#include <Qt3DRenderer/QDiffuseMapMaterial>


T3Entity::T3Entity(Qt3DNode* parent)
  : Qt3D::QEntity(parent)
{
  mesh = NULL;
  material = NULL;
  transform.addTransform(&translate);
  transform.addTransform(&scale);
  transform.addTransform(&rotate);
}

T3Entity::~T3Entity() {
}

///////////////////////

T3TwoDTexture::T3TwoDTexture(Qt3DNode* parent)
  : Qt3D::QAbstractTextureImage(parent)
{
  image = NULL;
}

T3TwoDTexture::~T3TwoDTexture() {
  if(image)
    delete image;
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
  inherited:copy(ref);
  const T3TwoDTexture* img = static_cast<const T3TwoDTexture *>(ref);
  image = img->image;
}

///////////////////////

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

T3Node::T3Node(Qt3DNode* parent, T3DataView* dv)
  : T3Entity(parent)
  , data_view(dv)
  , expanded(false)
  , caption(NULL)
{

}

T3Node::~T3Node() {
}

void T3Node::setCaption(const QString& txt) {
  if(!caption) {
    createCaption();
  }
  caption->setText(txt);
}

void T3Node::createCaption() {
  if(caption) return;
  caption = new T3TwoDText(this);
  setDefaultCaptionTransform();
}

void T3Node::setDefaultCaptionTransform() {
  if(!caption) return;
  caption->scale.setScale(0.05f);
  caption->translate.setTranslation(QVector3D(0.0f, -0.02f, 0.02f));
  caption->rotate.setAxis(QVector3D(1.0f, 0.0f, 0.0f)); // flip up by default
  caption->rotate.setAngleDeg(90);
}

#else

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoAsciiText.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoFont.h>


SO_NODE_ABSTRACT_SOURCE(T3Node);

void T3Node::insertChildBefore(SoGroup* group, SoNode* child, SoNode* before) {
  if (before) {
    int newindex = group->findChild(before);
    if (newindex < 0) {
      SoDebugError::post("T3Node::insertChildBefore",
                    "'before' node not found: %s",
                    before->getName().getString());
      return;
    }
    group->insertChild(child, newindex);
  } else
    group->addChild(child);
}

void T3Node::insertChildAfter(SoGroup* group, SoNode* child, SoNode* after) {
  if (after) {
    int newindex = group->findChild(after) + 1;
    if (newindex <= 0) {
      SoDebugError::post("T3Node::insertChildAfter",
                    "'after' node not found: %s",
                    after->getName().getString());
      return;
    }
    group->insertChild(child, newindex); //note: index allowed to be at end (==count)
  } else
    group->insertChild(child, 0);
}

SoNode* T3Node::getNodeByName(SoGroup* group, const char* name) {
  SoNode* rval;
  for (int i = 0; i < group->getNumChildren(); ++i) {
    rval = group->getChild(i);
    if (rval->getName() == name)
      return rval;
  }
  return NULL;
}


void T3Node::initClass()
{
  SO_NODE_INIT_ABSTRACT_CLASS(T3Node, SoSeparator, "Separator");
}

T3Node::T3Node(T3DataView* dataView__)
  : dataView_(dataView__)
{
  SO_NODE_CONSTRUCTOR(T3Node);

//  this->setName("topSeparator");

  transform_ = new SoTransform;
  transform_->setName("transform");
  topSeparator()->addChild(transform_);

  txfm_shape_ = NULL; // created in subclass's constructor
  material_ = NULL; // ditto
  captionNode_ = NULL;
}

T3Node::~T3Node()
{
  transform_ = NULL;
  txfm_shape_ = NULL;
  material_ = NULL;
}

void T3Node::initCommon() { // shared code, can only be called in a subclass constructor
  txfm_shape_ = new SoTransform;
  txfm_shape_->setName("txfm_shape");
  shapeSeparator()->addChild(txfm_shape_);

  material_ = new SoMaterial;
  material_->setName("material");
  shapeSeparator()->addChild(material_);
}

void T3Node::addRemoveChildNode(SoNode* node, bool adding) {
  SoDebugError::post("T3Node[subclass]::addRemoveChildNode",
                "operation not supported for this class");
}

const char* T3Node::caption() {
  SoAsciiText* cn = captionNode(false);
  if (cn)
    return cn->string[0].getString();
  else
    return NULL;
}

SoAsciiText* T3Node::captionNode(bool auto_create) {
  if (captionNode_ || !auto_create) return captionNode_;
  SoSeparator* cs = captionSeparator(auto_create); //note: ac is true
  SoBaseColor* bc = new SoBaseColor;

  if (T3DataView* dview = dataView()) {
    T3Panel* fr = dview->GetFrame();
    iColor txtcolr = fr->GetTextColor();
    bc->rgb.setValue(txtcolr.redf(), txtcolr.greenf(), txtcolr.bluef());
  }
  else {
    bc->rgb.setValue(0, 0, 0); //black is default for text
  }
  insertChildBefore(cs, bc, NULL); // at end
  captionNode_ = new SoAsciiText();
//  captionNode_->setName("captionNode");
  insertChildBefore(cs, captionNode_, NULL); // at end
  setDefaultCaptionTransform();
  return captionNode_;
}

SoTransform* T3Node::captionTransform(bool auto_create) {
  SoSeparator* cs = captionSeparator(auto_create);
  if (!cs) return NULL;
  SoTransform* rval = (SoTransform*)getNodeByName(cs, "captionTransform");;
  if (!rval && auto_create) {
    rval = new SoTransform();
    rval->setName("captionTransform");
    cs->insertChild(rval, 0); // first after sep
  }
  return rval;
}

void T3Node::setDefaultCaptionTransform() {
  transformCaption(iVec3f(0.0f, -0.02f, 0.02f));
  resizeCaption(.05f);	// default size
}

void T3Node::setCaption(const char* value) {
  int len = (value) ? (int)strlen(value) : 0; //64: can't possibly be >4G
  // only force getting captionNode if a non-blank value
  SoAsciiText* cn = captionNode(len > 0);
  // note: if node exists, we don't check for same value, so it will trigger a redraw
  // even if it is the same -- this could be changed if desired
  if (cn) {
    cn->string.setValue(value);
    setDefaultCaptionTransform();
  }
}

void T3Node::resizeCaption(float sz) {
  SoFont* font = captionFont(true);
  font->size.setValue(sz);
}

void T3Node::transformCaption(const iVec3f& translate)
{
  transformCaption(NULL, &translate);
}

void T3Node::transformCaption(const SbRotation& rotate, const iVec3f& translate)
{
  transformCaption(&rotate, &translate);
}

void T3Node::transformCaption(const SbRotation* rotate, const iVec3f* translate) {
  SoTransform* txfm = captionTransform(true);
  if (rotate) {
    txfm->rotation.setValue(*rotate);
  }
  if (translate)
    txfm->translation.setValue(SbVec3f(translate->x, translate->y, translate->z));
}

#endif // TA_QT3D


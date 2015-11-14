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

#ifndef T3DiffuseTransMapMaterial_h
#define T3DiffuseTransMapMaterial_h 1

// parent includes:
#include <ta_def.h>
#include <Qt3DRender/QMaterial>
#include <QColor>


// member includes:

// declare all other types mentioned but not required to include:
namespace Qt3D {

class QEffect;
class QTechnique;
class QParameter;
class QShaderProgram;
class QRenderPass;
class QParameterMapping;
class QAbstractTextureProvider;
}

class TA_API T3DiffuseTransMapMaterial : public Qt3DRender::QMaterial {
  // material supporting transparency alpha channel blending -- need to pull out alpha as a separat parameter based on the QParameter mechanism
  Q_OBJECT
  INHERITED(Qt3DRender::QMaterial)
public:
  Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
  Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
  Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)
  Q_PROPERTY(Qt3DRender::QAbstractTextureProvider *diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
  Q_PROPERTY(float textureScale READ textureScale WRITE setTextureScale NOTIFY textureScaleChanged)

  explicit T3DiffuseTransMapMaterial(Qt3DCore::QNode *parent = 0);
  ~T3DiffuseTransMapMaterial();

  
  QColor ambient() const;
  QColor specular() const;
  float shininess() const;
  Qt3DRender::QAbstractTextureProvider *diffuse() const;
  float textureScale() const;

  void setAmbient(const QColor &ambient);
  void setSpecular(const QColor &specular);
  void setShininess(float shininess);
  void setDiffuse(Qt3DRender::QAbstractTextureProvider *diffuse);
  void setTextureScale(float textureScale);

 signals:
  void ambientChanged();
  void diffuseChanged();
  void specularChanged();
  void shininessChanged();
  void textureScaleChanged();

 protected:
  Qt3DRender::QEffect *m_transEffect;
  Qt3DRender::QAbstractTextureProvider *m_diffuseTexture;
  Qt3DCore::QParameter *m_ambientParameter;
  Qt3DCore::QParameter *m_diffuseParameter;
  Qt3DCore::QParameter *m_specularParameter;
  Qt3DCore::QParameter *m_shininessParameter;
  Qt3DCore::QParameter *m_textureScaleParameter;
  Qt3DCore::QParameter *m_lightPositionParameter;
  Qt3DCore::QParameter *m_lightIntensityParameter;
  Qt3DRender::QTechnique *m_transGL3Technique;
  Qt3DRender::QTechnique *m_transGL2Technique;
  Qt3DRender::QTechnique *m_transES2Technique;
  Qt3DRender::QRenderPass *m_transGL3RenderPass;
  Qt3DRender::QRenderPass *m_transGL2RenderPass;
  Qt3DRender::QRenderPass *m_transES2RenderPass;
  Qt3DRender::QShaderProgram *m_transGL3Shader;
  Qt3DRender::QShaderProgram *m_transGL2ES2Shader;

  void init();
  void init_render_pass(Qt3DRender::QRenderPass* pass);
};

#endif // T3DiffuseTransMapMaterial_h

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

#ifndef T3PerVertexTransMaterial_h
#define T3PerVertexTransMaterial_h 1

// parent includes:
#include <ta_def.h>
#include <Qt3DRender/QMaterial>
#include <QColor>

// member includes:

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QEffect;
  class QTechnique;
  class QParameter;
  class QShaderProgram;
  class QRenderPass;
  class QParameterMapping;
  class QCullFace;
  class QDepthTest;
  class QNoDepthMask;
  class QBlendEquationArguments;
  class QBlendEquation;
  class QFilterKey;
}

class TA_API T3PerVertexTransMaterial : public Qt3DRender::QMaterial {
  // material supporting transparency alpha channel blending for per-vertex colors
  Q_OBJECT
  INHERITED(Qt3DRender::QMaterial)
public:
  Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
  Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

  explicit T3PerVertexTransMaterial(Qt3DCore::QNode *parent = 0);
  ~T3PerVertexTransMaterial();

  QColor specular() const;
  float shininess() const;

  void setSpecular(const QColor &specular);
  void setShininess(float shininess);

 signals:
  void specularChanged(const QColor &specular);
  void shininessChanged(float shininess);

protected:
  void handleSpecularChanged(const QVariant &var);
  void handleShininessChanged(const QVariant &var);

  Qt3DRender::QEffect *m_transEffect;
  Qt3DRender::QParameter *m_specularParameter;
  Qt3DRender::QParameter *m_shininessParameter;
  Qt3DRender::QTechnique *m_transGL3Technique;
  Qt3DRender::QTechnique *m_transGL2Technique;
  Qt3DRender::QTechnique *m_transES2Technique;
  Qt3DRender::QRenderPass *m_transGL3RenderPass;
  Qt3DRender::QRenderPass *m_transGL2RenderPass;
  Qt3DRender::QRenderPass *m_transES2RenderPass;
  Qt3DRender::QShaderProgram *m_transGL3Shader;
  Qt3DRender::QShaderProgram *m_transGL2ES2Shader;
  Qt3DRender::QCullFace *m_cullFace;
  Qt3DRender::QDepthTest *m_depthTest;
  Qt3DRender::QNoDepthMask *m_noDepthMask;
  Qt3DRender::QBlendEquationArguments *m_blendEqArgs;
  Qt3DRender::QBlendEquation *m_blendEq;
  Qt3DRender::QFilterKey *m_filterKey;
  
  void init();
  void init_render_pass(Qt3DRender::QRenderPass* pass);
};

#endif // T3PerVertexTransMaterial_h

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

#ifndef T3AmbientNoCullMaterial_h
#define T3AmbientNoCullMaterial_h 1

// parent includes:
#include <ta_def.h>
#include <Qt3DRender/QMaterial>
#include <QColor>

// member includes:

// declare all other types mentioned but not required to include:

namespace Qt3DRender {
  class QFilterKey;
  class QEffect;
  class QTechnique;
  class QParameter;
  class QShaderProgram;
  class QRenderPass;
  class QCullFace;
}


class TA_API T3AmbientNoCullMaterial : public Qt3DRender::QMaterial {
  Q_OBJECT
  Q_PROPERTY(QColor ambient READ ambient WRITE setAmbient NOTIFY ambientChanged)
public:
  explicit T3AmbientNoCullMaterial(Qt3DCore::QNode *parent = nullptr);
  ~T3AmbientNoCullMaterial();

  QColor ambient() const;

public Q_SLOTS:
  void setAmbient(const QColor &ambient);

 Q_SIGNALS:
  void ambientChanged(const QColor &ambient);

protected:    
  void init();

  void handleAmbientChanged(const QVariant &var);

  Qt3DRender::QEffect *m_phongEffect;
  Qt3DRender::QParameter *m_ambientParameter;
  Qt3DRender::QTechnique *m_ambNoCullGL3Technique;
  Qt3DRender::QTechnique *m_ambNoCullGL2Technique;
  Qt3DRender::QTechnique *m_ambNoCullES2Technique;
  Qt3DRender::QRenderPass *m_ambNoCullGL3RenderPass;
  Qt3DRender::QRenderPass *m_ambNoCullGL2RenderPass;
  Qt3DRender::QRenderPass *m_ambNoCullES2RenderPass;
  Qt3DRender::QShaderProgram *m_ambNoCullGL3Shader;
  Qt3DRender::QShaderProgram *m_ambNoCullGL2ES2Shader;
  Qt3DRender::QCullFace *m_cullFace;
  Qt3DRender::QFilterKey *m_filterKey;
};

#endif // T3AmbientNoCullMaterial_h

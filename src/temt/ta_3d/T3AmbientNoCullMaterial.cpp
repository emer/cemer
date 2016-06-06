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

#include "T3AmbientNoCullMaterial.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QNoDepthMask>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QFilterKey>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

using namespace Qt3DCore;
using namespace Qt3DRender;

T3AmbientNoCullMaterial::T3AmbientNoCullMaterial(QNode* parent)
  : QMaterial(parent)
  , m_phongEffect(new QEffect())
  , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
  , m_ambNoCullGL3Technique(new QTechnique())
  , m_ambNoCullGL2Technique(new QTechnique())
  , m_ambNoCullES2Technique(new QTechnique())
  , m_ambNoCullGL3RenderPass(new QRenderPass())
  , m_ambNoCullGL2RenderPass(new QRenderPass())
  , m_ambNoCullES2RenderPass(new QRenderPass())
  , m_ambNoCullGL3Shader(new QShaderProgram())
  , m_ambNoCullGL2ES2Shader(new QShaderProgram())
  , m_cullFace(new QCullFace())
  , m_filterKey(new QFilterKey)
{
  init();
}

void T3AmbientNoCullMaterial::init()
{
  connect(m_ambientParameter, &Qt3DRender::QParameter::valueChanged,
          this, &T3AmbientNoCullMaterial::handleAmbientChanged);
  m_ambNoCullGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/phong.vert"))));
  m_ambNoCullGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/ambient.frag"))));
  m_ambNoCullGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/phong.vert"))));
  m_ambNoCullGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/es2/ambient.frag"))));

  m_ambNoCullGL3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
  m_ambNoCullGL3Technique->graphicsApiFilter()->setMajorVersion(3);
  m_ambNoCullGL3Technique->graphicsApiFilter()->setMinorVersion(1);
  m_ambNoCullGL3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

  m_ambNoCullGL2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
  m_ambNoCullGL2Technique->graphicsApiFilter()->setMajorVersion(2);
  m_ambNoCullGL2Technique->graphicsApiFilter()->setMinorVersion(0);
  m_ambNoCullGL2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

  m_ambNoCullES2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
  m_ambNoCullES2Technique->graphicsApiFilter()->setMajorVersion(2);
  m_ambNoCullES2Technique->graphicsApiFilter()->setMinorVersion(0);
  m_ambNoCullES2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

  m_filterKey->setParent(this);
  m_filterKey->setName(QStringLiteral("renderingStyle"));
  m_filterKey->setValue(QStringLiteral("forward"));

  m_ambNoCullGL3Technique->addFilterKey(m_filterKey);
  m_ambNoCullGL2Technique->addFilterKey(m_filterKey);
  m_ambNoCullES2Technique->addFilterKey(m_filterKey);

  m_cullFace->setMode(QCullFace::NoCulling);

  m_ambNoCullGL3RenderPass->setShaderProgram(m_ambNoCullGL3Shader);
  m_ambNoCullGL2RenderPass->setShaderProgram(m_ambNoCullGL2ES2Shader);
  m_ambNoCullES2RenderPass->setShaderProgram(m_ambNoCullGL2ES2Shader);

  m_ambNoCullGL3RenderPass->addRenderState(m_cullFace);
  m_ambNoCullGL2RenderPass->addRenderState(m_cullFace);
  m_ambNoCullES2RenderPass->addRenderState(m_cullFace);

  m_ambNoCullGL3Technique->addRenderPass(m_ambNoCullGL3RenderPass);
  m_ambNoCullGL2Technique->addRenderPass(m_ambNoCullGL2RenderPass);
  m_ambNoCullES2Technique->addRenderPass(m_ambNoCullES2RenderPass);

  m_phongEffect->addTechnique(m_ambNoCullGL3Technique);
  m_phongEffect->addTechnique(m_ambNoCullGL2Technique);
  m_phongEffect->addTechnique(m_ambNoCullES2Technique);

  m_phongEffect->addParameter(m_ambientParameter);

  setEffect(m_phongEffect);
}


/*!
  Destroys the T3AmbientNoCullMaterial.
*/
T3AmbientNoCullMaterial::~T3AmbientNoCullMaterial()
{
}

void T3AmbientNoCullMaterial::handleAmbientChanged(const QVariant &var)
{
  emit ambientChanged(var.value<QColor>());
}

QColor T3AmbientNoCullMaterial::ambient() const
{
  return m_ambientParameter->value().value<QColor>();
}

void T3AmbientNoCullMaterial::setAmbient(const QColor &ambient)
{
  m_ambientParameter->setValue(ambient);
}




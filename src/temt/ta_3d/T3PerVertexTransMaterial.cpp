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

#include "T3PerVertexTransMaterial.h"

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

T3PerVertexTransMaterial::T3PerVertexTransMaterial(QNode *parent)
  : inherited(parent)
  , m_transEffect(new QEffect())
  , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
  , m_ambientParameter(new QParameter(QStringLiteral("ambient"), 0.2f))
  , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
  , m_transGL3Technique(new QTechnique())
  , m_transGL2Technique(new QTechnique())
  , m_transES2Technique(new QTechnique())
  , m_transGL3RenderPass(new QRenderPass())
  , m_transGL2RenderPass(new QRenderPass())
  , m_transES2RenderPass(new QRenderPass())
  , m_transGL3Shader(new QShaderProgram())
  , m_transGL2ES2Shader(new QShaderProgram())
  , m_cullFace(new QCullFace())
  , m_depthTest(new QDepthTest())
  , m_noDepthMask(new QNoDepthMask())
  , m_blendEqArgs(new QBlendEquationArguments())
  , m_blendEq(new QBlendEquation())
  , m_filterKey(new QFilterKey())
{
  QObject::connect(m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
  QObject::connect(m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
  QObject::connect(m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));

  init();
}

T3PerVertexTransMaterial::~T3PerVertexTransMaterial()
{
}

QColor T3PerVertexTransMaterial::specular() const
{
  return m_specularParameter->value().value<QColor>();
}

float T3PerVertexTransMaterial::ambient() const
{
  return m_ambientParameter->value().toFloat();
}

float T3PerVertexTransMaterial::shininess() const
{
  return m_shininessParameter->value().toFloat();
}

void T3PerVertexTransMaterial::setAmbient(float ambient)
{
  m_ambientParameter->setValue(ambient);
}

void T3PerVertexTransMaterial::setSpecular(const QColor &specular)
{
  m_specularParameter->setValue(specular);
}

void T3PerVertexTransMaterial::setShininess(float shininess)
{
  m_shininessParameter->setValue(shininess);
}

// renderPasses: RenderPass {
//  renderStates: [
//                 CullFace { mode : CullFace.Back },
//                 DepthTest { func: DepthTest.Less },
//                 DepthMask { mask: false },
//                 BlendState {
//                 srcRGB: BlendState.SrcAlpha
//                     dstRGB: BlendState.OneMinusSrcAlpha
//                   }
//                 ,BlendEquation {mode: BlendEquation.FuncAdd}
//                 ]
//     shaderProgram: alphaPhong


void T3PerVertexTransMaterial::init_render_pass(QRenderPass* pass) {
  // this is how we separate these out
  // QAnnotation* techannote = new QAnnotation;
  // techannote->setName("renderingStyle");
  // techannote->setValue("transparent");
  // pass->addAnnotation(techannote);
  
  pass->addRenderState(m_cullFace);
  pass->addRenderState(m_depthTest);
  pass->addRenderState(m_noDepthMask);
  pass->addRenderState(m_blendEqArgs);
  pass->addRenderState(m_blendEq);
}

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void T3PerVertexTransMaterial::init() {
  m_transGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/pervertextrans.vert"))));
  m_transGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/pervertextrans.frag"))));
  // m_transGL2ES2Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.vert"))));
  // m_transGL2ES2Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.frag"))));

  m_transGL3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
  m_transGL3Technique->graphicsApiFilter()->setMajorVersion(3);
  m_transGL3Technique->graphicsApiFilter()->setMinorVersion(1);
  m_transGL3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

  m_transGL2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
  m_transGL2Technique->graphicsApiFilter()->setMajorVersion(2);
  m_transGL2Technique->graphicsApiFilter()->setMinorVersion(0);
  m_transGL2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

  m_transES2Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGLES);
  m_transES2Technique->graphicsApiFilter()->setMajorVersion(2);
  m_transES2Technique->graphicsApiFilter()->setMinorVersion(0);
  m_transES2Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::NoProfile);

  m_filterKey->setParent(this);
  m_filterKey->setName(QStringLiteral("renderingStyle"));
  m_filterKey->setValue(QStringLiteral("forward"));

  m_transGL3RenderPass->setShaderProgram(m_transGL3Shader);
  m_transGL2RenderPass->setShaderProgram(m_transGL2ES2Shader);
  m_transES2RenderPass->setShaderProgram(m_transGL2ES2Shader);

  m_cullFace->setMode(QCullFace::Back);
  m_depthTest->setDepthFunction(QDepthTest::Less);
  m_blendEqArgs->setSourceRgb(QBlendEquationArguments::SourceAlpha);
  m_blendEqArgs->setDestinationRgb(QBlendEquationArguments::OneMinusSourceAlpha);
  m_blendEq->setBlendFunction(QBlendEquation::Add);

  init_render_pass(m_transGL3RenderPass);
  init_render_pass(m_transGL2RenderPass);
  init_render_pass(m_transES2RenderPass);
  
  m_transGL3Technique->addRenderPass(m_transGL3RenderPass);
  m_transGL2Technique->addRenderPass(m_transGL2RenderPass);
  m_transES2Technique->addRenderPass(m_transES2RenderPass);

  m_transEffect->addTechnique(m_transGL3Technique);
  m_transEffect->addTechnique(m_transGL2Technique);
  m_transEffect->addTechnique(m_transES2Technique);

  m_transEffect->addParameter(m_ambientParameter);
  m_transEffect->addParameter(m_specularParameter);
  m_transEffect->addParameter(m_shininessParameter);

  setEffect(m_transEffect);
}

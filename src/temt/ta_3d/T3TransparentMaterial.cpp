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

#include "T3TransparentMaterial.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDepthMask>
#include <Qt3DRender/QBlendState>
#include <Qt3DRender/QBlendEquation>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

using namespace Qt3DCore;
using namespace Qt3DRender;

T3TransparentMaterial::T3TransparentMaterial(QNode *parent)
  : inherited(parent)
  , m_transEffect(new QEffect())
  , m_ambientParameter(new QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
  , m_diffuseParameter(new QParameter(QStringLiteral("kd"), QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f)))
  , m_specularParameter(new QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
  , m_shininessParameter(new QParameter(QStringLiteral("shininess"), 150.0f))
  , m_alphaParameter(new QParameter(QStringLiteral("alpha"), 0.5f))
  , m_lightPositionParameter(new QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
  , m_lightIntensityParameter(new QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
  , m_transGL3Technique(new QTechnique())
  , m_transGL2Technique(new QTechnique())
  , m_transES2Technique(new QTechnique())
  , m_transGL3RenderPass(new QRenderPass())
  , m_transGL2RenderPass(new QRenderPass())
  , m_transES2RenderPass(new QRenderPass())
  , m_transGL3Shader(new QShaderProgram())
  , m_transGL2ES2Shader(new QShaderProgram())
{
  QObject::connect(m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
  QObject::connect(m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
  QObject::connect(m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
  QObject::connect(m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
  QObject::connect(m_alphaParameter, SIGNAL(valueChanged()), this, SIGNAL(alphaChanged()));
  init();
}

/*!
  \fn T3TransparentMaterial::~T3TransparentMaterial()

  Destroys the T3TransparentMaterial.
*/
T3TransparentMaterial::~T3TransparentMaterial()
{
}

/*!
  \property T3TransparentMaterial::ambient

  Holds the ambient color.
*/
QColor T3TransparentMaterial::ambient() const
{
  return m_ambientParameter->value().value<QColor>();
}

/*!
  \property T3TransparentMaterial::diffuse

  Holds the diffuse color.
*/
QColor T3TransparentMaterial::diffuse() const
{
  return m_diffuseParameter->value().value<QColor>();
}

/*!
  \property QColor T3TransparentMaterial::specular

  Holds the specular color.
*/
QColor T3TransparentMaterial::specular() const
{
  return m_specularParameter->value().value<QColor>();
}

/*!
  \property T3TransparentMaterial::shininess

  Holds the shininess exponent.
*/
float T3TransparentMaterial::shininess() const
{
  return m_shininessParameter->value().toFloat();
}

float T3TransparentMaterial::alpha() const
{
  return m_alphaParameter->value().toFloat();
}

void T3TransparentMaterial::setAmbient(const QColor &ambient)
{
  m_ambientParameter->setValue(ambient);
}

void T3TransparentMaterial::setDiffuse(const QColor &diffuse)
{
  m_diffuseParameter->setValue(diffuse);
}

void T3TransparentMaterial::setSpecular(const QColor &specular)
{
  m_specularParameter->setValue(specular);
}

void T3TransparentMaterial::setShininess(float shininess)
{
  m_shininessParameter->setValue(shininess);
}

void T3TransparentMaterial::setAlpha(float alpha)
{
  m_alphaParameter->setValue(alpha);
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


void T3TransparentMaterial::init_render_pass(QRenderPass* pass) {
  // this is how we separate these out
  QAnnotation* techannote = new QAnnotation;
  techannote->setName("renderingStyle");
  techannote->setValue("transparent");
  pass->addAnnotation(techannote);
  
  QCullFace* cf = new QCullFace;
  cf->setMode(QCullFace::Back);
  pass->addRenderState(cf);

  QDepthTest* dt = new QDepthTest;
  dt->setFunc(QDepthTest::Less);
  pass->addRenderState(dt);

  QDepthMask* dm = new QDepthMask;
  dm->setMask(false);
  pass->addRenderState(dm);
  
  QBlendState* bs = new QBlendState;
  bs->setSrcRGB(QBlendState::SrcAlpha);
  bs->setDstRGB(QBlendState::OneMinusSrcAlpha);
  pass->addRenderState(bs);

  QBlendEquation* be = new QBlendEquation;
  be->setMode(QBlendEquation::FuncAdd);
  pass->addRenderState(be);
  
}

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void T3TransparentMaterial::init() {
  m_transGL3Shader->setVertexShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.vert"))));
  m_transGL3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.frag"))));
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

  m_transGL3RenderPass->setShaderProgram(m_transGL3Shader);
  m_transGL2RenderPass->setShaderProgram(m_transGL2ES2Shader);
  m_transES2RenderPass->setShaderProgram(m_transGL2ES2Shader);

  init_render_pass(m_transGL3RenderPass);
  init_render_pass(m_transGL2RenderPass);
  init_render_pass(m_transES2RenderPass);
  
  m_transGL3Technique->addPass(m_transGL3RenderPass);
  m_transGL2Technique->addPass(m_transGL2RenderPass);
  m_transES2Technique->addPass(m_transES2RenderPass);

  m_transEffect->addTechnique(m_transGL3Technique);
  m_transEffect->addTechnique(m_transGL2Technique);
  m_transEffect->addTechnique(m_transES2Technique);

  m_transEffect->addParameter(m_ambientParameter);
  m_transEffect->addParameter(m_diffuseParameter);
  m_transEffect->addParameter(m_specularParameter);
  m_transEffect->addParameter(m_shininessParameter);
  m_transEffect->addParameter(m_alphaParameter);
  m_transEffect->addParameter(m_lightPositionParameter);
  m_transEffect->addParameter(m_lightIntensityParameter);

  setEffect(m_transEffect);
}




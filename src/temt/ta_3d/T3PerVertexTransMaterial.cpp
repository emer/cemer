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
#include <Qt3DRender/QOpenglFilter>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QDepthMask>
#include <Qt3DRender/QBlendState>
#include <Qt3DRender/QBlendEquation>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

T3PerVertexTransMaterial::T3PerVertexTransMaterial(Qt3D::QNode *parent)
  : inherited(parent)
  , m_transEffect(new Qt3D::QEffect())
  , m_specularParameter(new Qt3D::QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
  , m_ambientParameter(new Qt3D::QParameter(QStringLiteral("ambient"), 0.2f))
  , m_shininessParameter(new Qt3D::QParameter(QStringLiteral("shininess"), 150.0f))
  , m_lightPositionParameter(new Qt3D::QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
  , m_lightIntensityParameter(new Qt3D::QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
  , m_transGL3Technique(new Qt3D::QTechnique())
  , m_transGL2Technique(new Qt3D::QTechnique())
  , m_transES2Technique(new Qt3D::QTechnique())
  , m_transGL3RenderPass(new Qt3D::QRenderPass())
  , m_transGL2RenderPass(new Qt3D::QRenderPass())
  , m_transES2RenderPass(new Qt3D::QRenderPass())
  , m_transGL3Shader(new Qt3D::QShaderProgram())
  , m_transGL2ES2Shader(new Qt3D::QShaderProgram())
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


void T3PerVertexTransMaterial::init_render_pass(Qt3D::QRenderPass* pass) {
  // this is how we separate these out
  Qt3D::QAnnotation* techannote = new Qt3D::QAnnotation;
  techannote->setName("renderingStyle");
  techannote->setValue("transparent");
  pass->addAnnotation(techannote);
  
  Qt3D::QCullFace* cf = new Qt3D::QCullFace;
  cf->setMode(Qt3D::QCullFace::Back);
  pass->addRenderState(cf);

  Qt3D::QDepthTest* dt = new Qt3D::QDepthTest;
  dt->setFunc(Qt3D::QDepthTest::Less);
  pass->addRenderState(dt);

  Qt3D::QDepthMask* dm = new Qt3D::QDepthMask;
  dm->setMask(false);
  pass->addRenderState(dm);
  
  Qt3D::QBlendState* bs = new Qt3D::QBlendState;
  bs->setSrcRGB(Qt3D::QBlendState::SrcAlpha);
  bs->setDstRGB(Qt3D::QBlendState::OneMinusSrcAlpha);
  pass->addRenderState(bs);

  Qt3D::QBlendEquation* be = new Qt3D::QBlendEquation;
  be->setMode(Qt3D::QBlendEquation::FuncAdd);
  pass->addRenderState(be);
  
}

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void T3PerVertexTransMaterial::init() {
  m_transGL3Shader->setVertexShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/pervertextrans.vert"))));
  m_transGL3Shader->setFragmentShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/pervertextrans.frag"))));
  // m_transGL2ES2Shader->setVertexShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.vert"))));
  // m_transGL2ES2Shader->setFragmentShaderCode(Qt3D::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.frag"))));

  m_transGL3Technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::Desktop);
  m_transGL3Technique->openGLFilter()->setMajorVersion(3);
  m_transGL3Technique->openGLFilter()->setMinorVersion(1);
  m_transGL3Technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::Core);

  m_transGL2Technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::Desktop);
  m_transGL2Technique->openGLFilter()->setMajorVersion(2);
  m_transGL2Technique->openGLFilter()->setMinorVersion(0);
  m_transGL2Technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::None);

  m_transES2Technique->openGLFilter()->setApi(Qt3D::QOpenGLFilter::ES);
  m_transES2Technique->openGLFilter()->setMajorVersion(2);
  m_transES2Technique->openGLFilter()->setMinorVersion(0);
  m_transES2Technique->openGLFilter()->setProfile(Qt3D::QOpenGLFilter::None);

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
  m_transEffect->addParameter(m_specularParameter);
  m_transEffect->addParameter(m_shininessParameter);
  m_transEffect->addParameter(m_lightPositionParameter);
  m_transEffect->addParameter(m_lightIntensityParameter);

  setEffect(m_transEffect);
}



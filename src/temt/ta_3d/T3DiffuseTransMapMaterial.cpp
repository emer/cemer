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

#include "T3DiffuseTransMapMaterial.h"

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
#include <Qt3DRender/QTexture>
#include <QUrl>
#include <QVector3D>
#include <QVector4D>

T3DiffuseTransMapMaterial::T3DiffuseTransMapMaterial(Qt3DCore::QNode *parent)
  : inherited(parent)
  , m_transEffect(new Qt3DRender::QEffect())
  , m_diffuseTexture(new Qt3DRender::QTexture2D())
  , m_ambientParameter(new Qt3DCore::QParameter(QStringLiteral("ka"), QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f)))
  , m_diffuseParameter(new Qt3DCore::QParameter(QStringLiteral("diffuseTexture"), m_diffuseTexture))
  , m_specularParameter(new Qt3DCore::QParameter(QStringLiteral("ks"), QColor::fromRgbF(0.95f, 0.95f, 0.95f, 1.0f)))
  , m_shininessParameter(new Qt3DCore::QParameter(QStringLiteral("shininess"), 150.0f))
    , m_textureScaleParameter(new Qt3DCore::QParameter(QStringLiteral("texCoordScale"), 1.0f))
  , m_lightPositionParameter(new Qt3DCore::QParameter(QStringLiteral("lightPosition"), QVector4D(1.0f, 1.0f, 0.0f, 1.0f)))
  , m_lightIntensityParameter(new Qt3DCore::QParameter(QStringLiteral("lightIntensity"), QVector3D(1.0f, 1.0f, 1.0f)))
  , m_transGL3Technique(new Qt3DRender::QTechnique())
  , m_transGL2Technique(new Qt3DRender::QTechnique())
  , m_transES2Technique(new Qt3DRender::QTechnique())
  , m_transGL3RenderPass(new Qt3DRender::QRenderPass())
  , m_transGL2RenderPass(new Qt3DRender::QRenderPass())
  , m_transES2RenderPass(new Qt3DRender::QRenderPass())
  , m_transGL3Shader(new Qt3DRender::QShaderProgram())
  , m_transGL2ES2Shader(new Qt3DRender::QShaderProgram())
{
  QObject::connect(m_ambientParameter, SIGNAL(valueChanged()), this, SIGNAL(ambientChanged()));
  QObject::connect(m_diffuseParameter, SIGNAL(valueChanged()), this, SIGNAL(diffuseChanged()));
  QObject::connect(m_specularParameter, SIGNAL(valueChanged()), this, SIGNAL(specularChanged()));
  QObject::connect(m_shininessParameter, SIGNAL(valueChanged()), this, SIGNAL(shininessChanged()));
  QObject::connect(m_textureScaleParameter, SIGNAL(valueChanged()), this, SIGNAL(textureScaleChanged()));

  m_diffuseTexture->setMagnificationFilter(Qt3DRender::QAbstractTextureProvider::Linear);
  m_diffuseTexture->setMinificationFilter(Qt3DRender::QAbstractTextureProvider::LinearMipMapLinear);
  m_diffuseTexture->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::Repeat));
  m_diffuseTexture->setGenerateMipMaps(true);
  m_diffuseTexture->setMaximumAnisotropy(16.0f);
  
  init();
}

/*!
  \fn Qt3DRender::T3DiffuseTransMapMaterial::~T3DiffuseTransMapMaterial()

  Destroys the T3DiffuseTransMapMaterial.
*/
T3DiffuseTransMapMaterial::~T3DiffuseTransMapMaterial()
{
}

/*!
  \property Qt3DRender::T3DiffuseTransMapMaterial::ambient

  Holds the ambient color.
*/
QColor T3DiffuseTransMapMaterial::ambient() const
{
  return m_ambientParameter->value().value<QColor>();
}

/*!
  \property Qt3DRender::T3DiffuseTransMapMaterial::diffuse

  Holds the diffuse color.
*/
Qt3DRender::QAbstractTextureProvider* T3DiffuseTransMapMaterial::diffuse() const
{
  return m_diffuseParameter->value().value<Qt3DRender::QAbstractTextureProvider *>();
}

/*!
  \property QColor Qt3DRender::T3DiffuseTransMapMaterial::specular

  Holds the specular color.
*/
QColor T3DiffuseTransMapMaterial::specular() const
{
  return m_specularParameter->value().value<QColor>();
}

/*!
  \property Qt3DRender::T3DiffuseTransMapMaterial::shininess

  Holds the shininess exponent.
*/
float T3DiffuseTransMapMaterial::shininess() const
{
  return m_shininessParameter->value().toFloat();
}

float T3DiffuseTransMapMaterial::textureScale() const
{
    return m_textureScaleParameter->value().toFloat();
}

void T3DiffuseTransMapMaterial::setAmbient(const QColor &ambient)
{
  m_ambientParameter->setValue(ambient);
}

void T3DiffuseTransMapMaterial::setSpecular(const QColor &specular)
{
  m_specularParameter->setValue(specular);
}

void T3DiffuseTransMapMaterial::setShininess(float shininess)
{
  m_shininessParameter->setValue(shininess);
}

void T3DiffuseTransMapMaterial::setDiffuse(Qt3DRender::QAbstractTextureProvider *diffuseMap)
{
  m_diffuseParameter->setValue(QVariant::fromValue(diffuseMap));
}

void T3DiffuseTransMapMaterial::setTextureScale(float textureScale)
{
  m_textureScaleParameter->setValue(textureScale);
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


void T3DiffuseTransMapMaterial::init_render_pass(Qt3DRender::QRenderPass* pass) {
  // this is how we separate these out
  Qt3DRender::QAnnotation* techannote = new Qt3DRender::QAnnotation;
  techannote->setName("renderingStyle");
  techannote->setValue("transparent");
  pass->addAnnotation(techannote);
  
  Qt3DRender::QCullFace* cf = new Qt3DRender::QCullFace;
  cf->setMode(Qt3DRender::QCullFace::Back);
  pass->addRenderState(cf);

  Qt3DRender::QDepthTest* dt = new Qt3DRender::QDepthTest;
  dt->setFunc(Qt3DRender::QDepthTest::Less);
  pass->addRenderState(dt);

  Qt3DRender::QDepthMask* dm = new Qt3DRender::QDepthMask;
  dm->setMask(false);
  pass->addRenderState(dm);
  
  Qt3DRender::QBlendState* bs = new Qt3DRender::QBlendState;
  bs->setSrcRGB(Qt3DRender::QBlendState::SrcAlpha);
  bs->setDstRGB(Qt3DRender::QBlendState::OneMinusSrcAlpha);
  pass->addRenderState(bs);

  Qt3DRender::QBlendEquation* be = new Qt3DRender::QBlendEquation;
  be->setMode(Qt3DRender::QBlendEquation::FuncAdd);
  pass->addRenderState(be);
  
}

// TODO: Define how lights are properties are set in the shaders. Ideally using a QShaderData
void T3DiffuseTransMapMaterial::init() {
  m_transGL3Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/diffusetransmap.vert"))));
  m_transGL3Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/diffusetransmap.frag"))));
  // m_transGL2ES2Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.vert"))));
  // m_transGL2ES2Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/phongalpha.frag"))));

  m_transGL3Technique->openGLFilter()->setApi(Qt3DRender::QOpenGLFilter::Desktop);
  m_transGL3Technique->openGLFilter()->setMajorVersion(3);
  m_transGL3Technique->openGLFilter()->setMinorVersion(1);
  m_transGL3Technique->openGLFilter()->setProfile(Qt3DRender::QOpenGLFilter::Core);

  m_transGL2Technique->openGLFilter()->setApi(Qt3DRender::QOpenGLFilter::Desktop);
  m_transGL2Technique->openGLFilter()->setMajorVersion(2);
  m_transGL2Technique->openGLFilter()->setMinorVersion(0);
  m_transGL2Technique->openGLFilter()->setProfile(Qt3DRender::QOpenGLFilter::None);

  m_transES2Technique->openGLFilter()->setApi(Qt3DRender::QOpenGLFilter::ES);
  m_transES2Technique->openGLFilter()->setMajorVersion(2);
  m_transES2Technique->openGLFilter()->setMinorVersion(0);
  m_transES2Technique->openGLFilter()->setProfile(Qt3DRender::QOpenGLFilter::None);

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
  m_transEffect->addParameter(m_textureScaleParameter);
  m_transEffect->addParameter(m_lightPositionParameter);
  m_transEffect->addParameter(m_lightIntensityParameter);

  setEffect(m_transEffect);
}



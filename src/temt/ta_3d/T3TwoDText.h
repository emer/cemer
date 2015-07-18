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

#ifndef T3TwoDText_h
#define T3TwoDText_h 1

// parent includes:
#include <T3Entity>

// member includes:

// declare all other types mentioned but not required to include:

class T3TwoDText; // #IGNORE

#ifndef __MAKETA__

#include <QLabel>
#include <QImage>

class TA_API T3TwoDTexture : public Qt3D::QAbstractTextureImage {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS texture provider that returns a texture of a QLabel text object
  Q_OBJECT
  INHERITED(Qt3D::QAbstractTextureImage)
public:
  QImage*       image;          // image that the label is rendered into, used for the image data

  virtual void  renderLabel(T3TwoDText& txt);
  // render the label to the image
  
  Qt3D::QTextureDataFunctorPtr dataFunctor() const override;

  explicit T3TwoDTexture(Qt3DNode* parent = 0);
  ~T3TwoDTexture();

protected:
  void copy(const Qt3DNode *ref) override;
private:
  QT3D_CLONEABLE(T3TwoDTexture)
};

enum T3AlignText {              // for abstracting over Qt3D and SoAsciiText
  T3_ALIGN_LEFT  =  Qt::AlignLeft,
  T3_ALIGN_RIGHT = Qt::AlignRight,
  T3_ALIGN_CENTER = Qt::AlignHCenter,
  T3_ALIGN_JUSTIFY = Qt::AlignJustify,
};


class TA_API T3TwoDText : public T3Entity {
  // flat two-d text element that projects a QLabel onto a plane and shows that -- by default it is upright in the XY plane 
  Q_OBJECT
  INHERITED(T3Entity)
public:
  QLabel         label;          // label containing full info for what text to render and how
  QColor         bg_color;       // background color of the text -- defaults to white, but Qt::Transparent is useful if the rendering pass supports it..
  T3Entity*      plane;          // text plane object
  T3TwoDTexture* texture;        // texture for rendering

  virtual void  setText(const QString& txt);
  // set the text and update the rendered display (just setting in label does not update render) -- call updateRender() explictly if other properties of the label are changed -- set font etc in advance of calling setText for greatest efficiency

  virtual void  setTextColor(const QColor& fg_color);
  // set the text rendering color (foreground color) -- defaults to black

  virtual void  setFont(const QString& family, int pointSize = -1, int weight = -1,
                        bool italic = false);
  // set the font for the label, using these QFont parameters
  
  T3TwoDText(Qt3DNode* parent = 0);
  ~T3TwoDText();

public slots:
  virtual void  updateRender(); // update the rendered version of the text
  
};

#endif  // __MAKETA__

#endif // T3TwoDText_h

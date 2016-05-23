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
#include <T3ColorEntity>

// member includes:

// declare all other types mentioned but not required to include:

class T3TwoDText; // #IGNORE

#ifndef __MAKETA__

#include <Qt3DRender/QAbstractTextureImage>

#include <QLabel>
#include <QImage>

class TA_API T3TwoDTexture : public Qt3DRender::QAbstractTextureImage {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS texture provider that returns a texture of a QLabel text object
  Q_OBJECT
  INHERITED(Qt3DRender::QAbstractTextureImage)
public:
  QImage*       image;          // image that the label is rendered into, used for the image data

  virtual void  renderLabel(T3TwoDText& txt);
  // render the label to the image
  
  explicit T3TwoDTexture(Qt3DNode* parent = 0);
  ~T3TwoDTexture();

protected:
  void copy(const Qt3DNode *ref);
private:
  Qt3DRender::QTextureImageDataGeneratorPtr dataGenerator() const override;
};

enum T3AlignText {              // for abstracting over Qt3D and SoAsciiText
  T3_ALIGN_LEFT  =  Qt::AlignLeft,
  T3_ALIGN_RIGHT = Qt::AlignRight,
  T3_ALIGN_CENTER = Qt::AlignHCenter,
};

enum T3VPosition {
  T3_VPOS_BOTTOM,             // current translation specifies bottom of text -- adds an extra 0.5 * height to shift up
  T3_VPOS_CENTER,             // current translation indicates center of text
  T3_VPOS_TOP,                // current translation specifies top of text -- adds an extra -0.5 * height to shift down
};

class TA_API T3TwoDText : public T3Entity {
  // flat two-d text element that projects a QLabel onto a plane and shows that -- by default it is upright in the XY plane, size.y = 1.0, size.x = required width, size.z = tiny
  Q_OBJECT
  INHERITED(T3Entity)
public:
  QLabel         label;          // label containing full info for what text to render and how
  QColor         bg_color;       // background color of the text -- defaults to white, but Qt::Transparent is useful if the rendering pass supports it..
  T3AlignText    align;          // how to align the text horizontally -- does an extra translation of the plane based on setting e.g., LEFT will cause text to start at current translation position, while RIGHT causes it to end there.
  T3VPosition    v_pos;          // how to position the text relative to current overall translation
  T3ColorEntity*      plane;          // text plane object
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

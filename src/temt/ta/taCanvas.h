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

#ifndef taCanvas_h
#define taCanvas_h 1

// parent includes:
#include <taImage>

// member includes:
#ifndef __MAKETA__
#include <QPainter>
#include <QBrush>
#else
class QPainter; // #IGNORE
class QBrush; // #IGNORE
#endif

// declare all other types mentioned but not required to include:
class QPainterPath; // #IGNORE

TypeDef_Of(taCanvas);

class TA_API taCanvas : public taImage {
  // ##CAT_Data provides a mechanism for drawing onto an image buffer
INHERITED(taImage)
public:
  enum CoordType {
    PIXELS,			// raw pixels
    NORMALIZED,			// normalized 0-1 values
  };

  CoordType 	coord_type;	// type of coordinates to use in drawing (0,0 is lower left)

  virtual void 	InitCanvas();
  // #CAT_Canvas initialize the canvas -- MUST be called prior to drawing!
  inline bool	CheckInit() {
    TestError(!m_init, "CheckInit", "Must call InitCanvas before drawing!");
    return m_init;
  }
  virtual void	EraseRGBA(float r, float g, float b, float a);
  // #CAT_Canvas erase image to given rgba color
  virtual void	EraseName(const String& name);
  // #CAT_Canvas erase image to given color name

  virtual void	Point(float x, float y);
  // #CAT_Draw 
  virtual void	Line(float x1, float y1, float x2, float y2);
  // #CAT_Draw 
  virtual void 	Rect(float l, float b, float r, float t);
  // #CAT_Draw 
  virtual void 	Circle(float x, float y, float r);
  // #CAT_Draw 
  virtual void 	Ellipse(float x, float y, float rx, float ry);
  // #CAT_Draw 
  virtual void 	FillRect(float l, float b, float r, float t);
  // #CAT_Draw 
  virtual void 	FillCircle(float x, float y, float r);
  // #CAT_Draw 
  virtual void 	FillEllipse(float x, float y, float rx, float ry);
  // #CAT_Draw 
  
  virtual void 	NewPath();
  // #CAT_Path start a new path (must have done DrawPath or FillPath on any prior paths first!)
  virtual void 	MoveTo(float x, float y);
  // #CAT_Path move current path position to given coordinate without drawing -- will create a new path if not yet initialized
  virtual void 	LineTo(float x, float y);
  // #CAT_Path 
  virtual void 	CurveTo(float x, float y, float x1, float y1, float x2, float y2);
  // #CAT_Path draw Bezier curve to given x,y point, using x1,y1 and x2,y2 as control points that determine curvature
  virtual void 	DrawPath();
  // #CAT_Path draw the current path using current pen brush and close it -- a NewPath or MoveTo is required to start a new path
  virtual void 	FillPath();
  // #CAT_Path fill the current path using current fill brush and close it -- a NewPath or MoveTo is required to start again
  virtual void 	DrawFillPath();
  // #CAT_Path draw outline and fill the current path using current pen and fill brush and close it -- a NewPath or MoveTo is required to start again
  virtual void 	DeletePath();
  // #CAT_Path delete the current path -- called by DrawPath and FillPath -- use to remove accidental path if needed

  virtual void 	TextLeft(const String& txt, float x, float y);
  // #CAT_Text draw text left justified
  virtual void 	TextCenter(const String& txt, float x, float y);
  // #CAT_Text draw text centered
  virtual void 	TextRight(const String& txt, float x, float y);
  // #CAT_Text  draw text right justified
  virtual void	SetFont(const String& font_name, int point_size=-1, int weight=-1, bool italic=false);
  // #CAT_Text set font for text drawing operations

  virtual void 	ClipRect(float l, float b, float r, float t);
  // #CAT_Clip

  virtual void	PenColorRGBA(float r, float g, float b, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorHSVA(float h, float s, float v, float a);
  // #CAT_Draw h=hue (0-360), s=saturation (0-1), v=value/brightness (0-1), a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorCMYKA(float c, float m, float y, float k, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorName(const String& name);
  // #CAT_Draw 
  virtual void	PenWidth(float width);
  // #CAT_Draw 
  virtual void	FillColorRGBA(float r, float g, float b, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorHSVA(float h, float s, float v, float a);
  // #CAT_Draw h=hue (0-360), s=saturation (0-1), v=value/brightness (0-1), a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorCMYKA(float c, float m, float y, float k, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorName(const String& name);
  // #CAT_Draw 

  void	Copy_(const taCanvas& cp);
  TA_BASEFUNS(taCanvas);
protected:
#ifdef TA_GUI
  QPainter	q_painter;
  QPainterPath*  cur_path;
  QBrush	fill_brush;
  bool		m_init;
#endif
private:
  void Initialize();
  void Destroy();
};

#endif // taCanvas_h

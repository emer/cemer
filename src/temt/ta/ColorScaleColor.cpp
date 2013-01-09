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

#include "ColorScaleColor.h"
#include <iColor>
#include <QApplication>

// set the color to a new color based on the values given
void ColorScaleColor::SetColor(float r, float g, float b, float a, RGBA* background){
  color_.setRgba(r,g,b,a);

  // compute contrast color

  // first get background color from specified background and its
  // relationship to the gui's background

  float bgc = 0.0;
  if (background !=  NULL)
    bgc = ((background->r + background->g + background->b)/3.0f) *  background->a;
  float 	dr=0.0f,dg=0.0f,db=0.0f;
#ifdef TA_GUI
  if(taMisc::gui_active) {
    ((iColor)(QApplication::palette().color(QPalette::Active,                   QPalette::Background))).intensities(dr, dg, db);
  }
#endif
  bgc += ((dr+dg+db)/3.0f) * ((background == NULL) ? 1.0 :
                              (1.0 - background->a));

  // now get the intensity of the foreground color

  float		nr=1.0,ng=1.0,nb=1.0;
  color_.intensities(nr,ng,nb);
  float na = color_.alphaf();
  // compute the ratio of the background color to the foreground color
  float bw = ((nr+ng+nb)/3.0f) * na + (bgc * (1.0f-na));

  if(bw >= 0.5f)        // color is mostly white
    nr = ng = nb = 0.0; // make contrast black
  else                  // color is mostly black
    nr = ng = nb = 1.0; // make contrast white
  contrastcolor_.setRgba(nr,ng,nb,1.0);

}


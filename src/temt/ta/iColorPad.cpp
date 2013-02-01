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

#include "iColorPad.h"
#include <iColorScaleBar>


iColorPad::iColorPad(iColorScaleBar* tsb, BlockFill s, QWidget* parent)
:inherited(parent) {
  sb = tsb;
  sb->padlist.Add(this);
  padval = oldpadval = 0;
  fill_type = s;
//  if(nm !=  NULL) name = nm;
  ReFill();
};

iColorPad::~iColorPad(){
}

void iColorPad::GetColors(){
  ColorScale* scale = sb->bar->scale; // cache
  if (!scale) return; // safety
  if (fill_type == COLOR){
    iColor tfg;
    iColor ttc;
    float sc_val;
    scale->GetColor(padval,sc_val,&tfg,&ttc);
    fg = tfg;
    tc = ttc;
    bg = tc;
  }
  else {
    if(padval > sb->max()) {
      fg = scale->maxout.color();
      tc = scale->maxout.contrastcolor();
    }
    else if (padval < sb->min()) {
      fg = scale->minout.color();
      tc = scale->minout.contrastcolor();
    }
    else if(padval >= sb->zero()) {
      fg = scale->GetColor(-1);
      tc = scale->GetContrastColor(-1);
    }
    else {
      fg = scale->GetColor(0);
      tc = scale->GetContrastColor(0);
    }
    bg = scale->GetColor((int)scale->zero);
  }
}

void iColorPad::ReFill(){
  /*TODO
  ivLayoutKit* layout = ivLayoutKit::instance();
  ivWidgetKit* wkit = ivWidgetKit::instance();
  thename = NULL;
  GetColors();
  float percent = 0.0f;
  if(fill_type != COLOR) percent = sb->GetAbsPercent(padval);
  switch (fill_type) {
  case COLOR:   theback = new DynamicBackground(NULL,fg); break;
  case AREA:    theback = new Spiral(percent,fg,bg,width,height);
  case LINEAR:  theback = new Spiral(percent*percent,fg,bg,width,height);
  }
  if(name.empty())  // no text
    thepatch->body(layout->fixed(theback,width,height));
  else {
    const ivFont* fnt = wkit->font();
    ivFontBoundingBox b;
    String newname = name;
    int len = newname.length();
    fnt->string_bbox(newname.chars(), len, b);
    while((len >1) && (b.width() > width)){
      len--;
      fnt->string_bbox(newname.chars(), len, b);
    }
    newname = newname.before(len);
    thename = new DynamicLabel((char *) newname,fnt,tc);
    thepatch->body(layout->fixed
		   (layout->overlay
		    (theback,
		     layout->vcenter(taivM->wrap(thename),0)),
		    width,height));
  } */
}

void iColorPad::SetFillType(BlockFill b){
  fill_type = b;
  ReFill();
  update();
}

void iColorPad::Reset(){ /*TODO
  GetColors();
  float percent = 0.0f;
  if (fill_type != COLOR) percent = sb->GetAbsPercent(padval);
  switch(fill_type) {
  case COLOR:  ((DynamicBackground *) theback)->SetColor(fg); break;
  case AREA:   ((Spiral *) theback)->Set(percent,fg,bg); break;
  case LINEAR: ((Spiral *) theback)->Set(percent * percent,fg,bg);
  }
  if (thename != NULL){
    if(thename != NULL) thename->SetColor(tc);
  }
  thepatch->redraw();
  ivResource::flush(); */
}

void iColorPad::Set(float val) {
  if (padval == val) return;
  oldpadval = padval;
  padval = val;
  Reset();
}

float iColorPad::GetVal(){
  return padval;
}

void iColorPad::Toggle() { /*TODO
  if (padval == ((PiColorScaleBar*) sb)->GetSelectedVal()){
    Set(oldpadval);
  }
  else {
    Set(((PiColorScaleBar*) sb)->GetSelectedVal());
  } */
}

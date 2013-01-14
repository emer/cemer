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

#include "ColorScaleBar.h"
#include <ColorScale>
#include <ColorBar>
#include <ColorPad>
#include <iLineEdit>


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>

#include <math.h>

ColorScaleBar::ColorScaleBar(bool hor_, SpanMode sm_, bool adj, bool ed, QWidget* parent)
:inherited(parent)
{
  sm = sm_;
  Init(hor_, adj, ed);
};

ColorScaleBar::~ColorScaleBar(){
  padlist.Reset();
}

void ColorScaleBar::UpdatePads(){
  for (int i = 0; i < padlist.size; ++i) {
    ((ColorPad*) padlist.FastEl(i))->Reset();
  }
}

void ColorScaleBar::Init(bool hor_, bool adj, bool ed){
  cur_minmax_set = false;
  cur_min = -1.01f;
  cur_max = -0.99f;
  hor = hor_;
  adjustflag = adj;
  editflag = ed;
  bar = NULL;
  enlarger = NULL;
  shrinker = NULL;
  min_incr = NULL;
  min_decr = NULL;
  max_incr = NULL;
  max_decr = NULL;

  if (hor)
    layOuter = new QHBoxLayout(this);
  else
    layOuter = new QVBoxLayout(this);
  layOuter->setMargin(0);

  min_frep = new iLineEdit(this);
  min_frep->setCharWidth(6); // make them a bit smaller
  max_frep = new iLineEdit(this);
  max_frep->setCharWidth(6); // make them a bit smaller
  if (editflag) {
    connect(min_frep, SIGNAL(editingFinished()), this, SLOT(editor_accept()) );
    connect(max_frep, SIGNAL(editingFinished()), this, SLOT(editor_accept()) );
  } else { // no editing
    min_frep->setReadOnly(true);
    max_frep->setReadOnly(true);
  }

  if (adjustflag) {
    if (sm == RANGE) {
      enlarger = new QToolButton(this);
      ((QToolButton*)enlarger)->setArrowType(Qt::UpArrow);
      connect(enlarger, SIGNAL(pressed()), this, SLOT(Incr_Range()));
      shrinker = new QToolButton(this);
      ((QToolButton*)shrinker)->setArrowType(Qt::DownArrow);
      connect(shrinker, SIGNAL(pressed()), this, SLOT(Decr_Range()));
    } else {
      min_incr = new QToolButton(this);
      ((QToolButton*)min_incr)->setArrowType(Qt::UpArrow);
      connect(min_incr, SIGNAL(pressed()), this, SLOT(Incr_Min()));
      min_decr = new QToolButton(this);
      ((QToolButton*)min_decr)->setArrowType(Qt::DownArrow);
      connect(min_decr, SIGNAL(pressed()), this, SLOT(Decr_Min()));
      max_incr = new QToolButton(this);
      ((QToolButton*)max_incr)->setArrowType(Qt::UpArrow);
      connect(max_incr, SIGNAL(pressed()), this, SLOT(Incr_Max()));
      max_decr = new QToolButton(this);
      ((QToolButton*)max_decr)->setArrowType(Qt::DownArrow);
      connect(max_decr, SIGNAL(pressed()), this, SLOT(Decr_Max()));
    }
  }
}

void ColorScaleBar::InitLayout() {
//TODO : add remainder of controls, ex. min/max types
  if (hor) {
    layOuter->addWidget(min_frep);
    if (bar) { 
      bar->setMaximumHeight(min_frep->height()); // make them the same
      layOuter->addWidget(bar, 1);
    }
    layOuter->addWidget(max_frep);
    if (shrinker) { //note: both or none
      // put them side by side, to save vert room
      QBoxLayout* layBut = new QHBoxLayout; layOuter->addLayout(layBut);
      layBut->setMargin(0); //spacing=2
      layBut->setSpacing(0); //abut
      layBut->addWidget(enlarger);
      layBut->addWidget(shrinker);
     }
  } else { // vert
    if (enlarger) { //note: both or none
      QBoxLayout* layBut = new QHBoxLayout; layOuter->addLayout(layBut);
      layBut->setMargin(0); //spacing=2
      layBut->addWidget(enlarger);
      layBut->addWidget(shrinker);
     }
    layOuter->addWidget(max_frep);
    if (bar) layOuter->addWidget(bar);
    layOuter->addWidget(min_frep);
  }
}

void ColorScaleBar::Adjust(){
  emit_scaleValueChanged();
}

void ColorScaleBar::editor_accept() {
  // we only do anything if the values actually changed
  String min_str(min(), "%5.3f");
  String max_str(max(), "%5.3f");
  // check the string values, because converting numbers will cause roundoff
  if ((min_str == min_frep->text()) && (max_str == max_frep->text())) return;


  if (!GetScaleValues()) return;
  UpdatePads();
  Adjust();
}

void ColorScaleBar::emit_scaleValueChanged() {
  emit scaleValueChanged();
}

/* implicit
void ColorScaleBar::editor_reject(ivFieldEditor*) {
  UpdateScaleValues();
  UpdatePads();
  Adjust();
} */

#define scalebar_low_value	1.0e-10

const iColor ColorScaleBar::GetColor(int idx, bool* ok){
  int i = idx;
  if(i < 0) i = bar->scale->colors.size + i;
  return(bar->scale->GetColor(i, ok));
}

const iColor ColorScaleBar::GetContrastColor(int idx, bool* ok){
  int i = idx;
  if(i < 0) i = bar->scale->colors.size + i;
  return(bar->scale->GetContrastColor(i, ok));
}

float ColorScaleBar::GetVal(int idx) {
  return (((idx * (max()-min()))/(bar->blocks()-1)) + min());
}

bool ColorScaleBar::GetScaleValues(){
  bool ok = true;
  if (editflag) {
    float min_t = min_frep->text().toFloat(&ok);
    float max_t;
    if (ok)
      max_t = max_frep->text().toFloat(&ok);
    if (!ok) {
      UpdateScaleValues(); // set them back
      return ok;
    }
    bar->scale->SetMinMax(min_t, max_t);
  }
  return ok;
}
void ColorScaleBar::Incr_Range(){
  int i = 1; // used to be a param
  if(adjustflag) {
    GetScaleValues();
    int j;
    for(j=0;j<i;j++){
      float incr = std::max((float)(fabs((double)range())*.111111)
			       ,(float) scalebar_low_value);
      bar->scale->ModRange(range() + incr);
    }
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }

};

void ColorScaleBar::Decr_Range(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    for(j=0;j<i;j++){
      float decr =  std::max((fabs((double) range() )*.1),scalebar_low_value);
      bar->scale->ModRange(range() - decr);
    }
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
};

void ColorScaleBar::Incr_Min(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float min_t = min();
    float newmin = min_t;
    for(j=0;j<i;j++) {
      newmin = min_t  + std::max((float)(fabs((double) min_t)*.111111)
				    ,(float) scalebar_low_value);
      if(newmin > max()) newmin = max();
    }
    bar->scale->SetMinMax(newmin, max());
    UpdateScaleValues();
    UpdatePads();
    Adjust();

  }
}
void ColorScaleBar::Decr_Min(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float min_t = min();
    for (j=0;j<i;j++) {
      min_t -= std::max((fabs((double) min_t)*.1),scalebar_low_value);
    }
    bar->scale->SetMinMax(min_t, max());
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}

void ColorScaleBar::Incr_Max(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float max_t = max();
    for(j=0;j<i;j++) {
      max_t += std::max((fabs((double)max_t) * 0.111111),scalebar_low_value);
    }
    bar->scale->SetMinMax(min(), max_t);
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}
void ColorScaleBar::Decr_Max(){
  int i = 1; // used to be a param
  if (adjustflag) {
    GetScaleValues();
    int j;
    float max_t = max();
    float newmax = max_t;
    for(j=0;j<i;j++) {
      newmax = max_t - std::max((fabs((double) max_t )*.1),scalebar_low_value);
      if(newmax < min()) newmax = min();
      max_t = newmax;
    }
    bar->scale->SetMinMax(min(), max_t);
    UpdateScaleValues();
    UpdatePads();
    Adjust();
  }
}
void ColorScaleBar::SetRange(float val){
  bar->scale->ModRange(val);
  UpdateScaleValues();
};

void ColorScaleBar::SetColorScale(ColorScale* c){
  if(bar != NULL) bar->SetColorScale(c);
}

void ColorScaleBar::SetMinMax(float mn,float mx){
  if ((mn == min()) && (mx == max())) return;
  bar->scale->SetMinMax(mn, mx);
  UpdateScaleValues();
}

// rounds up the range to the nearest 1,2,or 5 value
void ColorScaleBar::SetRoundRange(float val){
  bar->scale->ModRange(val);
  UpdateScaleValues();
}

void ColorScaleBar::UpdateScaleValues() {
  if(cur_minmax_set && min() == cur_min && max() == cur_max)
    return;
  cur_minmax_set = true;
  cur_min = min();
  cur_max = max();

  String min_str(min(), "%5.3f");
  String max_str(max(), "%5.3f");

  min_frep->setText(min_str);
  max_frep->setText(max_str);
  UpdatePads();
}


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

#include "SoScrollBar.h"
#include <T3Misc>
#include <taMath_float>
#include <T3ExaminerViewer>

#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/draggers/SoTransformBoxDragger.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoTransform.h>


SO_NODE_SOURCE(SoScrollBar);

void SoScrollBar::initClass()
{
  SO_NODE_INIT_CLASS(SoScrollBar, SoSeparator, "SoSeparator");
}

void SoScrollBar_DragStartCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DragStartCB(dragger);
}

void SoScrollBar_DraggingCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DraggingCB(dragger);
}

void SoScrollBar_DragFinishCB(void* userData, SoDragger* dragr) {
  SoTranslate1Dragger* dragger = (SoTranslate1Dragger*)dragr;
  SoScrollBar* sb = (SoScrollBar*)userData;
  sb->DragFinishCB(dragger);
}

SoScrollBar::SoScrollBar(int min_, int max_, int val_, int ps_, int ss_, float wdth_,
			 float dpth_) {
  SO_NODE_CONSTRUCTOR(SoScrollBar);

  minimum_ = min_;
  maximum_ = max_;
  value_ = val_;
  pageStep_ = ps_;
  singleStep_ = ss_;
  width_ = wdth_;
  depth_ = dpth_;
  fixValues();

  switch_  = new SoSwitch;
  switch_->whichChild = SO_SWITCH_ALL;
  addChild(switch_);

  box_mat_ = new SoMaterial;
  box_mat_->diffuseColor.setValue(T3Misc::frame_clr_r, T3Misc::frame_clr_g, T3Misc::frame_clr_b);
  box_mat_->transparency.setValue(T3Misc::frame_clr_tr);
  switch_->addChild(box_mat_);

  box_ = new SoCube;
  box_->width = 1.0f;
  box_->height = width_;
  box_->depth = depth_;
  switch_->addChild(box_);

  slide_mat_ = new SoMaterial;
  slide_mat_->diffuseColor.setValue(T3Misc::drag_inact_clr_r, T3Misc::drag_inact_clr_g, T3Misc::drag_inact_clr_b);
  slide_mat_->emissiveColor.setValue(T3Misc::drag_inact_clr_r, T3Misc::drag_inact_clr_g, T3Misc::drag_inact_clr_b);
  slide_mat_->transparency.setValue(T3Misc::drag_inact_clr_tr);
  switch_->addChild(slide_mat_);

  pos_ = new SoTranslation;
  switch_->addChild(pos_);

  slider_sep_ = new SoSeparator;
  slider_tx_ = new SoTransform;
  slider_tx_->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), (float)(.5f * taMath_float::pi));
  slider_sep_->addChild(slider_tx_);
  slider_ = new SoCylinder;
  slider_->radius = .5f * width_;
  slider_->height = sliderSize();
  slider_sep_->addChild(slider_);

  active_sep_ = new SoSeparator;
  
  active_mat_ = new SoMaterial;
  active_mat_->diffuseColor.setValue(T3Misc::drag_activ_clr_r, T3Misc::drag_activ_clr_g, T3Misc::drag_activ_clr_b);
  active_mat_->emissiveColor.setValue(T3Misc::drag_activ_clr_r, T3Misc::drag_activ_clr_g, T3Misc::drag_activ_clr_b);
  active_mat_->transparency.setValue(T3Misc::drag_activ_clr_tr);
  active_sep_->addChild(active_mat_);
  active_sep_->addChild(slider_sep_);

  dragger_ = new SoTranslate1Dragger;
  dragger_->setPart("translator", slider_sep_);
  dragger_->setPart("translatorActive", active_sep_);
  dragger_->addFinishCallback(SoScrollBar_DragFinishCB, (void*)this);
  dragger_->addStartCallback(SoScrollBar_DragStartCB, (void*)this);
  dragger_->addMotionCallback(SoScrollBar_DraggingCB, (void*)this);
  switch_->addChild(dragger_);

//   pos_->translation.connectFrom(&dragger_->translation);

  valueChanged_cb_ = NULL;
  valueChanged_ud_ = NULL;

  repositionSlider();
}

void SoScrollBar::fixValues() {
  if(minimum_ > maximum_) {
    int tmp = maximum_;
    maximum_ = minimum_;
    minimum_ = tmp;
  }
  //  if(maximum_ == minimum_) maximum_ = minimum_ + 1;

  if(value_>maximum_) value_ = maximum_;
  if(value_<minimum_) value_ = minimum_;

  int range = maximum_ - minimum_;

  if(pageStep_ > range) {
    if(range == 0)
      pageStep_ = 1;
    else
      pageStep_ = range;
  }
}

///////////////////////
//  [         ][    ]
//  Min.....Max..PS..
//  [  doc length   ]
///////////////////////
//  [    PS         ]  (range = 0)


float SoScrollBar::getPos() {
  int range = maximum_ - minimum_;
  float scrng = ((float)range + (float)pageStep_); // scrollable range
  float pos = (float)value_ / scrng;
  return pos;
}

int SoScrollBar::getValFmPos(float pos) {
  int range = maximum_ - minimum_;
  float val = pos * (float)(range + pageStep_); // scrollable range
  if(val < 0.0f) val -= .5f;	// effect rounding
  else		 val += .5f;
  return (int)val;
}

float SoScrollBar::sliderSize() {
  int range = maximum_ - minimum_;
  float pspct = (float)pageStep_ / (float)(range + pageStep_);
  return MAX(pspct, width_);	// keep it within range
}

void SoScrollBar::repositionSlider() {
  fixValues();
  float slsz = sliderSize();
  slider_->height = slsz;
  if(slsz >= .999f) {
    switch_->whichChild = SO_SWITCH_NONE;
  }
  else {
    switch_->whichChild = SO_SWITCH_ALL;
  }
  pos_->translation.setValue(getPos()-.5f + .5f * slsz, 0.0f, 0.0f);
}

void SoScrollBar::setValue(int new_val) {
  if(value_ == new_val) return;
  value_ = new_val;
  repositionSlider();
}
void SoScrollBar::setMinimum(int new_min) {
  if(minimum_ == new_min) return;
  minimum_ = new_min;
  repositionSlider();
}
void SoScrollBar::setMaximum(int new_max) {
  if(maximum_ == new_max) return;
  maximum_ = new_max;
  repositionSlider();
}
void SoScrollBar::setPageStep(int new_ps) {
  if(pageStep_ == new_ps) return;
  pageStep_ = new_ps;
  slider_->height = MAX(pageStep_, width_); // don't shrink too far
  repositionSlider();
}
void SoScrollBar::setSingleStep(int new_ss) {
  if(singleStep_ == new_ss) return;
  singleStep_ = new_ss;
}
void SoScrollBar::setWidth(float new_width) {
  if(width_ == new_width) return;
  width_ = new_width;
  box_->height = width_;
  slider_->radius = .5f * width_;
  slider_->height = MAX(pageStep_, width_); // don't shrink too far
}
void SoScrollBar::setDepth(float new_depth) {
  if(depth_ == new_depth) return;
  depth_ = new_depth;
  box_->depth = depth_;
}

void SoScrollBar::setValueChangedCB(SoScrollBarCB cb_fun, void* user_data) {
  valueChanged_cb_ = cb_fun;
  valueChanged_ud_ = user_data;
}

void SoScrollBar::valueChangedCB() {
  if(valueChanged_cb_)
    (*valueChanged_cb_)(this, value_, valueChanged_ud_);
}

void SoScrollBar::DragStartCB(SoTranslate1Dragger* dragger) {
  start_val_ = value_;
  T3ExaminerViewer::so_scrollbar_is_dragging = true;
}

void SoScrollBar::DraggingCB(SoTranslate1Dragger* dragger) {
  SbVec3f trans = dragger->translation.getValue();
  
  int incr = getValFmPos(trans[0]);

  int nw_val = start_val_ + incr;
  value_ = nw_val;
  fixValues();
  int delta_val = value_ - start_val_;
  if(nw_val < minimum_ || nw_val > maximum_) {
    int range = maximum_ - minimum_;
    float nw_pos = (float)delta_val / ((float)range + (float)pageStep_);
    dragger->translation.setValue(nw_pos, 0.0f, 0.0f);
  }
  if(delta_val != 0) {
    valueChangedCB();
  }
}

void SoScrollBar::DragFinishCB(SoTranslate1Dragger* dragger) {
  SbVec3f trans = dragger->translation.getValue();

  int incr = getValFmPos(trans[0]);

  value_ = start_val_ + incr;
  fixValues();

  dragger->translation.setValue(0.0f, 0.0f, 0.0f);
  repositionSlider();

  valueChangedCB();
  T3ExaminerViewer::so_scrollbar_is_dragging = false; // this drops us out of the event loop!!
}


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

#ifndef SoScrollBar_h
#define SoScrollBar_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <Inventor/nodes/SoSeparator.h>
#endif

// member includes:

// declare all other types mentioned but not required to include:
class SoMaterial;
class SoTranslate1Dragger;
class SoTranslation;

// unfortunately you can't seem to do multiple inheritance for both QObject and So,
// so we need to do our own callbacks..

class SoScrollBar;

typedef void (*SoScrollBarCB)(SoScrollBar*, int, void*);
// callback function format: passes the scrollbar, current value, and void* user_data

TypeDef_Of(SoScrollBar);

class TA_API SoScrollBar: public SoSeparator { 
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS a scrollbar for scrolling a view, uses same interface as QScrollBar from Qt -- length is 1.0, default orientation is in X plane -- put transform in front to change
#ifndef __MAKETA__
typedef SoSeparator inherited;
  SO_NODE_HEADER(SoScrollBar);
#endif // def __MAKETA__
public:
  static void		initClass();
  SoScrollBar(int min_=0, int max_=10, int val_=0, int ps_=5, int ss_=1,
	      float wdth_ = .05f, float dpth_ = .01f);

  int	value() const		{ return value_; }	
  int	minimum() const		{ return minimum_; }
  int	maximum() const		{ return maximum_; }
  int	pageStep() const	{ return pageStep_; }
  int	singleStep() const	{ return singleStep_; }
  float width()	const		{ return width_; } // width is Y axis (length is X) 
  float depth()	const		{ return depth_; } // depth is Z axis

  void	setValue(int new_val);
  void	setMinimum(int new_min);
  void	setMaximum(int new_max);
  void	setPageStep(int new_ps);
  void	setSingleStep(int new_ss);
  void  setWidth(float new_width);
  void  setDepth(float new_depth);

  SoMaterial* getBoxMat() 		{ return box_mat_; } // #IGNORE
  SoMaterial* getSlideMat() 		{ return slide_mat_; } // #IGNORE
  SoMaterial* getActiveMat() 		{ return active_mat_; } // #IGNORE

  void	setValueChangedCB(SoScrollBarCB cb_fun, void* user_data = NULL);
  // #IGNORE set callback for when value changes

  void	DragStartCB(SoTranslate1Dragger* dragger);
  // callback: do not touch!
  void	DraggingCB(SoTranslate1Dragger* dragger);
  // callback: do not touch!
  void	DragFinishCB(SoTranslate1Dragger* dragger);
  // callback: do not touch!
protected:
  const char*  	getFileFormatName() const {return "Separator"; } 

  float	width_;
  float	depth_;	
  int	minimum_;
  int	maximum_;
  int	value_;
  int 	start_val_;
  int	pageStep_;		// also controls the size of the bar
  int	singleStep_;

  SoScrollBarCB	valueChanged_cb_; // value has changed callback
  void*		valueChanged_ud_; // user data

  // listed as ordered elements under overall sep
  SoMaterial* 	  box_mat_;	// box material
  SoCube* 	  box_;		// containing box for slider
  SoMaterial* 	  slide_mat_;	// slider material (inactive)
  SoTranslation*  pos_;		// position of slider
  SoTranslate1Dragger* dragger_; // the dragger

  // this is the slider that replaces guy in dragger
  SoSeparator*	slider_sep_;	// slider separator
  SoTransform*	slider_tx_;	// slider transform (rotate)
  SoCylinder*	slider_;	// slider itself

  SoSeparator*	active_sep_; 	// active slider sep
  SoMaterial* 	active_mat_;	// slider material (active)

  void	fixValues();		// make sure values are sensible
  float	getPos();		// get position for slider based on value
  int	getValFmPos(float pos);	// get value from position
  void	repositionSlider();	// reposition the slider based on current values
  float	sliderSize();		// get size of slider
  void	valueChangedCB();	// perform value changed callback
};

#endif // SoScrollBar_h

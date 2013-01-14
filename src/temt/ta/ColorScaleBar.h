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

#ifndef ColorScaleBar_h
#define ColorScaleBar_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif

// member includes:
#include <Widget_List>
#include <ColorBar>

// declare all other types mentioned but not required to include:
class iLineEdit;
class ColorScale;


class TA_API ColorScaleBar : public QWidget { // #IGNORE  Scalebar
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
  Q_OBJECT
public:
  enum SpanMode {
    RANGE,			// Range mode
    MIN_MAX 			// min max
  };
  float 		min() {return bar->scale->min;}
  float 		max() {return bar->scale->max;}
  float			range() {return bar->scale->range;}
  float			zero() {return bar->scale->zero;}

  SpanMode		sm;
  bool			adjustflag;	// do we have min/max/range adjuster buttons
  bool			editflag;	// is the label editable

  QBoxLayout*		layOuter;  // hor or vert as indicated
  ColorBar*		bar;		// actual bar

  iLineEdit*  		min_frep;
  iLineEdit*  		max_frep;

  QAbstractButton* 	enlarger;	// increase range button
  QAbstractButton* 	shrinker;	// decrease range button
  QAbstractButton* 	min_incr;	// increment minimum button
  QAbstractButton* 	min_decr;	// decrement minimum button
  QAbstractButton* 	max_incr;	// increment maximum button
  QAbstractButton* 	max_decr;	// decrement maximum button

  Widget_List	  	padlist; 	// #IGNORE list of color pads for palletes

  void			emit_scaleValueChanged();
  virtual void SetRange(float val);
  virtual void SetRoundRange(float val);
  virtual void SetMinMax(float min,float max);
  virtual float GetVal(int idx);	// returns value of the index
  virtual const iColor GetColor(int idx, bool* ok = NULL);	// return color[idx];
  virtual const iColor GetContrastColor(int idx, bool* ok = NULL);	// return color[idx];

  virtual void SetColorScale(ColorScale* c);

  virtual void UpdatePads();
  virtual void Adjust(); //TODO: prob not needed
  virtual bool GetScaleValues();      // gets from the edit into nums; if conv error, then returns false and sets values back
  virtual void UpdateScaleValues();   // puts from nums into glyph

  ColorScaleBar(bool hor, SpanMode sm, bool adj, bool ed, QWidget* parent = NULL);
  virtual ~ColorScaleBar();

public slots:
  virtual void editor_accept();
  //nn virtual void editor_reject(ivFieldEditor*);
  virtual void Incr_Range();
  virtual void Decr_Range();
  virtual void Incr_Min();
  virtual void Incr_Max();
  virtual void Decr_Min();
  virtual void Decr_Max();

#ifndef __MAKETA__
signals:
  void		scaleValueChanged(); // one of the values was changed
#endif

protected:
  bool		hor; // true for horizontal, false for vertical
  float		cur_min;	// currently displayed values
  float		cur_max;	// currently displayed values
  bool		cur_minmax_set;	// there are currently set values
  
  void 		InitLayout(); // call this in final inherited class constructor
private:
  void 		Init(bool hor, bool adj, bool ed);
};


#endif // ColorScaleBar_h

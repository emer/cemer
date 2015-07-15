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

#ifndef MotionGaborFilter_h
#define MotionGaborFilter_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <float_Matrix>

// declare all other types mentioned but not required to include:
class DataTable; // 


taTypeDef_Of(MotionGaborFilter);

class TA_API MotionGaborFilter : public taNBase {
  // #STEM_BASE ##CAT_Image defines a gabor filter oriented in space and time (gaussian times a plane sine wave) that moving highlights lines/edges in an image.
INHERITED(taNBase)
public:
  enum MotionGaborParam {
    CTR_X,
    CTR_Y,
    CTR_T,
    SPAT_ANGLE,
    TIME_ANGLE,
    PHASE,
    FREQ,
    FREQ_T,
    WIDTH,
    LENGTH,
    WIDTH_T,
    AMP,
  };

  int		x_size;		// overall size of the filtered region
  int		y_size;		// overall size of the filtered region
  int		t_size;		// overall size of the filtered region
  float		ctr_x;		// center in x coord
  float		ctr_y;		// center in y coord
  float		ctr_t;		// center in y coord

  float		spat_angle;	// angle of sine wave in 2-d space (in radians)
  float		time_angle;	// angle of sine wave in 2-d time (in radians)

  float		phase;		// phase of sine wave wrt the center of the gaussian (radians)
  float		freq;		// frequency of the sine wave
  float		freq_t;		// frequency of the sine wave

  float		width;		// width of the gaussian in the wave direction
  float		length;		// width of the gaussian in the wave direction
  float		width_t;	// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)
  bool		use_3d_gabors;
  float_Matrix  filter;		// #READ_ONLY #NO_SAVE #NO_COPY our filter

  virtual float	Eval(float x, float y, float t);
  // #CAT_GaborFilter evaluate gabor function for given coordinates
  virtual void	RenderFilter(float_Matrix& flt);
  // #CAT_GaborFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_GaborFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual float	GetParam(MotionGaborParam param);
  // #CAT_GaborFilter get particular parameter value

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a graph from it
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a grid view of it (reset an existing data first)

  virtual String& PrintParams(String& strm);
  // #CAT_GaborFilter output current parameter values to stream

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(MotionGaborFilter);
};

#endif // MotionGaborFilter_h

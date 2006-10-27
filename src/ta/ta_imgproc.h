// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

#ifndef ta_imgproc_h
#define ta_imgproc_h

#include "ta_math.h"
#include "ta_program.h"
#include "ta_geometry.h"

// image processing operations

class TA_API DoGFilterSpec : public taNBase {
  // defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
INHERITED(taNBase)
public:
  int		filter_width;	// half-width of the filter (typically 2 * sur_sigma)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * width + 1
  float		ctr_sigma;	// width of the narrower central gaussian
  float		sur_sigma;	// width of the wider surround gaussian (typically 2 * ctr_sigma)
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of radius filter_width -- makes the filter more radially symmetric
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #SHOW positive-valued components of filter
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #SHOW negative-valued components of filter
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #SHOW net overall filter

  virtual float	Eval(float x, float y);
  // evaluate dog function for given coordinates: NOTE: THIS IS NON-NORMALIZED, just the raw dog
  virtual void	RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt, float_Matrix& net_flt);
  // render filter into matrix
  virtual void	UpdateFilter();
  // make our personal filter (RenderFilter(filter)) according to current params

// todo: should just be a wizard call on filter..
//   virtual void	GraphFilter(GraphLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian
//   virtual void	GridFilter(GridLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGFilterSpec);
};

class TA_API GaborFilterSpec : public taNBase {
  // defines a gabor filter (gaussian times a plane sine wave) that highlights lines/edges in an image
INHERITED(taNBase)
public:
  enum GaborParam {
    CTR_X,
    CTR_Y,
    ANGLE,
    PHASE,
    FREQ,
    LENGTH,
    WIDTH,
    AMP,
  };

  int		x_size;		// overall size of the filtered region
  int		y_size;		// overall size of the filtered region
  float		ctr_x;		// center in x coord
  float		ctr_y;		// center in y coord
  float		angle;		// angle of sine wave in 2-d space (in radians)
  float		phase;		// phase of sine wave wrt the center of the gaussian (radians)
  float		freq;		// frequency of the sine wave
  float		length;		// length of the gaussian perpendicular to the wave direction
  float		width;		// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)
  float_Matrix	filter;		// #READ_ONLY #NO_SAVE #SHOW our filter

  virtual float	Eval(float x, float y);
  // evaluate gabor function for given coordinates
  virtual void	RenderFilter(float_Matrix& flt);
  // render filter into matrix
  virtual void	UpdateFilter();
  // make our personal filter (RenderFilter(filter)) according to current params

  virtual float	GetParam(GaborParam param); // get particular parameter value

// todo: should just be a wizard call on filter..
//   virtual void	GraphFilter(GraphLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian
//   virtual void	GridFilter(GridLog* disp_log); // #BUTTON #NULL_OK plot the filter gaussian

  virtual void	OutputParams(ostream& strm = cerr);

//   void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFilterSpec);
};

class TA_API GaborFitter : public GaborFilterSpec {
  // fits a gabor filter from image data
INHERITED(GaborFilterSpec)
public:
  float		fit_dist;	// #READ_ONLY #SHOW fit distance

  virtual float ParamDist(const GaborFilterSpec& oth);
  // return euclidian distance between parameters for this spec and the other one

//   virtual float	FitData(float_Matrix& data_vals, bool use_cur_vals = false);
  // find best-fitting parameters for given data.  use_cur_vals = use current values as initial reasonable guess (skip first-pass search)
//   virtual float	FitData_firstpass(float_Matrix& data_vals);
//   virtual float	SquaredDist(float_Matrix& data_vals);

//   virtual float	TestFit();	// #BUTTON test the fitting function
 
//   void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFitter);
};


class TA_API RetinalSpacingSpec : public taNBase {
  // defines the spacing of a filter relative to a specified retinal image size
INHERITED(taNBase)
public:
  TwoDCoord	retina_size;	// overall size of the retina
  TwoDCoord	border;		// border around retina that we don't process
  TwoDCoord	spacing;	// spacing between centers of filters in input
  TwoDCoord	input_size;	// #READ_ONLY #SHOW size of input region in pixels 
  TwoDCoord	output_size;	// #READ_ONLY #SHOW size of the filter output
  int		output_units;	// #READ_ONLY #SHOW total number of units to represent filter

  virtual void	UpdateSizes();
  // update the computed sizes

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinalSpacingSpec);
};

///////////////////////////////////////////////////////////////
// 	taImageProc

class TA_API taImageProc : public taOBase {
  // interactive environment for processing images
public:

  static bool	ReadImageGrey_float(float_Matrix& img_data, const String& fname);
  // reads image in greyscale from filename into float matrix: uses qt image reading functions -- if unsuccessful, returns false
  static bool	ReadImageRGB_float(float_Matrix& r_img, float_Matrix& g_img,
				   float_Matrix& b_img, const String& fname);
  // reads image in color from filename into separate float arrays for each color component: uses qt image reading functions -- if unsuccessful, returns false

  /*
  static bool	RenderBorder(float_Matrix& img_data);
  // make a uniform border around image -- makes off-screen stuff appear blank

  static bool	GetWeightedPixels(float coord, int size, int* pc, float* pw);
  // get pixel coordinates (pc[0], pc[1]) with norm weights (pw[0], [1]) for given floating coordinate coord

  static bool	ScaleImageData(float_Matrix& scaled_img, TwoDCoord& scaled_size, float_Matrix& orig_img, TwoDCoord& img_size, float scale);
  // change the size of the image
  static bool	RotateImageData(float_Matrix& rotated_img, float_Matrix& orig_img, TwoDCoord& img_size, float rotation);
  // rotate the image: rotation = 0-1 = 0-360 degrees

//   static bool	MakeEventSpecs();
//   // #MENU #MENU_ON_Generate #MENU_SEP_BEFORE make event specs based on current filters
//   static bool	PlotFilters();
//   // #MENU #NULL_OK plot the arrangement of the filters (centers) in the image event
  static bool	ScaleImage(float scale);
  // #MENU scale the current image by given amount
  static bool	RotateImage(float rotate);
  // #MENU scale the current image by given angle (1 = 360 degrees)

  static bool	DoGFilterImage(float_Matrix& flt_vals_on, float_Matrix& flt_vals_off,
			       float_Matrix& img_data,
			       TwoDCoord& img_size, FloatTwoDCoord& img_ctr_off,
			       FloatTwoDCoord& ret_ctr_off, bool add = false);
  // apply DoG filter to image data and return in flt_vals, with separate on and off-center values. img_ctr_off are normalized offsets from center of image, ret_ctr_off normalized offsets from center of retina, add = add values into the layers instead of equating

  static bool	DoGFilterImageBox(float_Matrix& img_data,
				  TwoDCoord& img_size, FloatTwoDCoord& img_ctr_off,
				  FloatTwoDCoord& ret_ctr_off, float scale);
  // Draw box in image data corresponding to paramters for FilterImage: img_ctr_off are normalized offsets from center of image, ret_ctr_off normalized offsets from center of retina




  static bool	FilterImage(Event* toev, float img_x_off=0, float img_y_off=0, float scale = 1.0f,
			    float rotate = 0.0f, float ret_x_off=0, float ret_y_off=0, bool add = false);
  // #MENU #FROM_GROUP_events #MENU_SEP_BEFORE filter current image into given event, with retina centered at given normalized offsets from center of image, scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified, add = add into filter values instead of setting
  static bool	FoveateImage(Event* toev, float box_ll_x, float box_ll_y, float box_ur_x, float box_ur_y,
			     float scale = 1.0f, float rotate = 0.0f,
			     float ret_x_off=0, float ret_y_off=0,
			     float img_x_off=0, float img_y_off=0, bool add = false, bool attend=false);
  // #MENU #FROM_GROUP_events filter current image into given event, with retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets
  static bool	FilterImageBox(float img_x_off=0, float img_y_off=0, float scale = 1.0f,
			       float rotate = 0.0f, float ret_x_off=0, float ret_y_off=0);
  // #MENU draw filter boxes on current image, with retina centered at given normalized offsets from center of image, scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified

  static bool	FoveateImageBox(float box_ll_x, float box_ll_y, float box_ur_x, float box_ur_y,
				float scale = 1.0f, float rotate = 0.0f,
				float ret_x_off=0, float ret_y_off=0,
				float img_x_off=0, float img_y_off=0);
  // #MENU draw filter boxes current image, with retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets
  */

  void 	Initialize();
  void	Destroy();
  TA_BASEFUNS(taImageProc);
};


#endif // ta_imgproc_h

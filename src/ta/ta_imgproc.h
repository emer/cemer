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

// image processing operations

#include "ta_math.h"
#include "ta_program.h"
#include "ta_geometry.h"
#include "ta_datatable.h"

#if defined(TA_GUI) && !defined(__MAKETA__)
# include <QImage>
#endif

class TA_API taImage : public taNBase {
  // ##CAT_Image represents an image in TA accessible way (wrapper for QT's QImage)
  INHERITED(taNBase)
public:
#ifdef TA_GUI
  QImage&  	GetImage() { return q_img; }
  // get the underlying qt image
  void  	SetImage(const QImage& img) { q_img = img; }
  // set the underlying qt image
#endif
  virtual bool	LoadImage(const String& fname);
  // load the image from given file name

  virtual float	GetPixelGrey_float(int x, int y);
  // get the pixel value as a greyscale for given coordinates
  virtual bool	GetPixelRGB_float(int x, int y, float& r, float& g, float& b);
  // get the pixel value as floating point RGB values for given coordinates

  virtual bool	ImageToGrey_float(float_Matrix& grey_data);
  // convert image to greyscale floating point image data: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  virtual bool	ImageToRGB_float(float_Matrix& rgb_data);
  // convert image to RGB floating point image data -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  virtual bool	ScaleImage(float sx, float sy, bool smooth=true);
  // scale image by given normalized scaling factors in each dimension
  virtual bool	RotateImage(float norm_deg, bool smooth=true);
  // rotate image by given normalized degrees (1 = 360deg)
  virtual bool	TranslateImage(float move_x, float move_y, bool smooth=true);
  // translate image by given normalized factors (-1 = all the way left, +1 = all the way right, etc)

  void 	Initialize();
  void	Destroy() { };
  void	Copy_(const taImage& cp);
  COPY_FUNS(taImage, inherited);
  TA_BASEFUNS(taImage);
protected:
#ifdef TA_GUI
  QImage	q_img;
#endif
};

class TA_API DoGFilterSpec : public taNBase {
  // #INLINE ##CAT_Image defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
  INHERITED(taNBase)
public:
  enum ColorChannel {		// indicator of which coor channel to filter on
    BLACK_WHITE,
    RED_GREEN,
    BLUE_YELLOW,
  };

  ColorChannel	color_chan;	// which color channel to operate on 
  int		filter_width;	// half-width of the filter (typically 2 * off_sigma)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * width + 1
  float		on_sigma;	// width of the narrower central 'on' gaussian
  float		off_sigma;	// width of the wider surround 'off' gaussian (typically 2 * on_sigma)
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of radius filter_width -- makes the filter more radially symmetric
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #SHOW on-gaussian 
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #SHOW off-gaussian (values are positive)
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #SHOW net overall filter (for display purposes)

  static float	GaussVal(float sqdist, float sig) {
    return 1.0f / (4.0f * taMath_float::pi * sig * sig) *
      taMath_float::exp(-sqdist / (2.0f * sig * sig)); }

  float		FilterPoint(int x, int y, float r_val, float g_val, float b_val);
  // apply filter at given x,y point to given color values

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
  // ##CAT_Image defines a gabor filter (gaussian times a plane sine wave) that highlights lines/edges in an image
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
  // ##CAT_Image fits a gabor filter from image data
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
  // #INLINE ##CAT_Image defines the spacing of a filter relative to a specified retinal image size
INHERITED(taNBase)
public:
  enum Region {
    FOVEA,
    PARAFOVEA,
    PERIPHERY,
  };

  Region	region;		// retinal region represented by this filter 
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

class TA_API DoGRetinaSpec : public taNBase {
  // ##CAT_Image specifies the spacing of Difference-of-Gaussian filters for the retina
INHERITED(taNBase)
public:
  DoGFilterSpec		dog;		// Difference of Gaussian retinal filters
  RetinalSpacingSpec	spacing;	// how to space DoG filters in the retina

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRetinaSpec);
};

class TA_API DoGRetinaSpecList : public taList<DoGRetinaSpec> {
  // ##CAT_Image a list of DoG retinal filters
INHERITED(taList<DoGRetinaSpec>)
public:

  TA_BASEFUNS(DoGRetinaSpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DoGRetinaSpec); }
  void 	Destroy()		{ };
};

///////////////////////////////////////////////////////////////
// 	taImageProc

class TA_API taImageProc : public taOBase {
  // ##CAT_Image basic image processing operations; images are either rgb = 3 dimensional matrix with 3rd dim = 3 or greyscale (2 dim matrix)
public:

  static bool	RenderBorder_float(float_Matrix& img_data);
  // make a uniform border 1 pixel wide around image, containing average value for that border region in original image: this value is what gets filled in when image is translated "off screen"

  static void	GetWeightedPixels_float(float coord, int size, int* pc, float* pw);
  // #IGNORE helper function: get pixel coordinates (pc[0], pc[1]) with norm weights (pw[0], [1]) for given floating coordinate coord

  static bool   TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
				   float move_x, float move_y);
  // move (translate) image by normalized move_x, move_y factors: 1 = center of image moves to right/top edge, -1 center moves to bottom/left
  static bool	RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img, float rotation);
  // rotate the image: rotation = normalized 0-1 = 0-360 degrees 
  static bool	ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img, float scale);
  // change the size of the image by normalized scaling factor (either rgb=3 dim or grey=2 dim)
  static bool   CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				int crop_width, int crop_height);
  // crop image to given size (-1 = use original image size), centered on the center of the image; border color of original image is used to fill in missing values

  static bool	TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				     float move_x=0.0, float move_y=0.0, float rotate=0.0,
				     float scale=1, int crop_width=-1, int crop_height=-1);
  // Transform an image by translation, rotation, scaling, and cropping, as determined by parameters (calls above functions; only if needed; if crop < 0 then no cropping); does RenderBorder for each step to preserve uniform background color
  
  static bool	DoGFilterRetina(float_Matrix& on_output, float_Matrix& off_output,
				float_Matrix& retina_img, DoGRetinaSpec& spec,
				bool superimpose = false);
  // apply DoG filter to input image, result in output (on = + vals, off = - vals). superimpose = add values into the outputs instead of overwriting

  static bool	AttentionFilter(float_Matrix& mat, float radius_pct);
  // apply an "attentional" filter to the matrix data: outside of radius, values are attenuated in proportion of squared distance outside of radius (r_sq / dist_sq) -- radius_pct is normalized proportion of maximum half-size of image (e.g., 1 = attention bubble extends to furthest edge of image; only corners are attenuated)

  void 	Initialize();
  void	Destroy();
  TA_BASEFUNS(taImageProc);
};

/////////////////////////////////////////////////////////
//   special objects that specify full set of image processing operations
/////////////////////////////////////////////////////////

class TA_API RetinaSpec : public taNBase {
  // ##CAT_Image full specification of retinal filtering based on difference-of-gaussian filters
INHERITED(taNBase)
public:
  enum ColorType {		// type of color processing to do (determines file loading)
    MONOCHROME,
    COLOR,
  };

  ColorType		color_type;	// type of color processing (determines file processing
  TwoDCoord		retina_size; 	// overall size of retina (auto copied to retina specs)
  DoGRetinaSpecList	dogs;		// the difference-of-gaussian retinal filters

  virtual void	DefaultFilters();
  // #BUTTON create a set of default filters

  virtual void	ConfigDataTable(DataTable* dt, bool new_cols = false);
  // #BUTTON configure a data table to hold all of the image data (if new_cols, reset any existing cols in data table before adding new ones)

  ///////////////////////////////////////////////////////////////////////
  // Basic filtering function: transforms image and then applies dog filters

  virtual bool	FilterImageData(float_Matrix& img_data, DataTable* dt,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false);
  // filter image data into given datatable, with retina centered at given normalized offsets from center of image, scaled by given factor (zoom), rotated by normalized units (1=360deg), superimpose = merge into filter values into last row of table; otherwise new row is added

  virtual bool	FilterImage(taImage& img, DataTable* dt,
			    float move_x=0, float move_y=0,
			    float scale = 1.0f, float rotate = 0.0f,
			    bool superimpose = false);
  // filter image into given datatable, with retina centered at given normalized offsets from center of image, scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified, superimpose = merge into filter values into last row of table; otherwise new row is added

  virtual bool	FilterImageName(const String& img_fname, DataTable* dt,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false);
  // #BUTTON load image from file and filter into given datatable, with retina centered at given normalized offsets from center of image, scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified, superimpose = merge into filter values into last row of table; otherwise new row is added

  ///////////////////////////////////////////////////////////////////////
  // Automatic foveation of an image based on a bounding box

  virtual DoGRetinaSpec* FindRetinalRegion(RetinalSpacingSpec::Region reg);
  // utility function to find spec that corresponds to fovea (i.e., smallest input_size)

  virtual bool	AttendRegion(DataTable* dt, RetinalSpacingSpec::Region region = RetinalSpacingSpec::FOVEA);
  // apply attentional weighting filter to filtered values, with radius = given region

  virtual bool	LookAtImageData(float_Matrix& img_data, DataTable* dt,
				RetinalSpacingSpec::Region region,
				float box_ll_x, float box_ll_y,
				float box_ur_x, float box_ur_y,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false, bool attend=false);
  // filter image data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets; attend = apply attentional weighting filter
  virtual bool	LookAtImage(taImage& img, DataTable* dt,
			    RetinalSpacingSpec::Region region,
			    float box_ll_x, float box_ll_y,
			    float box_ur_x, float box_ur_y,
			    float move_x=0, float move_y=0,
			    float scale = 1.0f, float rotate = 0.0f,
			    bool superimpose = false, bool attend=false);
  // filter image data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets; attend = apply attentional weighting filter
  virtual bool	LookAtImageName(const String& img_fname, DataTable* dt,
				RetinalSpacingSpec::Region region,
				float box_ll_x, float box_ll_y,
				float box_ur_x, float box_ur_y,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false, bool attend=false);
  // #BUTTON load image from file and filter into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets; attend = apply attentional weighting filter

  // todo: need a checkconfig here..

  void	UpdateRetinaSize();	// copy retina_size to dogs..

  void	UpdateAfterEdit();
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinaSpec);
};

/////////////////////////////////////////////////////////
//   programs to support image processing operations
/////////////////////////////////////////////////////////

class TA_API ImageProcCall : public StaticMethodCall { 
  // call a taImageProc function
INHERITED(StaticMethodCall)
public:
  TA_BASEFUNS(ImageProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ta_imgproc_h

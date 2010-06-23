// Copyright, 1995-2007, Regents of the University of Colorado,
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

#ifndef ta_imgproc_h
#define ta_imgproc_h

// image processing operations

// there are multiple levels of filtering and image processing, ordered in this file as:
// -- taImage, Canvas -- basic data and drawing on images
// -- basic image filtering: DoG, Gabor, etc
// -- taImgProc -- static container class with various image processing operations
// -- specific parameterizations for DoG and V1 levels of image processing, representing a
//	region of the retina (DoGRegionSpec, V1RegionSpec)
// -- an overall image processing object containing multiple regions, providing full
//	final coverage of the image processing from program user perspective:
//	RetinaProc, V1Proc

#include "ta_math.h"
#include "ta_program.h"
#include "ta_geometry.h"
#include "ta_datatable.h"

#if defined(TA_GUI) && !defined(__MAKETA__)
# include <QImage>
# include <QPainter>
# include <QPainterPath>
#endif

class QPainterPath;	// #IGNORE
class QBrush;		// #IGNORE

#ifdef TA_OS_WIN
# ifdef LoadImage
#   undef LoadImage
# endif
#endif

class TA_API taImage : public taNBase {
  // #STEM_BASE ##CAT_Image represents an image in TA accessible way (wrapper for QT's QImage)
  INHERITED(taNBase)
public:
#ifdef TA_GUI
  QImage&  	GetImage() { return q_img; }
  // get the underlying qt image
  void  	SetImage(const QImage& img) { q_img = img; }
  // #CAT_Image set the underlying qt image
#endif
  virtual bool	LoadImage(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_LOAD #EXT_png,jpg load the image from given file name (leave file name blank to get a chooser)
  virtual bool	SaveImage(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_SAVE #EXT_png,jpg save the image to given file name (leave file name blank to get a chooser) -- uses extension to determine format

  virtual float	GetPixelGrey_float(int x, int y);
  // #CAT_Image get the pixel value as a greyscale for given coordinates
  virtual bool	GetPixelRGB_float(int x, int y, float& r, float& g, float& b);
  // #CAT_Image get the pixel value as floating point RGB values for given coordinates

  bool	ImageToMatrix_grey(float_Matrix& grey_data);
  // #CAT_Data convert image to greyscale Matrix floating point image data (alias for ImageToGrey_float): note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  bool	ImageToMatrix_rgb(float_Matrix& rgb_data);
  // #CAT_Data convert image to RGB Matrix floating point image data (alias for ImageToMatrix_rgb) -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  bool	ImageFromMatrix_grey(const float_Matrix& grey_data);
  // #CAT_Data convert from greyscale Matrix floating point image data to this image: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  bool	ImageFromMatrix_rgb(const float_Matrix& rgb_data);
  // #CAT_Data convert from RGB Matrix floating point image data to this image -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  virtual bool	ImageToDataCell(DataTable* dt, Variant col, int row);
  // #BUTTON #CAT_Data set image to datatable cell indexed by col (name or number) and row -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ImageFromDataCell(DataTable* dt, Variant col, int row);
  // #BUTTON #CAT_Data set image from data in datatable cell indexed by col (name or number) and row -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ConfigDataColName(DataTable* dt, const String& col_nm, ValType val_type, 
				  bool rgb = true);
  // #CAT_Data configure data column with given name (if it doesn't exist, it is created) to represent current image -- if rgb is false, then a greyscale image is configured (2d)

  virtual bool	ScaleImage(float sx, float sy, bool smooth=true);
  // #BUTTON #CAT_Image scale image by given normalized scaling factors in each dimension
  virtual bool	RotateImage(float norm_deg, bool smooth=true);
  // #BUTTON #CAT_Image rotate image by given normalized degrees (1 = 360deg)
  virtual bool	TranslateImage(float move_x, float move_y, bool smooth=true);
  // #BUTTON #CAT_Image translate image by given normalized factors (-1 = all the way left, +1 = all the way right, etc)

  virtual bool	GetImageSize(int& width, int& height);
  // #CAT_Image get size of current image
  virtual bool	SetImageSize(int width, int height);
  // #BUTTON #CAT_Image set size of current image -- if currently same size, then returns false and nothing happens; otherwise, a new image data structure of given size is created, using ARGB32 format


  ////////////////////////////////////
  // Obsolete Interfaces: do not use:
  
  virtual bool	ImageToDataCellName(DataTable* dt, const String& col_nm, int row);
  // #CAT_zzzObsolete set image to datatable cell indexed by col name and row number -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ImageToGrey_float(float_Matrix& grey_data) { return ImageToMatrix_grey(grey_data); }
  // #CAT_zzzObsolete convert image to greyscale floating point image data: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  virtual bool	ImageToRGB_float(float_Matrix& rgb_data)  { return ImageToMatrix_rgb(rgb_data); }
  // #CAT_zzzObsolete convert image to RGB floating point image data -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..


  void	Copy_(const taImage& cp);
  TA_BASEFUNS(taImage);
protected:
#ifdef TA_GUI
  QImage	q_img;
#endif
private:
  void 	Initialize();
  void	Destroy() { };
};

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

/////////////////////////////////////////////////////////////
//		Threading code

#include "ta_thread.h"

class ImgProcThreadBase;

// this is the standard function call taking the thread number int value
// all threaded functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* ThreadImgProcCall;
#else
typedef taTaskMethCall2<ImgProcThreadBase, void, int, int> ThreadImgProcCall;
typedef void (ImgProcThreadBase::*ThreadImgProcMethod)(int, int);
#endif

class ImgProcCallThreadMgr;

class TA_API ImgProcCallTask : public taTask {
INHERITED(taTask)
public:
  ThreadImgProcCall* img_proc_call;	// method to call on the object

  override void run();
  // runs specified chunk of computation

  ImgProcCallThreadMgr* mgr() { return (ImgProcCallThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(ImgProcCallTask);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ImgProcCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for ImgProcCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  int		min_units;	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of computational units of work required to use threads at all -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  int		nibble_chunk;	// #MIN_1 #DEF_8 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.

  QAtomicInt	nibble_i;	// #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int		n_cmp_units;	// #IGNORE number of compute units to perform -- max of the nibbling..

  ImgProcThreadBase*	img_proc() 	{ return (ImgProcThreadBase*)owner; }

  override void		Run(ThreadImgProcCall* img_proc_call, int n_cmp_un);
  // #IGNORE run given function, splitting n_cmp_units computational units evenly across the available threads
  
  TA_BASEFUNS_NOCOPY(ImgProcCallThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ImgProcThreadBase : public taNBase {
  // #VIRT_BASE ##CAT_Image base class for image-processing code that uses threading -- defines a basic interface for thread calls to deploy filtering or other intensive computations
  INHERITED(taNBase)
public:
  ImgProcCallThreadMgr threads; // #CAT_Threads parallel threading of image processing computation

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(ImgProcThreadBase);
};

////////////////////////////////////////////////////////////////////
//		Basic Filters: DoG, Gabor...

class TA_API DoGFilter : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Image defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
  INHERITED(taNBase)
public:
  enum ColorChannel {		// indicator of which color channel to filter on
    LUMINANCE,			// just raw luminance (monochrome / black white)
    RED_CYAN,			// red vs. G + B = cyan
    GREEN_MAGENTA,		// green vs. R + B = magenta
    BLUE_YELLOW,		// blue vs. R + G = yellow
    YELLOW_BLUE,		// yellow vs. blue
  };

  int		filter_width;	// half-width of the filter (typically 2 * off_sigma)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * width + 1
  float		on_sigma;	// width of the narrower central 'on' gaussian
  float		off_sigma;	// width of the wider surround 'off' gaussian (typically 2 * on_sigma)
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of radius filter_width -- makes the filter more radially symmetric
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #NO_COPY on-gaussian 
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #NO_COPY off-gaussian (values are positive)
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #NO_COPY net overall filter (for display purposes)

  float		FilterPoint_rgb(int x, int y, ColorChannel color, 
				float r_val, float g_val, float b_val);
  // #CAT_DoGFilter apply filter at given x,y point given color values and color channel

  float		FilterPoint_grey(int x, int y, float grey) {
    return grey * net_filter.FastEl(x+filter_width, y+filter_width);
  }
  // #CAT_DoGFilter apply filter at given x,y point given greyscale value, only for LUMINANCE channel by definition

  virtual void	RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt, float_Matrix& net_flt);
  // #CAT_DoGFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_DoGFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGFilter);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API GaborFilter : public taNBase {
  // #STEM_BASE ##CAT_Image defines a gabor filter (gaussian times a plane sine wave) that highlights lines/edges in an image
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
  float_Matrix	filter;		// #READ_ONLY #NO_SAVE #NO_COPY our filter

  virtual float	Eval(float x, float y);
  // #CAT_GaborFilter evaluate gabor function for given coordinates
  virtual void	RenderFilter(float_Matrix& flt);
  // #CAT_GaborFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_GaborFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual float	GetParam(GaborParam param);
  // #CAT_GaborFilter get particular parameter value

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a graph from it
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a grid view of it (reset an existing data first)

  virtual void	OutputParams(ostream& strm = cerr);
  // #CAT_GaborFilter output current parameter values to stream

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFilter);
};


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

  virtual void	OutputParams(ostream& strm = cerr);
  // #CAT_GaborFilter output current parameter values to stream

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(MotionGaborFilter);
};

class TA_API GaborFitter : public GaborFilter {
  // ##CAT_Image fits a gabor filter from image data
INHERITED(GaborFilter)
public:
  float		fit_dist;	// #READ_ONLY #SHOW fit distance

  virtual float ParamDist(const GaborFilter& oth);
  // #CAT_GaborFilter return euclidian distance between parameters for this spec and the other one

//   virtual float	FitData(float_Matrix& data_vals, bool use_cur_vals = false);
  // find best-fitting parameters for given data.  use_cur_vals = use current values as initial reasonable guess (skip first-pass search)
//   virtual float	FitData_firstpass(float_Matrix& data_vals);
//   virtual float	SquaredDist(float_Matrix& data_vals);

//   virtual float	TestFit();	// #BUTTON test the fitting function
 
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFitter);
};


///////////////////////////////////////////////////////////////
// 	taImageProc

class TA_API taImageProc : public taNBase {
  // #STEM_BASE ##CAT_Image basic image processing operations; images are either rgb = 3 dimensional matrix with 3rd dim = 3 or greyscale (2 dim matrix)
  INHERITED(taNBase)
public:

  enum EdgeMode {		// how to deal with image edges
    CLIP,			// just hard clip edges, nothing fancy
    BORDER,			// render/preserve a 1 pixel border around everything
    WRAP,			// wrap the image around to the other side: no edges!
  };

  static bool	RenderBorder_float(float_Matrix& img_data);
  // #CAT_Render make a uniform border 1 pixel wide around image, containing average value for that border region in original image: this value is what gets filled in when image is translated "off screen"

  static bool	FadeEdgesToBorder_float(float_Matrix& img_data, int fade_width = 8);
  // #CAT_Render fade edges of image uniformly to the border color, over fade_width pixels

  static bool	RenderOccluderBorderColor_float(float_Matrix& img_data,
						float llx, float lly, float urx, float ury);
  // #CAT_Render render an occluder rectangle of given normalized size (ll = lower left corner (0,0 = farthest ll), ur = upper right (1,1 = farthest ur) using the border color

  static void	GetWeightedPixels_float(float coord, int size, int* pc, float* pw);
  // #IGNORE helper function: get pixel coordinates (pc[0], pc[1]) with norm weights (pw[0], [1]) for given floating coordinate coord

  static bool   TranslateImagePix_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
					int move_x, int move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by pixel move_x, move_y factors
  static bool   TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
				   float move_x, float move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by normalized move_x, move_y factors: 1 = center of image moves to right/top edge, -1 center moves to bottom/left
  static bool	RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img,
				  float rotation, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform rotate the image: rotation = normalized 0-1 = 0-360 degrees 
  static bool	ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img, float scale,
				 EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform change the size of the image by normalized scaling factor (either rgb=3 dim or grey=2 dim)
  static bool   CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				int crop_width, int crop_height, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform crop image to given size (-1 = use original image size), centered on the center of the image; border color of original image is used to fill in missing values

  static bool	TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				     float move_x=0.0f, float move_y=0.0f, float rotate=0.0f,
				     float scale=1.0f, int crop_width=-1, int crop_height=-1,
				     EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Transform an image by translation, rotation, scaling, and cropping, as determined by parameters (calls above functions; only if needed; if crop < 0 then no cropping); does RenderBorder for each step to preserve uniform background color
  static bool	SampleImageWindow_float(float_Matrix& out_img, float_Matrix& in_img,
					int win_width=320, int win_height=320,
					float ctr_x=0.5f, float ctr_y=0.5f,
					float rotate=0.0f, float scale=1.0f,
					EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Sample a window of given width and height centered on given normalized coordinate location in input image, with scaling and rotation (in normalized 0-1 units) as specified
  
  static bool	AttentionFilter(float_Matrix& mat, float radius_pct);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply an "attentional" filter to the matrix data: outside of radius, values are attenuated in proportion of squared distance outside of radius (r_sq / dist_sq) -- radius_pct is normalized proportion of maximum half-size of image (e.g., 1 = attention bubble extends to furthest edge of image; only corners are attenuated)

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  TA_BASEFUNS_NOCOPY(taImageProc);
private:
  void 	Initialize();
  void	Destroy();
};


////////////////////////////////////////////////////////////////////////////
//	Collections of filters and parameters for applying to images

////////////////////////////////////////////////////////////////////
//		Retinal Processing (DoG model)

class TA_API DoGRegionSpec : public ImgProcThreadBase {
  // #STEM_BASE ##CAT_Image specifies a region of Difference-of-Gaussian retinal filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs -- each region is a separate matrix column in a data table (and network layer), and has a specified spatial resolution
INHERITED(ImgProcThreadBase)
public:
  enum Ocularity {		// ocularity configuration
    MONOCULAR,			// monocular -- only one eye
    BINOCULAR,			// binocular -- both eyes
  };
  enum Eye {
    LEFT,			
    RIGHT,
  };
  enum Region {			// retinal region
    FOVEA,
    PARAFOVEA,
    PERIPHERY,
  };
  enum Resolution {		// level of resolution
    HI_RES,
    MED_RES,
    LOW_RES,
    VLOW_RES,
  };
  enum Color {			// color processing
    MONOCHROME,			// just luminance on/off
    COLOR,			// has luminance on/off plus 4 color contrasts: R, G, B, Y vs. other two colors
  };
  enum EdgeMode {		// how to deal with edges in the filter inputs
    CLIP,			// hard clip edges -- attempts to ensure that no clipping is necessary by making filters fit within inputs, but image input is clipped
    WRAP,			// wrap the image and any subsequent stages of filtering around the edges -- no edges!
  };
  enum RenormMode {		// how to renormalize output of filters
    NO_RENORM,			// do not renormalize
    LIN_RENORM,			// linear renormalization -- divide all filter output by max value
    LOG_RENORM,			// log renormalize -- take the log of all filter output with max = 1 multiplier -- this tends to work the best
  };
  enum DataSave {		// #BITS how to save data to the data table (computation happens on internal table)
    NO_SAVE 	= 0,	 	// #NO_BIT don't save data for this component
    SAVE_DATA	= 0x0001, 	// save data to the data table, by default with all features in subgroups within one matrix (unless SEP_MATRIX is checked)
    SEP_MATRIX	= 0x0002,	// each feature is saved in a separate 2d map, instead of a common subgroup within a single matrix
    ONLY_GUI	= 0x0004,	// only save when the gui is active -- for data that is for debugging and visualization purposes only, not presented to a network etc
  };
  enum SaveMode {		// how to add new data to the data table
    NONE_SAVE,			// don't save anything at all -- overrides any more specific save guys and prevents any addition or modification to the data table
    FIRST_ROW,			// always overwrite the first row -- does EnforceRows(1) if rows = 0
    ADD_ROW,			// always add a new row and write to that, preserving a history of inputs over time -- should be reset at some interval!
  };

  enum OutMatIdx {		// matrix indexes for matrix output storage
    FEAT_X,			// feature sub-group x
    FEAT_Y,			// feature sub-group y
    IMG_X,			// filters for image, x
    IMG_Y,			// filters for image, y
    TIME,			// time is out index
  };

  DataTableRef	data_table;	// data table for saving filter results for viewing and applying to networks etc
  SaveMode	save_mode;	// how to add new data to the data table
  DataSave	image_save;	// how to save the input image(s) for each filtering step
  Ocularity	ocularity;	// whether two eyes or only one is present
  Region	region;		// retinal region represented by this filter 
  Resolution	res;		// level of resolution represented by this filter (can use enum or any other arbitrary rating scale -- just for informational/matcing purposes)
  Color		color;		// what level of color information to process
  int		motion_frames;	// #MIN_0 how many frames of image information are to be retained for extracting motion signals -- 0 = no motion, 3 = typical for motion
  TwoDCoord	retina_size;	// overall size of the retina -- defines size of images that are processed by these filters -- scaling etc typically used to fit image to retina size
  TwoDCoord	border;		// border around retina that we don't process -- typically a 1 pixel background color border is retained in the input image processing, so this is subtracted -- if not using WRAP mode, then also ensure that this is >= than 1/2 of the width of the wide DoG
  TwoDCoord	input_size;	// #READ_ONLY #SHOW size of input region in pixels that is actually filtered -- retina_size - 2 * border
  EdgeMode	edge_mode;	// how to deal with image edges, and edges in general throughout the processing cascade
  DoGFilter	dog_specs;	// Difference of Gaussian retinal filter specification
  TwoDCoord	dog_spacing;	// spacing between centers of DoG filters in input
  RenormMode	dog_renorm;	// #DEF_LOG_RENORM how to renormalize the output of filters
  float		renorm_thr;	// #DEF_1e-05 threshold for the max filter output value to max-renormalize filter outputs such that the max is 1 -- below this value, consider the input to be blank and do not renorm
  DataSave	dog_save;	// how to save the DoG outputs for the current time step in the data table
  XYNGeom	dog_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for DoG filtering -- 2 total for monochrome, 6 total for color -- x must be 2, so either 2x1 or 2x3
  TwoDCoord	dog_img_geom; 	// #READ_ONLY #SHOW size of dog-filtered image output -- number of hypercolumns in each axis to cover entire output -- this is completely determined by retina_size, border and dog_spacing parameters

  CircMatrix	dog_circ_r; 	// #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time
  CircMatrix	dog_circ_l; 	// #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time

  float_Matrix	dog_out_r;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the right eye -- [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames
  float_Matrix	dog_out_l;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the left eye -- [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames

  virtual String GetDoGFiltName(int filt_no);
  // get name for each filter channel (0-5) = on;off;rvc;gvm;bvy;yvb

  virtual bool 	Init();
  // #BUTON initialize everything to be ready to start filtering -- calls InitFilters, InitOutMatrix, InitDataTable

  virtual bool	FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image = NULL);
  // main interface: filter input image(s) (if ocularity = BINOCULAR, must pass both images, else left eye is ignored) -- saves results in local output vectors, and data table if specified -- increments the time index if motion filtering

  virtual bool	InvertFilters(float_Matrix* right_eye_image, float_Matrix* left_eye_image = NULL);
  // filter input image(s) (if ocularity = BINOCULAR, must pass both images, else left eye is ignored) -- saves results in local output vectors, and data table if specified -- increments the time index if motion filtering

  virtual void	GraphDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter difference-of-gaussians into data table and generate a graph
  virtual void	GridDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter difference-of-gaussians into data table and generate a grid view

  virtual void	PlotDoGSpacing(DataTable* disp_data, float val = 1.0f);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  // todo: add CheckConfig!!

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRegionSpec);
protected:
  void	UpdateAfterEdit_impl();

  virtual void UpdateGeom();
  // update all geometry info -- called by UAE

  virtual bool InitFilters();
  // initialize the filters -- overload for derived types and call parent
  virtual bool InitOutMatrix();
  // initialize data output matrcies to fit output of filters
  virtual bool InitDataTable();
  // initialize data table to fit data saving as configured

  virtual bool FilterImage_impl();
  // implementation of filtering -- assumes cur_img_x args are set and everything is checked
  virtual void IncrTime();
  // increment time one step -- move the CircMatrix indexes
  virtual bool DoGFilterImage(float_Matrix* image, float_Matrix* out, CircMatrix* circ);
  // implementation of DoG filtering for a given image and output, circ index -- manages threaded calls to _thread version
  virtual void DoGFilterImage_thread(int dog_idx, int thread_no);
  // threaded routine for actually filtering given index of dog
  virtual bool RenormOutput_Frames(RenormMode mode, float_Matrix* out, CircMatrix* circ);
  // renormalize output of filters after filtering -- for output having motion frames
  virtual bool RenormOutput_NoFrames(RenormMode mode, float_Matrix* out);
  // renormalize output of filters after filtering -- for output without motion frames

  virtual bool ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
				     float_Matrix* left_eye_image = NULL);
  // send current input image(s)e step of dog output to data table for viewing
    virtual bool ImageToTable_impl(DataTable* dtab, float_Matrix* img, const String& col_sufx);
    // send current input image(s)e step of dog output to data table for viewing
  virtual bool DoGOutputToTable(DataTable* dtab);
  // send current time step of dog output to data table for viewing
    virtual bool DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
				       const String& col_sufx);
    // send current time step of dog output to data table for viewing

  virtual bool	DoGInvertFilter(float_Matrix* image, float_Matrix* out);
  // implementation of DoG filtering that actually does the heavy lifting

  // cache of args for current function call
  float_Matrix* cur_img_r;	// cur right eye image arg -- only valid during filter call
  float_Matrix* cur_img_l;	// cur left eye image arg -- only valid during filter call
  float_Matrix* cur_img;	// cur image -- only valid during filter call
  float_Matrix* cur_out;	// cur output buffer -- only valid during filter call
  CircMatrix*	cur_circ;	// current circular buffer index
  bool		rgb_img;	// is current image rgb?
  bool		wrap;		// whether edge_mode == WRAP
};


class TA_API DoGRegionSpecList : public taList<DoGRegionSpec> {
  // ##CAT_Image a list of DoG retinal filters
INHERITED(taList<DoGRegionSpec>)
public:

  virtual DoGRegionSpec* FindRetinalRegion(DoGRegionSpec::Region reg);
  // find first spec with given retinal region
  virtual DoGRegionSpec* FindRetinalRes(DoGRegionSpec::Resolution res);
  // find first spec with given resolution
  virtual DoGRegionSpec* FindRetinalRegionRes(DoGRegionSpec::Region reg,
					      DoGRegionSpec::Resolution res);
  // find first spec with given retinal region and resolution (falls back to res then reg if no perfect match)

  TA_BASEFUNS_NOCOPY(DoGRegionSpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DoGRegionSpec); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////
//		V1 Processing -- basic RF's

class TA_API V1SimpleSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 simple cells
INHERITED(taOBase)
public:
  int		n_angles;	// #DEF_4 number of different angles encoded -- currently only 4 is supported
  int		rf_size;	// #DEF_4 number of DoG filters to integrate over to form a line - this also determines the number of stacks to integrate over to make a square RF -- currently only 4 is supported 
  int		rf_half;	// #READ_ONLY half rf
  int		spacing;	// #DEF_4 spacing between neighboring V1S rf's -- should be either 1/2 of rf_size or full rf_size (tiling) -- tradeoff between number of filters (4x overall) vs. discrete edge aliasing problems 
  int		border;		// #READ_ONLY #SHOW border onto dog filters -- automatically computed based on wrap mode and spacing setting
  float		rf_norm;	// #READ_ONLY 1 / rf_size -- normalization factor

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1SimpleSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1MotionSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 motion coding by simple cells
INHERITED(taOBase)
public:
  int		n_speeds;	// #DEF_1 for motion coding, number of speeds in each direction to encode separately -- speeds are 1, 2, 4, 8, etc and extra_width is proportional to speed -- only applicable if motion_frames > 1
  int		extra_width;	// #DEF_1 additional width of encoding around the trajectory for the target speed -- allows for some fuzziness in encoding -- effective value is multiplied by speed, so it gets fuzzier as speed gets higher
  float		gauss_sig;	// #DEF_0.5 gaussian sigma for weighting the contribution of extra width guys -- normalized by effective extra_width

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1MotionSpec);
};

class TA_API V1BinocularSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 binocular cells
INHERITED(taOBase)
public:
  int		n_disps;	// #DEF_1 number of different disparities encoded in each direction away from the focal plane (e.g., 1 = 1 near and 1 far)
  int		disp_off;	// #DEF_2 offset from corresponding location for each disparity step
  int		extra_width;	// #DEF_1 additional width of encoding around the target disparity -- allows for some fuzziness in encoding
  float		gauss_sig;	// #DEF_0.5 gaussian sigma for weighting the contribution of extra width guys -- normalized by extra_width

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1BinocularSpec);
};

class TA_API V1ComplexSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 binocular cells
INHERITED(taOBase)
public:
  enum ComplexFilters { // #BITS flags for specifying which complex filters to include
    CF_NONE	= 0, // #NO_BIT
    END_STOP	= 0x0001, // end stop cells
    LEN_SUM	= 0x0002, // length summing cells
    DISP_EDGE	= 0x0004, // respond to an edge in disparity, integrating over all other simple cell tunings (orientation, polarity etc) -- only applicable if BINOCULAR ocularity
    MOTION_EDGE	= 0x0008, // respond to an edge in motion, integrating over all other simple cell tunings (orientation, polarity etc) -- only applicable if motion_frames > 1
#ifndef __MAKETA__
    CF_BASIC	= END_STOP | LEN_SUM, // #IGNORE #NO_BIT just the basic ones
    CF_EDGES	= DISP_EDGE | MOTION_EDGE, // #IGNORE #NO_BIT special complex edges
    CF_ALL	= END_STOP | LEN_SUM | DISP_EDGE | MOTION_EDGE, // #IGNORE #NO_BIT all complex filters
#endif
  };

  ComplexFilters filters; 	// which complex cell filtering to perform
  TwoDCoord	spat_rf;	// integrate over this many spatial locations (uses MAX operator over gaussian weighted filter matches at each location) in computing the response of the v1c cells -- produces a larger receptive field
  TwoDCoord	spat_rf_half;	// #READ_ONLY half rf
  TwoDCoord	spacing;	// how to space out the centers of the complex rfields -- typically 1/2 overlap with spat_rf
  TwoDCoord	border;		// #READ_ONLY #SHOW border onto v1s filters -- automatically computed based on wrap mode and spacing setting
  float		gauss_sig;	// #DEF_0.5 gaussian sigma for spatial rf -- weights the contribution of more distant locations more weakly
  float		focal_wt;	// #DEF_2 how much stronger should the focal disparity be relative to the others?  only for binocular case
  float		focal_wt_eff;	// #READ_ONLY normalized
  float		nonfocal_wt_eff; // #READ_ONLY normalized

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1ComplexSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1RegionSpec : public DoGRegionSpec {
  // #STEM_BASE ##CAT_Image specifies a region of V1 simple and complex filters -- used as part of overall V1Proc processing object -- takes retinal DoG filter inputs and produces filter activation outputs -- each region is a separate matrix column in a data table (and network layer), and has a specified spatial resolution
INHERITED(DoGRegionSpec)
public:
  enum V1Filters { // different stages of V1 filters
    V1_SIMPLE,	   // V1 Simple oriented, polarity sensitive cells
    V1_BINOCULAR,  // V1 Binocular integration 
    V1_COMPLEX,	   // V1 Complex cells
  };

  V1SimpleSpec	v1s_specs;	// specs for V1 simple filters -- first step after DoG -- encode simple oriented edges in same polarities/colors as in DoG layer, plus motion if applicable
  V1MotionSpec	v1s_motion;	// #CONDSHOW_OFF_motion_frames:0||1 specs for V1 motion filters within the simple processing layer
  RenormMode	v1s_renorm;	// #DEF_NO_RENORM how to renormalize the output of v1s static filters
  RenormMode	v1m_renorm;	// #DEF_NO_RENORM how to renormalize the output of v1s motion filters
  DataSave	v1s_save;	// how to save the V1 simple outputs for the current time step in the data table
  XYNGeom	v1s_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 simple filtering -- n_angles * 2 or 6 polarities (monochrome|color) + 2 polarities * 2 directions * n_speeds * n_angles -- configured automatically with x = n_angles
  XYNGeom	v1s_img_geom; 	// #READ_ONLY #SHOW size of v1 simple filtered image output -- number of hypercolumns in each axis to cover entire output -- this is determined by dog_img_geom, rf_size and half_ovlp setting

  V1BinocularSpec v1b_specs;	// #CONDSHOW_ON_ocularity:BINOCULAR specs for V1 binocular filters -- comes after V1 simple processing in binocular case
  DataSave	v1b_save;	// #CONDSHOW_ON_ocularity:BINOCULAR how to save the V1 binocular outputs for the current time step in the data table
  XYNGeom	v1b_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 binocular -- 3x v1s_feat_geom -- focus, near, far

  V1ComplexSpec v1c_specs;	// specs for V1 complex filters -- comes after V1 binocular processing 
  DataSave	v1c_save;	// how to save the V1 complex outputs for the current time step in the data table
  XYNGeom	v1c_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 complex filtering -- configured automatically with x = n_angles
  XYNGeom	v1c_img_geom; 	// #READ_ONLY #SHOW size of v1 complex filtered image output -- number of hypercolumns in each axis to cover entire output -- this is determined by ..
  
  int_Matrix	v1s_stencils; 	// #READ_ONLY #NO_SAVE stencils for simple cells [x,y][1 line: rf_size][n lines: rf_size][angles]
  float_Matrix	v1m_weights;  	// #READ_ONLY #NO_SAVE v1 simple motion weighting factors (1d)
  float_Matrix	v1b_weights;	// #READ_ONLY #NO_SAVE v1 binocular weighting factors (1d)
  float_Matrix	v1c_weights;	// #READ_ONLY #NO_SAVE v1 complex spatial weighting factors (2d)
  int_Matrix	v1c_stencils; 	// #READ_ONLY #NO_SAVE stencils for complex cells [x,y][3 (line_len)][angles]

  float_Matrix	v1s_out_l;	 // #READ_ONLY #NO_SAVE v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  float_Matrix	v1s_out_r;	 // #READ_ONLY #NO_SAVE v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  CircMatrix	v1s_circ_r;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time
  CircMatrix	v1s_circ_l;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time
  float_Matrix	v1b_out;	 // #READ_ONLY #NO_SAVE v1 binocular output [feat.x][feat.y][img.x][img.y] -- only present for BINOCULAR ocularity
  float_Matrix	v1c_out;	 // #READ_ONLY #NO_SAVE v1 complex output [feat.x][feat.y][img.x][img.y]

  virtual void	GridFilterInput(DataTable* disp_data, int unit_no=0, int gp_skip=2, bool ctrs_only=false);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter for a given unit as it will be applied to the entire input data -- gp_skip specifies the number of unit groups to increment -- 2 is good to avoid complete overlap -- ctrs_only will only plot the centers of the filters, not the actual raw filters themselves
  virtual void	GraphV1Filter(DataTable* disp_data, V1Filters filter, int unit_no);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the selected V1 filter into data table and generate a graph view
  virtual void	GridV1Filter(DataTable* disp_data, V1Filters filter);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the selected V1 filter into data table and generate a grid view

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1RegionSpec);
protected:
  float_Matrix*	cur_dog;
  CircMatrix*	cur_dog_circ;
  int		mot_feat_y;	// y axis index for start of motion features

  override void	UpdateAfterEdit_impl();

  override void	UpdateGeom();

  override bool InitFilters();
  override bool InitOutMatrix();
  override bool InitDataTable();

  virtual bool	InitFilters_V1Simple();
  virtual bool	InitFilters_V1Motion();
  virtual bool	InitFilters_V1Binocular();
  virtual bool	InitFilters_V1Complex();

  override bool	FilterImage_impl();
  override void IncrTime();

  virtual bool	V1SimpleFilter_Static(float_Matrix* dog, CircMatrix* dog_circ,
				      float_Matrix* out, CircMatrix* circ);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Static_thread(int v1s_idx, int thread_no);
  // do simple filters, static only on current inputs -- do it

  virtual bool	V1SimpleFilter_Motion(float_Matrix* out, CircMatrix* circ);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no);
  // do simple filters, static only on current inputs -- do it

  virtual bool	V1SRenormOutput_Static(float_Matrix* out, CircMatrix* circ);
  virtual bool	V1SRenormOutput_Motion(float_Matrix* out, CircMatrix* circ);

  virtual bool	V1BinocularFilter();
  // do binocular filters -- dispatch threads
  virtual void 	V1BinocularFilter_thread(int v1b_idx, int thread_no);
  // do binocular filters -- do it

  virtual bool	V1ComplexFilter();
  // do complex filters -- dispatch threads
  virtual void 	V1ComplexFilter_Monocular_thread(int v1c_idx, int thread_no);
  // do complex filters from monocular inputs -- do it
  virtual void 	V1ComplexFilter_Binocular_thread(int v1c_idx, int thread_no);
  // do complex filters from binocular inputs -- do it

  virtual bool V1SOutputToTable(DataTable* dtab);
  // simple to output table
  virtual bool V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
				     const String& col_sufx);
  // simple to output table impl
  virtual bool V1BOutputToTable(DataTable* dtab);
  // binocular to output table
  virtual bool V1COutputToTable(DataTable* dtab);
  // complex to output table
};


class TA_API RetinaProc : public taNBase {
  // #STEM_BASE ##CAT_Image ##DEF_CHILD_regions ##DEF_CHILDNAME_Regions full specification of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(taNBase)
public:
  taImageProc::EdgeMode	edge_mode;	// how to deal with edges in processing the raw images in preparation for presentation to the filters -- each region has its own filter-specific edge mode
  int 			fade_width;	// #CONDSHOW_ON_edge_mode:BORDER for border mode -- how wide of a frame to fade in around the border at the end of all the operations 
  DoGRegionSpecList	regions;	// defines regions of the visual input where the processing actually takes place -- most of the specification is at this level -- first region is used for retina size and other basic params

  float_Matrix		raw_image_r; 	// current raw input image presented to system, for right eye or only eye if monocular
  float_Matrix		raw_image_l; 	// current raw input image presented to system, for left eye -- only if binocular

  float_Matrix		xform_image_r; 	// current transformed version of raw image presented to system, for right eye or only eye if monocular
  float_Matrix		xform_image_l; 	// current transformed version of raw image presented to system, for left eye -- only if binocular

  virtual DoGRegionSpec* AddRegion()	{ return (DoGRegionSpec*)regions.New(1); }
  // #BUTTON #CAT_Filter add a new region 

  ///////////////////////////////////////////////////////////////////////
  // Basic functions operating on float image data: transform image, apply dog filters

  virtual bool	Init();
  // #BUTTON initialize the filters, data table, etc -- call this in the init_code of any Program that is using this object

  virtual bool	TransformImageData(float_Matrix* right_eye_image,
				   float_Matrix* left_eye_image = NULL, 
				   float move_x=0.0f, float move_y=0.0f,
				   float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image data in matrix format, with retina centered at given normalized offsets from center of image (move -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (1=same size, .5 = half size, 2 = double size), rotated by normalized units (1=360deg)

  virtual bool	LookAtImageData(float_Matrix* right_eye_image,
				float_Matrix* left_eye_image = NULL, 
				DoGRegionSpec::Region region = DoGRegionSpec::FOVEA,
				float box_ll_x=0.0f, float box_ll_y=0.0f,
				float box_ur_x=1.0f, float box_ur_y=1.0f,
				float move_x=0.0f, float move_y=0.0f,
				float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image data in matrix format, with region of retina centered and scaled to fit the box coordinates given in 0-1 normalized units (ll=lower-left, ur=upper-right); additional scale, rotate, and move params applied after foveation scaling and offsets

  virtual bool	FilterImageData();
  // #CAT_Filter filter retinal image data -- operates on images that were generated from prior Transform or LookAt calls -- must call one of those first.


  ///////////////////////////////////////////////////////////////////////
  // Transform Routines taking different sources for image input data

  virtual bool  ConvertImageToMatrix(float_Matrix& img_data, taImage* img, 
				     DoGRegionSpec::Color color);
  // #CAT_Image convert image file to img_data float matrix, converting to color or monochrome as specified

  virtual bool	TransformImage(taImage* right_eye_image, taImage* left_eye_image = NULL,
			       float move_x=0, float move_y=0,
			       float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image/s in image format, with retina centered at given normalized offsets from center of image (move -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (1=same size, .5 = half size, 2 = double size), rotated by normalized units (1=360deg)

  virtual bool	TransformImageName(const String& right_eye_img_fname,
				   const String& left_eye_img_fname = "",
				   float move_x=0, float move_y=0,
				   float scale = 1.0f, float rotate = 0.0f);
  // #BUTTON #CAT_Transform #FILE_DIALOG_LOAD load image/s from file/s and transform image data, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg)

  virtual bool	LookAtImage(taImage* right_eye_image,
			    taImage* left_eye_image = NULL, 
			    DoGRegionSpec::Region region = DoGRegionSpec::FOVEA,
			    float box_ll_x=0.0f, float box_ll_y=0.0f,
			    float box_ur_x=1.0f, float box_ur_y=1.0f,
			    float move_x=0.0f, float move_y=0.0f,
			    float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image/s in image format, with region of retina centered and scaled to fit the box coordinates given in 0-1 normalized units (ll=lower-left, ur=upper-right); additional scale, rotate, and move params applied after foveation scaling and offsets

  virtual bool	LookAtImageName(const String& right_eye_img_fname,
				const String& left_eye_img_fname = "",
				DoGRegionSpec::Region region = DoGRegionSpec::FOVEA,
				float box_ll_x=0.0f, float box_ll_y=0.0f,
				float box_ur_x=1.0f, float box_ur_y=1.0f,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f);
  // #BUTTON #CAT_Filter #FILE_DIALOG_LOAD load image/s from file/s and transform image data, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets

  ///////////////////////////////////////////////////////////////////////
  // Misc other processing operations

  virtual bool	AttendRegion(DataTable* dt, DoGRegionSpec::Region region = DoGRegionSpec::FOVEA);
  // #CAT_Filter apply attentional weighting filter to filtered values, with radius = given region

  // todo: need a checkconfig here..

  override taList_impl*	children_() {return &regions;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return regions.GetTA_Element(i, eltd); }

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinaProc);
protected:
  void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);

  virtual bool	TransformImageData_impl(float_Matrix& eye_image,
					float_Matrix& xform_image,
					float move_x=0.0f, float move_y=0.0f,
					float scale = 1.0f, float rotate = 0.0f);
  // implementation function that processes an eye input image into an xform output image

  virtual bool	LookAtImageData_impl(float_Matrix& eye_image,
				     float_Matrix& xform_image,
				     DoGRegionSpec::Region region = DoGRegionSpec::FOVEA,
				     float box_ll_x=0.0f, float box_ll_y=0.0f,
				     float box_ur_x=1.0f, float box_ur_y=1.0f,
				     float move_x=0.0f, float move_y=0.0f,
				     float scale = 1.0f, float rotate = 0.0f);
  // implementation function that processes an eye input image into an xform output image

};

SmartRef_Of(RetinaProc,TA_RetinaProc); // RetinaProcRef

class TA_API V1RetinaProc : public RetinaProc {
  // V1 version of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(RetinaProc)
public:
  virtual DoGRegionSpec* AddRegion()	{ return (DoGRegionSpec*)regions.New(1, &TA_V1RegionSpec); }

  void 	Initialize();
  void	Destroy() { };
  TA_BASEFUNS_NOCOPY(V1RetinaProc);
};

/////////////////////////////////////////////////////////
//   programs to support image processing operations
/////////////////////////////////////////////////////////

class TA_API ImageProcCall : public StaticMethodCall { 
  // call a taImageProc function
INHERITED(StaticMethodCall)
public:
  override String	GetToolbarName() const { return "img proc()"; }
  TA_BASEFUNS_NOCOPY(ImageProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 


///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	     OBSOLETE Code just for loading and converting

class TA_API GaborRFSpec : public taBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image Gabor receptive field spec (for V1)
  INHERITED(taBase)
public:
  int		n_angles;	// number of different angles
  float		freq;		// frequency of the sine wave
  float		length;		// length of the gaussian perpendicular to the wave direction
  float		width;		// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)
  float 	phase_off; 	// #AKA_phase_mod phase offset -- constant to add to all phases

  void 	Initialize() { };
  void	Destroy() { };
  SIMPLE_COPY(GaborRFSpec);
  TA_BASEFUNS(GaborRFSpec);
};

class TA_API MotionRFSpec : public taBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image Gabor receptive field spec (for V1)
  INHERITED(taBase)
public:
  int		n_angles;	// number of different angles
  float		freq;		// frequency of the sine wave
  float		freq_t;		// frequency of the sine wave related to time
  float		length;		// length of the gaussian perpendicular to the wave direction
  float		width;		// width of the gaussian in the wave direction
  float		width_t;	// width of the gaussian in the time direction
  float		t_mult;		// ??
  float		amp;		// amplitude (maximum value)
  int       	timesteps;      // number of timesteps a motion sensitive filter responds to

  void 	Initialize() { };
  void	Destroy() { };
  SIMPLE_COPY(MotionRFSpec);
  TA_BASEFUNS(MotionRFSpec);
};

class TA_API BlobRFSpec : public taBase {
  // #INLINE #INLINE_DUMP ##CAT_Image Color Blob receptive field specs (for V1)
  INHERITED(taBase)
public:
  int		n_sizes;	// number of different sizes
  float		width_st;	// starting center width
  float		width_inc;	// increment of width per unit

  void 	Initialize() { };
  void	Destroy() { };
  SIMPLE_COPY(BlobRFSpec);
  TA_BASEFUNS(BlobRFSpec);
};

class TA_API RetinalSpacingSpec : public taNBase {
  // ##OBSOLETE ##STEM_BASE #INLINE ##CAT_Image defines the spacing of a filter relative to a specified retinal image size
INHERITED(taNBase)
public:
  enum Region {			// retinal region
    FOVEA,
    PARAFOVEA,
    PERIPHERY,
  };
  enum Resolution {		// level of resolution
    HI_RES,
    MED_RES,
    LOW_RES,
    VLOW_RES,
  };

  Region	region;		// retinal region represented by this filter 
  Resolution	res;		// level of resolution represented by this filter (can use enum or any other arbitrary rating scale -- just for informational/matcing purposes)
  TwoDCoord	retina_size;	// overall size of the retina
  TwoDCoord	border;		// border around retina that we don't process NOTE: can be negative for wrap-around case -- code will automatically wrap out-of-range numbers
  TwoDCoord	spacing;	// spacing between centers of filters in input
  TwoDCoord	input_size;	// #READ_ONLY #SHOW size of input region in pixels 
  TwoDCoord	output_size;	// #READ_ONLY #SHOW size of the filter output
  int		output_units;	// #READ_ONLY #SHOW total number of units to represent filter

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinalSpacingSpec);
};

class TA_API DoGRetinaSpec : public taNBase {
  // ##OBSOLETE ##CAT_Image specifies the spacing of Difference-of-Gaussian filters for the retina
INHERITED(taNBase)
public:
  // note: this must be same as one in taImageProc
  enum EdgeMode {		// how to deal with image edges
    CLIP,			// just hard clip edges, nothing fancy
    BORDER,			// render/preserve a 1 pixel border around everything
    WRAP,			// wrap the image around to the other side: no edges!
  };

  DoGFilter		dog;		// Difference of Gaussian retinal filters
  RetinalSpacingSpec	spacing;	// how to space DoG filters in the retina

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRetinaSpec);
};

class TA_API DoGRetinaSpecList : public taList<DoGRetinaSpec> {
  // ##OBSOLETE ##CAT_Image a list of DoG retinal filters
INHERITED(taList<DoGRetinaSpec>)
public:
  TA_BASEFUNS_NOCOPY(DoGRetinaSpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DoGRetinaSpec); }
  void 	Destroy()		{ };
};

class TA_API GaborV1SpecBase : public ImgProcThreadBase {
  // ##OBSOLETE #STEM_BASE ##CAT_Image basic V1 model as either blob (DOG) or gabor filters with a specified rf width -- used for generating connections or for explicit filter operation in GaborV1Spec
INHERITED(ImgProcThreadBase)
public:
  enum V1FilterType {  
    GABOR,			// filter using gabors (orientation tuned)
    BLOB,			// filter using blobs (color contrast tuned)
    COPY,			// just copy from retinal inputs, summing over on and off fields
    MOTIONDISP_GABOR			// filter using gabors (orientation tuned) that are oriented in time and space and receive from two retinas
  };

  V1FilterType	filter_type; 	// what type of filter to use?
  TwoDCoord 	rf_width;	// width of the receptive field into the retinal inputs -- enforced to be even numbers, to enable the 1/2 overlap constraint for neighboring rf's

  TwoDCoord 	rf_ovlp;	//  half-width of the receptive field into the retinal inputs, which is the amount that the receptive fields overlap   CONDEDIT_ON_filter_type:COPY
  int		n_filters;	// #READ_ONLY #SHOW number of filters -- computed from appropriate _rf specifications in terms of number of angles/sizes etc.
  GaborRFSpec	gabor_rf;	// #CONDEDIT_ON_filter_type:GABOR parameters for gabor filter specs
  BlobRFSpec	blob_rf;	// #CONDEDIT_ON_filter_type:BLOB parameters for blob filter specs
  MotionRFSpec	motiondisp_gabor_rf;	// #CONDEDIT_ON_filter_type:MOTIONDISP_GABOR parameters for motion/disp gabor filter specs

  bool use_3d_gabors; //motion only
  bool two_phase;

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborV1SpecBase);
};

class TA_API GaborV1Spec : public GaborV1SpecBase {
  // implements Gabor or DoG (Blob) filtering of a DoG filtered input image, as a model of V1 -- supports integration of multiple filters per unit for greater effective resolution without additional network processing cost
INHERITED(GaborV1SpecBase)
public:
  RetinalSpacingSpec::Region region; // retinal region represented by this filter -- for matching up with associated retinal outputs
  RetinalSpacingSpec::Resolution res; // resolution represented by this filter -- for matching up with associated retinal outputs

  XYNGeom	un_geom;  	// size of one 'hypercolumn' unit of orientation detectors -- sets the datatable geometry -- must include room for number of angles/sizes, on/off and n_filters_per_gp (for COPY mode, this is entire size of layer)
  XYNGeom	gp_geom;  	// #CONDEDIT_OFF_filter_type:COPY size of full set of groups of hypercolumns to process entire set of inputs: with wrap, is input_size / rf_ovlp, subtract 1 for !wrap
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -input_ovlp (wrapped to right/top) and goes +input_ovlp past the right/top edge (wrapped to left/bottom)
  XYNGeom	n_filter_gps;	// #CONDEDIT_OFF_filter_type:COPY number of groups of filters in each axis -- replicates filters (in interdigitated fashion if n_filters_per_gp > 1) across multiple adjacent locations and integrates into summary value (with gaussian weighting from center)
  int		n_filters_per_gp; // #CONDEDIT_OFF_filter_type:COPY number of filters per group -- when n_filter_gps > 1, may be useful to have multiple interdigitated filter units to reduce redundancy and produce better effective resolution
  XYNGeom	tot_filter_gps;	// #READ_ONLY #SHOW n_filter_gps * n_filters_per_gp -- total number of filter groups per location
  XYNGeom	filter_gp_ovlp;	// #READ_ONLY #SHOW tot_filter_gps / 2 -- overlap of filter groups, in terms of groups of filters
  XYNGeom	input_ovlp;	// #READ_ONLY #SHOW filter_gp_ovlp * rf_ovlp + rf_ovlp -- overlap of filter groups, in terms of input coordinates -- how much to move over in input space when processing -- note that each subsequent group moves over an extra rf_ovlp
  XYNGeom	trg_input_size;	// #READ_ONLY #SHOW target input size: gp_geom * input_ovlp for wrap; (gp_geom - 1) * input_ovlp for !wrap
  float		gp_gauss_sigma;	  // width of gaussian weighting factor over the filter groups, in normalized terms relative to tot_filter_gps widths
  float_Matrix	gp_gauss_mat;	  // #READ_ONLY #NO_SAVE #NO_COPY group gaussian vals 
	
  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborV1Spec);
};


class TA_API MotionDispGaborV1Spec : public GaborV1SpecBase {
  // implements Gabor or DoG (Blob) filtering of a DoG filtered input image, as a model of V1 -- supports integration of multiple filters per unit for greater effective resolution without additional network processing cost
INHERITED(GaborV1SpecBase)
public:
  RetinalSpacingSpec::Region region; // retinal region represented by this filter -- for matching up with associated retinal outputs
  RetinalSpacingSpec::Resolution res; // resolution represented by this filter -- for matching up with associated retinal outputs

  int 		rf_time;	// size of rf time dimension
  XYNGeom	un_geom;  	// size of one 'hypercolumn' unit of orientation detectors -- sets the datatable geometry -- must include room for number of angles/sizes, on/off and n_filters_per_gp (for COPY mode, this is entire size of layer)
  XYNGeom	gp_geom;  	// #CONDEDIT_OFF_filter_type:COPY size of full set of groups of hypercolumns to process entire set of inputs: with wrap, is input_size / rf_ovlp, subtract 1 for !wrap
  bool		wrap;		// if true, then connectivity has a wrap-around structure so it starts at -input_ovlp (wrapped to right/top) and goes +input_ovlp past the right/top edge (wrapped to left/bottom)
  XYNGeom	n_filter_gps;	// #CONDEDIT_OFF_filter_type:COPY number of groups of filters in each axis -- replicates filters (in interdigitated fashion if n_filters_per_gp > 1) across multiple adjacent locations and integrates into summary value (with gaussian weighting from center)
  int		n_filters_per_gp; // #CONDEDIT_OFF_filter_type:COPY number of filters per group -- when n_filter_gps > 1, may be useful to have multiple interdigitated filter units to reduce redundancy and produce better effective resolution
  XYNGeom	tot_filter_gps;	// #READ_ONLY #SHOW n_filter_gps * n_filters_per_gp -- total number of filter groups per location
  XYNGeom	filter_gp_ovlp;	// #READ_ONLY #SHOW tot_filter_gps / 2 -- overlap of filter groups, in terms of groups of filters
  XYNGeom	input_ovlp;	// #READ_ONLY #SHOW filter_gp_ovlp * rf_ovlp + rf_ovlp -- overlap of filter groups, in terms of input coordinates -- how much to move over in input space when processing -- note that each subsequent group moves over an extra rf_ovlp
  XYNGeom	trg_input_size;	// #READ_ONLY #SHOW target input size: gp_geom * input_ovlp for wrap; (gp_geom - 1) * input_ovlp for !wrap
  float		gp_gauss_sigma;	  // width of gaussian weighting factor over the filter groups, in normalized terms relative to tot_filter_gps widths
  float_Matrix	gp_gauss_mat;	  // #READ_ONLY #NO_SAVE #NO_COPY group gaussian vals 
  float disp_gauss_sigma; // width of gaussian weighting factor over the disparity comparison
  int		disparity_width;	//size of the area to compare disparities
  int		disparity_offset;	//disparity offset increments
  bool 		max_in_disparity;
	
  float_Matrix  disp_gauss_mat; // #READ_ONLY #NO_SAVE #NO_COPY disparity group gaussian vals

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(MotionDispGaborV1Spec);
};

class TA_API GaborV1SpecList : public taList<GaborV1Spec> {
  // ##OBSOLETE ##CAT_Image a list of Gabor V1 filters
INHERITED(taList<GaborV1Spec>)
public:

  TA_BASEFUNS_NOCOPY(GaborV1SpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_GaborV1Spec); }
  void 	Destroy()		{ };
};

class TA_API MotionDispGaborV1SpecList : public taList<MotionDispGaborV1Spec> {
  // ##OBSOLETE ##CAT_Image a list of Gabor V1 filters
INHERITED(taList<MotionDispGaborV1Spec>)
public:
  TA_BASEFUNS_NOCOPY(MotionDispGaborV1SpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_MotionDispGaborV1Spec); }
  void 	Destroy()		{ };
};

class TA_API RetinaSpec : public ImgProcThreadBase {
  // ##OBSOLETE #STEM_BASE ##CAT_Image ##DEF_CHILD_dogs ##DEF_CHILDNAME_DOG_Filters full specification of retinal filtering based on difference-of-gaussian filters
INHERITED(ImgProcThreadBase)
public:
  enum ColorType {		// type of color processing to do (determines file loading)
    MONOCHROME,
    COLOR,
  };

  ColorType		color_type;	// type of color processing (determines file processing
  TwoDCoord		retina_size; 	// overall size of retina (auto copied to retina specs)
  taImageProc::EdgeMode	edge_mode;	// how to deal with edges
  int 			fade_width;	// #CONDEDIT_ON_edge_mode:BORDER for border mode -- how wide of a frame to fade in around the border at the end of all the operations (-1 = use width of max off-center DOG sigma, 0 = none)
  float			renorm_thr;	// #DEF_1e-5 threshold overall maximum intensity value required to perform renormalization -- if below this value, no renorm is performed (i.e., it remains effectively blank)
  DoGRetinaSpecList	dogs;		// the difference-of-gaussian retinal filters

  override taList_impl*	children_() {return &dogs;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return dogs.GetTA_Element(i, eltd); }

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinaSpec);
};

SmartRef_Of(RetinaSpec,TA_RetinaSpec); // RetinaSpecRef

class TA_API V1GaborSpec : public taNBase {
  // ##OBSOLETE #STEM_BASE ##CAT_Image ##DEF_CHILD_gabors ##DEF_CHILDNAME_Gabor_Filters full specification of V1 gabor (oriented edge detectors) filtering -- takes output of RetinaSpec as input
INHERITED(taNBase)
public:
  GaborV1SpecList	gabors;		// the gabor (and blob) V1 filters
  bool			wrap;		// if true, then filtering has a wrap-around structure, starting at -1/2 offset (wrapped to right/top) and goes +1/2 offset past the right/top edge (wrapped to left/bottom)
  RetinaSpecRef		retina;		// the specs for the retinal filter that we follow
  float			norm_max;	// #DEF_0.95 max value to normalize output activations to -- set to -1 to turn off normalization
  float			norm_thr;	// #DEF_0.01 threshold maximum activation value for renormalizing  -- if below this value, no renormalization is applied

  override taList_impl*	children_() {return &gabors;}
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return gabors.GetTA_Element(i, eltd); }

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1GaborSpec);
};

class TA_API MotionDispV1GaborSpec : public taNBase {
  // ##OBSOLETE #STEM_BASE ##CAT_Image ##DEF_CHILD_gabors ##DEF_CHILDNAME_Gabor_Filters full specification of motion/disparity tuned V1 gabor (oriented edge detectors) filtering -- takes output of RetinaSpec as input.
INHERITED(taNBase)
public:
  MotionDispGaborV1SpecList	gabors;		// the gabor (and blob) V1 filters
  bool			wrap;		// if true, then filtering has a wrap-around structure, starting at -1/2 offset (wrapped to right/top) and goes +1/2 offset past the right/top edge (wrapped to left/bottom)
  RetinaSpecRef		retina;		// the specs for the retinal filter that we follow
  float			norm_max;	// #DEF_0.95 max value to normalize output activations to -- set to -1 to turn off normalization
  float			norm_thr;	// #DEF_0.01 threshold maximum activation value for renormalizing  -- if below this value, no renormalization is applied

  override taList_impl*	children_() {return &gabors;}
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return gabors.GetTA_Element(i, eltd); }

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(MotionDispV1GaborSpec);
};

#endif // ta_imgproc_h

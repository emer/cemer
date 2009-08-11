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
  // #CAT_File #FILE_DIALOG_LOAD load the image from given file name (leave file name blank to get a chooser)
  virtual bool	SaveImage(const String& fname);
  // #CAT_File #FILE_DIALOG_SAVE save the image to given file name (leave file name blank to get a chooser) -- uses extension to determine format

  virtual float	GetPixelGrey_float(int x, int y);
  // #CAT_Image get the pixel value as a greyscale for given coordinates
  virtual bool	GetPixelRGB_float(int x, int y, float& r, float& g, float& b);
  // #CAT_Image get the pixel value as floating point RGB values for given coordinates

  virtual bool	ImageToGrey_float(float_Matrix& grey_data);
  // #CAT_Data convert image to greyscale floating point image data: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  virtual bool	ImageToRGB_float(float_Matrix& rgb_data);
  // #CAT_Data convert image to RGB floating point image data -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  virtual bool	ImageToDataCell(DataTable* dt, int col, int row);
  // #CAT_Data set image to datatable cell indexed by col and row numbers -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ImageToDataCellName(DataTable* dt, const String& col_nm, int row);
  // #CAT_Data set image to datatable cell indexed by col name and row number -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ConfigDataColName(DataTable* dt, const String& col_nm, ValType val_type, 
				  bool rgb = true);
  // #CAT_Data configure data column with given name (if it doesn't exist, it is created) to represent current image -- if rgb is false, then a greyscale image is configured (2d)

  virtual bool	ScaleImage(float sx, float sy, bool smooth=true);
  // #CAT_Image scale image by given normalized scaling factors in each dimension
  virtual bool	RotateImage(float norm_deg, bool smooth=true);
  // #CAT_Image rotate image by given normalized degrees (1 = 360deg)
  virtual bool	TranslateImage(float move_x, float move_y, bool smooth=true);
  // #CAT_Image translate image by given normalized factors (-1 = all the way left, +1 = all the way right, etc)

  virtual bool	GetImageSize(int& width, int& height);
  // #CAT_Image get size of current image
  virtual bool	SetImageSize(int width, int height);
  // #CAT_Image set size of current image -- if currently same size, then returns false and nothing happens; otherwise, a new image data structure of given size is created, using ARGB32 format

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

  virtual void Filter_Thread(int cmp_unit_index, int thread_no=-1) { };
  // thread-wise filtering function, takes computation unit index (task-specific -- just defines what element of problem it is working on) and thread no -- all necc state should be on obj -- can actually call the function anything you want but this shows the basic format

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(ImgProcThreadBase);
};



class TA_API DoGFilterSpec : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Image defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
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
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #NO_COPY on-gaussian 
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #NO_COPY off-gaussian (values are positive)
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #NO_COPY net overall filter (for display purposes)

  float		FilterPoint(int x, int y, float r_val, float g_val, float b_val);
  // #CAT_DoGFilter apply filter at given x,y point to given color values

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
  TA_SIMPLE_BASEFUNS(DoGFilterSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API GaborFilterSpec : public taNBase {
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
  TA_SIMPLE_BASEFUNS(GaborFilterSpec);
};

class TA_API GaborFitter : public GaborFilterSpec {
  // ##CAT_Image fits a gabor filter from image data
INHERITED(GaborFilterSpec)
public:
  float		fit_dist;	// #READ_ONLY #SHOW fit distance

  virtual float ParamDist(const GaborFilterSpec& oth);
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

////////////////////////////////////////////////////////////////////
//		Retinal Processing (DoG model)

class TA_API RetinalSpacingSpec : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Image defines the spacing of a filter relative to a specified retinal image size
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

  virtual void	UpdateSizes();
  // update the computed sizes

  virtual void	PlotSpacing(DataTable* disp_data, float val = 1.0f);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinalSpacingSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API DoGRetinaSpec : public taNBase {
  // ##CAT_Image specifies the spacing of Difference-of-Gaussian filters for the retina
INHERITED(taNBase)
public:
  // note: this must be same as one in taImageProc
  enum EdgeMode {		// how to deal with image edges
    CLIP,			// just hard clip edges, nothing fancy
    BORDER,			// render/preserve a 1 pixel border around everything
    WRAP,			// wrap the image around to the other side: no edges!
  };

  DoGFilterSpec		dog;		// Difference of Gaussian retinal filters
  RetinalSpacingSpec	spacing;	// how to space DoG filters in the retina

  virtual bool	FilterRetina(float_Matrix& on_output, float_Matrix& off_output,
			     float_Matrix& retina_img, bool superimpose = false,
			     EdgeMode edge = BORDER);
  // apply DoG filter to input image, result in output (on = + vals, off = - vals). superimpose = add values into the outputs instead of overwriting

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view

  virtual void	PlotSpacing(DataTable* disp_data, float val = 1.0f);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRetinaSpec);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API DoGRetinaSpecList : public taList<DoGRetinaSpec> {
  // ##CAT_Image a list of DoG retinal filters
INHERITED(taList<DoGRetinaSpec>)
public:

  virtual DoGRetinaSpec* FindRetinalRegion(RetinalSpacingSpec::Region reg);
  // find first spec with given retinal region
  virtual DoGRetinaSpec* FindRetinalRes(RetinalSpacingSpec::Resolution res);
  // find first spec with given resolution
  virtual DoGRetinaSpec* FindRetinalRegionRes(RetinalSpacingSpec::Region reg,
					      RetinalSpacingSpec::Resolution res);
  // find first spec with given retinal region and resolution (falls back to res then reg if no perfect match)

  TA_BASEFUNS_NOCOPY(DoGRetinaSpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_DoGRetinaSpec); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////
//		V1 Processing (Gabor model)

class TA_API GaborRFSpec : public taBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Spec Gabor receptive field spec (for V1)
  INHERITED(taBase)
public:
  int		n_angles;	// number of different angles
  float		freq;		// frequency of the sine wave
  float		length;		// length of the gaussian perpendicular to the wave direction
  float		width;		// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)

  void 	Initialize();
  void	Destroy() { };
  SIMPLE_COPY(GaborRFSpec);
  TA_BASEFUNS(GaborRFSpec);
};

class TA_API BlobRFSpec : public taBase {
  // #INLINE #INLINE_DUMP ##CAT_Spec Blob receptive field specs (for V1)
  INHERITED(taBase)
public:
  int		n_sizes;	// number of different sizes
  float		width_st;	// starting center width
  float		width_inc;	// increment of width per unit

  void 	Initialize();
  void	Destroy() { };
  SIMPLE_COPY(BlobRFSpec);
  TA_BASEFUNS(BlobRFSpec);
};

class TA_API GaborV1SpecBase : public ImgProcThreadBase {
  // #STEM_BASE ##CAT_Image basic V1 model as either blob (DOG) or gabor filters with a specified rf width -- used for generating connections or for explicit filter operation in GaborV1Spec
INHERITED(ImgProcThreadBase)
public:
  enum V1FilterType {
    GABOR,			// filter using gabors (orientation tuned)
    BLOB,			// filter using blobs (color contrast tuned)
    COPY,			// just copy from retinal inputs, summing over on and off fields
  };

  V1FilterType	filter_type; 	// what type of filter to use?
  TwoDCoord 	rf_width;	// width of the receptive field into the retinal inputs -- enforced to be even numbers, to enable the 1/2 overlap constraint for neighboring rf's
  TwoDCoord 	rf_ovlp;	// #CONDEDIT_ON_filter_type:COPY half-width of the receptive field into the retinal inputs, which is the amount that the receptive fields overlap
  int		n_filters;	// #READ_ONLY #SHOW number of filters -- computed from appropriate _rf specifications in terms of number of angles/sizes etc.
  GaborRFSpec	gabor_rf;	// #CONDEDIT_ON_filter_type:GABOR parameters for gabor filter specs
  BlobRFSpec	blob_rf;	// #CONDEDIT_ON_filter_type:BLOB parameters for blob filter specs

  taBase_List 	gabor_specs; 	// #READ_ONLY #NO_SAVE underlying gabor generators (type GaborFilterSpec)
  taBase_List 	blob_specs; 	// #READ_ONLY #NO_SAVE underlying DoG generators (type DoGFilterSpec)

  virtual bool 	InitFilters();
  // initialize the filters based on the RF specs

  virtual void	GraphFilter(DataTable* disp_data, int unit_no);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph of a given unit number's gabor / blob filter
  virtual void	GridFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view of all the gabor or blob filters

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborV1SpecBase);
protected:
  virtual bool 	InitFilters_Gabor();
  virtual bool 	InitFilters_Blob();
  virtual bool 	InitFilters_Copy();

  void	UpdateAfterEdit_impl();
};

/////////////////////////////////////////////////////////////////////////////////
//	GaborV1Spec filter geometry
//
// ..vvVVvv..  gaussian weights
//  3 filt gps     2 filt gps  2 filt gps
// 2 filt per gp   2 per gp    3 per gp
// 0 1 0 1 0 1     0 1 0 1     0 1 2 0 1 2
// 1 0 1 0 1 0     1 0 1 0     1 2 0 1 2 0
// 0 1 0 1 0 1     0 1 0 1     2 0 1 2 0 1
// 1 0 1 0 1 0     1 0 1 0     0 1 2 0 1 2
// 0 1 0 1 0 1                 1 2 0 1 2 0
// 1 0 1 0 1 0                 2 0 1 2 0 1
//
// ordering within un_geom (filters_per_gp, angle, on/off):
// -01 /01 |01 \01 recv, on
// -01 /01 |01 \01 recv, off
//
// grid spacing is rf_ovlp
// overall spacing between rfs is 1/2 * n_filter_gps * n_filters_per_gp * rf_ovlp

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

  virtual void	UpdateGeoms();	// update all the geometry values based on current setting

  virtual bool	SetGpGeomFmInputSize(TwoDCoord& input_size);
  // set the gp_geom based on the given input_size (size of input to filter) and existing filter parameters
  virtual bool 	SetGpGeomFmRetSpec(DoGRetinaSpecList& dogs);
  // set the gp_geom based on the input size on the given list of retinal dog filters by region and resolution

  virtual bool	FilterInput(float_Matrix& v1_output, DoGFilterSpec::ColorChannel c_chan,
			    float_Matrix& on_input, float_Matrix& off_input,
			    bool superimpose = false);
  // actually perform the filtering operation on input patterns: calls threading deploy

  override void Filter_Thread(int cmp_unit_index, int thread_no=-1);
  // this is thread target function, deploys to following based on type:

  virtual bool	FilterInput_Gabor(int cmp_idx);
  // actually perform the filtering operation on input patterns: Gabors
  virtual bool	FilterInput_Blob(int cmp_idx);
  // actually perform the filtering operation on input patterns: Blobs
  virtual bool	FilterInput_Copy(int cmp_idx);
  // actually perform the filtering operation on input patterns: Copy

  virtual void	GridFilterInput(DataTable* disp_data, int unit_no=0, int gp_skip=2, bool ctrs_only=false);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter for a given unit as it will be applied to the entire input data -- gp_skip specifies the number of unit groups to increment -- 2 is good to avoid complete overlap -- ctrs_only will only plot the centers of the filters, not the actual raw filters themselves

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborV1Spec);
protected:
  override bool 	InitFilters_Gabor();
  override bool 	InitFilters_Blob();

  void	UpdateAfterEdit_impl();

  // cache of items for current function call
  float_Matrix* cur_v1_output;
  DoGFilterSpec::ColorChannel cur_c_chan;
  float_Matrix* cur_on_input;
  float_Matrix* cur_off_input;
  bool 		cur_superimpose;
};

class TA_API GaborV1SpecList : public taList<GaborV1Spec> {
  // ##CAT_Image a list of Gabor V1 filters
INHERITED(taList<GaborV1Spec>)
public:

  virtual bool UpdateSizesFmRetina(DoGRetinaSpecList& dogs);
  // calls SetGpGeomFmRetSpec on all the items in the list

  TA_BASEFUNS_NOCOPY(GaborV1SpecList);
private:
  void	Initialize() 		{ SetBaseType(&TA_GaborV1Spec); }
  void 	Destroy()		{ };
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
  
  static bool	DoGFilterRetina(float_Matrix& on_output, float_Matrix& off_output,
				float_Matrix& retina_img, DoGRetinaSpec& spec,
				bool superimpose = false, EdgeMode edge=BORDER);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply DoG filter to input image, result in output (on = + vals, off = - vals). superimpose = add values into the outputs instead of overwriting (just a call to equiv function on spec)

  static bool	AttentionFilter(float_Matrix& mat, float radius_pct);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply an "attentional" filter to the matrix data: outside of radius, values are attenuated in proportion of squared distance outside of radius (r_sq / dist_sq) -- radius_pct is normalized proportion of maximum half-size of image (e.g., 1 = attention bubble extends to furthest edge of image; only corners are attenuated)

  virtual bool	GaborFilterV1(float_Matrix& v1_output, DoGFilterSpec::ColorChannel c_chan,
			      float_Matrix& on_input, float_Matrix& off_input,
			      GaborV1Spec& spec, bool superimpose = false, EdgeMode edge=BORDER);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply Gabor filter to on/off input channels to produce a V1-like processing of the input image.  superimpose = add values instead of overwriting (this is just a call to equiv function on spec)

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  TA_BASEFUNS_NOCOPY(taImageProc);
private:
  void 	Initialize();
  void	Destroy();
};

//////////////////////////////////////////////////////////////////////////////////////
//   special objects that specify full set of image processing operations

class TA_API RetinaSpec : public ImgProcThreadBase {
  // #STEM_BASE ##CAT_Image ##DEF_CHILD_dogs ##DEF_CHILDNAME_DOG_Filters full specification of retinal filtering based on difference-of-gaussian filters
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

  virtual DoGRetinaSpec* AddFilter()	{ return (DoGRetinaSpec*)dogs.New(1); }
  // #BUTTON #CAT_Filter add a new retina filter

  virtual void	DefaultFilters();
  // #BUTTON #CAT_Filter create a set of default filters

  virtual void	ConfigDataTable(DataTable* dt, bool reset_cols = false);
  // #BUTTON #CAT_Config #NULL_OK_0 #NULL_TEXT_0_NewDataTable configure a data table to hold all of the image data (if reset_cols, reset any existing cols in data table before adding new ones) (if dt == NULL, a new one is created in data.InputData)

  virtual void	PlotSpacing(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the arrangement of the filters (centers) in the data table and generate a grid view

  ///////////////////////////////////////////////////////////////////////
  // Basic functions operating on float image data: transform image, apply dog filters

  virtual DataCol* GetRetImageColumn(DataTable* dt);
  // #CAT_Transform get the RetinaImage column with appropriate args for ensuring its correct size etc has been set

  virtual bool	TransformImageData(float_Matrix& img_data, DataTable* dt,
				   float move_x=0.0f, float move_y=0.0f,
				   float scale = 1.0f, float rotate = 0.0f,
				   bool superimpose = false);
  // #CAT_Transform transform image data into datatable, with retina centered at given normalized offsets from center of image (move), scaled by given factor (zoom), rotated by normalized units (1=360deg), superimpose = merge into filter values into last row of table; otherwise new row is added -- impl routine for other functions to call (doesn't do any display updating)

  virtual bool	LookAtImageData(float_Matrix& img_data, DataTable* dt,
				RetinalSpacingSpec::Region region,
				float box_ll_x, float box_ll_y,
				float box_ur_x, float box_ur_y,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false);
  // #CAT_Transform transform image data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and move params applied after foveation scaling and offsets, if superimpose, only do for last one!)

  virtual bool	FilterImageData(DataTable* dt, bool superimpose = false, int renorm = 1);
  // #CAT_Filter filter retinal image data in RetinaImage column produced by TransformImageData_impl or LookAtImageData_impl in given datatable -- superimpose = merge into filter values into last row of table; otherwise writes over with new data -- impl routine for other functions to call (doesn't do any display updating), renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  virtual void	Filter_Thread(int cmp_unit_index, int thread_no=-1);
  // threading function for doing filtering


  ///////////////////////////////////////////////////////////////////////
  // Transform Routines taking different sources for image input data

  virtual bool  ConvertImageToMatrix(taImage& img, float_Matrix& img_data);
  // #CAT_Image convert image file to img_data float matrix
  virtual bool  RecordImageName(taImage& img, DataTable* dt);
  // #CAT_Image record name of image in Name column of data table

  virtual bool	TransformImage(taImage& img, DataTable* dt,
			       float move_x=0, float move_y=0,
			       float scale = 1.0f, float rotate = 0.0f,
			       bool superimpose = false);
  // #CAT_Transform transform image data into datatable, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg), superimpose = merge into filter values into last row of table; otherwise new row is added -- impl routine for other functions to call (doesn't do any display updating)

  virtual bool	TransformImageName(const String& img_fname, DataTable* dt,
				   float move_x=0, float move_y=0,
				   float scale = 1.0f, float rotate = 0.0f,
				   bool superimpose = false);
  // #BUTTON #CAT_Transform #FILE_DIALOG_LOAD load image from file and transform image data into datatable, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg), superimpose = merge into filter values into last row of table; otherwise new row is added -- impl routine for other functions to call (doesn't do any display updating)

  virtual bool	LookAtImage(taImage& img, DataTable* dt,
			    RetinalSpacingSpec::Region region,
			    float box_ll_x, float box_ll_y,
			    float box_ur_x, float box_ur_y,
			    float move_x=0, float move_y=0,
			    float scale = 1.0f, float rotate = 0.0f,
			    bool superimpose = false);
  // #CAT_Transform transform mage data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets

  virtual bool	LookAtImageName(const String& img_fname, DataTable* dt,
				RetinalSpacingSpec::Region region,
				float box_ll_x, float box_ll_y,
				float box_ur_x, float box_ur_y,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f,
				bool superimpose = false);
  // #BUTTON #CAT_Filter #FILE_DIALOG_LOAD load image from file and transform into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets

  ///////////////////////////////////////////////////////////////////////
  // Full transform and filter in one function call (also updates with WriteClose())

  virtual bool	XFormFilterImageData(float_Matrix& img_data, DataTable* dt,
				     float move_x=0, float move_y=0,
				     float scale = 1.0f, float rotate = 0.0f,
				     bool superimpose = false, int renorm=1);
  // #CAT_Filter transform and filter image data into given datatable, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg), superimpose = merge into filter values into last row of table; otherwise new row is added, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  virtual bool	XFormFilterImage(taImage& img, DataTable* dt,
				 float move_x=0, float move_y=0,
				 float scale = 1.0f, float rotate = 0.0f,
				 bool superimpose = false, int renorm=1);
  // #CAT_Filter transform and filter image into given datatable, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified, superimpose = merge into filter values into last row of table; otherwise new row is added, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  virtual bool	XFormFilterImageName(const String& img_fname, DataTable* dt,
				     float move_x=0, float move_y=0,
				     float scale = 1.0f, float rotate = 0.0f,
				     bool superimpose = false, int renorm=1);
  // #BUTTON #CAT_Filter #FILE_DIALOG_LOAD load image from file and filter into given datatable, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg), and with normalized retinal offset as specified, superimpose = merge into filter values into last row of table; otherwise new row is added, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  ///////////////////////////////////////////////////////////////////////
  // Full transform and filter with automatic foveation of an image based on a bounding box

  virtual bool	LookAtFilterImageData(float_Matrix& img_data, DataTable* dt,
				      RetinalSpacingSpec::Region region,
				      float box_ll_x, float box_ll_y,
				      float box_ur_x, float box_ur_y,
				      float move_x=0, float move_y=0,
				      float scale = 1.0f, float rotate = 0.0f,
				      bool superimpose = false, int renorm=1);
  // #CAT_Filter transform and filter image data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params applied after foveation scaling and offsets, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  virtual bool	LookAtFilterImage(taImage& img, DataTable* dt,
				  RetinalSpacingSpec::Region region,
				  float box_ll_x, float box_ll_y,
				  float box_ur_x, float box_ur_y,
				  float move_x=0, float move_y=0,
				  float scale = 1.0f, float rotate = 0.0f,
				  bool superimpose = false, int renorm=1);
  // #CAT_Filter transform and filter image data into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  virtual bool	LookAtFilterImageName(const String& img_fname, DataTable* dt,
				      RetinalSpacingSpec::Region region,
				      float box_ll_x, float box_ll_y,
				      float box_ur_x, float box_ur_y,
				      float move_x=0, float move_y=0,
				      float scale = 1.0f, float rotate = 0.0f,
				      bool superimpose = false, int renorm=1);
  // #BUTTON #CAT_Filter #FILE_DIALOG_LOAD load image from file and transform, filter into given datatable, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets, renorm = renormalize dynamic range to max = 1 across all filters (if 0 don't do, 1 = linear renorm, 2 = log renorm, if superimpose, only do for last one!)

  ///////////////////////////////////////////////////////////////////////
  // Misc other processing operations

  virtual bool	AttendRegion(DataTable* dt, RetinalSpacingSpec::Region region = RetinalSpacingSpec::FOVEA);
  // #CAT_Filter apply attentional weighting filter to filtered values, with radius = given region

  virtual bool	RenderOccluder(DataTable* dt, float llx, float lly, float urx, float ury);
  // #CAT_Render render an occluder rectangle of given normalized size (ll = lower left corner (0,0 = farthest ll), ur = upper right (1,1 = farthest ur) using the border color -- data table must have already been loaded with Transform or LookAt routines

  // todo: need a checkconfig here..

  void	UpdateRetinaSize();	// copy retina_size to dogs..
  int 	EffFadeWidth();		// get the effective fade width value

  override taList_impl*	children_() {return &dogs;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return dogs.GetTA_Element(i, eltd); }

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinaSpec);
protected:
  void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);

  // tmp arg values for threading ops
  DataTable*	cur_dt;
  float_Matrix* cur_ret_img;
  bool		cur_superimpose;
  int 		cur_renorm;
  float		cur_rescale;
  float		cur_renorm_factor;
  int		cur_phase;	// phase of processing -- multi-step!
  float_Matrix	max_vals;
};

SmartRef_Of(RetinaSpec,TA_RetinaSpec); // RetinaSpecRef

class TA_API V1GaborSpec : public taNBase {
  // #STEM_BASE ##CAT_Image ##DEF_CHILD_gabors ##DEF_CHILDNAME_Gabor_Filters full specification of V1 gabor (oriented edge detectors) filtering -- takes output of RetinaSpec as input
INHERITED(taNBase)
public:
  GaborV1SpecList	gabors;		// the gabor (and blob) V1 filters
  bool			wrap;		// if true, then filtering has a wrap-around structure, starting at -1/2 offset (wrapped to right/top) and goes +1/2 offset past the right/top edge (wrapped to left/bottom)
  RetinaSpecRef		retina;		// the specs for the retinal filter that we follow
  float			norm_max;	// #DEF_0.95 max value to normalize output activations to -- set to -1 to turn off normalization
  float			norm_thr;	// #DEF_0.01 threshold maximum activation value for renormalizing  -- if below this value, no renormalization is applied

  virtual GaborV1Spec*	AddFilter()	{ return (GaborV1Spec*)gabors.New(1); }
  // #BUTTON #CAT_Filter add a new v1 filter

  virtual void	DefaultFilters();
  // #BUTTON #CAT_Filter create a set of default filters

  virtual bool	UpdateSizesFmRetina();
  // update the sizes of our filters based on the retina spec values

  virtual void	ConfigDataTable(DataTable* dt, bool reset_cols = false);
  // #BUTTON #CAT_Config #NULL_OK_0 #NULL_TEXT_0_NewDataTable configure a data table to hold all of the image data (if reset_cols, reset any existing cols in data table before adding new ones) (if dt == NULL, a new one is created in data.InputData)

//   virtual void	PlotSpacing(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the arrangement of the filters (centers) in the data table and generate a grid view

  virtual bool	FilterRetinaData(DataTable* v1_out_dt, DataTable* ret_in_dt);
  // Perform the filtering function: operates on output of RetinaSpec processing

//   void	UpdateRetinaSize();	// copy retina_size to dogs..

  override taList_impl*	children_() {return &gabors;}
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return gabors.GetTA_Element(i, eltd); }

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1GaborSpec);
protected:
  void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);
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

#endif // ta_imgproc_h

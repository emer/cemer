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

#ifndef VisRegionSpecBase_h
#define VisRegionSpecBase_h 1

// parent includes:
#include <ImgProcThreadBase>

// member includes:
#include <DataTableRef>
#include <taVector2i>
#include <float_Matrix>

// declare all other types mentioned but not required to include:
class CircMatrix;

taTypeDef_Of(VisRegionParams);

class E_API VisRegionParams : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image basic params for a visual region
INHERITED(taOBase)
public:
  enum Ocularity {		// ocularity configuration
    MONOCULAR,			// monocular -- only one eye
    BINOCULAR,			// binocular -- both eyes
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
    COLOR,			// has luminance on/off and R-G, B-Y color filters
  };
  enum EdgeMode {		// how to deal with edges in the filter inputs
    CLIP,			// hard clip edges -- attempts to ensure that no clipping is necessary by making filters fit within inputs, but image input is clipped
    WRAP,			// wrap the image and any subsequent stages of filtering around the edges -- no edges!
  };

  Ocularity	ocularity;	// whether two eyes or only one is present
  Region	region;		// retinal region represented by this filter 
  Resolution	res;		// level of resolution represented by this filter (can use enum or any other arbitrary rating scale -- just for informational/matcing purposes)
  Color		color;		// what level of color information to process
  EdgeMode	edge_mode;	// how to deal with edges throughout the processing cascade -- the edge_mode for the raw image transformations are in the overall RetinaProc, and are not automatically sync'd with this (they can be different)
  float		renorm_thr;	// #DEF_1e-05 threshold for the max filter output value to max-renormalize filter outputs such that the max is 1 -- below this value, consider the input to be blank and do not renorm

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionParams);
};

taTypeDef_Of(VisRegionSizes);

class E_API VisRegionSizes : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image basic size values for a visual region -- defines the size of visual image that is presented to the filters
INHERITED(taOBase)
public:
  taVector2i	retina_size;	// overall size of the retina -- defines size of images that are processed by these filters -- scaling etc typically used to fit image to retina size
  taVector2i	border;		// border around retina that we don't process -- for non-WRAP mode, typically a 1 pixel background color border is retained in the input image processing, so this should be subtracted -- also for non-WRAP mode, good to ensure that this is >= than 1/2 of the width of the filters being applied
  taVector2i	input_size;	// #READ_ONLY #SHOW size of input region in pixels that is actually filtered -- retina_size - 2 * border

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionSizes);
protected:
  void	UpdateAfterEdit_impl() override;
};

taTypeDef_Of(VisAdaptation);

class E_API VisAdaptation : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image how responsiveness adapts over time
INHERITED(taOBase)
public:
  bool		on;		// apply adaptation?
  float         up_dt;          // #CONDSHOW_ON_on #MIN_0 rate for how fast adaptation increases in magnitude -- adapt += up_dt * act
  float		dn_dt;          // #CONDSHOW_ON_on #MIN_0 rate for how fast adaptation decreases in magnitude -- adapt += -down_dt * adapt

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisAdaptation);
// protected:
//   void	UpdateAfterEdit_impl() override;
};

taTypeDef_Of(VisRegionSpecBase);

class E_API VisRegionSpecBase : public ImgProcThreadBase {
  // #STEM_BASE ##CAT_Image base class for specifying a visual image region to be filtered according to a set of filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs
INHERITED(ImgProcThreadBase)
public:
  enum ColorChannel {		// indicator of which color channel to filter on -- passed to GetImageForChan
    LUMINANCE,			// just raw luminance (monochrome / black white)
    RED_GREEN,			// red vs. green pre-subtracted -- good for gabor / spatial double-opponent filters
    BLUE_YELLOW,                // blue vs. yellow pre-subtracted -- good for gabor / spatial double-opponent filters
    YELLOW,                     // yellow channel only
  };
  enum Eye {
    LEFT,			
    RIGHT,
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
    SAVE_DEBUG	= 0x0008,	// save extra debugging-level internal computation data relevant to this processing step
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
  VisRegionParams region;	// basic parameters for the region
  VisRegionSizes  input_size;	// size of the visual input image, including any borders etc
  VisAdaptation input_adapt;    // how to adapt the input ("retinal") responses over time
  int		motion_frames;	// #MIN_0 how many frames of image information are to be retained for extracting motion signals -- 0 = no motion, 3 = typical for motion

  virtual bool 	Init();
  // #BUTTON initialize everything to be ready to start filtering -- calls InitFilters, InitOutMatrix, InitDataTable

  virtual bool	FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image = NULL,
			    bool motion_only = false);
  // main interface: filter input image(s) (if ocularity = BINOCULAR, must pass both images, else left eye is ignored) -- saves results in local output vectors, and data table if specified -- increments the time index if motion filtering -- if motion_only = true, then only process up to level of motion, for faster processing of initial frames of motion sequence

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionSpecBase);
protected:
  void	UpdateAfterEdit_impl() override;

  // cache of args for current function call
  float_Matrix* cur_img_r;	// cur right eye image arg -- only valid during filter call
  float_Matrix* cur_img_l;	// cur left eye image arg -- only valid during filter call
  float_Matrix* cur_img;	// cur image -- only valid during filter call
  float_Matrix* cur_in;		// cur input buffer -- only valid during filter call
  float_Matrix* cur_out;	// cur output buffer -- only valid during filter call
  float_Matrix* cur_in2;	// cur input buffer -- only valid during filter call
  float_Matrix* cur_in_r;	// cur input buffer -- only valid during filter call
  float_Matrix* cur_out_r;	// cur output buffer -- only valid during filter call
  float_Matrix* cur_in_l;	// cur input buffer -- only valid during filter call
  float_Matrix* cur_out_l;	// cur output buffer -- only valid during filter call
  float_Matrix* cur_adapt;      // cur adaptation values -- only valid during filter call
  CircMatrix*	cur_circ;	// current circular buffer index
  bool		rgb_img;	// is current image rgb?
  bool		wrap;		// whether edge_mode == WRAP
  bool		cur_mot_only;	// current motion_only status

  // following are all computed by PrecomputeColor function
  float_Matrix cur_img_grey;	// greyscale version of color image
  float_Matrix cur_img_y;	// YELLOW version of color image
  float_Matrix cur_img_rg;	// RED - GREEN difference
  float_Matrix cur_img_by;	// BLUE - YELLOW difference

  float_Matrix cur_img_r_adapt; // accumulation of activation over time to drive adaptation
  float_Matrix cur_img_l_adapt; // accumulation of activation over time to drive adaptation


  virtual void UpdateGeom();
  // update all geometry info -- called by UAE

  virtual bool NeedsInit();
  // test to see if the system needs to be initialized -- just check if things fit with the current computed geometries -- be sure to test for both eyes if binocular..
  virtual bool InitFilters();
  // initialize the filters -- overload for derived types and call parent
  virtual bool InitOutMatrix();
  // initialize data output matrcies to fit output of filters
  virtual bool InitDataTable();
  // initialize data table to fit data saving as configured

  virtual bool FilterImage_impl(bool motion_only = false);
  // implementation of filtering -- assumes cur_img_x args are set and everything is checked -- if motion_only = true, then only process up to level of motion, for faster processing of initial frames of motion sequence
  virtual void IncrTime();
  // increment time one step -- move the CircMatrix indexes

  virtual void 	InputAdapt_thread(int thr_no);
  // adapt input image

  virtual void  ResetAdapt();
  // reset any current adaptation present in the system -- use this for a discontinuity in the input (simulated time passing) -- operates at all levels of adaptation, where applicable

  virtual bool PrecomputeColor(float_Matrix* img);
  // convert RGB color image to grey, yellow, and R-G and B-Y in separate images, which are what should be then used for filtering (stored in cur_img_xx float matrix's) -- get via GetImageForChan method -- also sets cur_img = img -- used by GetImageForChan
  virtual float_Matrix* GetImageForChan(ColorChannel cchan);
  // get the appropriate cur_img_* guy for given color channel

  virtual bool RenormOutput(RenormMode mode, float_Matrix* out);
  // renormalize output of filters after filtering

  virtual bool ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
				     float_Matrix* left_eye_image = NULL);
  // send current input image(s)e step of dog output to data table for viewing
    virtual bool ImageToTable_impl(DataTable* dtab, float_Matrix* img, const String& col_sufx);
    // send current input image(s)e step of dog output to data table for viewing

  virtual bool FourDimMatrixToTable(DataTable* dtab, float_Matrix* out,
                                    const String& col_name, bool fmt_only = false);
  // output a four-dimensional matrix to table, with given column name

  virtual bool  OutSaveOk(DataSave save_flags);
  // check if ok to save output to data table, based on save_flags -- checks for SAVE_DATA and if ONLY_GUI flag is on and whether gui is active or not -- true = save, false = don't save

};

#endif // VisRegionSpecBase_h

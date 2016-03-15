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

taTypeDef_Of(VisColorSpace);

class E_API VisColorSpace : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image transform RGB colors into CIE XYZ and LMS perceptual space, including color opponents, and back -- note that we use 0-1 normalized XYZ and LMS values throughout - not the 0-100 as is sometimes standard it seems..
INHERITED(taOBase)
public:
  enum  OppComp {               // opponent coding components
    L_C,                        // Long wavelength = Red component
    M_C,                        // Medium wavelength = Green component
    S_C,                        // Short wavelength = Blue component
    LM_C,                       // Long + Medium wavelength = Yellow component
    LvM_C,                      // L - M opponent contrast
    SvLM_C,                     // S - L+M opponent contrast
    GREY,                       // achromatic response (grey scale lightness)
    N_OP_C,                     // number of opponent components
  };
  
  static inline float sRGBvalToLinear(const float srgb) {
    if(srgb <= 0.04045f) return srgb / 12.92f;
    return powf((srgb + 0.055f) / 1.055f, 2.4f);
  }
  // #CAT_ColorSpace convert an sRGB rgb component to linear -- used in converting from sRGB to XYZ colors

  static inline float sRGBvalFromLinear(const float lin) {
    if(lin <= 0.0031308f) return 12.92f * lin;
    return (1.055f * powf(lin, 1.0f / 2.4f) + 0.055f);
  }
  // #CAT_ColorSpace convert an sRGB rgb linear component to non-linear sRGB value -- used in converting from XYZ to sRGB 
 
  static inline void sRGBtoLinear(float& r_lin, float& g_lin, float& b_lin,
                                  const float r_s, const float g_s, const float b_s) {
    r_lin = sRGBvalToLinear(r_s); g_lin = sRGBvalToLinear(g_s); b_lin = sRGBvalToLinear(b_s);
  }
  // #CAT_ColorSpace convert set of sRGB components to linear values (gamma correction)

  static inline void sRGBfromLinear(float& r_s, float& g_s, float& b_s,
                                  const float r_lin, const float g_lin, const float b_lin) {
    r_s = sRGBvalFromLinear(r_lin); g_s = sRGBvalFromLinear(g_lin); b_s = sRGBvalFromLinear(b_lin);
  }
  // #CAT_ColorSpace convert set of sRGB components to linear values (gamma correction)

  static inline void sRGBlinToXYZ(float& X, float& Y, float& Z,
                                  const float r_lin, const float g_lin, const float b_lin) {
    X = 0.4124f * r_lin + 0.3576f * g_lin + 0.1805f * b_lin;
    Y = 0.2126f * r_lin + 0.7152f * g_lin + 0.0722f * b_lin;
    Z = 0.0193f * r_lin + 0.1192f * g_lin + 0.9505f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear into XYZ CIE standard color space

  static inline void XYZtosRGBlin(float& r_lin, float& g_lin, float& b_lin,
                                  const float X, const float Y, const float Z) {
    r_lin = 3.2406f * X + -1.5372f * Y + -0.4986f * Z;
    g_lin = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
    b_lin = 0.0557f * X + -0.2040f * Y + 1.0570f * Z;
  }
  // #CAT_ColorSpace convert XYZ CIE standard color space to sRGB linear

  static inline void sRGBtoXYZ(float& X, float& Y, float& Z,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToXYZ(X,Y,Z, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB into XYZ CIE standard color space

  static inline void XYZtosRGB(float& r_s, float& g_s, float& b_s,
                               const float X, const float Y, const float Z) {
    float r_lin, g_lin, b_lin;
    XYZtosRGBlin(r_lin, g_lin, b_lin, X,Y,Z);
    sRGBfromLinear(r_s, g_s, b_s, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert XYZ CIE standard color space into sRGB

  static inline void XYZrenormD65(float& X, float& Y, float& Z) {
    X *= (1.0f / 0.95047f); Z *= (1.0f / 1.08883);
  }
  // #CAT_ColorSpace renormalize XZY values relative to the D65 outdoor white light values
  
  static inline void XYZtoLMS_CAT02(float& L, float& M, float& S,
                                    const float X, const float Y, const float Z) {
    L = 0.7328f * X + 0.4296f * Y + -0.1624f * Z;
    M = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    S = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert XYZ to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void sRGBlinToLMS_CAT02(float& L, float& M, float& S,
                                        const float r_lin, const float g_lin, const float b_lin) {
    L = 0.3904054f * r_lin + 0.54994122f * g_lin + 0.00892632f * b_lin;
    M = 0.0708416f * r_lin + 0.96317176f * g_lin + 0.00135775f * b_lin;
    S = 0.0491304f * r_lin + 0.21556128f * g_lin + 0.9450824f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) -- this is good for representing adaptation but NOT apparently good for representing appearances
  
  static inline void sRGBtoLMS_CAT02(float& L, float& M, float& S,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToLMS_CAT02(L,M,S, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void XYZtoLMS_HPE(float& L, float& M, float& S,
                                  const float X, const float Y, const float Z) {
    L = 0.38971f * X + 0.68898f * Y + -0.07868f * Z;
    M = -0.22981f * X + 1.18340f * Y + 0.04641f * Z;
    S = Z;
  }
  // #CAT_ColorSpace convert XYZ to Long, Medium, Short cone-based responses, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline void sRGBlinToLMS_HPE(float& L, float& M, float& S,
                                      const float r_lin, const float g_lin, const float b_lin) {
    L = 0.30567503f * r_lin + 0.62274014f * g_lin + 0.04530167f * b_lin;
    M = 0.15771291f * r_lin + 0.7697197f * g_lin + 0.08807348f * b_lin;
    S = 0.0193f * r_lin + 0.1192f * g_lin + 0.9505f * b_lin;
  }
  // #CAT_ColorSpace convert sRGB linear to Long, Medium, Short cone-based responses, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) -- this is good for representing adaptation but NOT apparently good for representing appearances
  
  static inline void sRGBtoLMS_HPE(float& L, float& M, float& S,
                               const float r_s, const float g_s, const float b_s) {
    float r_lin, g_lin, b_lin;
    sRGBtoLinear(r_lin, g_lin, b_lin, r_s, g_s, b_s);
    sRGBlinToLMS_HPE(L,M,S, r_lin, g_lin, b_lin);
  }
  // #CAT_ColorSpace convert sRGB to Long, Medium, Short cone-based responses, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline void LMStoXYZ_CAT02(float& X, float& Y, float& Z,
                                    const float L, const float M, const float S) {
    X = 1.096124f * L + 0.4296f * Y + -0.1624f * Z;
    Y = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    Z = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert Long, Medium, Short cone-based responses to XYZ, using the CAT02 transform from CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02)

  static inline void LMStoXYZ_HPE(float& X, float& Y, float& Z,
                                    const float L, const float M, const float S) {
    X = 1.096124f * L + 0.4296f * Y + -0.1624f * Z;
    Y = -0.7036f * X + 1.6975f * Y + 0.0061f * Z;
    Z = 0.0030f * X + 0.0136f * Y + 0.9834 * Z;
  }
  // #CAT_ColorSpace convert Long, Medium, Short cone-based responses to XYZ, using the Hunt-Pointer-Estevez transform -- this is closer to the actual response functions of the L,M,S cones apparently

  static inline float LuminanceAdaptation(const float bg_lum = 200.0f) {
    const float lum5 = 5.0f * bg_lum;
    float k = 1.0f / (lum5 + 1.0f);
    float k4 = k*k*k*k;
    float k4m1 = 1.0f - k4;
    float fl = .2f * k4 * lum5 + .1f * k4m1 * k4m1 * powf(lum5, 1.0f / 3.0f);
    return fl;
  }
  // #CAT_ColorSpace Luminance adaptation function -- equals 1 at background luminance of 200 so we generally ignore it..

  static inline float ResponseCompression(const float val) {
    float pval = powf(val, 0.42f);
    float rc = 0.1f + 4.0f * pval / (27.13f + pval);
    return rc;
  }
  // takes a 0-1 normalized LMS value and performs hyperbolic response compression -- val must ALREADY have the luminance adaptation applied to it using the luminance adaptation function, which is 1 at a background luminance level of 200 = 2, so you can skip that step if you assume that level of background

  static inline void LMStoOpponents(float& L_c, float& M_c, float& S_c, float& LM_c,
                                    float& LvM, float& SvLM, float& grey,
                                    const float L, const float M, const float S) {
    float L_rc = ResponseCompression(L); float M_rc = ResponseCompression(M); float S_rc = ResponseCompression(S);
    const float LmM = L_rc - M_rc;  const float MmS = M_rc - S_rc;  const float SmL = S_rc - L_rc;
    L_c = L_rc + (1.0f / 11.0f) * S_rc; 
    M_c = (12.0f / 11.0f) * M_rc; 
    LvM = L_c - M_c; // red-green subtracting "criterion for unique yellow"
    LM_c = (1.0f / 9.0f) * (L_rc + M_rc);
    S_c = (2.0f / 9.0f) * S_rc;
    SvLM = S_c - LM_c;          // blue-yellow contrast
    grey = 2.0f * L_rc + M_rc + .05f * S_rc - 0.305f;
    // if(grey < 0.0f) grey = 0.0f;
    // note: last term should be: 0.725 * (1/5)^-0.2 = grey background assumption (Yb/Yw = 1/5) = 1
  }
  // convert Long, Medium, Short cone-based responses to opponent components: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02
  
  static inline void sRGBtoOpponents(float& L_c, float& M_c, float& S_c, float& LM_c,
                                     float& LvM, float& SvLM, float& grey,
                                     const float r_s, const float g_s, const float b_s) {
    float L,M,S;
    sRGBtoLMS_HPE(L,M,S, r_s, g_s, b_s);
    LMStoOpponents(L_c, M_c, S_c, LM_c, LvM, SvLM, grey, L, M, S);
  }
  // #CAT_ColorSpace convert sRGB to opponent components via LMS using the HPE cone values: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02
  
  static void sRGBtoOpponentsImg(float_Matrix& opp_img, const float_Matrix& srgb_img);
  // #CAT_ColorSpace convert sRGB image to opponent components image (3d matrix of 2D images with outer dimension =7: L_c, M_c, S_c, LM_c, LvM, SvLM, grey -- done via LMS using the HPE cone values: Red - Green (LvM) and Blue - Yellow (SvLM) -- includes the separate components in these subtractions as well -- uses the CIECAM02 color appearance model (MoroneyFairchildHuntEtAl02) https://en.wikipedia.org/wiki/CIECAM02


  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisColorSpace);
// protected:
//   void	UpdateAfterEdit_impl() override;
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
    GREY,			// achromatic response -- lightness or luminance (monochrome / black white)
    RED_GREEN,			// red vs. green pre-subtracted -- good for gabor / spatial double-opponent filters
    BLUE_YELLOW,                // blue vs. yellow pre-subtracted -- good for gabor / spatial double-opponent filters
    RED,                        // red
    GREEN,                      // green
    BLUE,                       // blue 
    YELLOW,                     // yellow
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
  
  // following are all computed by PrecomputeColor function
  float_Matrix cur_img_opp;	// #READ_ONLY #NO_SAVE opponent component version of color image -- 7 diff factors in outer dim, as follows:
  float_MatrixPtr cur_img_L_c;	// #READ_ONLY #NO_SAVE RED = L channel
  float_MatrixPtr cur_img_M_c;	// #READ_ONLY #NO_SAVE GREEN = M channel
  float_MatrixPtr cur_img_S_c;	// #READ_ONLY #NO_SAVE BLUE = S channel
  float_MatrixPtr cur_img_LM_c;	// #READ_ONLY #NO_SAVE YELLOW = L+M channel 
  float_MatrixPtr cur_img_LvM;	// #READ_ONLY #NO_SAVE RED - GREEN opponent
  float_MatrixPtr cur_img_SvLM;	// #READ_ONLY #NO_SAVE BLUE - YELLOW opponent
  float_MatrixPtr cur_img_grey;	// #READ_ONLY #NO_SAVE monochrome response

  float_Matrix cur_img_r_adapt; // #READ_ONLY #NO_SAVE accumulation of activation over time to drive adaptation
  float_Matrix cur_img_l_adapt; // #READ_ONLY #NO_SAVE accumulation of activation over time to drive adaptation
  
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
  // convert RGB color image to cone response-based opponent color values, which are what should be then used for filtering (stored in cur_img_xx float matrix's) -- get via GetImageForChan method -- also sets cur_img = img -- used by GetImageForChan
  virtual float_Matrix* GetImageForChan(ColorChannel cchan);
  // get the appropriate cur_img_* guy for given color channel -- only avail if PrecomputeColor has been called

  virtual bool RenormOutput(RenormMode mode, float_Matrix* out);
  // renormalize output of filters after filtering

  virtual bool ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
                            float_Matrix* left_eye_image = NULL, bool fmt_only = false);
  // send current input image(s)e step of dog output to data table for viewing
    virtual bool ImageToTable_impl(DataTable* dtab, float_Matrix* img,
                                   const String& col_sufx, bool fmt_only = false);
    // send current input image(s)e step of dog output to data table for viewing

  virtual bool FourDimMatrixToTable(DataTable* dtab, float_Matrix* out,
                                    const String& col_name, bool fmt_only = false);
  // output a four-dimensional matrix to table, with given column name

  virtual bool  OutSaveOk(DataSave save_flags);
  // check if ok to save output to data table, based on save_flags -- checks for SAVE_DATA and if ONLY_GUI flag is on and whether gui is active or not -- true = save, false = don't save

};

#endif // VisRegionSpecBase_h

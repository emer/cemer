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

#ifndef V1RegionSpec_h
#define V1RegionSpec_h 1

// parent includes:
#include <VisRegionSpecBase>

// member includes:
#include <V1KwtaSpec>
#include <XYNGeom>
#include <float_Matrix>
#include <int_Matrix>
#include <CircMatrix>

// declare all other types mentioned but not required to include:
class DataTable; // 
class V1RetinaProc; // 

taTypeDef_Of(V1GaborSpec);

class E_API V1GaborSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 simple cells as gabor filters: 2d Gaussian envelope times a sinusoidal plane wave -- by default produces 2 phase asymmetric edge detector filters
INHERITED(taOBase)
public:
  bool          on;             // is this filter active?
  float         wt;             // #CONDSHOW_ON_on how much relative weight does this filter have when combined with other filters (e.g., in the polarity-independent filters)
  float		gain;		// #CONDSHOW_ON_on #DEF_2 overall gain multiplier applied after gabor filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  int		size;	        // #AKA_filter_size #CONDSHOW_ON_on #DEF_6;8;12;16;24;48;64 size of the overall filter -- number of pixels wide and tall for a square matrix used to encode the filter -- filter is centered within this square -- computational speed advantage for it to be a multiple of 4
  int		spacing;	// #CONDSHOW_ON_on how far apart to space the centers of the gabor filters -- 1 = every pixel, 2 = every other pixel, etc -- high-res should be 1 or 2, lower res can be increments therefrom
  float		wvlen;		// #CONDSHOW_ON_on #DEF_6;12;18;24;48;64  wavelength of the sine waves -- number of pixels over which a full period of the wave takes place (computation adds a 2 PI factor to translate into pixels instead of radians)
  float		gauss_sig_len;	// #CONDSHOW_ON_on #DEF_0.225;0.3 gaussian sigma for the length dimension (elongated axis perpendicular to the sine waves) -- normalized as a function of size
  float		gauss_sig_wd;	// #CONDSHOW_ON_on #DEF_0.15;0.2 gaussian sigma for the width dimension (in the direction of the sine waves) -- normalized as a function of size
  float		phase_off;	// #CONDSHOW_ON_on #DEF_0;1.5708 offset for the sine phase -- can make it into a symmetric gabor by using PI/2 = 1.5708
  bool		circle_edge;	// #CONDSHOW_ON_on #DEF_true cut off the filter (to zero) outside a circle of diameter size -- makes the filter more radially symmetric
  int		n_angles;	// #CONDSHOW_ON_on #DEF_4 number of different angles encoded -- currently only 4 is supported

  virtual void	RenderFilters(float_Matrix& fltrs);
  // generate filters into the given matrix, which is formatted as: [size][size][n_angles]

  virtual void	GridFilters(float_Matrix& fltrs, DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  TA_SIMPLE_BASEFUNS(V1GaborSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

taTypeDef_Of(V1sNeighInhib);

class E_API V1sNeighInhib : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image neighborhood inhibition for V1 simple layer -- inhibition spreads along orthogonal orientations to line tuning, to eliminate redundant reps of the same underlying edge
INHERITED(taOBase)
public:
  bool		on;		// whether to use neighborhood inhibition
  int		inhib_d; 	// #CONDSHOW_ON_on #DEF_1 distance of neighborhood for inhibition to apply to same feature in neighboring locations spreading out on either side along the orthogonal direction relative to the orientation tuning
  float		inhib_g;	// #CONDSHOW_ON_on #DEF_0.6:1 [0.6 for FFFB, 0.8 for KWTA] gain factor for feature-specific inhibition from neighbors -- this proportion of the neighboring feature's threshold-inhibition value (used in computing kwta) is spread among neighbors according to inhib_d distance

  int		tot_ni_len;	// #READ_ONLY total length of neighborhood inhibition stencils = 2 * neigh_inhib_d + 1

  TA_SIMPLE_BASEFUNS(V1sNeighInhib);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

taTypeDef_Of(V1MotionSpec);

class E_API V1MotionSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 motion coding by simple cells
INHERITED(taOBase)
public:
  bool		r_only;		// #DEF_true perform motion computation only on right eye signals -- this is the dominant eye and often all that is needed
  int		n_speeds;	// #DEF_1 for motion coding, number of speeds in each direction to encode separately -- only applicable if motion_frames > 1
  int		speed_inc;	// #DEF_1 how much to increase speed for each speed value -- how fast is the slowest speed basically
  int		tuning_width;	// #DEF_1 additional width of encoding around the trajectory for the target speed -- allows for some fuzziness in encoding -- effective value is multiplied by speed, so it gets fuzzier as speed gets higher
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for weighting the contribution of extra width guys -- normalized by effective tuning_width
  float		opt_thr;	// #DEF_0.01 optimization threshold -- skip if current value is below this value

  int		tot_width;	// #READ_ONLY total width = 1 + 2 * tuning_width

  TA_SIMPLE_BASEFUNS(V1MotionSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

taTypeDef_Of(V1ComplexSpec);

class E_API V1ComplexSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 complex cells, which integrate over v1 simple polarity invariant inputs to compute length sum and end stopping detectors
INHERITED(taOBase)
public:
  int		sg_rf;		// #DEF_1;2;4 #MIN_1 #MAX_4 only valid values are 1,2,4 -- number of v1 simple cells to sub-group over prior to computing subsequent steps (length sum, end stop) -- this grouping provides more isotropic coverage of the space, reduces the computational cost of subsequent steps, and also usefully makes it more robust to minor variations -- downside is that larger sg_n values produce more dense, overlapping activation patterns, which can ber problematic in cluttered visual scenes
  int		sg_spc;	        // #AKA_sg_spacing #DEF_1;2 #MIN_1 spacing between sub-group centers (see sg_rf) -- typically this works best as 1/2 of sg_rf size for half-overlap, but to stretch resolution the same value as sg_rf also does work, but typically leads to worse performance due to aliasing effects
 
  int		len_sum_len;	// #DEF_1 length (in pre-grouping of v1s/b rf's) beyond rf center (aligned along orientation of the cell) to integrate length summing -- this is a half-width, such that overall length is 1 + 2 * len_sum_len
  float		es_thr;		// #DEF_0.2 threshold for end stopping activation -- there are typically many "ghost" end stops, so this filters those out

  int		sg_half;	// #READ_ONLY sg_rf / 2
  int		sg_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  int		len_sum_width;	// #READ_ONLY 1 + 2 * len_sum_len -- computed
  float		len_sum_norm;	// #READ_ONLY 1.0 / len_sum_width -- normalize sum

  TA_SIMPLE_BASEFUNS(V1ComplexSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

taTypeDef_Of(VisSpatIntegSpec);

class E_API VisSpatIntegSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image spatial integration parameters for visual signals -- happens as last step after all other feature detection operations -- performs a MAX or AVG over rfields
INHERITED(taOBase)
public:
  taVector2i	spat_rf;	// integrate over this many spatial locations (uses MAX operator over gaussian weighted filter matches at each location) in computing the response of the v1c cells -- produces a larger receptive field -- always uses 1/2 overlap spacing
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for spatial rf -- weights the contribution of more distant locations more weakly
  bool		sum_rf;		// #DEF_false sum over the receptive field instead of computing the max (actually computes the average instead of sum)

  taVector2i	spat_half;	// #READ_ONLY half rf
  taVector2i	spat_spacing;	// #READ_ONLY 1/2 overlap spacing with spat_rf
  taVector2i	spat_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  TA_SIMPLE_BASEFUNS(VisSpatIntegSpec);
protected:
  void 	UpdateAfterEdit_impl() override;
private:
  void 	Initialize();
  void	Destroy() { };
};

taTypeDef_Of(V1RegionSpec);

class E_API V1RegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of V1 simple and complex filters -- used as part of overall V1Proc processing object -- produces Gabor and more complex filter activation outputs directly from image bitmap input -- each region is a separate matrix column in a data table (and network layer), and has a specified spatial resolution
INHERITED(VisRegionSpecBase)
public:
  enum ComplexFilters { // #BITS flags for specifying which complex filters to include
    CF_NONE	= 0, // #NO_BIT
    LEN_SUM	= 0x0001, // length summing cells -- just average along oriented line
    END_STOP	= 0x0002, // end stop cells -- len sum minus single same orientation point after a gap -- requires LEN_SUM
    V1C_V1S     = 0x0004, // include V1 simple (polarized) in the V1C output -- this is independent of SI_V1S_SG -- use for cases where SI is not used -- if sg4 is on, then the V1S will first be integrated over sg4
    CF_DEFAULT  = LEN_SUM | END_STOP,  // #IGNORE #NO_BIT this is the default setup
  };

  enum OptionalFilters { // #BITS flags for specifying additional output filters -- misc grab bag of outputs
    OF_NONE	= 0, // #NO_BIT
    ENERGY	= 0x0001, // overall energy of V1S feature detectors -- a single unit activation per spatial location in same resolution map as V1S -- output map is just 2D, not 4D -- and is always saved to a separate output column -- suitable for basic spatial mapping etc
  };

  enum SpatIntegFilters { // #BITS flags for specifying what is subject to additional spatial integration -- multiple can be selected
    SI_NONE	= 0, // #NO_BIT
    SI_V1S	= 0x0001, // V1 simple cell, with all selected polarities and color contrasts (but not motion or binocular depth)
    SI_V1PI	= 0x0002, // V1 polarity invariance, which just has angles and does a max over polarities and color contrasts -- lower dimensionality
    SI_V1PI_SG	= 0x0004, // V1 square grouped polarity invariance -- lower resolution grouped version of polarity invariant reps
    SI_V1S_SG	= 0x0008, // V1 square grouped raw V1S full polarity/color values -- lower resolution grouped version of V1S reps -- note that the SG version of V1S is not otherwise needed so this is what triggers its computation -- also unless SEP_MATRIX is flagged, this is output in one table with SI_V1C if those are also selected
    SI_V1C	= 0x0010, // V1 complex, which is length sum and end stop operating on top of V1SG square grouped (if sg4 option selected)
  };

  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X = 0,
    Y = 1,
  };
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE = 0,	   // along the direction of the line
    ORTHO = 1,	   // orthogonal to the line
  };
  enum OnOff {   // on (excitatory) vs. off (inhibitory)
    ON = 0,
    OFF = 1,
  };
  enum LeftRight {   // left vs. right -- directionality
    LEFT = 0,
    RIGHT = 1,
  };
  enum Angles {   // angles, 4 total
    ANG_0 = 0,
    ANG_45 = 1,
    ANG_90 = 2,
    ANG_135 = 3,
  };
  enum AngDir {   // ang = angle, dir = direction
    ANG = 0,
    DIR = 1,
  };

  /////////// Simple
  V1GaborSpec	v1s_specs;	// specs for V1 simple filters, computed using gabor filters directly onto the incoming image
  V1GaborSpec	v1s_specs_2;	// specs for second set of V1 simple filters, computed using gabor filters directly onto the incoming image -- MUST have same n_angles, spacing as v1s_specs, and generally good to have same size
  V1GaborSpec	v1s_specs_3;	// specs for third set of V1 simple filters, computed using gabor filters directly onto the incoming image -- MUST have same n_angles, spacing as v1s_specs, and generally good to have same size
  RenormMode	v1s_renorm;	// #DEF_NO_RENORM how to renormalize the output of v1s static filters -- applied prior to kwta
  V1KwtaSpec	v1s_kwta;	// k-winner-take-all inhibitory dynamics for the v1 simple stage -- important for cleaning up these representations for subsequent stages, especially binocluar disparity and motion processing, which require correspondence matching, and end stop detection
  V1sNeighInhib	v1s_neigh_inhib; // specs for V1 simple neighborhood-feature inhibition -- inhibition spreads in directions orthogonal to the orientation of the features, to prevent ghosting effects around edges
  VisAdaptation v1s_adapt;      // how to adapt the v1 simple cell responses over time
  DataSave	v1s_save;	// how to save the V1 simple outputs for the current time step in the data table
  XYNGeom	v1s_img_geom; 	// #READ_ONLY #SHOW size of v1 simple filtered image output -- number of hypercolumns in each axis to cover entire output
  XYNGeom	v1s_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 simple filtering -- n_angles (x) * 2 or 6 polarities (y; monochrome|color) -- configured automatically

  /////////// Motion
  V1MotionSpec	v1s_motion;	// #CONDSHOW_OFF_motion_frames:0||motion_frames:1 specs for V1 motion filters within the simple processing layer
  RenormMode	v1m_renorm;	// #CONDSHOW_OFF_motion_frames:0||motion_frames:1 #DEF_NO_RENORM how to renormalize the output of v1s motion filters
  XYNGeom	v1m_feat_geom; 	// #READ_ONLY size of one 'hypercolumn' of features for V1 motion filtering -- always x = angles; y = 2 * speeds

  /////////// Complex
  ComplexFilters v1c_filters; 	// which complex cell filtering to perform -- length sum is required for end stop, so you cannot just have end stop by itself.  the order of options here is the order in which they are output to the unit group (length sum = 1st row, then end stop for 2nd & 3rd rows, then v1s for 4th and 5th)
  V1ComplexSpec v1c_specs;	// #CONDSHOW_OFF_v1c_filters:0 specs for V1 complex filters -- comes after V1 binocular processing 
  RenormMode	v1c_renorm;	// #CONDSHOW_OFF_v1c_filters:0 #DEF_NO_RENORM how to renormalize the output of v1c filters, prior to kwta -- currently only applies to length sum
  DataSave	v1c_save;	// #CONDSHOW_OFF_v1c_filters:0 how to save the V1 complex outputs for the current time step in the data table

  XYNGeom	v1sg_img_geom; 	// #READ_ONLY size of v1 square grouping output image geometry -- input is v1s_img_geom, with either 2x2 or 4x4 spacing of square grouping operations reducing size by that amount
  XYNGeom	v1c_img_geom; 	// #CONDSHOW_OFF_v1c_filters:0 #READ_ONLY #SHOW size of v1 complex filtered image output -- number of hypercolumns in each axis to cover entire output -- this is equal to v1sq_img_geom if sq_gp4 is on, or v1s_img_geom if not
  XYNGeom	v1c_feat_geom; 	// #CONDSHOW_OFF_v1c_filters:0 #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 complex filtering -- includes length sum and end stop in combined output -- configured automatically with x = n_angles

  SpatIntegFilters spat_integ;	// what to perform spatial integration over
  VisSpatIntegSpec si_specs;	// #CONDSHOW_OFF_spat_integ:0 spatial integration output specs
  RenormMode	si_renorm;	// #CONDSHOW_OFF_spat_integ:0 #DEF_NO_RENORM how to renormalize spat integ output prior to performing kwta
  V1KwtaSpec	si_kwta;	// #CONDSHOW_OFF_spat_integ:0 k-winner-take-all inhibitory dynamics for spatial integration output -- 
  DataSave	si_save;	// #CONDSHOW_OFF_spat_integ:0 how to save the spatial integration outputs for the current time step in the data table
  XYNGeom	si_v1c_geom; 	// #CONDSHOW_OFF_spat_integ:0 #READ_ONLY #SHOW size of spat integ v1c image output

  OptionalFilters opt_filters; 	// optional filter outputs -- always rendered to separate tables in data table
  DataSave	opt_save;	// #CONDSHOW_OFF_opt_filters:0 how to save the optional outputs for the current time step in the data table

  //////////////////////////////////////////////////////////////
  //	Geometry and Stencils

  ///////////////////  V1S Geom/Stencils ////////////////////////
  int		n_colors;	// #READ_ONLY number of color channels to be processed (1 = monochrome, 4 = full color)
  int		n_polarities;	// #READ_ONLY #DEF_2 number of polarities per color -- always 2
  int		n_polclr;	// #READ_ONLY number of polarities * number of colors -- y dimension of simple features for example
  float_Matrix	v1s_gabor_filters; // #READ_ONLY #NO_SAVE gabor filters for v1s processing [size][size][n_angles]
  float_Matrix	v1s_gabor_filters_2; // #READ_ONLY #NO_SAVE gabor filters for v1s processing [size][size][n_angles]
  float_Matrix	v1s_gabor_filters_3; // #READ_ONLY #NO_SAVE gabor filters for v1s processing [size][size][n_angles]
  float_Matrix	v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles
  float_Matrix	v1s_ang_slopes_raw; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each of the angles -- non-normalized
  int_Matrix	v1s_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]

  ////////////////// V1M Motion Geom/Stencils
  int		v1m_in_polarities; // #READ_ONLY number of polarities used in v1 motion input processing (for history, etc) -- always set to 1 -- using polarity invariant inputs
  float_Matrix	v1m_weights;  	// #READ_ONLY #NO_SAVE v1 simple motion weighting factors (1d)
  int_Matrix	v1m_stencils; 	// #READ_ONLY #NO_SAVE stencils for motion detectors, in terms of v1s location offsets through time [x,y][1+2*tuning_width][motion_frames][directions:2][angles][speeds] (6d)
  int_Matrix	v1m_still_stencils; // #READ_ONLY #NO_SAVE stencils for motion detectors -- detecting stillness, in terms of v1s location offsets through time [x,y][1+2*tuning_width][motion_frames][angles] (4d)

  ///////////////////  V1C Geom/Stencils ////////////////////////
  int_Matrix	v1sg4_stencils; 	// #READ_ONLY #NO_SAVE stencils for v1 square 4x4 grouping -- represents center points of the lines for each angle [x,y,len][10][angles] -- there are 10 points for the 2 diagonal lines with 4 angles and 8 for horiz, vert -- only works if n_angles = 4 and line_len = 4 or 5
  int_Matrix	v1sg2_stencils; 	// #READ_ONLY #NO_SAVE stencils for v1 square 2x2 grouping -- represents center points of the lines for each angle [x,y,len][4][angles] -- there are 5 points for the 2 diagonal lines with 4 angles -- only works if n_angles = 4 and line_len = 4 or 5
  int_Matrix	v1ls_stencils;  // #READ_ONLY #NO_SAVE stencils for complex length sum cells [x,y][len_sum_width][angles]
  int_Matrix	v1ls_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]
  int_Matrix	v1es_stencils;  // #READ_ONLY #NO_SAVE stencils for complex end stop cells [x,y][pts=3(only stop)][len_sum,stop=2][dirs=2][angles] -- new version

  ///////////////////  Spat Integ Stencils / Geom
  float_Matrix	si_weights;	// #READ_ONLY #NO_SAVE spatial integration weights for weighting across rf
  XYNGeom	si_v1s_geom; 	// #READ_ONLY size of spat integ v1s image output
  XYNGeom	si_v1sg_geom; 	// #READ_ONLY size of spat integ v1sg image output

  //////////////////////////////////////////////////////////////
  //	Outputs

  ///////////////////  V1S Output ////////////////////////
  float_Matrix	v1s_out_r_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_l_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_r_adapt; // #READ_ONLY #NO_SAVE adaptation values for v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_l_adapt; // #READ_ONLY #NO_SAVE adaptation values for v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_gci;	 // #READ_ONLY #NO_SAVE v1 simple cell inhibitory conductances, for computing kwta
  float_Matrix	v1s_nimax;       // #READ_ONLY #NO_SAVE v1 simple cell neighbor inhibition values -- MAX of neighbor feature input activations
  float_Matrix	v1s_out_r;	 // #READ_ONLY #NO_SAVE v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1s_out_l;	 // #READ_ONLY #NO_SAVE v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off,2-6=colors if used]
  float_Matrix	v1pi_out_r;  	 // #READ_ONLY #NO_SAVE polarity invariance over v1 simple cell output -- max over polarities, right eye [feat.x][1][img.x][img.y]
  float_Matrix	v1pi_out_l;  	 // #READ_ONLY #NO_SAVE polarity invariance over v1 simple cell output -- max over polarities, left eye [feat.x][1][img.x][img.y]

  ///////////////////  V1M Motion Output ////////////////////////
  float_Matrix	v1m_out_r;	 // #READ_ONLY #NO_SAVE v1 motion cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = speed * dir * [0=on,1=off -- luminance only]
  float_Matrix	v1m_out_l;	 // #READ_ONLY #NO_SAVE v1 motion cell output, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = speed * dir * [0=on,1=off -- luminance only] 
  float_Matrix	v1m_maxout_r;	 // #READ_ONLY #NO_SAVE v1 motion cell output -- max over directions, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = v1m_in_polarities
  float_Matrix	v1m_maxout_l;	 // #READ_ONLY #NO_SAVE v1 motion cell output -- max over directions, left eye [feat.x][feat.y][img.x][img.y] -- feat.y = v1m_in_polarities
  float_Matrix	v1m_hist_r;	 // #READ_ONLY #NO_SAVE history of v1 simple cell output for motion computation, right eye [feat.x][feat.y][img.x][img.y][time] -- feat.y = [0=on,1=off -- luminance only]
  float_Matrix	v1m_hist_l;	 // #READ_ONLY #NO_SAVE history of v1 simple cell output for motion computation, left eye [feat.x][feat.y][img.x][img.y][time] -- feat.y = [0=on,1=off -- luminance only]
  CircMatrix	v1m_circ_r;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time -- attached to v1m_hist_r
  CircMatrix	v1m_circ_l;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time -- attached to v1m_hist_l
  float_Matrix	v1m_still_r; // #READ_ONLY #NO_SAVE places with stable features across motion window (no motion), for each v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off -- luminance only]
  float_Matrix	v1m_still_l; // #READ_ONLY #NO_SAVE places with stable features across motion window (no motion), for each v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y] -- feat.y = [0=on,1=off -- luminance only]

  ///////////////////  V1C Complex Output ////////////////////////
  float_Matrix	v1sg_out_r;	 // #READ_ONLY #NO_SAVE square 4x4 grouping of polarity invariant V1 reps -- reduces dimensionality and introduces robustness -- operates on v1pi inputs [v1pi_feat.x][1][v1sq_img.x][v1sq_img.y]
  float_Matrix	v1sg_out_l;	 // #READ_ONLY #NO_SAVE square 4x4 grouping of polarity invariant V1 reps -- reduces dimensionality and introduces robustness -- operates on v1pi inputs [v1pi_feat.x][1][v1sq_img.x][v1sq_img.y]
  float_Matrix	v1ls_out_r;	 // #READ_ONLY #NO_SAVE length sum output after kwta [feat.x][1][v1c_img.x][v1c_img.y]
  float_Matrix	v1ls_out_l;	 // #READ_ONLY #NO_SAVE length sum output after kwta [feat.x][1][v1c_img.x][v1c_img.y]
  float_Matrix	v1ls_gci;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory conductances, for computing kwta
  float_Matrix	v1es_out_r;	 // #READ_ONLY #NO_SAVE end stopping output -- operates on length sum and raw v1s/v1pi input [feat.x][2][v1c_img.x][v1c_img.y]
  float_Matrix	v1es_out_l;	 // #READ_ONLY #NO_SAVE end stopping output -- operates on length sum and raw v1s/v1pi input [feat.x][2][v1c_img.x][v1c_img.y]
  float_Matrix	v1es_gci;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory conductances, for computing kwta
  float_Matrix	v1cs_sg_out_r;	 // #READ_ONLY #NO_SAVE square grouping of v1s polarized
  float_Matrix	v1cs_sg_out_l;	 // #READ_ONLY #NO_SAVE square grouping of v1s polarized

  ///////////////////  SI Spatial Integration Output ////////////////////////
  float_Matrix	si_gci;	 	// #READ_ONLY #NO_SAVE inhibitory conductances, for computing kwta
  float_Matrix	si_v1s_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_out_raw;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_sg_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1pi_sg_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_sg_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1s_sg_out_raw; // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	v1s_sg_out;	 // #READ_ONLY #NO_SAVE square grouping of v1s 
  float_Matrix	v1s_sg_out_raw;	 // #READ_ONLY #NO_SAVE square grouping of v1s 
  float_Matrix	si_v1c_out;	 // #READ_ONLY #NO_SAVE spatial integration
  float_Matrix	si_v1c_out_raw;	 // #READ_ONLY #NO_SAVE spatial integration

  ///////////////////  OPT optional Output ////////////////////////
  float_Matrix	energy_out;	 // #READ_ONLY #NO_SAVE energy at each location: max activation over features for each image location -- [img.x][img.y]

  int		AngleDeg(int ang_no);
  // get angle value in degress based on angle number
  virtual void	GridGaborFilters(DataTable* disp_data, int which_filters = 1);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 Gabor Filters into the data table -- which_filters = 1 for basic ones, 2 = 2nd set, 3 = 3rd set
  virtual void	GridV1Stencils(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 stencils into data table and generate a grid view -- these are the effective receptive fields at each level of processing
  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view -- one row for each type of filter (scroll to see each in turn) -- light squares show bounding box of rf, skipping every other

  TA_SIMPLE_BASEFUNS(V1RegionSpec);
protected:
  float_Matrix* cur_out_acts;	// cur output activations -- for kwta thing
  float_Matrix* cur_still;	// cur still for motion
  float_Matrix* cur_maxout;	// cur maxout for motion
  float_Matrix* cur_hist;	// cur hist for motion
  float_Matrix* cur_v1b_in_r;	// current v1b input, r
  float_Matrix* cur_v1b_in_l;	// current v1b input, l
  float_Matrix* cur_v1s_gabor_filter;	// currrent gabor filter
  int           cur_v1s_off;            // v1s y offset

  void	UpdateAfterEdit_impl() override;

  void	UpdateGeom() override;

  bool NeedsInit() override;
  bool InitFilters() override;
  bool InitOutMatrix() override;
  bool InitDataTable() override;

  virtual bool	InitFilters_V1Simple();
  virtual bool	InitFilters_V1Motion();
  virtual bool	InitFilters_V1Complex();
  virtual bool	InitFilters_SpatInteg();

  bool	FilterImage_impl(bool motion_only = false) override;
  void  IncrTime() override;
  void  ResetAdapt() override;

  virtual bool	V1SimpleFilter();
  // do simple filters -- main wrapper
  virtual bool	V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
				      float_Matrix* out, float_Matrix* adapt);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Static_thread(int thr_no);
  // do simple filters, static only on current inputs -- do it
  virtual void 	V1SimpleFilter_Static_neighinhib_thread(int thr_no);
  // do neighborhood inhibition on simple filters
  virtual bool 	V1SimpleFilter_PolInvar(float_Matrix* v1s_out_in, float_Matrix* v1pi_out);
  // polarity invariance: max polarities of v1s_out 
  virtual void 	V1SimpleFilter_PolInvar_thread(int thr_no);
  // polarity invariance: max polarities of v1s_out 

  virtual bool	V1SimpleFilter_Motion(float_Matrix* in, float_Matrix* out, float_Matrix* maxout, 
		      float_Matrix* still, float_Matrix* hist, CircMatrix* circ);
  // do simple filters, motion on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Motion_thread(int thr_no);
  // do simple filters, motion on current inputs -- do it
  virtual void 	V1SimpleFilter_Motion_CpHist_thread(int thr_no);
  // do simple motion filters, copy v1s to history
  virtual void 	V1SimpleFilter_Motion_Still_thread(int thr_no);
  // do simple motion filters, compute non-moving (still) background

  virtual bool	V1ComplexFilter();
  // do complex filters -- dispatch threads
  virtual void 	V1ComplexFilter_SqGp(float_Matrix* pi_in, float_Matrix* sg_out);
  // square-group if selected
  virtual void 	V1ComplexFilter_LenSum(float_Matrix* ls_in, float_Matrix* ls_out);
  // length-sum
  virtual void 	V1ComplexFilter_EndStop(float_Matrix* pi_in, float_Matrix* ls_in,
                                        float_Matrix* es_out);
  // end stop
  virtual void  V1ComplexFilter_V1S_SqGp(float_Matrix* v1s_in, float_Matrix* sg_out);
  // v1s sg4
  virtual void 	V1ComplexFilter_SqGp_thread(int thr_no);
  // square-group if selected
  virtual void 	V1ComplexFilter_LenSum_thread(int thr_no);
  // length-sum
  virtual void 	V1ComplexFilter_EndStop_thread(int thr_no);
  // end stop

  virtual bool	SpatIntegFilter();
  // do spatial integration filters -- dispatch threads
  virtual void 	SpatIntegFilter_V1S_thread(int thr_no);
  virtual void 	SpatIntegFilter_V1PI_thread(int thr_no);
  virtual void 	SpatIntegFilter_V1PI_SG_thread(int thr_no);
  virtual void 	SpatIntegFilter_V1S_SqGp_thread(int thr_no);
  virtual void 	SpatIntegFilter_V1S_SG_thread(int thr_no);
  virtual void 	SpatIntegFilter_V1C_thread(int thr_no);

  virtual bool	V1OptionalFilter();
  // do optional filters -- dispatch threads
  virtual void 	V1OptionalFilter_Energy_thread(int thr_no);
  // energy as max activation output of v1pi_out_r

  virtual bool V1SOutputToTable(DataTable* dtab, bool fmt_only = false);
  // simple to output table
  virtual bool V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out, const String& col_sufx,
				     bool fmt_only = false);
  // simple to output table impl
  virtual bool V1MOutputToTable_impl(DataTable* dtab, float_Matrix* out, float_Matrix* maxout,
     float_Matrix* still, float_Matrix* hist, CircMatrix* circ, const String& col_sufx,
     bool fmt_only = false);
  // motion to output table impl
  virtual bool V1COutputToTable(DataTable* dtab, bool fmt_only = false);
  // complex to output table
  virtual bool V1COutputToTable_impl(DataTable* dtab, float_Matrix* ls_out,
                                     float_Matrix* es_out, float_Matrix* sg_out, 
                                     float_Matrix* v1sc_sg_out, float_Matrix* v1s_out,
                                     const String& col_sufx, bool fmt_only = false);
  // complex to output table
  virtual bool SIOutputToTable(DataTable* dtab, bool fmt_only = false);
  // Spat Invar to output table
  virtual bool OptOutputToTable(DataTable* dtab, bool fmt_only = false);
  // optional to output table
private:
  void 	Initialize();
  void	Destroy() { };
};

#endif // V1RegionSpec_h

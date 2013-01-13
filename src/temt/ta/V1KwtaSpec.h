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

#ifndef V1KwtaSpec_h
#define V1KwtaSpec_h 1

// parent includes:
#include <taOBase>

// member includes:
#include <FunLookup>

// declare all other types mentioned but not required to include:
class float_Matrix; // 


class TA_API V1KwtaSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image k-winners-take-all dynamics for v1 image processing stages -- based on Leabra dynamics with asymptotic activation settling, using the gelin version of the NXX1 activation function  -- see Leabra docs for more info on various parameters (LeabraUnitSpec, LeabraLayerSpec)
INHERITED(taOBase)
public:
  bool		on;	// is kwta active for this stage of processing?
  int		gp_k;	// #CONDSHOW_ON_on number of active units within a group (hyperocolumn) of features
  float		gp_g;	// #CONDSHOW_ON_on #DEF_0.02;0.1;0.4;0.6 gain on sharing of group-level inhibition with other unit groups throughout the layer -- spreads inhibition throughout the layer based on strength of competition happening within each unit group -- sets an effective minimum activity level
  float		kwta_pt; // #CONDSHOW_ON_on #DEF_0.5 k-winner-take-all inhibitory point value between avg of top k and remaining bottom units (uses KWTA_AVG_BASED -- 0.5 is gelin default)
  float		gain;	 // #CONDSHOW_ON_on #DEF_40 gain on the NXX1 activation function (based on g_e - g_e_thr value -- i.e. the gelin version of the function)
  float		nvar;	 // #CONDSHOW_ON_on #DEF_0.01 noise variance to convolve with XX1 function to obtain NOISY_XX1 function -- higher values make the function more gradual at the bottom
  float		g_bar_l; // #CONDSHOW_ON_on #DEF_0.1;0.3 leak current conductance value -- determines neural response to weak inputs -- a higher value can damp the neural response
  float		g_bar_e; // #READ_ONLY #NO_SAVE excitatory conductance multiplier -- multiplies filter input value prior to computing membrane potential -- general target is to have max excitatory input = .5, so with 0-1 normalized inputs, this value is automatically set to .5
  float		e_rev_e; // #READ_ONLY #NO_SAVE excitatory reversal potential -- automatically set to default value of 1 in normalized units
  float		e_rev_l; // #READ_ONLY #NO_SAVE leak and inhibition reversal potential -- automatically set to 0.3 (gelin default)
  float		thr;	 // #READ_ONLY #NO_SAVE firing threshold -- automatically set to default value of .5 (gelin default) 

  virtual bool	Compute_Kwta(float_Matrix& inputs, float_Matrix& outputs,
			     float_Matrix& gc_i_mat);
  // main interface: given the input matrix (raw output of filtering step), compute output activations via kwta function, using gc_i_mat to store inhib values per feature group, and g_i_tmp_mat as a temporary computation matrix for local spreading inhibition

  virtual void	Compute_Inhib(float_Matrix& inputs, float_Matrix& gc_i_mat);
  // inhib impl: given the input matrix (raw output of filtering step), compute inhibitory currrents for each feature group, stored in gc_i matrix
  virtual void	Compute_Act(float_Matrix& inputs, float_Matrix& outputs, float_Matrix& gc_i_mat);
  // activation impl: compute activations from inputs and computed inhibition

  virtual void	Compute_Inhib_IThr(float_Matrix& inputs, float_Matrix& gc_i_mat, 
				   float_Matrix& ithrs);
  // alternative compute inhib that computes all ithresh values into given array for subsequent usage in other inhibition computations
  virtual void	Compute_All_IThr(float_Matrix& inputs, float_Matrix& ithrs);
  // compute all ithresh values and store into temporary array -- for alternative activation schmes

  inline float 	Compute_IThresh(float gc_e) {
    return ((gc_e * e_rev_sub_thr_e + gbl_e_rev_sub_thr_l) / (thr_sub_e_rev_i));
  } 
  // compute inhibitory threshold value -- amount of inhibition to put unit right at firing threshold membrane potential

  inline float 	Compute_EThresh(float gc_i) {
    return ((gc_i * e_rev_sub_thr_i + gbl_e_rev_sub_thr_l) / (thr_sub_e_rev_e));
  } 
  // compute excitatory threshold value -- amount of excitation to put unit right at firing threshold membrane potential

  inline float 	Compute_EqVm(float gc_e, float gc_i) {
    float new_v_m = ((gc_e * e_rev_e + gber_l + (gc_i * e_rev_l)) / (gc_e + g_bar_l + gc_i));
    return new_v_m;
  }
  // compute equilibrium membrane potential from excitatory (gc_e) and inhibitory (gc_i) input currents (gc_e = raw filter value, gc_i = inhibition computed from kwta) -- in normalized units (e_rev_e = 1), and inhib e_rev_i = e_rev_l
  
  inline float 	Compute_ActFmVm_nxx1(float val, float thr) {
    float new_act;
    float val_sub_thr = val - thr;
    if(val_sub_thr <= nxx1_fun.x_range.min) {
      new_act = 0.0f;
    }
    else if(val_sub_thr >= nxx1_fun.x_range.max) {
      val_sub_thr *= gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(val_sub_thr);
    }
    return new_act;
  }

  inline float Compute_ActFmIn(float gc_e, float gc_i) {
    // gelin version:
    float g_e_thr = Compute_EThresh(gc_i);
    return Compute_ActFmVm_nxx1(gc_e, g_e_thr);
  }

  virtual void	CreateNXX1Fun();  // #CAT_Activation create convolved gaussian and x/x+1 

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1KwtaSpec);
  //protected:

#ifndef __MAKETA__
  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution
#endif
  float		gber_l;	 // #READ_ONLY #NO_SAVE g_bar_l * e_rev_l -- just a compute time saver
  float		e_rev_sub_thr_e;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_e - thr -- used for compute_ithresh
  float		e_rev_sub_thr_i;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_i - thr -- used for compute_ithresh
  float		gbl_e_rev_sub_thr_l;// #READ_ONLY #NO_SAVE #HIDDEN g_bar_l * (e_rev_l - thr) -- used for compute_ithresh
  float		thr_sub_e_rev_i;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_i used for compute_ithresh
  float		thr_sub_e_rev_e;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_e used for compute_ethresh

 protected:
  void 	UpdateAfterEdit_impl();
};

#endif // V1KwtaSpec_h

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
#include "network_def.h"

// member includes:
#include <FunLookup>
#include <float_Matrix>

// declare all other types mentioned but not required to include:

eTypeDef_Of(V1KwtaSpec);

class E_API V1KwtaSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image k-winners-take-all dynamics for v1 image processing stages -- based on Leabra dynamics with asymptotic activation settling, using the gelin version of the NXX1 activation function  -- see Leabra docs for more info on various parameters (LeabraUnitSpec, LeabraLayerSpec)
INHERITED(taOBase)
public:
  bool          on;     // turn on kwta-style inhibitory competition, implemented via feed-forward and feed-back inhibitory functions of net input and activation within the layer
  float 	gi;	// #CONDSHOW_ON_on #DEF_1.5:2 typically between 1.5-2 -- sets overall level of inhibition for feedforward / feedback inhibition at the unit group level (see lay_gi for layer level parameter)
  float		lay_gi;	// #CONDSHOW_ON_on #DEF_1:2 sets overall level of inhibition for feedforward / feedback inhibition for the entire layer level -- the actual inhibition at each unit group is then the MAX of this computed inhibition and that computed for the unit group individually
  float		ff;     // #HIDDEN #NO_SAVE #DEF_1 overall inhibitory contribution from feedforward inhibition -- computed from average netinput -- fixed to 1
  float		fb;     // #HIDDEN #NO_SAVE #DEF_0.5 overall inhibitory contribution from feedback inhibition -- computed from average activation
  int           n_cyc;  // #HIDDEN #NO_SAVE #DEF_20 number of cycle iterations to perform on fffb inhib
  int           cycle;  // #HIDDEN #NO_SAVE current cycle of fffb settling
  float         act_dt; // #HIDDEN #NO_SAVE #DEF_0.3 time constant for integrating activations -- only for FFFB inhib 
  float         fb_dt;  // #HIDDEN #NO_SAVE #DEF_0.7 time constant for integrating fb inhib
  float         max_da; // #HIDDEN #NO_SAVE current max delta activation for fffb settling
  float         max_da_crit; // #HIDDEN #NO_SAVE stopping criterion for activation change for fffb
  float         ff0;    // #HIDDEN #NO_SAVE #DEF_0.1 feedforward zero point in terms of average netinput -- below this level, no FF inhibition is computed -- the 0.1 default should be good for most cases -- fixed to 0.1
  float		gain;	 // #CONDSHOW_ON_on #DEF_20;40;80 gain on the NXX1 activation function (based on g_e - g_e_thr value -- i.e. the gelin version of the function)
  float		nvar;	 // #CONDSHOW_ON_on #DEF_0.01 noise variance to convolve with XX1 function to obtain NOISY_XX1 function -- higher values make the function more gradual at the bottom
  float		g_bar_l; // #CONDSHOW_ON_on #DEF_0.1;0.3 leak current conductance value -- determines neural response to weak inputs -- a higher value can damp the neural response
  float		g_bar_e; // #HIDDEN #NO_SAVE excitatory conductance multiplier -- multiplies filter input value prior to computing membrane potential -- general target is to have max excitatory input = .5, so with 0-1 normalized inputs, this value is automatically set to .5
  float		e_rev_e; // #HIDDEN #NO_SAVE excitatory reversal potential -- automatically set to default value of 1 in normalized units
  float		e_rev_l; // #HIDDEN #NO_SAVE leak and inhibition reversal potential -- automatically set to 0.3 (gelin default)
  float		thr;	 // #HIDDEN #NO_SAVE firing threshold -- automatically set to default value of .5 (gelin default) 

  enum InhibMode {
    OFF,                        // no inhibition
    FFFB,                       // feed-forward and feed-back inhibition
    KWTA,                       // kwta
    NOT_LOADED,                 // indication that this value was not loaded
  };
  InhibMode     mode;	// #OBSOLETE #NO_SAVE #HIDDEN old mode setting for loading old projects

  
  inline bool   On() { return on; }
  // is inhibition turned on?

  inline float    FFInhib(const float netin) {
    float ffi = 0.0f;
    if(netin > ff0) ffi = ff * (netin - ff0);
    return ffi;
  }
  // feedforward inhibition value as function of netinput

  inline float    FBInhib(const float act) {
    return fb * act;
  }
  // feedback inhibition value as function of netinput

  virtual bool	Compute_Inhib(float_Matrix& inputs, float_Matrix& outputs,
                              float_Matrix& gc_i_mat);
  // main interface: given the input matrix (raw output of filtering step), compute output activations via fffb inhibition function, using gc_i_mat to store inhib values per feature group

  virtual bool	Compute_Inhib_Extra(float_Matrix& inputs, float_Matrix& outputs,
                                    float_Matrix& gc_i_mat, float_Matrix& extra_inh);
  // main interface: given the input matrix (raw output of filtering step), compute output activations via fffb ihhibition function, using gc_i_mat to store inhib values per feature group -- extra_inh is extra inhibition to apply

  virtual void	Compute_FFFB(float_Matrix& inputs, float_Matrix& outputs,
                             float_Matrix& gc_i_mat);
  // inhib impl: given the input matrix (raw output of filtering step), compute inhibitory currrents for each feature group, stored in gc_i matrix -- using FFFB

  virtual void	Compute_Act(float_Matrix& inputs, float_Matrix& outputs,
                            float_Matrix& gc_i_mat);
  // activation impl: compute activations from inputs and computed inhibition
  virtual void	Compute_Act_Extra(float_Matrix& inputs, float_Matrix& outputs,
                                  float_Matrix& gc_i_mat, float_Matrix& extra_inh);
  // activation impl: compute activations from inputs and computed inhibition

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
  
  inline float 	Compute_ActFmVm_nxx1(float val, float tthr) {
    float new_act;
    float val_sub_thr = val - tthr;
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

  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution

  float		gber_l;	 // #READ_ONLY #NO_SAVE g_bar_l * e_rev_l -- just a compute time saver
  float		e_rev_sub_thr_e;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_e - thr -- used for compute_ithresh
  float		e_rev_sub_thr_i;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_i - thr -- used for compute_ithresh
  float		gbl_e_rev_sub_thr_l;// #READ_ONLY #NO_SAVE #HIDDEN g_bar_l * (e_rev_l - thr) -- used for compute_ithresh
  float		thr_sub_e_rev_i;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_i used for compute_ithresh
  float		thr_sub_e_rev_e;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_e used for compute_ethresh

 protected:
  void 	UpdateAfterEdit_impl() override;
};

#endif // V1KwtaSpec_h

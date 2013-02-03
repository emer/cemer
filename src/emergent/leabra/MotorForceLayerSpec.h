// Copyright, 1995-2013, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of Emergent
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef MotorForceLayerSpec_h
#define MotorForceLayerSpec_h 1

// parent includes:
#include <ScalarValLayerSpec>

// member includes:
#include <MinMaxRange>
#include <RandomSpec>

// declare all other types mentioned but not required to include:

TypeDef_Of(MotorForceSpec);

class LEABRA_API MotorForceSpec : public SpecMemberBase {
  // ##INLINE ##INLINE_DUMP ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_Leabra specs for scalar values
INHERITED(SpecMemberBase)
public:
  float		pos_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over position (in pos_range or normalized units, depending on norm_width)
  float		vel_width;	// sigma parameter of a gaussian specifying the tuning width of the coarse-coded integration over velocity (in vel_range or normalized units, depending on norm_width)
  bool		norm_width;	// #DEF_true if true, use normalized 0-1 width parameters instead of raw parameters
  bool		clip_vals;	// #DEF_true clip the pos and velocity values within their ranges, for purposes of computing values

  float		cur_pos;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos val
  float		cur_vel;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel val
  float		pos_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos min
  float		vel_min;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel min
  float		pos_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos range
  float		vel_range;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel range
  float		pos_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current pos increment
  float		vel_incr;	// #READ_ONLY #NO_SAVE #NO_INHERIT current vel increment
  float		pos_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective position width
  float		vel_width_eff;	// #READ_ONLY #NO_SAVE #NO_INHERIT effective velocity width

  virtual void	InitRanges(float pos_min, float pos_range, float vel_min, float vel_range);
  // #CAT_MotorForce initialize range parameters
  virtual void	InitVals(float pos, int pos_size, float pos_min, float pos_range, 
			float vel, int vel_size, float vel_min, float vel_range);
  // #CAT_MotorForce intiailize value and range parameters for subsequent calls
  virtual float	GetWt(int pos_gp_idx, int vel_gp_idx);
  // #CAT_MotorForce get weighting factor for position & velocity group at given indexes: MUST CALL InitVal first!

  override String       GetTypeDecoKey() const { return "LayerSpec"; }

  TA_SIMPLE_BASEFUNS(MotorForceSpec);
protected:
  SPEC_DEFAULTS;
private:
  void	Initialize();
  void 	Destroy()	{ };
  void	Defaults_init() { };
};

TypeDef_Of(MotorForceLayerSpec);

class LEABRA_API MotorForceLayerSpec : public ScalarValLayerSpec {
  // represents motor force as a function of joint position and velocity using scalar val layer spec: layer uses unit groups -- each group represents a force (typically localist), and groups are organized in X axis by position, Y axis by velocity.  Overall value is weighted average from neighboring unit groups
INHERITED(ScalarValLayerSpec)
public:
  MotorForceSpec motor_force;   // misc specs for motor force representation
  MinMaxRange	 pos_range;	// range of position values encoded over the X axis of unit groups in the layer
  MinMaxRange	 vel_range;	// range of velocity values encoded over the Y axis of unit groups in the layer
  bool		 add_noise;	// add some noise after computing value from layer
  RandomSpec	 force_noise;	// #CONDEDIT_ON_add_noise parameters for random added noise to forces

  virtual float	ReadForce(LeabraLayer* lay, LeabraNetwork* net, float pos, float vel);
  // #CAT_MotorForce read the force value from the layer, as a gaussian weighted average over units near the current position and velocity values
  virtual void	ClampForce(LeabraLayer* lay, LeabraNetwork* net, float force, float pos, float vel);
  // #CAT_MotorForce clamp the force value to the layer, as a gaussian weighted average over units near the current position and velocity values

  override void	Compute_BiasVal(LeabraLayer* lay, LeabraNetwork* net);
  
  bool  CheckConfig_Layer(Layer* lay, bool quiet=false);

  TA_SIMPLE_BASEFUNS(MotorForceLayerSpec);
protected:
  SPEC_DEFAULTS;
  void	UpdateAfterEdit_impl();
private:
  void 	Initialize();
  void	Destroy()		{ };
  void	Defaults_init() 	{ };
};

#endif // MotorForceLayerSpec_h

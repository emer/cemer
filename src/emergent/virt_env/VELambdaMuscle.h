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

#ifndef VELambdaMuscle_h
#define VELambdaMuscle_h 1

// parent includes:
#include <taNBase>

// member includes:
#include <MinMaxRange>
#include <float_CircBuffer>
#include <VEBody>

// declare all other types mentioned but not required to include:


taTypeDef_Of(VELambdaMuscle);

class TA_API VELambdaMuscle : public taNBase {
  // a Lambda-model (Gribble et al, 1998) muscle, used in context of a VE arm joint -- as a fairly accurate simplification, we assume a linear relationship between joint angle and muscle length, and a constant moment arm (accurate for extensors)
INHERITED(taNBase)
public:
  enum MuscleType {
    FLEXOR,                     // pulls a joint closed -- toward hi stop -- pectoralis for shoulder, biceps long head for elbow
    EXTENSOR,                   // pulls a joint open -- toward lo stop -- deltoid for shoulder, triceps lateral head
  };

  //////////////////////////////
  //    Control signals

  float         lambda_norm;    // normalized (0-1) desired length of the muscle: this is the only control signal
  float         lambda;         // #READ_ONLY #SHOW desired length of the muscle in muscle-length units
  float         co_contract_pct; // normalized (0-1) percent of co-contraction to apply -- shortens lambda by a fixed proportion of the co_contract_len value (below)
  float         extra_force;    // a constant additional force value to apply to the muscle -- can be used for co-contraction or additional force commands beyond the equilibrium point specification

  //////////////////////////////
  //    Parameters

  // statics
  MuscleType    muscle_type;            // what type of muscle is it -- controls relationship between angle and muscle length
  float         moment_arm;             // (m, .02 for elbow, .04 for shoulder) moment arm length for applying force (assumed fixed) -- positive for flexors and negative for extensors
  MinMaxRange   len_range;              // (m) effective length range of the muscle over which it can contract and expand -- this corresponds to the lo-hi stop range of angles of the joint -- for flexors, min = hi stop, max = lo stop, for extensors, min = lo stop, max = hi stop.  for elbow bicep/tricep, min=0.28 max=0.37, for shoulder pectoralis/deltoid min=0.05 max=0.15 (est)
  float         co_contract_len;        // maximum length available for co-contraction -- must be < .95 * len_range.min (i.e., if muscle is at its shortest length for the joint stop, this is how much extra shorter it can possibly command to be from there, and still be a positive number)
  float         rest_len;               // #READ_ONLY #SHOW resting length, computed during init from resting angle

  // dynamics
  float         step_size;              // #READ_ONLY (s) set in init -- world step size in seconds
  float         vel_damp;               // #DEF_0.06 (mu, s) velocity damping
  float         reflex_delay;           // #DEF_0.025 (d, s) reflex delay -- how slowly reflex control reacts to changes in muscle length and velocity
  int           reflex_delay_idx;       // #READ_ONLY (d, steps) reflex delay, computed in units of step size
  float         m_rec_grad;             // #DEF_11.2 (c, mm^-1) muscle MN recruitment gradient
  float         m_mag;                  // (rho, m^2) muscle force-generating magnitude (multiplier), related to cross-section of muscle size: biceps short head 2.1; biceps long head 11; deltoid 14.9; pectoralis 14.9; triceps lateral head 12.1; triceps long head 6.7;
  float         ca_dt;                  // #DEF_0.015 (tau, s) calcium kinetics time constant -- note only using a first-order exponential time decay filter
  float         ca_dt_cmp;              // #READ_ONLY (tau, 1/steps) calcium kinetics time constant actually used in cmputations -- note only using a first-order exponential time decay filter -- this value is in time steps, not time per se
  float         fv1;                    // #DEF_0.82 (f1, s/m) muscle force velocity dependence factor: constant offset
  float         fv2;                    // #DEF_0.5 (f2, s/m) muscle force velocity dependence factor: atan multiplier
  float         fv3;                    // #DEF_0.43 (f3, s/m) muscle force velocity dependence factor: atan constant offset
  float         fv4;                    // #DEF_0.58 (f4, s/m) muscle force velocity dependence factor: velocity multiplier
  float         passive_k;              // (k, N/m) passive stiffness: biceps short head 36.5; biceps long head 190.9; deltoid 258.5; pectoralis 258.5; triceps lateral head 209.9; triceps long head 116.3;

  //////////////////////////////
  //    State values
  float         len;            // #READ_ONLY #SHOW (l, cm) current muscle length
  float         dlen;           // #READ_ONLY #SHOW (l-dot, cm/s) current muscle length velocity: rate of change of length
  float         act;            // #READ_ONLY #SHOW (A, N?) current muscle activation value
  float         m_act_force;    // #READ_ONLY #SHOW (~M, N) current muscle force from activation
  float         m_force;        // #READ_ONLY #SHOW (M, N) current muscle force after low-pass filtering by ca_dt
  float         force;          // #READ_ONLY #SHOW (N) final force value
  float         torque;         // #READ_ONLY #SHOW (N) final torque (force * moment_arm)

  float_CircBuffer len_buf;     // #READ_ONLY #NO_SAVE current muscle length buffer (for delays)
  float_CircBuffer dlen_buf;    // #READ_ONLY #NO_SAVE current muscle length velocity buffer (for delays)

  VEBodyRef     muscle_obj;     // #SCOPE_VEWorld if non-null, update this object with the new length information as the muscle changes (must be cylinder or capsule obj shape)

  virtual float LenFmAngle(float norm_angle);
  // #CAT_Muscle compute muscle length from *normalized* joint angle (0 = lo stop, 1 = hi stop) -- uses a simple linear projection onto len_range which is fairly accurate

  virtual void  Init(float step_sz, float rest_norm_angle, float init_norm_angle,
                     float co_contract);
  // #CAT_Muscle initialize all parameters back to initial values, compute params, and set arm at initial angle (clear buffers, etc)

  virtual void  Compute_Force(float cur_norm_angle);
  // #CAT_Muscle compute force based on current parameters with given normalized angle (0 = lo stop, 1 = hi stop) (given by ODE presumably)

  virtual void  Compute_Lambda();
  // #CAT_Muscle compute lambda value from lambda_norm and co_contract_pct

  virtual void  SetTargAngle(float targ_norm_angle, float co_contract_pct);
  // #BUTTON #CAT_Muscle set target *normalized* (0 = lo stop, 1 = hi stop) angle for the joint, which computes the lambdas (target lengths) for the individual muscles -- the co_contract_pct determines what percentage of co-contraction (stiffnes) to apply, where the lambdas are shorter than they should otherwise be by the given amount, such that both will pull from opposite directions to cause the muscle to stay put

  TA_SIMPLE_BASEFUNS(VELambdaMuscle);
protected:
  void  UpdateAfterEdit_impl();

private:
  void  Initialize();
  void  Destroy()       { };
};

#endif // VELambdaMuscle_h

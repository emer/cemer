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

#ifndef VEArm_h
#define VEArm_h 1

// parent includes:
#include <VEObject>

// member includes:
#include <VEBody>
#include <float_Matrix>
#include <VEMuscle_List>
#include <DataTable>
#include <ColorScale>

// declare all other types mentioned but not required to include:
// class DataTable;

taTypeDef_Of(VEArmLengths);

class TA_API VEArmLengths : public taOBase {
  // #INLINE #INLINE_DUMP lengths of arm parameters
INHERITED(taOBase)
public:
  float         humerus;        // #DEF_0.28 length of humerus bone (upper arm)
  float         humerus_radius; // #DEF_0.015 radius (half width) of humerus
  float         ulna;           // #DEF_0.22 length of ulna bone (lower arm)
  float         ulna_radius;    // #DEF_0.015 radius (half width) of ulna
  float         hand;           // #DEF_0.08 length of hand
  float         hand_radius;    // #DEF_0.03 radius of hand
  float         elbow_gap;      // #DEF_0.08 gap between humerus and ulna
  float         wrist_gap;      // #DEF_0.03 gap between ulna and hand

  float         sh_off_x;        // #DEF_0 additional shoulder offset
  float         sh_off_y;        // #DEF_0 additional shoulder offset
  float         sh_off_z;        // #DEF_0 additional shoulder offset

  float         sh_x;            // #READ_ONLY full shoulder offset -- updated in config arm to be relative to torso center of mass
  float         sh_y;            // #READ_ONLY full shoulder offset -- updated in config arm to be relative to torso center of mass
  float         sh_z;            // #READ_ONLY full shoulder offset -- updated in config arm to be relative to torso center of mass

  float         humerus_mid;    // #READ_ONLY humerus / 2
  float         ulna_mid;       // #READ_ONLY ulna / 2
  float         hand_mid;       // #READ_ONLY hand / 2
  float         elbow_gap_mid;  // #READ_ONLY /2
  float         wrist_gap_mid;  // #READ_ONLY /2
  float 	La;             // #READ_ONLY the length of the humerus including the elbow gap
  float 	Lf;             // #READ_ONLY length of the forearm (ulna,hand,gaps)
  float         Ltot;           // #READ_ONLY total length of the arm

  TA_SIMPLE_BASEFUNS(VEArmLengths);
protected:
  void  UpdateAfterEdit_impl();

private:  
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmAngles);

class TA_API VEArmAngles : public taOBase {
  // #INLINE #INLINE_DUMP lengths of arm parameters
INHERITED(taOBase)
public:
  float         x;              // (beta) angle of the upper arm along the x (horizontal) axis -- this is how far forward or backward relative to the torso the arm is (picture a hinge pin along the horizontal axis at the top of the shoulder -- that is this angle)
  float         y;              // (gamma) angle of the upper arm along the y (vertical) axis -- this is spin around axis of arm and not very useful generally speaking
  float         z;              // (alpha) angle of the upper arm along the z (depth) axis -- this is how far to the left or right the arm is angled
  float         elbow;          // (delta) rotation of the ulna around the elbow on its hinge joint relative to the humerus -- 0 is straight in line with the humerus, and positive numbers produce contraction of that angle

  bool          up_y;           // #HIDDEN #NO_SAVE if true, then Y is the vertical axis -- this is the default
  float         alpha;          // #HIDDEN #NO_SAVE canonical(?) names for the angles
  float         beta;           // #HIDDEN #NO_SAVE canonical(?) names for the angles
  float         gamma;          // #HIDDEN #NO_SAVE canonical(?) names for the angles
  float         delta;          // #HIDDEN #NO_SAVE canonical(?) names for the angles

  void	UpdateAngles();
  // update alpha, beta, etc from x,y,z

  TA_SIMPLE_BASEFUNS(VEArmAngles);
protected:
  void  UpdateAfterEdit_impl();

private:  
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmDelays);

class TA_API VEArmDelays : public taOBase {
  // #INLINE #INLINE_DUMP delay parameters -- used to delay inputs/outputs to VEArm -- the arm starts receiving the relevant inputs/outputs at the time step specified (so a delay value of 1 = no delay)
INHERITED(taOBase)
public:
  float         pro_ms; // #DEF_35 proprioceptive delay period in msec for muscle length feedback from periphery
  float         vis_ms; // #DEF_150 visual delay period in msec for visual hand coordinate feedback from cortical visual system
  float         eff_ms; // #DEF_35 effector delay period in msec for motor command outputs to actually start affecting the muscles

  float         step_ms;        // #DEF_5 how long in msec is one step of the system, for purposes of delay computation -- translates ms values into step values -- may not reflect VE step size depending on scaling of that for numerical issues

  int           pro_st; // #READ_ONLY #SHOW proprioceptive delay period in steps
  int           vis_st; // #READ_ONLY #SHOW visual delay period in steps for hand coordinate inputs
  int           eff_st; // #READ_ONLY #SHOW effector delay period in steps for motor command outputs
  
  TA_SIMPLE_BASEFUNS(VEArmDelays);
protected:
  void  UpdateAfterEdit_impl();

private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmGains);

class TA_API VEArmGains : public taOBase {
  // #INLINE #INLINE_DUMP gain parameters -- used to scale forces applied to muscle insertion points in VEArm
INHERITED(taOBase)
public:
  float         stim;            // #DEF_200 gain factor in translating control signals into stimuli -- just an overall gain multiplier so that the other gains don't have to be quite so big
  float         musc_vel_norm;   // #DEF_100 normalization gain factor for sigmoidal compression function of muscle velocities -- produces more sensitive values
  float         hand_vel_norm;   // #DEF_1000 normalization gain factor for sigmoidal compression function of hand velocities -- produces more sensitive values
  float         hand_vra_dt;     // #DEF_0.1 hand velocity running average time constant
  float         ev;              // #DEF_2 gain factor for ERR_VEL control

  TA_SIMPLE_BASEFUNS(VEArmGains);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmPID);

class TA_API VEArmPID : public taOBase {
  // #INLINE #INLINE_DUMP PID control parameters for proportional, integral, derivative control signal that is typically used for online control of the arm
INHERITED(taOBase)
public:
  float         p;               // #DEF_40 P gain factor for proportional portion of PID control -- amount of stimulus in direct proportion to error between target and current length
  float         i;               // #DEF_5 I gain factor for integral portion of PID control -- if overshoot is a problem, then reduce this gain
  float         d;               // #DEF_5 D gain factor for derivative portion of PID control -- this is the most risky so typicaly set to be lower than p and i gains -- can be zero
  float         dra_dt;      // #DEF_1;0.3 time constant for integrating error derivatives for use in the D component of PID control -- this is what is actually used, so set to 1 if you want literal PID -- setting lower can result in a less noisy derivative term
  float         max_err;     // #DEF_0.05 maximum error value for PID control -- any  err_len value above this value is clipped, before being multiplied by p gain -- prevents extreme forces while still allowing for high gains -- if 0 or lower then not used

  TA_SIMPLE_BASEFUNS(VEArmPID);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmIOErr);

class TA_API VEArmIOErr : public taOBase {
  // #INLINE #INLINE_DUMP error parameters -- used to determine when a movement error occurs, and signal corrective action from the cerebellum
INHERITED(taOBase)
public:
  float         ra_dt;       // #DEF_1;0.3 #MAX_1 time constant for computing running average of temporal derivatives -- values < 1 result in smoother derivative estimates -- the averaged temporal derivative is key signal in driving IO error
  float         hand_thr;    // #DEF_0.07 threshold on del_hand_err_dt_ra values for driving an Inferior Olivary error signal for training cerebellum -- if error values are increasing over time, then this value is positive -- we react to even small positive increases by using a low positive number for this threshold
  float         musc_thr;    // #DEF_0.01 threshold on del_lens_err_dt_ra values for driving an Inferior Olivary error signal for training cerebellum -- if hand error triggered, and muscle error values are increasing over time, then this value is positive -- we react to even small positive increases by using a low positive number for this threshold

  TA_SIMPLE_BASEFUNS(VEArmIOErr);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmDamping);

class TA_API VEArmDamping : public taOBase {
  // #INLINE #INLINE_DUMP damping parameters -- used to reduce oscillations in arm movements
INHERITED(taOBase)
public:
  float         fac;             // angular damping factor
  float         thr;             // angular damping threshold

  TA_SIMPLE_BASEFUNS(VEArmDamping);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArm);

class TA_API VEArm : public VEObject {
  // #STEM_BASE a virtual environment arm object, consisting of 3 bodies: humerus, ulna, hand, and 3 joints: shoulder (a ball joint), elbow (a 2Hinge joint), and wrist (a FIXED joint for now)-- all constructed via ConfigArm -- bodies and joints are accessed by index so the order must not be changed
INHERITED(VEObject)
public:
  enum ArmBodies {               // indices of bodies for arm
    HUMERUS,
    ULNA,
    HAND,
    N_ARM_BODIES,
  };
  enum ArmJoints {               // indices of joints for arm
    SHOULDER,
    ELBOW,
    WRIST,
    N_ARM_JOINTS,
  };
  enum ArmSide {                 // which arm are we simulating here?
    RIGHT_ARM,
    LEFT_ARM,
  };
  enum UpAxis {                  // which axis points upwards?
    Y,
    Z,
  };
  enum MuscGeo {                 // type of muscle geometry 
    OLD_GEO,                     //  <-- 11 muscles
    NEW_GEO,                     //  <-- 12 muscles
  };
  enum MuscType {                // muscle model used
    LINEAR,                      // simple linear case where output force is proportional to stimulus
    HILL,                        // more complex Hill-type muscle used in Gribble et al, 1998
  };
  enum ControlType {             // type of controller to use
    PID,                         // proportional, integral, derivative controller -- separate gain terms for reach of these factors
    ERR_VEL,                     // stimulus is proportional to the current error minus the velocity of the muscles -- uses the single ev_gain factor 
  };
  enum ShowIPType { // whether to show muscle insertion points, and how to color them
    NO_IPS,                     // don't show any IP's
    IP_STIM,                    // show IPs, colored by muscle stimulation level
    IP_GAINS,                   // show IPs, colored by gain factors
    IP_LEN,                     // show IPs, colored by muscle length (normalized)
    IP_VEL,                     // show IPs, colored by muscle velocity (normalized)
    IP_TRG,                     // show IPs, colored by muscle target length (normalized)
    IP_ERR,                     // show IPs, colored by muscle length error
    IP_ERR_DRV,                 // show IPs, colored by muscle length error derivative
    IP_MUSC,                    // show IPs, colored by muscle number 
  };

  VEBodyRef     torso;           // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to

  DataTableRef  arm_input_data;  // data table that contains the information to present to a network, reflecting the properly delayed information from each relevant sensory channel, and IO error signals -- this is automatically generated every step of processing by the arm -- contains a single current record -- copy each row to another table if you want to keep a record..
  DataTableRef  arm_log_data;   // data table for logging all relevant current arm state information to monitor progress during the reach -- data is automatically logged if this is set, and log is reset when StartNewReach is called
  DataTableRef  arm_pro_data;   // data table containing arm muscle length and speed data, with proper initial padding (set at StartNewReach) to enable reading at delays.pro_st delay rows back from end
  DataTableRef  arm_vis_data;   // data table containing visual data (hand coordinates and hand speed), with proper initial padding (set at StartNewReach) to enable reading at delays.vis_st delay rows back from end
  DataTableRef  arm_eff_data;   // data table containing effector signal data, with proper initial padding (set at StartNewReach) to enable reading at delays.eff_st delay rows back from end

  ShowIPType    show_ips;        // whether to show the muscle insertion points (IPs) on the arms, as colored spheres -- color code use default (COLD_HOT) scale for type of information selected
  String        name_prefix;     // prefix to apply to all sub-objects (e.g., left_ or right_)
  ArmSide       arm_side;        // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  UpAxis        up_axis;         // which axis points upwards. This selects whether to use the COIN coordinate system (with the Y axis upwards), or the system originally used in SimMechanics (with the Z axis pointing upwards). Coordinates transformation between these systems comes through the CT matrix.
  MuscGeo       musc_geo;        // The muscle geometry. Geometries differ in the number of muscles (11 vs 12), and in the location of the insertion points
  MuscType      musc_type;       // The muscle model. Either linear (output force proportional to stimulus) or Hill-type (the muscle model used in Gribble et al. 1998) -- should correspond with actual type of muscle objects used
  float         hill_mu;         // #CONDSHOW_ON_musc_type:HILL #DEF_0.06 mu parameter for the Hill-type muscle -- dependence of muscle's threshold length on velocity
  ControlType   ctrl_type;       // type of controller to use to drive muscles in response to the difference between target lengths and current lengths
  float 	world_step;      // #READ_ONLY a copy of the owner VEWorld's stepsize, used for calculating speeds
  float         arm_time;        // #GUI_READ_ONLY #SHOW #NO_SAVE time counter for arm integration

  VEArmDelays   delays;          // Delay Parameters -- used to delay inputs/outputs to VEArm -- each is expressed as a discrete time step (1 step = 5 ms), where the arm starts receiving the relevant inputs/outputs at the time step specified (so a delay value of 1 = no delay)
  VEArmGains    gains;           // Gain Parameters -- used to scale forces applied to the muscle insertion points in the arm
  VEArmPID      pid;             // #CONDSHOW_ON_ctrl_type:PID PID control parameters
  VEArmIOErr    io_err;          // parameters for computing Inferior Olive error signals to drive learning of corrective action in the cerebellum
  VEArmDamping  damping;         // Damping Parameters -- used to reduce oscillations in arm movements
  VEArmLengths  alens;            // #READ_ONLY #SHOW arm length parameters provided by last config arm call
  VEArmAngles   init_angs;        // #READ_ONLY #SHOW initial angles computed or set by initialization functions (e.g., MoveToTarget, SetPose)
  VEArmAngles   targ_angs;        // #READ_ONLY #SHOW target angles, computed by SetTarget
  VEArmAngles   cur_angs;        // #READ_ONLY #SHOW current arm angles -- updated as the arm moves

  int           n_musc;          // #READ_ONLY the total number of muscles
  VEMuscle_List muscles;         // pointers to the muscles attached to the arm

  ////////////////////////////////////////
  //    Basic arm state variables

  float_Matrix  R;               // #READ_ONLY #HIDDEN #NO_SAVE target rotation matrix -- used as a tmp value for various routines for setting target lengths

  float_Matrix  ShouldIP;        // #EXPERT #NO_SAVE shoulder insertion points at rest
  float_Matrix  ArmIP;           // #EXPERT #NO_SAVE humerus insertion points at rest
  float_Matrix  FarmIP;          // #EXPERT #NO_SAVE ulna insertion points at rest
  float_Matrix  p1;              // #EXPERT #NO_SAVE first end points for bending lines
  float_Matrix  p2;              // #EXPERT #NO_SAVE second end points for bending lines
  float_Matrix  ct;              // #EXPERT #NO_SAVE An autoinverse rotation matrix which transforms coordinates from one system (Y axis upwards) to another (Z axis upwards).
  taVector3f    should_loc;      // #READ_ONLY the location of the shoulder in World coordinates

  float_Matrix  max_lens;        // #EXPERT #NO_SAVE maximum muscle lengths, initialized by ConfigArm, used to normalize lengths
  float_Matrix  min_lens;        // #EXPERT #NO_SAVE minimum muscle lengths, initialized by ConfigArm 
  float_Matrix  rest_lens;       // #EXPERT #NO_SAVE resting muscle lengths, initialized by ConfigArm 
  float_Matrix  spans;           // #EXPERT #NO_SAVE 1/(max_lens-min_lens). Used to speed up the calculation of norm_lengths.

  float_Matrix  lens;            // #EXPERT #NO_SAVE current lengths, computed by ComputeStim
  float         lens_mag;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of lens
  float_Matrix  vels;            // #EXPERT #NO_SAVE current velocities, computed by ComputeStim
  float         vels_mag;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of vels
  float_Matrix  lens_norm;       // #EXPERT #NO_SAVE normalized current muscle lengths
  float_Matrix  vels_norm;       // #EXPERT #NO_SAVE normalized muscle velocities

  ////////////////////////////////////////
  //    Muscle stimulus state

  float_Matrix  stims_p;         // #EXPERT #NO_SAVE PID p-driven stimulation values, computed by ComputeStim
  float         stims_p_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_p computed
  float_Matrix  stims_i;         // #EXPERT #NO_SAVE PID i-driven stimulation values, computed by ComputeStim
  float         stims_i_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_i computed
  float_Matrix  stims_d;         // #EXPERT #NO_SAVE PID d-driven stimulation values, computed by ComputeStim
  float         stims_d_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_d computed
  float_Matrix  stims;           // #EXPERT #NO_SAVE stimulation values, computed by ComputeStim
  float         stims_mag;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of stims computed
  float         stims_max;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE max of stims computed
  float_Matrix  gains_cur;       // #EXPERT #NO_SAVE current muscle gain values, set from cerebellar network or other external control -- should default to 1.0
  float         gains_mag;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of gains computed
  float         gains_max;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE maximum gain value across all muscles
  float_Matrix  forces;          // #EXPERT #NO_SAVE forces, computed by ComputeStim

  ///////////////////////////////////////////////////////////////////////////////////
  //    Target muscle lengths and errors: all current, no delays -- for PID control

  float_Matrix  targ_lens;       // #EXPERT #NO_SAVE target lengths, computed by the TargetLengths function for a given 3D target location
  float_Matrix  targ_lens_norm;  // #EXPERT #NO_SAVE normalized target muscle lengths
  float         targ_lens_mag;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of targ_lens
  float_Matrix  err_len;         // #EXPERT #NO_SAVE current errors (targ_lens - lens), computed by ComputeStim
  float         err_len_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err_len computed
  float_Matrix  err_len_prv;      // #EXPERT #NO_SAVE previous error len values, for PID

  float_Matrix  err_itg;         // #EXPERT #NO_SAVE integrated error over time (I in PID)
  float         err_itg_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err_int computed
  float_Matrix  err_drv;         // #EXPERT #NO_SAVE derivative of error over time (D in PID)
  float_Matrix  err_drv_dra;     // #EXPERT #NO_SAVE running-average of err_deriv -- uses pid_dra_dt parameter -- this is what is actually used in PID controller, so set pid_dra_dt to 1 if you want literal std D factor
  float         err_drv_dra_mag;  // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of running-average of err_deriv -- uses pid_dra_dt parameter

  ////////////////////////////////////////////////////////
  //    Target and current hand location, and errors

  taVector3f    targ_pos_abs;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE current target coordinates, in absolute world coordinates
  taVector3f    targ_pos_rel;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE current target coordinates, in shoulder-relative coordinates
  taVector3f    targ_pos_norm;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE normalized target coordinates, in shoulder-relative coordinates
  float         targ_rel_d;      // #READ_ONLY #SHOW #EXPERT #NO_SAVE distance to target (mag of targ_pos_rel)

  taVector3f    hand_pos_abs;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE current hand coordinates, in absolute world coordinates
  taVector3f    hand_pos_rel;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE current hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_pos_norm;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE normalized hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_pos_prv;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE previous hand coordinates, relative coordinates
  taVector3f    hand_pos_norm_prv; // #READ_ONLY #SHOW #EXPERT #NO_SAVE previous hand coordinates, relative coordinates, normalized
  taVector3f    hand_vel;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE hand velocity
  taVector3f    hand_vel_norm;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE normalized hand velocity
  float         hand_vel_mag;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE hand velocity
  float         hand_vra;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE temporal running average of hand_vel_mag using hand_vra_dt -- good measure of whether the reach has stopped

  taVector3f    hand_pos_err;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE targ_pos_rel - hand_pos_rel -- error vector of hand away from target location
  float         hand_pos_err_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE total distance away from target location


  ///////////////////////////////////////////////////////////////////////////////////
  //    delayed values read from data tables -- used for computing io_err

  float_Matrix  del_lens_norm;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE delayed normalized muscle lengths, read from delay table
  float_Matrix  del_vels_norm;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE delayed normalized muscle velocities, read from delay table

  taVector3f    del_hand_pos_norm; // #READ_ONLY #SHOW #EXPERT #NO_SAVE delayed normalized hand coordinates, in shoulder-relative coordinates, read from delay table
  taVector3f    del_hand_vel_norm; // #READ_ONLY #SHOW #EXPERT #NO_SAVE delayed normalized hand velocity, in shoulder-relative coordinates, read from delay table

  float_Matrix  del_stims;         // #EXPERT #NO_SAVE delayed stimulation values, computed by ComputeStim -- already have gains applied
  float_Matrix  del_gains;         // #EXPERT #NO_SAVE delayed gain values -- just for reference

  taVector3f    del_hand_err;      // #READ_ONLY #SHOW #EXPERT #NO_SAVE error in delayed normalized hand coordinates, in shoulder-relative coordinates, compared to targ_pos_norm
  float         del_hand_err_mag;  // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of the error in delayed normalized hand coordinates, in shoulder-relative coordinates, compared to targ_pos_norm
  
  float         del_hand_err_prv;  // #READ_ONLY #SHOW #EXPERT #NO_SAVE previous hand error magnitude 
  float         del_hand_err_dt;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE temporal derivative (rate of change) in hand error magnitude -- are errors increasing or decreasing over time??
  float         del_hand_err_dt_ra; // #READ_ONLY #SHOW #EXPERT #NO_SAVE running average of temporal derivative (rate of change) in hand error magnitude -- smooth out the rate of change of hand errors so we can only react to steady increases in errors, not just random fluctuations -- this is key value driving io_err computation
  bool          hand_io_err;        // #READ_ONLY #SHOW #NO_SAVE did the hand flag an IO error according to the io_err.hand_err_thr?


  float_Matrix  del_lens_err;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE errors in delayed normalized muscle lengths, read from delay table, compared to targ_lens_norm
  float_Matrix  del_lens_err_prv;  // #READ_ONLY #SHOW #EXPERT #NO_SAVE previous errors in delayed normalized muscle lengths, read from delay table, compared to targ_lens_norm
  float_Matrix  del_lens_err_dt;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE rate of change over time in errors in delayed normalized muscle lengths, read from delay table, compared to targ_lens_norm -- this is key factor in determining which muscles get the io error
  float_Matrix  musc_io_err;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE Inferior Olivary error signal for each muscle -- if hand_io_err is triggered, then each muscle has an error if its individual muscle rate of error change is over threshold
  float         musc_io_err_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE overall magnitude of Inferior Olivary error signal for each muscle -- if hand_io_err is triggered, then each muscle has an error if its individual muscle error is over threshold

  ColorScalePtr	color_scale;    // #IGNORE for coloring insertion points -- not saved..

  ///////////////////////////////////////////////////////////
  //            Initialization

  virtual bool  CheckArm(bool quiet = false);
  // #CAT_Init check to see if the arm is all configured OK -- it flags an error if not unless quiet -- returns true if OK, false if not

  virtual void	InitMuscles();
  // #CAT_Init initialize muscles and insertion point data

  virtual bool  ConfigArm(float humerus_length = 0.28, float humerus_radius = 0.015,
                          float ulna_length = 0.22, float ulna_radius = 0.015,
                          float hand_length = 0.08, float hand_radius = 0.03,
                          float elbow_gap = 0.08,   float wrist_gap = 0.03,
                          float sh_off_x = 0, float sh_off_y = 0, float sh_off_z = 0);
  // #BUTTON #CAT_Init configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual bool  ReConfigArm();
  // #BUTTON #CAT_Init reconfigure arm according to current arm length values stored in alens member -- can be used directly to initialize arm to default values

  virtual void  InitState();
  // #IGNORE initialize all the dynamic state variables used for computing muscle stimulation over time (i.e., the integ and deriv values in the PID controller), including resetting all the muscle gains back to 1.0
  virtual void  InitState_Base();
  // #IGNORE init basic state vars
  virtual void  InitState_Hand();
  // #IGNORE init hand-associated state vars

  virtual void SetTarget(taVector3f& trg_abs, taVector3f& trg_rel,
                         VEArmAngles& angs,
                         float trg_x, float trg_y, float trg_z,
                         bool add_gamma_noise = false);
  // #IGNORE impl sets up target info based on target coords -- computes R matrix and alpha.. angles in addition to all the targ_loc* values -- optionally add noise to gamma parameter
  virtual void ComputeRMatrix(float alpha, float beta, float gamma);
  // #IGNORE compute the magic R rotation matrix from angles (R is stored on Arm object)

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z, bool shoulder=true);
  // #BUTTON #CAT_Init place the hand at the target whose coordinates are the first 3 arguments. Reinitializes the arm prior to moving it into position.  Returns true if a move is made (even if the target is not reachable). The fourth argument indicates whether the coordinates are wrt the shoulder, or absolute 3D coords

  virtual bool SetPose(float x_ang, float y_ang, float z_ang, float elbow_ang);
  // #BUTTON #CAT_Init Set the arm pose according to rotation angles (in radians, 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) for the shoulder along the 3 different axes, and the additional rotation angle of the elbow relative to the shoulder.  Reinitializes the arm prior to moving it into position.  Returns true if a move is made.

  virtual bool SetPose_impl();
  // #IGNORE actually moves the arm -- common for both MoveToTarget and SetPose

  virtual bool Bender(taVector3f &p3, const taVector3f& a, const taVector3f& c,
                      const taVector3f& p1, const taVector3f& p2);
  // This function is the C++ equivalent of piece5.m -- the points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line -- If the muscle wraps around the bending line, Bender returns true, and inserts the value of the point of intersection with the muscle in the vector p3.


  ///////////////////////////////////////////////////////////
  //            Reach Target Location

  virtual bool TargetLengths(float trg_x, float trg_y, float trg_z);
  // #BUTTON #CAT_Target Set the targ_lens field of this Arm object to the muscle lengths which position the hand at the given coordinates. These targ_lens are used by the PID controller, e.g., by comparing against the current muscle lengths to obtain the err_len and other terms.  Returns false if failed.
  virtual bool NoisyTargetLengths(float trg_x, float trg_y, float trg_z);
  // #CAT_Target Like TargetLengths, but noise is applied to the arm rotation (gamma angle). Useful to generate sensible muscle lengths for training purposes.
  virtual bool AngToLengths(float_Matrix &tlens, float alpha, float beta, float gamma, float delta);
  // #IGNORE Given the four angles describing arm position, calculate the muscle lengths at that position -- implementation function for TargetLengths..

  virtual void GetRandomTarget(float& trg_x, float& trg_y, float& trg_z,
                               float x_ang_min = 0.0f, float x_ang_max = 1.5f,
                               float y_ang_min = 0.0f, float y_ang_max = 0.0f,
                               float z_ang_min = -3.14159, float z_ang_max = 3.14159,
                               float min_dist = 0.2f, float max_dist = 0.9f);
  // #CAT_Target generate a random target location that is actually reachable by this arm, by projecting a point out from the shoulder along a randomly generated angle within min/max range for each euler angle direction (x,y,z), with a distance randomly selected within normalized min/max range (1.0 = full length of arm) -- defaults work well for generating points in front of a typical arm configuration

  ///////////////////////////////////////////////////////////
  //            Updating and State info during reach

  virtual void StartNewReach();
  // #CAT_Updt start a new reach event -- resets and pads the data tables holding delayed information -- arm_pro_data, arm_vis_data, arm_eff_data -- with current state information -- be sure to call only after everything is properly initialized

  virtual void UpdateArmStep();
  // #CAT_Updt perform one step of arm updating -- called in Step_pre as part of standard VE updating process -- does CurStateFmArm, ComputeStim, WriteArmData, ReadDelArmData, ComputeIOErr, WriteArmInputData, WriteArmLogData

  virtual void CurStateFmArm();
  // #IGNORE harvest all the current state information from the Arm bodies, muscles, etc, including normalized values etc -- called as first step in ComputeStim

  virtual bool ComputeStim();
  // #IGNORE Computes the muscle stimulation to drive toward targ_lens, using selected controller mechanism (ctrl_typek, e.g., PID), based on current (non-delayed) muscle length information -- this encapsulates a basic equilibrium-point motor control system.  Stim is applied using musc_type via ApplyStim, which then updates the forces on the muscles.  It does not take a step of the VEWorld, and does not udpate the muscle insertion points.  This is called automatically by the Step function
  virtual bool ComputeStim_EV(); // #IGNORE ERR_VEL version of compute stim
  virtual bool ComputeStim_PID(); // #IGNORE PID version of compute stim
    virtual void InitState_Stim();
    // #IGNORE initialize state vals for Stim computation

  virtual void ApplyDelayedStim();
  // #IGNORE main call for applying stimuli to the muscles -- uses the proper delayed values -- called automatically in UpdateArmStep
  virtual bool ApplyStim(const float_Matrix& stms, float_Matrix &fs,
                         bool flip_sign = true);
  // #CAT_Updt Apply a stimulus to the arm muscles. The first argument is a vector matrix with the stimuli, and the second are the per-muscle gains. The third argument is a vector matrix where the resulting contraction forces will be stored; it should have 3 rows and Nmusc columns. if flip_sign, then the stimuli signs are flipped -- this is how the math works out for ComputeStim -- perhaps because these are contractions or something

  virtual void  ComputeIOErr();
  // using delayed values available to cerebellum, compute io_err value from hand and muscle values
    virtual void InitState_IOErr();
    // #IGNORE initialize state vals for IOErr computation

  virtual bool UpdateIPs();
  // #IGNORE Set the muscle IPs to the values in the rotated xxxIP matrices, and update norm_lengths -- called automatically after the ODE processing step
  virtual void ShowIP(int ipno, int prox_dist, taVector3f& ip_loc);
  // #IGNORE show the given muscle insertion point -- called by UpdateIPs

  ///////////////////////////////////////////////////////////
  //            Gain modulation -- how external control can modify the program

  virtual bool  SetMuscGains(const float_Matrix& new_gains);
  // #CAT_Gains set the per-muscle gains to given values -- gains should be n_musc in length (accessed in flat coordinates) -- these are the modulatory signals that can be provided by an additional control system, such as the cerebellum -- see also IncrMuscGain and DecayMuscGain
  virtual bool  SetAllMuscGains(float all_gain = 1.0f);
  // #CAT_Gains set all the per-muscle gains to given value
  virtual float SetMuscGain(int musc_no, float gn = 1.0f);
  // #CAT_Gains set gain for given muscle to given value -- return gain value
  virtual float IncrMuscGain(int musc_no, float gain_inc);
  // #CAT_Gains increment gain for given muscle to given value -- return gain value -- gain += gain_inc
  virtual float DecayMuscGain(int musc_no, float decay_rate);
  // #CAT_Gains decay muscle gain toward 1.0 value by given decay rate multiplier -- gain += decay_rate * (1 - gain)

  ///////////////////////////////////////////////////////////
  //            State grabbing utility functions

  virtual void Lengths(float_Matrix& len);
  // #CAT_State Put the current lengths of all muscles in the given matrix -- sets len geom to 1,n_musc
  virtual void NormLengths(float_Matrix& len_nrm, const float_Matrix& len);
  // #CAT_State normalize the lengths from len into len_nrm
  virtual void Vels(float_Matrix& vel);
  // #CAT_State Put the muscle contraction velocities of the last time step in the given matrix -- sets vel geom to 1,n_musc
  virtual void NormVels(float_Matrix& vel_nrm, const float_Matrix& vel);
  // #CAT_State normalize velocities from vel into vel_nrm

  virtual void HandPos(taVector3f& hpos);
  // #CAT_State get the current hand position into vector
  virtual void HandPosRel(taVector3f& hpos);
  // #CAT_State get the current hand position relative to shoulder into vector
  virtual void HandPosNorm(taVector3f& hpos_nrm, const taVector3f& hpos);
  // #CAT_State normalize the hand position 

  ///////////////////////////////////////////////////////////
  //            Data Table Management 

  virtual void  InitArmData();
  // #CAT_Data initialize and reset all the relevant data tables -- called automatically during configure arm and StartNewReach
    virtual DataTable* GetProgramTable(const String& dt_name);
    // #IGNORE get a table within owning program
    virtual void InitArmInputData();
    // #IGNORE arm_input_data
    virtual void InitArmLogData();
    // #IGNORE arm_log_data
    virtual void InitArmProData();
    // #IGNORE arm_pro_data
    virtual void InitArmVisData();
    // #IGNORE arm_vis_data
    virtual void InitArmEffData();
    // #IGNORE arm_eff_data

  virtual void PadArmData();
  // #IGNORE pad all with delay values -- only call after proper init pos set
    virtual void PadArmProData();
    // #IGNORE arm_pro_data -- pad with delay values -- only call after proper init pos set
    virtual void PadArmVisData();
    // #IGNORE arm_vis_data -- pad with delay values -- only call after proper init pos set
    virtual void PadArmEffData();
    // #IGNORE arm_eff_data -- pad with delay values -- only call after proper init pos set

  virtual void WriteArmData();
  // #IGNORE write all current arm data
    virtual void WriteArmProData();
    // #IGNORE write arm_pro_data
    virtual void WriteArmVisData();
    // #IGNORE write arm_vis_data
    virtual void WriteArmEffData();
    // #IGNORE write arm_eff_data

  virtual void WriteArmInputData();
  // #IGNORE write arm_input_data
  virtual void WriteArmLogData();
  // #IGNORE write arm_log_data

  virtual void ReadArmDelData();
  // #IGNORE read all delayed arm data
    virtual void ReadArmDelProData();
    // #IGNORE read arm_pro_data
    virtual void ReadArmDelVisData();
    // #IGNORE read arm_vis_data
    virtual void ReadArmDelEffData();
    // #IGNORE read arm_eff_data

  virtual bool SetTargetLengthsFmTable();
  // Update the unnormalized target lengths (targ_lens) using normalized values from a DataTable. The received DataTable must contain a float column named "lengths" with 4 dimensional cell geometry n x 1 by 1 x n_musc. n_musc is the number of muscles, and n is an integer equal or greater than 1.

  // these functions (step_pre and CurFromODE) are called by VEWorld Step
  // Step_pre updates everything including the muscle forces using UpdateArmStep
  // CurFromODE updates IP's calling UpdateIPs
  void Step_pre() override;
  void CurFromODE(bool updt_disp = false) override;
  void Init() override;

  TA_SIMPLE_BASEFUNS(VEArm);
protected:
  void  UpdateAfterEdit_impl() override;
  //  CheckConfig_impl() override; // todo

private:
  void  Initialize();
  void  Destroy();  
};

#endif // VEArm_h

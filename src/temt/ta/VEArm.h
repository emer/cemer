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
  float         humerus;        // #DEF_0.3 length of humerus bone (upper arm)
  float         humerus_radius; // #DEF_0.02 radius (half width) of humerus
  float         ulna;           // #DEF_0.24 length of ulna bone (lower arm)
  float         ulna_radius;    // #DEF_0.02 radius (half width) of ulna
  float         hand;           // #DEF_0.08 length of hand
  float         hand_radius;    // #DEF_0.03 radius of hand
  float         elbow_gap;      // #DEF_0.03 gap between humerus and ulna
  float         wrist_gap;      // #DEF_0.03 gap between ulna and hand

  float         sh_off_x;        // #DEF_0.0 additional shoulder offset
  float         sh_off_y;        // #DEF_0.0 additional shoulder offset
  float         sh_off_z;        // #DEF_0.0 additional shoulder offset

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
  // #INLINE #INLINE_DUMP delay parameters -- used to delay inputs/outputs to VEArm -- each is expressed as a number of time steps (1 step = 5 ms), where the arm starts receiving the relevant inputs/outputs at the time step specified (so a delay value of 1 = no delay)
INHERITED(taOBase)
public:
  int           pro;             // proprioceptive delay period for muscle length inputs -- constrained to be less than delays.vis [Default: 7]
  int           vis;             // visual delay period for hand coordinate inputs -- constrained to be greater than delays.pro [Default: 30]
  int           eff;             // effector delay period for motor command outputs -- this should correspond to total reaction time [Default: 62]
  
  TA_SIMPLE_BASEFUNS(VEArmDelays);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmErrors);

class TA_API VEArmErrors : public taOBase {
  // #INLINE #INLINE_DUMP error parameters -- used to determine when a movement error occurs, and signal corrective action from the cerebellum
INHERITED(taOBase)
public:
  float         max;             // maximum error value -- prevents extreme forces while still allowing for high gains -- if 0 or lower then not used
  float         norm_dra_dt;     // #DEF_0.3 time constant for computing normalized error derivative running average value -- values < 1 result in smoother derivative estimates -- norm_err_dra is useful for cerebellar control signal
  float         hand_vra_dt;     // #DEF_0.1 hand velocity running average time constant
  float         pid_dra_dt;      // #CONDSHOW_ON_owner.ctrl_type:PID time constant for integrating error derivatives for use in the D component of PID control -- this is what is actually used, so set to 1 if you want literal PID -- setting lower can result in a less noisy derivative term
  taVector3f    loc;             // #READ_ONLY #SHOW #EXPERT targ_loc - hand_loc -- error vector of hand away from target location (delayed by delays.vis)
  taVector3f    loc_actual;      // #READ_ONLY #SHOW #EXPERT targ_loc - hand_loc -- error vector of hand away from target location (not delayed)
  float         loc_mag;         // #READ_ONLY #SHOW #EXPERT total distance away from target location (delayed by delays.vis)
  float         loc_mag_actual;  // #READ_ONLY #SHOW #EXPERT total distance away from target location (not delayed)
  float_Matrix  len;             // #EXPERT #NO_SAVE current errors (targ_lens - lens), computed by ComputeStim
  float         len_mag;         // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err computed
  float_Matrix  len_norm;        // #READ_ONLY #SHOW #EXPERT normalized muscle errors
  float_Matrix  len_norm_dt;     // #READ_ONLY #SHOW #EXPERT normalized muscle error derivatives
  float_Matrix  len_norm_dra;    // #READ_ONLY #SHOW #EXPERT running average of norm_err_deriv using norm_err_dra_dt time constant -- these values can be useful for computing cerebellar control 
  float_Matrix  len_prv;         // #EXPERT #NO_SAVE previous error values in PID
  float_Matrix  len_norm_prv;    // #READ_ONLY #SHOW #EXPERT previous normalized muscle errors
  float_Matrix  itg;             // #EXPERT #NO_SAVE integrated error over time (I in PID)
  float         itg_mag;         // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err_int computed
  float_Matrix  drv;             // #EXPERT #NO_SAVE derivative of error over time (D in PID)
  float_Matrix  drv_dra;         // #EXPERT #NO_SAVE running-average of err_deriv -- uses pid_dra_dt parameter -- this is what is actually used in PID controller, so set pid_dra_dt to 1 if you want literal std D factor
  float         drv_dra_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of running-average of err_deriv -- uses pid_dra_dt parameter
  float_Matrix  io;              // #READ_ONLY #SHOW #EXPERT Inferior Olivary like error signal -- 1.0 if norm_err_dra[i] > io_err_thr else 0.0
  float         io_mag;          // #READ_ONLY #SHOW #EXPERT #NO_SAVE overall magnitude of io errors
  float         io_thr;          // threshold on norm_err_dra values for driving an Inferior Olivary error signal for training cerebellum

 
  TA_SIMPLE_BASEFUNS(VEArmErrors);
private:
  void  Initialize();
  void  Destroy() { };
};

taTypeDef_Of(VEArmGains);

class TA_API VEArmGains : public taOBase {
  // #INLINE #INLINE_DUMP gain parameters -- used to scale forces applied to muscle insertion points in VEArm
INHERITED(taOBase)
public:
  float         stim;            // gain factor in translating control signals into stimuli -- just an overall gain multiplier so that the other gains don't have to be quite so big
  float         vel_norm;        // speed normalization gain factor for a sigmoidal compression function
  float         ev;              // #CONDSHOW_ON_owner.ctrl_type:ERR_VEL gain factor for ERR_VEL control
  float         p;               // #CONDSHOW_ON_owner.ctrl_type:PID P gain factor for proportional portion of PID control -- amount of stimulus in direct proportion to error between target and current length
  float         i;               // #CONDSHOW_ON_owner.ctrl_type:PID I gain factor for integral portion of PID control -- if overshoot is a problem, then reduce this gain
  float         d;               // #CONDSHOW_ON_owner.ctrl_type:PID D gain factor for derivative portion of PID control -- this is the most risky so typicaly set to be lower than p and i gains -- can be zero
  float_Matrix  musc;            // #READ_ONLY #SHOW #EXPERT muscle-specific gain factors -- these operate in addition to the overall gain, and multiply the target - actual length to modulate the effective force applied on a given muscle -- these are what the cerebellum operates on -- default value should be 1.0
  float         musc_mag;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of gains.musc

  TA_SIMPLE_BASEFUNS(VEArmGains);
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

  VEBodyRef     torso;           // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to
  DataTableRef  arm_state;       // this points to the data table that contains a record of all the arm state information over time, used to implement delays
  bool          show_ips;        // show the muscle insertion points on the arms, as colored spheres -- color code use COLD_HOT scale for muscle group index (1-12 or so)
  String        name_prefix;     // prefix to apply to all sub-objects (e.g., left_ or right_)
  ArmSide       arm_side;        // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  UpAxis        up_axis;         // which axis points upwards. This selects whether to use the COIN coordinate system (with the Y axis upwards), or the system originally used in SimMechanics (with the Z axis pointing upwards). Coordinates transformation between these systems comes through the CT matrix.
  MuscGeo       musc_geo;        // The muscle geometry. Geometries differ in the number of muscles (11 vs 12), and in the location of the insertion points
  MuscType      musc_type;       // The muscle model. Either linear (output force proportional to stimulus) or Hill-type (the muscle model used in Gribble et al. 1998) -- should correspond with actual type of muscle objects used
  float         hill_mu;         // #CONDSHOW_ON_musc_type:HILL #DEF_0.06 mu parameter for the Hill-type muscle -- dependence of muscle's threshold length on velocity
  ControlType   ctrl_type;       // type of controller to use to drive muscles in response to the difference between target lengths and current lengths
  
  VEArmDelays   delays;          // Delay Parameters -- used to delay inputs/outputs to VEArm -- each is expressed as a discrete time step (1 step = 5 ms), where the arm starts receiving the relevant inputs/outputs at the time step specified (so a delay value of 1 = no delay)
  VEArmErrors   errors;          // Error Parameters -- used to determine when a movement error occurs, and signal corrective action from the cerebellum
  VEArmGains    gains;           // Gain Parameters -- used to scale forces applied to the muscle insertion points in the arm
  VEArmDamping  damping;         // Damping Parameters -- used to reduce oscillations in arm movements
  VEArmLengths  alens;            // #READ_ONLY #SHOW arm length parameters provided by last config arm call
  VEArmAngles   init_angs;        // #READ_ONLY #SHOW initial angles computed or set by initialization functions (e.g., MoveToTarget, SetPose)
  VEArmAngles   targ_angs;        // #READ_ONLY #SHOW target angles, computed by SetTarget
  VEArmAngles   cur_angs;        // #READ_ONLY #SHOW current arm angles -- updated as the arm moves

  float 	world_step;      // #READ_ONLY a copy of the owner VEWorld's stepsize, used for calculating speeds

  ColorScalePtr	color_scale;    // #IGNORE for coloring insertion points -- not saved..

  float_Matrix  ShouldIP;        // #EXPERT shoulder insertion points at rest
  float_Matrix  ArmIP;           // #EXPERT humerus insertion points at rest
  float_Matrix  FarmIP;          // #EXPERT ulna insertion points at rest
  float_Matrix  p1;              // #EXPERT first end points for bending lines
  float_Matrix  p2;              // #EXPERT second end points for bending lines
  float_Matrix  ct;              // #EXPERT An autoinverse rotation matrix which transforms coordinates from one system (Y axis upwards) to another (Z axis upwards).
  taVector3f    should_loc;      // #READ_ONLY #SHOW the location of the shoulder in World coordinates
  int           n_musc;          // #READ_ONLY the total number of muscles, as implied by the IP matrices

  taVector3f    targ_loc_abs;    // #READ_ONLY #SHOW #EXPERT current target coordinates, in absolute world coordinates
  taVector3f    hand_loc_abs;    // #READ_ONLY #SHOW #EXPERT current hand coordinates, in absolute world coordinates
  taVector3f    targ_loc_rel;    // #READ_ONLY #SHOW #EXPERT current target coordinates, in shoulder-relative coordinates
  float         targ_rel_d;      // #READ_ONLY #SHOW #EXPERT distance to target (mag of targ_loc_rel)
  taVector3f    hand_loc_rel;    // #READ_ONLY #SHOW #EXPERT delayed hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_loc_actual; // #READ_ONLY #SHOW #EXPERT current hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_loc_prv;    // #READ_ONLY #SHOW #EXPERT previous hand coordinates
  taVector3f    hand_vel;        // #READ_ONLY #SHOW #EXPERT hand velocity
  float         hand_vel_mag;    // #READ_ONLY #SHOW #EXPERT hand velocity
  float         hand_vra;        // #READ_ONLY #SHOW #EXPERT temporal running average of hand_vel_mag using hand_vra_dt -- good measure of whether the reach has stopped
  float_Matrix  targ_lens;       // #EXPERT target lengths, computed by the TargetLengths function for a given 3D target location
  float         targ_lens_mag;   // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of targ_lens
  float_Matrix  R;               // #READ_ONLY #HIDDEN #NO_SAVE target rotation matrix -- used as a tmp value for various routines for setting target lengths

  bool          reach_start;     // #READ_ONLY #HIDDEN flag used to tell whether it's the start of a reach or not (since VEArm doesn't have direct access to network.cycle)

  float         arm_time;        // #GUI_READ_ONLY #SHOW #NO_SAVE time counter for arm integration
  float_Matrix  lens;            // #EXPERT #NO_SAVE current lengths, computed by ComputeStim
  float         lens_mag;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of lens
  float_Matrix  vels;            // #EXPERT #NO_SAVE current velocities, computed by ComputeStim
  float         vels_mag;        // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of vels
  float_Matrix  stims_p;         // #EXPERT #NO_SAVE PID p-driven stimulation values, computed by ComputeStim
  float         stims_p_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_p computed
  float_Matrix  stims_i;         // #EXPERT #NO_SAVE PID i-driven stimulation values, computed by ComputeStim
  float         stims_i_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_i computed
  float_Matrix  stims_d;         // #EXPERT #NO_SAVE PID d-driven stimulation values, computed by ComputeStim
  float         stims_d_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_d computed
  float_Matrix  stims;           // #EXPERT #NO_SAVE stimulation values, computed by ComputeStim
  float_Matrix  stims_delay;     // #READ_ONLY #SHOW #EXPERT buffer table used by the ApplyStims method to delay muscle output when eff_delay is greater than 1
  float         stims_mag;       // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of stims computed
  float_Matrix  forces;          // #EXPERT #NO_SAVE forces, computed by ComputeStim

  float_Matrix  max_lens;        // #EXPERT maximum muscle lengths, initialized by ConfigArm, used to normalize lengths
  float_Matrix  min_lens;        // #EXPERT minimum muscle lengths, initialized by ConfigArm 
  float_Matrix  rest_lens;       // #EXPERT resting muscle lengths, initialized by ConfigArm 
  float_Matrix  spans;           // #HIDDEN 1/(max_lens-min_lens). Used to speed up the calculation of norm_lengths.

  // overall state variables for the arm
  float_Matrix  norm_lens;       // #READ_ONLY #SHOW #EXPERT normalized current muscle lengths
  float_Matrix  norm_targ_lens;  // #READ_ONLY #SHOW #EXPERT normalized target muscle lengths
  float_Matrix  norm_vels;       // #READ_ONLY #SHOW #EXPERT normalized muscle velocities
  VEMuscle_List muscles;         // pointers to the muscles attached to the arm

  virtual bool  CheckArm(bool quiet = false);
  // check to see if the arm is all configured OK -- it flags an error if not unless quiet -- returns true if OK, false if not

  virtual void	InitMuscles();
  // initialize muscles and insertion point data

  virtual bool  ConfigArm(float humerus_length = 0.3, float humerus_radius = 0.02,
                          float ulna_length = 0.24, float ulna_radius = 0.02,
                          float hand_length = 0.08, float hand_radius = 0.03,
                          float elbow_gap = 0.03,   float wrist_gap = 0.03,
                          float sh_off_x = 0, float sh_off_y = 0, float sh_off_z = 0);
  // #BUTTON configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual bool  ReConfigArm();
  // #BUTTON reconfigure arm according to current arm length values stored in alens member -- can be used directly to initialize arm to default values

  virtual void  InitDynamicState();
  // initialize all the dynamic state variables used for computing muscle stimulation over time (i.e., the integ and deriv values in the PID controller), including resetting all the muscle gains back to 1.0

  virtual void SetTarget(float trg_x, float trg_y, float trg_z,
                         bool add_gamma_noise = false);
  // #IGNORE impl sets up target info based on target coords -- computes R matrix and alpha.. angles in addition to all the targ_loc* values -- optionally add noise to gamma parameter
  virtual void ComputeRMatrix(float alpha, float beta, float gamma);
  // #IGNORE compute the magic R rotation matrix from angles (R is stored on Arm object)

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z, bool shoulder=true);
  // #BUTTON place the hand at the target whose coordinates are the first 3 arguments. This method can crash if the arm hasn't been set to its initial position with ConfigArm. Returns true if a move is made (even if the target is not reachable). The fourth argument indicates whether the coordinates are wrt the shoulder.

  virtual bool SetPose(float x_ang, float y_ang, float z_ang, float elbow_ang);
  // #BUTTON Set the arm pose according to rotation angles (in radians, 180deg = 3.1415, 90deg = 1.5708, 45deg = .7854) for the shoulder along the 3 different axes, and the additional rotation angle of the elbow relative to the shoulder.  You must have run ConfigArm before using this function. Returns true if a move is made.

  virtual bool SetPose_impl();
  // #IGNORE actually moves the arm -- common for both MoveToTarget and SetPose

  virtual bool TargetLengths(float trg_x, float trg_y, float trg_z);
  // #BUTTON Obtain the muscle lengths which position the hand at the given coordinates, and place them in the targ_lens matrix, which will have a length equal to the number of muscles. Returns false if failed.
  virtual bool NoisyTargetLengths(float trg_x, float trg_y, float trg_z);
  // Like TargetLengths, but noise is applied to the arm rotation (gamma angle). Useful to generate sensible muscle lengths for training purposes.
  virtual bool AngToLengths(float_Matrix &tlens, float alpha, float beta, float gamma, float delta);
  // #IGNORE Given the four angles describing arm position, calculate the muscle lengths at that position


  virtual void GetRandomTarget(float& trg_x, float& trg_y, float& trg_z,
                               float x_ang_min = 0.0f, float x_ang_max = 1.5f,
                               float y_ang_min = 0.0f, float y_ang_max = 0.0f,
                               float z_ang_min = -3.14159, float z_ang_max = 3.14159,
                               float min_dist = 0.2f, float max_dist = 0.9f);
  // generate a random target location that is actually reachable by this arm, by projecting a point out from the shoulder along a randomly generated angle within min/max range for each euler angle direction (x,y,z), with a distance randomly selected within normalized min/max range (1.0 = full length of arm) -- defaults work well for generating points in front of a typical arm configuration

  virtual bool UpdateIPs();
  // #BUTTON Set the muscle IPs to the values in the rotated xxxIP matrices, and update norm_lengths
  virtual void ShowIP(int ipno, int prox_dist, taVector3f& ip_loc);
  // #IGNORE show the given muscle insertion point
  virtual bool GetNormVals();
  // get all the normalized values -- norm_lens, vels, etc -- based on current state -- called automatically during processing (in UpdateIPs)

  virtual bool Lengths(float_Matrix& norm_len, bool normalize);
  // Put the current lengths of all muscles in the given matrix (if normalize is true, return normalized 0..1) -- sets len geom to 1,n_musc
  virtual bool Speeds(float_Matrix& vel, bool normalize);
  // Put the muscle contraction speeds of the last time step in the given matrix (if normalize is true, return normalized 0..1) -- sets vel geom to 1,n_musc

  virtual void SetReachStartFlag(bool flag);
  // Sets the hidden reach_start bool variable to true or false, telling the methods of VEArm whether it's the start of a new reach or not -- designed to be used only when network.cycle = 0
  virtual bool ApplyStim(const float_Matrix& stms, float_Matrix &fs,
                         bool flip_sign = true);
  // Apply a stimulus to the arm muscles. The first argument is a vector matrix with the stimuli. The second argument is a vector matrix where the resulting contraction forces will be stored; it should have 3 rows and Nmusc columns. if flip_sign, then the stimuli signs are flipped -- this is how the math works out for ComputeStim -- perhaps because these are contractions or something

  virtual bool SetMuscGains(const float_Matrix& new_gains);
  // set the per-muscle gains to given values -- gains should be n_musc in length -- accessed in flat coordinates
  virtual bool SetAllMuscGains(float all_gain = 1.0f);
  // set all the per-muscle gains to given value
  virtual float SetMuscGain(int musc_no, float gn = 1.0f);
  // set gain for given muscle to given value -- return gain value
  virtual float IncrMuscGain(int musc_no, float gain_inc);
  // increment gain for given muscle to given value -- return gain value -- gain += gain_inc
  virtual float DecayMuscGain(int musc_no, float decay_rate);
  // decay muscle gain toward 1.0 value by given decay rate multiplier -- gain += decay_rate * (1 - gain)

  virtual bool ComputeStim();
  // Computes and applies the muscle stimulation, using musc_type and ctrl_type, and then calls ApplyStim on the resuting stimulations, which then updates the forces on the muscles.  It does not take a step of the VEWorld, and does not udpate the muscle insertion points.  This is called automatically by the Step function
  virtual bool ComputeStim_EV(); // ERR_VEL version of compute stim
  virtual bool ComputeStim_PID(); // PID version of compute stim

  virtual bool InitDelayedInputsToTable();
  // Pad the arm_state table with resting/zero values (except target muscle lengths & error inputs): affects the first (vis_delay - 1) rows of hand coordinates, and the first (pro_delay - 1) rows of muscle length inputs, simulating dual sensory input delay
  virtual bool NormLengthsToTable();
  // Write the normalized muscle lengths into a datatable, in column named "lengths", in the row corresponding to pro_delay, formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormTargLengthsToTable();
  // Write the normalized target muscle lengths into a datatable, in column named "targ_lengths", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormSpeedsToTable();
  // Write the normalized muscle contraction speeds into a datatable, in column named "speeds", in the row corresponding to pro_delay, formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormHandCoordsToTable();
  // Write the normalized XYZ shoulder-centered coordinates of the hand into a datatable, in a column named "hand_coords", formated in a 4 dimensional 1x1 by 1x3 geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x3 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormErrDraToTable();
  // Write the norm_err_dra (normalized error derivative running average) in column named "norm_err_dra", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row. If pro_delay > 1, this error is calculated based on delayed input values within GetNormVals.
  virtual bool IOErrToTable();
  // Write the io_err (inferior olive error values) in column named "io_err", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row. If pro_delay > 1, this error is calculated based on delayed input values within GetNormVals.
  virtual bool ArmStateToTable();
  // Write normalized lengths, speeds, and target lengths to a datatable -- calls above functions -- all are formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row

  virtual bool SetTargetLengthsFmTable();
  // Update the unnormalized target lengths (targ_lens) using normalized values from a DataTable. The received DataTable must contain a float column named "lengths" with 4 dimensional cell geometry n x 1 by 1 x n_musc. n_musc is the number of muscles, and n is an integer equal or greater than 1.

  virtual void FormatLogData(DataTable& dt);
  // format a data table to contain a log of data about the arm
  virtual void LogArmData(DataTable& dt);
  // log current state information to given data table (should be formatted by FormatLogData)

  virtual void PrintElbowFmODE();
  // debugging..

  // these functions (step_pre and CurFromODE) are called by VEWorld Step -- they
  // automatically update the muscle forces using VEP_Reach, and update the IPs etc
  void Step_pre() override;
  void CurFromODE(bool updt_disp = false) override;
  void Init() override;

  TA_SIMPLE_BASEFUNS(VEArm);
protected:
  void  UpdateAfterEdit_impl() override;
  //  CheckConfig_impl() override; // todo

  virtual bool Bender(taVector3f &p3, taVector3f a, taVector3f c, taVector3f p1, taVector3f p2);
  // This function is the C++ equivalent of piece5.m.
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, Bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector p3.

private:
  void  Initialize();
  void  Destroy();  
};

#endif // VEArm_h

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

// declare all other types mentioned but not required to include:
// class DataTable;

taTypeDef_Of(VEArmDelays);

class TA_API VEArmDelays : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP delay parameters -- used to delay inputs/outputs to VEArm
INHERITED(taOBase)
public:
  int           vis_delay;      // visual delay period for hand coordinate inputs expressed as a number of time steps (1 step = 5 ms) -- constrained to be > pro_delay -- set both to 1 for no delay
  int           pro_delay;      // proprioceptive delay period for muscle length inputs expressed as a number of time steps (1 step = 5 ms) -- constrained to be < vis_delay -- set to 1 for no delay
  int           eff_delay;      // effector delay period for motor command outputs to VEArm, expressed as a number of time steps (1 time step = 5 ms) -- set to 1 for no delay
  
  void  Initialize();
  void  Destroy() { };
  TA_SIMPLE_BASEFUNS(VEArmDelays);
};

taTypeDef_Of(VEArmGains);

class TA_API VEArmGains : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP gain parameters -- used to scale forces applied to muscle insertion points in VEArm
INHERITED(taOBase)
public:
  float_Matrix  musc_gains;     // #READ_ONLY #SHOW #EXPERT muscle-specific gain factors -- these operate in addition to the overall gain, and multiply the target - actual length to modulate the effective force applied on a given muscle -- these are what the cerebellum operates on -- default value should be 1.0
  float         musc_gains_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of musc_gains
  float         ev_gain;        // #AKA_gain #CONDSHOW_ON_ctrl_type:ERR_VEL gain factor for ERR_VEL control
  float         p_gain;         // #CONDSHOW_ON_ctrl_type:PID P gain factor for proportional portion of PID control -- amount of stimulus in direct proportion to error between target and current length
  float         i_gain;         // #CONDSHOW_ON_ctrl_type:PID I gain factor for integral portion of PID control -- if overshoot is a problem, then reduce this gain
  float         d_gain;         // #CONDSHOW_ON_ctrl_type:PID D gain factor for derivative portion of PID control -- this is the most risky so typicaly set to be lower than p and i gains -- can be zero
  float         stim_gain;      // gain factor in translating control signals into stimuli -- just an overall gain multiplier so that the other gains don't have to be quite so big
  float         vel_norm_gain;  // speed normalization gain factor for a sigmoidal compression function

  void  Initialize();
  void  Destroy() { };
  TA_SIMPLE_BASEFUNS(VEArmGains);
};

taTypeDef_Of(VEArm);

class TA_API VEArm : public VEObject {
  // #STEM_BASE a virtual environment arm object, consisting of 3 bodies: humerus, ulna, hand, and 3 joints: shoulder (a ball joint), elbow (a 2Hinge joint), and wrist (a FIXED joint for now)-- all constructed via ConfigArm -- bodies and joints are accessed by index so the order must not be changed
INHERITED(VEObject)
public:
  enum ArmBodies {              // indices of bodies for arm
    HUMERUS,
    ULNA,
    HAND,
    N_ARM_BODIES,
  };
  enum ArmJoints {              // indices of joints for arm
    SHOULDER,
    ELBOW,
    WRIST,
    N_ARM_JOINTS,
  };
  enum ArmSide {                // which arm are we simulating here?
    RIGHT_ARM,
    LEFT_ARM,
  };
  enum UpAxis {                 // which axis points upwards?
    Y,
    Z,
  };
  enum MuscGeo {                 // type of muscle geometry 
    OLD_GEO,         //  <-- 11 muscles
    NEW_GEO,         //  <-- 12 muscles
  };
  enum MuscType {                // muscle model used
    LINEAR,                      // simple linear case where output force is proportional to stimulus
    HILL,                        // more complex Hill-type muscle used in Gribble et al, 1998
  };
  enum ControlType {            // type of controller to use
    PID,                        // proportional, integral, derivative controller -- separate gain terms for reach of these factors
    ERR_VEL,                    // stimulus is proportional to the current error minus the velocity of the muscles -- uses the single ev_gain factor 
  };

  DataTableRef  arm_state;      // this points to the data table that contains a record of all the arm state information over time, used to implement delays
  ArmSide       arm_side;       // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  VEBodyRef     torso;          // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to
  UpAxis        up_axis;        // which axis points upwards. This selects whether to use the COIN coordinate system (with the Y axis upwards), or the system originally used in SimMechanics (with the Z axis pointing upwards). Coordinates transformation between these systems comes through the CT matrix.
  MuscGeo       musc_geo;       // The muscle geometry. Geometries differ in the number of muscles (11 vs 12), and in the location of the insertion points
  MuscType      musc_type;      // The muscle model. Either linear (output force proportional to stimulus) or Hill-type (the muscle model used in Gribble et al. 1998) -- should correspond with actual type of muscle objects used
  float         hill_mu;        // #CONDSHOW_ON_musc_type:HILL #DEF_0.06 mu parameter for the Hill-type muscle -- dependence of muscle's threshold length on velocity
  ControlType   ctrl_type;      // type of controller to use to drive muscles in response to the difference between target lengths and current lengths
  VEArmDelays   delay_params;   // object containing delay parameters
  VEArmGains    gain_params;    // object containing gain parameters
  float         max_err;        // maximum error value -- prevents extreme forces while still allowing for high gains -- if 0 or lower then not used
  float         pid_dra_dt;     // #CONDSHOW_ON_ctrl_type:PID time constant for integrating error derivatives for use in the D component of PID control -- this is what is actually used, so set to 1 if you want literal PID -- setting lower can result in a less noisy derivative term
  float         damping;        // angular damping factor
  float         damping_thr;    // angular damping threshold
  float         norm_err_dra_dt; // time constant for computing normalized error derivative running average value -- values < 1 result in smoother derivative estimates -- norm_err_dra is useful for cerebellar control signal
  float         io_err_thr;      // threshold on norm_err_dra values for driving an Inferior Olivary error signal for training cerebellum
  float         hand_vra_dt;     // hand velocity running average time constant

  float 	La;     // #READ_ONLY #SHOW the length of the humerus
  float 	Lf;     // #READ_ONLY #SHOW length of the forearm (ulna,hand radius,gaps)
  float		elbow_gap;  // #READ_ONLY #SHOW the distance between ulna and humerus
  float 	wrist_gap;  // #READ_ONLY #SHOW the distance between hand and ulna
  float 	world_step; // #READ_ONLY a copy of the owner VEWorld's stepsize, used for calculating speeds
  float_Matrix  ShouldIP; // #EXPERT shoulder insertion points at rest
  float_Matrix  ArmIP;    // #EXPERT humerus insertion points at rest
  float_Matrix  FarmIP;   // #EXPERT ulna insertion points at rest
  float_Matrix  p1;       // #EXPERT first end points for bending lines
  float_Matrix  p2;       // #EXPERT second end points for bending lines
  float_Matrix  ct;       // #EXPERT An autoinverse rotation matrix which transforms coordinates from one system (Y axis upwards) to another (Z axis upwards).
  taVector3f    should_loc; // #READ_ONLY #SHOW the location of the shoulder in World coordinates
  int           n_musc;  // #READ_ONLY the total number of muscles, as implied by the IP matrices

  taVector3f    targ_loc_abs;  // #READ_ONLY #SHOW #EXPERT current target coordinates, in absolute world coordinates
  taVector3f    hand_loc_abs;  // #READ_ONLY #SHOW #EXPERT current hand coordinates, in absolute world coordinates
  taVector3f    targ_loc_rel;  // #READ_ONLY #SHOW #EXPERT current target coordinates, in shoulder-relative coordinates
  float         targ_rel_d;    // #READ_ONLY #SHOW #EXPERT distance to target (mag of targ_loc_rel)
  taVector3f    hand_loc_rel;  // #READ_ONLY #SHOW #EXPERT delayed hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_loc_actual; // #READ_ONLY #SHOW #EXPERT current hand coordinates, in shoulder-relative coordinates
  taVector3f    hand_loc_prv;  // #READ_ONLY #SHOW #EXPERT previous hand coordinates
  taVector3f    hand_vel;       // #READ_ONLY #SHOW #EXPERT hand velocity
  float         hand_vel_mag;   // #READ_ONLY #SHOW #EXPERT hand velocity
  float         hand_vra;       // #READ_ONLY #SHOW #EXPERT temporal running average of hand_vel_mag using hand_vra_dt -- good measure of whether the reach has stopped
  taVector3f    loc_err;     // #READ_ONLY #SHOW #EXPERT targ_loc - hand_loc -- error vector of hand away from target location
  float         loc_err_mag;  // #READ_ONLY #SHOW #EXPERT total distance away from target location
  float_Matrix  targ_lens;  // #EXPERT target lengths, computed by the TargetLengths function for a given 3D target location
  float         targ_lens_mag;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of targ_lens
  float_Matrix  R;          // #READ_ONLY #HIDDEN #NO_SAVE target rotation matrix -- used as a tmp value for various routines for setting target lengths
  float         alpha;      // #READ_ONLY #HIDDEN #NO_SAVE target rotation angle
  float         beta;      // #READ_ONLY #HIDDEN #NO_SAVE target rotation angle
  float         gamma;      // #READ_ONLY #HIDDEN #NO_SAVE target rotation angle
  float         delta;      // #READ_ONLY #HIDDEN #NO_SAVE target rotation angle

  float         arm_time;   // #GUI_READ_ONLY #SHOW #NO_SAVE time counter for arm integration
  float_Matrix  lens;       // #EXPERT #NO_SAVE current lengths, computed by ComputeStim
  float         lens_mag;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of lens
  float_Matrix  vels;       // #EXPERT #NO_SAVE current velocities, computed by ComputeStim
  float         vels_mag;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of vels
  float_Matrix  err;        // #EXPERT #NO_SAVE current errors (targ_lens - lens), computed by ComputeStim
  float         err_mag;    // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err computed
  float_Matrix  err_int;    // #EXPERT #NO_SAVE integrated error over time (I in PID)
  float         err_int_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of err_int computed
  float_Matrix  err_deriv;  // #EXPERT #NO_SAVE derivative of error over time (D in PID)
  float_Matrix  err_dra;    // #EXPERT #NO_SAVE running-average of err_deriv -- uses pid_dra_dt parameter -- this is what is actually used in PID controller, so set pid_dra_dt to 1 if you want literal std D factor
  float         err_dra_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of running-average of err_deriv -- uses pid_dra_dt parameter
  float_Matrix  err_prv;    // #EXPERT #NO_SAVE previous error values in PID
  float_Matrix  stims_p;    // #EXPERT #NO_SAVE PID p-driven stimulation values, computed by ComputeStim
  float         stims_p_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_p computed
  float_Matrix  stims_i;    // #EXPERT #NO_SAVE PID i-driven stimulation values, computed by ComputeStim
  float         stims_i_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_i computed
  float_Matrix  stims_d;    // #EXPERT #NO_SAVE PID d-driven stimulation values, computed by ComputeStim
  float         stims_d_mag; // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of PID stims_d computed
  float_Matrix  stims;      // #EXPERT #NO_SAVE stimulation values, computed by ComputeStim
  float         stims_mag;  // #READ_ONLY #SHOW #EXPERT #NO_SAVE magnitude of stims computed
  float_Matrix  forces;     // #EXPERT #NO_SAVE forces, computed by ComputeStim

  float_Matrix  max_lens;   // #EXPERT maximum muscle lengths, initialized by ConfigArm, used to normalize lengths
  float_Matrix  min_lens;   // #EXPERT minimum muscle lengths, initialized by ConfigArm 
  float_Matrix  rest_lens;  // #EXPERT resting muscle lengths, initialized by ConfigArm 
  float_Matrix  spans;      // #HIDDEN 1/(max_lens-min_lens). Used to speed up the calculation of norm_lengths.

  // overall state variables for the arm
  float_Matrix  delayedStims;   // #READ_ONLY #SHOW #EXPERT buffer table used by the ApplyStims method to delay muscle output when eff_delay is greater than 1
  float_Matrix  norm_lens;      // #READ_ONLY #SHOW #EXPERT normalized current muscle lengths
  float_Matrix  norm_targ_lens; // #READ_ONLY #SHOW #EXPERT normalized target muscle lengths
  float_Matrix  norm_vels;      // #READ_ONLY #SHOW #EXPERT normalized muscle velocities
  float_Matrix  norm_err;       // #READ_ONLY #SHOW #EXPERT normalized muscle errors
  float_Matrix  norm_err_deriv; // #READ_ONLY #SHOW #EXPERT normalized muscle error derivatives
  float_Matrix  norm_err_dra;   // #READ_ONLY #SHOW #EXPERT running average of norm_err_deriv using norm_err_dra_dt time constant -- these values can be useful for computing cerebellar control
  float_Matrix  io_err;         // #READ_ONLY #SHOW #EXPERT Inferior Olivary like error signal -- 1.0 if norm_err_dra[i] > io_err_thr else 0.0
  float         io_err_mag;     // #READ_ONLY #SHOW #EXPERT #NO_SAVE overall magnitude of io errors 
  float_Matrix  norm_err_prv;   // #READ_ONLY #SHOW #EXPERT previous normalized muscle errors
  VEMuscle_List muscles; // pointers to the muscles attached to the arm

  virtual bool  CheckArm(bool quiet = false);
  // check to see if the arm is all configured OK -- it flags an error if not unless quiet -- returns true if OK, false if not

  virtual void	InitMuscles();
  // initialize muscles and insertion point data

  virtual bool  ConfigArm(const String& name_prefix="",
                          float humerus_length = 0.3, float humerus_radius = 0.02,
                          float ulna_length = 0.24, float ulna_radius = 0.02,
                          float hand_length = 0.08, float hand_radius = 0.03,
                          float elbowGap = 0.03,   float wristGap = 0.03,
                          float sh_offX = 0, float sh_offY = 0, float sh_offZ = 0);
  // #BUTTON configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual void  InitDynamicState();
  // initialize all the dynamic state variables used for computing muscle stimulation over time (i.e., the integ and deriv values in the PID controller), including resetting all the muscle gains back to 1.0

  virtual void SetTarget(float trg_x, float trg_y, float trg_z,
                              bool add_gamma_noise = false);
  // #IGNORE impl sets up target info based on target coords -- computes R matrix and alpha.. angles in addition to all the targ_loc* values -- optionally add noise to gamma parameter
  virtual void ComputeRMatrix(float alpha, float beta, float gamma);
  // #IGNORE compute the magic R rotation matrix from angles (R is stored on Arm object)

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z, bool shoulder=true);
  // #BUTTON place the hand at the target whose coordinates are the first 3 arguments. This method can crash if the arm hasn't been set to its initial position with ConfigArm. Returns true if a move is made (even if the target is not reachable). The fourth argument indicates whether the coordinates are wrt the shoulder.

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
  virtual bool GetNormVals();
  // get all the normalized values -- norm_lens, vels, etc -- based on current state -- called automatically during processing (in UpdateIPs)

  virtual bool Lengths(float_Matrix& len, bool normalize);
  // Put the current lengths of all muscles in the given matrix (if normalize is true, return normalized 0..1) -- sets len geom to 1,n_musc
  virtual bool Speeds(float_Matrix& vel, bool normalize);
  // Put the muscle contraction speeds of the last time step in the given matrix (if normalize is true, return normalized 0..1) -- sets vel geom to 1,n_musc

  virtual void SetNewReachFlag(bool flag);
  // Sets the private isNewReach bool variable to true or false, telling the methods of VEArm whether it's the start of a new reach or not -- designed to be used only when network.cycle = 0
  virtual bool ApplyStim(const float_Matrix& stms, float_Matrix &fs,
                         bool flip_sign = true);
  // Apply a stimulus to the arm muscles. The first argument is a vector matrix with the stimuli. The second argument is a vector matrix where the resulting contraction forces will be stored; it should have 3 rows and Nmusc columns. if flip_sign, then the stimuli signs are flipped -- this is how the math works out for ComputeStim -- perhaps because these are contractions or something

  virtual bool SetMuscGains(const float_Matrix& gains);
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
  bool isNewReach; // flag used to tell whether it's the start of a new reach or not (since VEArm doesn't have direct access to network.cycle)

  void  Initialize();
  void  Destroy();  
};

#endif // VEArm_h

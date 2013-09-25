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


taTypeDef_Of(VEArm);

class TA_API VEArm : public VEObject {
  // a virtual environment arm object, consisting of 3 bodies: humerus, ulna, hand, and 3 joints: shoulder (a ball joint), elbow (a 2Hinge joint), and wrist (a FIXED joint for now)-- all constructed via ConfigArm -- bodies and joints are accessed by index so the order must not be changed
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

  ArmSide       arm_side;       // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  VEBodyRef     torso;          // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to
  UpAxis        up_axis;        // which axis points upwards. This selects whether to use the COIN coordinate system (with the Y axis upwards), or the system originally used in SimMechanics (with the Z axis pointing upwards). Coordinates transformation between these systems comes through the CT matrix.
  MuscGeo       musc_geo;       // The muscle geometry. Geometries differ in the number of muscles (11 vs 12), and in the location of the insertion points
  MuscType      musc_type;      // The muscle model. Either linear (output force proportional to stimulus) or Hill-type (the muscle model used in Gribble et al. 1998) -- should correspond with actual type of muscle objects used
  float         hill_mu;        // #CONDSHOW_ON_musc_type:HILL #DEF_0.06 mu parameter for the Hill-type muscle -- dependence of muscle's threshold length on velocity
  ControlType   ctrl_type;      // type of controller to use to drive muscles in response to the difference between target lengths and current lengths
  float         stim_gain;      // gain factor in translating control signals into stimuli -- just an overall gain multiplier so that the other gains don't have to be quite so big
  float         max_err;        // maximum error value -- prevents extreme forces while still allowing for high gains -- if 0 or lower then not used
  float         ev_gain;        // #AKA_gain #CONDSHOW_ON_ctrl_type:ERR_VEL gain factor for ERR_VEL control
  float         p_gain;         // #CONDSHOW_ON_ctrl_type:PID P gain factor for proportional portion of PID control -- amount of stimulus in direct proportion to error between target and current length
  float         i_gain;         // #CONDSHOW_ON_ctrl_type:PID I gain factor for integral portion of PID control -- if overshoot is a problem, then reduce this gain
  float         pid_i_thr;      // #CONDSHOW_ON_ctrl_type:PID threshold on value of error for updating the integral term in PID -- the integral is best for building up strength of small persistent errors, so this prevents it from being swamped by large errors, and allows a larger effective i_gain value -- set to 0 or lower to disable
  float         d_gain;         // #CONDSHOW_ON_ctrl_type:PID D gain factor for derivative portion of PID control -- this is the most risky so typicaly set to be lower than p and i gains -- can be zero
  float         pid_dt;         // #CONDSHOW_ON_ctrl_type:PID effective time constant for PID integral and derivative terms
  float         pid_dra_dt;     // #CONDSHOW_ON_ctrl_type:PID time constant for integrating error derivatives for use in the D component of PID control -- this is what is actually used, so set to 1 if you want literal PID -- setting lower can result in a less noisy derivative term
  float         damping;        // angular damping factor
  float         damping_thr;    // angular damping threshold 
  float         vel_norm_gain;  // speed normalization gain factor for a sigmoidal compression function
  float         norm_deriv_dt;  // effective time constant for computing norm_err_deriv -- divides by this number -- renormalizes range of derivatives
  float         norm_err_dra_dt; // time constant for computing normalized error derivative running average value -- values < 1 result in smoother derivative estimates -- norm_err_dra is useful for cerebellar control signal

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
  
  float_Matrix  targ_lens;  // #EXPERT target lengths, computed by the TargetLengths function for a given 3D target location
  float_Matrix  lens;       // #EXPERT #NO_SAVE current lengths, computed by ComputeStim
  float_Matrix  vels;       // #EXPERT #NO_SAVE current velocities, computed by ComputeStim
  float_Matrix  err;        // #EXPERT #NO_SAVE current errors (targ_lens - lens), computed by ComputeStim
  float_Matrix  err_int;    // #EXPERT #NO_SAVE integrated error over time (I in PID)
  float_Matrix  err_deriv;  // #EXPERT #NO_SAVE derivative of error over time (D in PID)
  float_Matrix  err_dra;    // #EXPERT #NO_SAVE running-average of err_deriv -- uses pid_dra_dt parameter -- this is what is actually used in PID controller, so set pid_dra_dt to 1 if you want literal std D factor
  float_Matrix  err_prv;    // #EXPERT #NO_SAVE previous error values in PID
  float_Matrix  stims;      // #EXPERT #NO_SAVE stimulation values, computed by ComputeStim
  float_Matrix  forces;     // #EXPERT #NO_SAVE forces, computed by ComputeStim

  float_Matrix  max_lens;   // #EXPERT maximum muscle lengths, initialized by ConfigArm, used to normalize lengths
  float_Matrix  min_lens;   // #EXPERT minimum muscle lengths, initialized by ConfigArm 
  float_Matrix  rest_lens;  // #EXPERT resting muscle lengths, initialized by ConfigArm 
  float_Matrix  spans;      // #HIDDEN 1/(max_lens-min_lens). Used to speed up the calculation of norm_lengths.

  // overall state variables for the arm
  float_Matrix  musc_gains;     // #READ_ONLY #SHOW muscle-specific gain factors -- these operate in addition to the overall gain, and multiply the target - actual length to modulate the effective force applied on a given muscle -- these are what the cerebellum operates on -- default value should be 1.0
  float_Matrix  norm_lens;      // #READ_ONLY #SHOW normalized current muscle lengths
  float_Matrix  norm_targ_lens; // #READ_ONLY #SHOW normalized target muscle lengths
  float_Matrix  norm_vels;      // #READ_ONLY #SHOW normalized muscle velocities
  float_Matrix  norm_err;       // #READ_ONLY #SHOW normalized muscle errors
  float_Matrix  norm_err_deriv; // #READ_ONLY #SHOW normalized muscle error derivatives
  float_Matrix  norm_err_dra;   // #READ_ONLY #SHOW running average of norm_err_deriv using norm_err_dra_dt time constant -- these values can be useful for computing cerebellar control
  float_Matrix  norm_err_prv;   // #READ_ONLY #SHOW previous normalized muscle errors
  float         avg_vel_mag;    // #READ_ONLY #SHOW average of normalized velocity magnitude (absolute value) -- useful to determine how much the arm is currently moving overall

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

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z);
  // #BUTTON place the hand at the specified target. This method can crash if the arm hasn't been set to its initial position. Returns true if a move is made (even if the target is not reachable).

  virtual bool TargetLengths(float trg_x, float trg_y, float trg_z);
  // #BUTTON Obtain the muscle lengths which position the hand at the given coordinates, and place them in the targ_lens matrix, which will have a length equal to the number of muscles. Returns false if failed.
  virtual bool TargetLengths_impl(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z);
  // #EXPERT Obtain the muscle lengths which position the hand at the given coordinates, and place them in the given matrix, which should have a length equal to the number of muscles. Returns false if failed.
  virtual bool NoisyTargetLengths(float trg_x, float trg_y, float trg_z);
  // Like TargetLengths, but noise is applied to the arm rotation (gamma angle). Useful to generate sensible muscle lengths for training purposes.
  virtual bool NoisyTargetLengths_impl(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z);
  // #EXPERT Implements the main functionality of NoisyTargetLengths

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

  virtual bool AngToLengths(float_Matrix &Len, float alpha, float beta, float gamma, float delta);
  // Given the four angles describing arm position, calculate the muscle lengths at that position

  virtual bool NormLengthsToTable(DataTable* len_table);
  // Write the normalized muscle lengths into a datatable, in column named "lengths", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormTargLengthsToTable(DataTable* len_table);
  // Write the normalized target muscle lengths into a datatable, in column named "targ_lengths", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormSpeedsToTable(DataTable* len_table);
  // Write the normalized muscle contraction speeds into a datatable, in column named "speeds", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormHandCoordsToTable(DataTable* coords_table);
  // Write the normalized XYZ shoulder-centered coordinates of the hand into a datatable, in a column named "hand_coords", formated in a 4 dimensional 1x1 by 1x3 geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x3 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool ArmStateToTable(DataTable* len_table);
  // Write normalized lengths, speeds, and target lengths to a datatable -- calls above functions -- all are formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row

  virtual bool SetTargetLengthsFmTable(DataTable* len_table);
  // Update the unnormalized target lengths (targ_lens) using normalized values from a DataTable. The received DataTable must contain a float column named "lengths" with 4 dimensional cell geometry n x 1 by 1 x n_musc. n_musc is the number of muscles, and n is an integer equal or greater than 1.

  // these functions (step_pre and CurFromODE) are called by VEWorld Step -- they
  // automatically update the muscle forces using VEP_Reach, and update the IPs etc
  override void Step_pre();
  override void CurFromODE(bool updt_disp = false);
  override void Init();

  TA_SIMPLE_BASEFUNS(VEArm);
protected:
  //  override CheckConfig_impl(); // todo

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

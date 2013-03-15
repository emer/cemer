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
    LINEAR,
    HILL,
  };

  ArmSide       arm_side;       // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  VEBodyRef     torso;          // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to
  UpAxis        up_axis;        // which axis points upwards. This selects whether to use the COIN coordinate system (with the Y axis upwards), or the system originally used in SimMechanics (with the Z axis pointing upwards). Coordinates transformation between these systems comes through the CT matrix.
  MuscGeo       musc_geo;       // The muscle geometry. Geometries differ in the number of muscles (11 vs 12), and in the location of the insertion points
  MuscType      musc_type;      // The muscle model. Either linear (output force proportional to stimulus) or Hill-type (the muscle model used in Gribble et al. 1998)
  float         gain;           // gain factor for multiplying forces
  float         damping;        // angular damping factor
  float         damping_thr;    // angular damping threshold 
  float         vel_norm_gain;  // speed normalization gain factor for a sigmoidal compression function

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
  float_Matrix  forces;     // #EXPERT forces, computed by the TargetLengths function for a given 3D target location

  float_Matrix  max_lens;   // #EXPERT maximum muscle lengths, initialized by ConfigArm, used to normalize lengths
  float_Matrix  min_lens;   // #EXPERT minimum muscle lengths, initialized by ConfigArm 
  float_Matrix  rest_lens;  // #EXPERT resting muscle lengths, initialized by ConfigArm 
  float_Matrix  spans;      // #HIDDEN 1/(max_lens-min_lens). Used to speed up the calculation of norm_lengths.

  // overall state variables for the arm
  float_Matrix  norm_lens;      // #READ_ONLY #SHOW normalized current muscle lengths
  float_Matrix  norm_targ_lens; // #READ_ONLY #SHOW normalized target muscle lengths
  float_Matrix  norm_vels;      // #READ_ONLY #SHOW normalized muscle velocities
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
                          float sh_offX = 0.25, float sh_offY = 0.2, float sh_offZ = 0);
  // #BUTTON configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z);
  // #BUTTON place the hand at the specified target. This method can crash if the arm hasn't been set to its initial position. Returns true if a move is made (even if the target is not reachable).

  virtual bool TargetLengths(float trg_x, float trg_y, float trg_z);
  // #BUTTON Obtain the muscle lengths which position the hand at the given coordinates, and place them in the targ_lens matrix, which will have a length equal to the number of muscles. Returns false if failed.
  virtual bool TargetLengths_impl(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z);
  // #EXPERT Obtain the muscle lengths which position the hand at the given coordinates, and place them in the given matrix, which should have a length equal to the number of muscles. Returns false if failed.
  virtual void GetRandomTarget(float& trg_x, float& trg_y, float& trg_z,
                               float x_ang_min = 0.0f, float x_ang_max = 1.5f,
                               float y_ang_min = 0.0f, float y_ang_max = 0.0f,
                               float z_ang_min = -3.14159, float z_ang_max = 3.14159,
                               float min_dist = 0.2f, float max_dist = 0.9f);
  // generate a random target location that is actually reachable by this arm, by projecting a point out from the shoulder along a randomly generated angle within min/max range for each euler angle direction (x,y,z), with a distance randomly selected within normalized min/max range (1.0 = full length of arm) -- defaults work well for generating points in front of a typical arm configuration

  virtual bool UpdateIPs();
  // #BUTTON Set the muscle IPs to the values in the rotated xxxIP matrices, and update norm_lengths

  virtual bool Lengths(float_Matrix& len, bool normalize);
  // Put the current lengths of all muscles in the given matrix (if normalize is true, return normalized 0..1) -- sets len geom to 1,n_musc
  virtual bool Speeds(float_Matrix& vel, bool normalize);
  // Put the muscle contraction speeds of the last time step in the given matrix (if normalize is true, return normalized 0..1) -- sets vel geom to 1,n_musc

  virtual bool ApplyStim(const float_Matrix& stims, float_Matrix &fs);
  // Apply a stimulus to the arm muscles. The first argument is a vector matrix with the stimuli. The second argument is a vector matrix where the resulting contraction forces will be stored; it should have 3 rows and Nmusc columns.

  virtual bool VEP_Reach(const float_Matrix& trg_lens, float gain, float_Matrix &fs);
  // Do one step of reaching using the velocity-controlled Equilibrium Point algorithm. This will calculate the activation (multiplying both errors by the gain), calculate (and store) the resulting forces, and apply them. It does not take a step of the VEWorld, and does not udpate the muscle insertion points.

  virtual bool AngToLengths(float_Matrix &Len, float alpha, float beta, float gamma, float delta);
  // Given the four angles describing arm position, calculate the muscle lengths at that position

  virtual bool NormLengthsToTable(DataTable* len_table);
  // Write the normalized muscle lengths into a datatable, in column named "lengths", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormTargLengthsToTable(DataTable* len_table);
  // Write the normalized target muscle lengths into a datatable, in column named "targ_lengths", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool NormSpeedsToTable(DataTable* len_table);
  // Write the normalized muscle contraction speeds into a datatable, in column named "speeds", formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row
  virtual bool ArmStateToTable(DataTable* len_table);
  // Write normalized lengths, speeds, and target lengths to a datatable -- calls above functions -- all are formatted with in a 4 dimensional 1x1 by 1 x n_musc (typically 12) geometry appropriate for writing to ScalarValLayerSpec layer, with unit groups arranged in a 1x12 group geometry, where the first unit of each unit group (1x1 inner dimension unit geometry) contains the scalar value that we write to. Always writes to the last row in the table, and ensures that there is at least one row

  virtual bool SetTargetLengthsFmTable(DataTable* len_table);
  // Update the unnormalized target lengths (targ_lens) using normalized values from a DataTable. The received DataTable must contain a float column named "targ_lengths" with 4 dimensional cell geometry 1 x 1 by 1 x n_musc.

  // these functions (step_pre and CurFromODE) are called by VEWorld Step -- they
  // automatically update the muscle forces using VEP_Reach, and update the IPs etc
  override void Step_pre();
  override void CurFromODE(bool updt_disp = false);

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

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
#include <VELinMuscle_List>

// declare all other types mentioned but not required to include:


TypeDef_Of(VEArm);

class TA_API VEArm : public VEObject{
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

  ArmSide       arm_side;       // is this the left or right arm?  affects the configuration of the arm, and where it attaches to the torso
  VEBodyRef     torso;          // the torso body -- must be set prior to calling ConfigArm -- this should be a VEBody in another object (typically in the same object group) that serves as the torso that the shoulder attaches to

  float 	La;     // #READ_ONLY #SHOW the length of the humerus
  float 	Lf;     // #READ_ONLY #SHOW length of the forearm (ulna,hand radius,gaps)
  float		elbow_gap;  // #READ_ONLY #SHOW the distance between ulna and humerus
  float 	wrist_gap;  // #READ_ONLY #SHOW the distance between hand and ulna
  float 	WorldStep; // #READ_ONLY a copy of the owner VEWorld's stepsize, used for calculating speeds
  float_Matrix ShouldIP; // shoulder insertion points at rest
  float_Matrix ArmIP; // humerus insertion points at rest
  float_Matrix FarmIP; // ulna insertion points at rest
  float_Matrix p1; // first end points for bending lines
  float_Matrix p2; // second end points for bending lines
  int 		Nmusc; // the total number of muscles, as implied by the IP matrices

  VELinMuscle_List muscles;  // all the muscles attached to the arm

  virtual bool  CheckArm(bool quiet = false);
  // check to see if the arm is all configured OK -- it flags an error if not unless quiet -- returns true if OK, false if not

  virtual void	InitMuscles();
  // initialize muscles and insertion point data

  virtual bool  ConfigArm(const String& name_prefix="",
                          float humerus_length = 0.3, float humerus_radius = 0.03,
                          float ulna_length = 0.24, float ulna_radius = 0.02,
                          float hand_length = 0.08, float hand_radius = 0.03,
                          float elbowGap = 0.03,   float wristGap = 0.03);
  // #BUTTON configure the arm bodies and joints, using the given length parameters and other options -- will update the lengths if the arm has already been created before -- returns success

  virtual bool MoveToTarget(float trg_x, float trg_y, float trg_z);
  // #BUTTON place the hand at the specified target. This method can crash if the arm hasn't been set to its initial position. Returns true if a move is made (even if the target is not reachable).

  virtual bool TargetLengths(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z);
  // Obtain the muscle lengths which position the hand at the given coordinates, and place them in the given matrix, which should have a length equal to the number of muscles. Returns false if failed.

  virtual bool UpdateIPs();
  // #BUTTON Setting the muscle IPs to the values in the xxxIP matrices

  virtual bool Lengths(float_Matrix &Len);
  // Put the current lengths of all muscles in the given matrix

  virtual bool Speeds(float_Matrix &Vel);
  // Put the muscle contraction speeds of the last time step in the given matrix

  virtual bool ApplyStim(float_Matrix stims, float_Matrix &forces);
  // Apply a stimulus to the arm muscles. The first argument is a vector matrix with the stimuli. The second argument is a vector matrix where the resulting contraction forces will be stored; it should have 3 rows and Nmusc columns.

  TA_SIMPLE_BASEFUNS(VEArm);
protected:
  //  override CheckConfig_impl(); // todo

  virtual bool bender(taVector3f &p3, taVector3f a, taVector3f c, taVector3f p1, taVector3f p2);
  // This function is the C++ equivalent of piece5.m.
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector p3.

private:
  void  Initialize();
  void  Destroy();
};

#endif // VEArm_h

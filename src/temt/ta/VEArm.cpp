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

#include "VEArm.h"

void VEArm::Initialize() {
  // just the default initial values here -- note that VEObject parent initializes all the space stuff in its Initialize, so you don't need to do that here
  arm_side = RIGHT_ARM;
  // note: torso ref is self initializing
  La = 0.3f;
  Lf = 0.33f;
  elbow_gap = 0.03f; // space left between bodies so joint can rotate
  wrist_gap = 0.03f;

  // DO NOT put anythying other than direct literal member inits in here -- no creating objects etc
}

bool VEArm::UpdateIPs() {

// We update the muscles' past lengths before setting new IPs
  for(int i=0; i<Nmusc; i++) {
    muscles[i]->old_length2 = muscles[i]->old_length1;
    muscles[i]->old_length1 = muscles[i]->Length();
  }

// To set new IPs, first we'll find the coordinates of the rotated IPs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEJoint* elbow = joints[ELBOW];

  float_Matrix R(2,3,3);
  humerus->cur_quat.ToRotMatrix(R);

  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

/*
  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);
*/

  // rotating the ulna's insertion points
        //elbow->CurFromODE(true);      // so the angle we get is actualized
  float delta = elbow->pos;     // DON'T KNOW IF THIS WILL WORK
  //taMisc::Info("elbow pos at UpdateIPs: ", String(delta), "\n");
  float UlnaShift_f[] = {0, 0, -La,
                         0, 0, -La,
                         0, 0, -La}; // should have one row per forearm IP
  float T_elbowRot_f[] = {1, 0, 0,
                0, cos(delta),  sin(delta),
                0, -sin(delta), cos(delta)};
  // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
  float_Matrix UlnaShift(2,3,3);
  UlnaShift.InitFromFloats(UlnaShift_f);
  float_Matrix T_elbowRot(2,3,3);
  T_elbowRot.InitFromFloats(T_elbowRot_f);

// first we shift the FarmIP's so the origin is at the elbow
  float_Matrix ShiftedIP(2,3,3);
  ShiftedIP = FarmIP - UlnaShift;

// we rotate the shifted IPs by the elbow bend (delta rotation)
  float_Matrix Rot1FarmIP(2,3,3);
  taMath_float::mat_mult(&Rot1FarmIP, &ShiftedIP, &T_elbowRot);
/*
  String ruout;
  Rot1FarmIP.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);
*/

// now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

// finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

/*
  String ripout;
  RotFarmIP.Print(ripout);
  taMisc::Info("rotated ulna IPs:\n", ripout);
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Now we assign the rotated IPs to the corresponding muscles
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8; i++) {  // the 8 shoulder to humerus muscles
    muscles[i]->IPprox.x = ShouldIP.FastElAsFloat(0,i);
    muscles[i]->IPprox.y = ShouldIP.FastElAsFloat(1,i);
    muscles[i]->IPprox.z = ShouldIP.FastElAsFloat(2,i);
    muscles[i]->IPdist.x = RotArmIP.FastElAsFloat(0,i);
    muscles[i]->IPdist.y = RotArmIP.FastElAsFloat(1,i);
    muscles[i]->IPdist.z = RotArmIP.FastElAsFloat(2,i);

    humerIP = muscles[i]->IPdist;
    shoulderIP = muscles[i]->IPprox;
    pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
    pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
    pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

    if(bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
      // if muscle wraps around bending line (except for muscles 3 and 4)
      muscles[i]->p3 = p3;
      muscles[i]->bend = true;
    } else {
      muscles[i]->bend = false;
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  muscles[8]->IPprox.x = ShouldIP.FastElAsFloat(0,8);
  muscles[8]->IPprox.y = ShouldIP.FastElAsFloat(1,8);
  muscles[8]->IPprox.z = ShouldIP.FastElAsFloat(2,8);
  muscles[8]->IPdist.x = RotFarmIP.FastElAsFloat(0,0);
  muscles[8]->IPdist.y = RotFarmIP.FastElAsFloat(1,0);
  muscles[8]->IPdist.z = RotFarmIP.FastElAsFloat(2,0);
  muscles[8]->bend = false;
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    muscles[8+i]->IPprox.x = RotArmIP.FastElAsFloat(0,7+i);
    muscles[8+i]->IPprox.y = RotArmIP.FastElAsFloat(1,7+i);
    muscles[8+i]->IPprox.z = RotArmIP.FastElAsFloat(2,7+i);
    muscles[8+i]->IPdist.x = RotFarmIP.FastElAsFloat(0,i);
    muscles[8+i]->IPdist.y = RotFarmIP.FastElAsFloat(1,i);
    muscles[8+i]->IPdist.z = RotFarmIP.FastElAsFloat(2,i);
    muscles[8+i]->bend = false;
  }
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

  return true;
}

void VEArm::Destroy() {
  CutLinks();
}

bool VEArm::CheckArm(bool quiet) {
  bool rval = true;
  if(!torso) {
    rval = false;
    TestError(!quiet, "CheckArm", "torso not set -- must specify a body in another object to serve as the torso");
  }
  if(bodies.size < N_ARM_BODIES) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of bodies < N_ARM_BODIES -- run ConfigArm");
  }
  if(joints.size < N_ARM_JOINTS) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of joints < N_ARM_JOINTS -- run ConfigArm");
  }
  if(Nmusc != (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6)) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of muscles doesn't match number of insertion points -- run ConfigArm");
  }
  // probably that's sufficient for a quick-and-dirty sanity check
  // Also: Nmuscl equal to # of elements in muscles
  return rval;
}

void VEArm::InitMuscles() {
  // Initializing the insertion point matrices, assuming RIGHT_ARM and shoulder at origin
  // Should copy this to ConfigArm

  // Here are all the muscle insertion points in the trunk and shoulder, as
  // labeled in (46) starting with point e
  float ShouldIP_f[] = { -0.05f,  0,      0,       -0.11f,  0,     -0.07f, // e,g
                         -0.08f,  0.05f,  0,       -0.08f, -0.05f,  0,     // i,k -0.08f,  0.01f,  0,       -0.08f, -0.01f,  0,     // i,k
                         -0.14f,  0.06f,  0.01f,   -0.10f, -0.05f,  0.01f, // m,o
                         -0.14f,  0.06f, -0.12f,   -0.10f, -0.05f, -0.12f, // q,s
                         -0.02f,  0.02f,  0.01f };                         // t

  ShouldIP.SetGeom(2,3,9);     // the matrix has two indices, 3 columns and 9 rows
  ShouldIP.InitFromFloats(ShouldIP_f);

  // Here are all the muscle insertion points in the arm, as labeled in (46),
  // starting with point d
  float ArmIP_f[] = {  0.02f,  0,      -0.05f,   -0.02f,  0,      -0.05f,   // d,f
                       0,      0.02f,   0,        0,     -0.02f,   0,      // h,j
                      -0.01f,  0.01f,  -0.06f,   -0.01f, -0.01f,  -0.06f,   // l,n
                      -0.01f,  0.01f,  -0.05f,   -0.01f, -0.01f,  -0.05f,   // p,r
                       0,     -0.015f, -0.06f,    0,      0.015f, -0.15f }; // v,x

  ArmIP.SetGeom(2,3,10);
  ArmIP.InitFromFloats(ArmIP_f);

  //String aout;
  //ArmIP.Print(aout);
  //taMisc::Info("init ArmIP:\n", aout);

  // Here are the muscle insertion points in the forearm, corresponding to the
  // biceps, the triceps, and the brachialis.
  float FarmIP_f[] = {  0,      0.015f, -La - 0.05f,
                        0,     -0.005f, -La + 0.03f,
                       -0.01f,  0.015f, -La - 0.04f };
  FarmIP.SetGeom(2,3,3);
  FarmIP.InitFromFloats(FarmIP_f);

  // Here are the initial and final points of the restricted bending lines for
  // each one of the muscles 1-8. The values for muscles 3,4 are not used
  float p1_f[] = {  0.03f, -0.02f,  0.02f,    0.01f,  0.03f, -0.02f,
                    0,      0.02f,  0.01f,    0,     -0.02f,  0.01f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f };
  p1.SetGeom(2,3,8);
  p1.InitFromFloats(p1_f);

  float p2_f[] = {  0.03f,  0.02f,  0.02f,    0.01f, -0.03f, -0.02f,
                    0,      0.02f, -0.01f,    0,     -0.02f, -0.01f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f };
  p2.SetGeom(2,3,8);
  p2.InitFromFloats(p2_f);

  // Initializing the muscles
  // the number of muscles is 1/2 the sum of points in ShoulderIP+ArmIP+FarmIP
  Nmusc = (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6);
  muscles.SetSize(Nmusc); // creating VELinearMuscle objects to populate group

  //UpdateIPs(); // attaching muscles to their corresponding insertion points
  // crashes the program at VEBody* humerus = bodies[HUMERUS];
  //for(int i=0; i<Nmusc; i++) // initializing past muscle lengths
  //muscles[i]->old_length2 = muscles[i]->old_length1 = muscles[i]->Length();
}

bool VEArm::ConfigArm(const String& name_prefix,
                      float humerus_length, float humerus_radius,
                      float ulna_length, float ulna_radius,
                      float hand_length, float hand_radius, 
                      float elbowGap, float wristGap) {
  // note: keeping torso out of it, so the arm is fully modular and can attach to anything

  if(TestError(!torso, "ConfigArm", "torso not set -- must specify a body in another object to serve as the torso before running ConfigArm"))
    return false;

  InitMuscles();

  if(bodies.size < N_ARM_BODIES)
    bodies.SetSize(N_ARM_BODIES); // auto-creates the VEBody objects up to specified size
  if(joints.size < N_ARM_JOINTS)
    joints.SetSize(N_ARM_JOINTS); // auto-creates the VEJoint objects up to specified size

  // Getting the stepsize of VEWorld containing this Arm
  VEWorld* Worldly = GetWorld();
  WorldStep = Worldly->stepsize;

  // this is how you access the bodies -- very fast and efficient
  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];

  VEJoint* shoulder = joints[SHOULDER];
  VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];

  //------- Setting standard geometry -------
  // todo: just need to know where to attach to torso..
  // torso.shape = VEBody::BOX;
  // torso.length = 0.4;
  // torso.init_pos.x = -0.25; torso.init_pos.y = 0; torso.init_pos.z = -0.2;
  // torso.cur_pos.x = -0.25; torso.cur_pos.y = 0; torso.cur_pos.z = -0.2;
  // torso.box.x = 0.35; torso.box.y = 0.1; torso.box.z = 0.4;
  // torso.SetValsToODE();

  elbow_gap = elbowGap;
  wrist_gap = wristGap;

  // todo: need to make all this conditional on arm_side!
  humerus->name = name_prefix + "Humerus";
  humerus->shape = VEBody::CAPSULE; humerus->long_axis = VEBody::LONG_Z;
  humerus->length = humerus_length; humerus->radius = humerus_radius;
  humerus->init_pos.x = 0; humerus->init_pos.y = 0; humerus->init_pos.z = -humerus_length/2;
  humerus->init_rot.x = 0; humerus->init_rot.y = 0; humerus->init_rot.z = 0;
  humerus->cur_pos.x = 0; humerus->cur_pos.y = 0; humerus->cur_pos.z = humerus->init_pos.z;
  humerus->cur_rot.x = 0; humerus->cur_rot.y = 0; humerus->cur_rot.z = 0;

  ulna->name = name_prefix + "Ulna";
  ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Z;
  ulna->length = ulna_length; ulna->radius = ulna_radius;
  ulna->init_pos.x = 0; ulna->init_pos.y = 0; ulna->init_pos.z =-humerus_length-(ulna_length/2)-elbow_gap;
  ulna->init_rot.x = 0; ulna->init_rot.y = 0; ulna->init_rot.z = 0;
  ulna->cur_pos.x = 0; ulna->cur_pos.y = 0; ulna->cur_pos.z = ulna->init_pos.z;
  ulna->cur_rot.x = 0; ulna->cur_rot.y = 0; ulna->cur_rot.z = 0;

  hand->name = name_prefix + "Hand";
  hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Z;
  hand->length = hand_length; hand->radius = hand_radius;
  hand->init_pos.x = 0; hand->init_pos.y = 0;
  hand->init_pos.z = -(humerus_length+ulna_length+elbow_gap+wrist_gap+(hand_length/2));
  hand->init_rot.x = 0; hand->init_rot.y = 0; hand->init_rot.z = 0;
  hand->cur_pos.x = 0; hand->cur_pos.y = 0; hand->cur_pos.z = hand->init_pos.z;
  hand->cur_rot.x = 0; hand->cur_rot.y = 0; hand->cur_rot.z = 0;

  La = humerus_length + elbow_gap/2;
  Lf = ulna_length + (elbow_gap/2) + wrist_gap + (hand_length/2);

  //-------- Creating initial joints -------

  //-------- Setting joint locations -------
  shoulder->name = name_prefix + "Shoulder";
  shoulder->joint_type = VEJoint::BALL;
  shoulder->body1 = torso; shoulder->body2 = humerus;

  elbow->name = name_prefix + "Elbow";
  elbow->joint_type = VEJoint::HINGE2;
  elbow->body1 = humerus; elbow->body2 = ulna;

  wrist->name = name_prefix + "Wrist";
  wrist->joint_type = VEJoint::FIXED;
  wrist->body1 = ulna; wrist->body2 = hand;

  // the shoulder anchor is wrt to torso's CM, should lie near one corner
  shoulder->anchor.x = 0.25f; shoulder->anchor.y = 0; shoulder->anchor.z = 0.2f;

  elbow->anchor.x = 0;  // set elbow joint's anchor point wrt humerus' CM
  elbow->anchor.y = 0;
  elbow->anchor.z = -(humerus->length/2 +(elbow_gap/2));

  wrist->anchor.x = 0; // set wrist joint's anchor point wrt ulna's CM
  wrist->anchor.y = 0;
  wrist->anchor.z = -(ulna->length/2 + (wrist_gap/2));

  elbow->axis.x = -1;  // setting elbow joint's axes
  elbow->axis.y = 0;
  elbow->axis.z = 0;
  elbow->axis2.x = 0;
  elbow->axis2.y = 0;
  elbow->axis2.z = -1;

// Initializing the insertion point matrices, assuming RIGHT_ARM and shoulder at origin
// Should copy this to ConfigArm

  // Here are all the muscle insertion points in the trunk and shoulder, as
  // labeled in (46) starting with point e
  float ShouldIP_f[] = { -0.05f,  0,      0,       -0.11f,  0,     -0.07f, // e,g
                         -0.08f,  0.05f,  0,       -0.08f, -0.05f,  0,     // i,k  -0.08f,  0.01f,  0,       -0.08f, -0.01f,  0,     // i,k
                         -0.14f,  0.06f,  0.01f,   -0.10f, -0.05f,  0.01f, // m,o
                         -0.14f,  0.06f, -0.12f,   -0.10f, -0.05f, -0.12f, // q,s
                         -0.02f,  0.02f,  0.01f };                         // t

  ShouldIP = float_Matrix(2,3,9);     // the matrix has two indices, 3 columns and 9 rows
  ShouldIP.InitFromFloats(ShouldIP_f);

  // Here are all the muscle insertion points in the arm, as labeled in (46),
  // starting with point d
  float ArmIP_f[] = {  0.02f,  0,      -0.05f,   -0.02f,  0,      -0.05f,   // d,f
                       0,      0.02f,   0,        0,     -0.02f,   0,       // h,j
                      -0.01f,  0.01f,  -0.06f,   -0.01f, -0.01f,  -0.06f,   // l,n
                      -0.01f,  0.01f,  -0.05f,   -0.01f, -0.01f,  -0.05f,   // p,r
                       0,     -0.015f, -0.06f,    0,      0.015f, -0.15f }; // v,x

  ArmIP = float_Matrix(2,3,10);
  ArmIP.InitFromFloats(ArmIP_f);

  //String aout;
  //ArmIP.Print(aout);
  //taMisc::Info("init ArmIP:\n", aout);

  // Here are the muscle insertion points in the forearm, corresponding to the
  // biceps, the triceps, and the brachialis.
  float FarmIP_f[] = {  0,      0.015f, -La - 0.05f,
                        0,     -0.005f, -La + 0.03f,
                       -0.01f,  0.015f, -La - 0.04f };
  FarmIP = float_Matrix(2,3,3);
  FarmIP.InitFromFloats(FarmIP_f);

  // Here are the initial and final points of the restricted bending lines for
  // each one of the muscles 1-8. The values for muscles 3,4 are not used
  float p1_f[] = {  0.06f, -0.02f,  0.02f,    0.01f,  0.03f, -0.02f, // changed first from 0.03f to 0.06f
                    0,      0.02f,  0.01f,    0,     -0.02f,  0.01f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f,
                   -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f };
  p1 = float_Matrix(2,3,8);
  p1.InitFromFloats(p1_f);

  float p2_f[] = {  0.06f,  0.02f,  0.02f,    0.01f, -0.03f, -0.02f, // changed first from 0.03f to 0.06f
                    0,      0.02f, -0.01f,    0,     -0.02f, -0.01f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f,
                   -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f };
  p2 = float_Matrix(2,3,8);
  p2.InitFromFloats(p2_f);

// Initializing the muscles
  // the number of muscles is 1/2 the sum of points in ShoulderIP+ArmIP+FarmIP
  Nmusc = (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6);
  muscles.SetSize(Nmusc);

  UpdateIPs(); // attaching muscles to their corresponding insertion points

  for(int i=0; i<Nmusc; i++) // initializing past muscle lengths
    muscles[i]->old_length2 = muscles[i]->old_length1 = muscles[i]->Length();

  Init();                       // this will attempt to init everything just created..

  DataChanged(DCR_ITEM_UPDATED); // this will in theory update the display

  return true;
}

bool VEArm::MoveToTarget(float trg_x, float trg_y, float trg_z) {
// at some point I should set the arm to its initial position here
  if(!CheckArm()) return false;

  if(trg_x == 0.0 && trg_y == 0.0) {
    trg_y = 0.000001f;  // this is to avoid a Gimble lock
  }

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];
  VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z};
  float_Matrix T(1,3);
  T.InitFromFloats(t_f);
  float D = taMath_float::vec_norm(&T);

  if(D < 0.1) {
    taMisc::Info("Target too close \n");
    // moving the target away, maintaining direction
    float Lfactor = 0.15/D;
    trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
    T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
    D = taMath_float::vec_norm(&T);
  } else if( D >= (La+Lf)) {
      taMisc::Info("Target too far \n");
    // bringing the target closer, maintaining direction
      float Lfactor = (La+Lf-0.01)/D;
      trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
      T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
      D = taMath_float::vec_norm(&T);
  }

  // From coordinates to angles as in (44)

  float delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
  float gamma = 0;
  float beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
  float alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

  if(trg_y < 0) { // if the target is behind
    alpha = taMath_float::pi - alpha;
  }

  taMisc::Info("alpha:", String(alpha), "beta:", String(beta), "gamma:", String(gamma));

  // Now we'll rotate the insertion points by the Euler angles in reverse order
  // This magic R matrix (from (42)) does it all in one step
  float sa = sin(alpha); float ca = cos(alpha);
  float sb = sin(beta);  float cb = cos(beta);
  float sc = sin(gamma); float cc = cos(gamma);

  float R_f[] = {ca*cc-sa*cb*sc, -ca*sc-sa*cb*cc, sa*sb,
                 sa*cc+ca*cb*sc, -sa*sc+ca*cb*cc, -ca*sb,
                 sb*sc,           sb*cc,          cb};
  float_Matrix R(2,3,3);
  R.InitFromFloats(R_f);

/*
  String Rout;
  R.Print(Rout);
  taMisc::Info("rotation matrix:\n", Rout, "\n");

  //  taQuaternion quat(alpha, beta, gamma);
  // this matches your matrix -- for some reason beta and gamma are reversed???
  //taQuaternion quat(beta, gamma, alpha);

  taQuaternion quat(beta,gamma,alpha);
  float_Matrix qR;
  quat.ToRotMatrix(qR);

  String qRout;
  qR.Print(qRout);
  taMisc::Info("quat rotation matrix:\n", qRout, "\n");

  // test direct quat rotation of first coords, against quat rotation matrix
  taVector3f arm1;
  arm1.FromMatrix(ArmIP);       // grab first coords
  quat.RotateVec(arm1);
  taMisc::Info("quat rotate first ArmIP coords:\n", arm1.GetStr(), "\n");


  // compute: RotArmIP = (R*ArmIP')';
  float_Matrix armipt;
  taMath_float::mat_transpose(&armipt, &ArmIP);

  String tout;
  armipt.Print(tout);
  taMisc::Info("transposed ArmIP:\n", tout);

  float_Matrix armipr;
  taMath_float::mat_mult(&armipr, &R, &armipt);

  String rout;
  armipr.Print(rout);
  taMisc::Info("rot pretransposed ArmIP:\n", rout);

  float_Matrix RotArmIP;
  taMath_float::mat_transpose(&RotArmIP, &armipr);

  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);
*/


//------ Rotating humerus -------
  //humerus->RotateEuler(beta,gamma,alpha,true); // may substitute by Init_Pos();
  //humerus->Init_Pos();
  //humerus->Init_Rotation();
  //Init();
  humerus->RotateEuler(beta,gamma,alpha,false);

  float HumCM_f[] = {0.0f,0.0f,(-La+(elbow_gap/2))/2};  // humerus' geometrical center at rest
  //float Elbow_f[] = {0.0f,0.0f,(-La+(elbow_gap/2))};  // elbow coordinates at rest
  float Elbow_f[] = {0.0f,0.0f,-La};  // elbow coordinates at rest

  float_Matrix HumCM(2,1,3);
  HumCM.InitFromFloats(HumCM_f);
  float_Matrix Elbow(2,1,3);
  Elbow.InitFromFloats(Elbow_f);

  float_Matrix RotHumCM(2,1,3);
  taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center
  float_Matrix RotElbow(2,1,3);
  taMath_float::mat_mult(&RotElbow, &R, &Elbow);  // rotating elbow

humerus->Translate(RotHumCM.FastEl(0),RotHumCM.FastEl(1),RotHumCM.FastEl(2)-(humerus->init_pos.z),false);


//------ Rotating ulna -------
/*
  taVector3f UlnaCM(0.0f, 0.0f, -Lf/2); // Ulna CM with origin at elbow
  taQuaternion flex(0.0f,delta,0.0f); // elbow flexion rotation
  flex.RotateVec(UlnaCM);   // Ulna CM after elbow flexion with origin at elbow
  UlnaCM.SetXYZ(UlnaCM.x,UlnaCM.y,UlnaCM.z-La);  // Ulna CM after elbow flexion
  quat.RotateVec(UlnaCM);       // Ulna CM after elbow flexion, origin at shoulder

  ulna->RotateEuler(delta, 0.0, 0.0, false);
  ulna->RotateEuler(beta,gamma,alpha,false);
  ulna->Translate(UlnaCM.x,UlnaCM.y,UlnaCM.z+La+Lf/2,false);
*/
  float UlnaCM_f[] = {0,0,-(ulna->length/2 + elbow_gap/2)};  // Ulna 'CM' with origin at elbow
  float Wrist_f[] = {0,0,-(ulna->length + elbow_gap/2 + wrist_gap/2)};  // wrist coords with origin at elbow
  float elbow_rot_f[] = {1 , 0, 0,
                0, cos(delta), -sin(delta),
                0, sin(delta), cos(delta)};
  float_Matrix UlnaCM(2,1,3);
  UlnaCM.InitFromFloats(UlnaCM_f);
  float_Matrix Wrist(2,1,3);
  Wrist.InitFromFloats(Wrist_f);
  float_Matrix elbow_rot(2,3,3);
  elbow_rot.InitFromFloats(elbow_rot_f);

  String erout;
  elbow_rot.Print(erout);
  taMisc::Info("elbow rotation matrix :\n", erout);

  float_Matrix Rot1UlnaCM(2,1,3);
  taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);
  float_Matrix Rot1Wrist(2,1,3);
  taMath_float::mat_mult(&Rot1Wrist, &elbow_rot, &Wrist);

  String ruout;
  Rot1UlnaCM.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);

  Rot1UlnaCM.Set(Rot1UlnaCM.FastEl(2)-La,2); // setting origin at shoulder
  Rot1Wrist.Set(Rot1Wrist.FastEl(2)-La,2);

  String rudout;
  Rot1UlnaCM.Print(rudout);
  taMisc::Info("rotated ulna after translation:\n", rudout);

  float_Matrix Rot2UlnaCM(2,1,3);
  taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation
  float_Matrix Rot2Wrist(2,1,3);
  taMath_float::mat_mult(&Rot2Wrist, &R, &Rot1Wrist);

/*
  taVector3f Rot2UlnaCM;
  Rot2UlnaCM.FromMatrix(Rot1UlnaCM);
  quat.RotateVec(Rot2UlnaCM);
*/
  ulna->RotateEuler(beta+delta,gamma,alpha,false);
  ulna->Translate(Rot2UlnaCM.FastEl(0),Rot2UlnaCM.FastEl(1),Rot2UlnaCM.FastEl(2)-(ulna->init_pos.z),false);

  hand->RotateEuler(beta+delta,gamma,alpha,false);
  hand->Translate(trg_x,trg_y,trg_z-(hand->init_pos.z),false);

  // sending the values of the bodies to ODE
  CurToODE();

  // calculating and updating the joint values
  // setting the axes for the elbow joint
  elbow->axis.x = cos(alpha);
  elbow->axis.y = sin(alpha);
  elbow->axis.z = 0.0f;
  elbow->axis2.x = -sin(alpha)*sin(beta+delta); // sin(beta+delta) normalizes the norm of axis2
  elbow->axis2.y = cos(alpha)*sin(beta+delta);
  elbow->axis2.z = -cos(beta+delta);

  // sending the joint axes and anchor points to ODE
  //elbow->anchor.FromMatrix(RotElbow); // set elbow joint's anchor point
  // wrist->anchor.FromMatrix(Rot2Wrist); // not necessary since relative coordinates are used
  wrist->Init_Anchor();
  elbow->Init_Anchor();

  // looking at the values of the joints in ODE
  dJointID ejid = (dJointID)elbow->joint_id;
  //dJointSetHinge2Anchor(ejid,   elbow->anchor.x, elbow->anchor.y, elbow->anchor.z);
  float angl = (float)dJointGetHinge2Angle1(ejid);
  float ax1[] = {0.0f, 0.0f, 0.0f, 0.0f}, ax2[] = {0.0f, 0.0f, 0.0f, 0.0f};
  float anch[] = {0.0f, 0.0f, 0.0f, 0.0f};
  dJointGetHinge2Anchor(ejid, anch);
  dJointGetHinge2Axis1(ejid, ax1);
  dJointGetHinge2Axis2(ejid, ax2);
  float_Matrix max1(1,4), max2(1,4), manch(1,4);
  String smax1, smax2, smanch;
  max1.InitFromFloats(ax1); max2.InitFromFloats(ax2); manch.InitFromFloats(anch);
  max1.Print(smax1); max2.Print(smax2); manch.Print(smanch);
  taMisc::Info("elbow angle: ", String(angl), "\n");
  taMisc::Info("elbow anchor: ", smanch, "\n");
  taMisc::Info("elbow axis1: ", smax1, "\n");
  taMisc::Info("elbow axis2: ", smax2, "\n");
  //elbow->Init_Anchor();
  //CurFromODE(true);

  DataChanged(DCR_ITEM_UPDATED); // this will in theory update the display

  return true;
}

bool VEArm::bender(taVector3f &p3, taVector3f a, taVector3f c, taVector3f p1, taVector3f p2) {
  // This function is the C++ equivalent of piece5.m.
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector i.

  //---- declaring the variables to be used for bend test ----
  //taVector3f a(-10,-5,-12), c(-1,-1,-5), p1(-3,-4,-7), p2(-2,-4,2), p3(0,0,0);
  taVector3f b, r, v, s;
  float gam;

//---- preliminary calculations ----
  b = p2 - p1;  // b points from p1 to p2
  b.MagNorm();  // b is now a unit vector
  r = p1 - ((p1*b).Sum())*b;  // shortest line from origin to bending line
  //taMisc::Info("r = ", r.GetStr(), "\n");
//----------------------------------

// calculating side %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  v = p1 - a - (((p1-a)*b).Sum())*b;  // shortest vector from 'a' to bending line
  s.x = b.y*v.z - v.y*b.z;
  s.y = v.x*b.z - b.x*v.z;
  s.z = b.x*v.y - v.x*b.y;
  gam = ((c-r)*s).Sum();
  //taMisc::Info("v = ", v.GetStr(), ", s = ", s.GetStr(), "\n");
  //taMisc::Info("gam = ", String(gam), "\n");
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // get bending point depending on side ()()()()()()()()()()()
  //()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
  if(gam > 0) {  // if muscle should wrap around bending line
    taVector3f ar,cr,abs_b;
    float f1,f2,f3,f4,ap,bp,cp,k,k1,k2,n1,n2,ind;

    ar = a-r;  // I shift so b can be considered to intersect the origin
    cr = c-r;
    f1 = ar.SqMag();
    f2 = cr.SqMag();
    f3 = (ar*b).Sum();
    f4 = (cr*b).Sum();
    ap = f2 + (f3*f3) - f1 - (f4*f4);
    bp = 2*(f1*f4 - f2*f3 + f3*f4*(f4-f3));
    cp = f2*f3*f3 - f1*f4*f4;

    // selecting the root with the smallest sum of distances
    k1 = (-bp + sqrt(bp*bp - 4*ap*cp))/(2*ap);
    k2 = (-bp - sqrt(bp*bp - 4*ap*cp))/(2*ap);
    n1 = (ar-k1*b).Mag() + (cr-k1*b).Mag();
    n2 = (ar-k2*b).Mag() + (cr-k2*b).Mag();
    if(n1 < n2) {
      k = k1;
    }
    else {
      k = k2;
    }

    // limiting p3 between p1 and p2
    abs_b = b;  abs_b.Abs();
    // this is because b may have entries equal to zero
    if(abs_b.x > abs_b.y && abs_b.x > abs_b.z)
      ind = 0;
    else if(abs_b.y > abs_b.x && abs_b.y > abs_b.z)
      ind = 1;
    else
      ind = 2;

    if(ind == 0) {
      k1 = (p1.x - r.x)/b.x;
      k2 = (p2.x - r.x)/b.x; }
    else if(ind == 1) {
      k1 = (p1.y - r.y)/b.y;
      k2 = (p2.y - r.y)/b.y; }
    else if(ind == 2) {
      k1 = (p1.z - r.z)/b.z;
      k2 = (p2.z - r.z)/b.z; }

    k = MIN(k,MAX(k1,k2));
    k = MAX(k,MIN(k1,k2)); // k is now restricted between p1 and p2
    p3 = r + k*b;           // the point of intersection between muscle and bending line

    //taMisc::Info("p3 = ", p3.GetStr());
    return true;
  }
  //taMisc::Info("p3 = ", p3.GetStr());
  return false;
  //()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()
}

bool VEArm::TargetLengths(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z) {

  if(TestWarning(trgLen.count() != Nmusc, "","The matrix provided to TargetLengths doesn't match the number of muscles \n"))
                return false;

  if(!CheckArm()) return false;

  if(trg_x == 0.0 && trg_y == 0.0) {
    trg_y = 0.000001f;  // this is to avoid a Gimble lock
  }

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z};
  float_Matrix T(1,3);
  T.InitFromFloats(t_f);
  float D = taMath_float::vec_norm(&T);

  if(D < 0.1) {
    taMisc::Info("Target too close \n");
    // moving the target away, maintaining direction
    float Lfactor = 0.15/D;
    trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
    T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
    D = taMath_float::vec_norm(&T);
  } else if( D >= (La+Lf) ) {
      taMisc::Info("Target too far \n");
    // bringing the target closer, maintaining direction
      float Lfactor = (La+Lf-0.01)/D;
      trg_x *= Lfactor; trg_y *= Lfactor; trg_z *= Lfactor;
      T.SetFmVar(trg_x,0); T.SetFmVar(trg_y,1); T.SetFmVar(trg_z,2);
      D = taMath_float::vec_norm(&T);
  }

  // From coordinates to angles as in (44)

  float delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
  float gamma = 0;
  float beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
  float alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

  if(trg_y < 0) {  // if the target is behind
    alpha = taMath_float::pi - alpha;
  }

  //taMisc::Info("alpha:", String(alpha), "beta:", String(beta), "gamma:", String(gamma));

  // Now we'll rotate the insertion points by the Euler angles in reverse order
  // This magic R matrix (from (42)) does it all in one step
  float sa = sin(alpha); float ca = cos(alpha);
  float sb = sin(beta);  float cb = cos(beta);
  float sc = sin(gamma); float cc = cos(gamma);

  float R_f[] = {ca*cc-sa*cb*sc, -ca*sc-sa*cb*cc, sa*sb,
                 sa*cc+ca*cb*sc, -sa*sc+ca*cb*cc, -ca*sb,
                 sb*sc,           sb*cc,          cb};
  float_Matrix R(2,3,3);
  R.InitFromFloats(R_f);
  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  String out;
  RotArmIP.Print(out);
  taMisc::Info("rotated ArmIP:\n", out);

// rotating the ulna's insertion points
  //float UlnaCM_f[] = {0,0,-(ulna->length/2 + elbow_gap/2)};  // Ulna CM with origin at elbow
  float UlnaShift_f[] = {0, 0, -La,
                         0, 0, -La,
                         0, 0, -La}; // should have one row per forearm IP
  float T_elbowRot_f[] = {1 , 0, 0,
                0, cos(delta),  sin(delta),
                0, -sin(delta), cos(delta)};
  // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
  float_Matrix UlnaShift(2,3,3);
  UlnaShift.InitFromFloats(UlnaShift_f);
  float_Matrix T_elbowRot(2,3,3);
  T_elbowRot.InitFromFloats(T_elbowRot_f);

// first we shift the FarmIP's so the origin is at the elbow
  float_Matrix ShiftedIP(2,3,3);
  ShiftedIP = FarmIP - UlnaShift;

// we rotate the shifted IPs by the elbow bend (delta rotation)
  float_Matrix Rot1FarmIP(2,3,3);
  taMath_float::mat_mult(&Rot1FarmIP, &ShiftedIP, &T_elbowRot);
/*
  String ruout;
  Rot1FarmIP.Print(ruout);
  taMisc::Info("rotated ulna before translation:\n", ruout);
*/

// now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

// finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

  String ripout;
  RotFarmIP.Print(ripout);
  taMisc::Info("rotated ulna IPs:\n", ripout);

// next we obtain the distance between the rotated IPs and the original IPs
// this code is highly dependent on the muscle geometry
  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8; i++) { // the 8 shoulder to humerus muscles
    shoulderIP.x =  ShouldIP.FastElAsFloat(0,i);
    shoulderIP.y =  ShouldIP.FastElAsFloat(1,i);
    shoulderIP.z =  ShouldIP.FastElAsFloat(2,i);
    humerIP.x = RotArmIP.FastElAsFloat(0,i);
    humerIP.y = RotArmIP.FastElAsFloat(1,i);
    humerIP.z = RotArmIP.FastElAsFloat(2,i);
    pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
    pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
    pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

    if(bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
      // if muscle wraps around bending line (except for muscles 3 and 4)
      c1 = shoulderIP - p3; c2 = p3 - humerIP;
      trgLen.Set(c1.Mag()+c2.Mag(),i);
    } else {
      c1 = shoulderIP - humerIP;
      trgLen.Set(c1.Mag(),i);
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  c1.x = ShouldIP.FastElAsFloat(0,8) - RotFarmIP.FastElAsFloat(0,0);
  c1.y = ShouldIP.FastElAsFloat(1,8) - RotFarmIP.FastElAsFloat(1,0);
  c1.z = ShouldIP.FastElAsFloat(2,8) - RotFarmIP.FastElAsFloat(2,0);
  trgLen.Set(c1.Mag(),8);
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    c1.x = RotArmIP.FastElAsFloat(0,7+i) - RotFarmIP.FastElAsFloat(0,i);
    c1.y = RotArmIP.FastElAsFloat(1,7+i) - RotFarmIP.FastElAsFloat(1,i);
    c1.z = RotArmIP.FastElAsFloat(2,7+i) - RotFarmIP.FastElAsFloat(2,i);
    trgLen.Set(c1.Mag(),8+i);
  }

  String trLout;
  trgLen.Print(trLout);
  taMisc::Info("target lengths: ", trLout, "\n");
  return true;
}

bool VEArm::Lengths(float_Matrix &Len) {
  if(TestWarning(Len.count() != Nmusc, "","The matrix provided to Lengts() doesn't match the number of muscles \n"))
    return false;

  for(int i=0; i<Nmusc; i++) {
    Len.Set(muscles[i]->Length(),i);
  }
  return true;
}

bool VEArm::Speeds(float_Matrix &Vel) {
  if(TestWarning(Vel.count() != Nmusc, "","The matrix provided to Speeds() doesn't match the number of muscles \n"))
    return false;

  for(int i=0; i<Nmusc; i++) {
    Vel.Set(muscles[i]->Speed(),i);
  }
  return true;
}

bool VEArm::ApplyStim(float_Matrix stims, float_Matrix &forces) {
  if(TestWarning(stims.count() != Nmusc, "","The stimulus matrix doesn't match the number of muscles \n"))
    return false;
  else if(TestWarning(forces.count() != 3*Nmusc, "","The forces matrix doesn't match the number of muscles \n"))
    return false;

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];

  taVector3f daforce(0.0f, 0.0f, 0.0f);
  for(int i=0; i<Nmusc; i++) {
    daforce = muscles[i]->Contract(stims.FastElAsFloat(i));
    forces.Set(daforce.x, i, 0);
    forces.Set(daforce.y, i, 1);
    forces.Set(daforce.z, i, 2);

    if(i < 8) {  // muscles from shoulder to humerus
      humerus->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    } else if(i == 8) {  // biceps
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    } else { // triceps and brachialis
      humerus->AddForceAtPos(-daforce.x,-daforce.y,-daforce.z,muscles[i]->IPprox.x,muscles[i]->IPprox.y,muscles[i]->IPprox.z,false,false);
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z,muscles[i]->IPdist.x,muscles[i]->IPdist.y,muscles[i]->IPdist.z,false,false);
    }

  }
  return true;
}


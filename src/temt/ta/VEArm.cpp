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
#include <VEWorld>
#include <taMath_float>
#include <DataTable>
#include <DataCol>
#include <Random>

#include <VELinearMuscle>
#include <VEHillMuscle>


#include <taMisc>


void VEArm::Initialize() {
  // just the default initial values here -- note that VEObject parent initializes all the space stuff in its Initialize, so you don't need to do that here
  arm_side = RIGHT_ARM;
  up_axis = Y;
  musc_geo = NEW_GEO;
  musc_type = LINEAR;
  // note: torso ref is self initializing
  La = 0.3f;
  Lf = 0.33f;
  elbow_gap = 0.03f; // space left between bodies so joint can rotate
  wrist_gap = 0.03f;
  
  gain = 20.0f;
  damping = 0.1f;
  damping_thr = 0.05f;
  vel_norm_gain = 15.0f;

  float CT_f[] = {-1.0f, 0,    0,
                      0, 0,    1.0f,
                      0, 1.0f, 0};
  ct.SetGeom(2,3,3);
  ct.InitFromFloats(CT_f);

  should_loc.x = 0; should_loc.y = 0; should_loc.z = 0;

  // default maximum, minimum, and resting lengths
  float maxs[] = {.175f, .175f, .2f, .105f, .105f, .225f, .21f, .27f, .235f, .385f, .29f, .23f};
  float mins[] = {.08f, .08f, .08f, .058f, .058f, .15f, .135f, .13f, .15f, .27f, .22f, .12f};
  float rests[] = {0.155f, 0.144f, 0.115f, 0.093f, 0.067f, 0.18f, 
                   0.159f, 0.172f, 0.172f, 0.353f, 0.237f, 0.184f};
  max_lens.SetGeom(1,12);
  min_lens.SetGeom(1,12);
  rest_lens.SetGeom(1,12);
  max_lens.InitFromFloats(maxs);
  min_lens.InitFromFloats(mins);
  rest_lens.InitFromFloats(rests);

  // DO NOT put anythying other than direct literal member inits in here -- no creating objects etc
}

bool VEArm::UpdateIPs() {

// We update the muscles' past lengths before setting new IPs
  for(int i=0; i<n_musc; i++) {
    muscles[i]->UpOld();
  }

// To set new IPs, first we'll find the coordinates of the rotated IPs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEJoint* elbow = joints[ELBOW];

  float_Matrix R(2,3,3);
  humerus->cur_quat.ToRotMatrix(R);

  if(up_axis == Y) {  // changing coordinates for R as a linear transformation
                      // taking advantage of ct being autoinverse

    // Here's the deal: If the humerus has a long axis LONG_Y then the inital rotation is
    // set to be pi/2 around -X by VEBody::Init_Rotation(). This causes R to be this rotation
    // when the arm is in its initial position. To counter this I apply a rotation of pi/2 
    // around the X axis. 
    float UR_f[] = { 1.0f,  0,     0,
                        0,  0,    -1.0f,   // 0, 0, -1.0f,
                        0,  1.0f,  0};
    float_Matrix UR(2,3,3);
    UR.InitFromFloats(UR_f);
    float_Matrix cR = float_Matrix(2,3,3);
    cR = R;
    taMath_float::mat_mult(&R, &cR, &UR);
  }
/*
  String outR;
  R.Print(outR);
  taMisc::Info("R matrix in UpdateIPs:\n", outR);
*/
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

  float UlnaShift_f[9] = {0.0f}; // initializes all zeros
  float T_elbowRot_f[9] = {0.0f};

  if(up_axis == Z) {
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, 0, -La,
                           0, 0, -La,
                           0, 0, -La}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta),  sin(delta),
                    0, -sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
    */
    UlnaShift_f[2] = UlnaShift_f[5] = UlnaShift_f[8] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = sin(delta);
    T_elbowRot_f[7] = -sin(delta);
  }
  else { // up_axis == Y 
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, -La, 0,
                           0, -La, 0,
                           0, -La, 0}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta), -sin(delta),
                    0, sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation after the ct bilateral multiplication
    */
    UlnaShift_f[1] = UlnaShift_f[4] = UlnaShift_f[7] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = -sin(delta);
    T_elbowRot_f[7] = sin(delta);
  }

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

  if(musc_geo == OLD_GEO) {
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

      if(Bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
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
  } else {  // musc_geo==NEW_GEO
    for(int i=0; i<9; i++) {  // the 9 shoulder to humerus muscles
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

      if(Bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=2 && i!=3) {
        // if muscle wraps around bending line (except for muscles 3 and 4)
        muscles[i]->p3 = p3;
        muscles[i]->bend = true;
      } else {
        muscles[i]->bend = false;
      }
    }
    // next muscle is the biceps, from shoulder to forearm
    muscles[9]->IPprox.x = ShouldIP.FastElAsFloat(0,9);
    muscles[9]->IPprox.y = ShouldIP.FastElAsFloat(1,9);
    muscles[9]->IPprox.z = ShouldIP.FastElAsFloat(2,9);
    muscles[9]->IPdist.x = RotFarmIP.FastElAsFloat(0,0);
    muscles[9]->IPdist.y = RotFarmIP.FastElAsFloat(1,0);
    muscles[9]->IPdist.z = RotFarmIP.FastElAsFloat(2,0);
    muscles[9]->bend = false;
    // the triceps and the brachialis connect from humerus to ulna
    for(int i=1; i<=2; i++) {
      muscles[9+i]->IPprox.x = RotArmIP.FastElAsFloat(0,8+i);
      muscles[9+i]->IPprox.y = RotArmIP.FastElAsFloat(1,8+i);
      muscles[9+i]->IPprox.z = RotArmIP.FastElAsFloat(2,8+i);
      muscles[9+i]->IPdist.x = RotFarmIP.FastElAsFloat(0,i);
      muscles[9+i]->IPdist.y = RotFarmIP.FastElAsFloat(1,i);
      muscles[9+i]->IPdist.z = RotFarmIP.FastElAsFloat(2,i);
      muscles[9+i]->bend = false;
    }
  }
  //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

  //......... updating the normalized lengths ..........
  Lengths(norm_lens, true); // true = normalize

  Speeds(norm_vels, true); // true = normalize
  avg_vel_mag = 0.0f;
  for(int i=0; i<norm_vels.size; i++) {
    avg_vel_mag += fabsf(norm_vels.FastEl(i) - 0.5f); // offset from .5
  }
  avg_vel_mag /= (float)norm_vels.size;
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
  if(n_musc != (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6)) {
    rval = false;
    TestError(!quiet, "CheckArm", "number of muscles doesn't match number of insertion points -- run ConfigArm");
  }
  // probably that's sufficient for a quick-and-dirty sanity check
  return rval;
}

void VEArm::InitMuscles() {
  // Initializing the insertion point matrices, assuming RIGHT_ARM, up_axis=Z.
  // These coordinates always assume the shoulder at the origin.

//-------- Definining the default matrices (RIGHT_ARM, up_axis=Z) ---------
  if(musc_geo == OLD_GEO) {
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
    float p1_f[] = {  0.06f, -0.02f,  0.02f,    0.01f,  0.03f, -0.02f, // changed first from 0.03f to 0.06f
                      0,      0.02f,  0.01f,    0,     -0.02f,  0.01f,
                     -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f,
                     -0.02f,  0.03f,  0.02f,   -0.01f, -0.05f, -0.07f };
    p1.SetGeom(2,3,8);
    p1.InitFromFloats(p1_f);

    float p2_f[] = {  0.06f,  0.02f,  0.02f,    0.01f, -0.03f, -0.02f, // changed first from 0.03f to 0.06f
                      0,      0.02f, -0.01f,    0,     -0.02f, -0.01f,
                     -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f,
                     -0.03f,  0.03f, -0.06f,    0,     -0.04f,  0.02f };
    p2.SetGeom(2,3,8);
    p2.InitFromFloats(p2_f);
  }
  else {  // musc_geo == NEW_GEO
    // Here are all the muscle insertion points in the trunk and shoulder, as
    // labeled in (60) starting with point e
    float ShouldIP_f[] = { -0.05f, -0.01f,  0,      -0.05f,  0.01f,  0,      // e,g     
                           -0.11f,  0,     -0.07f,  -0.08f,  0.01f,  0,      // i,k  
                           -0.08f, -0.01f,  0,      -0.14f,  0.06f,  0,      // m,o 
                           -0.10f, -0.05f,  0,      -0.14f,  0.06f, -0.12f,  // q,s
                           -0.10f, -0.05f, -0.12f,  -0.02f,  0.02f,  0.01f}; // u,v  
    ShouldIP.SetGeom(2,3,10);     // the matrix has two indices, 3 columns and 10 rows
    ShouldIP.InitFromFloats(ShouldIP_f);

    // Here are all the muscle insertion points in the arm, as labeled in (60),
    // starting with point d
    float ArmIP_f[] = {  0.015f, -0.01f,  -0.05f,   0.015f,  0.01f,  -0.05f,   // d,f
                        -0.02f,   0,      -0.05f,   0,       0.02f,   0,       // h,j
                         0,      -0.02f,   0,       0.01f,   0.01f,  -0.06f,   // l,n
                         0.01f,  -0.01f,  -0.06f,  -0.01f,   0.01f,  -0.06f,   // p,r
                        -0.01f,  -0.01f,  -0.06f,   0,      -0.015f, -0.06f,   // t,x
                         0,       0.015f, -0.15f};                             // z
    ArmIP.SetGeom(2,3,11);
    ArmIP.InitFromFloats(ArmIP_f);

    //String aout;
    //ArmIP.Print(aout);
    //taMisc::Info("init ArmIP:\n", aout);

    // Here are the muscle insertion points in the forearm, corresponding to the
    // biceps, the triceps, and the brachialis.
    float FarmIP_f[] = {  0,      0.015f, -La - 0.05f,    // w
                          0,     -0.005f, -La + 0.03f,    // y
                         -0.01f,  0.015f, -La - 0.04f };  // zz
    FarmIP.SetGeom(2,3,3);
    FarmIP.InitFromFloats(FarmIP_f);

    // Here are the initial and final points of the restricted bending lines for
    // each one of the muscles 1-9. The values for muscles 4,5 are not used
    float p1_f[] = {  0.03f, -0.04f,  0.01f,    0.03f,  0,      0.03f,  // for muscles 1,2 
                      0.01f,  0.03f, -0.02f,    0,      0.02f,  0.01f,  // for muscles 3,4 
                      0,     -0.02f,  0.01f,   -0.02f,  0.03f,  0.02f,  // for muscles 5,6 
                     -0.01f, -0.05f, -0.07f,   -0.02f,  0.03f,  0.02f,  // for muscles 7,8
                     -0.01f, -0.05f, -0.07f,  };                        // for muscle  9 
    p1.SetGeom(2,3,9);
    p1.InitFromFloats(p1_f);

    float p2_f[] = {  0.03f,  0,      0.03f,    0.03f,  0.04f,  0.01f,  // for muscles 1,2 
                      0.01f, -0.03f, -0.02f,    0,      0.02f, -0.01f,  // for muscles 3,4 
                      0,     -0.02f, -0.01f,   -0.03f,  0.03f, -0.06f,  // for muscles 5,6 
                      0,     -0.04f,  0.02f,   -0.03f,  0.03f, -0.06f,  // for muscles 7,8
                      0,     -0.04f,  0.02f,  };                        // for muscle  9 
    p2.SetGeom(2,3,9);
    p2.InitFromFloats(p2_f);
  }

  //------- Transforming IP matrices according to up_axis and arm_side -----
  int ex = 0;  // used to change the size of the auxiliary matrices when musc_geo=NEW_GEO
  if(musc_geo == NEW_GEO)
    ex = 1;

  // if we have left arm, up_axis=Y...
  if(up_axis == Y && arm_side == LEFT_ARM) {  // in this case we must change the IPs' coordinate system
    float CT2_f[] = {1.0f, 0,    0,       // like ct, but without inverting x (not a proper rotation)
                        0, 0,    1.0f,    // this matrix is used for a left arm with Y upwards
                        0, 1.0f, 0};
    float_Matrix ct2(2,3,3);
    ct2.InitFromFloats(CT2_f);

    float_Matrix SIP = float_Matrix(2,3,9+ex); SIP = ShouldIP;
    float_Matrix AIP = float_Matrix(2,3,10+ex); AIP = ArmIP;
    float_Matrix FIP = float_Matrix(2,3,3); FIP = FarmIP;
    float_Matrix pp1 = float_Matrix(2,3,8+ex); pp1 = p1;
    float_Matrix pp2 = float_Matrix(2,3,8+ex); pp2 = p2;
    taMath_float::mat_mult(&ShouldIP, &SIP, &ct2); // ct postmultiplies because there is one point per row in SIP
    taMath_float::mat_mult(&ArmIP, &AIP, &ct2);
    taMath_float::mat_mult(&FarmIP, &FIP, &ct2);
    taMath_float::mat_mult(&p1, &pp1, &ct2);
    taMath_float::mat_mult(&p2, &pp2, &ct2);
  }
  else if(up_axis == Y && arm_side == RIGHT_ARM) {
    float CT2_f[] = {-1.0f,    0,       0,     // In the case of a right arm there is an additional reflexion 
                         0,    0,    1.0f,    // wrt the YZ plane
                         0, 1.0f,       0};
    float_Matrix ct2(2,3,3);
    ct2.InitFromFloats(CT2_f);

    float_Matrix SIP = float_Matrix(2,3,9+ex); SIP = ShouldIP;
    float_Matrix AIP = float_Matrix(2,3,10+ex); AIP = ArmIP;
    float_Matrix FIP = float_Matrix(2,3,3); FIP = FarmIP;
    float_Matrix pp1 = float_Matrix(2,3,8+ex); pp1 = p1;
    float_Matrix pp2 = float_Matrix(2,3,8+ex); pp2 = p2;
    taMath_float::mat_mult(&ShouldIP, &SIP, &ct2); // ct postmultiplies because there is one point per row in SIP
    taMath_float::mat_mult(&ArmIP, &AIP, &ct2);
    taMath_float::mat_mult(&FarmIP, &FIP, &ct2);
    taMath_float::mat_mult(&p1, &pp1, &ct2);
    taMath_float::mat_mult(&p2, &pp2, &ct2);
  }
  else if(up_axis == Z && arm_side == LEFT_ARM) {
    float CT2_f[] = {-1.0f,    0,       0,     // In the case of a left arm with up_axis=Z we only need to reflect
                         0, 1.0f,       0,    // wrt the YZ plane
                         0,    0,    1.0f};
    float_Matrix ct2(2,3,3);
    ct2.InitFromFloats(CT2_f);

    float_Matrix SIP = float_Matrix(2,3,9+ex); SIP = ShouldIP;
    float_Matrix AIP = float_Matrix(2,3,10+ex); AIP = ArmIP;
    float_Matrix FIP = float_Matrix(2,3,3); FIP = FarmIP;
    float_Matrix pp1 = float_Matrix(2,3,8+ex); pp1 = p1;
    float_Matrix pp2 = float_Matrix(2,3,8+ex); pp2 = p2;
    taMath_float::mat_mult(&ShouldIP, &SIP, &ct2); // ct postmultiplies because there is one point per row in SIP
    taMath_float::mat_mult(&ArmIP, &AIP, &ct2);
    taMath_float::mat_mult(&FarmIP, &FIP, &ct2);
    taMath_float::mat_mult(&p1, &pp1, &ct2);
    taMath_float::mat_mult(&p2, &pp2, &ct2);
  }

  //------------- Initializing the muscle objects ---------------
  // the number of muscles is 1/2 the sum of points in ShoulderIP+ArmIP+FarmIP
  n_musc = (int)((FarmIP.count() + ArmIP.count() + ShouldIP.count())/6);

  muscles.Reset();

  if(musc_type == LINEAR) {
    muscles.el_typ = &TA_VELinearMuscle; 
    muscles.SetSize(n_musc); // creating VEMuscle pointers
  }
  else  // musc_type == HILL
  {
    muscles.el_typ = &TA_VEHillMuscle; 
    muscles.SetSize(n_musc); // creating VEMuscle pointers
  }

  //----------- Initializing maximum, minimum, and resting lengths --------------
  float_Matrix tlens; 
  tlens.SetGeom(1,n_musc);
  max_lens.SetGeom(1,n_musc);
  min_lens.SetGeom(1,n_musc);
  rest_lens.SetGeom(1,n_musc);
  float pi  = taMath_float::pi;

  if(musc_geo == OLD_GEO) {
    // these are the angles at which muscles attain their maximum length for right arms
    float alphaM[] = {pi/2, pi/4, pi/300, pi/300, -.7*pi, .3*pi, -.7*pi, .3*pi, pi, pi/300, pi/300};
    float betaM[] = {.03*pi, .8*pi, pi/300, pi/300, .6*pi, .6*pi, .6*pi, .6*pi, pi/30, pi/300,pi/300}; 
    float gammaM[] = {pi/2, 0, -1.3f, 1.3f, -1.4f, .5f, -1.0f, -pi/2, 1.0f, 0, 0};
    float deltaM[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9*pi, 0};

    // these are the angles at which muscles attain their minimum length for right arms
    float alpham[] = {pi/2, pi/2, pi/300, pi/300, pi/5, -.7*pi, pi/5, -.7*pi, pi/4/ pi/300, pi/300};
    float betam[] = {.7*pi, .03*pi, pi/300, pi/300, .7*pi, .6*pi, .3*pi, .3*pi, pi/30, pi/300, pi/300};
    float gammam[] = {pi/2, -pi/2, 1.2f, -1.2f, pi/2, 1.0f, 1.0f, 1.4f, 0.2f, 0, 0};
    float deltam[] = {0, 0, 0, 0, 0, 0, 0, 0, 0.8*pi, 0, 0.8*pi};

    if(arm_side == LEFT_ARM) {  // switching arm side reverses alpha and gamma
      for(int i=0; i<n_musc; i++) {
        alphaM[i] = -alphaM[i];
        alpham[i] = -alpham[i];
        gammaM[i] = -gammaM[i];
        gammam[i] = -gammam[i];
      }
    }

    for(int i=0; i<n_musc; i++) {
      AngToLengths(tlens,alphaM[i],betaM[i],gammaM[i],deltaM[i]);
      max_lens.Set(tlens.FastEl(i)+0.003f,i);

      AngToLengths(tlens,alpham[i],betam[i],gammam[i],deltam[i]);
      min_lens.Set(tlens.FastEl(i)-0.004f,i);
    }
  } else { // musc_geo == NEW_GEO
    // these are the angles at which muscles attain their maximum length for right arms
    float alphaM[] = {pi/7, pi-pi/7, -pi/2, pi/300, pi/300, -.7*pi, .3*pi, -.7*pi, .3*pi, pi, pi/300, pi/300};
    float betaM[] = {pi/20, pi/20, .8*pi, pi/300, pi/300, .6*pi, .6*pi, .6*pi, .6*pi, pi/30, pi/300,pi/300}; 
    float gammaM[] = {1.4f, 1.0f, 1.4f, -1.45f, 1.45f, -.5f, .5f, -.5f, -.5f, 1.0f, 0, 0};
    float deltaM[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9*pi, 0};

    // these are the angles at which muscles attain their minimum length for right arms
    float alpham[] = {-pi/2, -pi/2, pi/2, pi/300, pi/300, pi/5, -.7*pi, pi/5, -.7*pi, pi/4, pi/300, pi/300};
    float betam[] = {.8*pi, .8*pi, pi/20, pi/300, pi/300, .7*pi, .6*pi, .3*pi, .3*pi, pi/30, pi/300, pi/300};
    float gammam[] = {pi/2, pi/2, -pi/2, 1.45f, -1.45f, pi/2, -.5f, 0, 1.4f, 0.2f, 0, 0};
    float deltam[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8*pi, 0, 0.8*pi};

    if(arm_side == LEFT_ARM) {   // switching arm side reverses alpha and gamma
      for(int i=0; i<n_musc; i++) {
        alphaM[i] = -alphaM[i];
        alpham[i] = -alpham[i];
        gammaM[i] = -gammaM[i];
        gammam[i] = -gammam[i];
      }
    }

    for(int i=0; i<n_musc; i++) {
      AngToLengths(tlens,alphaM[i],betaM[i],gammaM[i],deltaM[i]);
      max_lens.Set(tlens.FastEl(i)+0.003f,i);

      AngToLengths(tlens,alpham[i],betam[i],gammam[i],deltam[i]);
      min_lens.Set(tlens.FastEl(i)-0.003f,i);
    }
  }

  if(arm_side == RIGHT_ARM) {
    AngToLengths(rest_lens, -pi/4, pi/8, 0, pi/4);  // setting resting lengths
  } else { // arm_side == LEFT_ARM
    AngToLengths(rest_lens, pi/4, pi/8, 0, pi/4);  // setting resting lengths
  }

  // Initializing the spans vector
  spans.SetGeom(1,n_musc);
  for(int i=0; i<n_musc; i++) 
    spans.Set(1/(max_lens.FastEl(i)-min_lens.FastEl(i)),i);

}


bool VEArm::ConfigArm(const String& name_prefix,
                      float humerus_length, float humerus_radius,
                      float ulna_length, float ulna_radius,
                      float hand_length, float hand_radius, 
                      float elbowGap, float wristGap,
                      float sh_offX, float sh_offY, float sh_offZ) {
                      
  // tor_cm are the coordinates of the torso's center of mass
  // sh_off are the coordinates of the shoulder's anchor point wrt default position

  if(TestError(!torso, "ConfigArm", "torso not set -- must specify a body in another object to serve as the torso before running ConfigArm"))
    return false;

  float tor_cmX = torso->init_pos.x;
  float tor_cmY = torso->init_pos.y;
  float tor_cmZ = torso->init_pos.z;

  // Assuming that torso is a box!
  // changing sh_off variables so they are wrt torso's CM
  if(arm_side == RIGHT_ARM) 
  {
    if(up_axis == Y) 
    {
      sh_offY = sh_offY + (torso->box.y)/2.0f;
      sh_offX = sh_offX - (torso->box.x)/2.0f - 0.07f;
    } 
    else 
    { // up_axis == Z
      sh_offZ = sh_offZ + (torso->box.z)/2.0f;
      sh_offX = sh_offX + (torso->box.x)/2.0f + 0.07f;
    }
  } 
  else 
  {  // arm_side == LEFT
    if(up_axis == Y) 
    {
      sh_offY = sh_offY + (torso->box.y)/2.0f;
      sh_offX = sh_offX + (torso->box.x)/2.0f + 0.07f;
    } 
    else 
    { // up_axis == Z
      sh_offZ = sh_offZ + (torso->box.z)/2.0f;
      sh_offX = sh_offX - (torso->box.x)/2.0f - 0.07f;
    }
  }

  float CT_f[] = {-1.0f, 0,    0,
                      0, 0,    1.0f,
                      0, 1.0f, 0};
  ct.SetGeom(2,3,3);
  ct.InitFromFloats(CT_f);

  if(bodies.size < N_ARM_BODIES)
    bodies.SetSize(N_ARM_BODIES); // auto-creates the VEBody objects up to specified size
  if(joints.size < N_ARM_JOINTS)
    joints.SetSize(N_ARM_JOINTS); // auto-creates the VEJoint objects up to specified size

  // Getting the stepsize of VEWorld containing this Arm
  VEWorld* Worldly = GetWorld();
  world_step = Worldly->stepsize;

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

  float shX, shY, shZ;  // shoulder coordinates wrt the World coordinate system
  shX = tor_cmX + sh_offX; // The sh variables are here because the name is shorter than should_loc
  shY = tor_cmY + sh_offY;
  shZ = tor_cmZ + sh_offZ;

  should_loc.x = shX;
  should_loc.y = shY;
  should_loc.z = shZ;

  elbow_gap = elbowGap;
  wrist_gap = wristGap;

  // todo: need to make all this conditional on arm_side!
  if(up_axis == Z) {
    humerus->name = name_prefix + "Humerus";
    humerus->shape = VEBody::CAPSULE; humerus->long_axis = VEBody::LONG_Z;
    humerus->length = humerus_length; humerus->radius = humerus_radius;
    humerus->init_pos.x = shX; humerus->init_pos.y = shY; humerus->init_pos.z = shZ - humerus_length/2;
    humerus->init_rot.x = 0; humerus->init_rot.y = 0; humerus->init_rot.z = 0;
    humerus->cur_pos.x = shX; humerus->cur_pos.y = shY; humerus->cur_pos.z = humerus->init_pos.z;
    humerus->cur_rot.x = 0; humerus->cur_rot.y = 0; humerus->cur_rot.z = 0;

    ulna->name = name_prefix + "Ulna";
    ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Z;
    ulna->length = ulna_length; ulna->radius = ulna_radius;
    ulna->init_pos.x = shX; ulna->init_pos.y = shY; ulna->init_pos.z = shZ-humerus_length-(ulna_length/2)-elbow_gap;
    ulna->init_rot.x = 0; ulna->init_rot.y = 0; ulna->init_rot.z = 0;
    ulna->cur_pos.x = shX; ulna->cur_pos.y = shY; ulna->cur_pos.z = ulna->init_pos.z;
    ulna->cur_rot.x = 0; ulna->cur_rot.y = 0; ulna->cur_rot.z = 0;

    hand->name = name_prefix + "Hand";
    hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Z;
    hand->length = hand_length; hand->radius = hand_radius;
    hand->init_pos.x = shX; hand->init_pos.y = shY;
    hand->init_pos.z = shZ-(humerus_length+ulna_length+elbow_gap+wrist_gap+(hand_length/2));
    hand->init_rot.x = 0; hand->init_rot.y = 0; hand->init_rot.z = 0;
    hand->cur_pos.x = shX; hand->cur_pos.y = shY; hand->cur_pos.z = hand->init_pos.z;
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

    // the shoulder anchor is wrt to torso's CM
    shoulder->anchor.x = sh_offX; 
    shoulder->anchor.y = sh_offY; 
    shoulder->anchor.z = sh_offZ;

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
  }

  if(up_axis == Y) {
    humerus->name = name_prefix + "Humerus";
    humerus->shape = VEBody::CAPSULE; humerus->long_axis = VEBody::LONG_Y;
    humerus->length = humerus_length; humerus->radius = humerus_radius;
    humerus->init_pos.x = shX; humerus->init_pos.z = shZ; humerus->init_pos.y = shY-humerus_length/2;
    humerus->init_rot.x = 0; humerus->init_rot.z = 0; humerus->init_rot.y = 0;
    humerus->cur_pos.x = shX; humerus->cur_pos.z = shZ; humerus->cur_pos.y = humerus->init_pos.y;
    humerus->cur_rot.x = 0; humerus->cur_rot.z = 0; humerus->cur_rot.y = 0;

    ulna->name = name_prefix + "Ulna";
    ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Y;
    ulna->length = ulna_length; ulna->radius = ulna_radius;
    ulna->init_pos.x = shX; ulna->init_pos.z = shZ; ulna->init_pos.y = shY-humerus_length-(ulna_length/2)-elbow_gap;
    ulna->init_rot.x = 0; ulna->init_rot.z = 0; ulna->init_rot.y = 0;
    ulna->cur_pos.x = shX; ulna->cur_pos.z = shZ; ulna->cur_pos.y = ulna->init_pos.y;
    ulna->cur_rot.x = 0; ulna->cur_rot.z = 0; ulna->cur_rot.y = 0;

    hand->name = name_prefix + "Hand";
    hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Y;
    hand->length = hand_length; hand->radius = hand_radius;
    hand->init_pos.x = shX; hand->init_pos.z = shZ;
    hand->init_pos.y = shY - (humerus_length+ulna_length+elbow_gap+wrist_gap+(hand_length/2));
    hand->init_rot.x = 0; hand->init_rot.z = 0; hand->init_rot.y = 0;
    hand->cur_pos.x = shX; hand->cur_pos.z = shZ; hand->cur_pos.y = hand->init_pos.y;
    hand->cur_rot.x = 0; hand->cur_rot.z = 0; hand->cur_rot.y = 0;

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
    shoulder->anchor.x = sh_offX; shoulder->anchor.y = sh_offY; shoulder->anchor.z = sh_offZ;

    elbow->anchor.x = 0;  // set elbow joint's anchor point wrt humerus' CM and body coordinates
    //elbow->anchor.y = 0;  // the last coordinate is the long axis
    //elbow->anchor.z = -(humerus->length/2 +(elbow_gap/2));
    elbow->anchor.z = 0;  // the last coordinate is the long axis
    elbow->anchor.y = -(humerus->length/2 +(elbow_gap/2));

    wrist->anchor.x = 0; // set wrist joint's anchor point wrt ulna's CM and body coordinates
    wrist->anchor.z = 0; // the last coordinate is the long axis
    wrist->anchor.y = -(ulna->length/2 + (wrist_gap/2));

    elbow->axis.x = 1;  // setting elbow joint's axes
    elbow->axis.y = 0;
    elbow->axis.z = 0;
    elbow->axis2.x = 0;
    elbow->axis2.z = 0;
    elbow->axis2.y = -1;
  }

  //--------- Initializing muscles and insertion points -------------
  // the order of the next instructions is important

  InitMuscles();  // Initializes insertion point matrices and the VEMuscle_List
 
  norm_lens.SetGeom(1,n_musc); // setting the size of the normalized lengths float_Matrix
  norm_targ_lens.SetGeom(1,n_musc);
  norm_vels.SetGeom(1,n_musc);

  UpdateIPs(); // attaching muscles to their corresponding insertion points

  for(int i=0; i<n_musc; i++) {
    muscles[i]->InitBuffs(); //initializing the buffers which store past values of len and dlen
  }


  //----------- Setting damping --------------
  humerus->SetAngularDamping(damping);
  humerus->SetAngularDampingThreshold(damping_thr);
  ulna->SetAngularDamping(damping);
  ulna->SetAngularDampingThreshold(damping_thr);

  //---------- Init all created objects and update ODE -----------
  // NO: you can't count on ODE being initialized at this point -- that should happen later
  // Init();                       // this will attempt to init everything just created..
  // sending the values of the bodies to ODE
  // CurToODE();

  SigEmitUpdated(); // this will in theory update the display

  // displaying damping parameters for humerus
  // dBodyID hbid = (dBodyID)humerus->body_id;
  // float handamp = dBodyGetAngularDamping(hbid);
  // float hantre = dBodyGetAngularDampingThreshold(hbid);
  // taMisc::Info("humerus angular damping: ", String(handamp), "\n");
  // taMisc::Info("humerus angular damping threshold: ", String(hantre), "\n");

  return true;
}

bool VEArm::MoveToTarget(float trg_x, float trg_y, float trg_z) {
// at some point I should set the arm to its initial position here
  if(!CheckArm()) return false;

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];
  VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];

// first we change the target coordinates to have the origin at the shoulder
  trg_x = trg_x - should_loc.x;
  trg_y = trg_y - should_loc.y;
  trg_z = trg_z - should_loc.z;

  if(up_axis == Z) {
    if(trg_x == 0.0 && trg_y == 0.0) {
      trg_y = 0.000001f;  // this is to avoid a Gimble lock
    }
  } 
  else {  // up_axis == Y
    if(trg_x == 0.0 && trg_z == 0.0) {
      trg_z = 0.000001f;  // this is to avoid a Gimble lock
    }
  } 

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z}; // not necessary to check up_axis here;
                                       // T is only used to get D

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

  float alpha, beta, gamma, delta;

  if(up_axis == Z) {
    delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
    gamma = 0;
    beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
    alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

    if(trg_y < 0) { // if the target is behind
      alpha = taMath_float::pi - alpha;
    }
  }
  else { // up_axis == Y
    delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
    gamma = 0;
    beta = acos(-trg_y/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
    alpha = asin(trg_x/sqrt(trg_x*trg_x + trg_z*trg_z));

    if(trg_z < 0) { // if the target is behind
      alpha = taMath_float::pi - alpha;
    }
  }

  // taMisc::Info("alpha:", String(alpha), "beta:", String(beta), "gamma:", String(gamma));

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

  if(up_axis == Y) {  // changing coordinates for R as a linear transformation
                      // taking advantage of ct being autoinverse
    float_Matrix cR = float_Matrix(2,3,3);
    taMath_float::mat_mult(&cR, &R, &ct);
    taMath_float::mat_mult(&R, &ct, &cR);
  }


//------ Rotating humerus -------

  if(up_axis == Z) {
    humerus->RotateEuler(beta,gamma,alpha,false);
    float HumCM_f[] = {0.0f,0.0f,(-La+(elbow_gap/2))/2};  // humerus' geometrical center at rest
    //float Elbow_f[] = {0.0f,0.0f,-La};  // elbow coordinates at rest

    float_Matrix HumCM(2,1,3);
    HumCM.InitFromFloats(HumCM_f);
    //float_Matrix Elbow(2,1,3);
    //Elbow.InitFromFloats(Elbow_f);

    float_Matrix RotHumCM(2,1,3);
    taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center
    // float_Matrix RotElbow(2,1,3);
    // taMath_float::mat_mult(&RotElbow, &R, &Elbow);  // rotating elbow

    humerus->Translate(RotHumCM.FastEl(0),RotHumCM.FastEl(1),RotHumCM.FastEl(2)+(humerus->length/2),false);
  
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
    //float Wrist_f[] = {0,0,-(ulna->length + elbow_gap/2 + wrist_gap/2)};  // wrist coords with origin at elbow
    float elbow_rot_f[] = {1 , 0, 0,
                  0, cos(delta), -sin(delta),
                  0, sin(delta), cos(delta)};
    float_Matrix UlnaCM(2,1,3);
    UlnaCM.InitFromFloats(UlnaCM_f);
    //float_Matrix Wrist(2,1,3);
    //Wrist.InitFromFloats(Wrist_f);
    float_Matrix elbow_rot(2,3,3);
    elbow_rot.InitFromFloats(elbow_rot_f);

    String erout;
    elbow_rot.Print(erout);
    // taMisc::Info("elbow rotation matrix :\n", erout);

    float_Matrix Rot1UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);
    //float_Matrix Rot1Wrist(2,1,3);
    //taMath_float::mat_mult(&Rot1Wrist, &elbow_rot, &Wrist);

    String ruout;
    Rot1UlnaCM.Print(ruout);
    // taMisc::Info("rotated ulna before translation:\n", ruout);

    Rot1UlnaCM.Set(Rot1UlnaCM.FastEl(2)-La,2); // setting origin at shoulder
    //Rot1Wrist.Set(Rot1Wrist.FastEl(2)-La,2);

    String rudout;
    Rot1UlnaCM.Print(rudout);
    // taMisc::Info("rotated ulna after translation:\n", rudout);

    float_Matrix Rot2UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation
    //float_Matrix Rot2Wrist(2,1,3);
    //taMath_float::mat_mult(&Rot2Wrist, &R, &Rot1Wrist);

    ulna->RotateEuler(beta+delta,gamma,alpha,false);
    ulna->Translate(Rot2UlnaCM.FastEl(0),Rot2UlnaCM.FastEl(1),Rot2UlnaCM.FastEl(2)+humerus->length+(ulna->length/2)+elbow_gap,false);
    hand->RotateEuler(beta+delta,gamma,alpha,false);
    hand->Translate(trg_x,trg_y,trg_z+(humerus->length+ulna->length+elbow_gap+wrist_gap+(hand->length/2)),false);

    //------- sending the values of the bodies to ODE --------
    CurToODE();

    //------- calculating and updating the joint values --------
    // setting the axes for the elbow joint
    elbow->axis.x = cos(alpha);
    elbow->axis.y = sin(alpha);
    elbow->axis.z = 0.0f;
    elbow->axis2.x = -sin(alpha)*sin(beta+delta); // sin(beta+delta) normalizes the norm of axis2
    elbow->axis2.y = cos(alpha)*sin(beta+delta);
    elbow->axis2.z = -cos(beta+delta);
  } // up_axis == Z
  else { // up_axis == Y
    humerus->RotateEuler(beta,gamma,taMath_float::pi + alpha,false);

    float HumCM_f[] = {0.0f,(-La+(elbow_gap/2))/2,0.0f};  // humerus' geometrical center at rest
    //float Elbow_f[] = {0.0f,-La,0.0f};  // elbow coordinates at rest

    float_Matrix HumCM(2,1,3);
    HumCM.InitFromFloats(HumCM_f);

    float_Matrix RotHumCM(2,1,3);
    taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center

    humerus->Translate(RotHumCM.FastEl(0),RotHumCM.FastEl(1)+(humerus->length/2),RotHumCM.FastEl(2),false);
 
    float UlnaCM_f[] = {0,-(ulna->length/2 + elbow_gap/2),0};  // Ulna 'CM' with origin at elbow
    float elbow_rot_f[] = {1 , 0, 0,
                  0, cos(delta),  sin(delta),
                  0, -sin(delta), cos(delta)};
    float_Matrix UlnaCM(2,1,3);
    UlnaCM.InitFromFloats(UlnaCM_f);
    float_Matrix elbow_rot(2,3,3);
    elbow_rot.InitFromFloats(elbow_rot_f);

    String erout;
    elbow_rot.Print(erout);
    // taMisc::Info("elbow rotation matrix :\n", erout);

    float_Matrix Rot1UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);

    String ruout;
    Rot1UlnaCM.Print(ruout);
    // taMisc::Info("rotated ulna before translation:\n", ruout);

    Rot1UlnaCM.Set(Rot1UlnaCM.FastEl(1)-La,1); // setting origin at shoulder

    String rudout;
    Rot1UlnaCM.Print(rudout);
    // taMisc::Info("rotated ulna after translation:\n", rudout);

    float_Matrix Rot2UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation

    ulna->RotateEuler(beta+delta,gamma,taMath_float::pi+alpha,false);
    ulna->Translate(Rot2UlnaCM.FastEl(0),Rot2UlnaCM.FastEl(1)+humerus->length+(ulna->length/2)+elbow_gap,Rot2UlnaCM.FastEl(2),false);
    hand->RotateEuler(beta+delta,gamma,taMath_float::pi+alpha,false);
    hand->Translate(trg_x,trg_y+(humerus->length+ulna->length+elbow_gap+wrist_gap+(hand->length/2)),trg_z,false);

    //------ sending the values of the bodies to ODE -------
    CurToODE();

    //-------- calculating and updating the joint values --------
    // setting the axes for the elbow joint
    elbow->axis.x = -cos(alpha);
    elbow->axis.z = sin(alpha);
    elbow->axis.y = 0.0f;
    elbow->axis2.x = sin(alpha)*sin(beta+delta); // sin(beta+delta) normalizes the norm of axis2
    elbow->axis2.z = cos(alpha)*sin(beta+delta);
    elbow->axis2.y = -cos(beta+delta);
  } // up_axis == Y


  //------- sending the joint axes and anchor points to ODE ---------
  wrist->Init_Anchor();
  elbow->Init_Anchor();

  //-------- looking at the values of the joints in ODE ---------
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
  // taMisc::Info("elbow angle: ", String(angl), "\n");
  // taMisc::Info("elbow anchor: ", smanch, "\n");
  // taMisc::Info("elbow axis1: ", smax1, "\n");
  // taMisc::Info("elbow axis2: ", smax2, "\n");
  //elbow->Init_Anchor();
  //CurFromODE(true);

  SigEmitUpdated(); // this will in theory update the display

  return true;
}

bool VEArm::Bender(taVector3f &p3, taVector3f a, taVector3f c, taVector3f p1, taVector3f p2) {
  // This function is the C++ equivalent of piece5.m.
  // The points a and c are insertion points, whereas p1 and p2 are the extremes of the bending line.
  // If the muscle wraps around the bending line, Bender returns true, and inserts the value of
  // the point of intersection with the muscle in the vector i.

  //---- declaring the variables to be used for bend test ----
  //taVector3f a(-10,-5,-12), c(-1,-1,-5), p1(-3,-4,-7), p2(-2,-4,2), p3(0,0,0);
  taVector3f b, r, v, s;
  float gam;

//---- preliminary calculations ----
  if(arm_side == RIGHT_ARM)
    b = p2 - p1;  // b points from p1 to p2
  else // left arm
    b = p1 - p2; // switching the Y and Z axes switches the bending side, so we invert b

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

void VEArm::GetRandomTarget(float& trg_x, float& trg_y, float& trg_z,
                            float x_ang_min, float x_ang_max,
                            float y_ang_min, float y_ang_max,
                            float z_ang_min, float z_ang_max,
                            float min_dist, float max_dist) {
  // todo: make this more general in terms of orientation etc -- makes a lot of assumptions
  taVector3f euler_rot;
  euler_rot.x = Random::UniformMinMax(x_ang_min, x_ang_max);
  euler_rot.y = Random::UniformMinMax(y_ang_min, y_ang_max);
  euler_rot.z = Random::UniformMinMax(z_ang_min, z_ang_max);

  float tot_len = (La + Lf);
  float max_len = tot_len * max_dist;
  float min_len = tot_len * min_dist;

  taVector3f vec;
  vec.x = 0;
  vec.y = 0;
  vec.z = Random::UniformMinMax(min_len, max_len);

  taQuaternion quat;
  quat.FromEulerVec(euler_rot);
  quat.RotateVec(vec);
  vec += should_loc;
  trg_x = vec.x;
  trg_y = vec.y;
  trg_z = vec.z;
}

bool VEArm::TargetLengths(float trg_x, float trg_y, float trg_z) {
  bool rval = TargetLengths_impl(targ_lens, trg_x, trg_y, trg_z);
  norm_targ_lens = targ_lens;
  norm_targ_lens -= min_lens;
  norm_targ_lens *= spans;
  return rval;
}

bool VEArm::TargetLengths_impl(float_Matrix &trgLen, float trg_x, float trg_y, float trg_z) {
  trgLen.SetGeom(1,n_musc);     // always set to be more robust

  if(!CheckArm()) return false;

// first we change the target coordinates to have the origin at the shoulder
  trg_x = trg_x - should_loc.x;
  trg_y = trg_y - should_loc.y;
  trg_z = trg_z - should_loc.z;

// Avoiding Gimble locks
  if(up_axis == Z) {
    if(trg_x == 0.0 && trg_y == 0.0) {
      trg_y = 0.000001f;  // this is to avoid a Gimble lock
    }
  } 
  else {  // up_axis == Y
    if(trg_x == 0.0 && trg_z == 0.0) {
      trg_z = 0.000001f;  // this is to avoid a Gimble lock
    }
  }

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];

  // target coordinates
  float t_f[] = {trg_x, trg_y, trg_z};
  float_Matrix T(1,3);
  T.InitFromFloats(t_f);
  float D = taMath_float::vec_norm(&T);

// handling targets that are too far or too close
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
  float alpha, beta, gamma, delta;

  if(up_axis == Z) {
    delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
    gamma = 0;
    beta = acos(-trg_z/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
    alpha = asin(-trg_x/sqrt(trg_x*trg_x + trg_y*trg_y));

    if(trg_y < 0) { // if the target is behind
      alpha = taMath_float::pi - alpha;
    }
  }
  else { // up_axis == Y
    delta = taMath_float::pi - acos((La*La + Lf*Lf - D*D) / (2.0*La*Lf));
    gamma = 0;
    beta = acos(-trg_y/D) - acos((D*D + La*La - Lf*Lf)/(2.0*D*La));
    alpha = asin(trg_x/sqrt(trg_x*trg_x + trg_z*trg_z));

    if(trg_z < 0) { // if the target is behind
      alpha = taMath_float::pi - alpha;
    }
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

  if(up_axis == Y) {  // changing coordinates for R as a linear transformation
                      // taking advantage of ct being autoinverse
    float_Matrix cR = float_Matrix(2,3,3);
    taMath_float::mat_mult(&cR, &R, &ct);
    taMath_float::mat_mult(&R, &ct, &cR);
  }

  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  // String out;
  // RotArmIP.Print(out);
  // taMisc::Info("rotated ArmIP:\n", out);

// rotating the ulna's insertion points
  float UlnaShift_f[9] = {0.0f}; // initializes all zeros
  float T_elbowRot_f[9] = {0.0f};

  if(up_axis == Z) {
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, 0, -La,
                           0, 0, -La,
                           0, 0, -La}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta),  sin(delta),
                    0, -sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
    */
    UlnaShift_f[2] = UlnaShift_f[5] = UlnaShift_f[8] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = sin(delta);
    T_elbowRot_f[7] = -sin(delta);
  }
  else { // up_axis == Y 
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, -La, 0,
                           0, -La, 0,
                           0, -La, 0}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta), -sin(delta),
                    0, sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation after the ct bilateral multiplication
    */
    UlnaShift_f[1] = UlnaShift_f[4] = UlnaShift_f[7] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = -sin(delta);
    T_elbowRot_f[7] = sin(delta);
  }

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

  // String ripout;
  // RotFarmIP.Print(ripout);
  // taMisc::Info("rotated ulna IPs:\n", ripout);

// next we obtain the distance between the proximal IPs and the distal IPs
// this code is dependent on the muscle geometry
  int k = 0;
  if(musc_geo==NEW_GEO) k = 1; // offset to change assignments below

  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8+k; i++) { // the 8 or 9 shoulder to humerus muscles
    shoulderIP.x =  ShouldIP.FastElAsFloat(0,i);
    shoulderIP.y =  ShouldIP.FastElAsFloat(1,i);
    shoulderIP.z =  ShouldIP.FastElAsFloat(2,i);
    humerIP.x = RotArmIP.FastElAsFloat(0,i);
    humerIP.y = RotArmIP.FastElAsFloat(1,i);
    humerIP.z = RotArmIP.FastElAsFloat(2,i);
    pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
    pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
    pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

    if(Bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=(2+k) && i!=(3+k)) {
      // if muscle wraps around bending line (except for muscles 3(4) and 4(5))
      c1 = shoulderIP - p3; c2 = p3 - humerIP;
      trgLen.Set(c1.Mag()+c2.Mag(),i);
    } else {
      c1 = shoulderIP - humerIP;
      trgLen.Set(c1.Mag(),i);
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  c1.x = ShouldIP.FastElAsFloat(0,8+k) - RotFarmIP.FastElAsFloat(0,0);
  c1.y = ShouldIP.FastElAsFloat(1,8+k) - RotFarmIP.FastElAsFloat(1,0);
  c1.z = ShouldIP.FastElAsFloat(2,8+k) - RotFarmIP.FastElAsFloat(2,0);
  trgLen.Set(c1.Mag(),8+k);
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    c1.x = RotArmIP.FastElAsFloat(0,7+k+i) - RotFarmIP.FastElAsFloat(0,i);
    c1.y = RotArmIP.FastElAsFloat(1,7+k+i) - RotFarmIP.FastElAsFloat(1,i);
    c1.z = RotArmIP.FastElAsFloat(2,7+k+i) - RotFarmIP.FastElAsFloat(2,i);
    trgLen.Set(c1.Mag(),8+k+i);
  }

  // String trLout;
  // trgLen.Print(trLout);
  // taMisc::Info("target lengths: ", trLout, "\n");
  return true;
}

bool VEArm::Lengths(float_Matrix& len, bool normalize) {
  // note: please follow coding conventions of using lower-case for variables
  len.SetGeom(1, n_musc);       // far better to enforce geom than warn about it
  for(int i=0; i<n_musc; i++) {
    len.Set(muscles[i]->Length(),i);
  }
  if(normalize) {
    len -= min_lens;
    len *= spans;  // normalizing lengths (spans is initialized in InitMuscles) 
  }
  return true;
}

bool VEArm::Speeds(float_Matrix& vel, bool normalize) {
  vel.SetGeom(1, n_musc);       // far better to enforce geom than warn about it
  for(int i=0; i<n_musc; i++) {
    vel.Set(muscles[i]->Speed(),i);
    if(normalize) {
      vel.FastEl(i) = 1.0f / (1.0f + expf(-vel_norm_gain * vel.FastEl(i)));
    }
  }
  return true;
}

bool VEArm::ApplyStim(const float_Matrix& stims, float_Matrix &fs) {
  if(TestWarning(stims.count() != n_musc, "","The stimulus matrix doesn't match the number of muscles \n"))
    return false;
  fs.SetGeom(2, 3, n_musc);     // this is fast if it matches already

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];

  int k = 0;
  if(musc_geo == NEW_GEO) k = 1; // offset to change assignments below

  taVector3f daforce(0.0f, 0.0f, 0.0f);
  for(int i=0; i<n_musc; i++) {
    daforce = muscles[i]->Contract(stims.FastElAsFloat(i));
    fs.Set(daforce.x, 0, i);
    fs.Set(daforce.y, 1, i);
    fs.Set(daforce.z, 2, i);

    if(i < 8+k) {  // muscles from shoulder to humerus
      humerus->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x + should_loc.x, muscles[i]->IPdist.y + should_loc.y, muscles[i]->IPdist.z + should_loc.z,false,false);
    }
    else if(i == 8+k) {  // biceps
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x + should_loc.x, muscles[i]->IPdist.y + should_loc.y, muscles[i]->IPdist.z + should_loc.z,false,false);
    }
    else { // triceps and brachialis
      humerus->AddForceAtPos(-daforce.x,-daforce.y,-daforce.z, muscles[i]->IPprox.x + should_loc.x, muscles[i]->IPprox.y + should_loc.y, muscles[i]->IPprox.z + should_loc.z,false,false);
      ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x + should_loc.x, muscles[i]->IPdist.y + should_loc.y, muscles[i]->IPdist.z + should_loc.z,false,false);
    }

  }
  return true;
}

bool VEArm::VEP_Reach(const float_Matrix& trg_lens, float gain, float_Matrix &fs) {
// Do one step of reaching using the velocity-controlled Equilibrium Point algorithm. 
// This will calculate the activation (multiplying both errors by the gain), calculate 
// (and store) the resulting fs, and apply them.
// It does not take a step of the VEWorld, and does not udpate the muscle insertion points.

  if(TestWarning(trg_lens.count() != n_musc, "","The targets matrix doesn't match the number of muscles \n"))
    return false;

  fs.SetGeom(2, 3, n_musc);     // this is fast if it matches already

  float_Matrix len_error(1,n_musc), vel_error(1,n_musc);

  if(musc_type == LINEAR) {
    float_Matrix lens(1,n_musc), vels(1,n_musc); 
    Lengths(lens, false);  // storing the lengths and speeds in lens and vels respectively
    Speeds(vels, false);
    len_error = lens - trg_lens;
    len_error *= gain;
    vel_error = len_error + vels;
    vel_error *= gain;
    ApplyStim(vel_error, fs);
  }
  else { // musc_type == HILL
    float_Matrix old_len(1,n_musc), old_vel(1,n_musc);

    for(int i=0; i<n_musc; i++) {
      old_len.Set(muscles[i]->Old_Length(),i); 
      old_vel.Set(muscles[i]->Old_Speed(),i); 
    }
    len_error = old_len - trg_lens;
    len_error *= gain;
    vel_error = len_error + old_vel;
    vel_error *= gain;
    old_vel *= 0.06; //muscles[0]->mu;  // old_vel = mu*old_vel
    float_Matrix act(1,n_musc);
    act = vel_error + old_vel;
    ApplyStim(act, fs);
    //String velst;
    //act.Print(velst);
    //taMisc::Info("applied stim: ", velst, "\n");
  }
  return true;
}


bool VEArm::AngToLengths(float_Matrix &Len, float alpha, float beta, float gamma, float delta) {
  Len.SetGeom(1,n_musc);     // always set to be more robust

  if(!CheckArm()) return false;

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

  if(up_axis == Y) {  // changing coordinates for R as a linear transformation
                      // taking advantage of ct being autoinverse
    float_Matrix cR = float_Matrix(2,3,3);
    taMath_float::mat_mult(&cR, &R, &ct);
    taMath_float::mat_mult(&R, &ct, &cR);
  }

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
  float UlnaShift_f[9] = {0.0f}; // initializes all zeros
  float T_elbowRot_f[9] = {0.0f};

  if(up_axis == Z) {
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, 0, -La,
                           0, 0, -La,
                           0, 0, -La}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta),  sin(delta),
                    0, -sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
    */
    UlnaShift_f[2] = UlnaShift_f[5] = UlnaShift_f[8] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = sin(delta);
    T_elbowRot_f[7] = -sin(delta);
  }
  else { // up_axis == Y 
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, -La, 0,
                           0, -La, 0,
                           0, -La, 0}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(delta), -sin(delta),
                    0, sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation after the ct bilateral multiplication
    */
    UlnaShift_f[1] = UlnaShift_f[4] = UlnaShift_f[7] = -La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = -sin(delta);
    T_elbowRot_f[7] = sin(delta);
  }

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

  // String ripout;
  // RotFarmIP.Print(ripout);
  // taMisc::Info("rotated ulna IPs:\n", ripout);

// next we obtain the distance between the proximal IPs and the distal IPs
// this code is dependent on the muscle geometry
  int k = 0;
  if(musc_geo==NEW_GEO) k = 1; // offset to change assignments below

  taVector3f c1, c2, shoulderIP, humerIP, pv1, pv2, p3;
  for(int i=0; i<8+k; i++) { // the 8 or 9 shoulder to humerus muscles
    shoulderIP.x =  ShouldIP.FastElAsFloat(0,i);
    shoulderIP.y =  ShouldIP.FastElAsFloat(1,i);
    shoulderIP.z =  ShouldIP.FastElAsFloat(2,i);
    humerIP.x = RotArmIP.FastElAsFloat(0,i);
    humerIP.y = RotArmIP.FastElAsFloat(1,i);
    humerIP.z = RotArmIP.FastElAsFloat(2,i);
    pv1.x = p1.FastElAsFloat(0,i); pv2.x = p2.FastElAsFloat(0,i);
    pv1.y = p1.FastElAsFloat(1,i); pv2.y = p2.FastElAsFloat(1,i);
    pv1.z = p1.FastElAsFloat(2,i); pv2.z = p2.FastElAsFloat(2,i);

    if(Bender(p3,shoulderIP,humerIP,pv1,pv2) && i!=(2+k) && i!=(3+k)) {
      // if muscle wraps around bending line (except for muscles 3(4) and 4(5))
      c1 = shoulderIP - p3; c2 = p3 - humerIP;
      Len.Set(c1.Mag()+c2.Mag(),i);
    } else {
      c1 = shoulderIP - humerIP;
      Len.Set(c1.Mag(),i);
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  c1.x = ShouldIP.FastElAsFloat(0,8+k) - RotFarmIP.FastElAsFloat(0,0);
  c1.y = ShouldIP.FastElAsFloat(1,8+k) - RotFarmIP.FastElAsFloat(1,0);
  c1.z = ShouldIP.FastElAsFloat(2,8+k) - RotFarmIP.FastElAsFloat(2,0);
  Len.Set(c1.Mag(),8+k);
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    c1.x = RotArmIP.FastElAsFloat(0,7+k+i) - RotFarmIP.FastElAsFloat(0,i);
    c1.y = RotArmIP.FastElAsFloat(1,7+k+i) - RotFarmIP.FastElAsFloat(1,i);
    c1.z = RotArmIP.FastElAsFloat(2,7+k+i) - RotFarmIP.FastElAsFloat(2,i);
    Len.Set(c1.Mag(),8+k+i);
  }
/*
  String trLout;
  Len.Print(trLout);
  taMisc::Info("resulting lengths: ", trLout, "\n");
*/
  return true;
}

bool VEArm::NormLengthsToTable(DataTable* len_table) {
  char col_name[] = "lengths"; 
  DataCol* dc = len_table->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(len_table->rows == 0)       // empty table, make sure we have at least 1 row
    dc->EnforceRows(1);
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(norm_lens.SafeEl(i),-1, // -1 = last row
                     0,0,0,i);
  }
  return true;
}

bool VEArm::NormTargLengthsToTable(DataTable* len_table) {
  char col_name[] = "targ_lengths"; 
  DataCol* dc = len_table->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(len_table->rows == 0)       // empty table, make sure we have at least 1 row
    dc->EnforceRows(1);
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(norm_targ_lens.SafeEl(i),-1, // -1 = last row
                     0,0,0,i);
  }
  return true;
}

bool VEArm::NormSpeedsToTable(DataTable* vel_table) {
  char col_name[] = "speeds"; 
  DataCol* dc = vel_table->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(vel_table->rows == 0)       // empty table, make sure we have at least 1 row
    dc->EnforceRows(1);
  
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(norm_vels.SafeEl(i),-1, // -1 = last row
                     0,0,0,i); // normalizing using the logistic function
  }
  return true;
}

bool VEArm::NormHandCoordsToTable(DataTable* coords_table) {
  char col_name[] = "hand_coords"; 
  DataCol* dc = coords_table->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,3);
  if(coords_table->rows == 0)       // empty table, make sure we have at least 1 row
    dc->EnforceRows(1);
  
  VEBody* hand = bodies[HAND];
  float maxl = La+Lf;

  if(up_axis == Y) {
    dc->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0); // -1 = last row
    dc->SetMatrixVal(0.5f+(hand->cur_pos.y-should_loc.y)/(1.9f*maxl),-1, 0,0,0,1); // -1 = last row
    dc->SetMatrixVal(0.1f+(hand->cur_pos.z-should_loc.z)/(maxl),-1, 0,0,0,2); // can't reach behind
  } else {
    dc->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0); // -1 = last row
    dc->SetMatrixVal(0.1f+(hand->cur_pos.y-should_loc.y)/(maxl),-1, 0,0,0,1); // can't reach behind
    dc->SetMatrixVal(0.5f+(hand->cur_pos.z-should_loc.z)/(1.9f*maxl),-1, 0,0,0,2); // -1 = last row
  } 
  return true;
}


bool VEArm::ArmStateToTable(DataTable* table) {
  NormLengthsToTable(table);
  NormTargLengthsToTable(table);
  NormSpeedsToTable(table);
  return true;
}

bool VEArm::SetTargetLengthsFmTable(DataTable* len_table) {
  char col_name[] = "lengths";
  DataCol* dc = len_table->FindColName(col_name, true); // find the "lengths" column, error msg if not found
  MatrixGeom std_geom(4,12,1,1,n_musc);

  if(TestError(dc->cell_geom != std_geom, "SetTargetLengthsFmTable","The geometry of the table provided to GetTargetLengths() is not standard\n"))
  {
    //return false;
  }

  taMatrix* cell_mat = dc->GetValAsMatrix(-1); // -1 = last row

  for(int i=0; i<n_musc; i++)
  {
    norm_targ_lens.Set(cell_mat->SafeElAsFloat(0,0,0,i),i);
  }
  //norm_targ_lens.CopyFrom(cell_mat);
  targ_lens = norm_targ_lens;
  targ_lens /= spans;
  targ_lens += min_lens;
  return true;
}

void VEArm::Step_pre() {
  VEP_Reach(targ_lens, gain, forces);
  inherited::Step_pre();
}

void VEArm::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  UpdateIPs();
  CurToODE();
}

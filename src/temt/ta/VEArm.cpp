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

TA_BASEFUNS_CTORS_DEFN(VEArmLengths);

TA_BASEFUNS_CTORS_DEFN(VEArmAngles);

TA_BASEFUNS_CTORS_DEFN(VEArmDelays);

TA_BASEFUNS_CTORS_DEFN(VEArmErrors);

TA_BASEFUNS_CTORS_DEFN(VEArmGains);

TA_BASEFUNS_CTORS_DEFN(VEArmDamping);

TA_BASEFUNS_CTORS_DEFN(VEArm);

void VEArmLengths::Initialize() {
  humerus = 0.3f;
  humerus_radius = 0.02f;
  ulna = 0.24f;
  ulna_radius = 0.02f;
  hand = 0.08f;
  hand_radius = 0.03f;
  elbow_gap = 0.03f;
  wrist_gap = 0.03f;

  sh_off_x = 0.0f;
  sh_off_y = 0.0f;
  sh_off_z = 0.0f;

  sh_x = 0.0f;
  sh_y = 0.0f;
  sh_z = 0.0f;

  humerus_mid = 0.5f * humerus;
  ulna_mid = 0.5f * ulna;
  hand_mid = 0.5f * hand;
  elbow_gap_mid = 0.5f * elbow_gap;
  wrist_gap_mid = 0.5f * wrist_gap;

  La = humerus + elbow_gap_mid;
  Lf = ulna + elbow_gap_mid + wrist_gap + hand_mid;
  Ltot = La + Lf;
}

void VEArmLengths::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  humerus_mid = 0.5f * humerus;
  ulna_mid = 0.5f * ulna;
  hand_mid = 0.5f * hand;
  elbow_gap_mid = 0.5f * elbow_gap;
  wrist_gap_mid = 0.5f * wrist_gap;
  
  La = humerus + elbow_gap_mid;
  Lf = ulna + elbow_gap_mid + wrist_gap + hand_mid;
  Ltot = La + Lf;
}

void VEArmAngles::Initialize() {
  x = 0.0f;
  y = 0.0f;
  z = 0.0f;
  elbow = 0.0f;
  alpha = 0.0f;
  beta = 0.0f;
  gamma = 0.0f;
  delta = 0.0f;
}

void VEArmAngles::UpdateAngles() {
  if(up_y) {
    alpha = z;
    beta =  x;
    gamma = y;
    delta = elbow;
  }
  else {
    // todo: is there a diff rotation for up_z?
    alpha = z;
    beta =  x;
    gamma = y;
    delta = elbow;
  }
}

void VEArmAngles::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  UpdateAngles();
}

void VEArmDelays::Initialize() {
  vis = 30;
  pro = 7;
  eff = 1;
}

void VEArmErrors::Initialize() {
  max = 0.05f;
  norm_dra_dt = 0.3f;
  hand_vra_dt = 0.1f;
  pid_dra_dt = 1.0f;
  io_thr = 0.001f;
}

void VEArmGains::Initialize() {
  stim = 200.0f;
  vel_norm = 15.0f;
  ev = 2.0f;
  p = 40.0f;
  i = 5.0f;
  d = 5.0f;
  musc_mag = 0.0f;
}

void VEArmDamping::Initialize() {
  fac = 0.0f;
  thr = 0.05f;
}

void VEArm::Initialize() {
  // just the default initial values here -- note that VEObject parent initializes all the space stuff in its Initialize, so you don't need to do that here

  show_ips = false;

  arm_side = RIGHT_ARM;
  up_axis = Y;
  musc_geo = NEW_GEO;
  musc_type = LINEAR;
  hill_mu = 0.06f;
  ctrl_type = PID;
  
  // note: torso ref is self initializing
  world_step = 0.01f;

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

  init_angs.up_y = (up_axis == Y);
  targ_angs.up_y = (up_axis == Y);
  cur_angs.up_y =  (up_axis == Y);

  targ_rel_d = 0.0f;
  hand_vel_mag = 0.0f;
  hand_vra = 0.0f;
  targ_lens_mag = 0.0f;
  lens_mag = 0.0f;
  vels_mag = 0.0f;
  stims_p_mag = 0.0f;
  stims_i_mag = 0.0f;
  stims_d_mag = 0.0f;
  stims_mag = 0.0f;

  reach_start = true;

  err_loc_mag = 0.0f;
  err_loc_cur_mag = 0.0f;
  err_len_mag = 0.0f;
  err_itg_mag = 0.0f;
  err_drv_dra_mag = 0.0f;
  err_io_mag = 0.0f;

  // DO NOT put anythying other than direct literal member inits in here -- no creating objects etc
}

void VEArm::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if(TestError(!(bool)arm_state, "VEArm", "arm_state is null -- you must set this to a data table!"))
    {
      // todo: if null, actually auto-create for user.
    }

  init_angs.up_y = (up_axis == Y);
  targ_angs.up_y = (up_axis == Y);
  cur_angs.up_y =  (up_axis == Y);
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

  R.SetGeom(2,3,3);
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
  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);
  
  //String Rout;
  //R.Print(Rout);
  //taMisc::Info("R used in UpdateIPs:\n", Rout);

  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  // rotating the ulna's insertion points
  elbow->CurFromODE(true);      // so the angle we get is actualized
  cur_angs.elbow = elbow->pos;     // DON'T KNOW IF THIS WILL WORK

  if(up_axis == Y) { 
    // Now we get angle directly from humerus and ulna body rotations
    // NOTE: will need an equivalent for UP_Z -- falls back on elbow->pos for now
    taVector3f humrot;
    humerus->cur_quat_raw.ToEulerVec(humrot);
    cur_angs.x = -humrot.x;
    cur_angs.y = humrot.y;
    cur_angs.z = humrot.z;
    taVector3f ulnarot;
    ulna->cur_quat_raw.ToEulerVec(ulnarot);
    cur_angs.elbow = -cur_angs.x - ulnarot.x;
  }

  cur_angs.UpdateAngles();	// get alpha, beta etc

  float UlnaShift_f[9] = {0.0f}; // initializes all zeros
  float T_elbowRot_f[9] = {0.0f};

  if(up_axis == Z) {
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, 0, -alens.La,
                           0, 0, -alens.La,
                           0, 0, -alens.La}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(cur_angs.delta),  sin(cur_angs.delta),
                    0, -sin(cur_angs.delta), cos(cur_angs.delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
    */
    UlnaShift_f[2] = UlnaShift_f[5] = UlnaShift_f[8] = -alens.La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(cur_angs.delta);
    T_elbowRot_f[5] = sin(cur_angs.delta);
    T_elbowRot_f[7] = -sin(cur_angs.delta);
  }
  else { // up_axis == Y 
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, -alens.La, 0,
                           0, -alens.La, 0,
                           0, -alens.La, 0}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,
                    0, cos(cur_angs.delta), -sin(cur_angs.delta),
                    0, sin(cur_angs.delta), cos(cur_angs.delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation after the ct bilateral multiplication
    */
    UlnaShift_f[1] = UlnaShift_f[4] = UlnaShift_f[7] = -alens.La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(cur_angs.delta);
    T_elbowRot_f[5] = -sin(cur_angs.delta);
    T_elbowRot_f[7] = sin(cur_angs.delta);
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

  // now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

  // finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

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

      ShowIP(i, 0, muscles[i]->IPprox);
      ShowIP(i, 1, muscles[i]->IPdist);

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

    ShowIP(8, 0, muscles[8]->IPprox);
    ShowIP(8, 1, muscles[8]->IPdist);

    // the triceps and the brachialis connect from humerus to ulna
    for(int i=1; i<=2; i++) {
      muscles[8+i]->IPprox.x = RotArmIP.FastElAsFloat(0,7+i);
      muscles[8+i]->IPprox.y = RotArmIP.FastElAsFloat(1,7+i);
      muscles[8+i]->IPprox.z = RotArmIP.FastElAsFloat(2,7+i);
      muscles[8+i]->IPdist.x = RotFarmIP.FastElAsFloat(0,i);
      muscles[8+i]->IPdist.y = RotFarmIP.FastElAsFloat(1,i);
      muscles[8+i]->IPdist.z = RotFarmIP.FastElAsFloat(2,i);
      muscles[8+i]->bend = false;

      ShowIP(8+i, 0, muscles[8+i]->IPprox);
      ShowIP(8+i, 1, muscles[8+i]->IPdist);
    }
  }
  else {  // musc_geo==NEW_GEO
    for(int i=0; i<9; i++) {  // the 9 shoulder to humerus muscles
      muscles[i]->IPprox.x = ShouldIP.FastElAsFloat(0,i);
      muscles[i]->IPprox.y = ShouldIP.FastElAsFloat(1,i);
      muscles[i]->IPprox.z = ShouldIP.FastElAsFloat(2,i);
      muscles[i]->IPdist.x = RotArmIP.FastElAsFloat(0,i);
      muscles[i]->IPdist.y = RotArmIP.FastElAsFloat(1,i);
      muscles[i]->IPdist.z = RotArmIP.FastElAsFloat(2,i);

      ShowIP(i, 0, muscles[i]->IPprox);
      ShowIP(i, 1, muscles[i]->IPdist);

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

    ShowIP(9, 0, muscles[9]->IPprox);
    ShowIP(9, 1, muscles[9]->IPdist);

    // taMisc::DebugInfo("biceps ipdist:", muscles[9]->IPdist.GetStr());

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

      ShowIP(9+i, 0, muscles[9+i]->IPprox);
      ShowIP(9+i, 1, muscles[9+i]->IPdist);
    }
  }
  //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_


  GetNormVals();

  return true;
}

void VEArm::ShowIP(int ipno, int prox_dist, taVector3f& ip_loc) {
  if(!show_ips) {
    if(bodies.size > N_ARM_BODIES) {
      bodies.SetSize(N_ARM_BODIES);
    }
    return;
  }

  int tot_ips = 12;
  if(musc_geo == OLD_GEO)
    tot_ips = 11;
  int tot_bods = N_ARM_BODIES + 2 * tot_ips;

  if(bodies.size < tot_bods) {
    bodies.SetSize(tot_bods);
    // then initialize all the bodies -- use colorscale scheme etc.

    if(!color_scale) {
      color_scale = new ColorScale;
      taBase::Own(color_scale, this);
    }
    color_scale->SetMinMax(0, (float)tot_ips);
    float sc_val;

    for(int i=0;i<tot_ips;i++) {
      iColor ic = color_scale->GetColor((float)i, sc_val);
      VEBody* ipbodp = bodies[N_ARM_BODIES + 2*i];
      ipbodp->name = String("ip_") + String(i) + "_p";
      ipbodp->color.setColor(ic);
      ipbodp->shape = VEBody::SPHERE;
      ipbodp->radius = 0.01f;
      ipbodp->SetBodyFlag(VEBody::NO_COLLIDE);

      VEBody* ipbodd = bodies[N_ARM_BODIES + 2*i+1];
      ipbodd->name = String("ip_") + String(i) + "_d";
      ipbodd->color.setColor(ic);
      ipbodd->shape = VEBody::SPHERE;
      ipbodd->radius = 0.01f;
      ipbodd->SetBodyFlag(VEBody::NO_COLLIDE);
    }
  }

  VEBody* ipbod = bodies[N_ARM_BODIES + ipno*2 + prox_dist];
  ipbod->init_pos = ip_loc;
  ipbod->Init();
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
    float FarmIP_f[] = {  0,      0.015f, -alens.La - 0.05f,
                          0,     -0.005f, -alens.La + 0.03f,
                         -0.01f,  0.015f, -alens.La - 0.04f };
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
    float FarmIP_f[] = {  0,      0.015f, -alens.La - 0.05f,    // w
                          0,     -0.005f, -alens.La + 0.03f,    // y
                         -0.01f,  0.015f, -alens.La - 0.04f };  // zz
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

  stims_delay.SetGeom(2, n_musc, delays.eff - 1); // buffer table of muscle stimuli -- used by ApplyStim method when delays.eff > 1

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
  gains.musc.SetGeom(1,n_musc);
  SetAllMuscGains(1.0f);
  const float pi  = taMath_float::pi;

  if(musc_geo == OLD_GEO) {
    // these are the angles at which muscles attain their maximum length for right arms
    float alphaM[] = {pi/2, pi/4, pi/300, pi/300, -.7f*pi, .3f*pi, -.7f*pi, .3f*pi, pi, pi/300, pi/300};
    float betaM[] = {.03f*pi, .8f*pi, pi/300, pi/300, .6f*pi, .6f*pi, .6f*pi, .6f*pi, pi/30, pi/300,pi/300}; 
    float gammaM[] = {pi/2, 0, -1.3f, 1.3f, -1.4f, .5f, -1.0f, -pi/2, 1.0f, 0, 0};
    float deltaM[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9f*pi, 0};

    // these are the angles at which muscles attain their minimum length for right arms
    float alpham[] = {pi/2, pi/2, pi/300, pi/300, pi/5, -.7f*pi, pi/5, -.7f*pi, pi/4/ pi/300, pi/300};
    float betam[] = {.7f*pi, .03f*pi, pi/300, pi/300, .7f*pi, .6f*pi, .3f*pi, .3f*pi, pi/30, pi/300, pi/300};
    float gammam[] = {pi/2, -pi/2, 1.2f, -1.2f, pi/2, 1.0f, 1.0f, 1.4f, 0.2f, 0, 0};
    float deltam[] = {0, 0, 0, 0, 0, 0, 0, 0, 0.8f*pi, 0, 0.8f*pi};

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
      max_lens.Set(tlens.FastEl1d(i)+0.003f,i);

      AngToLengths(tlens,alpham[i],betam[i],gammam[i],deltam[i]);
      min_lens.Set(tlens.FastEl1d(i)-0.004f,i);
    }
  } else { // musc_geo == NEW_GEO
    // these are the angles at which muscles attain their maximum length for right arms
    float alphaM[] = {pi/7, pi-pi/7, -pi/2, pi/300, pi/300, -.7f*pi, .3f*pi, -.7f*pi, .3f*pi, pi, pi/300, pi/300};
    float betaM[] = {pi/20, pi/20, .8f*pi, pi/300, pi/300, .6f*pi, .6f*pi, .6f*pi, .6f*pi, pi/30, pi/300,pi/300}; 
    float gammaM[] = {1.4f, 1.0f, 1.4f, -1.45f, 1.45f, -.5f, .5f, -.5f, -.5f, 1.0f, 0, 0};
    float deltaM[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.9f*pi, 0};

    // these are the angles at which muscles attain their minimum length for right arms
    float alpham[] = {-pi/2, -pi/2, pi/2, pi/300, pi/300, pi/5, -.7f*pi, pi/5, -.7f*pi, pi/4, pi/300, pi/300};
    float betam[] = {.8f*pi, .8f*pi, pi/20, pi/300, pi/300, .7f*pi, .6f*pi, .3f*pi, .3f*pi, pi/30, pi/300, pi/300};
    float gammam[] = {pi/2, pi/2, -pi/2, 1.45f, -1.45f, pi/2, -.5f, 0, 1.4f, 0.2f, 0, 0};
    float deltam[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.8f*pi, 0, 0.8f*pi};

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
      max_lens.Set(tlens.FastEl1d(i)+0.003f,i);

      AngToLengths(tlens,alpham[i],betam[i],gammam[i],deltam[i]);
      min_lens.Set(tlens.FastEl1d(i)-0.003f,i);
    }
  }

  if(arm_side == RIGHT_ARM) {
    AngToLengths(rest_lens, -pi/4, pi/8, 0, pi/4);  // setting resting lengths
  }
  else { // arm_side == LEFT_ARM
    AngToLengths(rest_lens, pi/4, pi/8, 0, pi/4);  // setting resting lengths
  }

  // Initializing the spans vector
  spans.SetGeom(1,n_musc);
  for(int i=0; i<n_musc; i++) {
    spans.Set(1.0f/(max_lens.FastEl1d(i)-min_lens.FastEl1d(i)),i);
  }
}


bool VEArm::ConfigArm(float humerus_length, float humerus_radius,
                      float ulna_length, float ulna_radius,
                      float hand_length, float hand_radius, 
                      float elbow_gap, float wrist_gap,
                      float sh_off_x, float sh_off_y, float sh_off_z) {
                      
  // tor_cm are the coordinates of the torso's center of mass
  // sh_off are the coordinates of the shoulder's anchor point wrt default position

  if(TestError(!torso, "ConfigArm", "torso not set -- must specify a body in another object to serve as the torso before running ConfigArm"))
    return false;

  alens.humerus = humerus_length;
  alens.humerus_radius = humerus_radius;
  alens.ulna = ulna_length;
  alens.ulna_radius = ulna_radius;
  alens.hand = hand_length;
  alens.hand_radius = hand_radius;
  alens.elbow_gap = elbow_gap;
  alens.wrist_gap = wrist_gap;

  alens.sh_off_x = sh_off_x;
  alens.sh_off_y = sh_off_y;
  alens.sh_off_z = sh_off_z;

  alens.UpdateAfterEdit();      // this computes all the _mid and La, Lf values from above
  
  return ReConfigArm();
}

bool VEArm::ReConfigArm() {
  if(TestError(!torso, "ReConfigArm", "torso not set -- must specify a body in another object to serve as the torso before running ConfigArm"))
    return false;

  float tor_cmX = torso->init_pos.x;
  float tor_cmY = torso->init_pos.y;
  float tor_cmZ = torso->init_pos.z;

  // Assuming that torso is a box!
  // changing sh_off variables so they are wrt torso's CM
  if(arm_side == RIGHT_ARM) {
    if(up_axis == Y)  {
      alens.sh_y = alens.sh_off_y + (torso->box.y)/2.0f;
      alens.sh_x = alens.sh_off_x - (torso->box.x)/2.0f - 0.07f;
    } 
    else { // up_axis == Z
      alens.sh_z = alens.sh_off_z + (torso->box.z)/2.0f;
      alens.sh_x = alens.sh_off_x + (torso->box.x)/2.0f + 0.07f;
    }
  } 
  else {  // arm_side == LEFT
    if(up_axis == Y) {
      alens.sh_y = alens.sh_off_y + (torso->box.y)/2.0f;
      alens.sh_x = alens.sh_off_x + (torso->box.x)/2.0f + 0.07f;
    } 
    else { // up_axis == Z
      alens.sh_z = alens.sh_off_z + (torso->box.z)/2.0f;
      alens.sh_x = alens.sh_off_x - (torso->box.x)/2.0f - 0.07f;
    }
  }

  float shX, shY, shZ;  // shoulder coordinates wrt the World coordinate system
  shX = tor_cmX + alens.sh_x; // The sh variables are here because the name is shorter than should_loc
  shY = tor_cmY + alens.sh_y;
  shZ = tor_cmZ + alens.sh_z;

  should_loc.x = shX;
  should_loc.y = shY;
  should_loc.z = shZ;

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

  if(up_axis == Z) {
    humerus->name = name_prefix + "Humerus";
    humerus->shape = VEBody::CAPSULE; humerus->long_axis = VEBody::LONG_Z;
    humerus->length = alens.humerus; humerus->radius = alens.humerus_radius;
    humerus->init_pos.x = shX; humerus->init_pos.y = shY; humerus->init_pos.z = shZ - alens.humerus_mid;
    //humerus->init_rot.x = 0; humerus->init_rot.y = 0; humerus->init_rot.z = 0; humerus->init_rot.rot = 0;
    humerus->init_quat.SetSXYZ(1,0,0,0);
    humerus->cur_pos.x = shX; humerus->cur_pos.y = shY; humerus->cur_pos.z = humerus->init_pos.z;
    //humerus->cur_rot.x = 0; humerus->cur_rot.y = 0; humerus->cur_rot.z = 0; humerus->cur_rot.rot = 0;
    humerus->cur_quat.SetSXYZ(1,0,0,0);
    humerus->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat

    ulna->name = name_prefix + "Ulna";
    ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Z;
    ulna->length = alens.ulna; ulna->radius = alens.ulna_radius;
    ulna->init_pos.x = shX; ulna->init_pos.y = shY; ulna->init_pos.z = shZ-alens.humerus-(alens.ulna_mid)-alens.elbow_gap;
    //ulna->init_rot.x = 0; ulna->init_rot.y = 0; ulna->init_rot.z = 0;
    ulna->init_quat.SetSXYZ(1,0,0,0);
    ulna->cur_pos.x = shX; ulna->cur_pos.y = shY; ulna->cur_pos.z = ulna->init_pos.z;
    //ulna->cur_rot.x = 0; ulna->cur_rot.y = 0; ulna->cur_rot.z = 0;
    ulna->cur_quat.SetSXYZ(1,0,0,0);
    ulna->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat

    hand->name = name_prefix + "Hand";
    hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Z;
    hand->length = alens.hand; hand->radius = alens.hand_radius;
    hand->init_pos.x = shX; hand->init_pos.y = shY;
    hand->init_pos.z = shZ-(alens.humerus+alens.ulna+alens.elbow_gap+alens.wrist_gap+alens.hand_mid);
    //hand->init_rot.x = 0; hand->init_rot.y = 0; hand->init_rot.z = 0;
    hand->init_quat.SetSXYZ(1,0,0,0);
    hand->cur_pos.x = shX; hand->cur_pos.y = shY; hand->cur_pos.z = hand->init_pos.z;
    //hand->cur_rot.x = 0; hand->cur_rot.y = 0; hand->cur_rot.z = 0;
    hand->cur_quat.SetSXYZ(1,0,0,0);
    hand->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat

    //-------- Creating initial joints -------

    //-------- Setting joint locations -------
    shoulder->name = name_prefix + "Shoulder";
    shoulder->joint_type = VEJoint::BALL;
    shoulder->body1 = torso; shoulder->body2 = humerus;

    elbow->name = name_prefix + "Elbow";
    elbow->joint_type = VEJoint::HINGE;
    elbow->body1 = humerus; elbow->body2 = ulna;

    wrist->name = name_prefix + "Wrist";
    wrist->joint_type = VEJoint::FIXED;
    wrist->body1 = ulna; wrist->body2 = hand;

    // the shoulder anchor is wrt to torso's CM
    shoulder->anchor.x = alens.sh_x; 
    shoulder->anchor.y = alens.sh_y; 
    shoulder->anchor.z = alens.sh_z;

    elbow->anchor.x = 0;  // set elbow joint's anchor point wrt humerus' CM
    elbow->anchor.y = 0;
    elbow->anchor.z = -(alens.humerus_mid +(alens.elbow_gap_mid));

    wrist->anchor.x = 0; // set wrist joint's anchor point wrt ulna's CM
    wrist->anchor.y = 0;
    wrist->anchor.z = -(alens.ulna_mid + (alens.wrist_gap_mid));

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
    humerus->length = alens.humerus; humerus->radius = alens.humerus_radius;
    humerus->init_pos.x = shX; humerus->init_pos.z = shZ; humerus->init_pos.y = shY-alens.humerus_mid;
    //humerus->init_rot.x = 0; humerus->init_rot.z = 0; humerus->init_rot.y = 0;
    humerus->init_quat.SetSXYZ(1,0,0,0);
    humerus->cur_pos.x = shX; humerus->cur_pos.z = shZ; humerus->cur_pos.y = humerus->init_pos.y;
    //humerus->cur_rot.x = 0; humerus->cur_rot.z = 0; humerus->cur_rot.y = 0;
    humerus->cur_quat.SetSXYZ(1,0,0,0);
    humerus->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat

    ulna->name = name_prefix + "Ulna";
    ulna->shape = VEBody::CAPSULE; ulna->long_axis = VEBody::LONG_Y;
    ulna->length = alens.ulna; ulna->radius = alens.ulna_radius;
    ulna->init_pos.x = shX; ulna->init_pos.z = shZ; ulna->init_pos.y = shY-alens.humerus-(alens.ulna_mid)-alens.elbow_gap;
    //ulna->init_rot.x = 0; ulna->init_rot.z = 0; ulna->init_rot.y = 0;
    ulna->init_quat.SetSXYZ(1,0,0,0);
    ulna->cur_pos.x = shX; ulna->cur_pos.z = shZ; ulna->cur_pos.y = ulna->init_pos.y;
    //ulna->cur_rot.x = 0; ulna->cur_rot.z = 0; ulna->cur_rot.y = 0;
    ulna->cur_quat.SetSXYZ(1,0,0,0);
    ulna->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat

    hand->name = name_prefix + "Hand";
    hand->shape = VEBody::CAPSULE; hand->long_axis = VEBody::LONG_Y;
    hand->length = alens.hand; hand->radius = alens.hand_radius;
    hand->init_pos.x = shX; hand->init_pos.z = shZ;
    hand->init_pos.y = shY - (alens.humerus+alens.ulna+alens.elbow_gap+alens.wrist_gap+(alens.hand_mid));
    //hand->init_rot.x = 0; hand->init_rot.z = 0; hand->init_rot.y = 0;
    hand->init_quat.SetSXYZ(1,0,0,0);
    hand->cur_pos.x = shX; hand->cur_pos.z = shZ; hand->cur_pos.y = hand->init_pos.y;
    //hand->cur_rot.x = 0; hand->cur_rot.z = 0; hand->cur_rot.y = 0;
    hand->cur_quat.SetSXYZ(1,0,0,0);
    hand->Init_Rotation(); // the axis-angle, Euler, and ODE representations get updated from init_quat
    
    //-------- Creating initial joints -------

    //-------- Setting joint locations -------
    shoulder->name = name_prefix + "Shoulder";
    shoulder->joint_type = VEJoint::BALL;
    shoulder->body1 = torso; shoulder->body2 = humerus;

    elbow->name = name_prefix + "Elbow";
    elbow->joint_type = VEJoint::HINGE;
    elbow->body1 = humerus; elbow->body2 = ulna;

    wrist->name = name_prefix + "Wrist";
    wrist->joint_type = VEJoint::FIXED;
    wrist->body1 = ulna; wrist->body2 = hand;

    // the shoulder anchor is wrt to torso's CM, should lie near one corner
    // the shoulder anchor is wrt to torso's CM
    shoulder->anchor.x = alens.sh_x; 
    shoulder->anchor.y = alens.sh_y; 
    shoulder->anchor.z = alens.sh_z;

    elbow->anchor.x = 0;  // set elbow joint's anchor point wrt humerus' CM and body coordinates
    elbow->anchor.z = 0;  // the last coordinate is the long axis
    elbow->anchor.y = -(alens.humerus_mid + alens.elbow_gap_mid);

    wrist->anchor.x = 0; // set wrist joint's anchor point wrt ulna's CM and body coordinates
    wrist->anchor.z = 0; // the last coordinate is the long axis
    wrist->anchor.y = -(alens.ulna_mid + (alens.wrist_gap_mid));

    elbow->axis.x = 1;  // setting elbow joint's axes
    elbow->axis.y = 0;
    elbow->axis.z = 0;
    elbow->axis2.x = 0;
    elbow->axis2.z = 0;
    elbow->axis2.y = -1;
  }

  // start off with no initial motion
  humerus->init_lin_vel.SetXYZ(0.0f, 0.0f, 0.0f);
  humerus->init_ang_vel.SetXYZ(0.0f, 0.0f, 0.0f);

  ulna->init_lin_vel.SetXYZ(0.0f, 0.0f, 0.0f);
  ulna->init_ang_vel.SetXYZ(0.0f, 0.0f, 0.0f);

  hand->init_lin_vel.SetXYZ(0.0f, 0.0f, 0.0f);
  hand->init_ang_vel.SetXYZ(0.0f, 0.0f, 0.0f);
  
  //--------- Initializing muscles and insertion points -------------
  // the order of the next instructions is important

  InitMuscles();  // Initializes insertion point matrices and the VEMuscle_List
 
  gains.musc.SetGeom(1,n_musc); 
  SetAllMuscGains(1.0f);

  UpdateIPs(); // attaching muscles to their corresponding insertion points

  for(int i=0; i<n_musc; i++) {
    muscles[i]->InitBuffs(); //initializing the buffers which store past values of len and dlen
  }


  //----------- Setting damping --------------
  humerus->SetAngularDamping(damping.fac);
  humerus->SetAngularDampingThreshold(damping.thr);
  ulna->SetAngularDamping(damping.fac);
  ulna->SetAngularDampingThreshold(damping.thr);

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

  Worldly->Init();		// always re-init everything in environment

  return true;
}

void VEArm::SetTarget(float trg_x, float trg_y, float trg_z, 
                           bool add_gamma_noise) {
  targ_loc_abs.SetXYZ(trg_x, trg_y, trg_z);
  targ_loc_rel = targ_loc_abs - should_loc;

  if(up_axis == Z) {
    if(targ_loc_rel.x == 0.0 && targ_loc_rel.y == 0.0) {
      targ_loc_rel.y = 0.000001f;  // this is to avoid a Gimbal lock
    }
  } 
  else {  // up_axis == Y
    if(targ_loc_rel.x == 0.0 && targ_loc_rel.z == 0.0) {
      targ_loc_rel.z = 0.000001f;  // this is to avoid a Gimbal lock
    }
  } 

  targ_rel_d = targ_loc_rel.Mag();

  int retry_cnt = 0;
  while(targ_rel_d < 0.1f || targ_rel_d >= alens.Ltot) {
    if(targ_rel_d < 0.1f) {
      // moving the target away, maintaining direction
      float Lfactor = 1.1f;
      targ_loc_rel *= Lfactor;
      targ_rel_d = targ_loc_rel.Mag();
    }
    else if( targ_rel_d >= alens.Ltot) {
      // bringing the target closer, maintaining direction
      float Lfactor = 0.9f;
      targ_loc_rel *= Lfactor;
      targ_rel_d = targ_loc_rel.Mag();
    }
    retry_cnt++;
    if(retry_cnt > 100) {
      taMisc::Warning("Could not get target within proper range -- dist:", String(targ_rel_d),
                      "range = 0.1 to", String(alens.Ltot));
      break;
    }      
  }

  // From coordinates to angles as in (44)  
  float dsq = targ_rel_d * targ_rel_d;
  float lalf2 = 2.0f * alens.La * alens.Lf;
  float dla2 = 2.0f * targ_rel_d * alens.La;

  if(up_axis == Z) {
    targ_angs.elbow = taMath_float::pi - acos((alens.La*alens.La + alens.Lf*alens.Lf - dsq) / (lalf2));
    targ_angs.y = 0;
    targ_angs.x = acos(-targ_loc_rel.z/targ_rel_d) - acos((dsq + alens.La*alens.La - alens.Lf*alens.Lf)/(dla2));
    targ_angs.z = asin(-targ_loc_rel.x/sqrt(targ_loc_rel.x*targ_loc_rel.x +
                                           targ_loc_rel.y*targ_loc_rel.y));
    if(targ_loc_rel.y < 0) { // if the target is behind
      targ_angs.z = taMath_float::pi -targ_angs.z;
    }
  }
  else { // up_axis == Y
    targ_angs.elbow = taMath_float::pi - acos((alens.La*alens.La + alens.Lf*alens.Lf - dsq) / (lalf2));
    targ_angs.y = 0;
    targ_angs.x = acos(-targ_loc_rel.y/targ_rel_d) - acos((dsq + alens.La*alens.La -   alens.Lf*alens.Lf)/(dla2));
    targ_angs.z = asin(targ_loc_rel.x/sqrt(targ_loc_rel.x*targ_loc_rel.x +
                                          targ_loc_rel.z*targ_loc_rel.z));
    if(targ_loc_rel.z < 0) { // if the target is behind
      targ_angs.z = taMath_float::pi - targ_angs.z;
    }
  }

  if(add_gamma_noise) {
    // todo: this should be parameterizable
    targ_angs.y += Random::UniformMinMax(-3.14*.8, 3.14*.5);
  }

  targ_angs.UpdateAfterEdit(); // updates a,b,g

  ComputeRMatrix(targ_angs.alpha, targ_angs.beta, targ_angs.gamma);
}

void VEArm::ComputeRMatrix(float alp, float bet, float gam) {
  // Now we'll rotate the insertion points by the Euler angles in reverse order
  // This magic R matrix (from (42)) does it all in one step
  float sa = sin(alp); float ca = cos(alp);
  float sb = sin(bet);  float cb = cos(bet);
  float sc = sin(gam); float cc = cos(gam);

  float R_f[] = {ca*cc-sa*cb*sc, -ca*sc-sa*cb*cc, sa*sb,
                 sa*cc+ca*cb*sc, -sa*sc+ca*cb*cc, -ca*sb,
                 sb*sc,           sb*cc,          cb};
  R.SetGeom(2,3,3);
  R.InitFromFloats(R_f);

  if(up_axis == Y) {  // changing coordinates for R as a linear transformation
                      // taking advantage of ct being autoinverse
    float_Matrix cR = float_Matrix(2,3,3);
    taMath_float::mat_mult(&cR, &R, &ct);
    taMath_float::mat_mult(&R, &ct, &cR);
  }
}

bool VEArm::MoveToTarget(float trg_x, float trg_y, float trg_z, bool shoulder) {
  if(!CheckArm()) return false;
  
  ReConfigArm();                // start from same initial starting point!

  if(shoulder) { // coordinates are wrt a reference frame with origin at the shoulder
    trg_x = trg_x + should_loc.x;
    trg_y = trg_y + should_loc.y;
    trg_z = trg_z + should_loc.z;
  }

  VEArmAngles save_angs;
  save_angs = targ_angs;
  
  SetTarget(trg_x, trg_y, trg_z); // this updates the 4 arm angles and the R matrix
  init_angs = targ_angs;
  targ_angs = save_angs;

  return SetPose_impl();
}

bool VEArm::SetPose(float x_ang, float y_ang, float z_ang, float elbow_ang) {
  if(!CheckArm()) return false;

  ReConfigArm();                // start from same initial starting point!

  init_angs.x = x_ang; init_angs.y = y_ang; init_angs.z = z_ang;
  init_angs.elbow = elbow_ang;
  init_angs.UpdateAfterEdit();
  
  ComputeRMatrix(init_angs.alpha, init_angs.beta, init_angs.gamma);

  return SetPose_impl();
}

bool VEArm::SetPose_impl() {

  VEBody* humerus = bodies[HUMERUS];
  VEBody* ulna = bodies[ULNA];
  VEBody* hand = bodies[HAND];
  VEJoint* elbow = joints[ELBOW];
  VEJoint* wrist = joints[WRIST];
  dJointID elbow_jid = (dJointID)elbow->joint_id;
  dJointID wrist_jid = (dJointID)wrist->joint_id;
  VEWorld* ArmWorld = GetWorld();

  //------ Rotating humerus -------

  if(up_axis == Z) {
    humerus->RotateEulerZXZ(init_angs.alpha,init_angs.beta,init_angs.gamma,false,true); // an absolute rotation that only sets current values
    // humerus->RotateEuler(init_angs.beta,init_angs.gamma,init_angs.alpha,false);  
    //  humerus->RotateEuler(init_angs.alpha,init_angs.beta,init_angs.gamma,false);
    float HumCM_f[] = {0.0f,0.0f,(-alens.La+(alens.elbow_gap_mid))/2.0f};  // humerus' geometrical center at rest
    float elbow_loc_f[] = {0.0f,0.0f,-alens.La};  // elbow joint's anchor at rest
    //    float elbow_loc_f[] = {0.0f,0.0f,(-alens.La+(alens.elbow_gap))};  // elbow joint's anchor at rest
    float init_elb_axis[] = {-1.0f,0.0f,0.0f};  // elbow joint's axis at rest
    float_Matrix HumCM(2,1,3);
    float_Matrix elbow_loc(2,1,3); 
    float_Matrix elb_axis(2,1,3);
    HumCM.InitFromFloats(HumCM_f);
    elbow_loc.InitFromFloats(elbow_loc_f);
    elb_axis.InitFromFloats(init_elb_axis);
    String elblock1, elblock2, elblock3;
    elbow_loc.Print(elblock1);
    // taMisc::Info("initial elbow_loc:", elblock1, "\n");

    float_Matrix RotHumCM(2,1,3);
    float_Matrix rot_elb_loc(2,1,3);
    float_Matrix rot_elb_axis(2,1,3);
    taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center
    taMath_float::mat_mult(&rot_elb_loc, &R, &elbow_loc);
    taMath_float::mat_mult(&rot_elb_axis, &R, &elb_axis);
    
    humerus->Translate(RotHumCM.FastEl1d(0),RotHumCM.FastEl1d(1),
                       RotHumCM.FastEl1d(2)+(alens.humerus_mid),false);
  
    float UlnaCM_f[] = {0,0,-(alens.ulna_mid + alens.elbow_gap_mid)};  // Ulna 'CM' with origin at elbow
    //float Wrist_f[] = {0,0,-(alens.ulna + alens.elbow_gap_mid + alens.wrist_gap_mid)};  // wrist coords with origin at elbow
    float elbow_rot_f[] = {1 , 0, 0,
                           0, cosf(init_angs.delta), -sinf(init_angs.delta),
                           0, sinf(init_angs.delta), cosf(init_angs.delta)};
    float_Matrix UlnaCM(2,1,3);
    UlnaCM.InitFromFloats(UlnaCM_f);
    //float_Matrix Wrist(2,1,3);
    //Wrist.InitFromFloats(Wrist_f);
    float_Matrix elbow_rot(2,3,3);
    elbow_rot.InitFromFloats(elbow_rot_f);

    String erout;
    elbow_rot.Print(erout);

    float_Matrix Rot1UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);
    //float_Matrix Rot1Wrist(2,1,3);
    //taMath_float::mat_mult(&Rot1Wrist, &elbow_rot, &Wrist);

    String ruout;
    Rot1UlnaCM.Print(ruout);

    Rot1UlnaCM.Set(Rot1UlnaCM.FastEl1d(2)-alens.La,2); // setting origin at shoulder
    //Rot1Wrist.Set(Rot1Wrist.FastEl1d(2)-alens.La,2);

    String rudout;
    Rot1UlnaCM.Print(rudout);

    float_Matrix Rot2UlnaCM(2,1,3);
    taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation
    //float_Matrix Rot2Wrist(2,1,3);
    //taMath_float::mat_mult(&Rot2Wrist, &R, &Rot1Wrist);

    ulna->RotateEulerZXZ(init_angs.alpha,init_angs.beta+init_angs.delta,init_angs.gamma,false,true);  // init_angs.beta+init_angs.delta?????
    //ulna->RotateEuler(init_angs.beta+init_angs.delta,init_angs.gamma,init_angs.alpha,false);
    ulna->Translate(Rot2UlnaCM.FastEl1d(0),Rot2UlnaCM.FastEl1d(1),
                    Rot2UlnaCM.FastEl1d(2)+alens.humerus+
                    alens.ulna_mid+alens.elbow_gap,false);
    hand->RotateEulerZXZ(init_angs.alpha,init_angs.beta+init_angs.delta,init_angs.gamma,false,true);
    //hand->RotateEuler(init_angs.beta+init_angs.delta,init_angs.gamma,init_angs.alpha,false);
    // hand->Translate(trg_x,trg_y,trg_z,false,true);
    
    //------- calculating and updating the joint values --------
    // setting the axes for the elbow joint
    elbow->axis.x = rot_elb_axis.FastEl1d(0);
    elbow->axis.y = rot_elb_axis.FastEl1d(1);
    elbow->axis.z = rot_elb_axis.FastEl1d(2);
    elbow->pos = init_angs.delta;
  } // up_axis == Z

  /////////////////////////////////////////////////////////////////////////////
  // up_axis == Y
  else {

    float HumCM_f[] = {0.0f,0.5f * (-alens.La+alens.elbow_gap_mid), 0.0f};
    // humerus' geometrical center at rest
    float elbow_loc_f[] = {0.0f,-alens.La, 0.0f};
    //    float elbow_loc_f[] = {0.0f,-alens.La+alens.elbow_gap_mid, 0.0f};
    // elbow joint's anchor at rest
    float init_elb_axis[] = {-1.0f,0.0f,0.0f};  // elbow joint's axis at rest
    
    float_Matrix HumCM(2,1,3);
    float_Matrix elbow_loc(2,1,3);
    float_Matrix elb_axis(2,1,3);
    HumCM.InitFromFloats(HumCM_f);
    elbow_loc.InitFromFloats(elbow_loc_f);
    elb_axis.InitFromFloats(init_elb_axis);
    
    String elblock1, elblock2, elblock3;
    elbow_loc.Print(elblock1);
    // taMisc::Info("initial elbow_loc:", elblock1, "\n");
    
    float_Matrix RotHumCM(2,1,3);
    float_Matrix rot_elb_loc(2,1,3);
    float_Matrix rot_elb_axis(2,1,3);
    taMath_float::mat_mult(&RotHumCM, &R, &HumCM);  // rotating geometrical center
    taMath_float::mat_mult(&rot_elb_loc, &R, &elbow_loc);
    taMath_float::mat_mult(&rot_elb_axis, &R, &elb_axis);    

    humerus->Translate(RotHumCM.FastEl1d(0),RotHumCM.FastEl1d(1)+(alens.humerus_mid),
                       RotHumCM.FastEl1d(2),false); // cur, not init

    // this version shows that beta is x axis, gamma is y, and alpha is z
    //    humerus->RotateEuler(-init_angs.beta,init_angs.gamma,init_angs.alpha,false); // NOT init
    humerus->RotateEulerYXY(init_angs.alpha,-init_angs.beta,init_angs.gamma,false); // NOT init
    // humerus->RotateEulerZXZ(taMath_float::pi + init_angs.alpha,init_angs.beta,init_angs.gamma,false,true);
    // humerus->RotateEuler(init_angs.beta,init_angs.gamma,taMath_float::pi + init_angs.alpha,false);

    taMath_float::mat_mult(&elbow_loc, &R, &elbow_loc);
 
    float UlnaCM_f[] = {0,-(alens.ulna_mid + alens.elbow_gap_mid),0};  // Ulna 'CM' with origin at elbow
    float handCM_f[] = {0,-alens.Lf,0};  // Hand 'CM' with origin at elbow
    float elbow_rot_f[] = {1 , 0, 0,
                  0, cosf(init_angs.delta),  sinf(init_angs.delta),
                  0, -sinf(init_angs.delta), cosf(init_angs.delta)};
    float_Matrix UlnaCM(2,1,3);
    UlnaCM.InitFromFloats(UlnaCM_f);
    float_Matrix handCM(2,1,3);
    handCM.InitFromFloats(handCM_f);
    float_Matrix elbow_rot(2,3,3);
    elbow_rot.InitFromFloats(elbow_rot_f);

    String erout;
    elbow_rot.Print(erout);
    // taMisc::Info("elbow rotation matrix :\n", erout);

    float_Matrix Rot1UlnaCM(2,1,3);
    float_Matrix Rot1handCM(2,1,3);
    taMath_float::mat_mult(&Rot1UlnaCM, &elbow_rot, &UlnaCM);
    taMath_float::mat_mult(&Rot1handCM, &elbow_rot, &handCM);

    String ruout;
    Rot1UlnaCM.Print(ruout);
    // taMisc::Info("rotated ulna before translation:\n", ruout);

    Rot1UlnaCM.Set(Rot1UlnaCM.FastEl1d(1)-alens.La,1); // setting origin at shoulder
    Rot1handCM.Set(Rot1handCM.FastEl1d(1)-alens.La,1); // setting origin at shoulder

    String rudout;
    Rot1UlnaCM.Print(rudout);
    // taMisc::Info("rotated ulna after translation:\n", rudout);

    float_Matrix Rot2UlnaCM(2,1,3);
    float_Matrix Rot2handCM(2,1,3);
    taMath_float::mat_mult(&Rot2UlnaCM, &R, &Rot1UlnaCM); // applying shoulder rotation
    taMath_float::mat_mult(&Rot2handCM, &R, &Rot1handCM); // applying shoulder rotation

    ulna->Translate(Rot2UlnaCM.FastEl1d(0),
                    Rot2UlnaCM.FastEl1d(1)+alens.humerus+alens.elbow_gap+
                    alens.ulna_mid,Rot2UlnaCM.FastEl1d(2),false); // NOT init

    //ulna->RotateEuler(-(init_angs.beta+init_angs.delta),init_angs.gamma,init_angs.alpha, false); // NOT init
    ulna->RotateEulerYXY(0,-init_angs.delta,0, false); // NOT init
    ulna->RotateEulerYXY(init_angs.alpha,-init_angs.beta,init_angs.gamma, false); // NOT init
    // ulna->RotateEulerZXZ(taMath_float::pi+init_angs.alpha,init_angs.beta+init_angs.delta,init_angs.gamma,false,true);
    // ulna->RotateEuler(init_angs.beta+init_angs.delta,init_angs.gamma,taMath_float::pi+init_angs.alpha,false);

    hand->Translate(Rot2handCM.FastEl1d(0)+should_loc.x,
                    Rot2handCM.FastEl1d(1)+should_loc.y,
                    Rot2handCM.FastEl1d(2)+should_loc.z,false,true); // NOT init, absolute position

    //hand->RotateEuler(-(init_angs.beta+init_angs.delta),-init_angs.gamma,init_angs.alpha, false); // NOT init
    hand->RotateEulerYXY(0,-init_angs.delta,0,false);
    hand->RotateEulerYXY(init_angs.alpha,-init_angs.beta,init_angs.gamma,false);
    // hand->RotateEuler(init_angs.beta+init_angs.delta,init_angs.gamma,taMath_float::pi+init_angs.alpha,false);
    // hand->Translate(trg_x, trg_y+(alens.humerus+alens.ulna+alens.elbow_gap+
    //                 alens.wrist_gap+(hand->length_mid)),trg_z,false);

    //-------- calculating and updating the joint values --------
    // setting the axes for the elbow joint
    elbow->pos = init_angs.delta;
    elbow->axis.x = rot_elb_axis.FastEl1d(0);
    elbow->axis.y = rot_elb_axis.FastEl1d(1);
    elbow->axis.z = rot_elb_axis.FastEl1d(2);
  } // up_axis == Y

  // VERY IMPORTANT:
  // do NOT set init values -- this is the problem - fixes the joint in the wrong place!!
  CurToODE();

  //------- updating the muscle insertion points and lengths ---------
  UpdateIPs();
  for(int i=0; i<n_musc; i++) {
    muscles[i]->InitBuffs(); //initializing the buffers which store past values of len and dlen
    }
  Lengths(lens,false);  // updating muscle lengths. false = don't normalize. Probably unnecessary
  Speeds(vels,false);  // updating muscle speeds
  InitDynamicState();  // Initialize all dynamic variables 
  SigEmitUpdated(); // this will in theory update the display
  reach_start = true; // this flag indicates that a new reach is to begin

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

  float tot_len = alens.Ltot;
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
  if(!CheckArm()) return false;

  SetTarget(trg_x, trg_y, trg_z);
  bool rval = AngToLengths(targ_lens,targ_angs.alpha, targ_angs.beta,
                           targ_angs.gamma, targ_angs.delta);
  norm_targ_lens = targ_lens;
  norm_targ_lens -= min_lens;
  norm_targ_lens *= spans;
  return rval;
}

bool VEArm::NoisyTargetLengths(float trg_x, float trg_y, float trg_z) {
  if(!CheckArm()) return false;

  SetTarget(trg_x, trg_y, trg_z, true); // noisy
  bool rval = AngToLengths(targ_lens,targ_angs.alpha, targ_angs.beta,
                           targ_angs.gamma, targ_angs.delta);
  norm_targ_lens = targ_lens;
  norm_targ_lens -= min_lens;
  norm_targ_lens *= spans;
  return rval;
}

bool VEArm::AngToLengths(float_Matrix &tlens, float alpha, float beta, float gamma,
                         float delta) {
  tlens.SetGeom(1,n_musc);     // always set to be more robust
  if(!CheckArm()) return false;
  
  ComputeRMatrix(alpha,beta,gamma);
  float_Matrix RT(2,3,3); // the transpose of R
  taMath_float::mat_transpose(&RT, &R);
  
  // rotating the humerus' insertion points
  float_Matrix RotArmIP;
  taMath_float::mat_mult(&RotArmIP, &ArmIP, &RT);

  // rotating the ulna's insertion points
  float UlnaShift_f[9] = {0.0f}; // initializes all zeros
  float T_elbowRot_f[9] = {0.0f};

  if(up_axis == Z) {
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, 0, -alens.La,
                           0, 0, -alens.La,
                           0, 0, -alens.La}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0
                          0, cos(delta),  sin(delta),
                          0, -sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation
    */
    UlnaShift_f[2] = UlnaShift_f[5] = UlnaShift_f[8] = -alens.La;
    T_elbowRot_f[0] = 1;
    T_elbowRot_f[4] = T_elbowRot_f[8] = cos(delta);
    T_elbowRot_f[5] = sin(delta);
    T_elbowRot_f[7] = -sin(delta);
  }
  else { // up_axis == Y 
    /* The matrices we're defining are these:
    float UlnaShift_f[] = {0, -alens.La, 0,
                           0, -alens.La, 0,
                           0, -alens.La, 0}; // should have one row per forearm IP
    float T_elbowRot_f[] = {1 , 0, 0,                     
                      0, cos(delta), -sin(delta),
                      0, sin(delta), cos(delta)};
    // T_elbowRot is the TRANSPOSED rotation matrix for the delta rotation after the ct bilateral multiplication
    */
    UlnaShift_f[1] = UlnaShift_f[4] = UlnaShift_f[7] = -alens.La;
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

  // now we set the origin at the shoulder
  float_Matrix ReshiftedIP(2,3,3);
  ReshiftedIP = Rot1FarmIP + UlnaShift;

  // finally we apply the shoulder rotation
  float_Matrix RotFarmIP(2,3,3);
  taMath_float::mat_mult(&RotFarmIP, &ReshiftedIP, &RT);

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
      tlens.Set(c1.Mag()+c2.Mag(),i);
    }
    else {
      c1 = shoulderIP - humerIP;
      tlens.Set(c1.Mag(),i);
    }
  }
  // next muscle is the biceps, from shoulder to forearm
  c1.x = ShouldIP.FastElAsFloat(0,8+k) - RotFarmIP.FastElAsFloat(0,0);
  c1.y = ShouldIP.FastElAsFloat(1,8+k) - RotFarmIP.FastElAsFloat(1,0);
  c1.z = ShouldIP.FastElAsFloat(2,8+k) - RotFarmIP.FastElAsFloat(2,0);
  tlens.Set(c1.Mag(),8+k);
  // the triceps and the brachialis connect from humerus to ulna
  for(int i=1; i<=2; i++) {
    c1.x = RotArmIP.FastElAsFloat(0,7+k+i) - RotFarmIP.FastElAsFloat(0,i);
    c1.y = RotArmIP.FastElAsFloat(1,7+k+i) - RotFarmIP.FastElAsFloat(1,i);
    c1.z = RotArmIP.FastElAsFloat(2,7+k+i) - RotFarmIP.FastElAsFloat(2,i);
    tlens.Set(c1.Mag(),8+k+i);
  }

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
      vel.FastEl1d(i) = 1.0f / (1.0f + expf(-(gains.vel_norm) * vel.FastEl1d(i)));
    }
  }
  return true;
}

void VEArm::SetReachStartFlag(bool flag) {
  reach_start = flag; // true if network.cycle = 0, else false
}

bool VEArm::ApplyStim(const float_Matrix& stm, float_Matrix &fs, bool flip_sign) {
  if(delays.eff > 1) {
    if(reach_start) { // start of a new reach, need to reset the buffer table
      stims_delay.Clear(); // sets all values to zero
      stims_delay.SetGeom(2, n_musc, delays.eff - 1); // default geometry -- delays.eff-1 rows, n_musc columns
    }
    
    stims_delay.AddFrame(); // since it's 2-dimensional, adds one blank row
    int last_row = stims_delay.Frames() - 1; // this is the row where the current stims are stored (last row)
    int delay_row = stims_delay.Frames() - delays.eff; // this is the row where the delayed stims are stored (delays.eff rows up)

    stims_delay.CopyFrame(stm, last_row); // copy the current stims into the last row
    float_Matrix* stmD = stims_delay.GetFrameSlice(delay_row); // get the delayed stims as a matrix with the same geometry as stm

    // now we run the usual ApplyStim code, replacing 'stm' with 'stmD':
    if(TestWarning(stmD->count() != n_musc, "","The delayed stimulus matrix doesn't match the number of muscles \n"))
      return false;
    fs.SetGeom(2, 3, n_musc);     // this is fast if it matches already

    VEBody* humerus = bodies[HUMERUS];
    VEBody* ulna = bodies[ULNA];

    int k = 0;
    if(musc_geo == NEW_GEO) k = 1; // offset to change assignments below

    taVector3f daforce(0.0f, 0.0f, 0.0f);
    for(int i=0; i<n_musc; i++) {
      if(flip_sign) {
        daforce = muscles[i]->Contract(-(gains.stim) * stmD->FastElAsFloat(i));
      }
      else {
        daforce = muscles[i]->Contract(gains.stim * stmD->FastElAsFloat(i));
      }
      fs.Set(daforce.x, 0, i);
      fs.Set(daforce.y, 1, i);
      fs.Set(daforce.z, 2, i);

      if(i < 8+k) {  // muscles from shoulder to humerus
        humerus->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                               should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                               muscles[i]->IPdist.z + should_loc.z,false,false);
      }
      else if(i == 8+k) {  // biceps
        ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                            should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                            muscles[i]->IPdist.z + should_loc.z,false,false);
      }
      else { // triceps and brachialis
        humerus->AddForceAtPos(-daforce.x,-daforce.y,-daforce.z, muscles[i]->IPprox.x +
                               should_loc.x, muscles[i]->IPprox.y + should_loc.y,
                               muscles[i]->IPprox.z + should_loc.z,false,false);
        ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                            should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                            muscles[i]->IPdist.z + should_loc.z,false,false);
      }
    }
  }
  else { // no effector delay, run original code:
    if(TestWarning(stm.count() != n_musc, "","The stimulus matrix doesn't match the number of muscles \n"))
      return false;
    fs.SetGeom(2, 3, n_musc);     // this is fast if it matches already

    VEBody* humerus = bodies[HUMERUS];
    VEBody* ulna = bodies[ULNA];

    int k = 0;
    if(musc_geo == NEW_GEO) k = 1; // offset to change assignments below

    taVector3f daforce(0.0f, 0.0f, 0.0f);
    for(int i=0; i<n_musc; i++) {
      if(flip_sign) {
        daforce = muscles[i]->Contract(-(gains.stim) * stm.FastElAsFloat(i));
      }
      else {
        daforce = muscles[i]->Contract(gains.stim * stm.FastElAsFloat(i));
      }
      fs.Set(daforce.x, 0, i);
      fs.Set(daforce.y, 1, i);
      fs.Set(daforce.z, 2, i);

      if(i < 8+k) {  // muscles from shoulder to humerus
        humerus->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                               should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                               muscles[i]->IPdist.z + should_loc.z,false,false);
      }
      else if(i == 8+k) {  // biceps
        ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                            should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                            muscles[i]->IPdist.z + should_loc.z,false,false);
      }
      else { // triceps and brachialis
        humerus->AddForceAtPos(-daforce.x,-daforce.y,-daforce.z, muscles[i]->IPprox.x +
                               should_loc.x, muscles[i]->IPprox.y + should_loc.y,
                               muscles[i]->IPprox.z + should_loc.z,false,false);
        ulna->AddForceAtPos(daforce.x,daforce.y,daforce.z, muscles[i]->IPdist.x +
                            should_loc.x, muscles[i]->IPdist.y + should_loc.y,
                            muscles[i]->IPdist.z + should_loc.z,false,false);
      }
    }
  }
  return true;
}

bool VEArm::SetMuscGains(const float_Matrix& new_gains) {
  gains.musc.SetGeom(1,n_musc); // should be but justin
  int mx = MIN(gains.musc.size, new_gains.size);
  for(int i=0; i<mx; i++) {
    gains.musc.FastEl_Flat(i) = new_gains.FastEl_Flat(i);
  }
  return true;
}

bool VEArm::SetAllMuscGains(float all_gain) {
  gains.musc.SetGeom(1,n_musc); // should be but justin
  for(int i=0; i<n_musc; i++) {
    gains.musc.FastEl_Flat(i) = all_gain;
  }
  return true;
}

float VEArm::SetMuscGain(int musc_no, float gn) {
  gains.musc.SetGeom(1,n_musc); // should be but justin
  if(musc_no < n_musc) {
    gains.musc.FastEl_Flat(musc_no) = gn;
    return gains.musc.FastEl_Flat(musc_no);
  }
  return 0.0f;
}

float VEArm::IncrMuscGain(int musc_no, float gn_inc) {
  gains.musc.SetGeom(1,n_musc); // should be but justin
  if(musc_no < n_musc) {
    gains.musc.FastEl_Flat(musc_no) += gn_inc;
    return gains.musc.FastEl_Flat(musc_no);
  }
  return 0.0f;
}

float VEArm::DecayMuscGain(int musc_no, float decay) {
  gains.musc.SetGeom(1,n_musc); // should be but justin
  if(musc_no < n_musc) {
    float& mg = gains.musc.FastEl_Flat(musc_no);
    mg += decay * (1.0f - mg);
    return mg;
  }
  return 0.0f;
}

bool VEArm::ComputeStim() {
  // Do one step of reaching using the velocity-controlled Equilibrium Point algorithm. 
  // This will calculate the activation (multiplying both errors by the gain), calculate 
  // (and store) the resulting fs, and apply them.
  // It does not take a step of the VEWorld, and does not udpate the muscle insertion points.

  if(TestWarning(targ_lens.count() != n_musc, "","The targets matrix doesn't match the number of muscles \n"))
    return false;

  // SetGeom is fast if already in that geom
  lens.SetGeom(1,n_musc);
  vels.SetGeom(1, n_musc);
  if(musc_type == LINEAR) {
    Lengths(lens, false);
    Speeds(vels, false);
  }
  else { // musc_type == HILL
    for(int i=0; i<n_musc; i++) {
      lens.Set(muscles[i]->Old_Length(),i); 
      vels.Set(muscles[i]->Old_Speed(),i); 
    }
  }

  err_len.SetGeom(1,n_musc);
  err_len = targ_lens - lens;       // most need this anyway
  err_itg.SetGeom(1,n_musc);
  err_drv.SetGeom(1,n_musc);
  err_drv_dra.SetGeom(1,n_musc);
  err_len_prv.SetGeom(1,n_musc);
  stims.SetGeom(1,n_musc);
  stims_p.SetGeom(1,n_musc);
  stims_i.SetGeom(1,n_musc);
  stims_d.SetGeom(1,n_musc);
  forces.SetGeom(2, 3, n_musc);

  bool rval = true;
  switch(ctrl_type) {
  case PID:
    rval = ComputeStim_PID();
    break;
  case ERR_VEL:
    rval = ComputeStim_EV();
    break;
  }

  targ_lens_mag = taMath_float::vec_norm(&targ_lens);
  lens_mag = taMath_float::vec_norm(&lens);
  vels_mag = taMath_float::vec_norm(&vels);
  err_len_mag = taMath_float::vec_norm(&(err_len));
  stims_mag = taMath_float::vec_norm(&stims);
  gains.musc_mag = taMath_float::vec_norm(&(gains.musc));

  return rval;
}

bool VEArm::ComputeStim_PID() {
  err_itg += err_len * world_step;
  if(arm_time > 2.0f * world_step) {
    err_drv = err_len - err_len_prv;
    err_drv /= world_step;
  }
  const float dtc = (1.0f - errors.pid_dra_dt);
  err_drv_dra *= dtc;
  err_drv_dra += err_drv * errors.pid_dra_dt;
  if(errors.max > 0.0f) {
    for(int i=0; i<n_musc; i++) {
      float er = err_len.FastEl1d(i);
      if(er > errors.max) er = errors.max;
      else if(er < -(errors.max)) er = -(errors.max);
      stims_p.FastEl1d(i) = er * gains.p;
    }
  }
  else {
    stims_p = err_len * gains.p;
  }
  stims_i = err_itg * gains.i;
  stims_d = err_drv_dra * gains.d;
  stims = stims_p; stims += stims_i; stims += stims_d;
  err_len_prv = err_len;

  if(gains.musc.size == n_musc)
    stims *= gains.musc;  // muscle specific gain on final stimulation
  ApplyStim(stims, forces);

  err_itg_mag = taMath_float::vec_norm(&(err_itg));
  err_drv_dra_mag = taMath_float::vec_norm(&(err_drv_dra));
  stims_p_mag = taMath_float::vec_norm(&stims_p);
  stims_i_mag = taMath_float::vec_norm(&stims_i);
  stims_d_mag = taMath_float::vec_norm(&stims_d);

  return true;
}

bool VEArm::ComputeStim_EV() {
  err_len *= gains.ev;
  stims = err_len - vels;
  stims *= gains.ev;
  if(musc_type == HILL) {
    stims += vels * hill_mu;
  }
  if(gains.musc.size == n_musc)
    stims *= gains.musc;  // muscle specific gain on final stimulation
  ApplyStim(stims, forces, true); // flip sign
  return true;
}

void VEArm::FormatLogData(DataTable& dt) {
  DataCol* dc;
  dc = dt.FindMakeCol("time", VT_FLOAT);
  dc->desc = "current time step of physics";

  dc = dt.FindMakeCol("targ_x", VT_FLOAT);
  dc->desc = "target location x coordinate -- relative coords";
  dc = dt.FindMakeCol("targ_y", VT_FLOAT);
  dc->desc = "target location y coordinate -- relative coords";
  dc = dt.FindMakeCol("targ_z", VT_FLOAT);
  dc->desc = "target location z coordinate -- relative coords";

  dc = dt.FindMakeCol("hand_x", VT_FLOAT);
  dc->desc = "hand location x coordinate -- relative coords";
  dc = dt.FindMakeCol("hand_y", VT_FLOAT);
  dc->desc = "hand location y coordinate -- relative coords";
  dc = dt.FindMakeCol("hand_z", VT_FLOAT);
  dc->desc = "hand location z coordinate -- relative coords";
  dc = dt.FindMakeCol("hand_vra", VT_FLOAT);
  dc->desc = "hand velocity magnitude (speed) running average -- use to determine when reach has stopped";

  dc = dt.FindMakeCol("loc_err_mag", VT_FLOAT);
  dc->desc = "magnitude of location error -- distance of hand away from target";

  dc = dt.FindMakeCol("lens_mag", VT_FLOAT);
  dc->desc = "total length of the muscles";
  dc = dt.FindMakeCol("vels_mag", VT_FLOAT);
  dc->desc = "total speed of the muscles";

  dc = dt.FindMakeCol("err_mag", VT_FLOAT);
  dc->desc = "total magnitude of the target - current muscle length error";
  dc = dt.FindMakeCol("err_int_mag", VT_FLOAT);
  dc->desc = "total magnitude of the integrated error, drives the I in PID control";
  dc = dt.FindMakeCol("err_dra_mag", VT_FLOAT);
  dc->desc = "total magnitude of the error derivative running average, drives the D in PID control";
  dc = dt.FindMakeCol("stims_mag", VT_FLOAT);
  dc->desc = "total muscle stimulation magnitude -- total amount of muscle control exerted";
  dc = dt.FindMakeCol("stims_p_mag", VT_FLOAT);
  dc->desc = "total proportional stimulation, P in PID";
  dc = dt.FindMakeCol("stims_i_mag", VT_FLOAT);
  dc->desc = "total integrated error stimulation, I in PID";
  dc = dt.FindMakeCol("stims_d_mag", VT_FLOAT);
  dc->desc = "total error derivative stimulation, D in PID";

  dc = dt.FindMakeCol("musc_gains_mag", VT_FLOAT);
  dc->desc = "muscle gains magnitude -- this is how the cerebellum modulates the motion";

  dc = dt.FindMakeCol("io_err_mag", VT_FLOAT);
  dc->desc = "inferior olive err magnitude -- how much error is occuring now";

  // dc = dt.FindMakeCol("", VT_FLOAT);
  // dc->desc = "";

}

void VEArm::LogArmData(DataTable& dt) {
  dt.AddBlankRow();
  DataCol* dc;
  dc = dt.FindMakeCol("time", VT_FLOAT);
  dc->SetVal(arm_time, -1);

  dc = dt.FindMakeCol("targ_x", VT_FLOAT);
  dc->SetVal(targ_loc_rel.x, -1);
  dc = dt.FindMakeCol("targ_y", VT_FLOAT);
  dc->SetVal(targ_loc_rel.y, -1);
  dc = dt.FindMakeCol("targ_z", VT_FLOAT);
  dc->SetVal(targ_loc_rel.z, -1);

  dc = dt.FindMakeCol("hand_x", VT_FLOAT);
  dc->SetVal(hand_loc_cur.x, -1);
  dc = dt.FindMakeCol("hand_y", VT_FLOAT);
  dc->SetVal(hand_loc_cur.y, -1);
  dc = dt.FindMakeCol("hand_z", VT_FLOAT);
  dc->SetVal(hand_loc_cur.z, -1);

  dc = dt.FindMakeCol("hand_vel_mag", VT_FLOAT);
  dc->SetVal(hand_vel_mag, -1);

  dc = dt.FindMakeCol("hand_vra", VT_FLOAT);
  dc->SetVal(hand_vra, -1);

  dc = dt.FindMakeCol("loc_err_mag", VT_FLOAT);
  dc->SetVal(err_loc_cur_mag, -1);

  dc = dt.FindMakeCol("rot_err", VT_FLOAT);
  dc->SetVal(0.0f, -1);

  dc = dt.FindMakeCol("lens_mag", VT_FLOAT);
  dc->SetVal(lens_mag, -1);

  dc = dt.FindMakeCol("vels_mag", VT_FLOAT);
  dc->SetVal(vels_mag, -1);

  dc = dt.FindMakeCol("err_mag", VT_FLOAT);
  dc->SetVal(err_len_mag, -1);

  dc = dt.FindMakeCol("err_int_mag", VT_FLOAT);
  dc->SetVal(err_itg_mag, -1);

  dc = dt.FindMakeCol("err_dra_mag", VT_FLOAT);
  dc->SetVal(err_drv_dra_mag, -1);

  dc = dt.FindMakeCol("stims_mag", VT_FLOAT);
  dc->SetVal(stims_mag, -1);

  dc = dt.FindMakeCol("stims_p_mag", VT_FLOAT);
  dc->SetVal(stims_p_mag, -1);

  dc = dt.FindMakeCol("stims_i_mag", VT_FLOAT);
  dc->SetVal(stims_i_mag, -1);

  dc = dt.FindMakeCol("stims_d_mag", VT_FLOAT);
  dc->SetVal(stims_d_mag, -1);

  dc = dt.FindMakeCol("musc_gains_mag", VT_FLOAT);
  dc->SetVal(gains.musc_mag, -1);

  dc = dt.FindMakeCol("io_err_mag", VT_FLOAT);
  dc->SetVal(err_io_mag, -1);

  // dc = dt.FindMakeCol("", VT_FLOAT);
  //  dc->SetVal(, -1);

  dt.WriteClose();
}

bool VEArm::InitDelayedInputsToTable() {
  const char col_name_0[] = "lengths";
  const char col_name_1[] = "targ_lengths";
  const char col_name_2[] = "norm_err_dra";
  const char col_name_3[] = "io_err";
  const char col_name_4[] = "speeds";
  const char col_name_5[] = "hand_coords";

  DataCol* dc_l = arm_state->FindMakeColMatrix(col_name_0, VT_FLOAT, 4, 1,1,1,n_musc);
  DataCol* dc_t = arm_state->FindMakeColMatrix(col_name_1, VT_FLOAT, 4, 1,1,1,n_musc);
  DataCol* dc_n = arm_state->FindMakeColMatrix(col_name_2, VT_FLOAT, 4, 1,1,1,n_musc);
  DataCol* dc_i = arm_state->FindMakeColMatrix(col_name_3, VT_FLOAT, 4, 1,1,1,n_musc);
  DataCol* dc_s = arm_state->FindMakeColMatrix(col_name_4, VT_FLOAT, 4, 1,1,1,n_musc);
  DataCol* dc_h = arm_state->FindMakeColMatrix(col_name_5, VT_FLOAT, 4, 1,1,1,3);

  VEBody* hand = bodies[HAND];
  float maxl = alens.Ltot;

  if(arm_state->rows == 0)
    arm_state->EnforceRows(1);

  if(arm_state->rows == 1) {
    int n = 1;
    // start with one row & no data
    for(n=1; n < delays.pro; n++) {
      for(int i=0; i<n_musc; i++) {
        dc_l->SetMatrixVal(norm_lens.SafeEl(i),-1,0,0,0,i); // these will correspond to resting muscle lengths, since this method is called before the reach begins
        dc_t->SetMatrixVal(norm_targ_lens.SafeEl(i),-1,0,0,0,i); // these are currently constant (will change once we have a moving target)
        dc_n->SetMatrixVal(err_len_norm_dra.SafeEl(i),-1,0,0,0,i); // these are calculated using the delayed inputs from this table
        dc_i->SetMatrixVal(err_io.SafeEl(i),-1,0,0,0,i); // these are calculated using delayed inputs
        dc_s->SetMatrixVal(0.0f,-1,0,0,0,i); // the hand is at rest, so the muscle speeds should all be zero
      }
      if(up_axis == Y) { // these correspond to a hand at rest, since this table is populated before the start of a reach
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.y-should_loc.y)/(1.9f*maxl),-1, 0,0,0,1);
        dc_h->SetMatrixVal(0.1f+(hand->cur_pos.z-should_loc.z)/(maxl),-1, 0,0,0,2);
      }
      else { // up_axis == Z
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
        dc_h->SetMatrixVal(0.1f+(hand->cur_pos.y-should_loc.y)/(maxl),-1, 0,0,0,1);
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.z-should_loc.z)/(1.9f*maxl),-1, 0,0,0,2);
      }
      arm_state->AddBlankRow();
    }
    // now we should have (delays.pro - 1) rows of data, one blank row, and index n = delays.pro
    for(; n < delays.vis; n++) {
      for(int i=0; i<n_musc; i++) {
        dc_l->SetMatrixVal(norm_lens.SafeEl(i),-1,0,0,0,i);
        dc_t->SetMatrixVal(norm_targ_lens.SafeEl(i),-1,0,0,0,i);
        dc_n->SetMatrixVal(err_len_norm_dra.SafeEl(i),-1,0,0,0,i);
        dc_i->SetMatrixVal(err_io.SafeEl(i),-1,0,0,0,i);
        dc_s->SetMatrixVal(norm_vels.SafeEl(i),-1,0,0,0,i); // this is the only change -- start giving the system current muscle velocities at the (delays.pro) row
      }
      if(up_axis == Y) {
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.y-should_loc.y)/(1.9f*maxl),-1, 0,0,0,1);
        dc_h->SetMatrixVal(0.1f+(hand->cur_pos.z-should_loc.z)/(maxl),-1, 0,0,0,2);
      }
      else {
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
        dc_h->SetMatrixVal(0.1f+(hand->cur_pos.y-should_loc.y)/(maxl),-1, 0,0,0,1);
        dc_h->SetMatrixVal(0.5f+(hand->cur_pos.z-should_loc.z)/(1.9f*maxl),-1, 0,0,0,2);
      }
      arm_state->AddBlankRow();
    }
    // now we should have (delays.vis - 1) rows of data and one blank row!
  } // if delays.pro = delays.vis = 1, we should have just one blank row
  return true;
}

bool VEArm::NormLengthsToTable() {
  const char col_name[] = "lengths";
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  int d_row = delays.vis - delays.pro + 1;
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(0.0f,-1, // -1 = last row (most recent)
                     0,0,0,i);
    dc->SetMatrixVal(norm_lens.SafeEl(i),-d_row, // -d_row = row corresponding to delays.pro
                     0,0,0,i);                   // if delays.pro = delays.vis = 1, this will simply
  }                                              // overwrite the last row of arm_state w/ current vals
  return true;
}

bool VEArm::NormTargLengthsToTable() {
  const char col_name[] = "targ_lengths"; 
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(norm_targ_lens.SafeEl(i),-1, // -1 = last row
                     0,0,0,i);
  }
  return true;
}

bool VEArm::NormSpeedsToTable() {
  const char col_name[] = "speeds"; 
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  int d_row = delays.vis - delays.pro + 1;
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(0.0f,-1, // -1 = last row
                     0,0,0,i);
    dc->SetMatrixVal(norm_vels.SafeEl(i),-d_row, // -d_row = row corresponding to delays.pro
                     0,0,0,i);                   // if delays.pro = delays.vis = 1, this will simply
  }                                              // overwrite the last row of arm_state w/ current vals
  return true;
}

bool VEArm::NormHandCoordsToTable() {
  const char col_name[] = "hand_coords"; 
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,3);
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  
  VEBody* hand = bodies[HAND];
  float maxl = alens.Ltot;

  if(up_axis == Y) {
    // -1 = last row
    dc->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
    dc->SetMatrixVal(0.5f+(hand->cur_pos.y-should_loc.y)/(1.9f*maxl),-1, 0,0,0,1);
    dc->SetMatrixVal(0.1f+(hand->cur_pos.z-should_loc.z)/(maxl),-1, 0,0,0,2);
  }
  else {
    dc->SetMatrixVal(0.5f+(hand->cur_pos.x-should_loc.x)/(1.9f*maxl),-1, 0,0,0,0);
    dc->SetMatrixVal(0.1f+(hand->cur_pos.y-should_loc.y)/(maxl),-1, 0,0,0,1);
    dc->SetMatrixVal(0.5f+(hand->cur_pos.z-should_loc.z)/(1.9f*maxl),-1, 0,0,0,2);
  } 
  return true;
}

bool VEArm::NormErrDraToTable() {
  const char col_name[] = "norm_err_dra"; 
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(err_len_norm_dra.SafeEl(i),-1, // -1 = last row
                     0,0,0,i);
  }
  return true;
}

bool VEArm::IOErrToTable() {
  const char col_name[] = "io_err"; 
  DataCol* dc = arm_state->FindMakeColMatrix(col_name, VT_FLOAT, 4, 1,1,1,n_musc);
  if(arm_state->rows == 0)       // empty table, make sure we have at least 1 row
    arm_state->EnforceRows(1);
  
  for(int i=0; i<n_musc; i++) {
    dc->SetMatrixVal(err_io.SafeEl(i),-1, // -1 = last row
                     0,0,0,i);
  }
  return true;
}

bool VEArm::ArmStateToTable() {
  NormLengthsToTable();
  NormTargLengthsToTable();
  NormSpeedsToTable();
  return true;
}

bool VEArm::SetTargetLengthsFmTable() {
  const char col_name[] = "lengths";
  DataCol* dc = arm_state->FindColName(col_name, true); // find the "lengths" column, error msg if not found
  MatrixGeom std_geom1(4,12,1,1,n_musc);
  MatrixGeom std_geom2(4,22,1,1,n_musc);

  if(TestError((dc->cell_geom != std_geom1) && (dc->cell_geom != std_geom2), "SetTargetLengthsFmTable","The geometry of the table provided to SetTargetLengthsFmTable() is not standard\n"))
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
  ComputeStim();
  arm_time += world_step;
  inherited::Step_pre();
}

void VEArm::CurFromODE(bool updt_disp) {
  inherited::CurFromODE(updt_disp);
  UpdateIPs();
  CurToODE();
}

bool VEArm::GetNormVals() {
  norm_lens.SetGeom(1,n_musc);
  norm_targ_lens.SetGeom(1,n_musc);
  norm_vels.SetGeom(1,n_musc);
  err_len_norm.SetGeom(1,n_musc);
  err_len_norm_dt.SetGeom(1,n_musc);
  err_len_norm_dra.SetGeom(1,n_musc);
  err_len_norm_prv.SetGeom(1,n_musc);
  err_io.SetGeom(1,n_musc);

  VEBody* hand = bodies[HAND];

  if(reach_start) {
    hand_loc_abs = hand->cur_pos;
    hand_loc_rel = hand_loc_abs - should_loc;
    hand_loc_cur = hand_loc_rel;
  }
  else {
    taMatrixPtr mat(arm_state->GetValAsMatrix("hand_coords", -(delays.vis)));
    hand_loc_rel.FromMatrix(*mat);
    hand_loc_abs = hand->cur_pos;
    hand_loc_cur = hand_loc_abs - should_loc;
  }

  err_loc = targ_loc_rel - hand_loc_rel;
  err_loc_cur = targ_loc_rel - hand_loc_cur;
  err_loc_mag = err_loc.Mag();
  err_loc_cur_mag = err_loc_cur.Mag();

  hand_vel = hand_loc_rel - hand_loc_prv;
  hand_vel_mag = hand_vel.Mag();
  hand_vra = (1.0f - errors.hand_vra_dt) * hand_vra + errors.hand_vra_dt * hand_vel_mag;
  hand_loc_prv = hand_loc_rel;

  Lengths(norm_lens, true); // true = normalize
  Speeds(norm_vels, true); // true = normalize

  if(reach_start) {
    err_len_norm = norm_targ_lens - norm_lens;
  }
  else {
    taMatrixPtr mat(arm_state->GetValAsMatrix("lengths", -(delays.vis)));
    err_len_norm = norm_targ_lens - *mat;
  }
  
  if(arm_time > 2.0f * world_step) {
    err_len_norm_dt = err_len_norm - err_len_norm_prv;
    err_len_norm_dt /= world_step;
    const float dtc = (1.0f - errors.norm_dra_dt);
    err_len_norm_dra *= dtc;
    err_len_norm_dra += (err_len_norm_dt * errors.norm_dra_dt);
    for(int i=0; i<n_musc; i++) {
      float ne = err_len_norm_dra.FastEl1d(i);
      float& io = err_io.FastEl1d(i);
      if(ne > errors.io_thr)
        io = 1.0f;
      else
        io = 0.0f;
    }
  }

  err_io_mag = taMath_float::vec_norm(&(err_io));

  err_len_norm_prv = err_len_norm;

  return true;
}

void VEArm::InitDynamicState() {
  SetAllMuscGains(1.0f);

  arm_time = 0.0f;

  VEBody* hand = bodies[HAND];
  hand_loc_prv = hand->cur_pos - should_loc;
  hand_vel_mag = 0.0f;
  hand_vra = 0.0f;

  norm_lens.SetGeom(1,n_musc);
  norm_targ_lens.SetGeom(1,n_musc);
  norm_vels.SetGeom(1,n_musc);
  err_len_norm.SetGeom(1,n_musc);
  err_len_norm_dt.SetGeom(1,n_musc);
  err_len_norm_dra.SetGeom(1,n_musc);
  err_len_norm_prv.SetGeom(1,n_musc);
  err_io.SetGeom(1,n_musc);

  err_len.SetGeom(1,n_musc);
  err_itg.SetGeom(1,n_musc);
  err_drv.SetGeom(1,n_musc);
  err_drv_dra.SetGeom(1,n_musc);
  err_len_prv.SetGeom(1,n_musc);
  stims.SetGeom(1,n_musc);
  forces.SetGeom(2, 3, n_musc);

  err_len.InitVals(0.0f);
  err_itg.InitVals(0.0f);
  err_drv.InitVals(0.0f);
  err_drv_dra.InitVals(0.0f);
  err_len_prv.InitVals(0.0f);
  stims.InitVals(0.0f);
  forces.InitVals(0.0f);

  err_len_norm.InitVals(0.0f);
  err_len_norm_dt.InitVals(0.0f);
  err_len_norm_dra.InitVals(0.0f);
  err_len_norm_prv.InitVals(0.0f);
  err_io.InitVals(0.0f);
}

void VEArm::Init() {
  InitDynamicState();
  VEWorld* Worldly = GetWorld();
  world_step = Worldly->stepsize;
  inherited::Init();
}

void VEArm::PrintElbowFmODE() {
  VEJoint* elbow = joints[ELBOW];
  void* elbow_jid = elbow->joint_id;
  VEWorld* ArmWorld = GetWorld();
  taVector3f haxis, hanchor;

#if 1
  ArmWorld->GetHingeAxis(elbow_jid, haxis);
  taMisc::Info("elbow axis = ", haxis.GetStr(), "\n");
  ArmWorld->GetHingeAnchor(elbow_jid, hanchor);
  taMisc::Info("elbow anchor = ", hanchor.GetStr(), "\n");
#endif
}

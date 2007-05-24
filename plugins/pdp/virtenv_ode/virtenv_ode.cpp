#include "virtenv_ode.h"
#include <QtPlugin>
#include <iostream>

const taVersion VEOdePlugin::version(1,0,0,0);

VEOdePlugin::VEOdePlugin(QObject*){}

int VEOdePlugin::InitializeTypes() {
  ta_Init_virtenv_ode();  // call the maketa-generated type initialization routine
  taMisc::Init_Types();		// this is required if plugin defines any new So/Inventor types
  return 0;
}

int VEOdePlugin::InitializePlugin() {
  return 0;
}

const char* VEOdePlugin::url() {
  return "http://grey.colorado.edu/cgi-bin/trac.cgi";
}

Q_EXPORT_PLUGIN2(virtenv_ode, VEOdePlugin)

/////////////////////////////////////////////////////////////////////////////////
//		Actual ODE Code

////////////////////////////////////////////////
//		bodies (ridid object elements)

#include <Inventor/SbLinear.h>

void VEBody::Initialize() {
  body_id = NULL;
  flags = BF_NONE;
  mass = 1.0f;
  mass_shape = CYLINDER;
  mass_radius = .5f;
  mass_length = 1.0f;
  mass_box = 1.0f;
  use_fname = true;
  color.Set(0.2f, 0.2f, .5f, .5f);	// transparent blue.. why not..
  fixed_joint_id = NULL;
}

void VEBody::Destroy() {
  CutLinks();
}

void VEBody::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

VEWorld* VEBody::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEBody::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return wld;
  return wld->world_id;
}

bool VEBody::CreateODE() {
  if(body_id) return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create object!"))
    return false;
  body_id = (dBodyID)dBodyCreate(wid);
  return (bool)body_id;
}

void VEBody::DestroyODE() {
  if(body_id) dBodyDestroy((dBodyID)body_id);
  body_id = NULL;
  if(fixed_joint_id) dJointDestroy((dJointID)fixed_joint_id);
  fixed_joint_id = NULL;
}

void VEBody::SetValsToODE() {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "SetValsToODE", "no valid world id -- cannot create stuff!"))
    return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetPosition(bid, init_pos.x, init_pos.y, init_pos.z);

  dMatrix3 R;
  dRFromAxisAndAngle(R, init_rot.x, init_rot.y, init_rot.z, init_rot.rot);
  dBodySetRotation(bid, R);

  if(HasBodyFlag(FIXED)) {
    dBodySetLinearVel(bid, 0.0f, 0.0f, 0.0f);
    dBodySetAngularVel(bid,  0.0f, 0.0f, 0.0f);
    fixed_joint_id = dJointCreateFixed(wid, 0);
    dJointAttach((dJointID)fixed_joint_id, bid, 0);	// 0 = attach to static object
  }
  else {
    dBodySetLinearVel(bid, init_lin_vel.x, init_lin_vel.y, init_lin_vel.z);
    dBodySetAngularVel(bid, init_ang_vel.x, init_ang_vel.y, init_ang_vel.z);
  }

  SetMassToODE();
}

void VEBody::SetMassToODE() {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  switch(mass_shape) {
  case SPHERE:
    dMassSetSphereTotal(&mass_ode, mass, mass_radius);
    break;
  case CAPPED_CYLINDER:
    dMassSetCappedCylinderTotal(&mass_ode, mass, mass_long_axis, mass_radius, mass_length);
    break;
  case CYLINDER:
    dMassSetCylinderTotal(&mass_ode, mass, mass_long_axis, mass_radius, mass_length);
    break;
  case BOX:
    dMassSetBoxTotal(&mass_ode, mass, mass_box.x, mass_box.y, mass_box.z);
    break;
  }
  dBodySetMass(bid, &mass_ode);
}

void VEBody::GetValsFmODE() {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  const dReal* opos = dBodyGetPosition(bid);
  cur_pos.x = opos[0]; cur_pos.y = opos[1]; cur_pos.z = opos[2];

  // ODE quaternion = w,x,y,z; Inventor = x,y,z,w
  const dReal* quat = dBodyGetQuaternion(bid);
  SbRotation sbrot;
  sbrot.setValue(-quat[3], quat[0], quat[1], -quat[2]);
  SbVec3f rot_ax;
  sbrot.getValue(rot_ax, cur_rot.rot);
  cur_rot.x = rot_ax[0]; cur_rot.y = rot_ax[1]; cur_rot.z = rot_ax[2];

  const dReal* olv = dBodyGetLinearVel(bid);
  cur_lin_vel.x = olv[0]; cur_lin_vel.y = olv[1]; cur_lin_vel.z = olv[2];
  const dReal* oav = dBodyGetAngularVel(bid);
  cur_ang_vel.x = oav[0]; cur_ang_vel.y = oav[1]; cur_ang_vel.z = oav[2];

  DataChanged(DCR_ITEM_UPDATED); // update displays..
  // not necc (nonrelativistic!)
  //  dBodyGetMass(bid, &mass);
}

/////////////////////////////////////////////
//		Group

void VEBody_Group::SetValsToODE() {
  VEBody* ob;
  taLeafItr i;
  FOR_ITR_EL(VEBody, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEBody_Group::GetValsFmODE() {
  VEBody* ob;
  taLeafItr i;
  FOR_ITR_EL(VEBody, ob, this->, i) {
    ob->GetValsFmODE();
  }
}


////////////////////////////////////////////////
//		Joints

void VEJoint::Initialize() {
  joint_id = NULL;
  flags = JF_NONE;
  set_type = NO_JOINT;
  joint_type = HINGE;
  axis.x = 1.0f;
  axis2.x = 1.0f;
  lo_stop = -3.1415;
  hi_stop = 3.1415;
  stop_bounce = 0.0;
}

void VEJoint::Destroy() {
  CutLinks();
}

void VEJoint::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

VEWorld* VEJoint::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEJoint::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return wld;
  return wld->world_id;
}

bool VEJoint::CreateODE() {
  if(joint_id && joint_type == set_type) return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create joint!"))
    return false;
  if(joint_id)
    DestroyODE();
  switch(joint_type) {
  case BALL:
    joint_id = (dJointID)dJointCreateBall(wid, 0);
    break;
  case HINGE:
    joint_id = (dJointID)dJointCreateHinge(wid, 0);
    break;
  case SLIDER:
    joint_id = (dJointID)dJointCreateSlider(wid, 0);
    break;
  case UNIVERSAL:
    joint_id = (dJointID)dJointCreateUniversal(wid, 0);
    break;
  case HINGE2:
    joint_id = (dJointID)dJointCreateHinge2(wid, 0);
    break;
  case FIXED:
    joint_id = (dJointID)dJointCreateFixed(wid, 0);
    break;
//   case PR:
//     joint_id = (dJointID)dJointCreatePR(wid, 0);
//     break;
  }
  set_type = joint_type;
  return (bool)joint_id;
}

void VEJoint::DestroyODE() {
  if(joint_id) dJointDestroy((dJointID)joint_id);
  joint_id = NULL;
}

void VEJoint::SetValsToODE() {
  if(!joint_id || joint_type != set_type) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  if(TestError(!body1 || !body1->body_id,
	       "SetValsToODE", "body1 of joint MUST be specified and already exist!"))
    return;
  if(TestError(!body2 || !body2->body_id,
	       "SetValsToODE", "body2 of joint MUST be specified and already exist -- use fixed field on body to set fixed bodies!"))
    return;

  dJointAttach(jid, (dBodyID)body1->body_id, (dBodyID)body2->body_id);

  FloatTDCoord wanchor = body1->init_pos + anchor; // world anchor offset from body1 position

  switch(joint_type) {
  case BALL:
    dJointSetBallAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case HINGE:
    dJointSetHingeAxis(jid, axis.x, axis.y, axis.z);
    dJointSetHingeAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case SLIDER:
    dJointSetSliderAxis(jid, axis.x, axis.y, axis.z);
    break;
  case UNIVERSAL:
    dJointSetUniversalAxis1(jid, axis.x, axis.y, axis.z);
    dJointSetUniversalAxis2(jid, axis2.x, axis2.y, axis2.z);
    dJointSetUniversalAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case HINGE2:
    dJointSetHinge2Axis1(jid, axis.x, axis.y, axis.z);
    dJointSetHinge2Axis2(jid, axis2.x, axis2.y, axis2.z);
    dJointSetHinge2Anchor(jid, wanchor.x, wanchor.y, wanchor.z);
    break;
  case FIXED:
    break;
//   case PR:
//     dJointSetPRAnchor(jid, wanchor.x, wanchor.y, wanchor.z);
//     dJointSetPRAxis1(jid, axis.x, axis.y, axis.z);
//     dJointSetPRAxis2(jid, axis2.x, axis2.y, axis2.z);
//     break;
  }

  if(HasJointFlag(USE_STOPS)) {
    switch(joint_type) {
    case HINGE:
      dJointSetHingeParam(jid, dParamLoStop, lo_stop);
      dJointSetHingeParam(jid, dParamHiStop, hi_stop);
      dJointSetHingeParam(jid, dParamBounce, stop_bounce);
      break;
    case SLIDER:
      dJointSetSliderParam(jid, dParamLoStop, lo_stop);
      dJointSetSliderParam(jid, dParamHiStop, hi_stop);
      dJointSetSliderParam(jid, dParamBounce, stop_bounce);
      break;
    case UNIVERSAL:
      // todo: set these!
      break;
    case HINGE2:
      break;
    case FIXED:
      break;
    }
  }

  if(HasJointFlag(FEEDBACK)) {
    dJointSetFeedback(jid, &ode_fdbk_obj);
  }
}

void VEJoint::GetValsFmODE() {
  if(!HasJointFlag(FEEDBACK)) return;
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  dJointGetFeedback(jid);
  cur_force1.x = ode_fdbk_obj.f1[0]; cur_force1.y = ode_fdbk_obj.f1[1]; cur_force1.z = ode_fdbk_obj.f1[2];
  cur_force2.x = ode_fdbk_obj.f2[0]; cur_force2.y = ode_fdbk_obj.f2[1]; cur_force2.z = ode_fdbk_obj.f2[2];
  cur_torque1.x = ode_fdbk_obj.t1[0]; cur_torque1.y = ode_fdbk_obj.t1[1]; cur_torque1.z = ode_fdbk_obj.t1[2];
  cur_torque2.x = ode_fdbk_obj.t2[0]; cur_torque2.y = ode_fdbk_obj.t2[1]; cur_torque2.z = ode_fdbk_obj.t2[2];

  switch(joint_type) {
  case BALL:
    break;
  case HINGE:
    pos = dJointGetHingeAngle(jid);
    vel = dJointGetHingeAngleRate(jid);
    break;
  case SLIDER:
    pos = dJointGetSliderPosition(jid);
    vel = dJointGetSliderPositionRate(jid);
    break;
  case UNIVERSAL:
    pos = dJointGetUniversalAngle1(jid);
    vel = dJointGetUniversalAngle1Rate(jid);
    pos2 = dJointGetUniversalAngle2(jid);
    vel2 = dJointGetUniversalAngle2Rate(jid);
    break;
  case HINGE2:
    pos = dJointGetHinge2Angle1(jid);
    vel = dJointGetHinge2Angle1Rate(jid);
    vel2 = dJointGetHinge2Angle2Rate(jid);
    break;
  case FIXED:
    break;
  }

  DataChanged(DCR_ITEM_UPDATED); // update displays..
}

void VEJoint::ApplyForce(float force1, float force2) {
  if(!joint_id) CreateODE();
  if(!joint_id) return;
  dJointID jid = (dJointID)joint_id;

  switch(joint_type) {
  case BALL:
    break;
  case HINGE:
    dJointAddHingeTorque(jid, force1);
    break;
  case SLIDER:
    dJointAddSliderForce(jid, force1);
    break;
  case UNIVERSAL:
    dJointAddUniversalTorques(jid, force1, force2);
    break;
  case HINGE2:
    dJointAddHinge2Torques(jid, force1, force2);
    break;
  case FIXED:
    break;
  }
}

/////////////////////////////////////////////
//		Group

void VEJoint_Group::SetValsToODE() {
  VEJoint* ob;
  taLeafItr i;
  FOR_ITR_EL(VEJoint, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEJoint_Group::GetValsFmODE() {
  VEJoint* ob;
  taLeafItr i;
  FOR_ITR_EL(VEJoint, ob, this->, i) {
    ob->GetValsFmODE();
  }
}

////////////////////////////////////////////////
//	Object: collection of bodies and joints

void VEObject::Initialize() {
}
void VEObject::Destroy() {
  CutLinks();
}

VEWorld* VEObject::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEObject::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return wld;
  return wld->world_id;
}

void VEObject::SetValsToODE() {
  bodies.SetValsToODE();	// bodies first!
  joints.SetValsToODE();
}

void VEObject::GetValsFmODE() {
  bodies.GetValsFmODE();	// bodies first!
  joints.GetValsFmODE();
}

/////////////////////////////////////////////
//		Group

void VEObject_Group::SetValsToODE() {
  VEObject* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObject, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEObject_Group::GetValsFmODE() {
  VEObject* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObject, ob, this->, i) {
    ob->GetValsFmODE();
  }
}


///////////////////////////////////////////////////////////////
//	World!

void VEWorld::Initialize() {
  world_id = NULL;
  gravity.y = -9.81f;
  step_type = STD_STEP;
  stepsize = .01f;
  quick_iters = 20;
}

void VEWorld::Destroy() {
  CutLinks();
}

void VEWorld::CutLinks() {
  objects.CutLinks();
  DestroyODE();			// do this last!
  inherited::CutLinks();
}

bool VEWorld::CreateODE() {
  world_id = (dWorldID)dWorldCreate();
  return (bool)world_id;
}

void VEWorld::DestroyODE() {
  if(world_id) dWorldDestroy((dWorldID)world_id);
  world_id = NULL;
}

void VEWorld::SetValsToODE() {
  if(!world_id) CreateODE();
  if(!world_id) return;
  dWorldID wid = (dWorldID)world_id;
  dWorldSetGravity(wid, gravity.x, gravity.y, gravity.z);

  StructUpdate(true);
  objects.SetValsToODE();
  StructUpdate(false);		// trigger full rebuild!
}

void VEWorld::GetValsFmODE() {
  objects.GetValsFmODE();
  // not really nec
  // void dWorldGetGravity (dWorldID, dVector3 gravity);
}

void VEWorld::Step() {
  if(!world_id) return;
  dWorldID wid = (dWorldID)world_id;
  if(step_type == STD_STEP) {
    dWorldStep(wid, stepsize);
  }
  else {
    dWorldQuickStep(wid, stepsize);
  }
  GetValsFmODE();
}


/////////////////////////////////////////////////////////////////////////
//		So Inventor objects

SO_NODE_SOURCE(T3VEWorld);

void T3VEWorld::initClass()
{
  SO_NODE_INIT_CLASS(T3VEWorld, T3NodeParent, "T3NodeParent");
}

T3VEWorld::T3VEWorld(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEWorld);
}

T3VEWorld::~T3VEWorld()
{
  
}

/////////////////////////////////////////////
//	Object

SO_NODE_SOURCE(T3VEObject);

void T3VEObject::initClass()
{
  SO_NODE_INIT_CLASS(T3VEObject, T3NodeParent, "T3NodeParent");
}

T3VEObject::T3VEObject(void* world)
:inherited(world)
{
  SO_NODE_CONSTRUCTOR(T3VEObject);
}

T3VEObject::~T3VEObject()
{
  
}

/////////////////////////////////////////////
//	Body

SO_NODE_SOURCE(T3VEBody);

void T3VEBody::initClass()
{
  SO_NODE_INIT_CLASS(T3VEBody, T3NodeLeaf, "T3NodeLeaf");
}

T3VEBody::T3VEBody(void* bod, bool show_drag)
:inherited(bod)
{
  SO_NODE_CONSTRUCTOR(T3VEBody);

  show_drag_ = show_drag;
  drag_ = NULL;
}

T3VEBody::~T3VEBody()
{
  
}

///////////////////////////////////////////////////////////////////////
//		T3 DataView Guys


#include <QGroupBox>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <Inventor/SoInput.h>
#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoFont.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>

void VEBodyView::Initialize(){
  data_base = &TA_VEBody;
}

void VEBodyView::Copy_(const VEBodyView& cp) {
  name = cp.name;
}

void VEBodyView::Destroy() {
  CutLinks();
}

bool VEBodyView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void VEBodyView::SetBody(VEBody* ob) {
  if (Body() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
    }
  }
}

void VEBodyView::Render_pre() {
  m_node_so = new T3VEBody(this);

  SoSeparator* ssep = m_node_so->shapeSeparator();

  VEBody* ob = Body();
  if(ob) {
    if(ob->use_fname && !ob->obj_fname.empty()) {
      SoInput in;
      if (in.openFile(ob->obj_fname)) {
	SoSeparator* root = SoDB::readAll(&in);
	if (root) {
	  ssep->addChild(root);
	  SoTransform* tx = m_node_so->txfm_shape();
	  ob->obj_xform.CopyTo(tx);
	  goto finish;
	}
      }
      taMisc::Error("object file:", ob->obj_fname, "not found, reverting to mass_shape");
    }
    switch(ob->mass_shape) {
    case VEBody::SPHERE: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->mass_radius;
      ssep->addChild(sp);
      break;
    }
    case VEBody::CAPPED_CYLINDER:
    case VEBody::CYLINDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->mass_radius;
      sp->height = ob->mass_length;
      ssep->addChild(sp);
      SoTransform* tx = m_node_so->txfm_shape();
      if(ob->mass_long_axis == VEBody::LONG_X)
	tx->rotation.setValue(SbVec3f(0.0f, 0.0f, 1.0f), 1.5708);
      else if(ob->mass_long_axis == VEBody::LONG_Z)
	tx->rotation.setValue(SbVec3f(1.0f, 0.0f, 0.0f), 1.5708);
      break;
    }
    case VEBody::BOX: {
      SoCube* sp = new SoCube;
      sp->width = ob->mass_box.x;
      sp->depth = ob->mass_box.z;
      sp->height = ob->mass_box.y;
      ssep->addChild(sp);
      break;
    }
    }
  }      
 finish:

  inherited::Render_pre();
}

void VEBodyView::Render_impl() {
  inherited::Render_impl();

  T3VEBody* node_so = (T3VEBody*)this->node_so(); // cache
  if(!node_so) return;
  VEBody* ob = Body();
  if(!ob) return;

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->cur_pos.x, ob->cur_pos.y, ob->cur_pos.z);
  tx->rotation.setValue(SbVec3f(ob->cur_rot.x, ob->cur_rot.y, ob->cur_rot.z), ob->cur_rot.rot);

  SoMaterial* mat = node_so->material();
  mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
  mat->transparency.setValue(1.0f - ob->color.a);
}

//////////////////////////
//   VEObjectView	//
//////////////////////////

void VEObjectView::Initialize(){
  data_base = &TA_VEObject;
}

void VEObjectView::Copy_(const VEObjectView& cp) {
  name = cp.name;
}

void VEObjectView::Destroy() {
  CutLinks();
}

bool VEObjectView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void VEObjectView::SetObject(VEObject* ob) {
  if (Object() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
    }
  }
}

void VEObjectView::BuildAll() {
  Reset();
  VEObject* obj = Object();
  if(!obj) return;

  VEBody* bod;
  taLeafItr i;
  FOR_ITR_EL(VEBody, bod, obj->bodies., i) {
    VEBodyView* ov = new VEBodyView();
    ov->SetBody(bod);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEObjectView::Render_pre() {
  m_node_so = new T3VEObject(this);
  inherited::Render_pre();
}

void VEObjectView::Render_impl() {
  inherited::Render_impl();

  T3VEObject* node_so = (T3VEObject*)this->node_so(); // cache
  if(!node_so) return;
  VEObject* ob = Object();
  if(!ob) return;
}

//////////////////////////
//   VEWorldView	//
//////////////////////////

VEWorldView* VEWorld::NewView(T3DataViewFrame* fr) {
  return VEWorldView::New(this, fr);
}

VEWorldView* VEWorldView::New(VEWorld* wl, T3DataViewFrame*& fr) {
  if (!wl) return NULL;
  if (fr) {
    //note: even if fr specified, need to insure it is right proj for object
    if (!wl->SameScope(fr, &TA_taProject)) {
      taMisc::Error("The viewer you specified is not in the same Project as the world.");
      return NULL;
    }
    // check if already viewing this obj there, warn user
    // no, because we want to be able to graph and grid in same view!
//     T3DataView* dv = fr->FindRootViewOfData(wl);
//     if (dv) {
//       if (taMisc::Choice("This table is already shown in that frame -- would you like"
//           " to show it in a new frame?", "&Ok", "&Cancel") != 0) return NULL;
//       fr = NULL; // make a new one
//     }
  } 
  if (!fr) {
    fr = T3DataViewer::GetBlankOrNewT3DataViewFrame(wl);
  }
  if (!fr) return NULL; // unexpected...
  
  VEWorldView* vw = new VEWorldView;
  fr->AddView(vw);
  vw->SetWorld(wl);
  // make sure we get it all setup!
  vw->BuildAll();
  fr->Render();
  fr->ViewAll();
  fr->GetCameraPosOrient();
  return vw;
}

void VEWorldView::Initialize() {
  data_base = &TA_VEWorld;
  children.SetBaseType(&TA_VEObjectView);
}

void VEWorldView::InitLinks() {
  inherited::InitLinks();
//   taBase::Own(table_orient, this);
}

void VEWorldView::CutLinks() {
  inherited::CutLinks();
}

void VEWorldView::Copy_(const VEWorldView& cp) {
  display_on = cp.display_on;
}

void VEWorldView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
}

String VEWorldView::GetLabel() const {
  VEWorld* wl = World(); 
  if(wl) return wl->GetName();
  return "(no world)";
}

String VEWorldView::GetName() const {
  VEWorld* wl = World(); 
  if(wl) return wl->GetName();
  return "(no world)";
}

const String VEWorldView::caption() const {
  VEWorld* wl = World(); 
  String rval;
  if (wl) {
    rval = wl->GetDisplayName();
  } else rval = "(no world";
  return rval;
}

void VEWorldView::SetWorld(VEWorld* wl) {
  if (World() == wl) return;
  if (wl) {
    SetData(wl);
    if (m_wvp) m_wvp->Refresh(); // to update name
  } else {
    Unbind(); // also does kids
  }
}

void VEWorldView::OnWindowBind_impl(iT3DataViewFrame* vw) {
  inherited::OnWindowBind_impl(vw);
  if (!m_wvp) {
    m_wvp = new VEWorldViewPanel(this);
    vw->viewerWindow()->AddPanel(m_wvp, false); // no tab yet
    vw->RegisterPanel(m_wvp);
  }
}

void VEWorldView::InitDisplay(bool init_panel) {
  if (init_panel) {
    InitPanel();
    UpdatePanel();
  }
  // descend into sub items
//   LayerView* lv;
//   taListItr i;
//   FOR_ITR_EL(LayerView, lv, children., i) {
//     InitDisplay_Layer(lv, false);
//   }
}

void VEWorldView::UpdateDisplay(bool update_panel) {
  if (update_panel) UpdatePanel();
  Render_impl();
}

void VEWorldView::InitPanel() {
  if (m_wvp)
    m_wvp->InitPanel();
}

void VEWorldView::UpdatePanel() {
  if (m_wvp)
    m_wvp->UpdatePanel();
}

bool VEWorldView::isVisible() const {
  return (taMisc::gui_active && isMapped());
}

void VEWorldView::BuildAll() {
  Reset();
  VEWorld* wl = World();
  if(!wl) return;

  VEObject* obj;
  taLeafItr i;
  FOR_ITR_EL(VEObject, obj, wl->objects., i) {
    VEObjectView* ov = new VEObjectView();
    ov->SetObject(obj);
    children.Add(ov);
    ov->BuildAll();
  }
}

void VEWorldView::Render_pre() {
  InitPanel();

  m_node_so = new T3VEWorld(this);

  inherited::Render_pre();
}

void VEWorldView::Render_impl() {
  inherited::Render_impl();

  T3VEWorld* node_so = (T3VEWorld*)this->node_so(); // cache
  if(!node_so) return;

  SoFont* font = node_so->captionFont(true);
  float font_size = 0.4f;
  font->size.setValue(font_size); // is in same units as geometry units of network
  node_so->setCaption(caption().chars());
}

// void VEWorldView::setDisplay(bool value) {
//   if (display_on == value) return;
//   display_on = value;
//   UpdateDisplay(false);		// 
// }

////////////////////////////////////////////////////////////

VEWorldViewPanel::VEWorldViewPanel(VEWorldView* dv_)
:inherited(dv_)
{
  int font_spec = taiMisc::fonMedium;
  updating = 0;
  QWidget* widg = new QWidget();
  //note: we don't set the values of all controls here, because dv does an immediate refresh
  layOuter = new QVBoxLayout(widg);
  layOuter->setSpacing(taiM->vsep_c);

  ////////////////////////////////////////////////////////////////////////////
  layDispCheck = new QHBoxLayout(layOuter);

  setCentralWidget(widg);
}

VEWorldViewPanel::~VEWorldViewPanel() {
  VEWorldView* wv_ = wv();
  if (wv_) {
    wv_->m_wvp = NULL;
  }
}

void VEWorldViewPanel::GetImage_impl() {
  inherited::GetImage_impl();
  VEWorldView* wv_ = wv();
}

void VEWorldViewPanel::InitPanel() {
  VEWorldView* wv_ = wv();
  if(!wv_) return;
  ++updating;
  // fill monitor values
//   GetVars();
  --updating;
}


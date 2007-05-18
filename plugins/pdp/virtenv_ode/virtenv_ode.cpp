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


void VEObj::Initialize() {
  body_id = NULL;
  mass = 1.0f;
  mass_shape = CYLINDER;
  mass_radius = .5f;
  mass_length = 1.0f;
  mass_box = 1.0f;
  color.Set(0.2f, 0.2f, .5f, .5f);	// transparent blue.. why not..
}

void VEObj::Destroy() {
  CutLinks();
}

void VEObj::CutLinks() {
  DestroyODE();
  inherited::CutLinks();
}

VEWorld* VEObj::GetWorld() {
  return GET_MY_OWNER(VEWorld);
}

void* VEObj::GetWorldID() {
  VEWorld* wld = GetWorld();
  if(!wld) return wld;
  return wld->world_id;
}

bool VEObj::CreateODE() {
  if(body_id) return true;
  dWorldID wid = (dWorldID)GetWorldID();
  if(TestError(!wid, "CreateODE", "no valid world id -- cannot create object!"))
    return false;
  body_id = (dBodyID)dBodyCreate(wid);
  return (bool)body_id;
}

void VEObj::DestroyODE() {
  if(body_id) dBodyDestroy((dBodyID)body_id);
  body_id = NULL;
}

void VEObj::SetValsToODE() {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  dBodySetPosition(bid, pos.x, pos.y, pos.z);

  dMatrix3 R;
  dRFromAxisAndAngle(R, orient.x, orient.y, orient.z, orient.rot);
  dBodySetRotation(bid, R);

  dBodySetLinearVel(bid, lin_vel.x, lin_vel.y, lin_vel.z);
  dBodySetAngularVel(bid, ang_vel.x, ang_vel.y, ang_vel.z);

  SetMassToODE();
}

void VEObj::SetMassToODE() {
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

void VEObj::GetValsFmODE() {
  if(!body_id) CreateODE();
  if(!body_id) return;
  dBodyID bid = (dBodyID)body_id;
  const dReal* opos = dBodyGetPosition(bid);
  pos.x = opos[0]; pos.y = opos[1]; pos.z = opos[2];

// const dReal * dBodyGetRotation (dBodyID);
  // todo: need to get axis & angle back from 4x3 return matrix

  const dReal* olv = dBodyGetLinearVel(bid);
  lin_vel.x = olv[0]; lin_vel.y = olv[1]; lin_vel.z = olv[2];
  const dReal* oav = dBodyGetAngularVel(bid);
  ang_vel.x = oav[0]; ang_vel.y = oav[1]; ang_vel.z = oav[2];

  DataChanged(DCR_ITEM_UPDATED); // update displays..
  // not really necc:
  //  dBodyGetMass(bid, &mass);
}

void VEObj::ResetPosVel() {
  pos = 0.0f;
  lin_vel = 0.0f;
  ang_vel = 0.0f;
  DataChanged(DCR_ITEM_UPDATED); // update displays..
}

/////////////////////////////////////////////
//		Group

void VEObj_Group::SetValsToODE() {
  VEObj* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObj, ob, this->, i) {
    ob->SetValsToODE();
  }
}

void VEObj_Group::GetValsFmODE() {
  VEObj* ob;
  taLeafItr i;
  FOR_ITR_EL(VEObj, ob, this->, i) {
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
  DestroyODE();
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

  objects.SetValsToODE();
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
//		So Inventor Objects

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
//	obj

SO_NODE_SOURCE(T3VEObj);

void T3VEObj::initClass()
{
  SO_NODE_INIT_CLASS(T3VEObj, T3NodeLeaf, "T3NodeLeaf");
}

T3VEObj::T3VEObj(void* obj, bool show_drag)
:inherited(obj)
{
  SO_NODE_CONSTRUCTOR(T3VEObj);

  show_drag_ = show_drag;
  drag_ = NULL;
}

T3VEObj::~T3VEObj()
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

void VEObjView::Initialize(){
  data_base = &TA_VEObj;
}

void VEObjView::Copy_(const VEObjView& cp) {
  name = cp.name;
}

void VEObjView::Destroy() {
  CutLinks();
}

bool VEObjView::SetName(const String& value) { 
  name = value;  
  return true; 
} 

void VEObjView::SetObj(VEObj* ob) {
  if (Obj() == ob) return;
  SetData(ob);
  if(ob) {
    if (name != ob->name) {
      name = ob->name;
    }
  }
}

void VEObjView::Render_pre() {
  m_node_so = new T3VEObj(this);

  SoSeparator* ssep = m_node_so->shapeSeparator();

  VEObj* ob = Obj();
  if(ob) {
    if(!ob->obj_fname.empty()) {
      SoInput in;
      if (in.openFile(ob->obj_fname)) {
	SoSeparator* root = SoDB::readAll(&in);
	if (root) {
	  ssep->addChild(root);
	  goto finish;
	}
      }
      taMisc::Error("object file:", ob->obj_fname, "not found, reverting to mass_shape");
    }
    switch(ob->mass_shape) {
    case VEObj::SPHERE: {
      SoSphere* sp = new SoSphere;
      sp->radius = ob->mass_radius;
      ssep->addChild(sp);
      break;
    }
    case VEObj::CAPPED_CYLINDER:
    case VEObj::CYLINDER: {
      SoCylinder* sp = new SoCylinder;
      sp->radius = ob->mass_radius;
      sp->height = ob->mass_length;
      ssep->addChild(sp);
      break;
    }
    case VEObj::BOX: {
      SoCube* sp = new SoCube;
      sp->width = ob->mass_box.x;
      sp->depth = ob->mass_box.y;
      sp->height = ob->mass_box.z;
      ssep->addChild(sp);
      break;
    }
    }
  }      
 finish:
  SoTransform* tx = m_node_so->txfm_shape();
  ob->obj_xform.CopyTo(tx);

  inherited::Render_pre();
}

void VEObjView::Render_impl() {
  inherited::Render_impl();

  T3VEObj* node_so = (T3VEObj*)this->node_so(); // cache
  if(!node_so) return;
  VEObj* ob = Obj();
  if(!ob) return;

  SoTransform* tx = node_so->transform();
  tx->translation.setValue(ob->pos.x, ob->pos.y, ob->pos.z);
  tx->rotation.setValue(SbVec3f(ob->orient.x, ob->orient.y, ob->orient.z), ob->orient.rot);

  SoMaterial* mat = node_so->material();
  mat->diffuseColor.setValue(ob->color.r, ob->color.g, ob->color.b);
  mat->transparency.setValue(1.0f - ob->color.a);
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
  children.SetBaseType(&TA_VEObjView);
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

  VEObj* obj;
  taLeafItr i;
  FOR_ITR_EL(VEObj, obj, wl->objects., i) {
    VEObjView* ov = new VEObjView();
    ov->SetObj(obj);
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


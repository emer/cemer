// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#ifndef TA_VIRTENV_QTSO_H
#define TA_VIRTENV_QTSO_H

///////////////////////////////////////////////////////////////////////
//		T3 DataView Code

#include "ta_virtenv.h" 
#include "ta_virtenv_so.h" 
#include "t3viewer.h"
#include "ta_qtgroup.h"

class VEBodyView;
class VEStaticView;
class VEWorldView;
class VEWorldViewPanel;

class TA_API VEBodyView: public T3DataView {
  // view of one body
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VEBody*		Body() const { return (VEBody*)data();}
  virtual void		SetBody(VEBody* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  virtual void		SetDraggerPos();
  // set dragger position, based on shape
  virtual void		FixOrientation(bool force=false);
  // fix the orientation of the body for different shape axes (capsule and cylinder) -- if force, do it regardless of whether IsCurShape is true (i.e. for render_pre)

  override bool		ignoreDataChanged() const;

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEBodyView& cp);
  TA_BASEFUNS(VEBodyView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VEObjCarouselView: public VEBodyView {
  // view of VEObjCarousel
INHERITED(VEBodyView)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VEObjCarousel*	ObjCarousel() const { return (VEObjCarousel*)data();}
  virtual void		SetObjCarousel(VEObjCarousel* ob);

  void	Copy_(const VEObjCarouselView& cp);
  TA_BASEFUNS(VEObjCarouselView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VEJointView: public T3DataView {
  // view of one joint
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of joint this one is associated with

  VEJoint*		Joint() const { return (VEJoint*)data();}
  virtual void		SetJoint(VEJoint* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  virtual void		SetDraggerPos();
  // set dragger position, based on shape
  virtual void		FixOrientation(bool force=false);
  // fix the orientation of the body for different shape axes (capsule and cylinder) -- if force, do it regardless of whether IsCurShape is true (i.e. for render_pre)

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEJointView& cp);
  TA_BASEFUNS(VEJointView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VEObjectView: public T3DataViewPar {
  // view of one object: a group of bodies
INHERITED(T3DataViewPar)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VEObject*		Object() const { return (VEObject*)data();}
  virtual void		SetObject(VEObject* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  override void		BuildAll();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEObjectView& cp);
  TA_BASEFUNS(VEObjectView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VEStaticView: public T3DataView {
  // view of one static environment element
INHERITED(T3DataView)
friend class VEWorldView;
public:
  String	name;		// name of static item this one is associated with

  VEStatic*		Static() const { return (VEStatic*)data();}
  virtual void		SetStatic(VEStatic* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  virtual void		SetDraggerPos();
  // set dragger position, based on shape
  virtual void		FixOrientation(bool force=false);
  // fix the orientation of the body for different shape axes (capsule and cylinder)  -- if force, do it regardless of whether IsCurShape is true (i.e. for render_pre)

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEStaticView& cp);
  TA_BASEFUNS(VEStaticView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VESpaceView: public T3DataViewPar {
  // view of one space
INHERITED(T3DataViewPar)
friend class VEWorldView;
public:
  String	name;		// name of body this one is associated with

  VESpace*		Space() const { return (VESpace*)data();}
  virtual void		SetSpace(VESpace* ob);
  
  DATAVIEW_PARENT(VEWorldView)

  override bool		SetName(const String& nm);
  override String	GetName() const 	{ return name; } 

  override void		BuildAll();
  
  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VESpaceView& cp);
  TA_BASEFUNS(VESpaceView);
protected:
  void	Initialize();
  void	Destroy();

  override void		Render_pre();
  override void		Render_impl();
};

class TA_API VEWorldView : public T3DataViewMain {
  // a virtual environment world viewer
INHERITED(T3DataViewMain)
friend class VEWorldViewPanel;
public:
  static VEWorldView* New(VEWorld* wl, T3DataViewFrame*& fr);

  bool		display_on;  	// #DEF_true 'true' if display should be updated
  bool		drag_objs;	// allow user to drag/rotate/rescale objects
  bool		show_joints;	// show a visual representation of the joints

  virtual const String	caption() const; // what to show in viewer

  VEWorld*		World() const {return (VEWorld*)data();}
  virtual void		SetWorld(VEWorld* wl);

  virtual void		InitDisplay(bool init_panel = true);
  // does a hard reset on the display, reinitializing variables etc.  Note does NOT do Updatedisplay -- that is a separate step
  virtual void		UpdateDisplay(bool update_panel = true);
  // full re-render of the display (generally calls Render_impl)

  virtual void		InitPanel();
  // lets panel init itself after struct changes
  virtual void		UpdatePanel();
  // after changes to props


  virtual void		SetupCameras();
  // configure the cameras during rendering -- called by Render_impl
  virtual void		CreateLights();
  // create the lights during render_pre
  virtual void		CreateTextures();
  // create the textures during render_pre
  virtual void		SetupLights();
  // configure the lights during rendering -- called by Render_impl

  virtual QImage	GetCameraImage(int cam_no);
  // get the output of the given camera number (currently 0 or 1)

  bool			isVisible() const; // gui_active, mapped and display_on

  override void		BuildAll();
  
  override String	GetLabel() const;
  override String	GetName() const;
  override void		OnWindowBind_impl(iT3DataViewFrame* vw);

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const VEWorldView& cp);
  T3_DATAVIEWFUNS(VEWorldView, T3DataViewMain) // 
protected:
#ifndef __MAKETA__
  QPointer<VEWorldViewPanel> m_wvp;
//   T3OffscreenRenderer*	cam_renderer;
  SoOffscreenRenderer*	cam_renderer;
#endif

  override void		DataUpdateView_impl();
  override void 	UpdateAfterEdit_impl();

  override void		Render_pre();
  override void		Render_impl();

};

class TA_API VEWorldViewPanel: public iViewPanelFrame {
  // frame for gui interface to a VEWorldView -- usually posted by the worldview
INHERITED(iViewPanelFrame)
  Q_OBJECT
public:

  QVBoxLayout*		layOuter;
  QHBoxLayout*		 layDispCheck;
  QCheckBox*		  chkDisplay;
  QCheckBox*		  chkDragObjs;
  QCheckBox*		  chkShowJoints;

  QHBoxLayout*		 layCams;
  QVBoxLayout*		  layCam0;
  QLabel*		  labcam0;
  QLabel*		  labcam0_txt;

  QVBoxLayout*		  layCam1;
  QLabel*		  labcam1;
  QLabel*		  labcam1_txt;

  iMethodButtonMgr*	meth_but_mgr;
  QWidget*		widCmdButtons;

  VEWorldView*		wv() {return (VEWorldView*)m_dv;} //

  VEWorldViewPanel(VEWorldView* dv_);
  ~VEWorldViewPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
  override TypeDef*	GetTypeDef() const {return &TA_VEWorldViewPanel;}

protected:
  bool			req_full_redraw;
  override void		UpdatePanel_impl();
  override void		GetValue_impl();

// public slots:
//   void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

  //protected slots:
};


#endif // TA_VIRTENV_QTSO_H

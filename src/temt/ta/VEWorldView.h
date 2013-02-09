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

#ifndef VEWorldView_h
#define VEWorldView_h 1

// parent includes:
#include <T3DataViewMain>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif

// declare all other types mentioned but not required to include:
class VEWorld; // 
class T3Panel; // 
class QImage; //  #IGNORE
class iT3Panel; // #IGNORE
class iViewPanelOfVEWorld; //
class SoOffscreenRendererQt; // #IGNORE

taTypeDef_Of(VEWorldView);

class TA_API VEWorldView : public T3DataViewMain {
  // a virtual environment world viewer
INHERITED(T3DataViewMain)
friend class iViewPanelOfVEWorld;
public:
  static VEWorldView* New(VEWorld* wl, T3Panel*& fr);

  bool		display_on;  	// #DEF_true 'true' if display should be updated
  bool		drag_objs;	// allow user to drag/rotate/rescale objects
  float		drag_size;	// size of the dragger controls
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
  // #IGNORE get the output of the given camera number (currently 0 or 1)

  bool			isVisible() const;
  override bool		ignoreSigEmit() const { return !isVisible(); }

  override void		BuildAll();
  
  virtual void		UpdateName();
  override void		OnWindowBind_impl(iT3Panel* vw);
  // #IGNORE

  void 	Initialize();
  void 	Destroy()	{ CutLinks(); }
  void 	InitLinks();
  void	CutLinks();
  void	Copy_(const VEWorldView& cp);
  T3_DATAVIEWFUNS(VEWorldView, T3DataViewMain) // 
protected:
#ifndef __MAKETA__
  QPointer<iViewPanelOfVEWorld> m_wvp;
  SoOffscreenRendererQt*	cam_renderer;
#endif
  bool			nowin_rebuild_done; // #NO_SAVE flag for manual rebuild of scene for nowin mode -- only do this once

  override void		SigRecvUpdateAfterEdit_impl();
  override void		SigRecvUpdateView_impl();
  override void 	UpdateAfterEdit_impl();

  override void		Render_pre();
  override void		Render_impl();

};

#endif // VEWorldView_h

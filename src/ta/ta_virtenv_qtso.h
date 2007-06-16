/* -*- C++ -*- */
/*=============================================================================
//									      //
// This file is part of the TypeAccess/C-Super-Script software package.	      //
//									      //
// Copyright (C) 1995 Randall C. O'Reilly, Chadley K. Dawson, 		      //
//		      James L. McClelland, and Carnegie Mellon University     //
//     									      //
// Permission to use, copy, modify, and distribute this software and its      //
// documentation for any purpose is hereby granted without fee, provided that //
// the above copyright notice and this permission notice appear in all copies //
// of the software and related documentation.                                 //
// 									      //
// Note that the PDP++ software package, which contains this package, has a   //
// more restrictive copyright, which applies only to the PDP++-specific       //
// portions of the software, which are labeled as such.			      //
//									      //
// Note that the taString class, which is derived from the GNU String class,  //
// is Copyright (C) 1988 Free Software Foundation, written by Doug Lea, and   //
// is covered by the GNU General Public License, see ta_string.h.             //
// The iv_graphic library and some iv_misc classes were derived from the      //
// InterViews morpher example and other InterViews code, which is             //
// Copyright (C) 1987, 1988, 1989, 1990, 1991 Stanford University             //
// Copyright (C) 1991 Silicon Graphics, Inc.				      //
//									      //
// THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,         //
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 	      //
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  	      //
// 									      //
// IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE FOR ANY SPECIAL,    //
// INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES  //
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT     //
// ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY,      //
// ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS        //
// SOFTWARE. 								      //
==============================================================================*/

#ifndef TA_VIRTENV_QTSO_H
#define TA_VIRTENV_QTSO_H

///////////////////////////////////////////////////////////////////////
//		T3 DataView Code

#include "ta_virtenv.h" 
#include "ta_virtenv_so.h" 
#include "t3viewer.h"

class SoOffscreenRenderer; // #IGNORE

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

  void 	SetDefaultName() {} // leave it blank
  void	Copy_(const VEBodyView& cp);
  TA_BASEFUNS(VEBodyView);
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
#endif
  SoOffscreenRenderer*	cam_renderer;

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
  QHBoxLayout*		 layCams;

  QVBoxLayout*		  layCam0;
  QLabel*		  labcam0;
  QLabel*		  labcam0_txt;

  QVBoxLayout*		  layCam1;
  QLabel*		  labcam1;
  QLabel*		  labcam1_txt;

  VEWorldView*		wv() {return (VEWorldView*)m_dv;} //

  virtual void		InitPanel();

  VEWorldViewPanel(VEWorldView* dv_);
  ~VEWorldViewPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
  override TypeDef*	GetTypeDef() const {return &TA_VEWorldViewPanel;}

protected:
  int			updating; // to prevent recursion
  override void		GetImage_impl();

// public slots:
//   void			viewWin_NotifySignal(ISelectableHost* src, int op); // forwarded to netview

  //protected slots:
};


#endif // TA_VIRTENV_QTSO_H

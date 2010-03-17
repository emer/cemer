// Copyright, 1995-2007, Regents of the University of Colorado,
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



// t3viewer.h: basic types for objects that can be independently viewed in space

#ifndef T3VIEWER_H
#define T3VIEWER_H

#include "ta_group.h"
#include "ta_qt.h"
#include "ta_qtviewer.h"
#include "t3node_so.h"
#include "ta_def.h"

#include "ta_TA_type.h"

#include "igeometry.h"
#include "ta_geometry.h"
#include "safeptr_so.h"

#ifndef __MAKETA__
# include <qevent.h>
# include <qwidget.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/SbLinear.h>
# include <Quarter/Quarter.h>
# include <Quarter/QuarterWidget.h>

using SIM::Coin3D::Quarter::QuarterWidget;
#else
class QuarterWidget;	// #IGNORE
#endif

// externals
class taiClipData;
class taiMimeItem;
class taiMimeSource;
class TDCoord;
class FloatTransform;
class SoPath; // #IGNORE
class SoCamera; // #IGNORE
class SbViewportRegion; // #IGNORE
class T3Node;

// forwards
class T3DataView;
class T3DataViewPar;
class T3DataViewRoot;
class T3DataViewMain;

class T3DataView_List;
class iT3ViewspaceWidget;
class iSoSelectionEvent; // #IGNORE
class iT3DataViewFrame; // #IGNORE
class iT3DataViewer; // #IGNORE
class T3DataViewFrame;
class T3DataViewer;
class T3ExaminerViewer;

class QtThumbWheel;		// #IGNORE

SoPtr_Of(T3Node);

class TA_API t3Misc {
// global params and funcs for T3 viewing system
public:
  static const float	pts_per_geom; // #DEF_72 chars*pt size / so unit
  static const float	geoms_per_pt; // number of geoms per point (1/72)
  static const float	char_ht_to_wd_pts; // ratio of ht to wd in metrics: 12/8
  static const float	char_base_fract; // fraction of total ht below baseline
};

//////////////////////////////////////////////////////////////////////////////
//   T3ExaminerViewer -- customized 

// note: this now requires Quarter instead of SoQt

class TA_API T3SavedView : public taNBase {
  // ##NO_TOKENS #NO_UPDATE_AFTER ##CAT_View Saves camera parameters for the Quarter Examiner Viewer -- name of view is name of object -- can store other arbitrary info in UserData for this guy
INHERITED(taNBase)
public:
  bool		view_saved;	// #READ_ONLY #SHOW whether a view is currently saved or not
  FloatTDCoord	pos;		// #READ_ONLY position of camera in view
  FloatRotation	orient;		// #READ_ONLY orientation of camera in view
  float		focal_dist;	// #READ_ONLY focal distance

  void		getCameraParams(SoCamera* cam);
  // get the camera parameters into our saved values
  bool		setCameraParams(SoCamera* cam);
  // set the camera parmeters from our saved values (returns false if no view saved)

  void		SetCameraPos(float x, float y, float z) { pos.SetXYZ(x,y,z); }
  // #CAT_Display set camera position
  void		SetCameraOrient(float x, float y, float z, float r) { orient.SetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void		SetCameraFocDist(float fd) { focal_dist = fd; }
  // #CAT_Display set camera focal distance

  void		GetCameraPos(float& x, float& y, float& z) { pos.GetXYZ(x,y,z); }
  // #CAT_Display set camera position
  void		GetCameraOrient(float& x, float& y, float& z, float& r) { orient.GetXYZR(x,y,z,r); }
  // #CAT_Display set camera orientation -- x,y,z axis and r rotation value
  void		GetCameraFocDist(float& fd) { fd = focal_dist; }
  // #CAT_Display set camera focal distance

  QToolButton*	view_button;	// #IGNORE view button for this view
  taiAction*	view_action;	// #IGNORE action for the gotoview function

  TA_SIMPLE_BASEFUNS(T3SavedView);
private:
  void	Initialize();
  void	Destroy()	{ };
};

class TA_API T3SavedView_List: public taList<T3SavedView> {
  // ##CAT_View list of saved views
  INHERITED(taList<T3SavedView>)
public:

  void	SetCameraPos(int view_no, float x, float y, float z);
  // #CAT_Display for given view number, set camera position
  void	SetCameraOrient(int view_no, float x, float y, float z, float r);
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void	SetCameraFocDist(int view_no, float fd);
  // #CAT_Display for given view number, set camera focal distance

  void	GetCameraPos(int view_no, float& x, float& y, float& z);
  // #CAT_Display for given view number, set camera position
  void	GetCameraOrient(int view_no, float& x, float& y, float& z, float& r);
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void	GetCameraFocDist(int view_no, float& fd);
  // #CAT_Display for given view number, set camera focal distance

  TA_BASEFUNS_NOCOPY(T3SavedView_List);
private:
  void	Initialize();
  void	Destroy() { };
};


class TA_API T3ExaminerViewer : public QWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS provides a full examiner viewer interface built on top of the QuarterWidget viewer, replicating the GUI of the SoQtExaminerViewer
  Q_OBJECT
  INHERITED(QWidget)
public:
  T3ExaminerViewer(iT3ViewspaceWidget* parent = NULL);
  ~T3ExaminerViewer();

  enum ViewerMode {		// our persistent sense of the view mode -- quarter may get out of sync
    VIEW = 0,			// move camera around (must be 0 so equiv to interaction off)
    INTERACT = 1,		// interactive (can select -- red arrow) (must be 1 so equiv to interaction on)
    PAN = 2,			// panning instead of rotating (not yet avail -- for future use)
    SEEK = 3,			// seeking (not a persistent state -- not used currently -- for future use)
  };


  ViewerMode		viewer_mode; 	// our current viewer mode -- used to make sure that quarter is in appropriate state

  iT3ViewspaceWidget*	t3vw;		// owner widget
  T3DataViewFrame* 	GetFrame();	// get my owning frame, from t3vw

  //////////////////////////////////////////////
  //   Overall widget structure

  QVBoxLayout*	  main_vbox;	   // overall vertical box, containing main_hbox, bot_hbox
  QHBoxLayout*	  main_hbox;	   // overall horizontal box, containing lhs_vbox, quarter, rhs_vbox
  QVBoxLayout*	  lhs_vbox; 	// overall box for all decoration on the left-hand-side of widget -- contains (optional -- none in default impl) buttons at top and vrot_wheel at bottom
    QVBoxLayout*  lhs_button_vbox; // buttons on the top-left hand side (empty by default)
  QuarterWidget*  quarter;	// the quarter viewer -- in the middle of the widget
  QVBoxLayout*	  rhs_vbox; 	// overall box for all decoration on the right-hand-side of widget -- contains buttons at top and zoom_wheel at bottom
    QVBoxLayout*  rhs_button_vbox; // buttons on the top-right hand side
  QHBoxLayout*	  bot_hbox;	   // overall box for all decoration on the bottom of widget -- contains (optional -- none in default impl) buttons at right and hrot_wheel at left
    QHBoxLayout*  bot_button_hbox; // buttons on the bottom right side

  //////////////////////////////////////////////
  //   Thumbwheels

  QtThumbWheel*	  zoom_wheel;	// the zoom wheel (dolly in SoQt)
  QtThumbWheel*	  vrot_wheel;	// the vertical rotation wheel (Rotx in SoQt)
  QtThumbWheel*	  hrot_wheel;	// the horizontal rotation wheel (Roty in SoQt)
  QtThumbWheel*	  vpan_wheel;	// the vertical panning wheel 
  QtThumbWheel*	  hpan_wheel;	// the horizontal panning wheel
  QLabel*	  zoom_lbl;	// labels
  QLabel*	  vrot_lbl;
  QLabel*	  hrot_lbl;
  QLabel*	  vpan_lbl;
  QLabel*	  hpan_lbl;

  //////////////////////////////////////////////
  //   Standard view buttons (on RHS)

  QToolButton*	  interact_button; // (red arrow) -- mouse actions interact with elements in the display
  QToolButton*	  view_button;     // (white hand) -- mouse actions move view around
  QToolButton*	  view_all_button; // (eyeball) -- repositions camera so everything is in view
  QToolButton* 	  seek_button;	   // (flashlight) -- zooms display to view clicked objects
  QToolButton* 	  snapshot_button; // (camera) -- save an image of the current view to a file
  QToolButton* 	  print_button;    // (printer) -- print current view to a file

  static const int   n_views;	   // number of saved view parameters to save (length of saved_views)
  T3SavedView_List saved_views; // saved view information
  int		   cur_view_no;	// current view number -- last one to have gotoView called -- -1 if not done yet
  NameVar_PArray     dyn_buttons; // dynamic button names
  taiAction_List     dyn_actions; // dynamic button actions -- has all the relevant properties

  //////////////////////////////////////////////
  //   Constructor helper methods

  void    Constr_RHS_Buttons();		// construct right-hand-side pushbuttons
  void    Constr_LHS_Buttons();		// construct left-hand-side pushbuttons
  void    Constr_Bot_Buttons();		// construct bottom pushbuttons

  int	  addDynButton(const String& label, const String& tooltip);
  // add a new dynamic button -- returns button number (may already exist)
  taiAction* getDynButton(int but_no);
  // get given dynamic button action -- has all the relevant info
  taiAction* getDynButtonName(const String& label);
  // get dynamic button action by name -- has all the relevant info
  void	  setDynButtonChecked(int but_no, bool onoff, bool mutex = true);
  // set the "checked" status of the dyn button to given state -- if mutex is true, then all other buttons are turned off when current one is changed (regardless of on/off state, all are off)

  void 	  removeAllDynButtons();
  // remove all the dynamic buttons
  bool 	  removeDynButton(int but_no);
  // remove given dynamic button
  bool 	  removeDynButtonName(const String& label);
  // remove given dynamic button by name

  //////////////////////////////////////////////
  //   Functions that actually do stuff

  SoCamera*  		getViewerCamera() const;
  // helper function get the quarter viewer camera (not immediately avail on quarter widget)
  const SbViewportRegion& getViewportRegion() const;
  // helper function get the quarter viewer viewport region (not immediately avail on quarter widget)
  virtual void 	  	viewAll();
  // view all objects in scene -- compared to QuarterWidget's default version, this one doesn't leave such a huge margin around everything so you really fill the window

  virtual void		zoomView(const float zoom_value);
  // zoom view camera in/out by given amount: adjusts both camera pos and focal length -- associated with the zoom_wheel on the right side of viewer
  virtual void		horizRotateView(const float rot_value);
  // horizontally rotate view camera (actually around the vertical Y axis) -- associated with the hrot_wheel on bottom of viewer
  virtual void		vertRotateView(const float rot_value);
  // vertically rotate view camera (actually around the horizontal or X axis) -- associated with the vrot_wheel on left hand side of viewer
  virtual void		horizPanView(const float pan_value);
  // horizontally pan (move) view camera -- associated with the hpan_wheel on bottom of viewer
  virtual void		vertPanView(const float pan_value);
  // vertically pan (move) view camera  -- associated with the vpan_wheel on right hand side of viewer

  virtual void		syncViewerMode();
  // ensure that quarter is in the same viewer mode that we think it should be in -- this should be called upon any redraw that might knock quarter out of whack.
  bool			syncCursor();
  // set our cursor to match what quarter says it should be..

  virtual void		setInteractionModeOn(bool onoff, bool re_render = true);
  // set the interaction mode on or off (if off, then it is in view mode) -- also updates button states -- also calls Render on parent dataview if mode has changed, so that interactors can be updated (unless re_render = false)
  bool			interactionModeOn() { return (bool)viewer_mode; }
  // is interaction mode on or not -- this is the interface that all 3rd parties should consult -- not the one on quarter which can often be out of sync!

  virtual void		saveView(int view_no);
  // save the current camera view information to given saved view
  virtual void		gotoView(int view_no);
  // restore the saved camera view information to the current view
  virtual bool	  	nameView(int view_no, const String& name);
  // add a new label for given saved view location
  virtual void	  	updtViewName(int view_no);
  // update displayed name from saved_views -- during init

  virtual QImage	grabImage();
  // grab the current viewer image to a pixmap
  virtual void		saveImage(const QString& fname);
  // save the current viewer image to given file name
  virtual void		printImage();
  // print the current viewer image to a printer

public slots:
  void hrotwheelChanged(int value);
  void vrotwheelChanged(int value);
  void zoomwheelChanged(int value);
  void hpanwheelChanged(int value);
  void vpanwheelChanged(int value);

  void interactbuttonClicked();
  void viewbuttonClicked();
  void viewallbuttonClicked();
  void seekbuttonClicked();
  void snapshotbuttonClicked();
  void printbuttonClicked();

  void gotoviewbuttonClicked(int view_no);
  void saveviewTriggered(int view_no);
  void nameviewTriggered(int view_no);
  void savenameviewTriggered(int view_no);

  void dynbuttonClicked(int but_no);

#ifndef __MAKETA__
signals:
  void viewSaved(int view_no);	// the given view location was saved (e.g., can now save to more permanent storage)
  void viewSelected(int view_no); // the given view location was selected for viewing -- other saved elements can now be activated by this signal
  void dynbuttonActivated(int but_no); // dynamic button of given number was activated by user
#endif

protected:
  // start values on wheels for computing deltas
  int	hrot_start_val;
  int	vrot_start_val;
  int	zoom_start_val;
  int	hpan_start_val;
  int	vpan_start_val;

  virtual void	RotateView(const SbVec3f& axis, const float ang);
  // implementation function that will rotate view camera given angle (in radians) around given axis
  virtual void	PanView(const SbVec3f& dir, const float dist);
  // implementation function that will move (pan) view camera given distance in given direction 

  override bool event(QEvent* ev_);
  override void keyPressEvent(QKeyEvent* e);
  override bool eventFilter(QObject *obj, QEvent *event);
};


////////////////////////////////////////////////////
//   T3DataView_List

class TA_API T3DataView_List: public DataView_List { // ##NO_TOKENS
INHERITED(DataView_List)
friend class T3DataView;
public:

 virtual T3DataView*  FindData(taBase* dat, int& idx);
 // find DataView guy with data() == dat (fills in its index in list), returns NULL & idx=-1 for not found

  TA_DATAVIEWLISTFUNS(T3DataView_List, DataView_List, T3DataView)
private:
  NOCOPY(T3DataView_List)
  void			Initialize() {SetBaseType(&TA_T3DataView);}
  void			Destroy() {}
};

//////////////////////////
//   T3DataView		//
//////////////////////////

/*
  DataView objects that have 3D reps
  Usually inherit directly from T3DataView, or if it is a viewer-rooted
  object, T3DataViewV

  rendering goes in two steps:
  BuildAll
  Render

  subclasses may add additional steps or other ways of ordering these
*/

class TA_API T3DataView: public taDataView, public virtual IObjectSelectable {
  // #NO_TOKENS #VIRT_BASE base class for 3d-based DataView objects
INHERITED(taDataView)
friend class T3DataViewer;
friend class T3DataView_List;
friend class T3DataViewPar;
public:
  enum DataNodeFlags { //TODO: following copied from browsing; will need to be modified
    DNF_IS_FOLDER 	= 0x001, // true for list/group folder nodes (note: does *not* indicate whether item can contain other things or not)
    DNF_IS_ALIAS 	= 0x002, // true after children have been created (after clicking on node)
    DNF_UPDATE_NAME	= 0x004, // typically for list items, update the visual name (tree, tab, etc.) after item edited
    DNF_CAN_BROWSE	= 0x008, // can be a new browser root
    DNF_CAN_DRAG	= 0x010, // 16 can allow drags
    DNF_NO_CAN_DROP	= 0x020, // 32 cannot accept drops
    DNF_IS_MEMBER 	= 0x040, // 64 true for members (and root), not for list/group items -- helps node configure edit ops
    DNF_IS_LIST_NODE 	= 0x080 // true for nodes in a list view (in panel, not on tree)
  };

  static T3DataView*	GetViewFromPath(const SoPath* path); // #IGNORE search path backwards to find the innermost T3DataView

  int			flags; // #READ_ONLY #NO_SAVE any of T3DataViewFlags TODO: tbd
  FloatTransform*	m_transform;  // #READ_ONLY #OWN_POINTER transform, created only if not unity

  virtual const iColor	bgColor(bool& ok) const {ok = false; return iColor();}
    // high-level items can optionally supply a bg color, for when they are singles
  virtual bool		hasChildren() const {return false;}
  virtual bool		hasViewProperties() const {return false;}
  // true if this item has editable view properties, and should be shown

  virtual bool		expandable() const {return false;}
  virtual void		setExpandable(bool) {}
  DATAVIEW_PARENT(T3DataView) // always a T3DataView (except root guy)
  FloatTransform*	transform(bool auto_create = false);  // transform, in Inventor coords

  override bool		isMapped() const; // only true if in gui mode and gui stuff exists 
  T3Node*		node_so() const {return m_node_so.ptr();} //
  virtual T3DataViewRoot* root();
  virtual T3DataViewFrame* GetFrame();
  // get the T3DataViewFrame that owns us
  virtual T3ExaminerViewer* GetViewer();
  // #IGNORE get the Viewer that contains us

  void 			AddRemoveChildNode(SoNode* node, bool adding);
    // can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
  override void 	Close(); // usually delegates to parent->CloseChild
  virtual void 		CloseChild(taDataView* child) {}
  virtual void		BuildAll() {}
  // subclass-dependent operation -- reinvoked after major update -- builds any sub-dataview objects, but not the corresponding So guys (which is done in render)
  override void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void		DataDestroying(); // called by data when it is destroying -- usual action is to destroy ourself
  override void		ChildClearing(taDataView* child); // NOTE: child is always a T3DataView
  override void		ChildRendered(taDataView* child); //  NOTE: child is always a T3DataView

  virtual void		OnWindowBind(iT3DataViewFrame* vw); // called after the viewer creates/fills the main window (for dataviews embedded in main viewer only), or when DataView added to existing viewer
  virtual void		ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void		UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node
  
  virtual void		ViewProperties(); // #MENU #VIEWMENU show the view properties for this object

  override void		CutLinks();
  TA_DATAVIEWFUNS(T3DataView, taDataView);

public: // ISelectable interface (only not in IDataLinkClient)
  override ISelectableHost* host() const; //
  override MemberDef*	md() const {return m_md;}
  override ISelectable*	par() const;
  override taiDataLink*	viewLink() const; // data of the link
  override GuiContext	shType() const {return GC_DUAL_DEF_DATA;} 
  override taiDataLink*	clipParLink(GuiContext sh_typ = GC_DEFAULT) const; // not par_link 
protected:
//  override void		FillContextMenu_impl(taiActions* menu, GuiContext sh_typ);
  override void 	QueryEditActionsS_impl_(int& allowed, int& forbidden,
    GuiContext sh_typ) const;

protected:
  MemberDef*		m_md; // memberdef of this item in its parent

  void			setNode(T3Node* node); // make changes via this
  
  virtual void		AddRemoveChildNode_impl(SoNode* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
  override void		ChildRemoving(taDataView* child); // #IGNORE called from list; we also forward to DataViewer; we also remove visually
  virtual void		Constr_Node_impl() {} // create the node_so rep -- called in RenderPre, null'ed in Clear

  virtual void		OnWindowBind_impl(iT3DataViewFrame* vw) {} // override for something this class
  override void		Clear_impl();
  virtual void		ReInit_impl(); // default just calls clear() on the so, if it exists
  override void		Render_pre(); //
  override void		Render_impl();

  override void 	DataStructUpdateEnd_impl(); // our own customized version, similar to generic base
  override void  	DataRebuildView_impl() {DataStructUpdateEnd_impl();} // same as StructEnd

protected:
  T3DataView*		last_child_node; // #IGNORE last child node created, so we can pass to createnode
  override void		DataUpdateAfterEdit_impl(); // called by data for an UAE
  override void		UpdateAfterEdit_impl();

private:
  T3NodePtr		m_node_so; // Inventor node DO NOT MOVE FROM PRIVATE!!! DON'T EVEN THINK ABOUT IT!!! YOU ARE STILL THINKING ABOUT IT... STOP!!!!!!!
  void	Copy_(const T3DataView& cp);
  void	Initialize();
  void	Destroy();
};
TA_SMART_PTRS(T3DataView);

#define T3_DATAVIEWFUNS(b,i) \
  TA_DATAVIEWFUNS(b,i);


//////////////////////////
//   T3DataView_PtrList	//
//////////////////////////

class TA_API T3DataView_PtrList: public taPtrList<T3DataView> { // for selection lists, aux lists, etc.
#ifndef __MAKETA__
typedef taPtrList<T3DataView> inherited;
#endif
public:

protected:
  override void*	El_Ref_(void* it) { taBase::Ref((taBase*)it); return it; } // when pushed
  override void* 	El_unRef_(void* it) { taBase::UnRef((taBase*)it); return NULL; } 
   //TODO: changed from unRef 11/28/05 -- this now has delete semantics, will require objects
   // that are owned elsewhere to have ++ref count before adding here
};


//////////////////////////
//   T3DataViewPar	//
//////////////////////////

class TA_API T3DataViewPar: public T3DataView { 
  // #VIRT_BASE #NO_TOKENS T3DataView that has child T3DataView's
#ifndef __MAKETA__
typedef T3DataView inherited;
#endif
public:
  T3DataView_List	children; // #SHOW #READ_ONLY #SHOW_TREE
  override bool		hasChildren() const {return (children.size > 0);}

  override void		OnWindowBind(iT3DataViewFrame* vw);
  override void 	CloseChild(taDataView* child);
  override void		ReInit(); //note: does a depth-first calls to children, before self

  override void		InitLinks();
  override void		CutLinks();
  T3_DATAVIEWFUNS(T3DataViewPar, T3DataView)

protected:
  override void		DoActionChildren_impl(DataViewAction acts);
  
private:
  void	Copy_(const T3DataViewPar& cp);
  void			Initialize() {}
  void			Destroy() {CutLinks();}
};

class TA_API T3DataViewRoot: public T3DataViewPar {
  // #NO_TOKENS Root item for a viewwidget type
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class iT3DataViewFrame;
friend class T3DataViewFrame;
public:
  ISelectableHost*	host; // ss/b set by owner

//note: typically never has a non-null parent, because it is rooted in non-T3DataView  
  override T3DataViewRoot* root() {return this;}
  override bool		isRootLevelView() const {return true;} 
    
  T3_DATAVIEWFUNS(T3DataViewRoot, T3DataViewPar)

// ISelectable i/f
  override GuiContext	shType() const {return GC_DUAL_DEF_VIEW;} // the default for mains

protected:
  override void		Constr_Node_impl();

private:
  NOCOPY(T3DataViewRoot)
  void			Initialize();
  void			Destroy() {}
};

class TA_API T3DataViewMain: public T3DataViewPar {
  // base class for major self-contained view objects, which can be independently positioned etc (main classes under T3DataViewRoot)
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
public:
  FloatTransform	main_xform;
  // this is the overall transform (position, scale, rotation) for this view object (typically can be adjusted by view's transform dragbox)

  override bool		isTopLevelView() const {return true;} 

  virtual void		CopyFromViewFrame(T3DataViewPar* cp);
  // copy key view frame parameters from other view object
  
  override void		InitLinks();
  T3_DATAVIEWFUNS(T3DataViewMain, T3DataViewPar) // 
private:
  void 	Copy_(const T3DataViewMain& cp);
  void	Initialize() {}
  void	Destroy() {}
};


//////////////////////////
//   iSoSelectionEvent	//
//////////////////////////

class TA_API iSoSelectionEvent { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS note: this is not a Qt event
public:
  bool 			is_selected;
  const SoPath*		path;
  
  iSoSelectionEvent(bool is_selected_, const SoPath* path_);
};


//////////////////////////
//   iT3ViewspaceWidget	//
//////////////////////////

/*
  Scene graph:
  SoSeparator* root_so
    SoGroup*   callbacks -- we put any callback nodes in here
    SoSeparator* items -- the actual items get rendered

*/
class TA_API iT3ViewspaceWidget: public QWidget, public ISelectableHost { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS widget that encapsulates an Inventor viewer; adds context menu handling, and optional scroll bars
  Q_OBJECT
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
friend class T3Viewspace;
public:
  enum SelectionMode {
    SM_NONE,
    SM_SINGLE,
    SM_MULTI
  };

  SoSelectionPtr	sel_so; //#IGNORE
  
  SoSeparator*		root_so() {return m_root_so.ptr();} // always valid -- you can add your own lights, camera, etc.
  QScrollBar*		horScrollBar(bool auto_create = false);
  void			setHasHorScrollBar(bool value);
  QScrollBar*		verScrollBar(bool auto_create = false);
  void			setHasVerScrollBar(bool value);

  SelectionMode		selMode() {return m_selMode;}
    // #IGNORE true adds a SoSelection node, and selection call back
  void			setSelMode(SelectionMode); // #IGNORE true adds a SoSelection node, and selection call back

  T3ExaminerViewer* 	t3viewer() {return m_t3viewer;}
  void			setT3viewer(T3ExaminerViewer* value); // must be called once, after creation
  void 			setSceneGraph(SoNode* sg);
  void			setTopView(taDataView* tv); // set topmost view; for show/hide -- calls SetVisible if visible on set

  void			deleteScene(); // deletes the scene -- usually only called internally, not by clients of this component
  void 			ContextMenuRequested(const QPoint& pos); // #IGNORE called from render area

  inline iT3DataViewFrame* i_data_frame() const {return m_i_data_frame;}

  iT3ViewspaceWidget(iT3DataViewFrame* parent);
  iT3ViewspaceWidget(QWidget* parent = NULL);
  ~iT3ViewspaceWidget();

#ifndef __MAKETA__
signals:
  void			initScrollBar(QScrollBar* sb); // orientation will be in sb
#endif

public: // ISelectableHost i/f
  override bool 	hasMultiSelect() const {return true;} // always
  override QWidget*	widget() {return this;} 
protected:
//  override void 	EditAction_Delete(ISelectable::GuiContext gc_typ);
  override void		UpdateSelectedItems_impl(); 

protected:
  static void		SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void		SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE

  QScrollBar*		m_horScrollBar;
  QScrollBar*		m_verScrollBar;
  T3ExaminerViewer* 	m_t3viewer;
  SoSeparatorPtr	m_root_so; //
  SoNode*		m_scene; // actual top item set by user
  SelectionMode		m_selMode; // #IGNORE true adds a SoSelection node, and selection call back
  iT3DataViewFrame*	m_i_data_frame; // #IGNORE our parent object
  taDataViewRef		m_top_view; // #IGNORE topmost view obj, for show/hide ctrl
  signed char		m_last_vis; // #IGNORE keeps track of last hide/show state, to avoid spurious calls when several in a row: -1=hide, 0=init, 1=show

  override void 	showEvent(QShowEvent* ev);
  override void 	hideEvent(QHideEvent* ev);

  void			SoSelectionEvent(iSoSelectionEvent* ev); // #IGNORE
  void			LayoutComponents(); // called on resize or when comps change (ex scrollers)
  QScrollBar*		MakeScrollBar(bool ver);
  void 			resizeEvent(QResizeEvent* ev); // override

private:
  void			init();
};


//////////////////////////
//   iT3DataViewFrame	//
//////////////////////////

class TA_API iT3DataViewFrame : public QWidget, public IDataViewWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(QWidget)
friend class T3DataViewFrame;
friend class iT3DataViewer;
friend class T3DataViewer;
public:
#ifndef __MAKETA__
  QPointer<iViewPanelSet> panel_set; // contains ctrl panels for all our guys
#endif
//  taiMenu* 		fileExportInventorMenu;

  iT3ViewspaceWidget*	t3vs;

  T3ExaminerViewer* 	t3viewer() {return m_t3viewer;}

  T3DataViewRoot*	root();
  virtual void		setSceneTop(SoNode* node); // set top of scene -- usually called during Render_post
  override int		stretchFactor() const {return 4;} // 4/2 default
  inline T3DataViewFrame* viewer() const {return (T3DataViewFrame*)m_viewer;}
  iT3DataViewer* 	viewerWidget() const;

  void 			NodeDeleting(T3Node* node); 
    // called when a node is deleting; basically used to deselect

  virtual void		T3DataViewClosing(T3DataView* node); // used mostly to remove from selection list
  void			RegisterPanel(iViewPanelFrame* pan);

  iT3DataViewFrame(T3DataViewFrame* viewer_, QWidget* parent = NULL); 
  ~iT3DataViewFrame(); //

public: // menu and menu overrides
  void			viewRefresh(); // rebuild/refresh the current view

public slots:
  virtual void 		fileExportInventor();
  virtual void 		viewSaved(int view_no); // connect to viewSaved on examiner viewer

public: // IDataViewWidget i/f
  override QWidget*	widget() {return this;}
protected:
  override void		Constr_impl();
  override void		Refresh_impl(); // note: we just do the lite Render_impl stuff
  
protected:
  T3ExaminerViewer* 	m_t3viewer;
  virtual void		Render_pre(); // #IGNORE
  virtual void		Render_impl();  // #IGNORE
  virtual void		Render_post(); // #IGNORE
  virtual void		Reset_impl(); // note: delegated from DataViewer::Clear_impl

protected:
//  void			hideEvent(QHideEvent* ev);
  void			showEvent(QShowEvent* ev);
  void			Showing(bool showing); 
  
private:
  void			Init();
}; //

/* T3DataViewFrame
  
  The frame can hold multiple objects, however the typical cases are:
   1 object ("single")-- ex., a graph or table view
   N objects (!"single") -- ex., a network, with viewlets, graphs, etc.
   
  We treat some things differently, depending on single or multi:
  		single		multi
  DisplayName	item name	frame name
  bg color	slave to item	

*/

class TA_API T3DataViewFrame : public DataViewer {
  // ##DEF_NAME_ROOT_Frame top-level taDataViewer object that contains one 3D data view of multiple objects
INHERITED(DataViewer)
friend class T3DataView;
friend class T3DataViewer;
public:
  enum StereoView {
    STEREO_NONE,		// use monoscopic rendering (default)
    STEREO_ANAGLYPH, 	 	// Render stereo by superimposing two images of the same scene, but with different color filters over the left and right view (or "eye").  This is a way of rendering stereo which works on any display, using color-filter glasses. Such glasses are usually cheap and easy to come by.
    STERO_QUADBUFFER,		// Render stereo by using OpenGL quad-buffers. This is the most common interface for stereo rendering for more expensive hardware devices, such as shutter glasses and polarized glasses. The well known Crystal Eyes glasses are commonly used with this type of stereo display.
    STEREO_INTERLEAVED_ROWS, 	// Interleaving / interlacing rows from the left and right eye is another stereo rendering method requiring special hardware. One example of a provider of shutter glasses working with interleaved glasses is VRex: http://www.vrex.com/
    STEREO_INTERLEAVED_COLUMNS,  // Same basic technique as SoQtViewer::STEREO_INTERLEAVED_ROWS, only it is vertical lines that are interleaved / interlaced, instead of horizontal lines.
  };

  T3DataViewRoot	root_view; // #SHOW_TREE placeholder item -- contains the actual root(s) DataView items as children
  taColor		bg_color; // #NO_ALPHA background color of the frame (note: alpha transparency value not used)
  taColor		text_color; // color to use for text in the frame -- may need to change this from default of black depending on the bg_color
  bool			headlight_on; // turn the camera headlight on for illuminating the scene -- turn off only if there is another source of light within the scenegraph -- otherwise it will be dark!
  StereoView		stereo_view;  // what type of stereo display to render, if any
  T3SavedView_List	saved_views;  // saved camera position views from viewer -- this is the persitent version copied from camera

  bool			singleMode() const
    {return (root_view.children.size == 1);}
    // true when one and only one child obj -- it can control some of our properties by default
  T3DataView*		singleChild() const; // return the single child
  
  inline iT3DataViewFrame* widget() {return (iT3DataViewFrame*)inherited::widget();} // lex override
  const iColor		GetBgColor() const; // get the effective bg color
  const iColor		GetTextColor() const; // get the effective text color
  
  virtual void		AddView(T3DataView* view); // add a view
  // #IGNORE 
  virtual T3DataView*	FindRootViewOfData(taBase* data);
  // #IGNORE looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual void		ViewAll();
  // #CAT_Display reset the camera position to view everything in the display
  virtual void		GetSavedView(int view_no);
  // #CAT_Display copy given saved view on the T3ExaminerViewer to our saved view information (for persistence) -- does not actually grab the current view, just the previously saved data -- see SaveCurView
  virtual void		SetSavedView(int view_no);
  // #CAT_Display copy our saved view to the T3ExaminerViewer -- does not go to that view, just sets data -- see GoToSavedView
  virtual void		SetAllSavedViews();
  // #CAT_Display copy all our saved views to the T3ExaminerViewer -- during initialization

  virtual void		SaveCurView(int view_no);
  // #CAT_Display save the current T3 examiner viewer view parameters to the given view (on us and the viewer) -- for programmatic usage
  virtual void		GoToSavedView(int view_no);
  // #CAT_Display tell the viewer to go to given saved view parameters (copies our parameters in case they have been locally modified programmatically)

  void	SetCameraPos(int view_no, float x, float y, float z)
  { saved_views.SetCameraPos(view_no, x,y,z); }
  // #CAT_Display for given view number, set camera position
  void	SetCameraOrient(int view_no, float x, float y, float z, float r)
  { saved_views.SetCameraOrient(view_no, x,y,z,r); }
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void	SetCameraFocDist(int view_no, float fd)
  { saved_views.SetCameraFocDist(view_no, fd); }
  // #CAT_Display for given view number, set camera focal distance

  void	GetCameraPos(int view_no, float& x, float& y, float& z)
  { saved_views.GetCameraPos(view_no, x,y,z); }
  // #CAT_Display for given view number, set camera position
  void	GetCameraOrient(int view_no, float& x, float& y, float& z, float& r)
  { saved_views.GetCameraOrient(view_no, x,y,z,r); }
  // #CAT_Display for given view number, set camera orientation -- x,y,z axis and r rotation value
  void	GetCameraFocDist(int view_no, float& fd)
  { saved_views.GetCameraFocDist(view_no, fd); }
  // #CAT_Display for given view number, set camera focal distance

  override QPixmap	GrabImage(bool& got_image);
  override bool		SaveImageAs(const String& fname = "", ImageFormat img_fmt = PNG);
  virtual  void		SetImageSize(int width, int height);
  // #CAT_Display #BUTTON set size of 3d viewer (and thus SaveImageAs image) to given size parameters

  virtual bool		SaveImageEPS(const String& fname = "");
  // #CAT_Display save image in EPS format
  virtual bool		SaveImageIV(const String& fname = "");
  // #CAT_Display save image in IV (open inventor) format

  virtual void		CopyFromViewFrame(T3DataViewFrame* cp);
  // #CAT_Display copy key view information from another view frame (saved views, other view info)

  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // we notify viewer
  void	InitLinks();
  void	CutLinks();
  TA_DATAVIEWFUNS(T3DataViewFrame, DataViewer)

protected:
  static void		SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void		SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE


  override void		Constr_impl(QWidget* gui_parent = NULL);
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
  override void		Constr_post();
  override void 	WindowClosing(CancelOp& cancel_op); // #IGNORE
  override void		Clear_impl(); // #IGNORE
  override void		Render_pre(); // #IGNORE
  override void		Render_impl();  // #IGNORE
  override void		Render_post(); // #IGNORE
  override void		Reset_impl(); //  #IGNORE
private:
  void	Copy_(const T3DataViewFrame& cp);
  void			Initialize();
  void			Destroy();
};

class TA_API T3DataViewFrame_List: public DataViewer_List { // #NO_TOKENS
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(T3DataViewFrame_List, DataViewer_List, T3DataViewFrame)
private:
  NOCOPY(T3DataViewFrame_List)
  void 	Initialize() { SetBaseType(&TA_T3DataViewFrame);}
  void	Destroy() {}
};


//////////////////////////
//   iT3DataViewer	//
//////////////////////////

class iTabWidget; // IGNORE
#ifndef __MAKETA__
class TA_API iTabBarEx : public iTabBarBase {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(iTabBarBase)
public:
  iTabBarEx(iTabWidget* parent = NULL);
  
protected:
  iTabWidget*		m_tab_widget;
  override void	contextMenuEvent(QContextMenuEvent* ev);
};

class TA_API iTabWidget : public QTabWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(QTabWidget)
friend class iTabBarEx;
public:
  iTabWidget(QWidget* parent = NULL);

  iTabBarBase* 	GetTabBar();
  
#ifndef __MAKETA__
signals:
  void 		customContextMenuRequested2(const QPoint& pos, int tab_idx);
#endif

protected:
  void 		emit_customContextMenuRequested2(const QPoint& pos, int tab_idx);
  override void contextMenuEvent(QContextMenuEvent* ev);
};

#endif

class TA_API iT3DataViewer : public iFrameViewer {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(iFrameViewer)
friend class T3DataViewer;
public:
  iTabWidget*		tw;

  inline T3DataViewer*	viewer() {return (T3DataViewer*)m_viewer;}
  iT3DataViewFrame*	iViewFrame(int idx) const; // the frame in indicated tab
  T3DataViewFrame*	viewFrame(int idx) const; // view of frame in indicate tab -- NOT necessarily same as view idx if all frames not viewed in frames

  iTabBarBase*		tabBar() { if(!tw) return NULL; return tw->GetTabBar(); }
  int			TabIndexByName(const String& nm) const;
  bool			SetCurrentTab(int tab_idx); 
  // focus indicated tab -- returns success
  bool			SetCurrentTabName(const String& tab_nm); 
  // focus indicated tab -- returns success

  void			AddT3DataViewFrame(iT3DataViewFrame* dvf, int idx = -1);
  void			UpdateTabNames();
  
  iT3DataViewer(T3DataViewer* viewer_, QWidget* parent = NULL); 
  ~iT3DataViewer(); //

public slots:
  void			AddFrame();
  void			DeleteFrame(int tab_idx);
  void			FrameProperties(int tab_idx);
  void			FocusFirstTab(); // hack to get first tab focussed on load
  
protected: // IDataViewWidget i/f
  int 			last_idx;
  override void		Refresh_impl();
  
  virtual void		FillContextMenu_impl(taiMenu* menu, int tab_idx);
  
protected slots:
  void 			tw_customContextMenuRequested2(const QPoint& pos, int tab_idx);
  void			tw_currentChanged(int tab_idx);

protected:
  override void 	focusInEvent(QFocusEvent* ev);

private:
  void			Init();
};


class TA_API T3DataViewer : public FrameViewer {
  // ##DEF_NAME_ROOT_T3Frames ##DEF_NAME_STYLE_2 top-level taDataViewer object that contains one 3D data view of multiple objects
INHERITED(FrameViewer)
friend class T3DataView;
public:
  static T3DataViewFrame* GetBlankOrNewT3DataViewFrame(taBase* obj);
    // gets the first blank, or a new, frame, in the default proj browser for the obj -- used by all the view-creating routines for various viewable objs (tables, nets, etc.)
    
  T3DataViewFrame_List	frames; // 

  override bool		isRootLevelView() const {return true;} 
  inline iT3DataViewer*	widget() {return (iT3DataViewer*)inherited::widget();} // lex override

  iTabBarBase*		tabBar() { if(!widget()) return NULL; return widget()->tabBar(); }

  virtual T3DataView*	FindRootViewOfData(taBase* data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual T3DataViewFrame* FirstEmptyT3DataViewFrame(); // find the first frame with no contents (to avoid making a new one)
  virtual T3DataViewFrame* NewT3DataViewFrame(); // #MENU #MENU_ON_Object #MENU_CONTEXT create and map a new frame
  void			FrameChanged(T3DataViewFrame* frame); 
    // called by frame if changes, we update names
    
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const T3DataViewer& cp);
  TA_DATAVIEWFUNS(T3DataViewer, FrameViewer)

protected:
  // from taDataView
  override void		DataChanged_Child(taBase* child, int dcr, void* op1, void* op2);
  override void		DoActionChildren_impl(DataViewAction act); // just one act

  // DataViewer
  override void 	Constr_impl(QWidget* gui_parent);
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
//override void 	ResolveChanges(CancelOp& cancel_op); // nn: no "dirty" for T3
  override void		Reset_impl(); //  #IGNORE
  override void 	GetWinState_impl(); // set gui state; only called if mapped
  override void 	SetWinState_impl(); // fetch gui state; only called if mapped
  
  virtual void 		ConstrFrames_impl();

private:
  void			Initialize();
  void			Destroy();
};


#endif



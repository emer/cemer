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
# include <QTabBar>
# include <qwidget.h>
# include <Inventor/nodes/SoSeparator.h>
# include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#endif

// externals
class taiClipData;
class taiMimeItem;
class taiMimeSource;
class TDCoord;
class FloatTransform;
class SoPath; // #IGNORE
class SbPList; // #IGNORE
class SoCamera; // #IGNORE
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

SoPtr_Of(T3Node);

class TA_API t3Misc {
// global params and funcs for T3 viewing system
public:
  static const float	pts_per_geom; // #DEF_72 chars*pt size / so unit
  static const float	geoms_per_pt; // number of geoms per point (1/72)
  static const float	char_ht_to_wd_pts; // ratio of ht to wd in metrics: 12/8
  static const float	char_base_fract; // fraction of total ht below baseline
};

///////////////////////////////////////
//	custom T3 viewer object
///////////////////////////////////////

// goals: get rid of unused buttons, and add new ones(?)
// fix view button to regularize the view as well.

class SbPList; // #IGNORE

class TA_API T3ExaminerViewer_qobj : public QObject {
  Q_OBJECT
  INHERITED(QObject)
public:
  T3ExaminerViewer*	s;
  T3ExaminerViewer_qobj(T3ExaminerViewer* s_) {s = s_;}
  ~T3ExaminerViewer_qobj() {s = NULL;}

public slots:
  void interactbuttonClicked();
  void viewbuttonClicked();
  void homebuttonClicked();
  void sethomebuttonClicked();
  void viewallbuttonClicked();
  void seekbuttonClicked();
  void snapshotbuttonClicked();
  void printbuttonClicked();
};

class TA_API T3ExaminerViewer : public SoQtExaminerViewer {
  SOQT_OBJECT_HEADER(T3ExaminerViewer, SoQtExaminerViewer);
public:
  T3ExaminerViewer(iT3ViewspaceWidget* parent = NULL,
		   const char * name = NULL,
		   bool embed = true);
  ~T3ExaminerViewer();

  iT3ViewspaceWidget*		t3vw;

  static void zoom(SoCamera* cam, const float diffvalue);
  // zoom in/out by given amount: adjusts both camera pos and focal length

  T3DataViewFrame* GetFrame();
  // get my owning frame, from t3vw

  override void viewAll();
  // make this actually fill the damn screen!
  override void saveHomePosition();
  override void resetToHomePosition();
  // use our saved values in frame dude

public: // slot callbacks
  void interactbuttonClicked();
  void viewbuttonClicked();
  void homebuttonClicked();
  void sethomebuttonClicked();
  void viewallbuttonClicked();
  void seekbuttonClicked();
  void snapshotbuttonClicked();
  void printbuttonClicked();

protected:
  T3ExaminerViewer_qobj*	q;
  QPushButton* interactbutton;
  QPushButton* viewbutton;
  QPushButton* seekbutton;
  SbPList*     priv_button_list;

  override void processEvent(QEvent* ev_);
  override void createViewerButtons(QWidget* parent, SbPList* buttonlist);
  override QWidget * buildRightTrim(QWidget * parent);
  virtual void fixViewerButtons(); // fix the viewer buttons to actually fit..

#ifndef __MAKETA__ // doesn't like SbBool..
  override void setSeekMode(SbBool enable); // #IGNORE override standard one to do nothing
  virtual void setSeekMode_doit(SbBool enable);
  // #IGNORE this is the one that calls the original -- only activated by the user!
#endif
};

//////////////////////////
//   T3DataView_List	//
//////////////////////////

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
  FloatTransform*	m_transform;  // #READ_ONLY transform, created only if not unity

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
  virtual SoQtViewer*	GetViewer();
  // #IGNORE get the SoQtViewer that contains us

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
//TODO  virtual BrDataLink*	GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
//  override bool		HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not
//obs  virtual String 	GetName() const = 0; // base name of item (could be blank)
  virtual void		ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void		UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node
  
  virtual void		ViewProperties(); // #MENU #VIEWMENU show the view properties for this object

  override void		CutLinks();
  TA_DATAVIEWFUNS(T3DataView, taDataView);

public: // ISelectable interface (only not in IDataLinkClient)
  override ISelectableHost* host() const; //
  override MemberDef*	md() const {return m_md;}
  override ISelectable*	par() const;
//  override taiDataLink* par_link() const; // from parent data
//  override MemberDef* 	par_md() const; // as for par_link
  override taiDataLink*	viewLink() const; // data of the link
  override GuiContext	shType() const {return GC_DUAL_DEF_DATA;} 
  override taiDataLink*	clipParLink(GuiContext sh_typ = GC_DEFAULT) const; // not par_link 
/*override int		EditAction_(ISelectable_PtrList& sel_items, int ea);
  override void 		FillContextMenu(ISelectable_PtrList& sel_items, taiActions* menu);
  override taiClipData*	GetClipData(const ISelectable_PtrList& sel_items, int src_edit_action,
    bool for_drag) const;
  override taiClipData*	GetClipDataSingle(int src_edit_action, bool for_drag) const;
  override taiClipData*	GetClipDataMulti(const ISelectable_PtrList& sel_items, 
    int src_edit_action, bool for_drag) const;
  override int		QueryEditActions_(taiMimeSource* ms) const; */
protected:
//  override void		FillContextMenu_impl(taiActions* menu, GuiContext sh_typ);
  override void 	QueryEditActionsS_impl_(int& allowed, int& forbidden,
    GuiContext sh_typ) const;

protected:
  MemberDef*		m_md; // memberdef of this item in its parent

  void			setNode(T3Node* node); // make changes via this
  
  virtual void		AddRemoveChildNode_impl(SoNode* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
//override void 	ChildAdding(taDataView* child) {} // #IGNORE called from list;
  override void		ChildRemoving(taDataView* child); // #IGNORE called from list; we also forward to DataViewer; we also remove visually
  virtual void		Constr_Node_impl() {} // create the node_so rep -- called in RenderPre, null'ed in Clear
//nn  virtual void		DestroyPanels();

  virtual void		OnWindowBind_impl(iT3DataViewFrame* vw) {} // override for something this class
  override void		Clear_impl();
  virtual void		ReInit_impl(); // default just calls clear() on the so, if it exists
  override void		Render_pre(); //
  override void		Render_impl();
//  override void		Render_post();
//  override void		Reset_impl();

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
    
  override void 	ChildRemoving(taDataView* child); 
  
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

  SoQtRenderArea* 	renderArea() {return m_renderArea;}
  void			setRenderArea(SoQtRenderArea* value); // must be called once, after creation
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
  SoQtRenderArea* 	m_renderArea;
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
friend class T3DataViewer;
public:
#ifndef __MAKETA__
  QPointer<iViewPanelSet> panel_set; // contains ctrl panels for all our guys
#endif
//  taiMenu* 		fileExportInventorMenu;

  iT3ViewspaceWidget*	t3vs;

  SoQtViewer* 		ra() {return m_ra;} //TODO: maybe should not cache; should get from body()
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

public: // IDataViewWidget i/f
  override QWidget*	widget() {return this;}
protected:
  override void		Constr_impl();
  override void		Refresh_impl(); // note: we just do the lite Render_impl stuff
  
protected:
  SoQtViewer* 		m_ra;
  virtual void		Render_pre(); // #IGNORE
  virtual void		Render_impl();  // #IGNORE
  virtual void		Render_post(); // #IGNORE
  virtual void		Reset_impl(); // note: delegated from DataViewer::Clear_impl

protected:
  void			hideEvent(QHideEvent* ev);
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
  FloatTDCoord		camera_pos;	// position of camera in view
  FloatRotation		camera_orient;	// orientation of camera in view
  float			camera_focdist; // focalDistance of camera in view
  taColor		bg_color; // #NO_ALPHA background color of the frame (note: alpha transparency value not used)
  bool			headlight_on; // turn the camera headlight on for illuminating the scene -- turn off only if there is another source of light within the scenegraph -- otherwise it will be dark!
  StereoView		stereo_view;  // what type of stereo display to render, if any

  bool			singleMode() const
    {return (root_view.children.size == 1);}
    // true when one and only one child obj -- it can control some of our properties by default
  T3DataView*		singleChild() const; // return the single child
  
  inline iT3DataViewFrame* widget() {return (iT3DataViewFrame*)inherited::widget();} // lex override
  const iColor		GetBgColor() const; // get the effective bg color
  
  virtual void		AddView(T3DataView* view); // add a view
  virtual T3DataView*	FindRootViewOfData(TAPtr data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual void		ViewAll();
  // reset the camera position to view everything in the display
  virtual void		GetCameraPosOrient();
  // get camera's current position and orientation from viewer into my fields (for saving)
  virtual void		SetCameraPosOrient();
  // set camera's current position and orientation from viewer from my fields (during loading)

  override QPixmap	GrabImage(bool& got_image);
  override bool		SaveImageAs(const String& fname = "", ImageFormat img_fmt = EPS);
  virtual  void		SetImageSize(int width, int height);
  // set size of SaveImageAs image to given size parameters

  virtual bool		SaveImageEPS(const String& fname = "");
  // save image in EPS format
  virtual bool		SaveImageIV(const String& fname = "");
  // save image in IV (open inventor) format

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
  override void 	Dump_Save_pre();
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
class TA_API iTabBarEx : public QTabBar {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(QTabBar)
public:
  iTabBarEx(iTabWidget* parent = NULL);
  
protected:
  iTabWidget*		m_tab_widget;
  override void		contextMenuEvent(QContextMenuEvent* ev);
};

class TA_API iTabWidget : public QTabWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS decodes tab for context menu
  Q_OBJECT
INHERITED(QTabWidget)
friend class iTabBarEx;
public:
  iTabWidget(QWidget* parent = NULL);
  
#ifndef __MAKETA__
signals:
  void 		customContextMenuRequested2(const QPoint& pos, int tab_idx);
#endif

protected:
  void 			emit_customContextMenuRequested2(const QPoint& pos,
     int tab_idx);
  override void		contextMenuEvent(QContextMenuEvent* ev);
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
  override void		Refresh_impl();
  
  virtual void		FillContextMenu_impl(taiMenu* menu, int tab_idx);
  
protected slots:
  void 			tw_customContextMenuRequested2(const QPoint& pos, int tab_idx);

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

  virtual T3DataView*	FindRootViewOfData(TAPtr data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

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
  override void		DataChanged_Child(TAPtr child, int dcr, void* op1, void* op2);
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



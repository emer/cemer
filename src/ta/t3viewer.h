// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
#include "ta_qtviewer.h"
#include "t3node_so.h"
#include "ta_def.h"

#include "ta_TA_type.h"

#include "igeometry.h"
#include "ta_geometry.h"
#include "safeptr_so.h"

#ifndef __MAKETA__
  #include <qevent.h>
  #include <qwidget.h>
  #include <Inventor/nodes/SoSeparator.h>
#endif

// externals
class taiClipData;
class taiMimeItem;
class taiMimeSource;
class TDCoord;
class FloatTransform;
class SoPath; // #IGNORE
class T3Node;

// forwards
class T3DataView;
class T3DataViewPar;
class T3DataViewRoot;

class T3DataView_List;
class iT3ViewspaceWidget;
class iSoSelectionEvent; // #IGNORE
class iT3DataViewFrame; // #IGNORE
class iT3DataViewer; // #IGNORE
class T3DataViewFrame;
class T3DataViewer;

SoPtr_Of(T3Node);

class TA_API t3Misc {
// global params and funcs for T3 viewing system
public:
  static const float	pts_per_geom; // #DEF_72 chars*pt size / so unit
  static const float	geoms_per_pt; // number of geoms per point (1/72)
  static const float	char_ht_to_wd_pts; // ratio of ht to wd in metrics: 12/8
  static const float	char_base_fract; // fraction of total ht below baseline
};

//////////////////////////
//   T3DataView_List	//
//////////////////////////

class TA_API T3DataView_List: public DataView_List { // ##NO_TOKENS
INHERITED(DataView_List)
friend class T3DataView;
public:
  TA_DATAVIEWLISTFUNS(T3DataView_List, DataView_List, T3DataView)
private:
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
  // ##NO_TOKENS base class for 3d-based DataView objects
#ifndef __MAKETA__
  typedef taDataView	inherited;
#endif
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

  virtual bool		hasChildren() const {return false;}
  virtual bool		selectEditMe() const { return false; }
  // for selection events -- if true, then edit this view object instead of the data() object

  virtual bool		expandable() const {return false;}
  virtual void		setExpandable(bool) {}
  DATAVIEW_PARENT(T3DataView) // always a T3DataView (except root guy)
  FloatTransform*	transform(bool auto_create = false);  // transform, in Inventor coords

  override bool		isMapped() const; // only true if in gui mode and gui stuff exists 
  T3Node*		node_so() const {return m_node_so.ptr();} //
  virtual T3DataViewRoot* root();
//obs  T3DataViewer*		viewer() const;
//obs  iT3DataViewer*	viewer_win() const {return (iT3DataViewer*)viewer_win_();}

  void 			AddRemoveChildNode(SoNode* node, bool adding);
    // can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
  override void 	Close(); // usually delegates to parent->CloseChild
  virtual void 		CloseChild(taDataView* child) {}
  virtual void		BuildAll() {}
  // subclass-dependent operation -- reinvoked after major update -- builds any sub-dataview objects, but not the corresponding So guys (which is done in render)
  override void		DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);
  override void		DataDestroying(); // called by data when it is destroying -- usual action is to destroy ourself
//nn  void			EditAction(int ea); // do the edit action; invokes _impl
//nn?  virtual void		InsertItem(T3DataView* item, T3DataView* after); // used by child's constructor
//  void			SetPos(const TDCoord& pos); // sets origin, also doing ta->so coordinate flip
//  virtual void		SetPos(int x, int y, int z); // sets origin, also doing ta->so coordinate flip

  virtual void		OnWindowBind(iT3DataViewFrame* vw); // called after the viewer creates/fills the main window (for dataviews embedded in main viewer only), or when DataView added to existing viewer
//TODO  virtual BrDataLink*	GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
//  override bool		HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not
//obs  virtual String 	GetName() const = 0; // base name of item (could be blank)
  virtual void		ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void		UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node
  
  override void		UpdateAfterEdit(); //note: not _impl
  override void		CutLinks();
  TA_DATAVIEWFUNS(T3DataView, taDataView);

public: // ISelectable interface (only not in IDataLinkClient)
  override MemberDef*	md() const {return m_md;} // memberdef of this item in its parent
  override taiDataLink* par_link() const; // from parent data
  override MemberDef* 	par_md() const; // as for par_link
  override String	view_name() const;
  override ISelectableHost* host() const;
//  override taiClipData*	GetClipData(int src_edit_action, bool for_drag);
//  override int		QueryEditActions(taiMimeSource* ms) const; // simpler version uses Query
//  override taiMimeItem*	GetMimeItem();
  override void		ChildClearing(taDataView* child); // NOTE: child is always a T3DataView
  virtual void		ChildRendered(taDataView* child); //  NOTE: child is always a T3DataView
protected:
//  override int		EditAction_impl(taiMimeSource* ms, int ea);
  override void		FillContextMenu_EditItems_impl(taiMenu* menu, int allowed);
  override void		FillContextMenu_impl(taiMenu* menu);
  override void		QueryEditActionsS_impl_(int& allowed, int& forbidden) const;

public:
  virtual void		fileNew() {} // this section for all the delegated menu commands
  virtual void		fileOpen() {} // this section for all the delegated menu commands
  virtual void		fileSave() {} // this section for all the delegated menu commands
  virtual void		fileSaveAs() {} // this section for all the delegated menu commands
  virtual void		fileClose() {}

protected:
  MemberDef*		m_md; // memberdef of this item in its parent
  T3NodePtr		m_node_so; // Inventor node

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
protected:
  T3DataView*		last_child_node; // #IGNORE last child node created, so we can pass to createnode
//nn  DataLink_QObj*	qobj; // #IGNORE delegate object, when we need to connect or signal
//TODO  virtual void		Assert_QObj(); // makes sure the qobj is created
//  override void		DataDataChanged_impl(int dcr, void* op1, void* op2); // called when the data item has changed, esp. ex lists and groups, EXCEPT UpdateAfterEdit, which gets dispatched direct
  override void		DataUpdateAfterEdit_impl(); // called by data for an UAE

private:
  void			Initialize();
  void			Destroy();
};

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

class TA_API T3DataViewPar: public T3DataView { // T3DataView that has child T3DataView's
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
  void			Initialize() {}
  void			Destroy() {CutLinks();}
};

class TA_API T3DataViewRoot: public T3DataViewPar { // Root item for a viewwidget type
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class iT3DataViewFrame;
friend class T3DataViewFrame;
public:
  ISelectableHost*	host; // ss/b set by owner

//note: typically never has a non-null parent, because it is rooted in non-T3DataView  
  override T3DataViewRoot* root() {return this;}

  T3_DATAVIEWFUNS(T3DataViewRoot, T3DataViewPar)

protected:
  override void		Constr_Node_impl();

private:
  void			Initialize() {host = NULL;}
  void			Destroy() {}
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

  void			deleteScene(); // deletes the scene -- usually only called internally, not by clients of this component
  void 			ContextMenuRequested(const QPoint& pos); // #IGNORE called from render area

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
  override void		UpdateSelectedItems_impl(); 

protected:
  static void		SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void		SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE

  QScrollBar*		m_horScrollBar;
  QScrollBar*		m_verScrollBar;
  SoQtRenderArea* 	m_renderArea;
//nn  iRenderAreaWrapper*	m_raw; // render area wrapper
  SoSeparatorPtr	m_root_so; //
  SoNode*		m_scene; // actual top item set by user
  SelectionMode		m_selMode; // #IGNORE true adds a SoSelection node, and selection call back

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
public:
//  taiMenu* 		fileExportInventorMenu;

  iT3ViewspaceWidget*	t3vs;

  SoQtViewer* 		ra() {return m_ra;} //TODO: maybe should not cache; should get from body()
  T3DataViewRoot*	root();
  virtual void		setSceneTop(SoNode* node); // set top of scene -- usually called during Render_pre
  override int		stretchFactor() const {return 4;} // 4/2 default
  inline T3DataViewFrame* viewer() {return (T3DataViewFrame*)m_viewer;}
  iT3DataViewer* 	viewerWidget() const;

  virtual void		T3DataViewClosing(T3DataView* node); // used mostly to remove from selection list

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

private:
  void			Init();
};


class TA_API T3DataViewFrame : public DataViewer {
  // top-level taDataViewer object that contains one 3D data view of multiple objects
INHERITED(DataViewer)
friend class T3DataView;
friend class T3DataViewer;
public:
  T3DataViewRoot	root_view; // #SHOW_TREE placeholder item -- contains the actual root(s) DataView items as children
  FloatTDCoord		camera_pos;	// position of camera in view
  FloatRotation		camera_orient;	// orientation of camera in view
  float			camera_focdist; // focalDistance of camera in view

  inline iT3DataViewFrame* widget() {return (iT3DataViewFrame*)inherited::widget();} // lex override

  virtual void		AddView(T3DataView* view); // add a view
  virtual T3DataView*	FindRootViewOfData(TAPtr data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual void		GetCameraPosOrient();
  // get camera's current position and orientation from viewer into my fields (for saving)
  virtual void		SetCameraPosOrient();
  // set camera's current position and orientation from viewer from my fields (during loading)

  void	DataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // we notify viewer
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const T3DataViewFrame& cp);
  COPY_FUNS(T3DataViewFrame, DataViewer)
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
  void			Initialize();
  void			Destroy();
};

class TA_API T3DataViewFrame_List: public DataViewer_List { // #NO_TOKENS
INHERITED(DataViewer_List)
public:
  TA_DATAVIEWLISTFUNS(T3DataViewFrame_List, DataViewer_List, T3DataViewFrame)
private:
  void 	Initialize() { SetBaseType(&TA_T3DataViewFrame);}
  void	Destroy() {}
};


//////////////////////////
//   iT3DataViewer	//
//////////////////////////

class TA_API iT3DataViewer : public iFrameViewer {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(iFrameViewer)
friend class T3DataViewer;
public:
  QTabWidget*		tw;

  inline T3DataViewer*	viewer() {return (T3DataViewer*)m_viewer;}

  void			AddT3DataViewFrame(iT3DataViewFrame* dvf, int idx = -1);
  void			UpdateTabNames();
  
  iT3DataViewer(T3DataViewer* viewer_, QWidget* parent = NULL); 
  ~iT3DataViewer(); //

protected: // IDataViewWidget i/f
  override void		Refresh_impl();
  
private:
  void			Init();
};


class TA_API T3DataViewer : public FrameViewer {
  // top-level taDataViewer object that contains one 3D data view of multiple objects
INHERITED(FrameViewer)
friend class T3DataView;
public:
  static T3DataViewFrame* GetBlankOrNewT3DataViewFrame(taBase* obj);
    // gets the first blank, or a new, frame, in the default proj browser for the obj -- used by all the view-creating routines for various viewable objs (tables, nets, etc.)
    
  T3DataViewFrame_List	frames; // 

  inline iT3DataViewer*	widget() {return (iT3DataViewer*)inherited::widget();} // lex override

  virtual T3DataView*	FindRootViewOfData(TAPtr data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual T3DataViewFrame* FirstEmptyT3DataViewFrame(); // find the first frame with no contents (to avoid making a new one)
  virtual T3DataViewFrame* NewT3DataViewFrame(); // #MENU #MENU_ON_Object #MENU_CONTEXT create and map a new frame
  void			FrameChanged(T3DataViewFrame* frame); 
    // called by frame if changes, we update names
  void	InitLinks();
  void	CutLinks();
  void	Copy_(const T3DataViewer& cp);
  COPY_FUNS(T3DataViewer, FrameViewer)
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
  
  virtual void 		ConstrFrames_impl();

private:
  void			Initialize();
  void			Destroy();
};


#endif



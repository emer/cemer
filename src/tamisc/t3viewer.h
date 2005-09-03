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

// t3viewer.h: basic types for objects that can be independently viewed in space

#ifndef T3VIEWER_H
#define T3VIEWER_H

#include "ta_group.h"
#include "qtdefs.h"
//#include "ta_qt.h"
#include "ta_qtviewer.h"
//#include "ta_qtdata_def.h"
#include "t3node_so.h"
#include "ta_TA_type.h"
#include "tamisc_TA_type.h"

#include "igeometry.h"
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

// forwards
class T3DataView;
class T3DataViewPar;
class T3DataViewRoot;

class T3DataView_List;
class iT3ViewspaceWidget;
class iSoSelectionEvent; // #IGNORE
class iT3DataViewer; // #IGNORE
class T3DataViewer;

SoPtr_Of(T3Node);

//////////////////////////
//   T3DataView_List	//
//////////////////////////

class T3DataView_List: public taList<T3DataView> { // ##NO_TOKENS
#ifndef __MAKETA__
  typedef taList<T3DataView> inherited;
#endif
friend class T3DataView;
public:
  override TAPtr 	SetOwner(TAPtr); // #IGNORE
  override void		CutLinks();
  TA_BASEFUNS(T3DataView_List);
protected:
  override void 	El_Done_(void* it_);
  override void*	El_Own_(void* it);
  override void		El_disOwn_(void* it);
  override void		El_SetIndex_(void* it_, int idx);
  T3DataView*		data_view; // #IGNORE our owner, when owned by a T3DataView (normal case)
private:
  void			Initialize();
  void			Destroy();
};

//////////////////////////
//   T3DataView		//
//////////////////////////

/*
  DataView objects that have 3D reps
  Usually inherit directly from T3DataView, or if it is a viewer-rooted
  object, T3DataViewV

*/

class T3DataView: public taDataView, public virtual ISelectable {
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

  int			flags; // #READ_ONLY #NO_SAVE any of T3DataViewFlags TODO: tbd
  int			idx; // #READ_ONLY #NO_SAVE index of ourselves in parent list
  FloatTransform*	m_transform;  // #READ_ONLY transform, created only if not unity

  virtual bool		hasChildren() const {return false;}

  virtual bool		expandable() const {return false;}
  virtual void		setExpandable(bool) {}
  FloatTransform*	transform(bool auto_create = false);  // transform, in Inventor coords

  taiDataLink*		link() const {return (taiDataLink*)IDataLinkClient::m_link;}
  T3Node*		node_so() const {return m_node_so.ptr();} //
  virtual T3DataViewRoot* root();
//obs  T3DataViewer*		viewer() const;
//obs  iT3DataViewer*	viewer_win() const {return (iT3DataViewer*)viewer_win_();}

  void 			AddRemoveChildNode(SoNode* node, bool adding);
    // can be used for manually using non-default T3Node items in a child; add in Pre_impl, remove in Clear_impl
  override void 	Close(); // usually delegates to parent->CloseChild
  virtual void 		CloseChild(taDataView* child) {}
  virtual void		BuildAll() {} // subclass-dependent operation -- reinvoked after major update
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2);
  virtual void		DataDestroying(); // called by data when it is destroying -- usual action is to destroy ourself
//nn  void			EditAction(int ea); // do the edit action; invokes _impl
//nn?  virtual void		InsertItem(T3DataView* item, T3DataView* after); // used by child's constructor
//  void			SetPos(const TDCoord& pos); // sets origin, also doing ta->so coordinate flip
//  virtual void		SetPos(int x, int y, int z); // sets origin, also doing ta->so coordinate flip

  virtual void		OnWindowBind(iT3DataViewer* vw); // called after the viewer creates/fills the main window (for dataviews embedded in main viewer only), or when DataView added to existing viewer
//TODO  virtual BrDataLink*	GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
//  override bool		HasChildItems() {return false;} // used when node first created, to control whether we put a + expansion on it or not
//obs  virtual String 	GetName() const = 0; // base name of item (could be blank)
  virtual void		ReInit(); // perform a reinitialization, particularly of visual state -- overload _impl
  virtual void		UpdateChildNames(T3DataView*); // #IGNORE update child names of the indicated node

  override void		CutLinks();
  TA_DATAVIEWFUNS(T3DataView);

public: // ISelectable interface (only not in IDataLinkClient)
  override MemberDef*	md() const {return m_md;} // memberdef of this item in its parent
  override taiDataLink* par_link() const; // from parent data
  override MemberDef* 	par_md() const; // as for par_link
  override String	view_name() const;
  override IDataViewHost* host() const;
//  override taiClipData*	GetClipData(int src_edit_action, bool for_drag);
//  override int		GetEditActions(taiMimeSource* ms) const; // simpler version uses Query
//  override taiMimeItem*	GetMimeItem();
  override void		ChildClearing(taDataView* child); // NOTE: child is always a T3DataView
  virtual void		ChildRendered(taDataView* child); //  NOTE: child is always a T3DataView
protected:
//  override int		EditAction_impl(taiMimeSource* ms, int ea);
  override void		FillContextMenu_EditItems_impl(taiMenu* menu, int allowed);
  override void		FillContextMenu_impl(taiMenu* menu);
  override void		GetEditActionsS_impl_(int& allowed, int& forbidden) const;

public:
  virtual void		fileNew() {} // this section for all the delegated menu commands
  virtual void		fileOpen() {} // this section for all the delegated menu commands
  virtual void		fileSave() {} // this section for all the delegated menu commands
  virtual void		fileSaveAs() {} // this section for all the delegated menu commands
  virtual void		fileClose() {}

protected:
  MemberDef*		m_md; // memberdef of this item in its parent
  T3NodePtr		m_node_so; // Inventor node

  virtual void		Assert_Adapter(); // makes sure the QObject adapter is created

  virtual void		AddRemoveChildNode_impl(SoNode* node, bool adding); // generic base uses SoSeparator->addChild()/removeChild()-- replace to change
  virtual void 		ChildAdding(T3DataView* child) {} // #IGNORE called from list;
  virtual void 		ChildRemoving(T3DataView* child); // #IGNORE called from list; we also forward to DataViewer; we also remove visually
  virtual void		Constr_Node_impl() {} // create the node_so rep -- called in RenderPre, null'ed in Clear
//nn  virtual void		DestroyPanels();

  virtual void		OnWindowBind_impl(iT3DataViewer* vw) {} // override for something this class
  override void		Clear_impl(taDataView* par = NULL);
  virtual void		ReInit_impl(); // default just calls clear() on the so, if it exists
  override void		Render_pre(taDataView* par = NULL); //
  override void		Render_impl();
//  override void		Render_post();
//  override void		Reset_impl();

  // these are provided primarily for where the parent does the action on all children
  virtual void		Clear_impl_children() {}
  virtual void		Render_pre_children(taDataView* par = NULL) {} //
//nn  virtual void		Render_impl_children() {}
//nn  virtual void		Render_post_children() {}
//nn  virtual void		Reset_impl_children() {}

  virtual void		DataChanged_impl(T3DataView* nd, int dcr, void* op1, void* op2) {} // called for each node when the data item has changed, esp. ex lists and groups
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

#define T3_DATAVIEWFUNS(x) \
  TA_BASEFUNS(x);


//////////////////////////
//   T3DataView_PtrList	//
//////////////////////////

class T3DataView_PtrList: public taPtrList<T3DataView> { // for selection lists, aux lists, etc.
#ifndef __MAKETA__
typedef taPtrList<T3DataView> inherited;
#endif
public:

protected:
  override void*	El_Ref_(void* it) { taBase::Ref((taBase*)it); return it; }// when pushed
  override void* 	El_unRef_(void* it) { taBase::unRef((taBase*)it); return NULL; }
};


//////////////////////////
//   T3DataViewPar	//
//////////////////////////

class T3DataViewPar: public T3DataView { // T3DataView that has child T3DataView's
#ifndef __MAKETA__
typedef T3DataView inherited;
#endif
public:
  T3DataView_List	children; // #SHOW #READ_ONLY #BROWSE
  override bool		hasChildren() const {return (children.size > 0);}

  override void		OnWindowBind(iT3DataViewer* vw);
  override void 	CloseChild(taDataView* child);
  override void		ReInit(); //note: does a depth-first calls to children, before self

  override void		InitLinks();
  override void		CutLinks();
  T3_DATAVIEWFUNS(T3DataViewPar)

protected:
  override void		Render_impl();
  override void		Render_post();
  override void		Reset_impl();

  // these are provided primarily for where the parent does the action on all children
  // default action is to iterate all children and delegate action to the child
  override void		Clear_impl_children();
  override void		Render_pre_children(taDataView* par = NULL);
  virtual void		Render_impl_children();
  virtual void		Render_post_children();
  virtual void		Reset_impl_children();

private:
  void			Initialize() {}
  void			Destroy() {CutLinks();}
};

class T3DataViewRoot: public T3DataViewPar { // Root item for a viewwidget type
#ifndef __MAKETA__
typedef T3DataViewPar inherited;
#endif
friend class iT3DataViewer;
friend class T3DataViewer;
public:
  IDataViewHost*	host; // ss/b set by owner

  override T3DataViewRoot* root() {return this;}

  virtual T3DataView*	GetViewFromPath(const SoPath* path) const; // #IGNORE search path backwards to find the innermost T3DataView

  T3_DATAVIEWFUNS(T3DataViewRoot)

protected:
  override void		Constr_Node_impl();

private:
  void			Initialize() {host = NULL;}
  void			Destroy() {}
};

/*obs
class T3DataView_List: public taList<T3DataView> { // list to own T3DataView objects
public:
  virtual void		Render(SoGroup* par_so); // renders all the items
  virtual void		Clear(); // clears all the items (not: *not* the same as deleting them -- they stay)

  // pseudo-taDataView methods/events -- managed by dvf
  void 			SetData(TAPtr ta); // set the data to which this points -- must be subclass of data_base
  virtual void		DataDestroying(){} // notify a T3DataView parent
  virtual void		DataDataChanged(int dcr, void* op1 = NULL, void* op2 = NULL); // notify a T3DataView parent

  void			InitLinks();
  void			CutLinks();
  TA_BASEFUNS(T3DataView_List)
protected:
  T3DataViewForwarder*	dvf; // #IGNORE actual view client -- we forward its events to our like-named methods
private:
  void			Initialize();
  void			Destroy();
}; */


//////////////////////////
//   iSoSelectionEvent	//
//////////////////////////

class iSoSelectionEvent { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS note: this is not a Qt event
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
class iT3ViewspaceWidget: public QWidget { // widget that encapsulates an Inventor viewer; adds context menu handling, and optional scroll bars
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
  void 			emit_contextMenuRequested(const QPoint& pos); // #IGNORE

  iT3ViewspaceWidget(QWidget* parent = NULL);
  ~iT3ViewspaceWidget();

#ifndef __MAKETA__
signals:
  void 			contextMenuRequested(const QPoint& pos);
  void			SoSelectionEvent(iSoSelectionEvent* ev);
  void			initScrollBar(QScrollBar* sb); // orientation will be in sb
#endif

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

  void			emit_SoSelectionEvent(iSoSelectionEvent* ev); // #IGNORE
  void			LayoutComponents(); // called on resize or when comps change (ex scrollers)
  QScrollBar*		MakeScrollBar(bool ver);
  void 			resizeEvent(QResizeEvent* ev); // override

private:
  void			init();
};


//////////////////////////
//   iT3DataViewer	//
//////////////////////////

class iT3DataViewer : public iTabDataViewer {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS outer widget that contains 3D data views
  Q_OBJECT
friend class T3DataViewer;
public:
  taiMenu* 		fileExportInventorMenu;

  QSplitter*		splMain; // main splitter
  iT3ViewspaceWidget*	t3vs;

  SoQtViewer* 		ra() {return m_ra;} //TODO: maybe should not cache; should get from body()
  T3DataViewRoot*	root();
  virtual void		setSceneTop(SoNode* node); // set top of scene -- usually called during Render_pre
  T3DataViewer*		viewer() {return (T3DataViewer*)m_viewer;}

  virtual void		T3DataViewClosing(T3DataView* node); // used mostly to remove from selection list

  iT3DataViewer(void* root_, TypeDef* typ_, T3DataViewer* viewer_, QWidget* parent = NULL);
  ~iT3DataViewer(); //

public: // menu and menu overrides
  void			viewRefresh(); // rebuild/refresh the current view

public slots:
  virtual void 		fileExportInventor();

protected slots:
  void 			vs_contextMenuRequested(const QPoint& pos);
  void			vs_SoSelectionEvent(iSoSelectionEvent* ev);


protected:
  SoQtViewer* 		m_ra;
  void*			m_root;
  TypeDef*		m_typ;
  void 			Constr_Menu_impl(); // override
  void			Constr_Body_impl(); // override
  virtual void		Render_pre(); // #IGNORE
  virtual void		Render_impl();  // #IGNORE
  virtual void		Render_post(); // #IGNORE
  virtual void		Reset_impl(); // note: delegated from DataViewer::Clear_impl

private:
#ifndef __MAKETA__
  typedef iTabDataViewer inherited; // #IGNORE
#endif
};


//////////////////////////
//   T3DataViewer	//
//////////////////////////

class T3DataViewer : public DataViewer {
  // top-level DataViewer object that contains 3D data views
#ifndef __MAKETA__
  typedef DataViewer	inherited;
#endif
friend class T3DataView;
friend class iT3DataViewer;
public:
  T3DataViewRoot	root_view; // #BROWSE placeholder item -- contains the actual root(s) DataView items as children

  iT3DataViewer*	viewer_win() {return (iT3DataViewer*)m_window;}

  virtual void		AddView(T3DataView* view); // add a view
  override void		OpenNewWindow();

  void			InitLinks();
  void			CutLinks();
  TA_DATAVIEWFUNS(T3DataViewer)
protected:
  static void		SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void		SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE


  override void		OpenNewWindow_impl(); // #IGNORE
  override void 	WindowClosing(bool& cancel); // #IGNORE
  override void		Clear_impl(taDataView* par = NULL); // #IGNORE
  override void		Render_pre(taDataView* par = NULL); // #IGNORE
  override void		Render_impl();  // #IGNORE
  override void		Render_post(); // #IGNORE
  override void		Reset_impl(); //  #IGNORE
//  taDataView_List	link_views; // linked views
private:
  void			Initialize();
  void			Destroy();
};



#endif



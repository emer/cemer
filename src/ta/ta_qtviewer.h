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

#ifndef TA_QTVIEWER_H
#define TA_QTVIEWER_H

#include "qtdefs.h"
#include "ta_stdef.h"
#include "ta_base.h"
#include "ta_qtdata.h"
#include "ta_qtclipdata.h"


#ifndef __MAKETA__
  #include <qaction.h>
  #include <qframe.h>
  #include <Q3ListView>
  #include <qmainwindow.h>
  #include <qobject.h>
  #include <qpalette.h>
  #include <qpixmap.h>
  #include <QList>
  #include <qsplitter.h>
  #include <qtabbar.h>
  #include <qtoolbar.h>
#endif


// externals
class taGroup_impl;

// forwards this file:
class taiDataLink;
class tabDataLink;
class tabListDataLink;
class tabGroupDataLink; //

//obsclass iPanelTab; // #IGNORE
class iTabBar; // #IGNORE
class iTabView; // #IGNORE
class iDataPanel; //#IGNORE
class iDataPanelSet; // #IGNORE
class iDataPanel_PtrList;
class iTabView_PtrList; // #IGNORE

class iToolBar;
class iToolBar_List;

class WinGeometry;
class ToolBar;
class ToolBar_List;
class DataViewer;
class ISelectable;
class ISelectable_PtrList;
class DynMethodDesc; // #IGNORE
class DynMethod_PtrList; // #IGNORE
class IDataViewHost;
class iDataViewer;

class iListViewItem; //
class taiListDataNode;
class iListDataPanel; //


//////////////////////////
//   taiDataLink	//
//////////////////////////

class DataLink_QObj: public QObject {
Q_OBJECT
public:
  taiDataLink*	dl;
  DataLink_QObj(taiDataLink* dl_) {dl = dl_;}
};

class taiDataLink: public taDataLink { // interface for viewing system
#ifndef __MAKETA__
  typedef taDataLink inherited;
#endif
public:
  static String		AnonymousItemName(const String& type_name, int index); // [index]:Typename

  virtual void		FillContextMenu(taiActions* menu); // only override to prepend to menu
  virtual void		FillContextMenu_EditItems(taiActions* menu, int allowed) {}
  virtual const QPixmap* GetIcon(int bmf, int& flags_supported) {return NULL;}
  virtual taiDataLink*	GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
  virtual taiMimeItem*	GetMimeItem() {return NULL;} // replace
  virtual bool		ShowMember(MemberDef* md) {return false;} // asks this type if we should show the md member

  taiDataLink(void* data_, taDataLink* &link_ref_);
  DL_FUNS(taiDataLink) //

public: // this section for all the delegated menu commands
  virtual void		fileNew() {}
  virtual void		fileOpen() {}
  virtual void		fileSave() {}
  virtual void		fileSaveAs() {}
  virtual void		fileClose() {}

protected:
  DataLink_QObj*	qobj; // #IGNORE delegate object, when we need to connect or signal

  virtual void		Assert_QObj(); // makes sure the qobj is created
  virtual void		FillContextMenu_impl(taiActions* menu) {} // this is usually the one to override

  virtual ~taiDataLink(); // we only ever implicitly destroy, when 0 clients

public: // DO NOT CALL
  virtual void		QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {}
    // get edit items allowed for this one item
  virtual int		EditAction_impl(taiMimeSource* ms, int ea) {return 0;}
  virtual void		ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int& allowed, int& forbidden) {}
  virtual int		ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int ea) {return 0;}
};


//////////////////////////
//   tabDataLink	//
//////////////////////////

class tabDataLink: public taiDataLink { // DataLink for taBase objects
#ifndef __MAKETA__
typedef taiDataLink inherited;
#endif
public:
  taBase*		data() {return (taBase*)m_data;}

  override const QPixmap* GetIcon(int bmf, int& flags_supported);
    // delegates to taBase::GetDataNodeBitmap
  override bool		HasChildItems();
  override TypeDef*	GetDataTypeDef();
  override taiMimeItem* GetMimeItem();
  override String	GetName();
  override String	GetDisplayName();
  override bool		ShowMember(MemberDef* md); // asks this type if we should show the md member

  DL_FUNS(tabDataLink); //

public:// this section for all the delegated menu commands
  override void		fileOpen();
  override void		fileSave();
  override void		fileSaveAs();
  override void		fileClose(); //

protected:
  tabDataLink(taBase* data_, taDataLink* &link_ref_); //TODO: implementation for non taOBase-derived types
  override void		QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden);
  override int		EditAction_impl(taiMimeSource* ms, int ea);
  override void		ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
     taiMimeSource* ms,  int& allowed, int& forbidden);
  override int		ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int ea);
  override void		FillContextMenu_impl(taiActions* menu);
};


//////////////////////////
//   tabODataLink	//
//////////////////////////

class tabODataLink: public tabDataLink { // DataLink for taOBase objects
#ifndef __MAKETA__
typedef tabDataLink inherited;
#endif
public:
  taOBase*		data() {return (taOBase*)m_data;}
  tabODataLink(taOBase* data_);
  DL_FUNS(tabODataLink); //
};


//////////////////////////
//   tabListDataLink	//
//////////////////////////

class tabListDataLink: public tabODataLink {
  // DataLink for taList objects -- note that it also manages the ListView nodes
#ifndef __MAKETA__
typedef tabODataLink inherited;
#endif
public:
  taList_impl*		data() {return (taList_impl*)m_data;}
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols(); // number of columns in a list view for this item type
  override String	GetColHeading(int col); // header text for the indicated column
  override String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1);
  override bool		HasChildItems() {return true;} // at very least, has the 'items' subnode

  tabListDataLink(taList_impl* data_);
  DL_FUNS(tabListDataLink) //

public: // this section for all the delegated menu commands
  override void		fileNew();
};


//////////////////////////
//   tabGroupDataLink	//
//////////////////////////

class tabGroupDataLink: public tabListDataLink {
  // DataLink for taGroup objects -- adds additional 'subgroups' node under the 'items' node, for any subgroups
#ifndef __MAKETA__
typedef tabListDataLink inherited;
#endif
public:
  taGroup_impl*		data() {return (taGroup_impl*)m_data;}
  override bool		ShowMember(MemberDef* md); // asks this type if we should show the md member

  tabGroupDataLink(taGroup_impl* data_);
  DL_FUNS(tabGroupDataLink)
};



//////////////////////////
//   iToolBar		//
//////////////////////////

class iToolBar: public QToolBar {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
friend class ToolBar;
friend class iDataViewer;
public:
  ToolBar*		toolBar() {return m_toolBar;}
  iToolBar(ToolBar* toolBar_, const QString& label, iDataViewer* par_win);
   // constructor just does bare-bones create; Constr() does the actual work
  ~iToolBar();
protected:
  ToolBar*		m_toolBar;
  void 			showEvent(QShowEvent* e); // override
  void 			hideEvent(QHideEvent* e); // override
  virtual void		Showing(bool showing); // #IGNORE called by the show/hide handlers
private:
  typedef QToolBar	inherited;
};


//////////////////////////
//   iToolBar_List	//
//////////////////////////

#ifdef __MAKETA__
typedef iToolBar* iToolBar_ptr;
class iToolBar_List: public QList<iToolBar_ptr> {
#else
class iToolBar_List: public QList<iToolBar*> {
#endif
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS each BrowseWin maintains its existent toolbars in this list
friend class ToolBar;
public:
  ~iToolBar_List();
  iToolBar* 		FindToolBar(const char* name) const; // looks for toolbar by widget name, returns NULL if not found
protected:
  ToolBar*		m_toolBar;
};


//////////////////////////
//   ISelectable	//
//////////////////////////

/*
  this interface is for selectable items in the view, such as tree nodes in the browser, or
 Inventor objects in T3Viewer

 There are two major modes of clipboard handling: single selection, and multi-selection.
 In the single selection case, the ISelectable item itself controls everything; in this case,
   the sel_items list parameter can be null, or can be a list containing just the ISelectable
   item itself.
 In the multi-select case, the first selected item is the root (controlling) item. It must be
   passed a list of all the items (including itself, in position 0).

 NOTE: you must call viewer_win()->ObjectRemoving(item) in the implementation when an object is
 deleted or being removed from the viewing hierarchy.
*/

class ISelectable: public virtual IDataLinkClient { //
public: // Interface Properties and Methods
  virtual MemberDef*	md() const = 0; // memberdef in parent, if any, of the selected item
  virtual taiDataLink*	par_link() const = 0; // parent item's (if any) link
  virtual MemberDef* 	par_md() const = 0;// parent item's (if any) md
  virtual IDataViewHost* host() const = 0; //
//  IDataViewHost*	host() const {return viewer_win_();} // non-virtual, can be replaced/retyped
  virtual String	view_name() const = 0; // for members, the member name; for list items, the name if any, otherwise a created name using the index
  QWidget*		widget() const;

  virtual int		EditAction_(ISelectable_PtrList& sel_items, int ea);
   // do the indicated edit action (called from browser or list view); normally implement the _impl
  virtual void 		FillContextMenu(ISelectable_PtrList& sel_items, taiActions* menu);
   // normally implement the _impl
  virtual taiClipData*	GetClipData(const ISelectable_PtrList& sel_items, int src_edit_action,
    bool for_drag) const; // delegates to the link; normally not overridden
  virtual int		GetEditActions_(taiMimeSource* ms) const; // typically called on single item for acceptDrop
  int			GetEditActions_(const ISelectable_PtrList& sel_items) const;
    // called to get edit items available on clipboard for the sel_items
  virtual taiMimeItem*	GetMimeItem() const; // delegates to the link
  virtual int		RefUnref(bool ref) {return 1;} // ref'ed/unrefed in select lists etc.; optional, and can be used for lifetime mgt; returns count after operation


protected:
  virtual int		EditActionD_impl_(taiMimeSource* ms, int ea);
    // do Dst op for single selected item; generally doesn't need extending
  virtual int		EditActionS_impl_(int ea);
    // do Src op for single or one of multi selected items; CUT and COPY usually just a 1 return code; we actually implement the actual clipboard transfer
  virtual void		FillContextMenu_EditItems_impl(taiActions* menu, int allowed); // might be extended
  virtual void		FillContextMenu_impl(taiActions* menu) {} // link handles most, called in FCM
  virtual void		GetEditActionsD_impl_(taiMimeSource* ms, int& allowed, int& forbidden) const;
    // get Dst ops allowed for a single item,
  virtual void		GetEditActionsS_impl_(int& allowed, int& forbidden) const;
    // get Src ops allowed for a single item, possibly of many selected items
};


//////////////////////////
//   ISelectable_PtrList//
//////////////////////////

// Note: for inheritance hierarchies, only the first parent is searched, as that is
// always the primary class in cases where there is multiple inheritance

class ISelectable_PtrList: public taPtrList<ISelectable> { // for selection lists
#ifndef __MAKETA__
typedef taPtrList<ISelectable> inherited;
#endif
public:
  TypeDef*		Type1(); // data type of item
  TypeDef*		CommonSubtype1N(); // greatest common data subtype of items 1-N
  TypeDef*		CommonSubtype2N(); // greatest common data subtype of items 2-N
protected:
  override void*	El_Ref_(void* it) {((ISelectable*)it)->RefUnref(true); return it;}
  override void* 	El_unRef_(void* it) {((ISelectable*)it)->RefUnref(false);  return it;}

};


//////////////////////////
//   DynMethodDesc	//
//////////////////////////

class DynMethodDesc { // #IGNORE
friend class DynMethod_PtrList;
public:
  int			idx; // list index, used as param in the action callback routine
  int			dmd_type;
  MethodDef*		md;
protected:
  DynMethodDesc() {}
  ~DynMethodDesc() {}
};

//////////////////////////
//   DynMethod_PtrList	//
//////////////////////////

class DynMethod_PtrList: public taPtrList<DynMethodDesc> { // #IGNORE
#ifndef __MAKETA__
typedef taPtrList<DynMethodDesc> inherited;
#endif
public:
  enum DMDType {
    Type_1N,	// <methname>() method; applied to all selected object
    Type_1_2N,	// <methname>(Type2N* param) method; called on object 1 for objects 2:N
    Type_2N_1	// <methname>(Type1* param) method; called on objects 2:N for object 1
  };

  DynMethodDesc*	AddNew(int dmd_type, MethodDef* md); // creates new DMD and adds, returning ref
  void			Fill(ISelectable_PtrList& sel_items); // clear, then fill based on sel_items
  ~DynMethod_PtrList();
protected:
  override void 	El_Done_(void* it) {delete (DynMethodDesc*)it;}
  override void		El_SetIndex_(void* it, int idx) {((DynMethodDesc*)it)->idx = idx; };
  // sets the element's self-index
};


//////////////////////////
//   IDataViewHost	//
//////////////////////////

// Interface exposed by the GUI widget hosting the dataview

class IDataViewHost {
public:
  virtual QWidget*	This() = 0;
  virtual bool 		ObjectRemoving(ISelectable* item) = 0; // call from item when deleting or removing -- makes sure it is removed from sel lists, etc.

  virtual ~IDataViewHost() {}
};

//////////////////////////
//   iDataViewer	//
//////////////////////////

/* Clipboard Handling

  A subcontrol that wants to control clipboard handling must provide a signal slot interface,
  as follows; the slots/signals can have any names (* is optional):

  slots:
    void EditAction(int); // called when user requests the indicated edit action, ex. via menu or accel key
    void GetEditActions(int&); // called to get current valid edit actions
    *void SetActionsEnabled(TBD); // enables/disables any actions
  signals:
    void UpdateUi(); // control can call if something changed, to update the ui -- calls back its
       GetEditAction and SetActionsEnabled slot functions

  When a clipboard-enabled control (ex. the data browser tree) gets the focus, it should call:

    SetClipboardHandler(QWidget* handler_obj, ...) [see object below for api details]

  If a control knows that it is no longer active,  it should call.

    SetClipboardHandler(NULL)

  NOTE: however you can't call the above just because you lose focus, because this happens
    normally. ex. an edit control has focus, then user clicks on a toolbar button -- the
    edit control loses focus. Instead, the mechanism used is that basically unless something
    actively grabs the handler, it is left with the previous value. The taiData objects
    have a base mechanism built in so that when an implementation control gets focus, it
    unregisters clipboard handling, by default. Therefore, only taiData controls that implement
    it will actually get the focus, but simply clicking away on a toolbar button won't
    dehandle.

*/

/* Selection Handling

  Selectable items are maintained in the m_sel_items.
  The *current item* (curItem) is defined as the first (or only) selected item.
  The system does not make a distinction between "current" and "selected" (ex. as is made
    by Qt in its ListView).

  Items are managed as follows:

  curItem() - returns the first (or only) selected item
  setCurItem(i, forced) - clears selection list, and sets first item to i
  SelectionChanging(bool, forced) - batch changes can be placed inside true/false calls to this,
    so that only one SelectionChanged is issued
  AddSelectedItem(i, forced) - adds it to the selection list (an item is only added once)
  RemoveSelectedItem(i, forced) - removes the item, returning 'true' if item found and removed
  SelectionChanged(forced) - after all list changes, this is called; if the change came
    from the gui (ex., user picked something), then 'forced' will be false, otherwise
    true (indicating the object must reconfigure the gui to match the sel list)

*/
class iDataViewer: public QMainWindow, public IDataViewHost {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS gui portion of the DataViewer
  Q_OBJECT
#ifndef __MAKETA__
typedef QMainWindow inherited;
#endif
friend class taDataLink;
friend class DataViewer;
public:
  iToolBar_List	toolbars; // list of all created toolbars
  taiAction_List	actions; // our own list of all created actions
  taiAction_List	dyn_actions; // list of all dynamic actions currently available, based on selection
  DynMethod_PtrList	dyn_methods; // dynamic methods currently available, based on selection

  taiMenuBar*		menu;		// menu bar -- note: we use the window's built-in QMenu
  QWidget*		m_body;		// #IGNORE body of the window -- supplied by a descendant class

  taiMenu* 		fileMenu;
  taiMenu* 		fileExportMenu; // submenu -- empty and disabled in base
  taiMenu* 		editMenu;
  taiMenu* 		viewMenu;
  taiMenu* 		toolBarMenu;
  taiMenu* 		toolsMenu;
  taiMenu* 		actionsMenu; // statically added items first; bottom section is for dynamic
  taiMenu* 		helpMenu;
  taiAction* 		fileNewAction;
  taiAction* 		fileOpenAction;
  taiAction* 		fileSaveAction;
  taiAction* 		fileSaveAsAction;
  taiAction* 		fileCloseAction;
  taiAction* 		filePrintAction;
  taiAction* 		fileCloseWindowAction; //note: special, because it is always at bottom of File menu; root=Quit
  taiAction* 		editUndoAction;
  taiAction* 		editRedoAction;
  taiAction* 		editCutAction;
  taiAction* 		editCopyAction;
  taiAction* 		editPasteAction;
  taiAction* 		editDeleteAction;
  taiAction* 		editLinkAction;
  taiAction* 		viewRefreshAction;
  taiAction* 		helpHelpAction;
  taiAction* 		helpAboutAction;

//nn  iToolBar* 		applicationToolBar;

  ISelectable*		curItem() const {return m_sel_items.SafeEl(0);}
  virtual void		setCurItem(ISelectable* item, bool forceUpdate = false);
  ISelectable_PtrList&	sel_items() {return m_sel_items;}
  QObject* 		clipHandler() {return last_enabler;} // obj (if any) controlling clipboard handling

  //TODO: provide a list of multi-selects
  DataViewer*		viewer() {return m_viewer;} // usually replaced by strongly typed version

  virtual taiAction*	AddAction(taiAction* act); // add the action to the list, returning the instance (for convenience)
  virtual void		AddPanel(iDataPanel* panel) {} //  called by DataLink on creation
  virtual iToolBar*	AddToolBar(iToolBar* tb); // add the toolbar to the list, returning the instance (for convenience)
  virtual void		AddToolBarMenu(const String& name, int index);
  virtual void		Constr(); // #IGNORE constructs menu and body -- usually not overrriden (override _impl)
  iToolBar* 		Constr_ToolBar(ToolBar* tb, String name);
    // can be overriden to supply custom iToolBar
  virtual void		FillContextMenu(taiActions* menu); // s/b called by desc class, to put dynaction items onto menu
  virtual bool 		InitToolBar(const String& name, iToolBar* tb); // init the toolbar with specified name, returning true if handled
  void			emit_SetActionsEnabled();
  void			emit_GetEditActionsEnabled(int& ea); // param is one of the taiClipData EditAction values
  int			GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void 			setFrameGeometry(const iRect& r);
  void			setFrameGeometry(int left, int top, int width, int height); //bogus: see Qt docs on geometry under X
  void			SetClipboardHandler(QObject* handler_obj,
    const char* edit_enabled_slot = NULL,
    const char* edit_action_slot = NULL,
    const char* actions_enabled_slot = NULL,
    const char* update_ui_signal = NULL); // provides generic way for various subpanels to control the clipboard and object enabling; item typically calls this when it gets focus (multiple calls ok); can call with handler_obj NULL to disconnect

  // Viewer Selection Management functions (only concerns selections in viewer itself, not other handlers)
  virtual void 		SelectionChanging(bool begin, bool forced = false); // if used, must be called in pairs, with true then false
  virtual void 		AddSelectedItem(ISelectable* item,  bool forced = false);
  virtual bool 		RemoveSelectedItem(ISelectable* item,  bool forced = false); // 'true' if item was actually removed from (i.e. was in) list

  virtual void 		SelectionChanged(bool forced = false); // invoked when selection of current clipboard/focus handler changes
  void 			SetThisAsHandler(); // call when the viewer gets focus, to make it the handler

    // invoked when selection changes; replace _impl to implement
  virtual void		UpdateTabNames() {} // called by a datalink when a tab name might have changed

  iDataViewer(DataViewer* viewer_, QWidget* parent = NULL);
    // uses: WFlags flags = (WType_TopLevel | WStyle_SysMenu | WStyle_MinMax | WDestructiveClose)
//obs  iDataViewer(DataViewer* viewer_, QWidget* parent, WFlags flags); //note: only for completeness, usually use the one with implicit flags
  ~iDataViewer();

#ifndef __MAKETA__
signals:
  void 			EditAction(int ea); // param is one of the taiClipData editAction values
  void			GetEditActionsEnabled(int& ea); // sent to Clipboard server; param is one of the taiClipData EditAction values
  void			SetActionsEnabled(); // sent to Clipboard server -- enable/disable actions

  void			selectionChanged(ISelectable_PtrList& sels); // emitted when selection changes
#endif

public slots:
  virtual void 		fileNew(){}
  virtual void 		fileOpen(){}
  virtual void 		fileSave(){}
  virtual void 		fileSaveAs(){}
  virtual void 		fileClose(){}
  virtual void 		filePrint(){}
  virtual void 		fileCloseWindow(); // Quit (root) or Close Window (non-root)
/*  virtual void editUndo();
  virtual void editRedo();
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editFind(); */
  virtual void		viewRefresh() {} // rebuild/refresh the current view
/*  virtual void helpIndex();
  virtual void helpContents();*/
  virtual void 		helpAbout() {}

  virtual void		mnuEditAction(taiMenuEl* mel);
  virtual void		mnuDynAction(int idx); // a dynamic action

  void			actionsMenu_aboutToShow(); // populates dynamic items
  virtual void		UpdateUi(); // called after major events, to refresh menus, toolbars, etc.

public: // IDataViewHost interface
  override QWidget*	This() {return this;}
  override bool 	ObjectRemoving(ISelectable* item);

protected slots:
  void			ch_destroyed(); // cliphandler destroyed (just in case it doesn't deregister)
  virtual void 		this_GetEditActionsEnabled(int& ea); // for when viewer itself is clipboard handler
  virtual void 		this_EditAction(int param); // for when viewer itself is clipboard handler
  virtual void 		this_SetActionsEnabled(); // for when viewer itself is clipboard handler

  virtual void 		this_ToolBarSelect(int param); // user has selected or unselected one of the toolbars



protected:
  DataViewer*		m_viewer;
  bool			is_root; // true if this is a root window (has Quit menu)
  int			m_sel_chg_cnt; // counter to track batch selection changes
  ISelectable_PtrList	m_sel_items;
  int			m_last_action_idx; // index of last static action in actionMenu
  override void 	closeEvent(QCloseEvent* ev);
  override void 	customEvent (QCustomEvent* ev); // dispatch
  virtual void 		emit_EditAction(int param); // #IGNORE param is one of the taiClipData editAction values; desc can trap this and implement virtually, if desired
  virtual void 		selectionChangedEvent(QCustomEvent* ev);
  override void 	windowActivationChange(bool oldActive); // we manage active_wins in order of activation

  virtual void		Closing(bool forced, bool& cancel) {} // called to notify window in closeEvent, when certain to close
  virtual void		Constr_Menu_impl(); // #IGNORE constructs the menu and actions; MUST construct all static actions
  virtual void		Constr_Body_impl() {} // #IGNORE replace to construct body (and set in centralWidget)
  virtual void 		SetActionsEnabled_impl(); // invoked when selection changes
private:
  QObject* last_enabler; //we need to remember this, because you can't anonymously disconnect signals from your own slots
  QPixmap image0;
  QPixmap image1;
  QPixmap image2;
  QPixmap image3;
  QPixmap image4;
  QPixmap image5;
  QPixmap image6;
  QPixmap image7;
  QPixmap image8;
  void			init();
};


//////////////////////////
//   ToolBar		//
//////////////////////////

/*
toolbars are not automatically created, so 'mapped' indicates whether
user has opened that toolbar
*/
class ToolBar: public taNBase {// ##NO_INSTANCE ##NO_TOKENS proxy for Toolbars
friend class iToolBar;
friend class iDataViewer;
friend class DataViewer;
public:
  int			index; // #SHOW #NO_SAVE #READ_ONLY
  float			lft;  	// #HIDDEN when undocked, fractional position on screen
  float			top;	// #HIDDEN when undocked, fractional position on screen
  Orientation		o; // whether hor or vert
  bool			mapped; // #HIDDEN whether toolbar window has been created

  iToolBar*		toolBar() {return m_window;} // #IGNORE
  DataViewer*		viewer();
  iDataViewer*		viewer_win();

  virtual void 	GetWinPos(); // copy state of toolbar to us
  virtual void	SetWinPos();
  virtual void  Show();		// called when user selects from menu
  virtual void	Hide();		// called when user unselects from menu

  virtual void 	CloseWindow();		// #IGNORE close the toolbar

//  void	UpdateAfterEdit();
//  void	InitLinks();
  void	CutLinks();
  void	Copy_(const ToolBar& cp);
  COPY_FUNS(ToolBar, taNBase)
  TA_BASEFUNS(ToolBar)

protected:
  DataViewer*		m_viewer;  // #IGNORE cached
  iToolBar*		m_window;
  virtual void		Constr_Window_impl(); // #IGNORE sets the m_window instance
  virtual void		OpenNewWindow_impl(); // #IGNORE
  virtual void 		WindowClosing(); // #IGNORE

private:
  typedef taNBase	inherited;
  void 	Initialize();
  void	Destroy();
};

//////////////////////////
//   ToolBar_List	//
//////////////////////////

class ToolBar_List: public taList<ToolBar> {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS each BrowseWin maintains its existent toolbars in this list
public:
  iToolBar* 		FindToolBar(const char* name) const; // looks for toolbar by widget name, returns NULL if not found
  TA_BASEFUNS(ToolBar_List);
protected:
  void	El_SetIndex_(void* it, int index) 	{((ToolBar*)it)->index = index; }
  // sets the element's self-index
private:
  typedef taList<ToolBar> inherited;
  void			Initialize() {}
  void			Destroy() {}
};



//////////////////////////
//   WinGeometry	//
//////////////////////////

class WinGeometry : public taBase {
  // ##NO_TOKENS #INLINE Window geometry (position, size) saved in 1.0f-relative coordinates
public:
  static float	Offs(float cur, float by); // offset cur by 'by' amount (0 > by >= 1.0); wraps if >1

  DataViewer* 	owner;   	// #READ_ONLY #NO_SAVE pointer to owner
  float	 	lft; 		// left (horizontal)
  float		top;  		// top (vertical) NOTE: was "bottom" in Iv version
  float		wd;		// width
  float		ht;		// height

  virtual void		GetWinPos();	// get the window position from parent winbase
  virtual void		SetWinPos();	// set the window position from parent winbase
  virtual void 		ScriptWinPos(ostream& strm = cout);

  TAPtr			GetOwner(TypeDef* tp) const	{ return taBase::GetOwner(tp); }
  TAPtr 		GetOwner() const 		{ return (TAPtr)owner; }
  TAPtr 		SetOwner(TAPtr ta)		{ owner = (DataViewer*)ta; return ta; }

  void			UpdateAfterEdit();
  void			CutLinks()		{owner = NULL;}
  void 			Copy_(const WinGeometry& cp);
  COPY_FUNS(WinGeometry, taBase)
  TA_BASEFUNS(WinGeometry)
private:
  typedef taBase	inherited;
  void 	Initialize();
  void 	Destroy() {CutLinks();}
};



//////////////////////////
//   	DataViewer	//
//////////////////////////

class DataViewer : public taDataView {
  // #NO_TOKENS #VIRT_BASE the base type for objects with a top-level window or panel and a menu
#ifndef __MAKETA__
  typedef taDataView inherited;
#endif
friend class taDataLink;
friend class iDataViewer;
friend class WinGeometry;
public:
  enum PrintFmt {
    POSTSCRIPT,
    JPEG,
    TIFF,
    PDF //NOTE: new for Qt version
  };

  String		name;		// name of the object
  taiMenu_List		ta_menus;	// menu representations (from methods, non-menubuttons only)
  taiMenuBar*		menu;		// menu bar -- note: partially managed by the window
  String		win_name;	// #HIDDEN #NO_SAVE name field for the window
  String		file_name;	// #HIDDEN file name used in loading/saving
//  DataViewer*		win_owner;	// #READ_ONLY #NO_SAVE owner that has a window
  WinGeometry		win_pos;  	// #HIDDEN position/size of the window on the screen

  taFiler*		ta_file;	// #NO_SAVE #HIDDEN file manager for this -- always use refcount semantics
  taFiler*		print_file;	// #NO_SAVE #HIDDEN print file for this
  bool			iconified;	// #HIDDEN whether window is iconified or not
  bool			display_toggle;  // #DEF_true 'true' if display should be updated
  ToolBar_List		toolbars;	// #HIDDEN

  iDataViewer*	window() {return m_window;} // #IGNORE valid if in a window

  bool 			SetName(const char* nm) {return SetName(String(nm));}
  bool 			SetName(const String& nm) {name = nm; return true;}
  String		GetName() const {return name; }
  virtual bool	HasChanges() {return m_has_changes;} // 'true' when something needs to be saved
  taiDataLink*  GetDataLink(void* el, TypeDef* el_typ, int param = 0);
  virtual void	Changed(bool value = true); // default sets changes; call with 'false' to clear changes
  virtual bool	Save(); // call to save the object to current file, or new file if new; 'true' if saved
  int		Save(ostream& strm, TAPtr par=NULL, int indent=0);
  // Save object to a file
  int		SaveAs(ostream& strm, TAPtr par=NULL, int indent=0)
  { return Save(strm,par,indent); }
  // Save object to file
  int  		Load(istream& strm, TAPtr par=NULL);
  // load object from a file
  int		Edit(bool wait=false);
  void		Close();
  // close this structure/window
  virtual void	Print(PrintFmt format, const char* fname = NULL);
  // #MENU #ARGC_1 #MENU_SEP_BEFORE #LABEL_Print_(Window) Print this object's entire window (including buttons, etc) to file (prompted next) in given format
  virtual void	Print_Data(PrintFmt format, const char* fname = NULL);
  // #MENU #ARGC_1 #LABEL_Print_Data_(Only) Print only specific data associated with this window (not control buttons, etc) to file (prompted next) in given format
//nuke  virtual void  UpdateMenus(); // #MENU #MENU_SEP_BEFORE update all menus under me (inclusive)
  virtual void 	ScriptWinPos() 		{ win_pos.ScriptWinPos(cout); }
  // #NO_SCRIPT generate script code to position the window
  virtual void  Iconify();		// #MENU iconify the window (saves iconified state)
  virtual void	DeIconify();		// deiconify the window (saves deiconified state)
  virtual bool	IsMapped();		// return the 'mapped' status of the window
//  virtual void	ViewWindow(float left = -1.0f, float top = -1.0f, float width = -1.0f, float height = -1.0);
  virtual void	ViewWindow();
  // either de-iconfiy or create a new view to view this object

  virtual void 	OpenNewWindow();	// #IGNORE open a new window for this class
  virtual void 	CloseWindow();		// #IGNORE close the window or panel
  virtual void	Clear(); // checks to make sure we are mapped first
  virtual void	Render(); // checks to make sure we are mapped first
  virtual void	Reset(); // checks to make sure we are mapped first
  virtual void	SetFileName(const char* fname);	// #IGNORE set new file name for object
  virtual void	SetWinName();		// #IGNORE set the window name to object path/name

//nuke  virtual void  UpdateMenus_impl(); 	// #IGNORE actually does the work
  virtual bool	ThisMenuFilter(MethodDef* md); // #IGNORE filter this menu items
//nuke  virtual void	GetThisMenus();		// #IGNORE get the 'this' menus
//nuke  virtual void	GetThisMenus_impl(taiMenu* par_menu, taiMenu_List& ths_men, taiDataList& ths_meth, String prfx);
  // #IGNORE actually gets the menus
//nuke  virtual void 	GetMenu();		// #IGNORE get the menu items from MenuGroups

  virtual void	GetFileProps(TypeDef* td, String& fltr, bool& cmprs);
  // #IGNORE get file properties for given type
//obs  taFiler*	GetFileDlg();		// #IGNORE for this and its menugroups
  virtual void	GetPrintFileDlg(PrintFmt fmt);	// #IGNORE for the printfile
  virtual String GetPrintFileExt(PrintFmt fmt); // get string of file extension for given fmt

  virtual void	Raise();	// raise window to front
  virtual void	Lower();	// lower window to back

  virtual void  Print_impl(PrintFmt format, QWidget*, const char* fname=NULL); // #IGNORE
  virtual QWidget* GetPrintData(); // #IGNORE overload this

  int	Dump_Load_Value(istream& strm, TAPtr par=NULL);
  // process any pending iv events after loading..

  void	UpdateAfterEdit();
  void	InitLinks();
  virtual void	WinInit(); // called inside InitLinks, after our core InitLinks, but before window opened
  void	CutLinks();
  void	Copy_(const DataViewer& cp);
  COPY_FUNS(DataViewer, taDataView)
  TA_DATAVIEWFUNS(DataViewer)

protected:
  bool			m_is_root; // #IGNORE
  bool 			m_has_changes;
  iDataViewer*		m_window;	// #IGNORE each project gets a window
  taiActions*		cur_menu; // for building menu
  TypeDef*		link_type; // base type for GetDataLink calls
  virtual void		Constr_Window_impl() {} // #IGNORE implement this to set the m_window instance
//  virtual void		Constr_Menu_impl(); // #IGNORE constructs the view menu
  virtual void		Constr_Toolbars_impl(); // #IGNORE constructs the toolbars
  taDataLink*		GetDataLink_(void* el, TypeDef* typ, int param = 0); // gets the data link, gets existing or makes new; param can be used where el's have no typedefs (ex. class browsing)
  virtual void		OpenNewWindow_impl(); // #IGNORE
  virtual void 		WindowClosing(bool& cancel);
private:
  void 	Initialize();
  void	Destroy();
};


//////////////////////////////////
//	DataViewer_List		//
//////////////////////////////////

class DataViewer_List: public taList<DataViewer> { // #NO_TOKENS
public:
  TA_BASEFUNS(DataViewer_List)
private:
  void 	Initialize() { SetBaseType(&TA_DataViewer);}
  void	Destroy() {}
};


//////////////////////////
//   iTabDataViewer 	//
//////////////////////////

//Note: used by Browser and 3D Viewers
class iTabDataViewer : public iDataViewer { // viewer window used for class browsing
    Q_OBJECT
INHERITED(iDataViewer)
friend class iTabView;
public:
  virtual taiDataLink*	sel_link() const {return (curItem()) ? curItem()->link() : NULL;} // datalink of selected item that is controlling the current data panel view, ex. datalink of the selected tree node in a browser; return NULL if unknown, mult-select is in force, etc. -- controls things like clip handling
  virtual MemberDef*	sel_md() const {return (curItem()) ? curItem()->md() : NULL;}; // as for sel_link
  iTabView*		tabView() {return m_curTabView;} // currently active
  iTabView_PtrList*	tabViews() {return m_tabViews;} // currently active

  taiAction* 		viewSplitVerticalAction;
  taiAction* 		viewSplitHorizontalAction;
  taiAction* 		viewCloseCurrentViewAction;

  override void		AddPanel(iDataPanel* panel); // adds a new pane, and sets active in current tab
  void			AddPanelNewTab(iDataPanel* panel); // adds a new tab, sets panel active in it
  virtual iTabView*	AddTabView(QWidget* parCtrl, iTabView* splitBuddy = NULL); // adds a new tab view, optionally as a split
  void			SetPanel(iDataPanel* panel); // sets the panel active in current tab
  virtual void		TabView_Destroying(iTabView* tv); // called when a tabview deletes
  virtual void		TabView_Selected(iTabView* tv); // called when a tabview gets focus
  override void		UpdateTabNames(); // called by a datalink when a tab name might have changed
  iTabDataViewer(DataViewer* viewer_, QWidget* parent = NULL); //
//obs  iTabDataViewer(DataViewer* viewer_, QWidget* parent, WFlags flags);// not normally used
  ~iTabDataViewer();

public slots:
  virtual void		AddTab(); // causes tab bar to open a new tab, on current panel
  virtual void		CloseTab(); // causes current tab to close (unless only 1 tab)
  void			Closing(bool forced, bool& cancel); // override
  virtual void 		viewCloseCurrentView();
  virtual void 		viewSplitVertical();
  virtual void 		viewSplitHorizontal();

protected:
  iTabView_PtrList*	m_tabViews; // all created tab views
  iTabView*		m_curTabView; // tab view (split) that currently has the focus
  void			Constr_Menu_impl(); // override
  virtual iDataPanel* 	MakeNewDataPanel_(taiDataLink* link); // can be overridden, esp for Class browser and other non-tabase
  override void 	selectionChangedEvent(QCustomEvent* ev);
  void 			viewSplit(int o);

private:
  void			init();
};

//////////////////////////
// 	iTabBar 	//
//////////////////////////

class iTabBar: public QTabBar { // #IGNORE encapsulates the TabBar for iTabView
  Q_OBJECT
public:
  iDataPanel*		panel(int idx); // gets the current panel, if any

#ifndef __MAKETA__
  using			QTabBar::addTab; // bring also into scope
#endif
  int			addTab(iDataPanel* panel);
  void			SetPanel(int idx, iDataPanel* value, bool force = false); // set or remove (NULL) a panel
  
  iTabView*	tabView() {return (iTabView*)parent();}
  iTabBar(iTabView* parent_ = NULL);
  ~iTabBar();
protected:
//  QColor	defBackgroundColor; // for when tabpanel doesn't provide one
//  QColor	defBaseColor; // for when tabpanel doesn't provide one
  QPalette	defPalette;
  override void contextMenuEvent(QContextMenuEvent * e);
  override void mousePressEvent(QMouseEvent* e);
//  override void paint(QPainter* p, QTab* t, bool selected ) const;
};


//////////////////////////
//   iDataPanel_PtrList	//
//////////////////////////

class iDataPanel_PtrList: public taPtrList<iDataPanel> { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  iDataPanel_PtrList() {}
  ~iDataPanel_PtrList() {}
};


//////////////////////////
//   iTabView		//
//////////////////////////

//NOTE: maybe should inherit from QDockWindow -- then it would be dockable...
class iTabView: public QWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS  control for managing tabbed data pages
  Q_OBJECT
friend class iTabBar;
friend class iTabView_PtrList;
friend class iDataPanel;
public:
  QVBoxLayout*		layDetail;
    iTabBar*		tbPanels; //note: we always maintain at least one tab
    Q3WidgetStack*	wsPanels; //

  iDataPanel*		cur_panel(); // currently active panel
  iDataPanel*		panel(int idx = 0); // implementation-independent way to access panels
  int			panel_count();
  taiDataLink*		par_link() const {return (m_viewer_win) ? m_viewer_win->sel_link() : NULL;}
  MemberDef*		par_md() const {return (m_viewer_win) ? m_viewer_win->sel_md() : NULL;}
  iTabDataViewer* 	viewer_win() {return m_viewer_win;}

  void			Activated(bool val); // called by parent to indicate if we are active tabview or not
  bool 			ActivatePanel(taiDataLink* dl); // if a panel exists for the link, make it active and return true
  void			AddPanel(iDataPanel* panel); // adds a new pane, and sets active in current tab
  void			AddPanelNewTab(iDataPanel* panel); // adds a new tab, sets panel active in it
  void 			Closing(bool forced, bool& cancel);
  void 			DataPanelDestroying(iDataPanel* panel);
  virtual void		FillTabBarContextMenu(QMenu* contextMenu);
  virtual iDataPanel*	GetDataPanel(taiDataLink* link); // get panel for indicated link, or make new one; par_link is not necessarily data item owner (ex. link lists, references, etc.)
  void 			RemoveDataPanel(iDataPanel* panel);
  void 			SetPanel(iDataPanel* panel);
  void 			UpdateTabNames(); // called by a datalink when a tab name might have changed

  iTabView(QWidget* parent = NULL);
  iTabView(iTabDataViewer* data_viewer_, QWidget* parent = NULL);
  ~iTabView();

public slots:
  void 			AddTab();
  void 			CloseTab();
  virtual void		panelSelected(int idx);

protected:
  iTabDataViewer* 	m_viewer_win;

private:
  iDataPanel_PtrList	panels; // no external hanky-panky with this puppie
  void			init();
};

//////////////////////////
//   iTabView_PtrList	//
//////////////////////////

class iTabView_PtrList: public taPtrList<iTabView> {
public:
  void			DataPanelDestroying(iDataPanel* panel); // dispatch to all
  iTabView_PtrList() {}
  ~iTabView_PtrList();
};


//////////////////////////
//   iDataPanel 	//
//////////////////////////

class iDataPanel: public QFrame, public IDataLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS interface for panels (note: don't inherit directly)
  Q_OBJECT
friend class taDataLink;
friend class iPanelTab;
friend class iTabView;
friend class iDataPanelSet;
public:
  virtual bool		dirty() {return HasChanged();}
    // true if panel should not be replaced, but a new panel should be opened for the new item
  taiDataLink*		link() {return (taiDataLink*)m_link;}
  virtual bool		lockInPlace() {return false;}
    // true if panel should not be replaced when a user clicks around
  virtual String	panel_type() const {return _nilString;}
   //  this string is on the subpanel button for a panel (n/a to panelsets)
  virtual taiDataLink*	par_link() const = 0; // *current* visual parent link of this data panel; this could change dynamically, if a datapanel is shared across all referring instances, ex. link lists, references, etc. -- return NULL if unknown, not set, or not applicable -- controls things like clip enabling etc.
  virtual MemberDef*	par_md() const = 0; // as for par_link
//  DataViewer*		viewer() {return (m_dps) ? m_dps->viewer() : m_tabView->viewer();}
  iTabView*		tabView() {return m_tabView;} // tab view in which we are shown
  virtual iTabDataViewer* viewer_win() const = 0;


  virtual void		Closing(bool forced, bool& cancel) {} // called to notify panel is(forced==true)/wants(forced=false) to close -- set cancel 'true' (if not forced) to prevent
  virtual void		ClosePanel() = 0; // anyone can call this to get the panel to close (ex. edit panel contents are deleted externally)
  void 			ctrl_focusInEvent(QFocusEvent* ev); // can be called by a subcontrol when it gets focus; sets us as the clipboard handler
  virtual int		EditAction(int ea) {return 0;} //
  virtual int		GetEditActions() {return 0;} // after a change in selection, update the available edit actions (cut, copy, etc.)
  virtual void		GetImage() = 0; // called when reshowing a panel, to insure latest data
  virtual const iColor* GetTabColor(bool selected) const {return NULL;} // special color for tab; NULL means use default
  virtual bool		HasChanged() {return false;} // 'true' if user has unsaved changes -- used to prevent browsing away
  virtual String 	TabText() const; // text for the panel tab -- usually just the view_name of the curItem

  iDataPanel(taiDataLink* dl_); //note: created with no parent -- later added to stack
  ~iDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2)
    {DataChanged_impl(dcr, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  override void		DataLinkDestroying(taDataLink* dl) {} // called by DataLink when it is destroying --
  override TypeDef*	GetTypeDef() const {return &TA_iDataPanel;}

protected slots:
  virtual void 		this_GetEditActionsEnabled(int& ea); // for when panel is clipboard handler
  virtual void 		this_EditAction(int param); // for when panel is clipboard handler

protected:
  iTabView*		m_tabView; // tab view in which we are shown
  virtual void		DataChanged_impl(int dcr, void* op1, void* op2); // tab name may have changed
};


//////////////////////////
//   iDataPanelFrame 	//
//////////////////////////

class iDataPanelFrame: public iDataPanel {
  // interface for panel frames
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanel inherited;
#endif
friend class iDataPanelSet;
public:
  override taiDataLink*	par_link() const; // taken from dps if any, else from tabview
  override MemberDef*	par_md() const;
  override iTabDataViewer* viewer_win() const;


  override void		ClosePanel();
  override void		GetImage(); // called when reshowing a panel, to insure latest data
  override String 	TabText() const; // text for the panel tab -- usually just the text of the sel_node

  iDataPanelFrame(taiDataLink* dl_);
  ~iDataPanelFrame();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override void		DataLinkDestroying(taDataLink* dl); // called by DataLink when it is destroying --
  override TypeDef*	GetTypeDef() const {return &TA_iDataPanelFrame;}

protected:
  iDataPanelSet*	m_dps; // set if we are in a datapanelset
  virtual void		GetImage_impl() {} // #IGNORE called when reshowing a panel, to insure latest data (except not called if HasChanged true)
};

//////////////////////////
//   iViewPanelFrame 	//
//////////////////////////

class iViewPanelFrame: public iDataPanel {
  // frame for gui interface to a view element -- usually posted by the view, and locked
#ifndef __MAKETA__
typedef iDataPanel inherited;
#endif
  Q_OBJECT
public:
  taDataView*		dv() {return m_dv;} // can be statically replaced with subclass
  override bool		lockInPlace() {return true;}
    // true if panel should not be replaced, ex. if dirty, or viewpanel
  override taiDataLink*	par_link() const {return NULL;} // n/a
  override MemberDef*	par_md() const {return NULL;}
  override iTabDataViewer* viewer_win() const;


  virtual void		InitPanel(); // called on structural changes
  override void		ClosePanel();
  override void		GetImage(); // called when reshowing a panel, to insure latest data, or just to refresh
  override String 	TabText() const; // text for the panel tab -- usually just the text of the sel_node

  iViewPanelFrame(taDataView* dv_);
    // NOTE: dv will be nulled out if it destroys
  ~iViewPanelFrame();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;} //
  override void		DataLinkDestroying(taDataLink* dl); //note: dl is on the view, not underlying data
  override TypeDef*	GetTypeDef() const {return &TA_iViewPanelFrame;}

protected:
  taDataView*		m_dv;
  int			updating; // #IGNORE >0 used to suppress update-related widget signals
  virtual void		GetImage_impl() {} // called within +-updating pair, only called if dv exists
  virtual void		InitPanel_impl() {} // called within +-updating pair on structural changes
};

//////////////////////////
//   iDataPanelSet 	//
//////////////////////////

//NOTE: this class is only designed to handle a once-only adding of its subpanels; it cannot
// handle dynamically adding and removing subpanels

class iDataPanelSet: public iDataPanel { //  contains 0 or more sub-data-panels, and a small control bar for selecting panels
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanel inherited;
#endif
public:
  int			cur_panel_id; // -1 if none
  iDataPanel_PtrList	panels;
  QVBoxLayout*		layDetail;
    QFrame*		frmButtons;
      QHBoxLayout*	layButtons;
      Q3ButtonGroup*	buttons; // one QPushButton for each
    Q3WidgetStack*	wsSubPanels; // subpanels

  override taiDataLink*	par_link() const {return (m_tabView) ? m_tabView->par_link() : NULL;}
  override MemberDef*	par_md() const {return (m_tabView) ? m_tabView->par_md() : NULL;}
  override iTabDataViewer* viewer_win() const {return (m_tabView) ? m_tabView->viewer_win() : NULL;}

  iDataPanel*		cur_panel() const {return panels.SafeEl(cur_panel_id);} // NULL if none
  void			set_cur_panel_id(int cpi);

  void			AddSubPanel(iDataPanelFrame* pn);
  void			AllSubPanelsAdded(); // call after all subpanels added, to finalize layout

  override void		Closing(bool forced, bool& cancel);
  override void		ClosePanel();
  override void		GetImage();
  override const iColor* GetTabColor(bool selected) const;
  override bool		HasChanged();


  iDataPanelSet(taiDataLink* dl_);
  ~iDataPanelSet();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override void		DataLinkDestroying(taDataLink* dl) {} // nothing for us; subpanels handle
  override TypeDef*	GetTypeDef() const {return &TA_iDataPanelSet;}

public slots:
  void			btn_pressed(int id);

protected:
  void			removeChild(QObject* obj);
};

class iListViewItem: public Q3ListViewItem, public ISelectable {
  //  ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for Tree and List nodes
#ifndef __MAKETA__
typedef Q3ListViewItem inherited;
#endif
public:
  enum DataNodeFlags {
    DNF_IS_FOLDER 	= 0x001, // true for list/group folder nodes (note: does *not* indicate whether item can contain other things or not)
    DNF_IS_ALIAS 	= 0x002, // true after children have been created (after clicking on node)
    DNF_UPDATE_NAME	= 0x004, // typically for list items, update the visual name (tree, tab, etc.) after item edited
    DNF_CAN_BROWSE	= 0x008, // can be a new browser root
    DNF_CAN_DRAG	= 0x010, // 16 can allow drags
    DNF_NO_CAN_DROP	= 0x020, // 32 cannot accept drops
    DNF_IS_MEMBER 	= 0x040, // 64 true for members (and root), not for list/group items -- helps node configure edit ops
    DNF_IS_LIST_NODE 	= 0x080 // true for nodes in a list view (in panel, not on tree)
  };

  enum BrowseDropAction {
    BDA_MOVE,
    BDA_COPY,
    BDA_LINK,
    BDA_MOVE_AS_SUBGROUP, // moves item as a subgroup, ex a Unit group
    BDA_MOVE_AS_SUBITEM // moves item as a subitem, ex a sub spec, or subprocess
  };

  int			flags; // any of DataNodeFlags

  void* 		data() {return m_link->data();} //

//  DataViewer*		viewer();
//  iTabDataViewer*	viewer_win() const {return (iTabDataViewer*)viewer_win_();}

  iListViewItem(taiDataLink* link_, MemberDef* md_, iListViewItem* parent_,
    iListViewItem* last_child_, const String& tree_name, int flags_ = 0);
  iListViewItem(taiDataLink* link_, MemberDef* md_, Q3ListView* parent_,
    iListViewItem* last_child_, const String& tree_name, int flags_ = 0);
  ~iListViewItem();

  override bool 	acceptDrop (const QMimeSource* mime) const;
//  int			compare (Q3ListViewItem* item, int col, bool ascending) const; // override
  virtual void		DecorateDataNode(); // sets icon and other visual attributes, based on state of node

public: // ITypedObject interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iListViewItem;}

public: // IDataLinkClient interface
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2)
    {DataChanged_impl(dcr, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  override void		DataLinkDestroying(taDataLink* dl); // called by DataLink when it is destroying --

public: // ISelectable interface
  override MemberDef*	md() const {return m_md;}
  override String	view_name() const; // for members, the member name; for list items, the name if
//  override taiClipData*	GetClipData(int src_edit_action, bool for_drag);
//  override int		GetEditActions(taiMimeSource* ms) const; // simpler version uses Query
//  override taiMimeItem*	GetMimeItem();
protected:
//  override int		EditAction_impl(taiMimeSource* ms, int ea);
//  override void		FillContextMenu_EditItems_impl(taiMenu* menu, int allowed);
//  override void		FillContextMenu_impl(taiMenu* menu); // this is the one to extend in inherited classes
  override void		GetEditActionsS_impl_(int& allowed, int& forbidden) const;  // OR's in allowed; OR's in forbidden

protected:
  MemberDef*		m_md; // for members, the MemberDef (otherwise NULL)
//nn  void 			dragEntered(); // override
//nn  void 			dragLeft(); // override
  void			dropped(QDropEvent* e); // override
  virtual void		DataChanged_impl(int dcr, void* op1, void* op2) {} // called for each node when the data item has changed, esp. ex lists and groups
  void			init(taiDataLink* link_, MemberDef* md_, int flags_); // #IGNORE
};


//////////////////////////
//   taiListDataNode 	//
//////////////////////////

class taiListDataNode: public iListViewItem {
#ifndef __MAKETA__
typedef iListViewItem inherited;
#endif
public:
  int			num; // item number, starting from 1
  iListDataPanel*	panel; // logical parent node of the list items

  int 			compare (Q3ListViewItem *i, int col, bool ascending ) const; // override

  QString		text(int col) const; // override

  taiListDataNode(int num_, iListDataPanel* panel_, taiDataLink* link_,
    Q3ListView* parent_, taiListDataNode* last_child_, int flags_ = 0);
    //note: list flag automatically or'ed in
  ~taiListDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taiListDataNode;}

public: // ISelectable interface
  override taiDataLink* par_link() const; // we get from the panel, which gets from the viewer window
  override MemberDef* 	par_md() const; // as for par_link
  override IDataViewHost* host() const;
};


//////////////////////////
//   iListDataPanel 	//
//////////////////////////

class iListDataPanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  QVBoxLayout*		layOuter;
  Q3ListView*		list; //actually an iLDPListView

  override String	panel_type() const; // this string is on the subpanel button for this panel

  void			ClearList(); // for when data changes -- we just rebuild the list
  override int 		EditAction(int ea);
  void			FillList();
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected datanodes

  iListDataPanel(taiDataLink* dl_);
  ~iListDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iListDataPanel;}
protected:
  iListViewItem* 	mparentItem;
  void 			ConfigHeader();
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

protected slots:
  void			list_contextMenuRequested(Q3ListViewItem* item, const QPoint & pos, int col);
  void			list_selectionChanged(); //note: must use this parameterless version in Multi mode
};


#endif // TA_QTVIEWER_H

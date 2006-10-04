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

// ta_qtviewer.h -- gui classes for main window viewing

#ifndef TA_QTVIEWER_H
#define TA_QTVIEWER_H

#include "ta_viewer.h"
#include "ta_qtdata.h"


#ifndef __MAKETA__
# include <QAction>
# include <QDockWidget>
# include <QFrame>
# include <QMainWindow>
# include <QObject>
# include <qpalette.h>
# include <QIcon>
# include <QList>
# include <qtabbar.h>
# include <QTreeWidget>
# include <QTreeWidgetItem>
# include <qtoolbar.h>

# include "icliptoolwidget.h"
# include "itreewidget.h"
#endif


// externals
class taGroup_impl;

// (new) forwards this file:
class DataLink_QObj;
class taiDataLink;
class tabDataLink;
class tabListDataLink;
class tabGroupDataLink; //


class iTreeView;
class iTreeViewItem;
class taiTreeDataNode;
class taiListDataNode;
class iListDataPanel; //


class TA_API DataLink_QObj: public QObject {
Q_OBJECT
public:
  taiDataLink*	dl;
  DataLink_QObj(taiDataLink* dl_) {dl = dl_;} //
};

/*
  TODO: the GetDataMemberDef is not too reliable, and only works for owned taOBase, but that is
    typically sufficient for the browser, because typically only taOBase or later members are
    ever shown in the browser, which is really all that call helps
*/

class TA_API taiDataLink: public taDataLink { // interface for viewing system
INHERITED(taDataLink)
public:
  static String		AnonymousItemName(const String& type_name, int index); // [index]:Typename

  virtual void		FillContextMenu(taiActions* menu); // only override to prepend to menu
  virtual void		FillContextMenu_EditItems(taiActions* menu, int allowed) {}
  virtual bool		GetIcon(int bmf, int& flags_supported, QIcon& ic) {return false;}
  virtual taiDataLink*	GetListChild(int itm_idx) {return NULL;} // returns NULL when no more
  virtual taiMimeItem*	GetMimeItem() {return NULL;} // replace
  virtual bool		ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const
    {return false;} // asks this type if we should show the md member

  taiTreeDataNode* 	CreateTreeDataNode(MemberDef* md, taiTreeDataNode* parent,
    taiTreeDataNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a tree node as a parent
  taiTreeDataNode* 	CreateTreeDataNode(MemberDef* md, iTreeView* parent,
    taiTreeDataNode* after, const String& node_name, int dn_flags = 0);
    // create the proper tree node, with a iTreeView as a parent

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
  taiTreeDataNode* 	CreateTreeDataNode(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags); // combined version, only 1 xxPar is set
  virtual taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md,
    taiTreeDataNode* nodePar, iTreeView* tvPar, taiTreeDataNode* after,
    const String& node_name, int dn_flags) = 0; // NOTE: only 1 of the parents is non-null -- use that version of the taiTreeNode constructor

  virtual ~taiDataLink(); // we only ever implicitly destroy, when 0 clients

public: // DO NOT CALL
  virtual void		QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) {}
    // get edit items allowed for this one item
  virtual int		EditAction_impl(taiMimeSource* ms, int ea) {return 0;}
  virtual void		ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int& allowed, int& forbidden) {}
  virtual int		ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int ea) {return 0;} //
};


//////////////////////////
//   tabDataLink	//
//////////////////////////

class TA_API tabDataLink: public taiDataLink { // DataLink for taBase objects
INHERITED(taiDataLink)
public:
  taBase*		data() {return (taBase*)m_data;}
  taBase*		data() const {return (taBase*)m_data;}
  override bool		isBase() const {return true;} 
  
  override bool		GetIcon(int bmf, int& flags_supported, QIcon& ic);
    // delegates to taBase::GetDataNodeBitmap
  override bool		HasChildItems();
  override TypeDef*	GetDataTypeDef() const;
  override taiMimeItem* GetMimeItem();
  override String	GetName() const;
  override bool		ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const; // asks this type if we should show the md member
  override String	GetColText(const KeyString& key, int itm_idx = -1) const; // #IGNORE

  DL_FUNS(tabDataLink); //

public:// this section for all the delegated menu commands
  override void		fileOpen();
  override void		fileSave();
  override void		fileSaveAs();
  override void		fileClose(); //

protected:
  tabDataLink(taBase* data_, taDataLink* &link_ref_); //TODO: implementation for non taOBase-derived types
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
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

class TA_API tabODataLink: public tabDataLink { // DataLink for taOBase objects
INHERITED(tabDataLink)
public:
  taOBase*		data() {return (taOBase*)m_data;}
  taOBase*		data() const {return (taOBase*)m_data;}
  
  override MemberDef*	GetDataMemberDef() const;

  tabODataLink(taOBase* data_);
  DL_FUNS(tabODataLink); //
};


//////////////////////////
//   tabListDataLink	//
//////////////////////////

class TA_API tabListDataLink: public tabODataLink {
  // DataLink for taList objects -- note that it also manages the ListView nodes
INHERITED(tabODataLink)
public:
  taList_impl*		data() {return (taList_impl*)m_data;}
  taList_impl*		data() const {return (taList_impl*)m_data;}
  
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols() const; // number of columns in a list view for this item type
  override const KeyString GetListColKey(int col) const; // #IGNORE
  override String	GetColHeading(const KeyString& key) const; // #IGNORE
  override String	ChildGetColText(taDataLink* child, const KeyString& key, 
    int itm_idx = -1) const;	// #IGNORE
  override bool		HasChildItems() {return true;} // at very least, has the 'items' subnode

  tabListDataLink(taList_impl* data_);
  DL_FUNS(tabListDataLink) //

public: // this section for all the delegated menu commands
  override void		fileNew();
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};


//////////////////////////
//   tabGroupDataLink	//
//////////////////////////

class TA_API tabGroupDataLink: public tabListDataLink {
  // DataLink for taGroup objects -- adds additional 'subgroups' node under the 'items' node, for any subgroups
INHERITED(tabListDataLink)
public:
  taGroup_impl*		data() {return (taGroup_impl*)m_data;}
  const taGroup_impl*	data() const {return (taGroup_impl*)m_data;}
  
  tabGroupDataLink(taGroup_impl* data_);
  DL_FUNS(tabGroupDataLink)
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};


class TA_API IDataViewWidget { // interface that all DataViewer::widget() class must implement
friend class DataViewer;
public:
  virtual bool		isDirty() const {return false;}
  virtual QWidget*	widget() = 0; // return the widget
  DataViewer*		viewer() {return m_viewer;} // often lexically overridden to strongly type
  
  void			Constr() {Constr_impl();} // called virtually, after new, override impl
  void			Close(); // deletes us, and disconects us from viewer -- YOU MUST NOT MAKE ANY CALLS TO OBJ AFTER THIS
  
//  inline operator QWidget()	{return &(widget());} // enables convenient implicit conversion
  virtual void		ResolveChanges(CancelOp& cancel_op) {ResolveChanges_impl(cancel_op);} 
  // called from viewer prior to close; should normally autosave unsaved changes
  virtual void		SaveData() {}
      
  IDataViewWidget(DataViewer* viewer);
  virtual ~IDataViewWidget(); // informs mummy of our destruction
  
protected:
  DataViewer*		m_viewer; // our mummy
  virtual void		closeEvent_Handler(QCloseEvent* e,
    CancelOp def_cancel_op = CO_PROCEED);
    // default says "proceed", delegates decision to viewer; call with CO_NOT_CANCELLABLE for unconditional
  virtual void		Constr_impl() {} // override for virtual construction (called after new)
  virtual void		Constr_post() {} // called virtually, in DV::Constr_post -- entire win struct is now available
  virtual void		OnClosing_impl(CancelOp& cancel_op); // invoked in dtor (uncancellable); you should also invoke in the closeEvent (maybe cancellable)
  virtual void		ResolveChanges_impl(CancelOp& cancel_op) {}
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

 NOTE: you must call viewer_win()->ItemRemoving(item) in the implementation when an object is
 deleted or being removed from the viewing hierarchy.
*/

class TA_API ISelectable: public virtual IDataLinkClient { //
public: // Interface Properties and Methods
  virtual MemberDef*	md() const = 0; // memberdef in parent, if any, of the selected item
  virtual taiDataLink*	par_link() const = 0; // parent item's (if any) link
  virtual MemberDef* 	par_md() const = 0;// parent item's (if any) md
  virtual ISelectableHost* host() const = 0; //
  taBase*		taData() const; // if the data is taBase, this returns it
  virtual String	view_name() const = 0; // for members, the member name; for list items, the name if any, otherwise a created name using the index
  QWidget*		widget() const; // gets from host
  QObject*		clipHandlerObj() const; // shortcut for host()->clipHanderObj(); 

  virtual int		EditAction_(ISelectable_PtrList& sel_items, int ea);
   // do the indicated edit action (called from browser or list view); normally implement the _impl
  virtual void 		FillContextMenu(ISelectable_PtrList& sel_items, taiActions* menu);
   // normally implement the _impl
  virtual taiClipData*	GetClipData(const ISelectable_PtrList& sel_items, int src_edit_action,
    bool for_drag) const; // works for single or multi; normally not overridden
  virtual taiClipData*	GetClipDataSingle(int src_edit_action, bool for_drag) const; // normally not overridden
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

class TA_API ISelectable_PtrList: public taPtrList<ISelectable> { // for selection lists
#ifndef __MAKETA__
typedef taPtrList<ISelectable> inherited;
#endif
public:
  TypeDef*		Type1(); // data type of item
  TypeDef*		CommonSubtype1N(); // greatest common data subtype of items 1-N
  TypeDef*		CommonSubtype2N(); // greatest common data subtype of items 2-N
protected:
//TEMP nn??  override void*	El_Ref_(void* it) {((ISelectable*)it)->RefUnref(true); return it;}
//TEMP nn??  override void* 	El_unRef_(void* it) {((ISelectable*)it)->RefUnref(false);  return it;}

};


//////////////////////////
//   DynMethodDesc	//
//////////////////////////

class TA_API DynMethodDesc { // #IGNORE
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

class TA_API DynMethod_PtrList: public taPtrList<DynMethodDesc> { // #IGNORE
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

class SelectableHostHelper;

class TA_API ISelectableHost { // interface on the controlling widget hosting ISelectable items
friend class SelectableHostHelper;
public:
  enum NotifyOp { // notify ops for the NotifySignal -- note, passed as int in the sig/slot
    OP_GOT_FOCUS,
    OP_SELECTION_CHANGED,
    OP_DESTROYING
  };
  
  static const char* edit_enabled_slot; // for the SetClipboardHandler call, takes int& param
  static const char* edit_action_slot; // for the SetClipboardHandler call, takes int param
  static const char* actions_enabled_slot; // currently NULL
  static const char* update_ui_signal; // currently NULL
  
  ISelectable*		curItem() {return selItems().SafeEl(0);} // convenience
  virtual void		setCurItem(ISelectable* item, bool forceUpdate = false);
  virtual taiAction_List& dynActions() {return dyn_actions;} 
    // Action list, of current dynamic actions available
  virtual DynMethod_PtrList& dynMethods() {return dyn_methods;}
   // -- list of current dynamic methods availableselectionChanged
  QObject*		clipHandlerObj() const; 
    // provided so client can connect to us as a ClipHandler (EditEnabled, EditAction only)
  virtual bool 		hasMultiSelect() const = 0; // true if supports multi select
  virtual bool		selectionChanging() {return (m_sel_chg_cnt != 0);}
    // you can use this to escape from sundry gui notifies to yourself (to avoid recursion)
  virtual ISelectable_PtrList&	selItems() {return sel_items;} // currently selected items
  virtual QWidget*	widget() = 0; // provides a gui parent for things like context menus
  
  virtual void		FillContextMenu(taiActions* menu); 
    // s/b called by desc class, to put dynaction items onto menu
  virtual void		AddDynActions(taiActions* menu);
   // add the dynamic guys to the given menu (note: FillContextMenu does this too)
  
  virtual void 		SelectionChanging(bool begin, bool forced = true); // if used, must be called in pairs, with true then false
  virtual void 		ClearSelectedItems(bool forced = true);
  virtual void 		AddSelectedItem(ISelectable* item,  bool forced = true);
  virtual bool 		RemoveSelectedItem(ISelectable* item,  bool forced = true); 
    // 'true' if item was actually removed from (i.e. was in) list

  virtual void 		EditActionsEnabled(int&); // return enabled flags
  virtual void 		EditAction(int); // perform the action
  
  void			Emit_GotFocusSignal() {Emit_NotifySignal(OP_GOT_FOCUS);} 
    // only signal external guys should call (when we or a parent get focus)

  void 			Connect_SelectableHostNotifySignal(QObject* sink_obj,
    const char* sink_slot, bool discnct = false);  // connects (or disconnects) a sink (ex iFrame) to the notify signal raised when sel changes (or gets focus, etc.)
  void 			Connect_SelectableHostItemRemovingSlot(QObject* src_obj, 
    const char* src_signal, bool discnct = false); // connects (or disconnects) an optional ItemRemoving notification

  ISelectableHost();
  virtual ~ISelectableHost();
  
protected:
  int			m_sel_chg_cnt; 
   // counter to track batch selection changes; -ve means we are in Update (prog calls ignored) 
  ISelectable_PtrList	sel_items;
  DynMethod_PtrList	dyn_methods; // available dynamic methods
  taiAction_List	dyn_actions; // actions corresponding to methods (always 1:1)
  
  virtual void		UpdateSelectedItems_impl() = 0; 
    // called when force=true for changes, force gui to be selItems
  void			Emit_NotifySignal(NotifyOp op);
  virtual void		DoDynAction(int idx); // do the action in the list
  virtual void 		SelectionChanged(bool forced);
    // invoked when selection of current clipboard/focus handler changes
  virtual void		UpdateMethodsActions(); 
    // updates the dyn methods/actions

private:
  SelectableHostHelper* helper;
};

class TA_API SelectableHostHelper: public QObject { 
 // #IGNORE this is a helper QObject that handles signals/slots
  Q_OBJECT
INHERITED(QObject)
friend class ISelectableHost;
public:

public slots:
  void		EditActionsEnabled(int& ea) {host->EditActionsEnabled(ea);} 
    // callback for when we are ClipHandler
  void		EditAction(int ea) {host->EditAction(ea);} //  callback for when we are ClipHandler

#ifndef __MAKETA__
signals:
  void		NotifySignal(ISelectableHost* src, int op);
#endif

protected:
  ISelectableHost* 	host;
  
  override void 	customEvent(QEvent* ev); // dispatch
  void			Emit_NotifySignal(ISelectableHost::NotifyOp op);
  
  SelectableHostHelper(ISelectableHost* host_) {host = host_;}

protected slots:
  void		DynAction(int i) {host->DoDynAction(i);}
  void		ItemRemoving(ISelectable* item) {host->RemoveSelectedItem(item, false);}
private:
  SelectableHostHelper& operator=(const SelectableHostHelper&); // not allowed
};


class TA_API iFrameViewer: public QWidget, public IDataViewWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for things in the center splitter of main
  Q_OBJECT
INHERITED(QWidget)
friend class FrameViewer;
friend class MainWindowViewer;
friend class iMainWindowViewer;
public:
  
  virtual int		stretchFactor() const {return 2;} // helps set sensible alloc of space in split
  inline FrameViewer*	viewer() {return (FrameViewer*)m_viewer;} // usually lex overridden in subclass
  inline iMainWindowViewer* window() {return m_window;} // main window in which we are being shown
  
//nn??  virtual void		UpdateTabNames(); // called by a datalink when a tab name might have changed
  iFrameViewer(FrameViewer* viewer_, QWidget* parent = NULL);
  ~iFrameViewer();
  
public slots:
  void      	SelectableHostNotifySlot_Internal(ISelectableHost* src, int op);
    // connects guys nested below us; lets us trap, and forward
  void      	SelectableHostNotifySlot_External(ISelectableHost* src, int op);
   // from external guys (forwarded from main window)

#ifndef __MAKETA__
signals:
  void      	SelectableHostNotifySignal(ISelectableHost* src, int op);
    // forwarder, from all internal guys
#endif

public: // IDataViewerWidget i/f
  override QWidget*	widget() {return this;}
protected:
//  override void		Constr_impl(); // called virtually, after new

protected:
  iMainWindowViewer*    m_window;
  short int		shn_changing; // for marking forwarding, so we don't reflect back

  virtual iDataPanel* 	MakeNewDataPanel_(taiDataLink* link); // can be overridden, esp for Class browser and other non-tabase
  void 			hideEvent(QHideEvent* e); // override
  void 			showEvent(QShowEvent* e); // override
  virtual void		Showing(bool showing); // #IGNORE called by the show/hide handlers
  virtual void		SelectionChanged_impl(ISelectableHost* src_host) {}
    // we call this when we receive a valid incoming change, or maybe new focus
  
private:
  void			Init();
};


class TA_API iTabViewer : public iFrameViewer { // viewer window used for tabbed window
    Q_OBJECT
INHERITED(iFrameViewer)
friend class iTabView;
public:
  QSplitter*		spl_main; // toplevel guy
  
  virtual taiDataLink*	sel_link() const {return (cur_item) ? cur_item->link() : NULL;} // datalink of selected item that is controlling the current data panel view, ex. datalink of the selected tree node in a browser; return NULL if unknown, mult-select is in force, etc. -- controls things like clip handling
  virtual MemberDef*	sel_md() const {return (cur_item) ? cur_item->md() : NULL;}; // as for sel_link
  override int		stretchFactor() const {return 4;} // 3/2 default
  iTabView*		tabView() {return m_curTabView;} // currently active
  iTabView_PtrList*	tabViews() {return m_tabViews;} // currently active

  virtual void		AddPanel(iDataPanel* panel); // adds a new pane, and sets active in current tab
  void			AddPanelNewTab(iDataPanel* panel); // adds a new tab, sets panel active in it
  virtual iTabView*	AddTabView(QWidget* parCtrl, iTabView* splitBuddy = NULL); // adds a new tab view, optionally as a split
  void			SetPanel(iDataPanel* panel); // sets the panel active in current tab
  
  virtual void		TabView_Destroying(iTabView* tv); // called when a tabview deletes
  virtual void		TabView_Selected(iTabView* tv); // called when a tabview gets focus
  override void		UpdateTabNames(); // called by a datalink when a tab name might have changed
  iTabViewer(PanelViewer* viewer_, QWidget* parent = NULL); //
  ~iTabViewer();

public slots:
  virtual void		AddTab(); // causes tab bar to open a new tab, on current panel
  virtual void		CloseTab(); // causes current tab to close (unless only 1 tab)
  void			Closing(CancelOp& cancel_op); // override
  
  virtual void 		viewCloseCurrentView();
  virtual void 		viewSplitVertical();
  virtual void 		viewSplitHorizontal();

protected:
  iTabView_PtrList*	m_tabViews; // all created tab views
  iTabView*		m_curTabView; // tab view (split) that currently has the focus
  ISelectable*		cur_item; // the last item that was curItem -- NOTE: somewhat dangerous to cache, but according to spec, src_host should issue a new notify if this deletes
  void			Constr_Menu_impl(); // override
  override void		Constr_post(); // called virtually, in DV::Constr_post 
  override void		ResolveChanges_impl(CancelOp& cancel_op);
  override void 	SelectionChanged_impl(ISelectableHost* src_host); // called when sel changes
  void 			viewSplit(int o);

private:
  void			Init();
};

class TA_API iDockViewer: public QDockWidget, public IDataViewWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS gui portion of the DockViewer
  Q_OBJECT
INHERITED(QDockWidget)
public:
  inline DockViewer*	viewer() {return (DockViewer*)m_viewer;}
  
  iDockViewer(DockViewer* viewer_, QWidget* parent = NULL);
  ~iDockViewer();
  
public: // IDataViewerWidget i/f
  override QWidget*	widget() {return this;}
protected:
//  override void		Constr_impl();
  
protected:
  override void 	closeEvent(QCloseEvent* ev);
  void 			hideEvent(QHideEvent* e); // override
  void 			showEvent(QShowEvent* e); // override
  virtual void		Showing(bool showing); // #IGNORE called by the show/hide handlers

private:
  void			Init();
};


class TA_API iToolBoxDockViewer: public iDockViewer {
  Q_OBJECT
INHERITED(iDockViewer)
public:
  QBoxLayout*		layOuter;
  QToolBox*		  tbx;
  
  inline ToolBoxDockViewer*	viewer() {return (ToolBoxDockViewer*)m_viewer;}
  
  iToolBoxDockViewer(ToolBoxDockViewer* viewer_, QWidget* parent = NULL);
  ~iToolBoxDockViewer(); //
  
public: // IDataViewerWidget i/f
//  override QWidget*	widget() {return this;}
protected:
//  override void		Constr_impl();
  
private:
  void			Init();
};


class TA_API iToolBar: public QToolBar, public IDataViewWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(QToolBar)
friend class ToolBar;
friend class iMainWindowViewer;
  Q_OBJECT
public:
  ToolBar*		viewer() {return (ToolBar*)m_viewer;}
  iMainWindowViewer*	window() {return m_window;}
  
  iToolBar(ToolBar* viewer, QWidget* parent = NULL);
  ~iToolBar();
  
public: // IDataViewerWidget i/f
  override QWidget*	widget() {return this;}
protected:
  iMainWindowViewer*    m_window; // set when added
//  override void		Constr_impl();

protected:
  void 			hideEvent(QHideEvent* e); // override
  void 			showEvent(QShowEvent* e); // override
  virtual void		Showing(bool showing); // #IGNORE called by the show/hide handlers
private:
  void			Init();
};

class TA_API iToolBar_List: public taPtrList<iToolBar> {
 // each mainwin maintains its existent toolbars in this list
public:
protected:
  override String El_GetName_(void* it) const;
};


class TA_API iApplicationToolBar: public iToolBar {
  // widget for the Application toolbar -- created by ToolBar if name=Application
INHERITED(iToolBar)
public:
  iApplicationToolBar(ToolBar* viewer, QWidget* parent = NULL)
  :iToolBar(viewer, parent){}
 protected:
  override void		Constr_post(); 
};

class TA_API iBaseClipToolWidget: public iClipToolWidget {
  // for making drag/copy guys from a taBase instance (ex. see programs_qtso)
INHERITED(iClipToolWidget)
  Q_OBJECT
public:
  taBase*		base() const {return m_inst;}
  void			setBase(taBase* value); //
  
  
  // tooltip defaults to key_desc of the instance		
  iBaseClipToolWidget(taBase* inst = NULL, QWidget* parent = NULL);
  iBaseClipToolWidget(const QIcon & icon, taBase* inst = NULL, QWidget* parent = NULL);
  iBaseClipToolWidget(const String& tooltip, const QIcon & icon,
    taBase* inst = NULL, QWidget* parent = NULL);
  iBaseClipToolWidget(const String& text,
    taBase* inst = NULL, QWidget* parent = NULL);
  iBaseClipToolWidget(const String& tooltip, const String& text,
    taBase* inst = NULL, QWidget* parent = NULL);
   
  
protected:
  taSmartRef		m_inst;
  
  override QMimeData* 	mimeData() const; // delegates to the inst
  override QStringList  mimeTypes() const; // the ta custom type
private:
  void			Init(taBase* inst, String tooltip = _nilString);
};


//////////////////////////
//   iMainWindowViewer	//
//////////////////////////

class TA_API iMainWindowViewer: public QMainWindow, public IDataViewWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS gui portion of the DataViewer
  Q_OBJECT
INHERITED(QMainWindow)
friend class taDataLink;
friend class MainWindowViewer;
public: //
//nn  iToolBar_List		toolbars; // list of all created toolbars
  taiAction_List	actions; // our own list of all created actions

  taiMenuBar*		menu;		// menu bar -- note: we use the window's built-in QMenu
  QSplitter*		body;		// #IGNORE body of the window

  taiMenu* 		fileMenu;
  taiMenu* 		fileExportMenu; // submenu -- empty and disabled in base
  taiMenu* 		editMenu;
  taiMenu* 		viewMenu;
  taiMenu* 		frameMenu; // enumeration of all Frame guys
  taiMenu* 		toolBarMenu; // enumeration of all ToolBar guys
  taiMenu* 		dockMenu; // enumeration of all Dock guys
  taiMenu* 		toolsMenu;
  taiMenu* 		actionsMenu; // statically added items first; bottom section is for dynamic
  taiMenu* 		helpMenu;
  taiAction* 		fileNewAction;
  taiAction* 		fileOpenAction;
  taiAction* 		fileSaveAction;
  taiAction* 		fileSaveAsAction;
  taiAction* 		fileSaveAllAction;
  taiAction* 		fileCloseAction;
  taiAction* 		fileOptionsAction;
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
  taiAction* 		viewSplitVerticalAction;
  taiAction* 		viewSplitHorizontalAction;
  taiAction* 		viewCloseCurrentViewAction;

  taiAction*	        toolsClassBrowseAction;
  
  taiAction* 		helpHelpAction;
  taiAction* 		helpAboutAction;

  QObject* 		clipHandler() {return last_clip_handler;} // obj (if any) controlling clipboard handling
  
  taProject*		curProject() const; // only if we are a projviewer
  inline bool		isRoot() const {return m_is_root;} // if this is app root, closing quits
  inline bool		isProjViewer() const {return m_is_proj_viewer;} // if a project viewer, persistent

  inline MainWindowViewer* viewer() const {return (MainWindowViewer*)m_viewer;} 

  virtual taiAction*	AddAction(taiAction* act); // add the action to the list, returning the instance (for convenience)
  void			AddPanelNewTab(iDataPanel* panel); 
    // insures we have a iTabViewer; adds a new tab, sets panel active in it
  virtual void		AddToolBar(iToolBar* tb); // add the toolbar, showing it if it is mapped
  virtual void		AddFrameViewer(iFrameViewer* fv, int at_index = -1); // -1=end
#ifndef __MAKETA__
  virtual void		AddDockViewer(iDockViewer* dv,
    Qt::DockWidgetArea in_area = Qt::BottomDockWidgetArea); 
#endif
  int			GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void 			setFrameGeometry(const iRect& r);
  void			setFrameGeometry(int left, int top, int width, int height); //bogus: see Qt docs on geometry under X

  iMainWindowViewer(MainWindowViewer* viewer_, QWidget* parent = NULL);
    // uses: WFlags flags = (WType_TopLevel | WStyle_SysMenu | WStyle_MinMax | WDestructiveClose)
//obs  iMainWindowViewer(DataViewer* viewer_, QWidget* parent, WFlags flags); //note: only for completeness, usually use the one with implicit flags
  ~iMainWindowViewer();

public slots:
  virtual void 	fileNew(); // New Project (in new viewer)
  virtual void 	fileOpen(); // Open Project (in new viewer)
  virtual void 	fileSave();  // Save Project (only enabled if viewer)
  virtual void 	fileSaveAs();  // SaveAs Project (only enabled if viewer)
  virtual void 	fileSaveAll();  // Save All Projects (always enabled)
  virtual void 	fileClose(); // Close Project (only enabled if viewer)
  virtual void 	fileOptions(); // edits taMisc
  virtual void 	filePrint(){}
  virtual void 	fileCloseWindow(); // Quit (root) or Close Window (non-root)
/*  virtual void editUndo();
  virtual void editRedo();
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editFind(); */
  virtual void	viewRefresh() {} // rebuild/refresh the current view
  virtual void 	viewCloseCurrentView();
  virtual void 	viewSplitVertical();
  virtual void 	viewSplitHorizontal();
  
  virtual void	toolsClassBrowser();
/*  virtual void helpIndex();
  virtual void helpContents();*/
  virtual void 	helpAbout();

  virtual void	mnuEditAction(taiAction* mel);
//nn  void		actionsMenu_aboutToShow(); // populates dynamic items
  
  void		SetClipboardHandler(QObject* handler_obj,
    const char* edit_enabled_slot = NULL,
    const char* edit_action_slot = NULL,
    const char* actions_enabled_slot = NULL,
    const char* update_ui_signal = NULL); // see "Clipboard Handling" in .cpp
    
  void		SelectableHostNotifySlot(ISelectableHost* src_host, int op); 
    // see "Selection Handling" in .cpp
    
  virtual void	UpdateUi(); 
    // Clipboard server: called by cliphandler after major events, to refresh menus, toolbars, etc.

#ifndef __MAKETA__
signals:
  void 		EditAction(int ea); 
    // Clipboard server: param is one of the taiClipData editAction values
  void		GetEditActionsEnabled(int& ea); 
    // Clipboard server: param is one of the taiClipData EditAction values
  void		SetActionsEnabled(); 
    // Clipboard server: enable/disable actions
    
  void		SelectableHostNotifySignal(ISelectableHost* src_host, int op); 
    // see "Selection Handling" in .cpp
#endif

public: // IDataViewWidget i/f
  override bool		isDirty() const;
  override QWidget*	widget() {return this;}
  override void		SaveData();
protected:
  override void		Constr_impl();
  override void 	ResolveChanges_impl(CancelOp& cancel_op); // only for project browsers

protected slots:
  void			ch_destroyed(); // cliphandler destroyed (just in case it doesn't deregister)

  virtual void 		this_FrameSelect(taiAction* me); // user has selected or unselected one of the frames
  virtual void 		this_ToolBarSelect(taiAction* me); // user has selected or unselected one of the toolbars
  virtual void 		this_DockSelect(taiAction* me); // user has selected or unselected one of the docks

protected:
  bool			m_is_root; // true if this is a root window (has Quit menu)
  bool			m_is_proj_viewer; // true if this is a project viewer (false for simple browsers)
  int			m_last_action_idx; // index of last static action in actionMenu
  override void 	closeEvent(QCloseEvent* ev);
  virtual void 		emit_EditAction(int param); // #IGNORE param is one of the taiClipData editAction values; desc can trap this and implement virtually, if desired
  override void 	windowActivationChange(bool oldActive); // we manage active_wins in order of activation

  virtual void		Constr_MainMenu_impl(); // #IGNORE constructs the main menu items, and loads static images
  virtual void		Constr_Menu_impl(); // #IGNORE constructs the menu and actions; MUST construct all static actions
  virtual void		SelectableHostNotifying_impl(ISelectableHost* src_host, int op);
    // called when we should handle this for sure
  void 			UpdateActionsMenu(ISelectableHost* src_host, bool do_add);

private:
  QObject* last_clip_handler; //we need to remember this, because you can't anonymously disconnect signals from your own slots
  ISelectableHost* last_sel_server; // last guy to get focus
  QPixmap image0;
  QPixmap image1;
  QPixmap image2;
  QPixmap image3;
  QPixmap image4;
  QPixmap image5;
  QPixmap image6;
  QPixmap image7;
  QPixmap image8;
  void			Init();
};


//////////////////////////
// 	iTabBar 	//
//////////////////////////

class TA_API iTabBar: public QTabBar { // #IGNORE encapsulates the TabBar for iTabView
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

class TA_API iDataPanel_PtrList: public taPtrList<iDataPanel> { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  iTabView*	m_tabView; // optional, we manage the refs in panels if set
  iDataPanel_PtrList() {m_tabView = NULL;}
  ~iDataPanel_PtrList() {}
protected:
  override void*	El_Own_(void* it); // set ref to our tabview (on Add, not Link)	
  override void		El_disOwn_(void* it_); // remove ref if it is our tabview	
};


class TA_API iTabView: public QWidget {
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
  iTabViewer* 		tabViewerWin() {return m_viewer_win;}
  iMainWindowViewer* 	window() {return (m_viewer_win) ? m_viewer_win->window() : NULL;}

  void			Activated(bool val); // called by parent to indicate if we are active tabview or not
  bool 			ActivatePanel(taiDataLink* dl); // if a panel exists for the link, make it active and return true
  void			AddPanel(iDataPanel* panel); // adds a new pane, and sets active in current tab
  void			AddPanelNewTab(iDataPanel* panel); // adds a new tab, sets panel active in it
  void 			Closing(CancelOp& cancel_op);
  void 			DataPanelDestroying(iDataPanel* panel);
  virtual void		FillTabBarContextMenu(QMenu* contextMenu);
  virtual iDataPanel*	GetDataPanel(taiDataLink* link); // get panel for indicated link, or make new one; par_link is not necessarily data item owner (ex. link lists, references, etc.)
  void 			RemoveDataPanel(iDataPanel* panel);
  void			ResolveChanges(CancelOp& cancel_op);
  void			OnWindowBind(iTabViewer* itv); // called at constr_post time
  void 			SetPanel(iDataPanel* panel);

  iTabView(QWidget* parent = NULL);
  iTabView(iTabViewer* data_viewer_, QWidget* parent = NULL);
  ~iTabView();

public slots:
  void 			AddTab();
  void 			CloseTab();
  virtual void		panelSelected(int idx);
  void 			UpdateTabNames(); // called by a datalink when a tab name might have changed; panels also hook to this

protected:
  iTabViewer* 	m_viewer_win;

private:
  iDataPanel_PtrList	panels; // no external hanky-panky with this puppie
  void			Init();
};

//////////////////////////
//   iTabView_PtrList	//
//////////////////////////

class TA_API iTabView_PtrList: public taPtrList<iTabView> {
public:
  void			DataPanelDestroying(iDataPanel* panel); // dispatch to all
  iTabView_PtrList() {}
  ~iTabView_PtrList();
};


//////////////////////////
//   iDataPanel 	//
//////////////////////////

class TA_API iDataPanel: public QFrame, public IDataLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS interface for panels (note: don't inherit directly)
  Q_OBJECT
friend class taDataLink;
friend class iPanelTab;
friend class iDataPanel_PtrList;
friend class iDataPanelSet;
public:
  virtual void		setCentralWidget(QWidget* widg); // sets the contents
  virtual bool		dirty() {return HasChanged();}
    // true if panel should not be replaced, but a new panel should be opened for the new item
  taiDataLink*		link() {return (taiDataLink*)m_link;}
  const taiDataLink*	link() const {return (taiDataLink*)m_link;}
  virtual bool		lockInPlace() {return false;}
    // true if panel should not be replaced when a user clicks around
  virtual String	panel_type() const {return _nilString;}
   //  this string is on the subpanel button for a panel (n/a to panelsets)
  virtual taiDataLink*	par_link() const = 0; // *current* visual parent link of this data panel; this could change dynamically, if a datapanel is shared across all referring instances, ex. link lists, references, etc. -- return NULL if unknown, not set, or not applicable -- controls things like clip enabling etc.
  virtual MemberDef*	par_md() const = 0; // as for par_link
//  DataViewer*		viewer() {return (m_dps) ? m_dps->viewer() : m_tabView->viewer();}
  iTabView*		tabView() {return m_tabView;} // tab view in which we are shown
  virtual iTabViewer* 	tabViewerWin() const = 0;
  iMainWindowViewer* 	window() {return (m_tabView) ? m_tabView->window() : NULL;}


  virtual void		Closing(CancelOp& cancel_op) {} // called to notify panel is(forced==true)/wants(forced=false) to close -- set cancel 'true' (if not forced) to prevent
  virtual void		ClosePanel() = 0; // anyone can call this to get the panel to close (ex. edit panel contents are deleted externally)
  virtual void		GetImage() = 0; // called when reshowing a panel, to insure latest data
  virtual const iColor* GetTabColor(bool selected) const {return NULL;} // special color for tab; NULL means use default
  virtual bool		HasChanged() {return false;} // 'true' if user has unsaved changes -- used to prevent browsing away
  virtual void		OnWindowBind(iTabViewer* itv) {OnWindowBind_impl(itv);}
    // called in post, when all windows are built
  virtual void		ResolveChanges(CancelOp& cancel_op);
  virtual String 	TabText() const; // text for the panel tab -- usually just the view_name of the curItem

  iDataPanel(taiDataLink* dl_); //note: created with no parent -- later added to stack
  ~iDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2)
    {DataChanged_impl(dcr, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  override void		DataLinkDestroying(taDataLink* dl) {} // called by DataLink when it is destroying --
  override TypeDef*	GetTypeDef() const {return &TA_iDataPanel;}

protected:
  iTabView*		m_tabView; // tab view in which we are shown
  QScrollArea*		scr; // central scrollview
  virtual void		DataChanged_impl(int dcr, void* op1, void* op2); // tab name may have changed
  virtual void		OnWindowBind_impl(iTabViewer* itv) {}
  virtual void		ResolveChanges_impl(CancelOp& cancel_op) {}
};


//////////////////////////
//   iDataPanelFrame 	//
//////////////////////////

class TA_API iDataPanelFrame: public iDataPanel {
  // interface for panel frames
  Q_OBJECT
INHERITED(iDataPanel)
friend class iDataPanelSet;
public:
  override taiDataLink*	par_link() const; // taken from dps if any, else from tabview
  override MemberDef*	par_md() const;
  override iTabViewer*	tabViewerWin() const;


  override void		ClosePanel();
  override void		GetImage(); // called when reshowing a panel, to insure latest data
  override String 	TabText() const; // text for the panel tab -- usually just the text of the sel_node

  virtual void		AddedToPanelSet() {} // called when fully added to DataPanelSet
  
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

class TA_API iViewPanelFrame: public iDataPanel {
  // frame for gui interface to a view element -- usually posted by the view, and locked
  Q_OBJECT
INHERITED(iDataPanel)
public:
  taDataView*		dv() {return m_dv;} // can be statically replaced with subclass
  override bool		lockInPlace() {return true;}
    // true if panel should not be replaced, ex. if dirty, or viewpanel
  override taiDataLink*	par_link() const {return NULL;} // n/a
  override MemberDef*	par_md() const {return NULL;}
  override iTabViewer* tabViewerWin() const;


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

//NOTE: this class is only designed to handle a once-only adding of its subpanels; it cannot
// handle dynamically adding and removing subpanels

class TA_API iDataPanelSet: public iDataPanel { //  contains 0 or more sub-data-panels, and a small control bar for selecting panels
  Q_OBJECT
INHERITED(iDataPanel)
public:
  int			cur_panel_id; // -1 if none
  iDataPanel_PtrList	panels;
  QVBoxLayout*		layDetail;
    QFrame*		frmButtons;
      QHBoxLayout*	layButtons;
      QButtonGroup*	buttons; // one QPushButton for each (note: not a widget)
    Q3WidgetStack*	wsSubPanels; // subpanels

  override taiDataLink*	par_link() const {return (m_tabView) ? m_tabView->par_link() : NULL;}
  override MemberDef*	par_md() const {return (m_tabView) ? m_tabView->par_md() : NULL;}
  override iTabViewer* tabViewerWin() const {return (m_tabView) ? m_tabView->tabViewerWin() : NULL;}

  iDataPanel*		cur_panel() const {return panels.SafeEl(cur_panel_id);} // NULL if none
  void			set_cur_panel_id(int cpi);

  void			AddSubPanel(iDataPanelFrame* pn);
  void			AllSubPanelsAdded(); // call after all subpanels added, to finalize layout

  override void		Closing(CancelOp& cancel_op);
  override void		ClosePanel();
  override void		GetImage();
  override const iColor* GetTabColor(bool selected) const;
  override bool		HasChanged();
  override void 	ResolveChanges(CancelOp& cancel_op); // do the children first, then our impl


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
  override void		OnWindowBind_impl(iTabViewer* itv);
};

class TA_API iTreeView: public iTreeWidget, public ISelectableHost {
  //  ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for all views of iTreeViewItems
INHERITED(iTreeWidget)
  Q_OBJECT
friend class iTreeViewItem;
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjDataRole = Qt::UserRole + 1, // for additional data
    ColKeyRole,	// store a string in header to indicate the col key to use for data
  };
#endif
  enum TreeViewFlags { // #BITS
    TV_NONE		= 0, // #NO_BIT
    TV_AUTO_EXPAND	= 0x0001 // expands all automatically on open
  };
  enum ContextMenuPosition {
    CM_START,		// called before filling of menu -- use to add items to start
    CM_END		// called after filling menu -- use to add items to end
  };
  
#ifndef __MAKETA__  
  static void 		FillTypedList(const QList<QTreeWidgetItem*>& items,
    ISelectable_PtrList& list); // helper, for filling our own typed list
#endif
  
  const KeyString	colKey(int col) const; // the key we set for data lookup
  void			setColKey(int col, const KeyString& key); 
    // sets in ColKeyRole -- you can do it yourself if you want	
  void			setHeaderText(int col, const String& value); // convenience
  inline TreeViewFlags	tvFlags() const {return (TreeViewFlags)tv_flags;}
  void			setTvFlags(int value);
  

  iTreeView(QWidget* parent = 0, int tv_flags = 0);
   
#ifndef __MAKETA__
signals:
  void			FillContextMenuHookPre(ISelectable_PtrList& sel_items, taiMenu* menu);
    // hook to allow client to add items to start of context menu before it shows
  void			FillContextMenuHookPost(ISelectable_PtrList& sel_items, taiMenu* menu);
    // hook to allow client to add items to end of context menu before it shows
  void			ItemSelected(iTreeViewItem* item); 
    // NULL if none -- NOTE: the preferred way is to use ISelectableHost::Notify signal
#endif
  
public slots:
  virtual void		mnuNewBrowser(taiAction* mel); // called from context 'New Browse from here'; cast obj to taiTreeDataNode*
  void			ExpandAll(int max_levels = 6); 
    // expand all nodes, ml=-1 for "infinite" levels (there better not be any loops!!!)
  void			CollapseAll(); // collapse all nodes
  void			ExpandAllUnder(iTreeViewItem* item, int max_levels = 6); 
    // expand all nodes under item, ml=-1 for "infinite" levels (there better not be any loops!!!)
  void			CollapseAllUnder(iTreeViewItem* item); // collapse all nodes under item

public: // ISelectableHost i/f
  override bool 	hasMultiSelect() const;
  override QWidget*	widget() {return this;} 
protected:
  override void		UpdateSelectedItems_impl(); 
  
protected:
  int			tv_flags;
  void 			focusInEvent(QFocusEvent* ev); // override
  void			showEvent(QShowEvent* ev); // override, for expand all
  void 			ExpandAllUnder_impl(iTreeViewItem* item, int max_levels); // inner code
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected datanodes
#ifndef __MAKETA__
  override QMimeData* 	mimeData(const QList<QTreeWidgetItem*> items) const; 
    // we replace this and provide the ta custom mime data (not the treewidget data)
  override QStringList 	mimeTypes () const; // for dnd to work, we just permit almost anything via "text/plain", then decide on the drop whether to accept  
#endif
  virtual void		ItemDestroyingCb(iTreeViewItem* item); 
  
protected slots:
  void 			this_contextMenuRequested(QTreeWidgetItem* item,
    const QPoint & pos, int col ); //note: should probably rejig to use a virtual method
  void			this_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
  void 			this_itemSelectionChanged();
  void			ExpandAllUnderInt(void* item); 
  void			CollapseAllUnderInt(void* item); 
};


class TA_API iTreeViewItem: public iTreeWidgetItem, public ISelectable {
  //  ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for Tree and List nodes
INHERITED(iTreeWidgetItem)
friend class iTreeView;
public:
  enum DataNodeFlags {
    DNF_IS_FOLDER 	= 0x001, // true for list/group folder nodes (note: does *not* indicate whether item can contain other things or not)
    DNF_LAZY_CHILDREN 	= 0x002, // start w/ dummy child; create real children when expanded
    DNF_UPDATE_NAME	= 0x004, // typically for list items, update the visual name (tree, tab, etc.) after item edited
    DNF_CAN_BROWSE	= 0x008, // can be a new browser root
    DNF_CAN_DRAG	= 0x010, // 16 can allow drags
    DNF_NO_CAN_DROP	= 0x020, // 32 cannot accept drops
    DNF_IS_MEMBER 	= 0x040, // 64 true for members (and root), not for list/group items -- helps node configure edit ops
    DNF_IS_LIST_NODE 	= 0x080 // true for nodes in a list view (in panel, not on tree)
  };

/*nn  enum BrowseDropAction {
    BDA_MOVE,
    BDA_COPY,
    BDA_LINK,
    BDA_MOVE_AS_SUBGROUP, // moves item as a subgroup, ex a Unit group
    BDA_MOVE_AS_SUBITEM // moves item as a subitem, ex a sub spec, or subprocess
  }; */

  int			dn_flags; // any of DataNodeFlags

  void* 		data() {return m_link->data();} //
  iTreeView*		treeView() const;

  override bool 	acceptDrop(const QMimeData* mime) const;
  override void 	CreateChildren(); 
  virtual void		DecorateDataNode(); // sets icon and other visual attributes, based on state of node

  iTreeViewItem(taiDataLink* link_, MemberDef* md_, iTreeViewItem* parent_,
    iTreeViewItem* after, const String& tree_name, int dn_flags_ = 0);
  iTreeViewItem(taiDataLink* link_, MemberDef* md_, iTreeView* parent_,
    iTreeViewItem* after, const String& tree_name, int dn_flags_ = 0);
  ~iTreeViewItem();
  
public: // qt3 compatability functions, for convenience
  bool			dragEnabled() const {return flags() & Qt::ItemIsDragEnabled;}
  void			setDragEnabled(bool value) {Qt::ItemFlags f = flags(); if (value) 
    setFlags(f | Qt::ItemIsDragEnabled); else setFlags(f & ~Qt::ItemIsDragEnabled);}
    
  bool			dropEnabled() const {return flags() & Qt::ItemIsDropEnabled;}
  void			setDropEnabled(bool value) {Qt::ItemFlags f = flags(); if (value) 
    setFlags(f | Qt::ItemIsDropEnabled); else setFlags(f & ~Qt::ItemIsDropEnabled);}
    
  void			moveChild(int fm_idx, int to_idx); //note: to_idx is based on before
  void			swapChildren(int n1_idx, int n2_idx);

public: // ITypedObject interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iTreeViewItem;}

public: // IDataLinkClient interface
  override void		DataDataChanged(taDataLink*, int dcr, void* op1, void* op2)
    {DataChanged_impl(dcr, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  override void		DataLinkDestroying(taDataLink* dl); // called by DataLink when it is destroying --

public: // ISelectable interface
  override MemberDef*	md() const {return m_md;}
  override String	view_name() const; // for members, the member name; for list items, the name if
  override ISelectableHost* host() const;
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
  override void		dropped(const QMimeData* mime, const QPoint& pos);
  virtual void		DataChanged_impl(int dcr, void* op1, void* op2); // called for each node when the data item has changed, esp. ex lists and groups
  override void 	itemExpanded(bool value);
private:
  void			init(const String& tree_name, taiDataLink* link_, 
    MemberDef* md_, int dn_flags_); // #IGNORE
};


class TA_API taiTreeDataNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  taiTreeDataNode*	parent() const {return (taiTreeDataNode*) QTreeWidgetItem::parent();} //note: NULL for root item

  taiTreeDataNode*	FindChildForData(void* data, int& idx = no_idx); // find the Child Node (if any) that has data as the data of its link; NULL/-1 if not found
  virtual void		UpdateChildNames() {} // #IGNORE update child names of this node

  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~taiTreeDataNode();
public: // ITypedObject interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taiTreeDataNode;}
public: // ISelectable interface
  override taiDataLink* par_link() const; // we get from the panel, which gets from the viewer window
  override MemberDef* 	par_md() const; // as for par_link
//  override ISelectableHost* host() const;
protected:
  taiTreeDataNode*	last_member_node; // #IGNORE last member node created, so we know where to start list/group items
  taiTreeDataNode*	last_child_node; // #IGNORE last child node created, so we can pass to createnode
  override void 	CreateChildren_impl();
  override void		FillContextMenu_impl(taiActions* menu);
private:
  static int		no_idx; // dummy parameter
  void			init(taiDataLink* link_, int dn_flags_); // #IGNORE
};


class TA_API taiListDataNode: public iTreeViewItem {
INHERITED(iTreeViewItem)
public:
  int			num; // item number, starting from 1
  iListDataPanel*	panel; // logical parent node of the list items

  QString		text(int col) const; // override

  bool 			operator<(const QTreeWidgetItem& item) const; // override

  taiListDataNode(int num_, iListDataPanel* panel_, taiDataLink* link_,
    iTreeView* parent_, taiListDataNode* after, int dn_flags_ = 0);
    //note: list flag automatically or'ed in
  ~taiListDataNode(); //
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taiListDataNode;}

public: // ISelectable interface
  override taiDataLink* par_link() const; // we get from the panel, which gets from the viewer window
  override MemberDef* 	par_md() const; // as for par_link
};


//////////////////////////
//   iListDataPanel 	//
//////////////////////////

class TA_API iListDataPanel: public iDataPanelFrame {
  Q_OBJECT
#ifndef __MAKETA__
typedef iDataPanelFrame inherited;
#endif
public:
  iTreeView*		list; //actually an iLDPListView

  override String	panel_type() const; // this string is on the subpanel button for this panel

  void			ClearList(); // for when data changes -- we just rebuild the list
  void			FillList();
  
  iListDataPanel(taiDataLink* dl_);
  ~iListDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iListDataPanel;}
protected:
  iTreeViewItem* 	mparentItem;
  void 			ConfigHeader();
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
};

class TA_API iTextDataPanel: public iDataPanelFrame {
  // a panel frame for displaying text; used, ex. by Scripts and Programs
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
  QTextEdit*		txtText; // the text of the script
  
  virtual bool		readOnly();
  virtual void		setReadOnly(bool value);
  virtual void		setText(const String& value);
  

  override String	panel_type() const;

  override int 		EditAction(int ea);
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)

  iTextDataPanel(taiDataLink* dl_);
  ~iTextDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iTextDataPanel;}
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

protected slots:
  void 			textText_copyAvailable (bool yes);

};


#endif // TA_QTVIEWER_H

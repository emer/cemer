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

#ifndef TA_QTBROWSE_H
#define TA_QTBROWSE_H

#include "ta_base.h"
#include "ta_qttype_def.h"
#include "ta_qtbrowse_def.h"
#include "ta_qtviewer.h"

#ifndef __MAKETA__
  #include <Q3ListView>
#endif

// externals
class taBase;
class taList_impl;
class taGroup_impl;
class taiClipData;
class taiMimeItem;
class taiMimeSource;
class taiAction;
class taiMenu; //

// forwards this file
class taiTreeDataNode;
class tabTreeDataNode;
class tabListTreeDataNode;
class tabGroupTreeDataNode;
class iDataBrowserBase;
class iDataBrowser;
class DataBrowser; //


/* note used, for now (taList items placed directly below parent item, not in second level "items" folder )
class taiListItemsDataLink: public taiDataLink {
  // DataLink for taPtrList item objects -- handles any type of item content -- this is only the node handler for the 'items' node, not the node representing the list object itself
public:
  taPtrList_impl*	data() const {return (taPtrList_impl*)m_data;}
  void 			CreateChild(iListViewItem* par_node, iListViewItem* after_node, void* el);
  // used for items inserted after populating
  override void		CreateChildren(iListViewItem* par_node); // parent node will be the list object data link
  override bool		HasChildItems(); // true if has any items in list
  override String 	GetName() const;
  taiListItemsDataLink(taPtrList_impl* data_, iDataBrowser* browser_);
protected:
  override iDataPanel* CreateDataPanel(); // show a list panel
  override void		DataChanged_impl(iListViewItem* nd, int dcr, void* op1, void* op2);
}; */


class TA_API taiTreeDataNode: public iListViewItem {
#ifndef __MAKETA__
typedef iListViewItem inherited;
#endif
public:

  bool			children_created;

  taiTreeDataNode*	parent() const {return (taiTreeDataNode*) Q3ListViewItem::parent();} //note: NULL for root item
//obs  iDataBrowser*		browser_win() const {return (iDataBrowser*)viewer_win();}
  DataViewer*		browser() const;
  iDataBrowserBase* 	browser_win() const;

  virtual void 		CreateChildren(); // called by the Node when it needs to create its children
  taiTreeDataNode*	FindChildForData(void* data); // find the Child Node (if any) that has data as the data of its link
  virtual void		UpdateChildNames() {} // #IGNORE update child names of this node
  void 			moveItem(taiTreeDataNode* after); // enhances base method by letting us move to first with after=NULL
  override void		setOpen(bool value); // set true/false when use clicks +/-

  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taiTreeDataNode(taiDataLink* link_, MemberDef* md_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taiTreeDataNode();
public: // ITypedObject interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taiTreeDataNode;}
public: // ISelectable interface
  override taiDataLink* par_link() const; // we get from the panel, which gets from the viewer window
  override MemberDef* 	par_md() const; // as for par_link
  override IDataViewHost* host() const;
protected:
  taiTreeDataNode*	last_member_node; // #IGNORE last member node created, so we know where to start list/group items
  taiTreeDataNode*	last_child_node; // #IGNORE last child node created, so we can pass to createnode
  virtual void 		CreateChildren_impl(); // implement child creating here
  override void		FillContextMenu_impl(taiActions* menu);
private:
  void			init(taiDataLink* link_, int flags_); // #IGNORE
};


class TA_API tabTreeDataNode: public taiTreeDataNode {
#ifndef __MAKETA__
typedef taiTreeDataNode inherited;
#endif
public:
  taBase* 		data() {return ((tabDataLink*)m_link)->data();}
  tabDataLink* 		link() const {return (tabDataLink*)m_link;}


  tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  tabTreeDataNode(tabDataLink* link_, MemberDef* md_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~tabTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabTreeDataNode;}
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2);
private:
  void			init(tabDataLink* link_, int flags_); // #IGNORE
};


class TA_API tabListTreeDataNode: public tabTreeDataNode {
#ifndef __MAKETA__
typedef tabTreeDataNode inherited;
#endif
public:
  taList_impl* 		data() {return ((tabListDataLink*)m_link)->data();}
  tabListDataLink* 	link() const {return (tabListDataLink*)m_link;}

  void			AssertLastListItem(); // #IGNORE updates last_list_items_node -- called by Group node before dynamic inserts/updates etc.
  override void		UpdateChildNames(); // #IGNORE update child names of the indicated node

  tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~tabListTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabListTreeDataNode;}
protected:
  taiTreeDataNode*	last_list_items_node; // #IGNORE last list member node created, so we know where to start group items
  override void		DataChanged_impl(int dcr, void* op1, void* op2);
  override void 	CreateChildren_impl(); 
  void			CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after_node, void* el);
  void			UpdateListNames(); // #IGNORE updates names after inserts/deletes etc.
private:
  void			init(tabListDataLink* link_, int flags_); // #IGNORE
};


class TA_API tabGroupTreeDataNode: public tabListTreeDataNode {
#ifndef __MAKETA__
typedef tabListTreeDataNode inherited;
#endif
public:
  taGroup_impl* 	data() {return ((tabGroupDataLink*)m_link)->data();}
  tabGroupDataLink* 	link() const {return (tabGroupDataLink*)m_link;}

  void 			CreateSubGroup(taiTreeDataNode* after_node, void* el); // for dynamic changes to tree
  override void		UpdateChildNames(); // #IGNORE

  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~tabGroupTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabGroupTreeDataNode;}
protected:
  override void 	CreateChildren_impl(); 
  override void		DataChanged_impl(int dcr, void* op1, void* op2); // handle DCR_GROUP_xxx ops
  void			UpdateGroupNames(); // #IGNORE updates names after inserts/deletes etc.
private:
  void			init(tabGroupDataLink* link_, int flags_); // #IGNORE
};


//////////////////////////////////
//	iDataBrowser	//
//////////////////////////////////

/*
 * splMain is the core control for the browsing system -- it is a TreeView and Frame
   in a splitter -- the frame can show one or more Panels in a tab view -- clicking on a TreeNode
   causes the corresponding Panel to be shown in the active tab. The user can open new tabs, to
   keep the panel "bookmarked", so they don't have to keep navigating around the entire tree to
   switch views quickly between panels of interest.

   The destruction sequence is as follows:
     Window starts destructing
       DataBrowser starts destructing (splMain)
       *following happens in either order*
         Root DataNode starts destructing
           DataNode informs DataLink, which removes it --
             DataLink destructs if all DataNodes removed
               Destructing DataLink destructs its DataPanel, which removes it from tabbed parent
           DataNode recursively starts destructing its children, each informing DataLink as above
         Tabbed parent starts destructing, will destruct each DataPanel (if any left)
           DataPanel destructs -- if still connected to a link, it severs the link connection
*/

class TA_API iDataBrowserBase : public iTabDataViewer { // base of viewer window used for class browsing
    Q_OBJECT
INHERITED(iTabDataViewer)
friend class DataBrowser;
public:

  QSplitter*		splMain; // main splitter
  Q3ListView*		lvwDataTree; // actually an iListView

//  DataBrowser*		browser() {return (DataBrowser*)m_viewer;}
  void*			root() { return m_root;}

  iTabView*		AddTabView(QWidget* parCtrl, iTabView* splitBuddy = NULL);// override
  virtual taiTreeDataNode* CreateTreeDataNode(taiDataLink* link, MemberDef* md_, 
    taiTreeDataNode* parent_, taiTreeDataNode* last_child_, 
    const String& tree_name, int flags_);
  void			DataPanelDestroying(iDataPanel* panel); // override - called by DataPanel when it is destroying -- remove from all tabs
  void			TreeNodeDestroying(taiTreeDataNode* item); // #IGNORE check if curItem
//nn  void		RemovePanel(iDataPanel* panel); // removes and deletes the indicated panel
  taiClipData*		GetClipData(int src_edit_action, bool for_drag = false); // gets clipboard data (called on Cut/Copy or Drag)
//nn  int			GetEditActions(); // override
  void			lvwDataTree_focusInEvent(QFocusEvent* ev);
  void			Reset();
  override void 	SelectionChanged(bool forced = false); // invoked when selection changes; builtin clipboard
  ~iDataBrowserBase();

public slots:
  virtual void		mnuNewBrowser(taiAction* mel){} // called from context 'New Browse from here'; cast obj to taiNode*
  virtual void		mnuBrowseNodeDrop(int param) {mnuBrowseNodeDrop_param = param;} // called from within the node->dropped event

protected slots:
  virtual void		lvwDataTree_contextMenuRequested(Q3ListViewItem* item, const QPoint & pos, int col);
  virtual void		lvwDataTree_selectionChanged(Q3ListViewItem* item);
//  void btnRecurse_toggled(bool on);


protected:
  virtual taiTreeDataNode* CreateTreeDataNode_impl(taiDataLink* link, MemberDef* md_, 
    taiTreeDataNode* parent_, taiTreeDataNode* last_child_, const String& tree_name,
    int flags_) {return NULL;}
  iDataBrowserBase(void* root_, DataViewer* browser_,  QWidget* parent = 0);

public: // overridden slots
//  void 		fileNew();
//  void 		fileOpen();
//  void 		fileSave();
//  void 		fileSaveAs();
//  void 		fileSaveAll();
//  void 		fileClose();
//  virtual void filePrint();
//  virtual void editUndo();
//  virtual void editRedo();
//  virtual void editCut();
//  virtual void editCopy();
//  virtual void editPaste();
//  virtual void editFind();
//  virtual void helpIndex();
//  virtual void helpContents();
  void 		helpAbout();

protected:
  void*			m_root;
  int			mnuBrowseNodeDrop_param; // param from the mnuBrowseDrop slot -- called by a node, only valid for its call

  void 			Constr_Menu_impl(); // override
  void			Constr_Body_impl(); // replace to construct body
};

class TA_API iDataBrowser : public iDataBrowserBase { // viewer window used for class browsing of taBase objects
    Q_OBJECT
INHERITED(iDataBrowserBase)
friend class DataBrowser;
public:
  taiAction*	        toolsClassBrowseAction;
  
  DataBrowser*		browser() {return (DataBrowser*)m_viewer;}

  ~iDataBrowser();

public slots:
  virtual void		mnuNewBrowser(taiAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*
  virtual void		toolsClassBrowser();
  
protected:
  iDataBrowser(void* root_, MemberDef* md_, TypeDef* typ_, DataBrowser* browser_,
    QWidget* parent = 0);
  void 			Constr_Menu_impl(); // override

protected:
  MemberDef*		m_md;
  TypeDef*		m_typ;

  void 			ApplyRoot(); // #IGNORE actually applies the new root value set in m_root/m_typ
  override taiTreeDataNode* CreateTreeDataNode_impl(taiDataLink* link, MemberDef* md_,
    taiTreeDataNode* parent_, taiTreeDataNode* last_child_, 
    const String& tree_name, int flags_); // pass parent=null if this is a root item
};

//////////////////////////////////
//////////////////////////////////
//	DataBrowser		//
//////////////////////////////////

class TA_API DataBrowser : public DataViewer {
  // #NO_TOKENS the base type for objects with a top-level window or panel and a menu
friend class iDataBrowser;
public:
  //NOTE: "theoretically", browsing supports non-taBase classes, but this version only supports taBase
  static DataBrowser*	New(void* root_, MemberDef* md_, TypeDef* typ_, bool is_root = false);

  taBase*		root;
  MemberDef* 		md;

  iDataBrowser*		browser_win() {return (iDataBrowser*)m_window;}

//nn  void			TreeNodeDestroying(taiTreeDataNode* item); // #IGNORE check if curItem

  TA_BASEFUNS(DataBrowser)
protected:
  override void		Constr_Window_impl(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Clear_impl(); // #IGNORE
private:
  void			Initialize();
  void			Destroy() {}
};




#endif // TA_QTBROWSE_H

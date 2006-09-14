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
# include <QTreeWidget>
# include "itreewidget.h"
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
class tabTreeDataNode;
class tabListTreeDataNode;
class tabGroupTreeDataNode;
class iBrowseViewer;
class BrowseViewer; //


/* note used, for now (taList items placed directly below parent item, not in second level "items" folder )
class taiListItemsDataLink: public taiDataLink {
  // DataLink for taPtrList item objects -- handles any type of item content -- this is only the node handler for the 'items' node, not the node representing the list object itself
public:
  taPtrList_impl*	data() const {return (taPtrList_impl*)m_data;}
  void 			CreateChild(iTreeViewItem* par_node, iTreeViewItem* after_node, void* el);
  // used for items inserted after populating
  override void		CreateChildren(iTreeViewItem* par_node); // parent node will be the list object data link
  override bool		HasChildItems(); // true if has any items in list
  override String 	GetName() const;
  taiListItemsDataLink(taPtrList_impl* data_, iDataBrowser* browser_);
protected:
  override iDataPanel* CreateDataPanel(); // show a list panel
  override void		DataChanged_impl(iTreeViewItem* nd, int dcr, void* op1, void* op2);
}; */


class TA_API tabTreeDataNode: public taiTreeDataNode {
INHERITED(taiTreeDataNode)
public:
  taBase* 		data() {return ((tabDataLink*)m_link)->data();}
  tabDataLink* 		link() const {return (tabDataLink*)m_link;}


  tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabTreeDataNode(tabDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabTreeDataNode;}
private:
  void			init(tabDataLink* link_, int dn_flags_); // #IGNORE
};


class TA_API tabListTreeDataNode: public tabTreeDataNode {
INHERITED(tabTreeDataNode)
public:
  taList_impl* 		data() {return ((tabListDataLink*)m_link)->data();}
  tabListDataLink* 	link() const {return (tabListDataLink*)m_link;}

  void			AssertLastListItem(); // #IGNORE updates last_list_items_node -- called by Group node before dynamic inserts/updates etc.
  override void		UpdateChildNames(); // #IGNORE update child names of the indicated node

  tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabListTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabListTreeDataNode;}
protected:
  taiTreeDataNode*	last_list_items_node; // #IGNORE last list member node created, so we know where to start group items
  override void		DataChanged_impl(int dcr, void* op1, void* op2);
  override void 	CreateChildren_impl(); 
  void			CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after, void* el);
  void			UpdateListNames(); // #IGNORE updates names after inserts/deletes etc.
private:
  void			init(tabListDataLink* link_, int dn_flags_); // #IGNORE
};


class TA_API tabGroupTreeDataNode: public tabListTreeDataNode {
INHERITED(tabListTreeDataNode)
public:
  taGroup_impl* 	data() {return ((tabGroupDataLink*)m_link)->data();}
  tabGroupDataLink* 	link() const {return (tabGroupDataLink*)m_link;}

  void 			CreateSubGroup(taiTreeDataNode* after, void* el); 
    // for dynamic changes to tree
  override void		UpdateChildNames(); // #IGNORE

  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeView* parent_,
    taiTreeDataNode* after, const String& tree_name, int dn_flags_ = 0);
  ~tabGroupTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_tabGroupTreeDataNode;}
protected:
  override void 	CreateChildren_impl(); 
  override void		DataChanged_impl(int dcr, void* op1, void* op2); // handle DCR_GROUP_xxx ops
  void			UpdateGroupNames(); // #IGNORE updates names after inserts/deletes etc.
private:
  void			init(tabGroupDataLink* link_, int dn_flags_); // #IGNORE
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
       BrowseViewer starts destructing (splMain)
       *following happens in either order*
         Root DataNode starts destructing
           DataNode informs DataLink, which removes it --
             DataLink destructs if all DataNodes removed
               Destructing DataLink destructs its DataPanel, which removes it from tabbed parent
           DataNode recursively starts destructing its children, each informing DataLink as above
         Tabbed parent starts destructing, will destruct each DataPanel (if any left)
           DataPanel destructs -- if still connected to a link, it severs the link connection
*/

class TA_API iBrowseViewer : public iFrameViewer { // base of viewer window used for object and class browsing
    Q_OBJECT
INHERITED(iFrameViewer)
//friend class BrowseViewer;
public:

  iTreeView*		lvwDataTree; 

  inline BrowseViewer*	browser() {return (BrowseViewer*)m_viewer;}
  void*			root() {return (browser()) ? browser()->root() : NULL;}
  TypeDef*		root_typ() {return (browser()) ? browser()->root_typ : &TA_void;}
  MemberDef*		root_md() {return (browser()) ? browser()->root_md : NULL;}

//nn  void			DataPanelDestroying(iDataPanel* panel); // override - called by DataPanel when it is destroying -- remove from all tabs
//nn  void		RemovePanel(iDataPanel* panel); // removes and deletes the indicated panel
//nn  int			GetEditActions(); // override
  void			Reset();
  virtual void 		ApplyRoot(); // #IGNORE actually applies the new root value
  
  iBrowseViewer(BrowseViewer* browser_, QWidget* parent = 0);
  ~iBrowseViewer();

public slots:
  virtual void		mnuBrowseNodeDrop(int param) {mnuBrowseNodeDrop_param = param;} // called from within the node->dropped event

protected slots:
  virtual void		lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList& sel_items,
    taiMenu* menu);

protected:
  int			mnuBrowseNodeDrop_param; // param from the mnuBrowseDrop slot -- called by a node, only valid for its call
private:
  void			Init();
};


#endif // TA_QTBROWSE_H

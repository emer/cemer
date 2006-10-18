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

// ta_qtbrowse.cc

#include "ta_qtbrowse.h"

#include "ta_base.h"
#include "ta_group.h"
#include "ta_qtclipdata.h"
#include "ta_qtdata.h"
#include "ta_qttype.h"
#include "ta_qtviewer.h"
#include "ta_classbrowse.h"
#include "ta_qt.h"
#include "css_qt.h"
//TEMP:
#include "ta_qtdata.h"
#include "ta_qtgroup.h"

#include "icolor.h"
#include "igeometry.h"

#include <qclipboard.h>
#include <qcursor.h>
#include <qevent.h>
#include <QHeaderView>
#include <qlabel.h>
#include <qlayout.h>
#include <QMimeData>
#include <QMenu>
//#include <qpushbutton.h>
#include <qapplication.h>
//#include <qscrollview.h>
//#include <qtable.h>
#include <qtimer.h>


// SEE THE FILE ta_qtviewer.cc for a detailed explanation of class relations, and
// the creation/teardown sequences.


//////////////////////////////////
//   tabTreeDataNode 		//
//////////////////////////////////

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabTreeDataNode::init(tabDataLink* link_, int dn_flags_) {
}

tabTreeDataNode::~tabTreeDataNode()
{
}



//////////////////////////////////
//   taiListTreeDataNode 	//
//////////////////////////////////

tabListTreeDataNode::tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabDataLink*)link_, md_, parent_, last_child_, tree_name, 
  dn_flags_ | DNF_LAZY_CHILDREN)
{
  init(link_, dn_flags_);
}

tabListTreeDataNode::tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabListTreeDataNode::init(tabListDataLink* link_, int dn_flags_) {
  last_list_items_node = NULL;
}

tabListTreeDataNode::~tabListTreeDataNode()
{
}

void tabListTreeDataNode::AssertLastListItem() {
  void* el = data()->Peek_();
  if (el == NULL) {
    last_list_items_node = last_member_node;
    return;
  }
  
  last_list_items_node = this->FindChildForData(el);

}

void tabListTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  for (int i = 0; i < data()->size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = data()->GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
    }

    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen... unless null

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int dn_flags_tmp = iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_BROWSE| iTreeViewItem::DNF_CAN_DRAG;
    last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this, last_child_node, tree_nm, dn_flags_tmp);
  }

  last_list_items_node = last_child_node;
}

void tabListTreeDataNode::CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after, 
  void* el) 
{
  taPtrList_impl* list = data();
  TypeDef* typ = list->GetElType();
  if (!typ) return; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  if (!dl) return; // shouldn't happen unless null...
  //note: we don't make name because it is updated anyway
  //taiTreeDataNode* dn =
  dl->CreateTreeDataNode((MemberDef*)NULL, par_node, after, "",
    (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_BROWSE | iTreeViewItem::DNF_CAN_DRAG));
}

void tabListTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!this->children_created) return;
  switch (dcr) {
  case DCR_LIST_INIT: break;
  case DCR_LIST_ITEM_INSERT: {	// op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_); //null if not found
    if (!after_node) after_node = last_member_node; // insert, after
    CreateListItem(this, after_node, op1_);
  }
    break;
  case DCR_LIST_ITEM_REMOVE: {	// op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_); //null if not found
    if (gone_node) delete gone_node; // goodbye!
  }
    break;
  case DCR_LIST_ITEM_MOVED: {	// op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_member_node); // insert, after
    ++to_idx; // after
    moveChild(fm_idx, to_idx);
  }
    break;
  case DCR_LIST_ITEMS_SWAP: {	// op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeDataNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeDataNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    swapChildren(n1_idx, n2_idx); 
  }
    break;
  case DCR_LIST_SORTED: {	// no ops
    //TODO: we will probably need to delete all the children and start again,
    // OR, maybe we can just reshuffle ourselves!
    int nd_idx; // index of the node
    taList_impl* list = data(); // cache
    for (int i = 0; i < list->size; ++i) {
      TAPtr tab = (TAPtr)list->FastEl_(i);
      FindChildForData(tab, nd_idx);
      if (i == nd_idx) continue; // in right place already
      moveChild(nd_idx, i);
    }
  }
    break;
  default: return; // don't update names
  }
  UpdateListNames();
}

void tabListTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateListNames();
}

void tabListTreeDataNode::UpdateListNames() {
  String tree_nm;
  for (int i = 0; i < data()->size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = data()->GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    taiTreeDataNode* node1 = this->FindChildForData(el); //null if not found
    if (node1 != NULL)
      node1->setText(0, tree_nm);
  }
}


//////////////////////////////////
//   taiGroupTreeDataNode 	//
//////////////////////////////////

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeView* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((tabListDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void tabGroupTreeDataNode::init(tabGroupDataLink* link_, int dn_flags_) {
}

tabGroupTreeDataNode::~tabGroupTreeDataNode()
{
}

void tabGroupTreeDataNode::CreateChildren_impl() {
  inherited::CreateChildren_impl();
  String tree_nm;
  for (int i = 0; i < data()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = data()->gp.GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    last_child_node = dl->CreateTreeDataNode(NULL, this, last_child_node, tree_nm,
      (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG));
    //TODO: maybe this isn't right -- we really want the root group's md, because that is the only
          // one that has the defs
  }
}

void tabGroupTreeDataNode::CreateSubGroup(taiTreeDataNode* after_node, void* el) {
  taSubGroup* gp = &data()->gp;
  TypeDef* typ = gp->GetElType();
  // the subgroups are themselves taGroup items
  if (!typ) return; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
  if (!dl) return; // shouldn't happen unless null...

//  taiTreeDataNode* dn =
  dl->CreateTreeDataNode(NULL, this, after_node, "",
    (iTreeViewItem::DNF_UPDATE_NAME | iTreeViewItem::DNF_CAN_DRAG)); //gets its name in rename
}

void tabGroupTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (!this->children_created) return;
  AssertLastListItem();
  switch (dcr) {
  case DCR_GROUP_INSERT: {	// op1=item, op2=item_after, null=at beginning
    taiTreeDataNode* after_node = this->FindChildForData(op2_); //null if not found
    if (after_node == NULL) after_node = last_list_items_node; // insert, after lists
    CreateSubGroup(after_node, op1_);
  }
    break;
  case DCR_GROUP_REMOVE: {	// op1=item -- note, item not DisOwned yet, but has been removed from list
    taiTreeDataNode* gone_node = this->FindChildForData(op1_); //null if not found
    if (gone_node) delete gone_node; // goodbye!
  }
    break;
  case DCR_GROUP_MOVED: {	// op1=item, op2=item_after, null=at beginning
    int fm_idx;
    taiTreeDataNode* moved_node = this->FindChildForData(op1_, fm_idx); //null if not found
    if (!moved_node) break; // shouldn't happen
    int to_idx;
    taiTreeDataNode* after_node = this->FindChildForData(op2_, to_idx); //null if not found
    if (!after_node) to_idx = indexOfChild(last_list_items_node); // insert, after
    ++to_idx; // after
    moveChild(fm_idx, to_idx);
  }
    break;
  case DCR_GROUPS_SWAP: {	// op1=item1, op2=item2
    int n1_idx, n2_idx;
    taiTreeDataNode* node1 = this->FindChildForData(op1_, n1_idx); //null if not found
    taiTreeDataNode* node2 = this->FindChildForData(op2_, n2_idx); //null if not found
    if ((!node1) || (!node2)) break; // shouldn't happen
    swapChildren(n1_idx, n2_idx); 
  }
    break;
  default: return; // don't update names
  }
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateChildNames() {
  inherited::UpdateChildNames();
  UpdateGroupNames();
}

void tabGroupTreeDataNode::UpdateGroupNames() {
  String tree_nm;
  for (int i = 0; i < data()->gp.size; ++i) {
    // the subgroups are themselves taGroup items
    TypeDef* typ;
    void* el = data()->gp.GetTA_Element(i, typ); // gets the item, and its TypeDef
    if (!typ) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = taiViewType::StatGetDataLink(el, typ);
    if (!dl) continue; // shouldn't happen unless null...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    iTreeViewItem* node1 = this->FindChildForData(el); //null if not found
    if (node1 != NULL)
      node1->setText(0, tree_nm);
  }
}


/*void iBrowseViewer::RemovePanel(iDataPanel* panel) {
  // remove from tabs, deleting tabs (except last)
  for (int i = tbPanels->count() - 1; i >= 0; --i) {
    iPanelTab* pt = (iPanelTab*)tbPanels->tabAt(i);
    if (pt->panel == panel) {
      if (i > 0) {
        tbPanels->removeTab(pt);
      } else {
        pt->SetPanel((iDataPanel*)wsPanels->widget(0)); // set the blank default panel
      }
    }
  }
  delete panel;
} */


//////////////////////////
//   iBrowseViewer 	//
//////////////////////////

iBrowseViewer::iBrowseViewer(BrowseViewer* browser_, QWidget* parent)
:inherited(browser_, parent)
{
  Init();
}

iBrowseViewer::~iBrowseViewer()
{
}

void iBrowseViewer::Init() {
  mnuBrowseNodeDrop_param = -1;
  QVBoxLayout* lay = new QVBoxLayout(this);
  lay->setMargin(0);  lay->setSpacing(0);
  lvwDataTree = new iTreeView(this);
  lay->addWidget(lvwDataTree);
  lvwDataTree->setName("lvwDataTree");
  lvwDataTree->setSortingEnabled(false); // preserve enumeration order of items
  lvwDataTree->setColumnCount(1);
  lvwDataTree->header()->hide();
  //enable dnd support
  lvwDataTree->setDragEnabled(true);
  lvwDataTree->setAcceptDrops(true);
  lvwDataTree->setDropIndicatorShown(true);
  lvwDataTree->setHighlightRows(true); // show error objects
/*no! prevents from collapsing in splitters  int mw = (taiM->scrn_s.width() * 3) / 20; // 15% min for tree
  lvwDataTree->resize(mw, lvwDataTree->height()); // 15% min for tree
  lvwDataTree->setMinimumWidth(mw); // 15% min for tree
*/
  connect(lvwDataTree, SIGNAL(FillContextMenuHookPost(ISelectable_PtrList&, taiMenu*)),
      this, SLOT(lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList&, taiMenu*)) );
  lvwDataTree->Connect_SelectableHostNotifySignal(this, 
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

/*TODO: sort this out iTabView* iBrowseViewer::AddTabView(QWidget* parCtrl, iTabView* splitBuddy) {
  iTabView* rval = inherited::AddTabView(parCtrl, splitBuddy);
  // create a tab in the new tabview, based on cur item
  iDataPanel* pn;
  ISelectable* ci = curItem();
  if (!ci) goto exit;
  pn = NULL;
  pn = rval->GetDataPanel(ci->link());
  rval->SetPanel(pn);

exit:
  return rval;
} */

void iBrowseViewer::ApplyRoot() {
  void* m_root = root(); //cache
  if (!m_root) return;
  taiDataLink* dl = taiViewType::StatGetDataLink(m_root, root_typ());
  if (!dl) return; // shouldn't happen...

  // by definition, we should always be able to create a new browser on root of a browser
  int dn_flags_ = iTreeViewItem::DNF_CAN_BROWSE;
  
  // we treat root slightly different if it is true root, or is just a subsidiary named item
  taiTreeDataNode* node;
  //TODO: should add memberdef to constructor
  if (m_root == tabMisc::root)
    node = dl->CreateTreeDataNode(root_md(), lvwDataTree, NULL, "root",
      dn_flags_ | iTreeViewItem::DNF_IS_MEMBER);
  else //TODO: should really have a better scheme for root name -- what if it is unnamed???
    node = dl->CreateTreeDataNode(root_md(), lvwDataTree, NULL, dl->GetName(), 
      dn_flags_ | iTreeViewItem::DNF_UPDATE_NAME);
  // always show the first items under the root
  node->CreateChildren();
  lvwDataTree->setCurItem(node);
  lvwDataTree->setItemExpanded(node, true); // always open root node
}

/*was not an override! void iBrowseViewer::DataPanelDestroying(iDataPanel* panel) {
  // remove from tabs, deleting tabs (except always leave at least one tab, except when we are destroying)
  //note: not called by the blank panel, since it has no link
  tabView()->DataPanelDestroying(panel);
} */

void iBrowseViewer::lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList& /*sel_items*/,
   taiMenu* menu) 
{
//TODO:  FillContextMenu(menu);
}

void iBrowseViewer::Reset() {
  lvwDataTree->clear();
}


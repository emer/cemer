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


//////////////////////////
//    iListView 	//
//////////////////////////

class iListView: public iTreeWidget {
INHERITED(iTreeWidget)
public:
  iDataBrowserBase* browser_win;
  iListView(iDataBrowserBase* browser_win_, QWidget*  parent = NULL);

protected:

//  iListViewItem*	focus_item;

  void 			focusInEvent(QFocusEvent* ev); // override
//  void 			focusOutEvent(QFocusEvent* ev); // override

/*  void		doDragFocusEvent(QDropEvent* ev); // code is the same for all enter/move/leave events for focus
  void		contentsDragEnterEvent(QDragEnterEvent* ev); // override
  void 		contentsDragMoveEvent(QDragMoveEvent * ev); // override
  void 		contentsDragLeaveEvent(QDragLeaveEvent * ev); // override
  void		setDropFocus(iListViewItem* item); // draws a focus rec around the target (qt doesn't do this by default) */
};


iListView::iListView(iDataBrowserBase* browser_win_, QWidget* parent)
: inherited(parent)
{
  browser_win = browser_win_;
//  focus_item = NULL;
  setAcceptDrops(true);
}

void iListView::focusInEvent(QFocusEvent* ev) {
  browser_win->lvwDataTree_focusInEvent(ev);
}
/*NOTE: causes enabling to shut off for any out event, even clicking on the main menu!!!
void iListView::focusOutEvent(QFocusEvent* ev) {
  browser->emit_SetEditActionsEnabled(0);
} */

/* TODO (maybe): drag item feedback (ex. focus rect)
void iListView::contentsDragEnterEvent(QDragEnterEvent* ev) {
  doDragFocusEvent(ev);
}

void iListView::contentsDragMoveEvent(QDragMoveEvent * ev) {
  doDragFocusEvent(ev);
}
void iListView::contentsDragLeaveEvent(QDragLeaveEvent * ev) {
  doDragFocusEvent(ev);
}

void iListView::doDragFocusEvent(QDropEvent* ev) {
  QPoint pos = ev->pos();
  iListViewItem* item = (iListViewItem*)itemAt(pos);
  setFocusDrop(item);
}

void iListView::setDropFocus(iListViewItem* item) {
  if (item == focus_item) return;
  // restore old item
  if (focus_item) {
    redrawItem(focus_item);
  }
  item = focus_item;
  if (item) {

  }

} */



//////////////////////////////////
// 	taiTreeDataNode 	//
//////////////////////////////////

int taiTreeDataNode::no_idx; // dummy parameter

taiTreeDataNode::taiTreeDataNode(taiDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

taiTreeDataNode::taiTreeDataNode(taiDataLink* link_, MemberDef* md_, iTreeWidget* parent_,
  taiTreeDataNode* last_child_, const String& tree_name, int dn_flags_)
:inherited(link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

void taiTreeDataNode::init(taiDataLink* link_, int dn_flags_) {
  last_child_node = NULL;
  last_member_node = NULL;
}


taiTreeDataNode::~taiTreeDataNode() {
  iDataBrowserBase* db = browser_win();
  if (db)
    db->TreeNodeDestroying(this);
}

DataViewer* taiTreeDataNode::browser() const {
  iListView* lv = (iListView*)treeWidget();
  iDataBrowserBase* bw = (lv) ? lv->browser_win : NULL;
  return (bw) ? bw->viewer() : NULL;
}

iDataBrowserBase* taiTreeDataNode::browser_win() const {
  iListView* lv = (iListView*)treeWidget();
  return (lv) ? lv->browser_win : NULL;
}

void taiTreeDataNode::CreateChildren_impl() {
  MemberSpace* ms = &(link()->GetDataTypeDef()->members);
  for (int i = 0; i < ms->size; ++ i) {
    MemberDef* md = ms->FastEl(i);
    if (!link()->ShowMember(md)) continue;
    TypeDef* typ = md->type;
    void* el = md->GetOff(data()); //note: GetDataLink automatically derefs typ and el if pointers
    taiDataLink* dl = browser()->GetDataLink(el, typ);
    String tree_nm = md->GetLabel();
    last_child_node = browser_win()->CreateTreeDataNode(dl, md, this, last_child_node, tree_nm, (iListViewItem::DNF_IS_MEMBER));
  }
  last_member_node = last_child_node; //note: will be NULL if no members issued
}

void taiTreeDataNode::FillContextMenu_impl(taiActions* menu) {
  if (dn_flags & DNF_CAN_BROWSE) {
     //taiAction* mel =
     menu->AddItem("New Browser from here", taiMenu::use_default,
       taiAction::men_act, browser_win(), SLOT(mnuNewBrowser(taiAction*)), this);
  }
  inherited::FillContextMenu_impl(menu);
}

taiTreeDataNode* taiTreeDataNode::FindChildForData(void* data, int& idx) {
  for (int i = 0; i < childCount(); ++i) {
      taiTreeDataNode* rval = (taiTreeDataNode*)child(i);
      if (rval->link()->data() == data) {
        idx = i;
        return rval;
      }
  }
  idx = -1;
  return NULL;
}

IDataViewHost*  taiTreeDataNode::host() const {
  iListView* lv = (iListView*)treeWidget();
  return (lv) ? lv->browser_win : NULL;
}

taiDataLink* taiTreeDataNode::par_link() const {
  taiTreeDataNode* par = parent();
  return (par) ? par->link() : NULL;
}

MemberDef* taiTreeDataNode::par_md() const {
  taiTreeDataNode* par = parent();
  return (par) ? par->md() : NULL;
}


//////////////////////////////////
//   tabTreeDataNode 		//
//////////////////////////////////

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, taiTreeDataNode* parent_,
  taiTreeDataNode* last_child_,  const String& tree_name, int dn_flags_)
:inherited((taiDataLink*)link_, md_, parent_, last_child_, tree_name, dn_flags_)
{
  init(link_, dn_flags_);
}

tabTreeDataNode::tabTreeDataNode(tabDataLink* link_, MemberDef* md_, iTreeWidget* parent_,
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

void tabTreeDataNode::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  inherited::DataChanged_impl(dcr, op1_, op2_);
  if (dcr != DCR_ITEM_UPDATED) return;
  if (this->dn_flags & iListViewItem::DNF_UPDATE_NAME) {
    taiTreeDataNode* par_nd = (taiTreeDataNode*)this->parent();
    if (par_nd == NULL) {// null if already a root node -- just force our name to something sensible...
      String nm = link()->GetName();
      if (nm.empty())
        nm = "(" + link()->GetDataTypeDef()->name + ")";
      this->setText(0, nm);
      return;
    }
    taiDataLink* dl = par_nd->link();
    if (dl == NULL) return; // shouldn't happen...
    par_nd->UpdateChildNames();
  }
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

tabListTreeDataNode::tabListTreeDataNode(tabListDataLink* link_, MemberDef* md_, iTreeWidget* parent_,
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
    if (typ == NULL) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
    }

    taiDataLink* dl = browser()->GetDataLink(el, typ);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int dn_flags_tmp = iListViewItem::DNF_UPDATE_NAME | iListViewItem::DNF_CAN_BROWSE| iListViewItem::DNF_CAN_DRAG;
    last_child_node = browser_win()->CreateTreeDataNode(dl, (MemberDef*)NULL, this, last_child_node, tree_nm, dn_flags_tmp);
  }

  last_list_items_node = last_child_node;
}

void tabListTreeDataNode::CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after, 
  void* el) 
{
  taPtrList_impl* list = data();
  TypeDef* typ = list->GetElType();
  if (typ == NULL) return; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = browser()->GetDataLink(el, typ);
  if (dl == NULL) return; // shouldn't happen...
  //note: we don't make name because it is updated anyway
  //taiTreeDataNode* dn =
  browser_win()->CreateTreeDataNode(dl, (MemberDef*)NULL, par_node, after, "",
    (iListViewItem::DNF_UPDATE_NAME | iListViewItem::DNF_CAN_BROWSE | iListViewItem::DNF_CAN_DRAG));
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
  }
    break;
  default: return; // don't update names
  }
  UpdateListNames();
  browser_win()->UpdateTabNames();
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
    if (typ == NULL) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = browser()->GetDataLink(el, typ);
    if (dl == NULL) continue; // shouldn't happen...

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

tabGroupTreeDataNode::tabGroupTreeDataNode(tabGroupDataLink* link_, MemberDef* md_, iTreeWidget* parent_,
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
    if (typ == NULL) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = browser()->GetDataLink(el, typ);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    last_child_node = browser_win()->CreateTreeDataNode(dl, NULL, this, last_child_node, tree_nm,
      (iListViewItem::DNF_UPDATE_NAME | iListViewItem::DNF_CAN_DRAG));
    //TODO: maybe this isn't right -- we really want the root group's md, because that is the only
          // one that has the defs
  }
}

void tabGroupTreeDataNode::CreateSubGroup(taiTreeDataNode* after_node, void* el) {
  taSubGroup* gp = &data()->gp;
  TypeDef* typ = gp->GetElType();
  // the subgroups are themselves taGroup items
  if (typ == NULL) return; //TODO: maybe we should put a marker item in list???
  // if we get a taBase item, the type might only be the base type, not the derived type of the item
  // so we cast the item, and then grab the exact type right from the item
  if (typ->InheritsFrom(&TA_taBase)) {
      typ = ((taBase*)el)->GetTypeDef();
  }
  taiDataLink* dl = browser()->GetDataLink(el, typ);
  if (dl == NULL) return; // shouldn't happen...

//  taiTreeDataNode* dn =
  browser_win()->CreateTreeDataNode(dl, NULL, this, after_node, "",
    (iListViewItem::DNF_UPDATE_NAME | iListViewItem::DNF_CAN_DRAG)); //gets its name in rename
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
  browser_win()->UpdateTabNames();
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
    if (typ == NULL) continue; //TODO: maybe we should put a marker item in list???
    // if we get a taBase item, the type might only be the base type, not the derived type of the item
    // so we cast the item, and then grab the exact type right from the item
    if (typ->InheritsFrom(&TA_taBase)) {
        typ = ((taBase*)el)->GetTypeDef();
    }
    taiDataLink* dl = browser()->GetDataLink(el, typ);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
      tree_nm = "(subgroup " + String(i) + ")";
    } else {
      tree_nm = tree_nm + " subgroup";
    }
    iListViewItem* node1 = this->FindChildForData(el); //null if not found
    if (node1 != NULL)
      node1->setText(0, tree_nm);
  }
}


/*void iDataBrowser::RemovePanel(iDataPanel* panel) {
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
//   iDataBrowserBase 	//
//////////////////////////

iDataBrowserBase::iDataBrowserBase(void* root_, DataViewer* browser_,
  QWidget* parent)
: iTabDataViewer(browser_, parent)
{
  splMain = NULL;
  lvwDataTree = NULL;
  m_root = root_;
  mnuBrowseNodeDrop_param = -1;

/*TEMP  setCentralWidget( new QWidget( this, "qt_central_widget" ) );
  layOuter = new QVBoxLayout( centralWidget(), taiM->hspc_c, -1, "layOuter");
  browser = new taiDataBrowser(centralWidget(), "browser");
  layOuter->addWidget(browser); */


//  resize( QSize(745, 538).expandedTo(minimumSizeHint()) );
//TODO?? Qt3  clearWState( WState_Polished );

  taiMisc::viewer_wins.Add(this);
}

iDataBrowserBase::~iDataBrowserBase()
{
  taiMisc::viewer_wins.Remove(this);
    // no need to delete child widgets, Qt does it all for us
  toolbars.clear(); //note: widget deletion takes care of deleting the actual toolbars
  actions.Reset(); // ditto
}

iTabView* iDataBrowserBase::AddTabView(QWidget* parCtrl, iTabView* splitBuddy) {
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
}

void iDataBrowserBase::Constr_Menu_impl() {
  inherited::Constr_Menu_impl();
}

void iDataBrowserBase::Constr_Body_impl() {
  splMain = new QSplitter(this);
  splMain->setName("splMain");
  lvwDataTree = new iListView(this, splMain);
  lvwDataTree->setName("lvwDataTree");
  lvwDataTree->setSortingEnabled(false); // preserve enumeration order of items
  lvwDataTree->setColumnCount(1);
  lvwDataTree->header()->hide();
  lvwDataTree->setDragEnabled(true);
  lvwDataTree->setAcceptDrops(true);
  lvwDataTree->setDropIndicatorShown(true);
  int mw = (taiM->scrn_s.width() * 3) / 20; // 15% min for tree
  lvwDataTree->resize(mw, lvwDataTree->height()); // 15% min for tree
  lvwDataTree->setMinimumWidth(mw); // 15% min for tree

  m_curTabView = AddTabView(splMain);

  splMain->setResizeMode(lvwDataTree, QSplitter::KeepSize); // when user enlarges, it is the data pane that will resize

  connect(lvwDataTree, SIGNAL(contextMenuRequested(QTreeWidgetItem*, const QPoint &, int)),
      this, SLOT(lvwDataTree_contextMenuRequested(QTreeWidgetItem*, const QPoint &, int)) );
  connect(lvwDataTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
      this, SLOT(lvwDataTree_currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)) );

  m_body = splMain;
  splMain->show();
  setCentralWidget(splMain);
}

taiTreeDataNode* iDataBrowserBase::CreateTreeDataNode(taiDataLink* link, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_, const String& tree_name, int dn_flags_)
{
  if (link->HasChildItems()) {
    //note: the list item automatically enables lazy children
    dn_flags_ |= iListViewItem::DNF_LAZY_CHILDREN;
  }
  taiTreeDataNode* rval = CreateTreeDataNode_impl(link, md_,parent_, last_child_, 
      tree_name, dn_flags_);
  if (rval) {
    rval->DecorateDataNode();
  }
  return rval;
}

void iDataBrowserBase::DataPanelDestroying(iDataPanel* panel) {
  // remove from tabs, deleting tabs (except always leave at least one tab, except when we are destroying)
  //note: not called by the blank panel, since it has no link
  tabView()->DataPanelDestroying(panel);
/*note: there is no need to change the tree item in response to panel changes
   // may also need to change the current item
  taiTreeDataNode* next_cur = m_curItem; // do nothing by default
  if ((m_curItem != NULL) && (m_curItem->link()->panel() == panel)) {
    next_cur = m_curItem->parent(); // shouldn't be null, but would be for root...
  }
  // remove from tabs, deleting tabs (except always leave at least one tab, except when we are destroying)
  //note: not called by the blank panel, since it has no link
  tabView()->DataPanelDestroying(panel);
  if (next_cur != m_curItem) {
    m_curItem = NULL; // don't let setCurItem try to unset the panel, since it is deleted
    setCurItem(next_cur, true); // force, in case null
  }
*/
}
/* no: we only use these on tree items themselves -- too confusing for user
void iDataBrowserBase::fileNew() {
  ISelectable* ci = curItem();
  if (ci) ci->link()->fileNew();
}

void iDataBrowserBase::fileOpen() {
  ISelectable* ci = curItem();
  if (ci) ci->link()->fileOpen();
}

void iDataBrowserBase::fileSave() {
  ISelectable* ci = curItem();
  if (ci) ci->link()->fileSave();
}

void iDataBrowserBase::fileSaveAs() {
  ISelectable* ci = curItem();
  if (ci) ci->link()->fileSaveAs();
}

void iDataBrowserBase::fileClose() {
  ISelectable* ci = curItem();
  if (ci) ci->link()->fileClose();
}
*/
taiClipData* iDataBrowserBase::GetClipData(int src_edit_action, bool for_drag) {
  ISelectable* ci = curItem();
  if (ci) return ci->GetClipData(sel_items(), src_edit_action, for_drag);
  else    return NULL; //note: should happen, because we should have checked for data first
}
/*in qtviewer
int iDataBrowserBase::GetEditActions() { // after a change in selection, update the available edit actions (cut, copy, etc.)
  // we query the item(s) for both what they will allow, as well as what they cannot allow
  // for single items, the available actions are simply what the item will allow
  // for multi select, the available actions are the AND of what every item will allow and what no item will not allow
  // so we start with Allowed=1 and Forbidden=0; each item AND's its own Allowed, and OR's its own Forbidden
  int allowed = 0;
  int forbidden = 0;
  ISelectable* ci = curItem();
  if (ci) {
    taiMimeSource* ms = taiMimeSource::New(QApplication::clipboard()->data(QClipboard::Clipboard));
    ci->GetEditActions(ms, allowed, forbidden);
    delete ms;
  }
  return allowed & (~forbidden);
} */

void iDataBrowserBase::helpAbout() {
  if (tabMisc::root) tabMisc::root->Info();
}

void iDataBrowserBase::lvwDataTree_contextMenuRequested(QTreeWidgetItem* item, const QPoint & pos, int col ) {
  iListViewItem* nd = (iListViewItem*)item;
  if (nd == NULL) return; //TODO: could possibly be multi select

  taiMenu* menu = new taiMenu(this, taiMenu::normal, taiMisc::fonSmall);
  //TODO: any for us first (ex. delete)

  nd->FillContextMenu(sel_items(), menu); // also calls link menu filler

  FillContextMenu(menu);

  if (menu->count() > 0) { //only show if any items!
    menu->exec(pos);
  }
  delete menu;
}

void iDataBrowserBase::lvwDataTree_focusInEvent(QFocusEvent* ev) {
  SetThisAsHandler();
}

void iDataBrowserBase::lvwDataTree_currentItemChanged(QTreeWidgetItem* curr, 
    QTreeWidgetItem* /*prev*/) {
  setCurItem((taiTreeDataNode*)curr);
}

void iDataBrowserBase::Reset() {
  lvwDataTree->clear();
}

void iDataBrowserBase::SelectionChanged(bool forced) {
  // if setCurItem was invoked in code, need to set gui selection;
  // won't cause infinite recursion, because m_curItem is now set
  if (forced) {
    ISelectable* ci = curItem();
    if (ci)
      lvwDataTree->setCurrentItem((taiTreeDataNode*)ci->This());
  }
  inherited::SelectionChanged(forced);
}

void iDataBrowserBase::TreeNodeDestroying(taiTreeDataNode* item) {
  RemoveSelectedItem(item); // noop if not selected
}

//////////////////////////
//   iDataBrowser 	//
//////////////////////////

iDataBrowser::iDataBrowser(void* root_, MemberDef* md_, TypeDef* typ_, DataBrowser* browser_,
  QWidget* parent)
: iDataBrowserBase(root_, (DataViewer*)browser_, parent)
{
  m_md = md_;
  m_typ = typ_;
}

iDataBrowser::~iDataBrowser()
{
}

void iDataBrowser::ApplyRoot() {
  if (!m_root) return;
  taiDataLink* dl = browser()->GetDataLink(m_root, m_typ);
  if (dl == NULL) return; // shouldn't happen...

  // by definition, we should always be able to create a new browser on root of a browser
  int dn_flags_ = iListViewItem::DNF_CAN_BROWSE;
  
  // we treat root slightly different if it is true root, or is just a subsidiary named item
  taiTreeDataNode* node;
  //TODO: should add memberdef to constructor
  if (m_root == tabMisc::root)
    node = CreateTreeDataNode(dl, (MemberDef*)NULL, NULL, NULL, "root",
      dn_flags_ | iListViewItem::DNF_IS_MEMBER);
  else //TODO: should really have a better scheme for root name -- what if it is unnamed???
    node = CreateTreeDataNode(dl, (MemberDef*)NULL, NULL, NULL, dl->GetName(), 
      dn_flags_ | iListViewItem::DNF_UPDATE_NAME);
  // always show the first items under the root
  node->CreateChildren();
  setCurItem(node);
  lvwDataTree->setItemExpanded(node, true); // always open root node
}

void iDataBrowser::Constr_Menu_impl() {
  inherited::Constr_Menu_impl();
  toolsClassBrowseAction = AddAction(new taiAction(0, "Class Browser", QKeySequence(), "toolsClassBrowseAction"));
  toolsClassBrowseAction->AddTo(toolsMenu );
  connect( toolsClassBrowseAction, SIGNAL( activated() ), 
    this, SLOT( toolsClassBrowser() ) );
}

void iDataBrowser::toolsClassBrowser() {
  ClassBrowser* brows = ClassBrowser::New(&taMisc::types, TIK_TYPESPACE);
  if (brows == NULL) return;
  brows->ViewWindow();
}


taiTreeDataNode* iDataBrowser::CreateTreeDataNode_impl(taiDataLink* link, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_, const String& tree_name, int dn_flags_)
{
  taiTreeDataNode* rval = NULL;
  TypeDef* typ = link->GetTypeDef();
  // slightly different constructor depending on if root item or not
  if (parent_ == NULL) {
    if (typ->InheritsFrom(&TA_tabGroupDataLink))
      rval = new tabGroupTreeDataNode((tabGroupDataLink*)link, md_, lvwDataTree, last_child_,
        tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_tabListDataLink))
      rval = new tabListTreeDataNode((tabListDataLink*)link, md_, lvwDataTree, last_child_,
        tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_tabDataLink))
      rval = new tabTreeDataNode((tabDataLink*)link, md_, lvwDataTree, last_child_, tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_taiDataLink))
      rval = new taiTreeDataNode(link, md_, lvwDataTree, last_child_, tree_name, dn_flags_);
   // else not supported yet
  } else {
    if (typ->InheritsFrom(&TA_tabGroupDataLink))
      rval = new tabGroupTreeDataNode((tabGroupDataLink*)link, md_, parent_, last_child_,
        tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_tabListDataLink))
      rval = new tabListTreeDataNode((tabListDataLink*)link, md_, parent_, last_child_, tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_tabDataLink))
      rval = new tabTreeDataNode((tabDataLink*)link, md_, parent_, last_child_, tree_name, dn_flags_);
    else if (typ->InheritsFrom(&TA_taiDataLink))
      rval = new taiTreeDataNode(link, md_, parent_, last_child_, tree_name, dn_flags_);
   // else not supported yet
  }
  if (!rval) {
    rval = inherited::CreateTreeDataNode_impl(link, md_,parent_, last_child_, 
      tree_name, dn_flags_);
  }
  return rval;
}

void iDataBrowser::mnuNewBrowser(taiAction* mel) {
  taiTreeDataNode* node = (taiTreeDataNode*)(mel->usr_data.toPtr());
  taiDataLink* dl = node->link();
  TypeDef* td = dl->GetDataTypeDef();
  if (!td->InheritsFrom(&TA_taBase)) return;
  DataBrowser* brows = DataBrowser::New((taBase*)dl->data(), node->md());
  if (!brows) return;
  brows->ViewWindow();
  // move selection up to parent of cloned item, to reduce issues of changed data, confusion, etc.
  setCurItem(node->parent(), true);

  //TODO: (maybe!) delete the panel (to reduce excessive panel pollution)
}


//////////////////////////////////
// 	DataBrowser	 	//
//////////////////////////////////

DataBrowser* DataBrowser::New(TAPtr root, MemberDef* md, bool is_root) {
  if (!root) //sanity, but shouldn't happen
    return NULL;
  DataBrowser* rval = new DataBrowser();
  rval->Constr(root, md, is_root);
  return rval;
}


void DataBrowser::Initialize() {
  md = NULL;
  del_root_on_close = false; 
  link_type = &TA_taiDataLink;
}

void DataBrowser::InitLinks() {
  inherited::InitLinks();
  taBase::Own(root, this);
}

void DataBrowser::CutLinks() {
  inherited::CutLinks();
}

void DataBrowser::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  // if root has vanished, window must die
  if (!root && m_window) {
    CloseWindow();
  }
}

void DataBrowser::Constr(TAPtr root_, MemberDef* md_, bool is_root_) {
  m_is_root = is_root_;
  root = root_;
  md = md_;
}

void DataBrowser::Constr_Window_impl() {
  if (!root) return;
  m_window = new iDataBrowser(root, md, root->GetTypeDef(), this);
}

void DataBrowser::Clear_impl() {
  if (!m_window) return;
  browser_win()->Reset();
}

void DataBrowser::Render_impl() {
  if (!m_window) return;
  browser_win()->ApplyRoot();
}

/* nn void DataBrowser::TreeNodeDestroying(taiTreeDataNode* item) {
  if (!m_window) return;
  browser_win()->TreeNodeDestroying(item);
} */

void DataBrowser::WindowClosing(bool& cancel) {
  if (del_root_on_close && root) {
    int chs = taMisc::Choice("Closing this window will also close the object being viewed?", "Close", "&Cancel");
    switch (chs) {
    case 0:
      break; // proceed to inherited
    case 1:
      cancel = true;
      return;
    }
  }
  inherited::WindowClosing(cancel);
  // now, if *still* closing, and del, then do the del!
  if (cancel || !del_root_on_close) return;
  
  if (root)
    root->Close();
  
}

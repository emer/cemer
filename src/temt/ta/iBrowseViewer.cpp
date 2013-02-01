// Copyright, 1995-2013, Regents of the University of Colorado,
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

#include "iBrowseViewer.h"
#include <BrowseViewer>
#include <iTreeView>
#include <iTreeViewItem>
#include <iTreeSearch>
#include <MainWindowViewer>
#include <iMainWindowViewer>
#include <taiViewType>
#include <taiTreeDataNode>
#include <taiSigLink>

#include <tabMisc>
#include <taRootBase>


#include <QVBoxLayout>
#include <QHeaderView>



iBrowseViewer::iBrowseViewer(BrowseViewer* browser_, QWidget* parent)
  : inherited(browser_, parent)
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
  lvwDataTree = new iTreeView(this, iTreeView::TV_AUTO_EXPAND);
  lay->addWidget(lvwDataTree);

  lvwTreeSearch = new iTreeSearch(lvwDataTree, this);
  lay->addWidget(lvwTreeSearch);

  lvwDataTree->installEventFilter(mainWindowViewer()->widget()); // translate keys..
  lvwDataTree->main_window = mainWindowViewer()->widget();
  lvwDataTree->setObjectName("lvwDataTree");
  lvwDataTree->setSortingEnabled(false); // preserve enumeration order of items
  lvwDataTree->setSelectionMode(QAbstractItemView::ExtendedSelection); // multiselect
  lvwDataTree->setDefaultExpandLevels(8); // set fairly deep for ExpandAll
  lvwDataTree->setColumnCount(1);
  lvwDataTree->AddColDataKey(0, "", Qt::StatusTipRole); // show status tip
  lvwDataTree->AddColDataKey(0, "", Qt::ToolTipRole); // and tool tip
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
  connect(lvwDataTree, SIGNAL(FillContextMenuHookPost(ISelectable_PtrList&, taiActions*)),
      this, SLOT(lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList&, taiActions*)) );
  lvwDataTree->Connect_SelectableHostNotifySignal(this,
    SLOT(SelectableHostNotifySlot_Internal(ISelectableHost*, int)) );
}

void iBrowseViewer::ApplyRoot() {
  void* m_root = root(); //cache
  if (!m_root) return;
  taiSigLink* dl = taiViewType::StatGetSigLink(m_root, root_typ());
  if (!dl) return; // shouldn't happen...

  // by definition, we should always be able to create a new browser on root of a browser
  int dn_flags_ = iTreeViewItem::DNF_CAN_BROWSE;

  // we treat root slightly different if it is true root, or is just a subsidiary named item
  // also, we assume this guy is visible, so we don't apply the filter
  taiTreeDataNode* node;
  if (m_root == tabMisc::root)
    node = dl->CreateTreeDataNode(root_md(), lvwDataTree, NULL, "root",
      dn_flags_ | iTreeViewItem::DNF_IS_MEMBER | iTreeViewItem::DNF_NO_UPDATE_NAME);
  else {
    // if root is a member, we use that name, else the obj name
    MemberDef* md = root_md();
    String root_nm;
    if (md)
      root_nm = md->name;
    else {
      root_nm = dl->GetName();
      dn_flags_ |= iTreeViewItem::DNF_UPDATE_NAME; // change it on data changes
    }
    node = dl->CreateTreeDataNode(md, lvwDataTree, NULL, root_nm,
      dn_flags_);
  }
  // always show the first items under the root
  node->CreateChildren();
  lvwDataTree->setCurrentItem(node);//setCurItem(node);
  lvwDataTree->setItemExpanded(node, true); // always open root node
}

void iBrowseViewer::lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList& /*sel_items*/,
   taiActions* menu)
{
//note: nothing
}

void iBrowseViewer::Refresh_impl() {
  lvwDataTree->Refresh();
  inherited::Refresh_impl();
}

void iBrowseViewer::Reset() {
  lvwDataTree->clear();
}


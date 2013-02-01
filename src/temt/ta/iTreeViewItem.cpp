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

#include "iTreeViewItem.h"
#include <taiSigLink>
#include <ViewColor_List>
#include <iColor>
#include <taiClipData>
#include <taiMenu>

#include <DataChangedReason>
#include <taMisc>



class DataNodeDeleter: public QObject { // enables nodes to be put on deferredDelete list
public:
  iTreeViewItem* node;
  DataNodeDeleter(iTreeViewItem* node_): QObject() {node = node_;}
  ~DataNodeDeleter() {delete node;}
};

iTreeViewItem::iTreeViewItem(taiSigLink* link_, MemberDef* md_, iTreeViewItem* node,
  iTreeViewItem* after, const String& tree_name, int dn_flags_)
:inherited(node, after)
{
  init(tree_name, link_, md_, dn_flags_);
}

iTreeViewItem::iTreeViewItem(taiSigLink* link_, MemberDef* md_, iTreeView* parent,
  iTreeViewItem* after, const String& tree_name, int dn_flags_)
:inherited(parent, after)
{
  // assume by default we want to update name if no md, and not told not to
  if (!md_ && !(dn_flags_ & DNF_NO_UPDATE_NAME))
    dn_flags_ |= DNF_UPDATE_NAME;
  init(tree_name, link_, md_, dn_flags_);
}

void iTreeViewItem::init(const String& tree_name, taiSigLink* link_,
  MemberDef* md_, int dn_flags_)
{
  m_md = md_;
  dn_flags = dn_flags_;
  link_->AddDataClient(this); // sets link
/*OBS: we really aren't using links
  // links get name italicized
  //TODO: to avoid creating a brand new font for each item, we could
  // get an italicized version from Tree (and everyone would share)
  if (dn_flags & DNF_IS_LINK) {
    QFont fnt(treeView()->font());
    fnt.setItalic(true);
    setData(0, Qt::FontRole, fnt);
  }
*/
  setName(GetColText(0, tree_name));
  setDragEnabled(dn_flags & DNF_CAN_DRAG);
  setDropEnabled(!(dn_flags & DNF_NO_CAN_DROP));
  // note: lazy children moved to Decorate because uses virtuals
}

iTreeViewItem::~iTreeViewItem() {
  iTreeView* tv = treeView();
  if (tv) {
    tv->ItemDestroyingCb(this);
  }
}

bool iTreeViewItem::canAcceptDrop(const QMimeData* mime) const {
 return true;
/*obs always say yes, since we put up a drop menu, and this decode
  // would be done all the time
  taiMimeSource* ms = taiMimeSource::New(mime);
  int ea = QueryEditActions_(ms);
  bool rval = (ea & taiClipData::EA_DROP_OPS);
  delete ms;
  return rval;*/
}

/* nnint iTreeViewItem::compare (QTreeWidgetItem* item, int col, bool ascending) const {
  // if we have a visual parent, delegate to its data link, otherwise just do the default
  iTreeViewItem* par = parent();
  if (par)  {
    int rval = par->link->CompareChildItems(this, (iTreeViewItem*)item);
    if (ascending) return rval;
    else return rval * -1;
  } else
    return QTreeWidgetItem::compare(item, col, ascending);
} */

void iTreeViewItem::CreateChildren() {
  inherited::CreateChildren();
  DecorateDataNode();
}

void iTreeViewItem::DataChanged_impl(int dcr, void* op1_, void* op2_) {
  if (dcr != DCR_ITEM_UPDATED) return;
  if (this->dn_flags & iTreeViewItem::DNF_UPDATE_NAME) {
    String nm = link()->GetName();
    if (nm.empty())
      nm = "(" + link()->GetDataTypeDef()->name + ")";
    setName(nm); // col0, except list nodes
  }
  DecorateDataNode();
}

void iTreeViewItem::DataLinkDestroying(taSigLink*) {
  iTreeView* tv = treeView();
  if(tv) {
    tv->EmitTreeStructToUpdate();
  }
  delete this;
}

void iTreeViewItem::DecorateDataNode() {
  // note: first do lazy children, which was previously in ctor but can no longer
  // be because it uses virtual calls -- this is safe and conservative if children
  // are already made
  if (dn_flags & DNF_LAZY_CHILDREN) {
    enableLazyChildren();
  }

  int bmf = 0;
  int dn_flags_supported = 0;
  taiSigLink* link = this->link(); // local cache
  QIcon ic;
  if (isExpanded()) bmf |= NBF_FOLDER_OPEN;
  bool has_ic = link->GetIcon(bmf, dn_flags_supported, ic);
  //TODO (or in GetIcon somewhere) add link iconlet and any other appropriate mods
  if (has_ic)
    setIcon(0, ic);
  // fill out remaining col text and data according to key
  iTreeView* tv = treeView();
  if (!tv) return; //shouldn't happen
  // we only fiddle the font if item disabled or previously disabled
  // (otherwise, we'd be superfluously setting a Font into each item!)
  //  bool set_font = (!item_enabled);
  bool set_font = false;
  for (int i = 0; i < tv->columnCount(); ++i) {
    if (i == 0) {
      if (!set_font)
        set_font = data(0, Qt::FontRole).isValid();
      setText(0, GetColText(0, given_name));
    }
    else {
      setText(i, GetColText(i));
    }
    // font -- just for reference -- not used
//     if (set_font) {
//       if (item_enabled) {
//         // setting the font to nil causes the itemdelegate guy to use default
//         setData(i, Qt::FontRole, QVariant());
//       } else {
//         setData(i, Qt::FontRole, QVariant(tv->italicFont()));
//       }
//     }
    // then, col data, if any (empty map, otherwise)
    QMap_qstr_qvar map = tv->colDataKeys(i);
    if (!map.isEmpty()) {
      KeyString key;
      QMap_qstr_qvar::const_iterator itr;
      for (itr = map.constBegin(); itr != map.constEnd(); ++itr) {
        // remember, we used roles as map keys, and put our colkey in the value
        bool ok; // helps avoid errors, by making sure the mapkey is an int
        int role = itr.key().toInt(&ok);
        if (ok) {
          key = itr.value().toString();
          setData(i, role, link->GetColData(key, role));
        }
      }
    }
  }
  // if tree is using highlighting, then highlight according to state information
  if (tv->highlightRows()) {
    String dec_key = link->GetStateDecoKey();
    if(dec_key.nonempty()) {
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        if(vc->use_bg)          // prefer bg color; always set bg so no conflict with type info
          setBackgroundColor(vc->bg_color.color());
        else if(vc->use_fg)
          setBackgroundColor(vc->fg_color.color());
      }
      else {
        resetBackgroundColor();//setHighlightIndex(0);
      }
    }
    else {
      resetBackgroundColor();//setHighlightIndex(0);
    }
  }
  // if decoration enabled, then decorate away
  if (tv->decorateEnabled() && (taMisc::color_hints & taMisc::CH_BROWSER)) {
    String dec_key = link->GetTypeDecoKey(); // nil if none
    if (dec_key.nonempty()) {
      ViewColor* vc = taMisc::view_colors->FindName(dec_key);
      if(vc) {
        if(vc->use_fg)
          setTextColor(vc->fg_color.color());
        else if(vc->use_bg)
          setTextColor(vc->bg_color.color()); // always set text, even if bg, so no conflict with state info
      }
    }
  }
}

void iTreeViewItem::dropped(const QMimeData* mime, const QPoint& pos,
    int key_mods, WhereIndicator where)
{
  DropHandler(mime, pos, key_mods, where);
}

void iTreeViewItem::QueryEditActionsS_impl_(int& allowed, int& forbidden,
  GuiContext sh_typ) const
{
  if (dn_flags & DNF_IS_MEMBER) {
    forbidden |= (taiClipData::EA_CUT | taiClipData::EA_DELETE);
  }
  IObjectSelectable::QueryEditActionsS_impl_(allowed, forbidden, sh_typ);
}

void iTreeViewItem::FillContextMenu_impl(taiActions* menu,
  GuiContext sh_typ) {
  //taiAction* mel =
  menu->AddItem("Find from here (Alt+F)...", taiMenu::use_default,
                taiAction::men_act, treeView(), SLOT(mnuFindFromHere(taiAction*)), this);
  menu->AddItem("Replace in selected (Alt+R)...", taiMenu::use_default,
                taiAction::men_act, treeView(), SLOT(mnuReplaceFromHere(taiAction*)), this);
  // note: this causes it to always search from the first one entered!  need to trap
  // specific keyboard input
//   menu->AddItem("Find from here...", taiAction::men_act, treeView(),
//              SLOT(mnuFindFromHere(taiAction*)), this, QKeySequence("Alt+F"));
  IObjectSelectable::FillContextMenu_impl(menu, sh_typ);
}

const String iTreeViewItem::GetColText(int col, const String& def) const
{
  iTreeView* tv = treeView();
  taiSigLink* link = this->link(); // local cache
  String rval;
  if (tv && link) {
    KeyString key = tv->colKey(col);
    if (key.length() > 0) { // no point if no key
      const int max_chars = tv->maxColChars(col); // -1 if no limit
      const int col_format = tv->colFormat(col); // 0 if none
      if (col_format & iTreeView::CF_ELIDE_TO_FIRST_LINE)
        rval = link->GetColText(key).elidedToFirstLine();
      else
        rval = link->GetColText(key).elidedTo(max_chars);
    } else if (col == 0) { // use default name for col 0
      rval = link->GetDisplayName();
    }
  }
  if (rval.empty())
    rval = def;
  return rval;
}

ISelectableHost* iTreeViewItem::host() const {
  iTreeView* tv = treeView();
  return (tv) ? (ISelectableHost*)tv : NULL;
}

taiClipData* iTreeViewItem::GetClipDataSingle(int src_edit_action, bool for_drag,
                                              GuiContext sh_typ) const {
  taiClipData* rval = IObjectSelectable::GetClipDataSingle(src_edit_action, for_drag, sh_typ);
//   treeView()->clearSelection();
  return rval;
}

taiClipData* iTreeViewItem::GetClipDataMulti(const ISelectable_PtrList& sel_items,
                     int src_edit_action, bool for_drag, GuiContext sh_typ) const {
  taiClipData* rval = IObjectSelectable::GetClipDataMulti(sel_items, src_edit_action, for_drag, sh_typ);
//   treeView()->clearSelection();
  return rval;
}

void iTreeViewItem::itemExpanded(bool value) {
  inherited::itemExpanded(value); // creates children
  DecorateDataNode();
}

void* iTreeViewItem::linkData() const {
  return (m_link) ? m_link->data() : NULL;
}

void iTreeViewItem::moveChild(int fm_idx, int to_idx) {
  if (fm_idx == to_idx) return; // DOH!
  // if the fm is prior to to, we need to adjust index (for removal)
  if (fm_idx < to_idx) --to_idx;
  QTreeWidgetItem* tak = takeChild(fm_idx);
  insertChild(to_idx, tak);
  // need to re-assert the hidden, because everything goes visible after the take
  iTreeView* tv = treeView();
  if (!tv) return;
  QTreeWidgetItemIterator it(this);
  QTreeWidgetItem* item_;
  while ( (item_ = *it) ) {
    iTreeViewItem* item = dynamic_cast<iTreeViewItem*>(item_);
    if (item) {
      bool hide_it = !tv->ShowNode(item);
      bool is_hid = item->isHidden();
      if (hide_it != is_hid) {
        item->setHidden(hide_it);
      }
    }
    ++it;
  }
}

ISelectable* iTreeViewItem::par() const {
  iTreeViewItem* rval = parent();
  if (rval) return rval;
  else return NULL;
}

iTreeViewItem* iTreeViewItem::parent() const {
  return dynamic_cast<iTreeViewItem*>(inherited::parent());
}

void iTreeViewItem::setName(const String& value) {
  given_name = value;
  this->setText(0, value);
}

bool iTreeViewItem::ShowNode_impl(int show, const String&) const
{
  // if not a member, then we just always show, since it must be a list element,
  // or standalone item whose visibility will be controlled by a parent member somewhere
  if (!m_md) return true;
  //TODO: note, context is ignored for now
  return m_md->ShowMember((TypeItem::ShowMembs)show, TypeItem::SC_TREE);
}

void iTreeViewItem::swapChildren(int n1_idx, int n2_idx) {
  // we move higher to lower, then lower is next after, and moved to higher
  if (n1_idx > n2_idx) {int t = n1_idx; n1_idx = n2_idx; n2_idx = t;}
  moveChild(n2_idx, n1_idx);
  moveChild(n1_idx + 1, n2_idx);
}

iTreeView* iTreeViewItem::treeView() const {
  iTreeView* rval = dynamic_cast<iTreeView*>(treeWidget());
  return rval;
}



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

#ifndef iTreeViewItem_h
#define iTreeViewItem_h 1

// parent includes:
#ifndef __MAKETA__
#include <iTreeWidgetItem>
#else
class iTreeWidgetItem; // #IGNORE
#endif

#include <ISigLinkClient>
#include <IObjectSelectable>

// member includes:

// declare all other types mentioned but not required to include:
#ifndef __MAKETA__
#include <iTreeView>
#include <taiActions>
#else
class iTreeView;
class taiActions;
#endif

/* Node Bitmap flags
The icon client (ex. Browser) will query for an icon, passing the attributes such as Open, Linked, etc.
The icon supplier (ex. taBase object) returns an icon bitmap, and an indication of what attribute flags it
supports. The Browser can then add its own generic modifiers for attributes not supported by the supplier.
ex. the browser can add a generic small arrow to indicate a link
*/

enum NodeBitmapFlags {
  NBF_FOLDER_OPEN               = 0x01,
  NBF_LINK_ITEM                 = 0x02
};

TypeDef_Of(iTreeViewItem);

class TA_API iTreeViewItem: public iTreeWidgetItem,
  public virtual ISigLinkClient, public virtual IObjectSelectable {
  //  ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for Tree and List nodes
INHERITED(iTreeWidgetItem)
friend class iTreeView;
public:
  enum DataNodeFlags {
    DNF_IS_FOLDER       = 0x001, // true for list/group folder nodes (note: does *not* indicate whether item can contain other things or not)
    DNF_LAZY_CHILDREN   = 0x002, // start w/ dummy child; create real children when expanded
    DNF_UPDATE_NAME     = 0x004, // typically for list items, update the visual name (tree, tab, etc.) after item edited
    DNF_CAN_BROWSE      = 0x008, // can be a new browser root
    DNF_CAN_DRAG        = 0x010, // 16 can allow drags
    DNF_NO_CAN_DROP     = 0x020, // 32 cannot accept drops
    DNF_IS_MEMBER       = 0x040, // 64 true for members (and root), not for list/group items -- helps node configure edit ops
    DNF_IS_LIST_NODE    = 0x080, // true for nodes in a list view (in panel, not on tree)
    DNF_IS_LINK         = 0x100, // true for nodes that are links, not owned by parent
    DNF_NO_UPDATE_NAME  = 0x200 // for root items that don't have a md, we autoset UPDATE_NAME unless this flag is set
  };

  int                   dn_flags; // any of DataNodeFlags
  String                given_name; // name given to the obj during init or setName -- used as default for col0

  override bool         canAcceptDrop(const QMimeData* mime) const;
  void*                 linkData() const;
  virtual void          setName(const String& value); // is the first col, except for lists, which is 2nd -- note: only applicable for updates, not in constructor
  iTreeViewItem*        parent() const; // strongly typed version of base
  iTreeView*            treeView() const;

  virtual const String  GetColText(int col, const String& def = _nilString) const;

  override void         CreateChildren();
  void                  SigEmit(int dcr, void* op1, void* op2)
    {SigEmit_impl(dcr, op1, op2);} // primarily to support Refresh
  virtual void          DecorateDataNode(); // sets icon and other visual attributes, based on state of node
  bool                  ShowNode(int show) const
    {return ShowNode_impl(show, _nilString);}
    // whether to show the node, given the context
  bool                  ShowNode(int show, const String& context) const
    {return ShowNode_impl(show, context);}

  iTreeViewItem(taiSigLink* link_, MemberDef* md_, iTreeViewItem* parent_,
    iTreeViewItem* after, const String& tree_name, int dn_flags_ = 0);
  iTreeViewItem(taiSigLink* link_, MemberDef* md_, iTreeView* parent_,
    iTreeViewItem* after, const String& tree_name, int dn_flags_ = 0);
  ~iTreeViewItem();

public: // qt3 compatability functions, for convenience
  bool                  dragEnabled() const {return flags() & Qt::ItemIsDragEnabled;}
  void                  setDragEnabled(bool value) {Qt::ItemFlags f = flags(); if (value)
    setFlags(f | Qt::ItemIsDragEnabled); else setFlags(f & ~Qt::ItemIsDragEnabled);}

  bool                  dropEnabled() const {return flags() & Qt::ItemIsDropEnabled;}
  void                  setDropEnabled(bool value) {Qt::ItemFlags f = flags(); if (value)
    setFlags(f | Qt::ItemIsDropEnabled); else setFlags(f & ~Qt::ItemIsDropEnabled);}

  void                  moveChild(int fm_idx, int to_idx); //note: to_idx is based on before
  void                  swapChildren(int n1_idx, int n2_idx);

public: // ITypedObject interface
  override void*        This() {return (void*)this;}
  override TypeDef*     GetTypeDef() const {return &TA_iTreeViewItem;}

public: // ISigLinkClient interface
  override void         SigLinkRecv(taSigLink*, int dcr, void* op1, void* op2)
    {SigEmit(dcr, op1, op2);} // called when the data item has changed, esp. ex lists and groups
  override void         SigLinkDestroying(taSigLink* dl); // called by SigLink when it is destroying --

public: // ISelectable interface
  override taiSigLink* link() const {return ISigLinkClient::link();}
  override MemberDef*   md() const {return m_md;}
  override ISelectable* par() const;
  override ISelectableHost* host() const;
  override taiClipData* GetClipDataSingle(int src_edit_action, bool for_drag,
    GuiContext sh_typ = GC_DEFAULT) const;
  override taiClipData* GetClipDataMulti(const ISelectable_PtrList& sel_items,
    int src_edit_action, bool for_drag, GuiContext sh_typ = GC_DEFAULT) const;
//  override int                GetEditActions(taiMimeSource* ms) const; // simpler version uses Query
protected:
//  override int                EditAction_impl(taiMimeSource* ms, int ea);
//  override void               FillContextMenu_EditItems_impl(taiActions* menu, int allowed);
  override void         FillContextMenu_impl(taiActions* menu,
    GuiContext sh_typ); // this is the one to extend in inherited classes
  override void         QueryEditActionsS_impl_(int& allowed, int& forbidden,
    GuiContext sh_typ) const;  // OR's in allowed; OR's in forbidden

#ifndef __MAKETA__
protected:
  MemberDef*            m_md; // for members, the MemberDef (otherwise NULL)
  override void         dropped(const QMimeData* mime, const QPoint& pos,
    int key_mods, WhereIndicator where);
  virtual void          SigEmit_impl(int dcr, void* op1, void* op2); // called for each node when the data item has changed, esp. ex lists and groups
  override void         itemExpanded(bool value);

  virtual bool          ShowNode_impl(int show, const String& context) const;
private:
  void                  init(const String& tree_name, taiSigLink* link_,
    MemberDef* md_, int dn_flags_); // #IGNORE
#endif
};

#endif // iTreeViewItem_h

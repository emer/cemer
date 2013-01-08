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

#ifndef ISelectable_h
#define ISelectable_h 1

// parent includes:
#include <IDataLinkProxy>

// member includes:

// declare all other types mentioned but not required to include:
class MemberDef; // 
class taiDataLink; // 
class ISelectableHost; // 
class taBase; // 
class QWidget; // 
class QObject; // 
class TypeDef; // 
class ISelectable_PtrList; // 
class taiClipData; // 


class TA_API ISelectable: public virtual IDataLinkProxy { //
INHERITED(IDataLinkProxy)
friend class ISelectableHost;
public: // Interface Properties and Methods
  enum GuiContext { // primarily for the T3 guys that have dual identities (don't reorder these)
    GC_DEFAULT          = 0, // this code only used when requesting the default link
    GC_SINGLE_DATA      = 1, // for trees, etc., only one identity, and link() is data
    GC_DUAL_DEF_DATA    = 2, // for T3, guys like Layer, where def identity is the data
    GC_DUAL_DEF_VIEW    = 3,  // for T3, guys like GraphView, where def ident is the graphview obj itself
  };

  virtual MemberDef*    md() const {return NULL;} // memberdef in data parent, if any, of the selected item
  virtual taiDataLink*  own_link(GuiContext sh_typ = GC_DEFAULT) const; // owner item's link -- this is the *eff_data* parent (not the gui parent)
  virtual ISelectable*  par() const = 0; // gui parent, if any
  virtual taiDataLink*  par_link() const; // parent item's link -- this is the *gui* parent (not the data parent/owner)
  virtual taiDataLink*  viewLink() const {return NULL;} // only defined for T3 guys
  taiDataLink*          effLink(GuiContext sh_typ = GC_DEFAULT) const; // for trees and things like layers, we just just use the data, for things like graphs (where view==data) we use the view itself (override shType)
  virtual taiDataLink*  clipParLink(GuiContext sh_typ) const;
    // for tree stuff, we use the gui parent for clip ops; for t3 we use the eff_data owner
  virtual MemberDef*    par_md() const;// eff_data parent item's (if any) md
  virtual ISelectableHost* host() const = 0; //
  virtual GuiContext    shType() const {return GC_SINGLE_DATA;} // mediates menu handling, and default source for clip ops
  taBase*               taData(GuiContext sh_typ = GC_DEFAULT) const; // if the eff_data is taBase, this returns it
//obs  virtual String   view_name() const = 0; // for members, the member name; for list items, the name if any, otherwise a created name using the index
  QWidget*              widget() const; // gets from host
  QObject*              clipHandlerObj() const; // shortcut for host()->clipHanderObj();

  virtual TypeDef*      GetEffDataTypeDef(GuiContext sh_typ = GC_DEFAULT) const; // gets it from effLink
  virtual int           EditAction_(ISelectable_PtrList& sel_items, int ea,
    GuiContext sh_typ = GC_DEFAULT);
   // do the indicated edit action (called from browser or list view); normally implement the _impl
  virtual void          FillContextMenu(ISelectable_PtrList& sel_items,
    taiActions* menu, GuiContext sh_typ = GC_DEFAULT);
   // for multi or single (normally implement the _impl)
//  virtual void                FillContextMenu(taiActions* menu);
   // for single (normally implement the _impl)
  virtual taiClipData*  GetClipData(const ISelectable_PtrList& sel_items,
    int src_edit_action, bool for_drag, GuiContext sh_typ = GC_DEFAULT) const; // works for single or multi; normally not overridden
  virtual taiClipData*  GetClipDataSingle(int src_edit_action,
    bool for_drag, GuiContext sh_typ = GC_DEFAULT) const = 0;
  virtual taiClipData*  GetClipDataMulti(const ISelectable_PtrList& sel_items,
    int src_edit_action, bool for_drag, GuiContext sh_typ = GC_DEFAULT) const {return NULL;}// only needed if multi is handled
  virtual int           QueryEditActions_(taiMimeSource* ms,
    GuiContext sh_typ = GC_DEFAULT) const; // typically called on single item for canAcceptDrop
  int                   QueryEditActions_(const ISelectable_PtrList& sel_items,
    GuiContext sh_typ = GC_DEFAULT) const;
    // called to get edit items available on clipboard for the sel_items
  virtual int           RefUnref(bool ref) {return 1;} // ref'ed/unrefed in select lists etc.; optional, and can be used for lifetime mgt; returns count after operation

  ~ISelectable();
protected:
  void                  DropHandler(const QMimeData* mime, const QPoint& pos,
    int key_mods, int where);
    //  handles all aspects of a drag drop operation -- ALWAYS uses default context
    // (iTreeWidgetItem::WhereIndicator where)
  virtual int           EditActionD_impl_(taiMimeSource* ms,
    int ea, GuiContext sh_typ) = 0;
    // do Dst op for single selected item; generally doesn't need extending
  virtual int           EditActionS_impl_(int ea, GuiContext sh_typ) = 0;
    // do Src op for single or one of multi selected items; CUT and COPY usually just a 1 return code; we actually implement the actual clipboard transfer
  virtual void          FillContextMenu_EditItems_impl(taiActions* menu,
    int allowed, GuiContext sh_typ); // might be extended
  virtual void          FillContextMenu_impl(taiActions* menu,
    GuiContext sh_typ) {} // link handles most, called in FCM
  virtual void          GetContextCaptions(String& view_cap, String& obj_cap);
  virtual void          QueryEditActionsD_impl_(taiMimeSource* ms,
   int& allowed, int& forbidden, GuiContext sh_typ) const = 0;
    // get Dst ops allowed for a single item,
  virtual void          QueryEditActionsS_impl_(int& allowed,
   int& forbidden, GuiContext sh_typ) const = 0;
    // get Src ops allowed for a single item, possibly of many selected items
};

#endif // ISelectable_h

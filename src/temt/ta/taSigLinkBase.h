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

#ifndef taSigLinkBase_h
#define taSigLinkBase_h 1

// parent includes:
#include <taiSigLink>

// member includes:
#include <taBase>

// declare all other types mentioned but not required to include:


TypeDef_Of(taSigLinkBase);

class TA_API taSigLinkBase: public taiSigLink { // SigLink for taBase objects
INHERITED(taiSigLink)
public:
  taBase*               data() {return (taBase*)m_data;}
  taBase*               data() const {return (taBase*)m_data;}
  override bool         isBase() const {return true;}
  override int          checkConfigFlags() const; // we call CheckConfig
  override bool         isEnabled() const;
  override taiSigLink* ownLink() const;


  override bool         GetIcon(int bmf, int& flags_supported, QIcon& ic);
    // delegates to taBase::GetDataNodeBitmap
  override bool         HasChildItems();
  override TypeDef*     GetDataTypeDef() const;
  override String       GetPath() const {return data()->GetPath();}
  override String       GetPathNames() const {return data()->GetPathNames();}
  override String       GetPath_Long() const {return data()->GetPath_Long();}
  override String       GetTypeDecoKey() const;
  override String       GetStateDecoKey() const;
  override String       GetName() const;
  override String       GetDisplayName() const;
  override void         Search(iSearchDialog* dlg);
  override bool         ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const; // asks this type if we should show the md member
  override String       GetColText(const KeyString& key, int itm_idx = -1) const; // #IGNORE
  override const QVariant GetColData(const KeyString& key, int role) const;

  SL_FUNS(taSigLinkBase); //

protected:
  static void           SearchStat(taBase* tab, iSearchDialog* sd, int level = 0); // for searching

  taSigLinkBase(taBase* data_, taSigLink* &link_ref_);
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
  override void         QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden);
  override int          EditAction_impl(taiMimeSource* ms, int ea);
  override void         ChildQueryEditActions_impl(const MemberDef* par_md, taiSigLink* child,
     taiMimeSource* ms,  int& allowed, int& forbidden);
  override int          ChildEditAction_impl(const MemberDef* par_md, taiSigLink* child,
    taiMimeSource* ms, int ea);
  override void         FillContextMenu_impl(taiWidgetActions* menu);
};

#endif // taSigLinkBase_h

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

#ifndef taSigLinkTaBase_h
#define taSigLinkTaBase_h 1

// parent includes:
#include <taiSigLink>

// member includes:
#include <taBase>

// declare all other types mentioned but not required to include:


taTypeDef_Of(taSigLinkTaBase);

class TA_API taSigLinkTaBase: public taiSigLink { // SigLink for taBase objects
INHERITED(taiSigLink)
public:
  taBase*               data() {return (taBase*)m_data;}
  taBase*               data() const {return (taBase*)m_data;}
  bool         isBase() const override {return true;}
  int          checkConfigFlags() const override; // we call CheckConfig
  bool         isEnabled() const override;
  taiSigLink* ownLink() const override;


  bool         GetIcon(int bmf, int& flags_supported, QIcon& ic) override;
    // delegates to taBase::GetDataNodeBitmap
  bool         HasChildItems() override;
  TypeDef*     GetDataTypeDef() const override;
  String       GetPath() const override {return data()->GetPath();}
  String       GetPathNames() const override {return data()->GetPathNames();}
  String       GetPath_Long() const override {return data()->GetPath_Long();}
  String       GetTypeDecoKey() const override;
  String       GetStateDecoKey() const override;
  String       GetName() const override;
  String       GetDisplayName() const override;
  void         Search(iDialogSearch* dlg) override;
  bool         ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const override; // asks this type if we should show the md member
  String       GetColText(const KeyString& key, int itm_idx = -1) const override; // #IGNORE
  const QVariant GetColData(const KeyString& key, int role) const override;

  SL_FUNS(taSigLinkTaBase); //

protected:
  static void           SearchStat(taBase* tab, iDialogSearch* sd, int level = 0); // for searching

  taSigLinkTaBase(taBase* data_, taSigLink* &link_ref_);
  taiTreeNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
    iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags) override;
  void         QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) override;
  int          EditAction_impl(taiMimeSource* ms, int ea) override;
  void         ChildQueryEditActions_impl(const MemberDef* par_md, taiSigLink* child,
     taiMimeSource* ms,  int& allowed, int& forbidden) override;
  int          ChildEditAction_impl(const MemberDef* par_md, taiSigLink* child,
    taiMimeSource* ms, int ea) override;
  void         FillContextMenu_impl(taiWidgetActions* menu) override;
};

#endif // taSigLinkTaBase_h

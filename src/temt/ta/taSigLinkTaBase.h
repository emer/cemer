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
  bool         isBase() const CPP11_OVERRIDE {return true;}
  int          checkConfigFlags() const CPP11_OVERRIDE; // we call CheckConfig
  bool         isEnabled() const CPP11_OVERRIDE;
  taiSigLink* ownLink() const CPP11_OVERRIDE;


  bool         GetIcon(int bmf, int& flags_supported, QIcon& ic) CPP11_OVERRIDE;
    // delegates to taBase::GetDataNodeBitmap
  bool         HasChildItems() CPP11_OVERRIDE;
  TypeDef*     GetDataTypeDef() const CPP11_OVERRIDE;
  String       GetPath() const CPP11_OVERRIDE {return data()->GetPath();}
  String       GetPathNames() const CPP11_OVERRIDE {return data()->GetPathNames();}
  String       GetPath_Long() const CPP11_OVERRIDE {return data()->GetPath_Long();}
  String       GetTypeDecoKey() const CPP11_OVERRIDE;
  String       GetStateDecoKey() const CPP11_OVERRIDE;
  String       GetName() const CPP11_OVERRIDE;
  String       GetDisplayName() const CPP11_OVERRIDE;
  void         Search(iDialogSearch* dlg) CPP11_OVERRIDE;
  bool         ShowMember(MemberDef* md, TypeItem::ShowContext show_context) const CPP11_OVERRIDE; // asks this type if we should show the md member
  String       GetColText(const KeyString& key, int itm_idx = -1) const CPP11_OVERRIDE; // #IGNORE
  const QVariant GetColData(const KeyString& key, int role) const CPP11_OVERRIDE;

  SL_FUNS(taSigLinkTaBase); //

protected:
  static void           SearchStat(taBase* tab, iDialogSearch* sd, int level = 0); // for searching

  taSigLinkTaBase(taBase* data_, taSigLink* &link_ref_);
  taiTreeNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeNode* nodePar,
    iTreeView* tvPar, taiTreeNode* after, const String& node_name, int dn_flags) CPP11_OVERRIDE;
  void         QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden) CPP11_OVERRIDE;
  int          EditAction_impl(taiMimeSource* ms, int ea) CPP11_OVERRIDE;
  void         ChildQueryEditActions_impl(const MemberDef* par_md, taiSigLink* child,
     taiMimeSource* ms,  int& allowed, int& forbidden) CPP11_OVERRIDE;
  int          ChildEditAction_impl(const MemberDef* par_md, taiSigLink* child,
    taiMimeSource* ms, int ea) CPP11_OVERRIDE;
  void         FillContextMenu_impl(taiWidgetActions* menu) CPP11_OVERRIDE;
};

#endif // taSigLinkTaBase_h

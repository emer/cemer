// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   Emergent is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   Emergent is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.

#include "ta_qtclassbrowse.h"

#include "ta_type.h"

#include "icheckbox.h"
#include "ilineedit.h"
#include "ispinbox.h"

#include <qlayout.h>
#include <QTreeWidget>
#include "itreewidget.h"
#include <qpushbutton.h>
#include <qsplitter.h>
//#include <qtable.h>
#include <qtabwidget.h>

/*
  switch(tik) {
  case taMisc::TIK_ENUM:
    break;
  case taMisc::TIK_MEMBER:
    break;
  case taMisc::TIK_METHOD:
    break;
  case taMisc::TIK_TYPE:
    break;
  case taMisc::TIK_TOKEN:
    break;
  }
  
*/

//////////////////////////
//  taClassDataLink	//
//////////////////////////

taClassDataLink::taClassDataLink(taMisc::TypeInfoKind tik_, void* data_, taDataLink* &link_ref_)
:inherited(data_, link_ref_), tik(tik_)
{
  switch (tik) {
  case taMisc::TIK_ENUM: m_type = &TA_EnumDef; break;
  case taMisc::TIK_MEMBER: m_type = &TA_MemberDef; break;
  case taMisc::TIK_PROPERTY: m_type = &TA_PropertyDef; break;
  case taMisc::TIK_METHOD: m_type = &TA_MethodDef; break;
  case taMisc::TIK_TYPE: m_type = &TA_TypeDef; break;
  case taMisc::TIK_ENUMSPACE: m_type = &TA_EnumSpace; break;
  case taMisc::TIK_TOKENSPACE: m_type = &TA_TokenSpace; break;
  case taMisc::TIK_MEMBERSPACE: m_type = &TA_MemberSpace; break;
  case taMisc::TIK_PROPERTYSPACE: m_type = &TA_PropertySpace; break;
  case taMisc::TIK_METHODSPACE: m_type = &TA_MethodSpace; break;
  case taMisc::TIK_TYPESPACE: m_type = &TA_TypeSpace; break;
  default: m_type = NULL; // compiler food
  }
}

TypeDef* taClassDataLink::GetDataTypeDef() const {
  switch (tik) {
  case taMisc::TIK_ENUM: return &TA_EnumDef;
  case taMisc::TIK_MEMBER: return &TA_MemberDef;
  case taMisc::TIK_PROPERTY: return &TA_PropertyDef;
  case taMisc::TIK_METHOD: return &TA_MethodDef;
  case taMisc::TIK_TYPE: return &TA_TypeDef;
  case taMisc::TIK_ENUMSPACE: return &TA_EnumSpace;
  case taMisc::TIK_TOKENSPACE: return &TA_TokenSpace;
  case taMisc::TIK_MEMBERSPACE: return &TA_MemberSpace;
  case taMisc::TIK_PROPERTYSPACE: return &TA_PropertySpace;
  case taMisc::TIK_METHODSPACE: return &TA_MethodSpace;
  case taMisc::TIK_TYPESPACE: return &TA_TypeSpace;
  default: return &TA_void; // compiler food; anything but NULL!!!
  }
}


//////////////////////////
//  taTypeInfoDataLink	//
//////////////////////////

taTypeInfoDataLink::taTypeInfoDataLink(taMisc::TypeInfoKind tik_, TypeItem* data_)
:inherited(tik_, data_, data_->data_link)
{
}

taiTreeDataNode* taTypeInfoDataLink::CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new taTypeInfoTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taTypeInfoTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}

bool taTypeInfoDataLink::HasChildItems() {
  bool rval = false;
  switch(tik) {
  case taMisc::TIK_ENUM: break;
  case taMisc::TIK_MEMBER: break;
  case taMisc::TIK_PROPERTY: break;
  case taMisc::TIK_METHOD: break;
  case taMisc::TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    if (td->internal && !td->InheritsFormal(TA_template)) {
      break;
    }
    rval = true;
    }
  default:break; // compiler food
  }
  return rval;
}

String taTypeInfoDataLink::GetName() const {
  return data()->name;
}

String taTypeInfoDataLink::GetDisplayName() const {
  return data()->name;
}

iDataPanel* taTypeInfoDataLink::CreateDataPanel_impl() {
  taiTypeItemDataHost* tidh = 
    new taiTypeItemDataHost(data(), tik, true, false);
  tidh->Constr("", "", taiDataHost::HT_PANEL);
  EditDataPanel* dp = tidh->EditPanel(this);
  return dp;
} 


bool taTypeInfoDataLink::ShowMember(MemberDef* md) {
  return false; // na
}


//////////////////////////
//  taTypeSpaceDataLink_Base//
//////////////////////////

taTypeSpaceDataLink_Base::taTypeSpaceDataLink_Base(taMisc::TypeInfoKind tik_, 
  taPtrList_impl* data_, taDataLink* &link_ref_)
:inherited(tik_, data_, link_ref_)
{
}

taiTreeDataNode* taTypeSpaceDataLink_Base::CreateTreeDataNode_impl(MemberDef* md, 
  taiTreeDataNode* nodePar,
  iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags)
{
  taiTreeDataNode* rval = NULL;
  if (nodePar)
    rval = new taTypeSpaceTreeDataNode(this, md, nodePar, after, node_name, dn_flags);
  else
    rval = new taTypeSpaceTreeDataNode(this, md, tvPar, after, node_name, dn_flags);
  return rval;
}


//////////////////////////
//  taTypeSpaceDataLink	//
//////////////////////////

taTypeSpaceDataLink::taTypeSpaceDataLink(TypeSpace* data_)
:inherited(taMisc::TIK_TYPESPACE, data_, data_->data_link)
{
  dm = DM_DefaultRoot; // default for root, generally we override for other cases
}

taiDataLink* taTypeSpaceDataLink::GetListChild(int itm_idx) {
  TypeDef* el = static_cast<TypeDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = taiViewType::StatGetDataLink(el, &TA_TypeDef);
  return dl;
}

int taTypeSpaceDataLink::NumListCols() const {
  return 2;
}

String taTypeSpaceDataLink::GetColHeading(const KeyString& key) const {
  static String typ_name_("Type Name");
  static String typ_desc_("Type Name");
  if (key == taBase::key_name) return typ_name_;
  else if (key == taBase::key_desc) return typ_desc_;
  else return inherited::GetColHeading(key);
}

const KeyString taTypeSpaceDataLink::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_desc;
  default: return _nilKeyString;
  }
}

String taTypeSpaceDataLink::ChildGetColText(taDataLink* child, const KeyString& key,
  int itm_idx) const 
{
  if (child != NULL) {
    TypeDef* el = static_cast<TypeDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    if (key == taBase::key_name) return el->GetPathName();
    else if (key == taBase::key_desc) return el->desc;
  }
  return inherited::ChildGetColText(child, key, itm_idx);
}

bool taTypeSpaceDataLink::ShowChild(TypeDef* td) const {
  bool rval = false; // have to find at least one show condition
  if (dm & DM_ShowRoot) {
    if (td->InheritsFormal(&TA_class) && td->is_subclass) return false;
    else rval = true;
  }
  if (dm & DM_ShowNonRoot) {
    if (!(td->InheritsFormal(&TA_class) && td->is_subclass)) return false;
    else rval = true;
  }
  if (dm & DM_ShowEnums) {
    if (td->enum_vals.size == 0) return false;
    else rval = true;
  }
  if (dm & DM_ShowNonEnums) {
    if (td->enum_vals.size > 0) return false;
    else rval = true;
  }
  return rval;
}

//////////////////////////
//  taMethodSpaceDataLink	//
//////////////////////////

taMethodSpaceDataLink::taMethodSpaceDataLink(MethodSpace* data_)
:inherited(taMisc::TIK_METHODSPACE, data_, data_->data_link)
{
}

taiDataLink* taMethodSpaceDataLink::GetListChild(int itm_idx) {
  MethodDef* el = static_cast<MethodDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = taiViewType::StatGetDataLink(el, &TA_MethodDef);
  return dl;
}

int taMethodSpaceDataLink::NumListCols() const {
  return 3;
}

const KeyString taMethodSpaceDataLink::key_rval("rval");
const KeyString taMethodSpaceDataLink::key_params("params");

String taMethodSpaceDataLink::GetColHeading(const KeyString& key) const {
  static String meth_name_("Method Name");
  static String rval_("rval");
  static String params_("Params");
  if (key == taBase::key_name) return meth_name_;
  else if (key == key_rval) return rval_;
  else if (key == key_params) return params_;
  else return inherited::GetColHeading(key);
}

const KeyString taMethodSpaceDataLink::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return key_rval;
  case 2: return key_params;
  default: return _nilKeyString;
  }
}

String taMethodSpaceDataLink::ChildGetColText(taDataLink* child, const KeyString& key,
  int itm_idx) const 
{
  String rval;
  if (child != NULL) {
    MethodDef* el = static_cast<MethodDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    if (key == taBase::key_name) {
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == key_rval) rval = el->type->Get_C_Name();
    else if (key == key_params) rval = el->ParamsAsString();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}


//////////////////////////
//  taMemberSpaceDataLink	//
//////////////////////////

taMemberSpaceDataLink::taMemberSpaceDataLink(MemberSpace* data_)
:inherited(taMisc::TIK_MEMBERSPACE, data_, data_->data_link)
{
}

taiDataLink* taMemberSpaceDataLink::GetListChild(int itm_idx) {
  MemberDef* el = static_cast<MemberDef*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = taiViewType::StatGetDataLink(el, &TA_MemberDef);
  return dl;
}

int taMemberSpaceDataLink::NumListCols() const {
  return 2;
}

String taMemberSpaceDataLink::GetColHeading(const KeyString& key) const {
  static String memb_name_("Memb Name");
  static String memb_typ_("Memb Type");
  if (key == taBase::key_name) return memb_name_;
  else if (key == taBase::key_type) return memb_typ_;
  else return inherited::GetColHeading(key);
}

const KeyString taMemberSpaceDataLink::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_type;
  default: return _nilKeyString;
  }
}

String taMemberSpaceDataLink::ChildGetColText(taDataLink* child, const KeyString& key,
  int itm_idx) const
{
  String rval;
  if (child != NULL) {
    MemberDef* el = static_cast<MemberDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    if (key == taBase::key_name) { 
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == taBase::key_type)  rval = el->type->Get_C_Name();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}


//////////////////////////
//  taPropertySpaceDataLink	//
//////////////////////////

taPropertySpaceDataLink::taPropertySpaceDataLink(PropertySpace* data_)
:inherited(taMisc::TIK_PROPERTYSPACE, data_, data_->data_link)
{
}

taiDataLink* taPropertySpaceDataLink::GetListChild(int itm_idx) {
  MemberDefBase* el = static_cast<MemberDefBase*>(data()->PosSafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = taiViewType::StatGetDataLink(el, &TA_MemberDefBase);
  return dl;
}

int taPropertySpaceDataLink::NumListCols() const {
  return 2;
}

String taPropertySpaceDataLink::GetColHeading(const KeyString& key) const {
  static String prop_name_("Prop Name");
  static String prop_typ_("Memb Type");
  if (key == taBase::key_name) return prop_name_;
  else if (key == taBase::key_type) return prop_typ_;
  else return inherited::GetColHeading(key);
}

const KeyString taPropertySpaceDataLink::GetListColKey(int col) const {
  switch (col) {
  case 0: return taBase::key_name;
  case 1: return taBase::key_type;
  default: return _nilKeyString;
  }
}

String taPropertySpaceDataLink::ChildGetColText(taDataLink* child, const KeyString& key,
  int itm_idx) const
{
  String rval;
  if (child != NULL) {
    MemberDefBase* el = static_cast<MemberDefBase*>(static_cast<taTypeInfoDataLink*>(child)->data());
    if (key == taBase::key_name) { 
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
    } else if (key == taBase::key_type)  rval = el->type->Get_C_Name();
    else return inherited::ChildGetColText(child, key, itm_idx);
  }
  return rval;
}


//////////////////////////////////
//  taTypeInfoTreeDataNode	//
//////////////////////////////////

taTypeInfoTreeDataNode::taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, MemberDef* md,
  taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_,  const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeInfoTreeDataNode::taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, MemberDef* md,
  iTreeView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

void taTypeInfoTreeDataNode::init(taTypeInfoDataLink* link_, int flags_) {
}

taTypeInfoTreeDataNode::~taTypeInfoTreeDataNode() {
}
  
void taTypeInfoTreeDataNode::CreateChildren_impl() {
  if (children_created) return;
  // following is default for most items
  int flags = iTreeViewItem::DNF_IS_FOLDER | iTreeViewItem::DNF_NO_CAN_DROP;
  switch (tik) {
  case taMisc::TIK_ENUM: {
    }
    break;
  case taMisc::TIK_MEMBER: {
    }
    break;
  case taMisc::TIK_PROPERTY: {
    }
    break;
  case taMisc::TIK_METHOD: {
    /*MethodDef* md */ static_cast<MethodDef*>(data());
    //TODO: enumerate params
    //taiDataLink* dl = NULL;
    }
    break;
  case taMisc::TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    if (td->internal && !td->InheritsFormal(TA_template)) {
      break;
    }
    taiDataLink* dl = NULL;
    // if we are an enum, we handle differently (only showing enums, and directly under us)
    // don't sort them
    if (td->enum_vals.size > 0) {
      String tree_nm;
      for (int j = 0; j < td->enum_vals.size; ++j) {
        // do global processing for the type item
        EnumDef* ed = td->enum_vals.FastEl(j);
        //TODO: determine whether to show or not
         
        dl = taiViewType::StatGetDataLink(ed, &TA_EnumDef);

        if (dl == NULL) continue; // shouldn't happen...
    
        tree_nm = dl->GetDisplayName();
        if (tree_nm.empty()) {
        //TODO  tree_nm = link()->AnonymousItemName(typ->name, i);
        }
        int flags_j = 0;
        last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this,
          last_child_node, tree_nm, flags_j);
      }
      break;
    }
//    taTypeSpaceTreeDataNode dn = NULL;
    // for the user, we break sub_types into enums and non-enums
    // enums -- we don't sort them, just in programmer order
    if (td->HasEnumDefs()) {
      TypeSpace* ts = &td->sub_types;
      taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(
        taiViewType::StatGetDataLink(ts, &TA_TypeSpace));
      last_child_node = tsdl->CreateTreeDataNode(NULL, this, 
        last_child_node, "enums", flags); 
      tsdl->dm = taTypeSpaceDataLink::DM_DefaultEnum;
    }
   
    // true (non-enum) subtypes
    if (td->HasSubTypes()) {
      TypeSpace* st = &td->sub_types;
      taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(
        taiViewType::StatGetDataLink(st, &TA_TypeSpace));
      last_child_node = tsdl->CreateTreeDataNode( 
          NULL, this, last_child_node, "sub types", flags);
      tsdl->dm = taTypeSpaceDataLink::DM_DefaultSubTypes;
    }
    // members -- note: don't sort, since they are in a programmer order already
    if (td->members.size > 0) {
      dl = taiViewType::StatGetDataLink(&td->members, &TA_MemberSpace);
      last_child_node = dl->CreateTreeDataNode( 
        NULL, this, last_child_node, "members", flags); 
    }
   
    // methods 
    if (td->methods.size > 0) {
      dl = taiViewType::StatGetDataLink(&td->methods, &TA_MethodSpace);
      last_child_node = dl->CreateTreeDataNode( 
        NULL, this, last_child_node, "methods", flags); 
    }
   
    // properties 
    if (td->properties.size > 0) {
      dl = taiViewType::StatGetDataLink(&td->properties, &TA_PropertySpace);
      last_child_node = dl->CreateTreeDataNode( 
        NULL, this, last_child_node, "properties", flags); 
    }
    
    // child types
    TypeSpace* ct = &td->children;
    if (ct->size > 0) {
      taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(
        taiViewType::StatGetDataLink(ct, &TA_TypeSpace));
      last_child_node = tsdl->CreateTreeDataNode( 
        NULL, this, last_child_node, "child types", flags); 
      tsdl->dm = taTypeSpaceDataLink::DM_DefaultChildren;
    }
   
    }
  default:break; // compiler food
  }
  children_created = true;
}



//////////////////////////////////
//  taTypeSpaceTreeDataNode	//
//////////////////////////////////

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, 
  MemberDef* md, taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, 
  MemberDef* md, iTreeView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, md, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

void taTypeSpaceTreeDataNode::init(taTypeSpaceDataLink_Base* link_, int flags_) {
/*  m_child_type = NULL; 
  switch (link_->tik) {
  case taMisc::TIK_ENUMSPACE: 
    m_child_tik = taMisc::TIK_ENUM; 
    m_child_type = &TA_EnumDef; 
    break;
  case taMisc::TIK_TYPESPACE: 
    m_child_tik = taMisc::TIK_TYPE; 
    m_child_type = &TA_TypeDef; 
    break;
  case taMisc::TIK_METHODSPACE: 
    m_child_tik = taMisc::TIK_METHOD; 
    m_child_type = &TA_MethodDef; 
    break;
  case taMisc::TIK_MEMBERSPACE: 
    m_child_tik = taMisc::TIK_MEMBER; 
    m_child_type = &TA_MemberDef; 
    break;
  case taMisc::TIK_PROPERTYSPACE: 
    m_child_tik = taMisc::TIK_MEMBER; 
    m_child_type = &TA_MemberDef; 
    break;
  default:  break;
  //TODO: TOKENS, if used
  }*/
}

taTypeSpaceTreeDataNode::~taTypeSpaceTreeDataNode() {
}

taTypeInfoDataLink* taTypeSpaceTreeDataNode::child_link(int idx) {
  taiDataLink* dl = NULL;
  TypeItem* ti = static_cast<TypeItem*>(data()->PosSafeEl_(idx));
  if (ti != NULL) {
    dl = taiViewType::StatGetDataLink(ti, ti->GetTypeDef());
  }
  return static_cast<taTypeInfoDataLink*>(dl);
}

void taTypeSpaceTreeDataNode::CreateChildren_impl() {
//  inherited::CreateChildren();
  String tree_nm;
  taiTreeDataNode* last_child_node = NULL;
  int flags = 0;
    switch (tik) {
    case taMisc::TIK_MEMBERSPACE:
      flags = iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    case taMisc::TIK_METHODSPACE:
      flags = iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    case taMisc::TIK_TYPESPACE: 
      flags = iTreeViewItem::DNF_CAN_BROWSE | iTreeViewItem::DNF_NO_CAN_DROP;
      break;
    default: break;
    }
  for (int i = 0; i < data()->size; ++i) {
    // do global processing for the type item
//none    
    // do any modal type-specific processing for the item
    switch (tik) {
    case taMisc::TIK_MEMBERSPACE: {
      MemberDef* md = static_cast<MemberDef*>(data()->SafeEl_(i));
      if (!ShowMember(md)) continue;
    } break;
    case taMisc::TIK_METHODSPACE: {
      MethodDef* md = static_cast<MethodDef*>(data()->SafeEl_(i));
      if (!ShowMethod(md)) continue;
    } break;
    case taMisc::TIK_TYPESPACE: {
      TypeDef* td = static_cast<TypeDef*>(data()->SafeEl_(i));
      if (!ShowType(td)) continue;
    } break;
    default: break;
    }
    
    taTypeInfoDataLink* dl = child_link(i);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
    //TODO  tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int flags = iTreeViewItem::DNF_CAN_BROWSE;
    last_child_node = dl->CreateTreeDataNode((MemberDef*)NULL, this, 
      last_child_node, tree_nm, flags);
  }
}
/*TODO: can only make showing modal if browser will either populate or
at least refresh dynamically
*/

bool taTypeSpaceTreeDataNode::ShowItem(TypeItem* ti) const {
return true;
  // default doesn't show hidden items
  if (ti->HasOption("HIDDEN") || ti->HasOption("NO_SHOW"))
    return false;
  return true;
}

bool taTypeSpaceTreeDataNode::ShowMember(MemberDef* md) const {
return true;
//  if (!ShowItem(md)) return false;
//  return true;
  return md->ShowMember(treeView()->show(), 
    TypeItem::SC_ANY); // note: don't say "tree" because this is a meta context
}

bool taTypeSpaceTreeDataNode::ShowMethod(MethodDef* md) const {
return true;
//  if (!ShowItem(md)) return false;
//  return true;
  return md->ShowMethod(treeView()->show());
}
 
bool taTypeSpaceTreeDataNode::ShowType(TypeDef* td) const {
  // first, check with dm of link
  taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(link());
  if (!tsdl->ShowChild(td)) return false;
  if (!ShowItem(td)) return false;
  // basic behavior is that we don't show derivitive types, ex. consts, refs, ptrs, etc.
  if ((td->ptr > 0) 
    || (td->ref)
    || (td->formal)
  ) return false;
  // need to check parentage for const -- (note: const is not formal for some weird reason)
  if (td->InheritsFrom(&TA_const)) 
    return false;
  
  return true;
}


//////////////////////////
//   iClassBrowseViewer 	//
//////////////////////////

iClassBrowseViewer::iClassBrowseViewer(ClassBrowseViewer* browser_, QWidget* parent)
:inherited((BrowseViewer*)browser_, parent)
{
}

iClassBrowseViewer::~iClassBrowseViewer()
{
}

void iClassBrowseViewer::mnuNewBrowser(taiAction* mel) {
  taiTreeDataNode* node = (taiTreeDataNode*)(mel->usr_data.toPtr());
  taClassDataLink* dl = static_cast<taClassDataLink*>(node->link());
  MainWindowViewer* brows = MainWindowViewer::NewClassBrowser(dl->data(),
     dl->GetDataTypeDef(), dl->GetDataMemberDef());
  if (!brows) return;
  brows->ViewWindow();
}


//////////////////////////////////
//   taiTypeItemDataHost	//
//////////////////////////////////

taiTypeItemDataHost::taiTypeItemDataHost(TypeItem* ti_, taMisc::TypeInfoKind tik_, 
  bool read_only_, bool modal_, QObject* parent)
:inherited(NULL, NULL, read_only, modal_, parent)
{
  ti = ti_;
  tik = tik_;
}

void taiTypeItemDataHost::Constr_Data_Labels() {
//  taiData* dat;
  int row = 0; // makes following more generic
  
  // name
//  mb_dat = md->im->GetDataRep(this, NULL, body);
//  data_el->Add(mb_dat);
//    rep = mb_dat->GetRep();
  iCheckBox* chk = NULL;
  iLineEdit* rep = new iLineEdit(ti->name, body);
  rep->setReadOnly(true);
  AddNameData(row++, "name", "name of the type item", rep, NULL, NULL, true);
  
  // description
  rep = new iLineEdit(ti->desc, body);
  rep->setReadOnly(true);
  AddNameData(row++, "description", "description of the type item", rep,
    NULL, NULL, true);
  
  // opts
//  String tmp = taMisc::StrArrayToChar(ti->opts);//ti->opts.AsString();
  String tmp = ti->opts.AsString();
  rep = new iLineEdit(tmp, body);
  rep->setReadOnly(true);
  AddNameData(row++, "options", "ta # options, including inherited",
    rep, NULL, NULL, true);
  
  // stuff shared for Members/Props
  MemberDefBase* md = dynamic_cast<MemberDefBase*>(ti);
  if (md) {
    // type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "type", "type of the member/property",
      rep, NULL, NULL, true);
    // static
    chk = new iCheckBox(md->is_static, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_static", "static (non-instance) member/property",
     chk, NULL, NULL, true);
    // read-only
    chk = new iCheckBox(md->isReadOnly(), body);
    chk->setReadOnly(true);
    AddNameData(row++, "readOnly", "member/property is read-only (including in CSS/programs)", chk, NULL, NULL, true);
    // gui read-only
    chk = new iCheckBox(md->isGuiReadOnly(), body);
    chk->setReadOnly(true);
    AddNameData(row++, "guiReadOnly", "member/property is read-only in the gui (but may be writable in CSS/programs)", chk, NULL, NULL, true);
  }
  
  switch (tik) {
  case taMisc::TIK_ENUM: {
    EnumDef* ed = static_cast<EnumDef*>(ti);
    // value
    rep = new iLineEdit(String(ed->enum_no), body);
    rep->setReadOnly(true);
    AddNameData(row++, "value", "value of the enum", rep, NULL, NULL, true);
    break;
    }
  case taMisc::TIK_MEMBER:  {
    MemberDef* md = static_cast<MemberDef*>(ti);
    //nothing specific
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue((ta_intptr_t)md->GetOff(NULL));
    repi->setReadOnly(true);
    AddNameData(row++, "offset", "offset, in bytes, of the member", 
      repi, NULL, NULL, true);
    break;
    }
  case taMisc::TIK_PROPERTY:  {
//     PropertyDef* pd = static_cast<PropertyDef*>(ti);
    // for properties only, indicate (so can distinguish from members in .properties)
    chk = new iCheckBox(true, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_property", "this is a property (not a member)",
      chk, NULL, NULL, true);
    break;
    }
  case taMisc::TIK_METHOD:  {
    MethodDef* md = static_cast<MethodDef*>(ti);
    // return type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "return type", "return type of the method",
      rep, NULL, NULL, true);
    
    // params
    rep = new iLineEdit(md->ParamsAsString(), body);
    rep->setReadOnly(true);
    AddNameData(row++, "params", "params of the method", 
      rep, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_static, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_static", "static (non-instance) method",
      chk, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_virtual, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_virtual", "virtual (overridable) method", 
      chk, NULL, NULL, true);
    
    chk = new iCheckBox(md->is_override, body);
    chk->setReadOnly(true);
    AddNameData(row++, "is_override", "virtual override of a base method", 
      chk, NULL, NULL, true);
    break;
    }

  case taMisc::TIK_TYPE:  {
    TypeDef* td = static_cast<TypeDef*>(ti);
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue(td->size);
    repi->setReadOnly(true);
    AddNameData(row++, "size", "size, in bytes, of the type",
      repi, NULL, NULL, true);
    
    // plugin, if any
    String plg;
    if (td->plugin) {
      plg = td->plugin->name;
    }
    rep = new iLineEdit(plg, body);
    rep->setReadOnly(true);
    AddNameData(row++, "plugin class", "the classname of the IPlugin for the plugin in which this type was defined, if any",
      rep, NULL, NULL, true);
    
    // parents
    String pars;
    for (int i = 0; i < td->parents.size; ++i) {
      if (i > 0) pars.cat(", ");
      pars.cat(td->parents.FastEl(i)->name);
    }
    rep = new iLineEdit(pars, body);
    rep->setReadOnly(true);
    AddNameData(row++, "parents", "parent type(s)",
      rep, NULL, NULL, true);
    
    break;
    }
  default: break; // compiler food
  }
}


//////////////////////////
//   taTypeInfoViewType //
//////////////////////////

int taTypeInfoViewType::BidForView(TypeDef* td) {
  if (td->InheritsFrom(&TA_TypeItem) ||
    td->InheritsFrom(&TA_EnumSpace) ||
    td->InheritsFrom(&TA_TokenSpace) ||
    td->InheritsFrom(&TA_MemberSpace) ||
    td->InheritsFrom(&TA_PropertySpace) ||
    td->InheritsFrom(&TA_MethodSpace) ||
    td->InheritsFrom(&TA_TypeSpace)
  )
    return (inherited::BidForView(td) + 1);
  return 0;
}


taiDataLink* taTypeInfoViewType::GetDataLink(void* el, TypeDef* td) {
  if (!el) return NULL; 

  taMisc::TypeInfoKind tik = taMisc::TypeToTypeInfoKind(td);

  switch (tik) {
  case taMisc::TIK_ENUM:
  case taMisc::TIK_MEMBER:
  case taMisc::TIK_PROPERTY:
  case taMisc::TIK_METHOD:
  case taMisc::TIK_TYPE: {
    TypeItem* ti = static_cast<TypeItem*>(el);
    if (ti->data_link) return static_cast<taiDataLink*>(ti->data_link);
    else return new taTypeInfoDataLink(tik, ti);
    }
  case taMisc::TIK_MEMBERSPACE: {
    MemberSpace* s = static_cast<MemberSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMemberSpaceDataLink(s);
    }
  case taMisc::TIK_PROPERTYSPACE: {
    PropertySpace* s = static_cast<PropertySpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taPropertySpaceDataLink(s);
    }
  case taMisc::TIK_METHODSPACE: {
    MethodSpace* s = static_cast<MethodSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMethodSpaceDataLink(s);
    }
  case taMisc::TIK_TYPESPACE: {
    TypeSpace* s = static_cast<TypeSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taTypeSpaceDataLink(s);
    }
  case taMisc::TIK_TOKENSPACE:
  default:
    return NULL;
  }
  return NULL; //compiler food
}



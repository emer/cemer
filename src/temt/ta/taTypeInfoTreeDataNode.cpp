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

#include "taTypeInfoTreeDataNode.h"
#include <taiViewType>
#include <taTypeInfoDataLink>
#include <taTypeSpaceDataLink>

TypeDef_Of(EnumDef);

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
  case TypeItem::TIK_ENUM: {
    }
    break;
  case TypeItem::TIK_MEMBER: {
    }
    break;
  case TypeItem::TIK_PROPERTY: {
    }
    break;
  case TypeItem::TIK_METHOD: {
    // /*MethodDef* md */ static_cast<MethodDef*>(data());
    //TODO: enumerate params
    //taiDataLink* dl = NULL;
    }
    break;
  case TypeItem::TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    // if (td->internal && !td->IsTemplate()) {
    //   break;
    // }
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

void taTypeInfoTreeDataNode::willHaveChildren_impl(bool& will) const 
{
  will = true;
}


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

#include "ta_classbrowse.h"

#include "ta_type.h"

#include "icheckbox.h"
#include "ilineedit.h"
#include "ispinbox.h"

#include <qlayout.h>
#include <Q3ListView>
#include <qpushbutton.h>
#include <qsplitter.h>
//#include <qtable.h>
#include <qtabwidget.h>

/*
  switch(tik) {
  case TIK_ENUM:
    break;
  case TIK_MEMBER:
    break;
  case TIK_METHOD:
    break;
  case TIK_TYPE:
    break;
  case TIK_TOKEN:
    break;
  }
  
*/

//////////////////////////
//  taClassDataLink	//
//////////////////////////

taClassDataLink::taClassDataLink(TypeInfoKind tik_, void* data_, taDataLink* &link_ref_)
:inherited(data_, link_ref_), tik(tik_)
{
}

//////////////////////////
//  taTypeInfoDataLink	//
//////////////////////////

taTypeInfoDataLink::taTypeInfoDataLink(TypeInfoKind tik_, TypeItem* data_)
:inherited(tik_, data_, data_->data_link)
{
}

bool taTypeInfoDataLink::HasChildItems() {
  bool rval = false;
  switch(tik) {
  case TIK_ENUM: break;
  case TIK_MEMBER: break;
  case TIK_METHOD: break;
  case TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    if (td->internal) {
      break;
    }
    rval = true;
    }
  default:break; // compiler food
  }
  return rval;
}

TypeDef* taTypeInfoDataLink::GetDataTypeDef() const {
  return NULL;
}

taiMimeItem* taTypeInfoDataLink::GetMimeItem() {
//TODO:
  return NULL;
}

String taTypeInfoDataLink::GetName() const {
  return data()->name;
}

String taTypeInfoDataLink::GetDisplayName() const {
  return data()->name;
}

bool taTypeInfoDataLink::ShowMember(MemberDef* md) {
  return false; // na
}


//////////////////////////
//  taTypeSpaceDataLink_Base//
//////////////////////////

taTypeSpaceDataLink_Base::taTypeSpaceDataLink_Base(TypeInfoKind tik_, 
  taPtrList_impl* data_, taDataLink* &link_ref_)
:inherited(tik_, data_, link_ref_)
{
}


//////////////////////////
//  taTypeSpaceDataLink	//
//////////////////////////

taTypeSpaceDataLink::taTypeSpaceDataLink(TypeSpace* data_)
:inherited(TIK_TYPESPACE, data_, data_->data_link)
{
  dm = DM_DefaultRoot; // default for root, generally we override for other cases
}

taiDataLink* taTypeSpaceDataLink::GetListChild(int itm_idx) {
  TypeDef* el = static_cast<TypeDef*>(data()->SafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = ClassBrowser::StatGetDataLink(el, TIK_TYPE);
  return dl;
}

int taTypeSpaceDataLink::NumListCols() {
  return 1;
}

String taTypeSpaceDataLink::GetColHeading(int col) {
  static String typ_name_("Type Name");
  switch (col) {
  case 0: return typ_name_;
  default: return _nilString;
  }
  
}

String taTypeSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    TypeDef* el = static_cast<TypeDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: rval = el->GetPathName(); break;
    default: break;
    }
  }
  return rval;
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
:inherited(TIK_METHODSPACE, data_, data_->data_link)
{
}

taiDataLink* taMethodSpaceDataLink::GetListChild(int itm_idx) {
  MethodDef* el = static_cast<MethodDef*>(data()->SafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = ClassBrowser::StatGetDataLink(el, TIK_METHOD);
  return dl;
}

int taMethodSpaceDataLink::NumListCols() {
  return 3;
}

String taMethodSpaceDataLink::GetColHeading(int col) {
  static String meth_name_("Method Name");
  static String rval_("rval");
  static String params_("Params");
  switch (col) {
  case 0: return meth_name_;
  case 1: return rval_;
  case 2: return params_;
  default: return _nilString;
  }
  
}

String taMethodSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    MethodDef* el = static_cast<MethodDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0:
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
      break;
    case 1: rval = el->type->Get_C_Name(); break;
    case 2: rval = el->ParamsAsString(); break;
    default: break;
    }
  }
  return rval;
}


//////////////////////////
//  taMemberSpaceDataLink	//
//////////////////////////

taMemberSpaceDataLink::taMemberSpaceDataLink(MemberSpace* data_)
:inherited(TIK_MEMBERSPACE, data_, data_->data_link)
{
}

taiDataLink* taMemberSpaceDataLink::GetListChild(int itm_idx) {
  MemberDef* el = static_cast<MemberDef*>(data()->SafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = ClassBrowser::StatGetDataLink(el, TIK_MEMBER);
  return dl;
}

int taMemberSpaceDataLink::NumListCols() {
  return 2;
}

String taMemberSpaceDataLink::GetColHeading(int col) {
  static String memb_name_("Memb Name");
  static String memb_typ_("Memb Type");
  switch (col) {
  case 0: return memb_name_;
  case 1: return memb_typ_;
  default: return _nilString;
  }
  
}

String taMemberSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    MemberDef* el = static_cast<MemberDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: 
      if (el->is_static) rval = " static "; //note: sleazy leading space to sort before non-static
      rval += el->name; 
      break;
    case 1: rval = el->type->Get_C_Name(); break;
    default: rval = el->name; break;
    }
  }
  return rval;
}


//////////////////////////////////
//  taTypeInfoTreeDataNode	//
//////////////////////////////////

taTypeInfoTreeDataNode::taTypeInfoTreeDataNode(taTypeInfoDataLink* link_,  taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_,  const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeInfoTreeDataNode::taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, Q3ListView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
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
  int flags = iListViewItem::DNF_IS_FOLDER | iListViewItem::DNF_NO_CAN_DROP;
  switch (tik) {
  case TIK_ENUM: {
    }
    break;
  case TIK_MEMBER: {
    }
    break;
  case TIK_METHOD: {
    MethodDef* md = static_cast<MethodDef*>(data());
    //TODO: enumerate params
    taiDataLink* dl = NULL;
    }
    break;
  case TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    if (td->internal) {
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
         
        dl = ClassBrowser::StatGetDataLink(ed, TIK_ENUM);

        if (dl == NULL) continue; // shouldn't happen...
    
        tree_nm = dl->GetDisplayName();
        if (tree_nm.empty()) {
        //TODO  tree_nm = link()->AnonymousItemName(typ->name, i);
        }
        int flags_j = 0;
        last_child_node = browser_win()->CreateTreeDataNode(dl, (MemberDef*)NULL, this,
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
        ClassBrowser::StatGetDataLink(ts, TIK_TYPESPACE));
      last_child_node = browser_win()->CreateTreeDataNode(tsdl, 
        NULL, this, last_child_node, "enums", flags); 
      tsdl->dm = taTypeSpaceDataLink::DM_DefaultEnum;
    }
   
    // true (non-enum) subtypes
    if (td->HasSubTypes()) {
      TypeSpace* st = &td->sub_types;
      taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(
        ClassBrowser::StatGetDataLink(st, TIK_TYPESPACE));
      last_child_node = browser_win()->CreateTreeDataNode(tsdl, 
          NULL, this, last_child_node, "sub types", flags | iListViewItem::DNF_SORT_CHILDREN);
      tsdl->dm = taTypeSpaceDataLink::DM_DefaultSubTypes;
    }
    // members -- note: don't sort, since they are in a programmer order already
    if (td->members.size > 0) {
      dl = ClassBrowser::StatGetDataLink(&td->members, TIK_MEMBERSPACE);
      last_child_node = browser_win()->CreateTreeDataNode(static_cast<taMemberSpaceDataLink*>(dl), 
        NULL, this, last_child_node, "members", flags); 
    }
   
    // methods 
    if (td->methods.size > 0) {
      dl = ClassBrowser::StatGetDataLink(&td->methods, TIK_METHODSPACE);
      last_child_node = browser_win()->CreateTreeDataNode(static_cast<taMethodSpaceDataLink*>(dl), 
        NULL, this, last_child_node, "methods", flags | iListViewItem::DNF_SORT_CHILDREN); 
    }
   
    // child types
    TypeSpace* ct = &td->children;
    if (ct->size > 0) {
      taTypeSpaceDataLink* tsdl = static_cast<taTypeSpaceDataLink*>(
        ClassBrowser::StatGetDataLink(ct, TIK_TYPESPACE));
      last_child_node = browser_win()->CreateTreeDataNode(tsdl, 
        NULL, this, last_child_node, "child types", flags | iListViewItem::DNF_SORT_CHILDREN); 
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
  taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, 
  Q3ListView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

void taTypeSpaceTreeDataNode::init(taTypeSpaceDataLink_Base* link_, int flags_) {
  switch (link_->tik) {
  case TIK_TYPESPACE: m_child_tik = TIK_TYPE; break;
  case TIK_METHODSPACE: m_child_tik = TIK_METHOD; break;
  case TIK_MEMBERSPACE: m_child_tik = TIK_MEMBER; break;
  default:  break;
  //TODO: TOKENS, if used
  }
}

taTypeSpaceTreeDataNode::~taTypeSpaceTreeDataNode() {
}

taTypeInfoDataLink* taTypeSpaceTreeDataNode::child_link(int idx) {
  taiDataLink* dl = NULL;
  TypeItem* ti = static_cast<TypeItem*>(data()->SafeEl_(idx));
  if (ti != NULL) {
    dl = ClassBrowser::StatGetDataLink(ti, child_tik());
  }
  return static_cast<taTypeInfoDataLink*>(dl);
}

void taTypeSpaceTreeDataNode::CreateChildren_impl() {
//  inherited::CreateChildren();
  String tree_nm;
  taiTreeDataNode* last_child_node = NULL;
  int flags = 0;
    switch (tik) {
    case TIK_MEMBERSPACE:
      flags = iListViewItem::DNF_NO_CAN_DROP;
      break;
    case TIK_METHODSPACE:
      flags = iListViewItem::DNF_NO_CAN_DROP;
      break;
    case TIK_TYPESPACE: 
      flags = iListViewItem::DNF_CAN_BROWSE | iListViewItem::DNF_NO_CAN_DROP;
      break;
    default: break;
    }
  for (int i = 0; i < data()->size; ++i) {
    // do global processing for the type item
//none    
    // do any modal type-specific processing for the item
    switch (tik) {
    case TIK_MEMBERSPACE: {
      MemberDef* md = static_cast<MemberDef*>(data()->SafeEl_(i));
      if (!ShowMember(md)) continue;
    } break;
    case TIK_METHODSPACE: {
      MethodDef* md = static_cast<MethodDef*>(data()->SafeEl_(i));
      if (!ShowMethod(md)) continue;
    } break;
    case TIK_TYPESPACE: {
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
    int flags = iListViewItem::DNF_CAN_BROWSE;
    last_child_node = browser_win()->CreateTreeDataNode(dl, (MemberDef*)NULL, this, 
      last_child_node, tree_nm, flags);
  }
}

bool taTypeSpaceTreeDataNode::ShowItem(TypeItem* ti) const {
  // default doesn't show hidden items
  if (ti->HasOption("HIDDEN") || ti->HasOption("NO_SHOW"))
    return false;
  return true;
}

bool taTypeSpaceTreeDataNode::ShowMember(MemberDef* md) const {
  if (!ShowItem(md)) return false;
  return true;
}

bool taTypeSpaceTreeDataNode::ShowMethod(MethodDef* md) const {
  if (!ShowItem(md)) return false;
  return true;
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
//   iClassBrowser 	//
//////////////////////////

iClassBrowser::iClassBrowser(void* root_, TypeInfoKind tik_, ClassBrowser* browser_,
  QWidget* parent)
: iDataBrowserBase(root_, (DataViewer*)browser_, parent)
{
  m_tik = tik_;
}

iClassBrowser::~iClassBrowser()
{
}

void iClassBrowser::ApplyRoot() {
  if (!m_root) return;
  taiDataLink* dl = ClassBrowser::StatGetDataLink(m_root, m_tik);
  if (dl == NULL) return; // shouldn't happen...

  taiTreeDataNode* node;
  String nm = dl->GetName();
  if (nm.empty()) nm = "(root)";
  node = CreateTreeDataNode(dl, (MemberDef*)NULL, NULL, NULL, nm, iListViewItem::DNF_UPDATE_NAME);
    
  // always show the first items under the root
  node->CreateChildren();
  setCurItem(node);
  lvwDataTree->setOpen(node, true); // always open root node
}

taiTreeDataNode* iClassBrowser::CreateTreeDataNode_impl(taiDataLink* link, MemberDef* md_,
  taiTreeDataNode* parent_, taiTreeDataNode* last_child_, const String& tree_name, int flags_)
{
  TypeDef* typ = link->GetTypeDef();
  if (!typ->InheritsFrom(&TA_taClassDataLink)) {
    return inherited::CreateTreeDataNode_impl(link, md_, parent_, last_child_, tree_name, flags_);
    
  }
  
  taiTreeDataNode* rval = NULL;
    
  // slightly different constructor depending on if root item or not
  if (parent_ == NULL) {
    if (typ->InheritsFrom(&TA_taTypeInfoDataLink))
      rval = new taTypeInfoTreeDataNode((taTypeInfoDataLink*)link, lvwDataTree,
       last_child_, tree_name, flags_);
    else if (typ->InheritsFrom(&TA_taTypeSpaceDataLink_Base))
      rval = new taTypeSpaceTreeDataNode((taTypeSpaceDataLink_Base*)link, lvwDataTree,
       last_child_, tree_name, flags_);
  } else {
    if (typ->InheritsFrom(&TA_taTypeInfoDataLink))
      rval = new taTypeInfoTreeDataNode((taTypeInfoDataLink*)link, parent_,
       last_child_, tree_name, flags_);
    else if (typ->InheritsFrom(&TA_taTypeSpaceDataLink_Base))
      rval = new taTypeSpaceTreeDataNode((taTypeSpaceDataLink_Base*)link, parent_,
       last_child_, tree_name, flags_);
  }
  return rval;
}

iDataPanel* iClassBrowser::MakeNewDataPanel_(taiDataLink* link) {
  TypeDef* typ = link->GetTypeDef();
  if (!typ->InheritsFrom(&TA_taTypeInfoDataLink)) {
    return inherited::MakeNewDataPanel_(link);
  }
  taTypeInfoDataLink* dl = static_cast<taTypeInfoDataLink*>(link);
  taiTypeItemDataHost* tidh = 
    new taiTypeItemDataHost(dl->data(), dl->tik, true, false);
  tidh->Constr("", "", NULL, true);
  EditDataPanel* dp = tidh->EditPanel(link);
  return dp;
} 

void iClassBrowser::mnuNewBrowser(taiAction* mel) {
  taiTreeDataNode* node = (taiTreeDataNode*)(mel->usr_data.toPtr());
  taClassDataLink* dl = static_cast<taClassDataLink*>(node->link());
  ClassBrowser* brows = ClassBrowser::New(dl->data(), dl->tik);
  if (!brows) return;
  brows->ViewWindow();
  // move selection up to parent of cloned item, to reduce issues of changed data, confusion, etc.
  setCurItem(node->parent(), true);

  //TODO: (maybe!) delete the panel (to reduce excessive panel pollution)
}


//////////////////////////////////
// 	ClassBrowser	 	//
//////////////////////////////////

ClassBrowser* ClassBrowser::New(void* root_, TypeInfoKind tik) {
  ClassBrowser* rval = new ClassBrowser();
  rval->m_is_root = false; // class browser can never be the root
  rval->root = root_;
  rval->tik = tik;
  return rval;
}

taiDataLink* ClassBrowser::StatGetDataLink(void* el, TypeInfoKind tik) {
  if (el == NULL) return NULL; 

  switch (tik) {
  case TIK_ENUM:
  case TIK_MEMBER:
  case TIK_METHOD:
  case TIK_TYPE: {
    TypeItem* ti = static_cast<TypeItem*>(el);
    if (ti->data_link) return static_cast<taiDataLink*>(ti->data_link);
    else return new taTypeInfoDataLink(tik, ti);
    }
  case TIK_MEMBERSPACE: {
    MemberSpace* s = static_cast<MemberSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMemberSpaceDataLink(s);
    }
  case TIK_METHODSPACE: {
    MethodSpace* s = static_cast<MethodSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taMethodSpaceDataLink(s);
    }
  case TIK_TYPESPACE: {
    TypeSpace* s = static_cast<TypeSpace*>(el);
    if (s->data_link != NULL) return static_cast<taiDataLink*>(s->data_link);
    else return new taTypeSpaceDataLink(s);
    }
  case TIK_TOKENSPACE:
  default:
    return NULL;
  }
  return NULL; //compiler food
}

void ClassBrowser::Initialize() {
  link_type = &TA_taiDataLink;
}

void ClassBrowser::Constr_Window_impl() {
  if (!root) return;
  m_window = new iClassBrowser(root, tik, this);
}

void ClassBrowser::Clear_impl() {
  if (!m_window) return;
  browser_win()->Reset();
}

taDataLink* ClassBrowser::GetDataLink_(void* el, TypeDef* el_typ, int param) {
  return StatGetDataLink(el, static_cast<TypeInfoKind>(param));
}


void ClassBrowser::Render_impl() {
  if (!m_window) return;
  browser_win()->ApplyRoot();
}

void ClassBrowser::TreeNodeDestroying(taiTreeDataNode* item) {
  if (!m_window) return;
  browser_win()->TreeNodeDestroying(item);
}


//////////////////////////////////
//   taiTypeItemDataHost	//
//////////////////////////////////

taiTypeItemDataHost::taiTypeItemDataHost(TypeItem* ti_, TypeInfoKind tik_, 
  bool read_only_, bool modal_, QObject* parent)
:inherited(NULL, NULL, read_only, modal_, parent)
{
  ti = ti_;
  tik = tik_;
}

void taiTypeItemDataHost::Constr_Data() {
//note: everything done in constr_labels
}

void taiTypeItemDataHost::Constr_Labels() {
//  taiData* dat;
  int row = 0; // makes following more generic
  
  // name
//  mb_dat = md->im->GetDataRep(this, NULL, body);
//  data_el->Add(mb_dat);
//    rep = mb_dat->GetRep();
  iLineEdit* rep = new iLineEdit(ti->name, body);
  rep->setReadOnly(true);
//  AddName(row, "name", "name of the type item", rep);
  AddName(row, "name", "name of the type item", NULL);
  AddData(row++, rep, true);
  
  // description
  rep = new iLineEdit(ti->desc, body);
  rep->setReadOnly(true);
//  AddName(row, "description", "description of the type item", rep);
  AddName(row, "description", "description of the type item", NULL);
  AddData(row++, rep, true);
  
  // opts
//  String tmp = taMisc::StrArrayToChar(ti->opts);//ti->opts.AsString();
  String tmp = ti->opts.AsString();
  rep = new iLineEdit(tmp, body);
  rep->setReadOnly(true);
//  AddName(row, "options", "ta # options, including inherited", rep);
  AddName(row, "options", "ta # options, including inherited", NULL);
  AddData(row++, rep, true);
  
  switch (tik) {
  case TIK_ENUM: {
    EnumDef* ed = static_cast<EnumDef*>(ti);
    // value
    rep = new iLineEdit(String(ed->enum_no), body);
    rep->setReadOnly(true);
    AddName(row, "value", "value of the enum", NULL);
    AddData(row++, rep, true);
    break;
    }
  case TIK_MEMBER:  {
    MemberDef* md = static_cast<MemberDef*>(ti);
    // type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddName(row, "type", "type of the member", NULL);
    AddData(row++, rep, true);
    break;
    }

  case TIK_METHOD:  {
    MethodDef* md = static_cast<MethodDef*>(ti);
    // return type
    rep = new iLineEdit(md->type->Get_C_Name(), body);
    rep->setReadOnly(true);
    AddName(row, "return type", "return type of the method", NULL);
    AddData(row++, rep, true);
    
    // params
    rep = new iLineEdit(md->ParamsAsString(), body);
    rep->setReadOnly(true);
    AddName(row, "params", "params of the method", NULL);
    AddData(row++, rep, true);
    break;
    }

  case TIK_TYPE:  {
    TypeDef* td = static_cast<TypeDef*>(ti);
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue(td->size);
    repi->setReadOnly(true);
//    AddName(row, "size", "size, in bytes, of the type", repi);
    AddName(row, "size", "size, in bytes, of the type", NULL);
    AddData(row++, repi);
    break;
    }
  default: break; // compiler food
  }
}


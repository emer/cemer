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

TypeDef* taTypeInfoDataLink::GetDataTypeDef() {
  return NULL;
}

taiMimeItem* taTypeInfoDataLink::GetMimeItem() {
//TODO:
  return NULL;
}

String taTypeInfoDataLink::GetName() {
  return data()->name;
}

String taTypeInfoDataLink::GetDisplayName() {
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
}

taiDataLink* taTypeSpaceDataLink::GetListChild(int itm_idx) {
  TypeDef* el = static_cast<TypeDef*>(data()->SafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = ClassBrowser::StatGetDataLink(el, TIK_TYPE);
  return dl;
}

int taTypeSpaceDataLink::NumListCols() {
//TEMP
  return 1;
}

String taTypeSpaceDataLink::GetColHeading(int col) {
//TEMP
  switch (col) {
  case 0: return "Name";
  default: return _nilString;
  }
  
}

String taTypeSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    TypeDef* el = static_cast<TypeDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: rval = el->name;
    //TODO:
    }
  }
  return rval;
}



//////////////////////////
//  taEnumSpaceDataLink	//
//////////////////////////

taEnumSpaceDataLink::taEnumSpaceDataLink(EnumSpace* data_)
:inherited(TIK_ENUMSPACE, data_, data_->data_link)
{
}

taiDataLink* taEnumSpaceDataLink::GetListChild(int itm_idx) {
  EnumDef* el = static_cast<EnumDef*>(data()->SafeEl(itm_idx)); 
  if (el == NULL) return NULL;
  
  taiDataLink* dl = ClassBrowser::StatGetDataLink(el, TIK_ENUM);
  return dl;
}

int taEnumSpaceDataLink::NumListCols() {
//TEMP
  return 1;
}

String taEnumSpaceDataLink::GetColHeading(int col) {
//TEMP
  switch (col) {
  case 0: return "Name";
  default: return _nilString;
  }
  
}

String taEnumSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    EnumDef* el = static_cast<EnumDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: rval = el->name;
    //TODO:
    }
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
//TEMP
  return 1;
}

String taMethodSpaceDataLink::GetColHeading(int col) {
//TEMP
  switch (col) {
  case 0: return "Name";
  default: return _nilString;
  }
  
}

String taMethodSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    MethodDef* el = static_cast<MethodDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: rval = el->name;
    //TODO:
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
//TEMP
  return 1;
}

String taMemberSpaceDataLink::GetColHeading(int col) {
//TEMP
  switch (col) {
  case 0: return "Name";
  default: return _nilString;
  }
  
}

String taMemberSpaceDataLink::ChildGetColText(taDataLink* child, int col, int itm_idx) {
  String rval;
  if (child != NULL) {
    MemberDef* el = static_cast<MemberDef*>(static_cast<taTypeInfoDataLink*>(child)->data());
    switch (col) {
    case 0: rval = el->name;
    //TODO:
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
  
void taTypeInfoTreeDataNode::CreateChildren() {
  if (children_created) return;
  switch (tik) {
  case TIK_ENUM: break;
  case TIK_MEMBER: break;
  case TIK_METHOD: break;
  case TIK_TYPE: {
    TypeDef* td = static_cast<TypeDef*>(data());
    if (td->internal) {
      break;
    }
    taiDataLink* dl = NULL;
//    taTypeSpaceTreeDataNode dn = NULL;
    // enums
    dl = ClassBrowser::StatGetDataLink(&td->enum_vals, TIK_ENUMSPACE);
    last_child_node = new taTypeSpaceTreeDataNode(static_cast<taTypeSpaceDataLink*>(dl), 
      this, last_child_node, "enums",
      (iListViewItem::DNF_NO_CAN_DROP)); 
   
    // subtypes
    dl = ClassBrowser::StatGetDataLink(&td->sub_types, TIK_TYPESPACE);
    last_child_node = new taTypeSpaceTreeDataNode(static_cast<taTypeSpaceDataLink*>(dl), 
      this, last_child_node, "sub types",
      (iListViewItem::DNF_IS_FOLDER | iListViewItem::DNF_NO_CAN_DROP)); 
   
    // members
    // methods
    // child types
    dl = ClassBrowser::StatGetDataLink(&td->children, TIK_TYPESPACE);
    last_child_node = new taTypeSpaceTreeDataNode(static_cast<taTypeSpaceDataLink*>(dl), 
      this, last_child_node, "child types",
      (iListViewItem::DNF_IS_FOLDER | iListViewItem::DNF_NO_CAN_DROP)); 
   
    }
  default:break; // compiler food
  }
  
  //TODO: depends on type
  /*Type
  EnumSpace	enum_vals;	// if type is an enum, this is are the labels
  TypeSpace	sub_types;	// sub types scoped within class (incl enums)
  MemberSpace	members;	// member variables for class
  MethodSpace	methods;	// member functions (methods) for class */
  //Enum
  //Member
  //Method
  children_created = true;
}



//////////////////////////////////
//  taTypeSpaceTreeDataNode	//
//////////////////////////////////

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taTypeSpaceDataLink* link_, taiTreeDataNode* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

taTypeSpaceTreeDataNode::taTypeSpaceTreeDataNode(taTypeSpaceDataLink* link_, Q3ListView* parent_, 
  taiTreeDataNode* last_child_, const String& tree_name, int flags_)
:inherited(link_, NULL, parent_, last_child_, tree_name, flags_), tik(link_->tik)
{
  init(link_, flags_);
}

void taTypeSpaceTreeDataNode::init(taTypeSpaceDataLink* link_, int flags_) {
  switch (link_->tik) {
  case TIK_TYPESPACE: m_child_tik = TIK_TYPE; break;
  case TIK_ENUMSPACE: m_child_tik = TIK_ENUM; break;
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

void taTypeSpaceTreeDataNode::CreateChildren() {
//  inherited::CreateChildren();
  String tree_nm;
  taiTreeDataNode* last_child_node = NULL;
  for (int i = 0; i < data()->size; ++i) {
    taTypeInfoDataLink* dl = child_link(i);
    if (dl == NULL) continue; // shouldn't happen...

    tree_nm = dl->GetDisplayName();
    if (tree_nm.empty()) {
    //TODO  tree_nm = link()->AnonymousItemName(typ->name, i);
    }
    int flags = iListViewItem::DNF_CAN_BROWSE;
    last_child_node = browser_win()->CreateTreeDataNode(dl, (MemberDef*)NULL, this, last_child_node, tree_nm, flags);
  }
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
  node = CreateTreeDataNode(dl, (MemberDef*)NULL, NULL, NULL, dl->GetName(), iListViewItem::DNF_UPDATE_NAME);
    
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
    else if (typ->InheritsFrom(&TA_taTypeSpaceDataLink))
      rval = new taTypeSpaceTreeDataNode((taTypeSpaceDataLink*)link, lvwDataTree,
       last_child_, tree_name, flags_);
  } else {
    if (typ->InheritsFrom(&TA_taTypeInfoDataLink))
      rval = new taTypeInfoTreeDataNode((taTypeInfoDataLink*)link, parent_,
       last_child_, tree_name, flags_);
    else if (typ->InheritsFrom(&TA_taTypeSpaceDataLink))
      rval = new taTypeSpaceTreeDataNode((taTypeSpaceDataLink*)link, parent_,
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

void iClassBrowser::mnuNewBrowser(taiMenuEl* mel) {
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
  case TIK_ENUMSPACE: {
    EnumSpace* es = static_cast<EnumSpace*>(el);
    if (es->data_link != NULL) return static_cast<taiDataLink*>(es->data_link);
    else return new taEnumSpaceDataLink(es);
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
  AddName(row, "name", "name of the type item", rep);
  AddData(row++, rep, true);
  
  // description
  rep = new iLineEdit(ti->desc, body);
  rep->setReadOnly(true);
  AddName(row, "description", "description of the type item", rep);
  AddData(row++, rep, true);
  
  // opts
//  String tmp = taMisc::StrArrayToChar(ti->opts);//ti->opts.AsString();
  String tmp = ti->opts.AsString();
  rep = new iLineEdit(tmp, body);
  rep->setReadOnly(true);
  AddName(row, "options", "ta # options, including inherited", rep);
  AddData(row++, rep, true);
  
  switch (tik) {
  case TIK_ENUM: {
    EnumDef* ed = static_cast<EnumDef*>(ti);
    break;
    }
  case TIK_MEMBER:  {
    MemberDef* md = static_cast<MemberDef*>(ti);
    break;
    }

  case TIK_METHOD:  {
    MethodDef* ed = static_cast<MethodDef*>(ti);
    break;
    }

  case TIK_TYPE:  {
    TypeDef* td = static_cast<TypeDef*>(ti);
    // size
    iSpinBox* repi = new iSpinBox(body);
    repi->setValue(td->size);
    repi->setReadOnly(true);
    AddName(row, "size", "size, in bytes, of the type", repi);
    AddData(row++, repi);
    // ptr
    // ref
    // internal
    // formal
//  int		ptr;		// number of pointers
//  bool 		ref;		// true if a reference variable
//  bool		internal;	// true if an internal type (auto generated)
//  bool		formal;		// true if a formal type (e.g. class, const, enum..)
    break;
    }
  default: break; // compiler food
  }
}


/*
class taiCBItem: public QListViewItem {
public:
  TypeDef* typ;
  taiCBItem(TypeDef* typ_, QListView* parent, QString label1);
  taiCBItem(TypeDef* typ_, taiCBItem* parent, QString label1);
};

taiCBItem::taiCBItem(TypeDef* typ_, QListView* parent, QString label1)
: QListViewItem(parent, label1)
{
  typ = typ_;
}

taiCBItem::taiCBItem(TypeDef* typ_, taiCBItem* parent, QString label1)
: QListViewItem(parent, label1)
{
  typ = typ_;
}

#define NUM_MEMBER_COLS 3
#define NUM_METHOD_COLS 3

taiClassBrowser::taiClassBrowser(TypeDef* root_, QWidget* parent, const char* name)
: QScrollView(parent, name)
{
  mroot = NULL;
  mcurItem = NULL;
  mrecurse = false;

  this->setCaption("Class Browser");

  //TODO: make layOuter part of class
  QVBoxLayout* layOuter = new QVBoxLayout(this);
  splTree_Detail = new QSplitter(this, "splTree_Detail");
  layOuter->addWidget(splTree_Detail);
  lvwClassTree = new QListView(splTree_Detail, "lvwClassTree");
  layOuter->addWidget(lvwClassTree);
  frmDetail = new QFrame(splTree_Detail, "frmDetail");
  layDetail = new QVBoxLayout(frmDetail);
  frmDetailButtons = new QFrame(frmDetail, "frmDetailButtons");
  layDetail->addWidget(frmDetailButtons);
  layDetailButtons = new QHBoxLayout(frmDetailButtons);
  btnRecurse = new QPushButton(frmDetailButtons);
  layDetailButtons->addWidget(btnRecurse);
  //todo: more buttons here
  layDetailButtons->addStretch();
  ctlDetail = new QTabWidget(frmDetail, "ctlDetail");
  layDetail->addWidget(ctlDetail);
  tabMembers = new QTable(0, NUM_MEMBER_COLS, ctlDetail, "tabMembers");
  ctlDetail->addTab(tabMembers, "Members");
  tabMethods = new QTable(0, NUM_METHOD_COLS, ctlDetail, "tabMethods");
  ctlDetail->addTab(tabMethods, "Methods");


  lvwClassTree->addColumn("");

  btnRecurse->setText("Recurse");
  btnRecurse->setToggleButton(true);
  connect(btnRecurse, SIGNAL(toggled(bool)),
      this, SLOT(btnRecurse_toggled(bool)) );

  tabMembers->horizontalHeader()->setLabel(0, "P#");
  tabMembers->horizontalHeader()->setLabel(1, "Name: Type");
  tabMembers->horizontalHeader()->setLabel(2, "Description");

  tabMethods->horizontalHeader()->setLabel(0, "P#");
  tabMethods->horizontalHeader()->setLabel(1, "Name: RetType");
  tabMethods->horizontalHeader()->setLabel(2, "Description");


  connect(lvwClassTree, SIGNAL(selectionChanged(QListViewItem*)),
      this, SLOT(ItemSelected(QListViewItem*)) );
  setRoot(root_);

}

//taiClassBrowser::~taiClassBrowser() {}

void taiClassBrowser::ClearDetail() {
  tabMembers->setNumRows(0);
//  tabMembers->setText(0, 0, "");
//  tabMembers->setText(0, 1, "");
  tabMethods->setNumRows(0);
//  tabMethods->setText(0, 0, "");
//  tabMethods->setText(0, 1, "");
}

void taiClassBrowser::AddMember(MemberDef* mbd, TypeDef* typ, int parentLevel) {
  int i = tabMembers->numRows();
  tabMembers->setNumRows(i + 1);
  String nm = mbd->name + ": " + mbd->type->name;
  tabMembers->setText(i, 0, String(parentLevel));
  tabMembers->setText(i, 1, nm);
  tabMembers->setText(i, 2, mbd->desc); //TEMP
}

void taiClassBrowser::AddMethod(MethodDef* mthd, TypeDef* typ, int parentLevel) {
  int i = tabMethods->numRows();
  tabMethods->setNumRows(i + 1);
  String nm = mthd->name + ": " + mthd->type->name;
  tabMethods->setText(i, 0, String(parentLevel));
  tabMethods->setText(i, 1, nm);
  tabMethods->setText(i, 2, mthd->desc); //TEMP
}

void taiClassBrowser::AddMembers(TypeDef* typ, bool recurse, int parentLevel) {
  // add type's members
  for (int i = 0; i < typ->members.size; ++i) {
    MemberDef* mbd = typ->members.FastEl(i);
    AddMember(mbd, typ, parentLevel);
  }

  // add parent members, if requested and any
  if (recurse && (typ->parents.size > 0)) {
    for (int i = 0; i < typ->parents.size; ++i) {
      TypeDef* par = typ->parents.FastEl(i);
      AddMembers(par, true, parentLevel + 1);
    }
  }

}

void taiClassBrowser::AddMethods(TypeDef* typ, bool recurse, int parentLevel) {
  // add type's members
  for (int i = 0; i < typ->methods.size; ++i) {
    MethodDef* mthd = typ->methods.FastEl(i);
    AddMethod(mthd, typ, parentLevel);
  }

  // add parent members, if requested and any
  if (recurse && (typ->parents.size > 0)) {
    for (int i = 0; i < typ->parents.size; ++i) {
      TypeDef* par = typ->parents.FastEl(i);
      AddMethods(par, true, parentLevel + 1);
    }
  }

}

void taiClassBrowser::setCurItem(taiCBItem* item, bool forceUpdate) {
  if ((mcurItem == item) && (!forceUpdate)) return;
  ClearDetail();
  mcurItem = item;
  AddMembers(item->typ, mrecurse, 0);
  AddMethods(item->typ, mrecurse, 0);
}

void taiClassBrowser::setRoot(TypeDef* value) {
  if (mroot == value) return;
  mroot = value;
  Reset();
  if (mroot == NULL) return;

  taiCBItem* item = new taiCBItem(mroot, lvwClassTree, mroot->name);
  InitItem(mroot, item);
  AddItems(mroot, item);
}

void taiClassBrowser::AddItems(TypeDef* cls, taiCBItem* par_item) {
  for (int i = 0; i < cls->children.size; ++i) {
    TypeDef* typ = cls->children.FastEl(i);
    taiCBItem* item = new taiCBItem(typ, par_item, typ->name);
    InitItem(typ, item);
    // recursively add children
    AddItems(typ, item);
  }
}

void taiClassBrowser::InitItem(TypeDef* cls, taiCBItem* item) {
//  item->setText(0, cls->name);
  //TODO: more
}

void taiClassBrowser::Reset() {
  lvwClassTree->clear();
}

void taiClassBrowser::btnRecurse_toggled(bool on) {
  mrecurse = on;
  setCurItem(mcurItem, true);
}

void taiClassBrowser::ItemSelected(QListViewItem* item) {
  setCurItem((taiCBItem*)item);
}
*/


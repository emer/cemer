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

// ta_classbrowse.h -- class (type information) browser

#ifndef TA_CLASSBROWSE_H
#define TA_CLASSBROWSE_H

#include "ta_qtbrowse.h"
#include "ta_qtdialog.h"

// forwards

class taTypeInfoDataLink;
class taTypeSpaceDataLink;
class taTypeInfoTreeDataNode;
class taTypeSpaceTreeDataNode;

class iClassBrowser;
class ClassBrowser;

enum TypeInfoKind { // used in switch statements to particularize instances
  TIK_ENUM,
  TIK_MEMBER,
  TIK_METHOD,
  TIK_TYPE,
  TIK_MEMBERSPACE,
  TIK_METHODSPACE,
  TIK_TYPESPACE,
  TIK_TOKENSPACE
};

//////////////////////////
//   tabDataLink	//
//////////////////////////

class TA_API taClassDataLink: public taiDataLink { // DataLink for TypeInfo objects
INHERITED(taiDataLink)
public:
  const TypeInfoKind 	tik;
  
  DL_FUNS(taClassDataLink); //

protected:
  taClassDataLink(TypeInfoKind tik_, void* data_, taDataLink* &link_ref_);  //
};


class TA_API taTypeInfoDataLink: public taClassDataLink { // DataLink for TypeInfo objects
INHERITED(taClassDataLink)
public:
  TypeItem*		data() {return (TypeItem*)m_data;} //
  TypeItem*		data() const {return (TypeItem*)m_data;} //

//  override const QPixmap* GetIcon(int bmf, int& flags_supported);
    // delegates to taBase::GetDataNodeBitmap
  override bool		HasChildItems();
  override TypeDef*	GetDataTypeDef() const;
  override taiMimeItem* GetMimeItem();
  override String	GetName() const;
  override String	GetDisplayName() const;
  override bool		ShowMember(MemberDef* md); // asks this type if we should show the md member

  taTypeInfoDataLink(TypeInfoKind tik_, TypeItem* data_);  //
  DL_FUNS(taTypeInfoDataLink); //

protected:
/*  override void		QueryEditActions_impl(taiMimeSource* ms, int& allowed, int& forbidden);
  override int		EditAction_impl(taiMimeSource* ms, int ea);
  override void		ChildQueryEditActions_impl(const MemberDef* par_md, taiDataLink* child,
     taiMimeSource* ms,  int& allowed, int& forbidden);
  override int		ChildEditAction_impl(const MemberDef* par_md, taiDataLink* child,
    taiMimeSource* ms, int ea);
  override void		FillContextMenu_impl(taiMenu* menu); */
};


class TA_API taTypeSpaceDataLink_Base: public taClassDataLink {
  // DataLink for XxxSpace objects -- note that it also manages the ListView nodes
INHERITED(taClassDataLink)
public:
  taPtrList_impl*	data() {return (taPtrList_impl*)m_data;}
//  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
//  override int		NumListCols(); // number of columns in a list view for this item type
//  override String	GetColHeading(int col); // header text for the indicated column
//  override String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1);
  override bool		HasChildItems() {return true;} 

  taTypeSpaceDataLink_Base(TypeInfoKind tik_, taPtrList_impl* data_, 
    taDataLink* &link_ref_);
  DL_FUNS(taTypeSpaceDataLink_Base) //
};

class TA_API taTypeSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for TypeSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  enum DisplayMode { // #BITS
    DM_None		= 0x0000, // #NO_BIT
    DM_ShowRoot		= 0x0001, // show global items (really only applies to a root type space)
    DM_ShowNonRoot	= 0x0002, // show inherited items (really only applies to a root type space)
    DM_ShowEnums	= 0x0004, // show types that are enums (really only applies to .sub_types)
    DM_ShowNonEnums	= 0x0008, // show types that are not enums (really only applies to .sub_types)
  
#ifndef __MAKETA__    
    DM_DefaultRoot	= DM_ShowRoot | DM_ShowNonEnums,
    DM_DefaultChildren	= DM_ShowNonRoot | DM_ShowNonEnums,
    DM_DefaultEnum	= DM_ShowEnums,
    DM_DefaultSubTypes	= DM_ShowNonRoot | DM_ShowNonEnums,
    DM_Default		= DM_DefaultChildren
#endif
  };
  
  DisplayMode		dm;
  TypeSpace*		data() {return (TypeSpace*)m_data;}
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols(); // number of columns in a list view for this item type
  override String	GetColHeading(int col); // header text for the indicated column
  override String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1);
  
  bool			ShowChild(TypeDef* td) const; // determine whether to show, based on dm

  taTypeSpaceDataLink(TypeSpace* data_);
  DL_FUNS(taTypeSpaceDataLink) //
};


class TA_API taMethodSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for MethodSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  MethodSpace*		data() {return (MethodSpace*)m_data;}
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols(); // number of columns in a list view for this item type
  override String	GetColHeading(int col); // header text for the indicated column
  override String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1);

  taMethodSpaceDataLink(MethodSpace* data_);
  DL_FUNS(taMethodSpaceDataLink) //
};


class TA_API taMemberSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for MemberSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  MemberSpace*		data() {return (MemberSpace*)m_data;}
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols(); // number of columns in a list view for this item type
  override String	GetColHeading(int col); // header text for the indicated column
  override String	ChildGetColText(taDataLink* child, int col, int itm_idx = -1);

  taMemberSpaceDataLink(MemberSpace* data_);
  DL_FUNS(taMemberSpaceDataLink) //
};


class TA_API taTypeInfoTreeDataNode: public taiTreeDataNode { // node for type info, like type, enum, method, etc.
INHERITED(taiTreeDataNode)
public:
  const TypeInfoKind	tik;
  
  TypeItem* 		data() {return ((taTypeInfoDataLink*)m_link)->data();}
  taTypeInfoDataLink* 	link() const {return (taTypeInfoDataLink*)m_link;}

  
  taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeInfoTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taTypeInfoTreeDataNode;}
protected:
  override void 	CreateChildren_impl(); // called by the Node when it needs to create 
//  override void		DataChanged_impl(int dcr, void* op1, void* op2);
private:
  void			init(taTypeInfoDataLink* link_, int flags_); // #IGNORE
};


class TA_API taTypeSpaceTreeDataNode: public taiTreeDataNode { // node for spaces, ex. enumspace, typespace, etc.
INHERITED(taiTreeDataNode)
public:
  const TypeInfoKind	tik;
  TypeInfoKind		child_tik(){return m_child_tik;}
  
  taPtrList_impl* 	data() {return ((taTypeSpaceDataLink_Base*)m_link)->data();}
  taTypeInfoDataLink* 	child_link(int idx);
  taTypeSpaceDataLink_Base* 	link() const {return (taTypeSpaceDataLink_Base*)m_link;}
  bool			ShowItem(TypeItem* ti) const; 
    // determine whether to show, ex. based on a filter
  bool			ShowType(TypeDef* td) const; 
    // determine whether to show, ex. based on a filter
  bool			ShowMember(MemberDef* md) const; 
    // determine whether to show, ex. based on a filter
  bool			ShowMethod(MethodDef* md) const; 
    // determine whether to show, ex. based on a filter

//  override void		UpdateChildNames(); // #IGNORE update child names of the indicated node

  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, Q3ListView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeSpaceTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taTypeSpaceTreeDataNode;}
protected:
  TypeInfoKind		m_child_tik;
//  override void		DataChanged_impl(int dcr, void* op1, void* op2);
  override void 	CreateChildren_impl(); // called by the Node when it needs to create its children
  void			CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after_node, void* el);
//  void			UpdateListNames(); // #IGNORE updates names after inserts/deletes etc.
private:
  void			init(taTypeSpaceDataLink_Base* link_, int flags_); // #IGNORE
};


class TA_API iClassBrowser: public iDataBrowserBase { // viewer window used for class browsing
    Q_OBJECT
INHERITED(iDataBrowserBase)
friend class ClassBrowser;
public:

  ClassBrowser*		browser() {return (ClassBrowser*)m_viewer;}

  ~iClassBrowser();

public slots:
  virtual void		mnuNewBrowser(taiAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*

protected:
  iClassBrowser(void* root_, TypeInfoKind tik, ClassBrowser* browser_,
    QWidget* parent = 0);
  override taiTreeDataNode* 	CreateTreeDataNode_impl(taiDataLink* link, MemberDef* md_,
    taiTreeDataNode* parent_, taiTreeDataNode* last_child_, 
    const String& tree_name, int flags_); // pass parent=null if this is a root item

protected:
  TypeInfoKind		m_tik;

  void 			ApplyRoot(); // #IGNORE actually applies the new root value set in m_root/m_typ
  override iDataPanel* 	MakeNewDataPanel_(taiDataLink* link); 
};



//////////////////////////////////
//	DataBrowser		//
//////////////////////////////////

class TA_API ClassBrowser : public DataViewer {
  // #NO_TOKENS represents a class browser instance
friend class iClassBrowser;
public:
  static ClassBrowser*	New(void* root_, TypeInfoKind tik);
  static taiDataLink*  	StatGetDataLink(void* el, TypeInfoKind tik);
    // anyone can call this to get an object's datalink -- fetches existing or makes new

  void* 		root;
  TypeInfoKind 		tik;

  iClassBrowser*	browser_win() {return (iClassBrowser*)m_window;}

  void			TreeNodeDestroying(taiTreeDataNode* item); // #IGNORE check if curItem

  TA_BASEFUNS(ClassBrowser)
protected:
  override void		Constr_Window_impl(); // #IGNORE
  override void		Render_impl(); // #IGNORE
  override void		Clear_impl(); // #IGNORE
  override taDataLink*	GetDataLink_(void* el, TypeDef* el_typ, int param = 0); 
    // param is tik of item
private:
  void			Initialize();
  void			Destroy() {}
};


class TA_API taiTypeItemDataHost: public taiEditDataHost { // #IGNORE displays data on a TypeItem item
INHERITED(taiEditDataHost)
public:
  TypeItem*		ti; // #IGNORE
  TypeInfoKind		tik;
  
  taiTypeItemDataHost(TypeItem* ti_, TypeInfoKind tik, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
protected:
  override void 	Constr_Data();
  override void 	Constr_Labels();
};

#endif

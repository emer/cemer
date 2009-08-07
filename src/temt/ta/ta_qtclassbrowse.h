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

// ta_qtclassbrowse.h -- class (type information) browser

#ifndef TA_QTCLASSBROWSE_H
#define TA_QTCLASSBROWSE_H

#include "ta_qtdialog.h"
#include "ta_qtviewer.h"
#include "ta_qttype_def.h"

// forwards

class taTypeInfoDataLink;
class taTypeSpaceDataLink;
class taTypeInfoTreeDataNode;
class taTypeSpaceTreeDataNode;

class iClassBrowseViewer; //

//////////////////////////
//   tabDataLink	//
//////////////////////////

class TA_API taClassDataLink: public taiDataLink { // DataLink for TypeInfo objects
INHERITED(taiDataLink)
public:
  const taMisc::TypeInfoKind 	tik;
  TypeDef*		type() const {return m_type;}
  
  override TypeDef*	GetDataTypeDef() const;
  
  DL_FUNS(taClassDataLink); //

protected:
  TypeDef*		m_type;
  taClassDataLink(taMisc::TypeInfoKind tik_, void* data_, taDataLink* &link_ref_);  //
};


class TA_API taTypeInfoDataLink: public taClassDataLink { // DataLink for TypeInfo objects
INHERITED(taClassDataLink)
public:
  TypeItem*		data() {return (TypeItem*)m_data;} //
  TypeItem*		data() const {return (TypeItem*)m_data;} //

  override bool		HasChildItems();
  override String	GetName() const;
  override String	GetDisplayName() const;
  USING(inherited::ShowMember)
  override bool		ShowMember(MemberDef* md); // asks this type if we should show the md member

  taTypeInfoDataLink(taMisc::TypeInfoKind tik_, TypeItem* data_);  //
  DL_FUNS(taTypeInfoDataLink); //

protected:
  override iDataPanel* 	CreateDataPanel_impl(); 
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};


class TA_API taTypeSpaceDataLink_Base: public taClassDataLink {
  // DataLink for XxxSpace objects -- note that it also manages the ListView nodes
INHERITED(taClassDataLink)
public:
  taPtrList_impl*	data() {return (taPtrList_impl*)m_data;}
  override bool		HasChildItems() {return true;} 

  taTypeSpaceDataLink_Base(taMisc::TypeInfoKind tik_, taPtrList_impl* data_, 
    taDataLink* &link_ref_);
  DL_FUNS(taTypeSpaceDataLink_Base) //
protected:
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
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
  USING(inherited::GetListChild)
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override String	ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;
  
  bool			ShowChild(TypeDef* td) const; // determine whether to show, based on dm

  taTypeSpaceDataLink(TypeSpace* data_);
  DL_FUNS(taTypeSpaceDataLink) //
};


class TA_API taMethodSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for MethodSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  MethodSpace*		data() {return (MethodSpace*)m_data;}
  USING(inherited::GetListChild)
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols() const;
  static const KeyString key_rval;
  static const KeyString key_params;
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override String	ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;

  taMethodSpaceDataLink(MethodSpace* data_);
  DL_FUNS(taMethodSpaceDataLink) //
};


class TA_API taMemberSpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for MemberSpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  MemberSpace*		data() {return (MemberSpace*)m_data;}
  USING(inherited::GetListChild)
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override String	ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;

  taMemberSpaceDataLink(MemberSpace* data_);
  DL_FUNS(taMemberSpaceDataLink) //
};


class TA_API taPropertySpaceDataLink: public taTypeSpaceDataLink_Base {
  // DataLink for PropertySpace objects -- note that it also manages the ListView nodes
INHERITED(taTypeSpaceDataLink_Base)
public:
  PropertySpace*		data() {return (PropertySpace*)m_data;}
  USING(inherited::GetListChild)
  override taiDataLink*	GetListChild(int itm_idx); // returns NULL when no more
  override int		NumListCols() const;
  override const KeyString GetListColKey(int col) const;
  override String	GetColHeading(const KeyString& key) const;
  override String	ChildGetColText(taDataLink* child, const KeyString& key,
    int itm_idx = -1) const;

  taPropertySpaceDataLink(PropertySpace* data_);
  DL_FUNS(taPropertySpaceDataLink) //
};


class TA_API taTypeInfoTreeDataNode: public taiTreeDataNode { // node for type info, like type, enum, method, etc.
INHERITED(taiTreeDataNode)
public:
  const taMisc::TypeInfoKind	tik;
  
  USING(inherited::data)
  TypeItem* 		data() {return ((taTypeInfoDataLink*)m_link)->data();}
  taTypeInfoDataLink* 	link() const {return (taTypeInfoDataLink*)m_link;}

  
  taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, MemberDef* md, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeInfoTreeDataNode(taTypeInfoDataLink* link_, MemberDef* md, iTreeView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeInfoTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taTypeInfoTreeDataNode;}
protected:
  override void		willHaveChildren_impl(bool& will) const;
  override void 	CreateChildren_impl(); // called by the Node when it needs to create 
//  override void		DataChanged_impl(int dcr, void* op1, void* op2);
private:
  void			init(taTypeInfoDataLink* link_, int flags_); // #IGNORE
};


class TA_API taTypeSpaceTreeDataNode: public taiTreeDataNode { // node for spaces, ex. enumspace, typespace, etc.
INHERITED(taiTreeDataNode)
public:
  const taMisc::TypeInfoKind	tik;
  
  USING(inherited::data)
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

  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, taiTreeDataNode* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  taTypeSpaceTreeDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, iTreeView* parent_,
    taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
  ~taTypeSpaceTreeDataNode();
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_taTypeSpaceTreeDataNode;}
protected:
  override void		willHaveChildren_impl(bool& will) const;
  override void 	CreateChildren_impl(); // called by the Node when it needs to create its children
  void			CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after_node, void* el);
private:
  void			init(taTypeSpaceDataLink_Base* link_, int flags_); // #IGNORE
};


class TA_API iClassBrowseViewer: public iBrowseViewer { // viewer window used for class browsing
    Q_OBJECT
INHERITED(iBrowseViewer)
friend class ClassBrowser;
public:

  ClassBrowseViewer*		browser() {return (ClassBrowseViewer*)m_viewer;}

  iClassBrowseViewer(ClassBrowseViewer* browser_, QWidget* parent = 0);
  ~iClassBrowseViewer();

public slots:
  virtual void		mnuNewBrowser(taiAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*
};


class TA_API taiTypeItemDataHost: public taiEditDataHost { // #IGNORE displays data on a TypeItem item
INHERITED(taiEditDataHost)
public:
  TypeItem*		ti; // #IGNORE
  taMisc::TypeInfoKind		tik;
  
  taiTypeItemDataHost(TypeItem* ti_, taMisc::TypeInfoKind tik, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
protected:
  override void 	Constr_Data_Labels();
};

class TA_API taTypeInfoViewType: public taiViewType { // for TypeItem types and their spaces
INHERITED(taiViewType)
public:
  override int		BidForView(TypeDef*);
  override taiDataLink*	GetDataLink(void* data_, TypeDef* el_typ);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(taTypeInfoViewType, taiViewType)
};//




/*
class TA_API taTypeInfoDataLink: public taiDataLink { // DataLink for TypeDef objects
INHERITED(taiDataLink)
public:
  TypeDef*		data() {return (TypeDef*)m_data;} //
  TypeDef*		data() const {return (TypeDef*)m_data;} //

  override String	GetName() const;
  override TypeDef*	GetDataTypeDef() const;
  override String	GetDisplayName() const;
  USING(inherited::ShowMember)
  override bool		ShowMember(MemberDef* md); // asks this type if we should show the md member

  taTypeDefDataLink(TypeDef* data_, taDataLink* &link_ref_);  //
  DL_FUNS(taTypeDefDataLink); //

protected:
  override iDataPanel* 	CreateDataPanel_impl(); 
  override taiTreeDataNode* CreateTreeDataNode_impl(MemberDef* md, taiTreeDataNode* nodePar,
    iTreeView* tvPar, taiTreeDataNode* after, const String& node_name, int dn_flags);
};


class TA_API taiTypeDefDataNode: public taiTreeDataNode { // node for spaces, ex. enumspace, typespace, etc.
  INHERITED(taiTreeDataNode)
  public:
  
    USING(inherited::data)
    taPtrList_impl* 	data() {return ((taTypeSpaceDataLink_Base*)m_link)->data();}
    taTypeInfoDataLink* 	child_link(int idx);
    taTypeSpaceDataLink_Base* 	link() const {return (taTypeSpaceDataLink_Base*)m_link;}

    taiTypeDefDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, taiTreeDataNode* parent_,
                            taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
    taiTypeDefDataNode(taTypeSpaceDataLink_Base* link_, MemberDef* md, iTreeView* parent_,
                            taiTreeDataNode* last_child_, const String& tree_name, int flags_ = 0);
    ~taiTypeDefDataNode();
  public: // IDataLinkClient interface
    override void*	This() {return (void*)this;}
    override TypeDef*	GetTypeDef() const {return &TA_taiTypeDefDataNode;}
  protected:
    //override void	willHaveChildren_impl(bool& will) const;
    //override void 	CreateChildren_impl(); // called by the Node when it needs to create its children
    //void			CreateListItem(taiTreeDataNode* par_node, taiTreeDataNode* after_node, void* el);
  private:
    void			init(taTypeSpaceDataLink_Base* link_, int flags_); // #IGNORE
};


class TA_API iTypeDefBrowseViewer: public iBrowseViewer { // viewer window used for type browsing
  Q_OBJECT
  INHERITED(iBrowseViewer)
  friend class ClassBrowser;
  public:

    ClassBrowseViewer*		browser() {return (ClassBrowseViewer*)m_viewer;}

    iTypeDefBrowseViewer(ClassBrowseViewer* browser_, QWidget* parent = 0);
    ~iTypeDefBrowseViewer();

  public slots:
    virtual void		mnuNewBrowser(taiAction* mel); // called from context 'New Browse from here'; cast obj to taiNode*
};


class TA_API taTypeDefViewType: public taiViewType { // for TypeDef types 
  INHERITED(taiViewType)
  public:
    override int		BidForView(TypeDef*);
    override taiDataLink*	GetDataLink(void* data_, TypeDef* el_typ);
    void			Initialize() {}
    void			Destroy() {}
    TA_VIEW_TYPE_FUNS(taTypeDefViewType, taiViewType)
};

*/

#endif

// Copyright, 1995-2007, Regents of the University of Colorado,
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


// InterViews interface for groups

#ifndef TA_QTGROUP_H
#define TA_QTGROUP_H

#include "ta_stdef.h"
#include "ta_qttype_def.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_group.h"
#include "ta_qttype.h"

// externals
class taDoc; //
class QUrl;  // #IGNORE

//////////////////////////
// 	Edit Buttons	//
//////////////////////////

class TA_API gpiListEditButton : public taiEditButton {
public:
  gpiListEditButton(void* base, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
  // display the number of items in the list in the label
};

class TA_API gpiGroupEditButton : public taiEditButton {
public:
  gpiGroupEditButton(void* base, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  void		SetLabel();
  // display the number of items in the group in the label
};

// this one sets the name of the sub group..
class TA_API gpiSubEditButton : public taiEditButton {
public:
  String 	label;
  gpiSubEditButton(void* base, const char* nm,
	TypeDef* tp, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
};

// link groups don't have the option to create or xfer
class TA_API gpiLinkEditButton : public gpiGroupEditButton {
public:
  gpiLinkEditButton(void* base, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		GetMethMenus();	// exclude certain methods here..
};

// link lists don't have the option to create or xfer
class TA_API gpiListLinkEditButton : public gpiListEditButton {
public:
  gpiListLinkEditButton(void* base, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		GetMethMenus();	// exclude certain methods here..
};

class TA_API gpiArrayEditButton : public taiEditButton {
public:
  gpiArrayEditButton(void* base, TypeDef* tp, IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
  // display the number of items in the group in the label
};



//////////////////////////
//     Element Menus 	//
//////////////////////////

class TA_API taiListElsButtonBase : public taiItemPtrBase {
// for items in a list
INHERITED(taiItemPtrBase)
public:
  inline taBase*		item() const {return (taBase*)m_sel;}
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override
  int			viewCount() const {return 1;} // override
  const String		viewText(int index) const; // override

  taBase*			GetValue() {return item();}
  
  taiListElsButtonBase(TypeDef* typ_, IDataHost* host,
		       taiData* par, QWidget* gui_parent_, int flags_ = 0,
		       const String& flt_start_txt = "");
protected:
  
//nn  const String		itemTag() {return "Token: ";}
  const String		labelNameNonNull() const;

  virtual int 		BuildChooser_0(taiItemChooser* ic, TABLPtr top_lst, 
				       QTreeWidgetItem* top_item);
  // we use this recursively, and also in gpi guy
};

class TA_API taiListElsButton : public taiListElsButtonBase {
// for items in a list
INHERITED(taiListElsButtonBase)
public:
  TABLPtr		list;
  
  override const String	titleText();

  USING(inherited::GetImage)
  void			GetImage(TABLPtr base_lst, taBase* it);
  
  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  taiListElsButton(TypeDef* typ, IDataHost* host, taiData* par,
		   QWidget* gui_parent_, int flags_ = 0,
		   const String& flt_start_txt = ""); //note: typ is type of list
};


class TA_API taiGroupElsButton : public taiListElsButtonBase {
// for items in a list
INHERITED(taiListElsButtonBase)
public:
  taGroup_impl* 	grp;
  
  override const String	titleText();
  USING(inherited::GetImage)
  void			GetImage(taGroup_impl* base_grp, taBase* it);
  
  void			BuildChooser(taiItemChooser* ic, int view = 0); // override

  taiGroupElsButton(TypeDef* typ, IDataHost* host, taiData* par,
		    QWidget* gui_parent_, int flags_ = 0,
		    const String& flt_start_txt = ""); //note: typ is type of grp
protected:
  int 			BuildChooser_1(taiItemChooser* ic, taGroup_impl* top_grp, 
    QTreeWidgetItem* top_item); // we use this recursively, and also in gpi guy
};


class TA_API gpiListEls : public taiElBase {
  // #OBSOLETE menu of elements in the list 
  Q_OBJECT
public:
  bool		over_max;	// if over max_menu
  TABLPtr	ths;

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void  GetMenu(taiActions* menu, taiMenuAction* actn = NULL); // variant provided for MenuGroup_impl in winbase
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);

  QWidget*	GetRep();
  void		GetImage(TABLPtr base_lst, taBase* it);
  taBase*		GetValue();

  gpiListEls(taiActions::RepType rt, int ft, TABLPtr lst, TypeDef* tp, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk
  gpiListEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk

public slots:
  virtual void	Edit();		// edit callback
  virtual void	Choose();	// chooser callback

protected:
  virtual void	GetMenu_impl(TABLPtr lst, taiActions* menu, taiMenuAction* actn = NULL);
};

class TA_API gpiGroupEls : public gpiListEls {
  // menu of elements in the group
  Q_OBJECT
public:
  gpiGroupEls(taiActions::RepType rt, int ft, TABLPtr lst, TypeDef* tp, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
  gpiGroupEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp = NULL, IDataHost* host_ = NULL, taiData* par = NULL,
      QWidget* gui_parent_ = NULL, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk

//  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}

public slots:
  virtual void	ChooseGp();	// chooser callback

protected:
  override void		GetMenu_impl(TABLPtr cur_lst, taiActions* menu, taiMenuAction* actn = NULL);
};

class TA_API gpiSubGroups : public taiElBase {
  // menu of sub-groups within a group
  Q_OBJECT
public:
  TAGPtr	ths;
  bool		over_max;	// if over max_menu
//  TAGPtr	chs_obj;	// object chosen by the chooser

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void	GetMenu(taiActions* menu, taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);

  QWidget*	GetRep();
  void		GetImage(TAGPtr base_gp, TAGPtr gp);
  TAGPtr	GetValue();

  gpiSubGroups(taiActions::RepType rt, int ft, TAGPtr gp, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
  gpiSubGroups(taiMenu* existing_menu, TAGPtr gp, TypeDef* typ_ = NULL, IDataHost* host_ = NULL, taiData* par = NULL,
      QWidget* gui_parent_ = NULL, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
public slots:
  virtual void	Edit();		// edit callback
  virtual void	Choose();	// chooser callback

protected:
  virtual void	GetMenu_impl(TAGPtr gp, taiActions* menu, taiMenuAction* actn = NULL);
};

// TypeHier provides the guts, we just replace the NULL default with "Group"
class TA_API gpiElTypes : public taiTypeHier {
INHERITED(taiTypeHier)
public:
  TypeDef*	lst_typd;	// typedef of the list

  gpiElTypes(taiActions::RepType rt, int ft, TypeDef* lstd, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // no flags
  gpiElTypes(taiMenu* existing_menu, TypeDef* lstd, TypeDef* typ_, IDataHost* host_,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // no flags

  USING(inherited::GetMenu)
  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void		GetMenu(taiActions* menu, taiMenuAction* nact = NULL);
};

//////////////////////////////////
// 	 gpi Dialogs		//
//////////////////////////////////

class TA_API gpiMultiEditDataHost: public taiEditDataHost {
INHERITED(taiEditDataHost)
public:
  QWidget*		multi; // outer container
    QScrollArea*	scrMulti;		// scrollbars for the multi items
    QHBoxLayout*	lay_multi; // used by gpiGroupDialog to add its group buttons
    iEditGrid*		multi_body;

  gpiMultiEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  gpiMultiEditDataHost() 				{ };

  override void	ClearBody_impl();	// clear body data for reshowing
  void		AddMultiRowName(int row, const String& name, const String& desc)
    {taiEditDataHost::AddMultiRowName(multi_body, row, name, desc);} // adds a label item in first column of multi data area
  void		AddMultiColName(int col, const String& name, const String& desc)
    {taiEditDataHost::AddMultiColName(multi_body, col, name, desc);} // adds descriptive column text to top of a multi data item
  void		AddMultiData(int row, int col, QWidget* data)
    {taiEditDataHost::AddMultiData(multi_body, row, col, data);}  // add a data item in the multi-data area -- expands if necessary
  override void		SetMultiSize(int rows, int cols); //note: can never shrink
protected:
  int		multi_rows;
  int		multi_cols;
  bool		header_row; // set false if you don't want the header row
  override void Constr_Box(); // add the multi box
  override void 	Constr_Body();
  void			RebuildMultiBody(); // calls clear/constr, disabling updates
  virtual void		Constr_MultiBody(); // added in after Constr_Body -- also used for reshowing multi-body
  virtual void		ClearMultiBody_impl(); // clears multi-body for reshowing
};


class TA_API gpiList_ElData {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*	typ;
  taBase*		cur_base;
  Member_List	memb_el; // members
  taiDataList   data_el; // data elements for members

  gpiList_ElData(TypeDef* tp, taBase* base);
  virtual ~gpiList_ElData();
};


class TA_API gpiList_ElDataList : public taPtrList<gpiList_ElData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (gpiList_ElData*)it; } //

public:
  ~gpiList_ElDataList()        { Reset(); } //
};


class TA_API gpiListDataHost : public gpiMultiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(gpiMultiEditDataHost)
public:
  gpiList_ElDataList 	lst_data_el;	// list of data elements
  TABLPtr		cur_lst;
//  MemberSpace		lst_membs;	// list of members
  String_PArray		lst_membs; // list of members, by name
  int			num_lst_fields; // number of editble list memberdefs with fields

  gpiListDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  gpiListDataHost() 				{ };
  ~gpiListDataHost(); //

protected:
  override void		GetImage_Membs();
  override void		GetValue_Membs();
  override void		ClearMultiBody_impl(); // clears multi-body for reshowing

  override void		Constr_Strings();
  override void		Constr_MultiBody();
  virtual void		Constr_ElData(); 
  virtual void  	Constr_ListData();  	// construct list members themselves
  virtual void  	Constr_ListLabels();  	// construct list labels themselves
  override void 	Constr_Final(); //TEMP
};


class TA_API gpiCompactList_ElData {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*	typ;
  taBase*		cur_base;
  taiData*	data_el;	// data element provided by the typ->it

  gpiCompactList_ElData(TypeDef* tp, taBase* base, taiData* data_el = NULL);
  virtual ~gpiCompactList_ElData();
};


class TA_API gpiCompactList_ElDataList : public taPtrList<gpiCompactList_ElData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (gpiCompactList_ElData*)it; } //

public:
  ~gpiCompactList_ElDataList()        { Reset(); } //
};


class TA_API gpiCompactListDataHost : public gpiMultiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS compact vertical list for when the els have an inline rep
INHERITED(gpiMultiEditDataHost)
public:
  TABLPtr		cur_lst;
  gpiCompactList_ElDataList	lst_data_el;	// list of (inline) data elements

  gpiCompactListDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  gpiCompactListDataHost() 				{ };
  ~gpiCompactListDataHost();

  bool		ShowMember(MemberDef* md) const;

protected:
  override void		GetImage_Membs();
  override void		GetValue_Membs();
  override void		ClearMultiBody_impl(); // clears multi-body for reshowing

  override void		Constr_Strings();
  override void		Constr_MultiBody();
  virtual void		Constr_ElData(); 
  virtual void  	Constr_ListData();  	// construct list members themselves
  override void 	Constr_Final(); //TEMP
};



//////////////////////////////////
// 	gpiArray Edits		//
//////////////////////////////////

class TA_API gpiArrayEditDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
INHERITED(taiEditDataHost)
public:
  bool 		ShowMember(MemberDef* md) const;

  gpiArrayEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  gpiArrayEditDataHost() 		{ };
  ~gpiArrayEditDataHost();
protected:
  int			array_set; // index of array_set
  override void 	GetValue_Membs();
  override void 	GetImage_Membs();
  override void 	ClearBody_impl();
  override void		Constr_Data_Labels();
  virtual void		Constr_AryData_Labels();
};


//////////////////////////////////
// 	SArgEditDataHost	//
//////////////////////////////////

class TA_API SArgEditDataHost : public gpiArrayEditDataHost {
  // ##NO_INSTANCE
INHERITED(gpiArrayEditDataHost)
public:
  bool 		ShowMember(MemberDef* md) const;

  SArgEditDataHost(void* base, TypeDef* tp, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  SArgEditDataHost() 	{ };
protected:
  override void		Constr_AryData();
};

//////////////////////////////////
//  DocEditDataHost		//
//////////////////////////////////

class TA_API DocEditDataHost: public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit data host for editing raw HTML/wiki text
INHERITED(taiEditDataHost)
  Q_OBJECT
public:
  QTextEdit*		tedHtml; // r/w Html tab
    
  taDoc*		doc() const; // just returns cast of base
  
  override QWidget*	firstTabFocusWidget();

  DocEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
		  bool modal_ = false, QObject* parent = 0);
  DocEditDataHost() {init();} // just for instance
  ~DocEditDataHost() {}
protected:
  override void		Constr_Body();
  override void 	Constr_Box(); // add the docs box
  override void 	ClearBody_impl();
  override void 	GetValue_Membs();
  override void 	GetImage_Membs();

  override bool 	eventFilter(QObject* obj, QEvent* event);

private:
  void init();
};

class TA_API iDocEditDataPanel: public iDataPanelFrame {
  // a panel frame for editing doc raw HTML/wiki source text
  Q_OBJECT
INHERITED(iDataPanelFrame)
public:
  DocEditDataHost*	de; // the doc editor
  
  taDoc*		doc() {return (m_link) ? (taDoc*)(link()->data()) : NULL;}
  override String	panel_type() const {return "Doc Source";}

  override bool		HasChanged(); // 'true' if user has unsaved changes
  void			FillList();

  override QWidget*	firstTabFocusWidget();

  iDocEditDataPanel(taiDataLink* dl_);
  ~iDocEditDataPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iDocEditDataPanel;}
  override bool		ignoreDataChanged() const;

protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};

class TA_API tabDocViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabDocViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

#endif // ta_group_iv.h


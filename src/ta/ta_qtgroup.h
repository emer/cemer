// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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

class TA_API gpiListEls : public taiElBase {
  // menu of elements in the list
  Q_OBJECT
public:
  bool		over_max;	// if over max_menu
  TABLPtr	ths;

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void  GetMenu(taiActions* menu, taiMenuAction* actn = NULL); // variant provided for MenuGroup_impl in winbase
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);

  QWidget*	GetRep();
  void		GetImage(TABLPtr base_lst, TAPtr it);
  TAPtr		GetValue();

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
public:
  TypeDef*	lst_typd;	// typedef of the list

  gpiElTypes(taiActions::RepType rt, int ft, TypeDef* lstd, TypeDef* typ_, IDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // no flags
  gpiElTypes(taiMenu* existing_menu, TypeDef* lstd, TypeDef* typ_, IDataHost* host_,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // no flags

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_actions, actn);}
  override void		GetMenu(taiActions* menu, taiMenuAction* nact = NULL);
};

class TA_API gpiNewFuns : public taiData {
  // functions to call during New
public:
  static gpiNewFuns* 	CondNew(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0);
      // return an instance if there are any functions, else returns NULL;

  taiDataList	funs;
  Q3VBox* 		rep() {return (Q3VBox*)m_rep;}

  virtual void  	CallFuns(void* obj);

  gpiNewFuns(TypeDef* typ_, IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0); // no flags
};

//////////////////////////////////
// 	 gpi Dialogs		//
//////////////////////////////////

class TA_API gpiListNew : public taiDataHost {
INHERITED(taiDataHost)
public:
  // this is the function you actually call to create it..
  static TAPtr 	New(TABLPtr the_lst, int n_els = 1, TypeDef* td = NULL, QObject* parent = NULL);

  TABLPtr		ths;
  int			num;
  TypeDef*		typ;
  taiIncrField*		num_rep;
  gpiElTypes* 		typ_rep;
  gpiNewFuns*		fun_list;

  gpiListNew(TABLPtr lst, int n_els=1, TypeDef* td = NULL, bool read_only_ = false,
      bool modal_ = true, QObject* parent = NULL);
  ~gpiListNew();

  override void	ClearBody_impl();	//

public: // IDataHost
  override void GetImage();
  override void	GetValue();

protected:
  override void	Constr_Strings(const char* prompt="", const char* win_title="");
  override void	Constr_Body();
  virtual void	Constr_SubGpList()	{ }  // hook for group new
  override void	Constr_Final();
  
};

class TA_API gpiGroupNew : public gpiListNew {
public:
  // this is the function you actually call..
  static TAPtr 	New(TAGPtr the_gp, TAGPtr init_gp = NULL, int n_els=1,  TypeDef* td = NULL,
      QObject* parent = NULL);
  static TAPtr 	New(TAGPtr the_gp,  TypeDef* td, TAGPtr init_gp = NULL, int n_els=1,
      QObject* parent = NULL) {return New(the_gp, NULL, 1, td, parent);} // compatability routine for netstru.cc

  TAGPtr		in_gp;
  gpiSubGroups*	subgp_list;

  gpiGroupNew(TAGPtr gp, TAGPtr init_gp, int n_els=1, TypeDef* td = NULL,
    bool read_only_ = false, bool modal_ = true, QObject* parent = NULL);
  ~gpiGroupNew();

  override void	ClearBody_impl();	//
protected:
  override void	Constr_SubGpList();
  override void GetImage();
  override void	GetValue();
};


class TA_API gpiList_ElData {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*	typ;
  TAPtr		cur_base;
  taiDataList data_el;	// data elements

  gpiList_ElData(TypeDef* tp, TAPtr base);
  virtual ~gpiList_ElData();
};

class TA_API gpiList_ElDataList : public taPtrList<gpiList_ElData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (gpiList_ElData*)it; } //

public:
  ~gpiList_ElDataList()        { Reset(); } //
};



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
  override void Constr_Box(); // add the multi box
  override void 	Constr_Body();
  virtual void		Constr_MultiBody(); // added in after Constr_Body -- also used for reshowing multi-body
  virtual void		ClearMultiBody_impl(); // clears multi-body for reshowing
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
  ~gpiListDataHost();

  void		GetImage();
  void		GetValue();

//TODO  int		Edit();

  bool		ShowMember(MemberDef* md);

protected:
  override void		ClearMultiBody_impl(); // clears multi-body for reshowing

  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void		Constr_MultiBody();
  virtual void		Constr_ElData(); 
  virtual void  	Constr_ListData();  	// construct list members themselves
  virtual void  	Constr_ListLabels();  	// construct list labels themselves
  override void 	Constr_Final(); //TEMP
};


class TA_API gpiCompactListDataHost : public gpiMultiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS compact vertical list for when the els have an inline rep
INHERITED(gpiMultiEditDataHost)
public:
  TABLPtr		cur_lst;
  gpiList_ElDataList 	lst_data_el;	// list of data elements, only one data_el per item in list (inline)

  gpiCompactListDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  gpiCompactListDataHost() 				{ };
  ~gpiCompactListDataHost();

  void		GetImage();
  void		GetValue();

//TODO  int		Edit();

  bool		ShowMember(MemberDef* md);

protected:
  override void		ClearMultiBody_impl(); // clears multi-body for reshowing

  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void		Constr_MultiBody();
  virtual void		Constr_ElData(); 
  virtual void  	Constr_ListData();  	// construct list members themselves
  override void 	Constr_Final(); //TEMP
};



class TA_API gpiGroupDataHost : public gpiListDataHost {
public:
  taiDataList	sub_data_el;	// list of data elements for sub groups
  QWidget*	subData;
  QScrollArea*	scrSubData;
  QVBoxLayout*	laySubData;  // layout for the sub buttons

  gpiGroupDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  gpiGroupDataHost() 				{ };
  ~gpiGroupDataHost();

  override void	ClearBody_impl();	// clear body data for reshowing
  override void		GetImage();

//TODO  int		Edit();

protected:
  void		Constr_Strings(const char* prompt="", const char* win_title="");
  void		Constr_Box();
  override void	Constr_Body();
  virtual void	Constr_SubGpData(); // construct data for subgroups
};


//////////////////////////////////
// 	gpiArray Edits		//
//////////////////////////////////

class TA_API gpiArrayEditDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  int		n_ary_membs;

  bool 		ShowMember(MemberDef* md);
  void 		GetValue();
  void 		GetImage();
//TODO  int		Edit();

  gpiArrayEditDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  gpiArrayEditDataHost() 		{ };
  ~gpiArrayEditDataHost();
protected:
  override void ClearBody_impl();
  override void		Constr_Data();
  virtual void	Constr_AryData();
};


//////////////////////////////////
// 	SArgEditDataHost	//
//////////////////////////////////

class TA_API SArgEditDataHost : public gpiArrayEditDataHost {
  // ##NO_INSTANCE
public:
  bool 		ShowMember(MemberDef* md);

  SArgEditDataHost(void* base, TypeDef* tp, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  SArgEditDataHost() 	{ };
protected:
  override void		Constr_AryData();
};

class TA_API gpiSelectEditDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
  Q_OBJECT
public:
  SelectEdit*	sele;

  gpiSelectEditDataHost(void* base, TypeDef* td, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  gpiSelectEditDataHost()		{ };
  ~gpiSelectEditDataHost();

  override bool ShowMember(MemberDef* md);
  override void	GetValue();

protected:
  int 			base_items;	// #IGNORE number of base items, before the user-selected items
  QMenu*		mnuRemoveMember;  // #IGNORE we build this during body phase, but then add during method phase
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them
  override void		Constr_Body();
  void 			Constr_Methods();

  override void		FillLabelContextMenu_SelEdit(iContextLabel* sender, QMenu* menu, int& last_id);
  void			MakeMenuItem(QMenu* menu, const char* name, int index, int param, const char* slot);
  QMenu*		FindMenuItem(QMenu* par_menu, const char* label);

  override void		GetImage_impl(const MemberSpace& ms, const taiDataList& dl, void* base);
  override void		GetValue_impl(const MemberSpace& ms, const taiDataList& dl, void* base);

protected slots:
  virtual void		DoRemoveSelEdit(); // #IGNORE removes the sel_item_index item
  virtual void		mnuRemoveMember_select(int idx); // #IGNORE removes the indicated member
  virtual void		mnuRemoveMethod_select(int idx); // #IGNORE removes the indicated method
};

#endif // ta_group_iv.h


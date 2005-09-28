// Copyright (C) 1995-2005 Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
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

class gpiListEditButton : public taiEditButton {
public:
  gpiListEditButton(void* base, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
  // display the number of items in the list in the label
};

class gpiGroupEditButton : public taiEditButton {
public:
  gpiGroupEditButton(void* base, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  void		SetLabel();
  // display the number of items in the group in the label
};

// this one sets the name of the sub group..
class gpiSubEditButton : public taiEditButton {
public:
  String 	label;
  gpiSubEditButton(void* base, const char* nm,
	TypeDef* tp, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
};

// link groups don't have the option to create or xfer
class gpiLinkEditButton : public gpiGroupEditButton {
public:
  gpiLinkEditButton(void* base, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		GetMethMenus();	// exclude certain methods here..
};

// link lists don't have the option to create or xfer
class gpiListLinkEditButton : public gpiListEditButton {
public:
  gpiListLinkEditButton(void* base, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		GetMethMenus();	// exclude certain methods here..
};

class gpiArrayEditButton : public taiEditButton {
public:
  gpiArrayEditButton(void* base, TypeDef* tp, taiDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_ = 0);
  override void		SetLabel();
  // display the number of items in the group in the label
};



//////////////////////////
//     Element Menus 	//
//////////////////////////

class gpiListEls : public taiElBase {
  // menu of elements in the list
  Q_OBJECT
public:
  bool		over_max;	// if over max_menu
  TABLPtr	ths;

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  override void  GetMenu(taiMenu* menu, taiMenuAction* actn = NULL); // variant provided for MenuGroup_impl in winbase
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);

  QWidget*	GetRep();
  void		GetImage(TABLPtr base_lst, TAPtr it);
  TAPtr		GetValue();

  gpiListEls(int rt, int ft, TABLPtr lst, TypeDef* tp, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk
  gpiListEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // flags include: flgNullOk, flgNoList, flgEditOk

public slots:
  virtual void	Edit();		// edit callback
  virtual void	Choose();	// chooser callback

protected:
  virtual void	GetMenu_impl(TABLPtr lst, taiMenu* menu, taiMenuAction* actn = NULL);
};

class gpiGroupEls : public gpiListEls {
  // menu of elements in the group
  Q_OBJECT
public:
  gpiGroupEls(int rt, int ft, TABLPtr lst, TypeDef* tp, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk
  gpiGroupEls(taiMenu* existing_menu, TABLPtr gp, TypeDef* tp = NULL, taiDataHost* host_ = NULL, taiData* par = NULL,
      QWidget* gui_parent_ = NULL, int flags = 0); // uses flags: flgNullOk, flgNoGroup (aka flgNoList), flgNoInGroup, flgEditOk

//  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}

public slots:
  virtual void	ChooseGp();	// chooser callback

protected:
  override void		GetMenu_impl(TABLPtr cur_lst, taiMenu* menu, taiMenuAction* actn = NULL);
};

class gpiSubGroups : public taiElBase {
  // menu of sub-groups within a group
  Q_OBJECT
public:
  TAGPtr	ths;
  bool		over_max;	// if over max_menu
//  TAGPtr	chs_obj;	// object chosen by the chooser

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  override void	GetMenu(taiMenu* menu, taiMenuAction* actn = NULL);
  virtual void	UpdateMenu(taiMenuAction* actn = NULL);

  QWidget*	GetRep();
  void		GetImage(TAGPtr base_gp, TAGPtr gp);
  TAGPtr	GetValue();

  gpiSubGroups(int rt, int ft, TAGPtr gp, TypeDef* typ_, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
  gpiSubGroups(taiMenu* existing_menu, TAGPtr gp, TypeDef* typ_ = NULL, taiDataHost* host_ = NULL, taiData* par = NULL,
      QWidget* gui_parent_ = NULL, int flags_ = 0); // uses flags: flgNullOk, flgEditOk
public slots:
  virtual void	Edit();		// edit callback
  virtual void	Choose();	// chooser callback

protected:
  virtual void	GetMenu_impl(TAGPtr gp, taiMenu* menu, taiMenuAction* actn = NULL);
};

// TypeHier provides the guts, we just replace the NULL default with "Group"
class gpiElTypes : public taiTypeHier {
public:
  TypeDef*	lst_typd;	// typedef of the list

  gpiElTypes(int rt, int ft, TypeDef* lstd, TypeDef* typ_, taiDataHost* host_, taiData* par,
      QWidget* gui_parent_, int flags_ = 0); // no flags
  gpiElTypes(taiMenu* existing_menu, TypeDef* lstd, TypeDef* typ_, taiDataHost* host_,
      taiData* par, QWidget* gui_parent_, int flags_ = 0); // no flags

  void		GetMenu(taiMenuAction* actn = NULL) {GetMenu(ta_menu, actn);}
  override void		GetMenu(taiMenu* menu, taiMenuAction* nact = NULL);
};

class gpiNewFuns : public taiData {
  // functions to call during New
public:
  static gpiNewFuns* 	CondNew(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0);
      // return an instance if there are any functions, else returns NULL;

  taiDataList	funs;
  QVBox* 		rep() {return (QVBox*)m_rep;}

  virtual void  	CallFuns(void* obj);

  gpiNewFuns(TypeDef* typ_, taiDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_ = 0); // no flags
};

//////////////////////////////////
// 	 gpi Dialogs		//
//////////////////////////////////

class gpiListNew : public taiDataHost {
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

protected:
  override void	Constr_Strings(const char* prompt="", const char* win_title="");
  override void	Constr_Body();
  virtual void	Constr_SubGpList()	{ }  // hook for group new
  override void	Constr_Final();
  override void GetImage();
  override void	GetValue();
};

class gpiGroupNew : public gpiListNew {
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


////////////////////////////////
// 	gpiList Edits       //
////////////////////////////////

class gpiList_ElData {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS contains data_els for one member of List
public:
  TypeDef*	typ;
  TAPtr		cur_base;
  taiDataList data_el;	// data elements

  gpiList_ElData(TypeDef* tp, TAPtr base);
  virtual ~gpiList_ElData();
};

class gpiList_ElDataList : public taPtrList<gpiList_ElData> {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
protected:
  void	El_Done_(void* it)	{ delete (gpiList_ElData*)it; }

public:
  ~gpiList_ElDataList()        { Reset(); }
};

//////////////////////////////////////////////////////////
// 		gpiMultiEditDialog			//
//////////////////////////////////////////////////////////

class gpiMultiEditDataHost: public taiEditDataHost {
public:
  QWidget*	multi; // outer container
    QScrollView*	scrMulti;		// scrollbars for the multi items
    QHBoxLayout*	lay_multi; // used by gpiGroupDialog to add its group buttons
    iEditGrid*	multi_body;

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
};
//class tbScrollBox;		// #IGNORE
//class lrScrollBox;		// #IGNORE


//////////////////////////////////////////////////////////
// 		gpiListDialog				//
//////////////////////////////////////////////////////////

class gpiListDataHost : public gpiMultiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS
public:
  gpiList_ElDataList lst_data_el;	// list of data elements
  TABLPtr		cur_lst;
  MemberSpace		lst_membs;	// list of members
  int			num_lst_fields; // number of editble list memberdefs with fields

  gpiListDataHost(void* base, TypeDef* typ_, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0); //(TypeDef* tp, void* base);
  gpiListDataHost() 				{ };
  ~gpiListDataHost();

  override void	ClearBody_impl();	// clear body data for reshowing
  void		GetImage();
  void		GetValue();

//TODO  int		Edit();

  bool		ShowMember(MemberDef* md);

protected:
  override void		Constr_Strings(const char* prompt="", const char* win_title="");
  override void		Constr_Body();
  virtual void  Constr_ListData();  	// construct list members themselves
  virtual void  Constr_ListLabels();  	// construct list labels themselves
  override void Constr_Final(); //TEMP
};


////////////////////////////////
// 	taiGroup Edits      //
////////////////////////////////

class gpiGroupDataHost : public gpiListDataHost {
public:
  taiDataList	sub_data_el;	// list of data elements for sub groups
  QWidget*	subData;
  QScrollView*	scrSubData;
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

class gpiArrayEditDataHost : public taiEditDataHost {
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

class SArgEditDataHost : public gpiArrayEditDataHost {
  // ##NO_INSTANCE
public:
  bool 		ShowMember(MemberDef* md);

  SArgEditDataHost(void* base, TypeDef* tp, bool read_only_ = false,
  	bool modal_ = false, QObject* parent = 0);
  SArgEditDataHost() 	{ };
protected:
  override void		Constr_AryData();
};

class gpiSelectEditDataHost : public taiEditDataHost {
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
  QPopupMenu*		mnuRemoveMember;  // #IGNORE we build this during body phase, but then add during method phase
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them
  override void		Constr_Body();
  void 			Constr_Methods();

  override void		FillLabelContextMenu_SelEdit(iContextLabel* sender, QPopupMenu* menu, int& last_id);
  void			MakeMenuItem(QPopupMenu* menu, const char* name, int index, int param, const char* slot);
  QPopupMenu*		FindMenuItem(QPopupMenu* par_menu, const char* label);

  override void		GetImage_impl(const MemberSpace& ms, const taiDataList& dl, void* base);
  override void		GetValue_impl(const MemberSpace& ms, const taiDataList& dl, void* base);

protected slots:
  virtual void		DoRemoveSelEdit(); // #IGNORE removes the sel_item_index item
  virtual void		mnuRemoveMember_select(int idx); // #IGNORE removes the indicated member
  virtual void		mnuRemoveMethod_select(int idx); // #IGNORE removes the indicated method
};

#endif // ta_group_iv.h


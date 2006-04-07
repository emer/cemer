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


// ta_qttype_def.h: easily extendable interface support for all types

#ifndef TAI_QTTYPE_DEF_H
#define TAI_QTTYPE_DEF_H

#include "ta_type.h"
#include "ta_TA_type.h"

// externals
class taBase;
class taiData;
class IDataHost;
class taiEditDataHost;
class iColor;
class iDataPanel;
class iDataPanelFrame;
class iDataPanelSet;
class EditDataPanel;
class iDataViewer;
class taiDataLink;

class taiDataList;

class taiToken;
class taiTypeHier; //

//////////////////////////
// 	taiType 	//
//////////////////////////

class TA_API taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
public:
  static void		InitializeTypes();  // called at startup to initialize the type system
  TypeDef*		typ;		// typedef of base object
  int			bid;		// its bid
  taiTypeBase* 		m_sub_types;	// lower bid type (which has lower one, and so on) -- is cast to correct type
  String		orig_val;	// original value of the item
  bool			no_setpointer;
  // don't use SetPointer for taBase pointers (ie., for css or other secondary pointers)

  virtual TypeDef* GetTypeDef() const {return &TA_taiTypeBase;}
  virtual taiTypeBase* 	TypInst(TypeDef* td) {return new taiTypeBase(td);} // cast to correct type

  taiTypeBase(TypeDef* tp);
  taiTypeBase();
  virtual ~taiTypeBase();
private:
  void		init();
};

// macros for defining common routines and implementation

#define TAQT_TYPE_INSTANCE(x,y) x(TypeDef* tp)			\
: y(tp)		{ Initialize(); } 				\
x()		{ Initialize(); } 				\
~x()		{ Destroy(); } 					\
taiTypeBase* 	TypInst(TypeDef* td)	 			\
  { return (taiTypeBase*) new x(td); }				\
TypeDef*	GetTypeDef() const {return &TA_##x; }



//////////////////////////
// 	taiType 	//
//////////////////////////

class TA_API taiType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
public:
  virtual bool		allowsInline() const {return false;}
  virtual bool		handlesReadOnly() { return false; } // for types like taiString and taiInt whose editors handle readOnly
  virtual bool		isCompound() const {return false;} // true if requires multiple edit fields
  virtual bool		requiresInline() const {return false;}
  taiType* 		sub_types() {return (taiType*)m_sub_types;}
  taiType** 		addr_sub_types() {return (taiType**)&m_sub_types;}

  void 			AddToType(TypeDef* td);	// add an instance to a type
  virtual int		BidForType(TypeDef*) { return 1; }
  // bid for (appropriateness) for given type


  virtual taiData*	GetDataRep(IDataHost* host_, taiData* par, QWidget* gui_parent_,
  	taiType* parent_type_ = NULL, int flags = 0);
  // get taiData rep of type -- delegates to _impl of type, except if readonly and it can't handle ro; bg_color is for striping
  virtual bool		CanBrowse() {return false;} // only things from taBase classes up can be browse nodes

  virtual void		GetImage(taiData* dat, const void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void		GetValue(taiData* dat, void* base);
  // get the value from the representation -- same rules as GetDataRep

  void			Initialize();
  void			Destroy();
  TAQT_TYPE_INSTANCE(taiType, taiTypeBase);
protected:
  virtual bool		isReadOnly(taiData* dat, IDataHost* host_ = NULL); // works in both GetDataRep, passing par=dat, as well as GetImage/GetValue, passing dat=dat and dlg=NULL
  virtual taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_);
  // default behavior uses a taiField type
  virtual taiData*	GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent, int flags_);
  // default behavior same as GetDataRep_impl

  virtual void		GetImage_impl(taiData* dat, const void* base);
  // generate the gui representation of the data --  default behavior uses a taiField type
  virtual void		GetValue_impl(taiData* dat, void* base);
  // get the value from the representation --  default behavior uses a taiField type
};


//////////////////////////
//   taiViewType	//
//////////////////////////

#define TA_VIEW_TYPE_FUNS(x,y) \
x(TypeDef* tp): y(tp) {Initialize();} 			\
x()		{Initialize();} 				\
~x()		{Destroy();} 					\
taiTypeBase* 	TypInst(TypeDef* td)	 			\
  {return (taiTypeBase*) new x(td);}				\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}

class TA_API taiViewType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS viewer services for the type
#ifndef __MAKETA__
typedef taiTypeBase inherited; // #IGNORE
#endif
public:
  taiViewType* 		sub_types() {return (taiViewType*)m_sub_types;}
  taiViewType** 	addr_sub_types() {return (taiViewType**)&m_sub_types;}

  void 			AddView(TypeDef* td);	// add an instance to a type
  virtual int		BidForView(TypeDef*) {return 1;}
  virtual iDataPanel*	CreateDataPanel(taiDataLink* dl_); // creates a new data panel; normally override _impl
  virtual taiDataLink*	GetDataLink(void* data_) {return NULL;}
    // get an existing, or create new if needed
  virtual const iColor* GetEditColorInherit(taiDataLink* dl) const {return NULL;} // #IGNORE background color for edit dialog, include inherited colors from parents

  void			Initialize();
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(taiViewType, taiTypeBase)
protected:
  void 			DataPanelCreated(iDataPanelFrame* dp); // call in CreateDataPanel_impl for each dp frame created
  virtual void		CreateDataPanel_impl(taiDataLink* dl_) {} // create one or more dp's
private:
  iDataPanelFrame*	m_dp; // #IGNORE single instance created for a call
  iDataPanelSet*	m_dps; // #IGNORE created automatically if more than one datapanel is created
};



//////////////////////////
//   taiEdit		//
//////////////////////////

// offloads lots of the work to the edit panel...

class TA_API taiEdit : public taiType {
public:
  taiEdit* 		sub_types() {return (taiEdit*)m_sub_types;}
  taiEdit** 		addr_sub_types() {return (taiEdit**)&m_sub_types;}

  virtual int 		Edit(void* base=NULL, bool readonly=false, const iColor* bgcol = NULL); //note: rarely overridden
  virtual EditDataPanel* EditPanel(taiDataLink* link, void* base=NULL, bool readonly=false, const iColor* bgcol = NULL); //note: rarely overridden
  virtual int 		BidForType(TypeDef*) { return 0;}
  virtual int 		BidForEdit(TypeDef*) { return 1;}
//
  virtual void 		AddEdit(TypeDef* td); // add an edit to a type

  void			Initialize() {}
  void			Destroy() {}
  TAQT_TYPE_INSTANCE(taiEdit, taiType);
protected:
  virtual taiEditDataHost* CreateDataHost(void* base, bool readonly); // called when we need a new instance, overridden by subclasses
};


#define TAQT_EDIT_INSTANCE(x,y) x(TypeDef* tp)		\
: y(tp) 	{ Initialize();}			\
x()             { Initialize();}			\
~x()            { Destroy(); }				\
taiType* 	TypInst(TypeDef* td)			\
{ return (taiType*) new x(td); }			\
TypeDef*	GetTypeDef() const {return &TA_ ## x;}


#endif // TAI_QTTYPE_DEF_H

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

// ta_qttype_def.h: easily extendable interface support for all types

#ifndef TAI_QTTYPE_DEF_H
#define TAI_QTTYPE_DEF_H

#include "ta_type.h"
#include "ta_TA_type.h"

// externals
class taBase;
class taiData;
class taiDataHost;
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

class taiTypeBase {
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

class taiType: public taiTypeBase {
  // ##INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS graphically represents a type
public:

  virtual bool		handlesReadOnly() { return false; } // for types like taiString and taiInt whose editors handle readOnly
  taiType* 		sub_types() {return (taiType*)m_sub_types;}
  taiType** 		addr_sub_types() {return (taiType**)&m_sub_types;}

  void 			AddToType(TypeDef* td);	// add an instance to a type
  virtual int		BidForType(TypeDef*) { return 1; }
  // bid for (appropriateness) for given type


  virtual taiData*	GetDataRep(taiDataHost* host_, taiData* par, QWidget* gui_parent_,
  	taiType* parent_type_ = NULL);
  // get taiData rep of type -- delegates to _impl of type, except if readonly and it can't handle ro; bg_color is for striping
  virtual taiData*	GetDataRepEx(taiDataHost* host_, taiData* par, QWidget* gui_parent_, int or_flags_,
  	taiType* parent_type_ = NULL);
  // like GetDataRep, but lets us pass in additional flags to or -- used esp. when we want to force a read_only rep

  virtual bool		CanBrowse() {return false;} // only things from taBase classes up can be browse nodes

  virtual void		GetImage(taiData* dat, void* base);
  // generate the gui representation of the data -- same rules as GetDataRep
  virtual void		GetValue(taiData* dat, void* base);
  // get the value from the representation -- same rules as GetDataRep

  void			Initialize();
  void			Destroy();
  TAQT_TYPE_INSTANCE(taiType, taiTypeBase);
protected:
  iColor*		bg_color; // #IGNORE for when a striping bg_color passed in
  virtual bool		isReadOnly(taiData* dat, taiDataHost* host_ = NULL); // works in both GetDataRep, passing par=dat, as well as GetImage/GetValue, passing dat=dat and dlg=NULL
  virtual taiData*	GetDataRep_impl(taiDataHost* host_, taiData* par, QWidget* gui_parent, int flags_);
  // default behavior uses a taiField type

  virtual void		GetImage_impl(taiData* dat, void* base);
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

class taiViewType: public taiTypeBase {
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

class taiEdit : public taiType {
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

// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/PDP++
//
//   TA/PDP++ is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   TA/PDP++ is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.



// pdp_qt.h : Qt and Inventor stuff for pdp

#ifndef pdp_qt_h
#define pdp_qt_h 1

#include "pdpbase.h"
#include "spec.h"
#include "ta_qttype.h"
#include "ta_qtdata.h"
#include "ta_qtdialog.h"
//#include "array_qt.h"
#include "t3viewer.h"
#include "pdp_TA_type.h"


// forwards this file
class pdpDataViewer;

class PDP_API taiProgVarType : public taiClassType { 
INHERITED(taiClassType)
public:
  bool		requiresInline() const {return true;}
  bool		handlesReadOnly() { return true; } 
  int		BidForType(TypeDef* td);

  TAQT_TYPE_INSTANCE(taiProgVarType, taiClassType);
protected:
  taiData*	GetDataRepInline_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
};


class PDP_API taiSpecMember : public taiMember {
  // special for the spec type member (adds the unique box)
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  void		GetMbrValue(taiData* dat, void* base, bool& first_diff);

  void		CmpOrigVal(taiData* dat, void* base, bool& first_diff);

  virtual bool	NoCheckBox(IDataHost* host_) const; // deterimine if check box should be used

  TAQT_MEMBER_INSTANCE(taiSpecMember, taiMember);
protected:
  override taiData*	GetDataRep_impl(IDataHost* host_, taiData* par, QWidget* gui_parent_, int flags_);
  override void		GetImage_impl(taiData* dat, const void* base);
};


class PDP_API taiProgVarBase: public taiCompData { 
  //note: this set of classes uses a static New instead of new because of funky virtual Constr
INHERITED(taiCompData)
  Q_OBJECT
public:
  QWidget*	rep() const { return (QWidget*)m_rep; } 
  bool		fillHor() {return true;} // override 
  ~taiProgVarBase();

  void			Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void  	GetImage(const ProgVar* var);
  virtual void	 	GetValue(ProgVar* var) const;

protected:
  int			m_changing; // used to prevent recursions
  taiToggle*		tglIgnore;
  taiField*		fldName;
  
  virtual void		Constr_impl(QWidget* gui_parent_, bool read_only_); //override
  override void		GetImage_impl(const void* base) {GetImage((const ProgVar*)base);}
  override void		GetValue_impl(void* base) const {GetValue((ProgVar*)base);} 
  taiProgVarBase(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
};


class PDP_API taiProgVar: public taiProgVarBase { 
  //note: this set of classes uses a static New instead of new because of funky virtual Constr
INHERITED(taiProgVarBase)
  Q_OBJECT
public:
  static taiProgVar*	New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
  
  ~taiProgVar();

  void  		GetImage(const ProgVar* var);
  void	 		GetValue(ProgVar* var) const;

protected: 
  taiVariant*		vfVariant;
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); //override
  taiProgVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


class PDP_API taiEnumProgVar: public taiProgVarBase {
INHERITED(taiProgVarBase)
  Q_OBJECT
public:
  static taiEnumProgVar* New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
  ~taiEnumProgVar();

  void  		GetImage(const ProgVar* var); // override
  void	 		GetValue(ProgVar* var) const; // override

protected:
  taiTypeHier*		thEnumType;
  taiComboBox*		cboEnumValue;
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); 
  void			DataChanged_impl(taiData* chld); // override
  taiEnumProgVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


class PDP_API taiObjectProgVar: public taiProgVarBase {
INHERITED(taiProgVarBase)
  Q_OBJECT
public:
  static taiObjectProgVar* New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);

  void  		GetImage(const ProgVar* var); // override
  void	 		GetValue(ProgVar* var) const; // override
  ~taiObjectProgVar();

protected:
  taiTypeHier*		thValType;
  taiToggle*		chkMakeNew;
  QLabel*		lblObjectValue;
  taiToken*		tkObjectValue;
  
  void			MakeNew_Setting(bool value); // common code jigs visibility
  
  void			Constr_impl(QWidget* gui_parent_, bool read_only_); 
  void			DataChanged_impl(taiData* chld); // override
  taiObjectProgVar(TypeDef* typ_, IDataHost* host, taiData* par, QWidget* gui_parent_, int flags = 0);
};


//////////////////////////
//   ipdpDataViewer	//
//////////////////////////

class PDP_API ipdpDataViewer: public iT3DataViewer { //#IGNORE
friend class pdpDataViewer;
public:

  pdpDataViewer*	viewer() {return (pdpDataViewer*)m_viewer;}

  ipdpDataViewer(void* root_, TypeDef* typ_, pdpDataViewer* viewer_,
      QWidget* parent = NULL);
  ~ipdpDataViewer();

protected:
//  override void 	SetActionsEnabled_impl();
};

//////////////////////////
//   pdpDataViewer	//
//////////////////////////

class PDP_API pdpDataViewer: public T3DataViewer { // #INSTANCE master viewer for network, i/o, etc.
INHERITED(T3DataViewer)
public:
  static pdpDataViewer*	New(ProjectBase* proj); // populates all elements (all layers, prjns, i/o, etc.)

  ProjectBase*		proj() {return (ProjectBase*)m_data;} // aka data
  ipdpDataViewer*	net_win() {return (ipdpDataViewer*)m_window;}

  TA_BASEFUNS(pdpDataViewer)
protected:
  override void		Constr_Window_impl();  // #IGNORE implement this to set the m_window instance
private:
  void		Initialize();
  void		Destroy();
};


#endif // pdp_qt_h

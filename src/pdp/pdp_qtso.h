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
#include "ta_qtbrowse.h"
//#include "array_qt.h"
#include "t3viewer.h"
#include "pdp_TA_type.h"


// forwards this file
class pdpDataViewer;
class iPdpDataBrowser;
class PdpDataBrowser;

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


class PDP_API taiProgVar: public taiCompData { 
  //note: this set of classes uses a static New instead of new because of funky virtual Constr
INHERITED(taiCompData)
  Q_OBJECT
public:
  static taiProgVar*	New(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
  
  QWidget*		rep() const { return (QWidget*)m_rep; } 
  inline int		varType() const {return vt;} // current vartype selected by control
  bool			fillHor() {return true;} // override 
  ~taiProgVar();

  void			Constr(QWidget* gui_parent_); // inits a widget, and calls _impl within InitLayout-EndLayout calls
  virtual void  	GetImage(const ProgVar* var);
  virtual void	 	GetValue(ProgVar* var) const;

protected:
  enum StackControls { // #IGNORE indexes of controls in the stack
    scInt,
    scField, // string and most numbers
    scToggle, // bool
    scBase, // taBase
    scEnum, // regular Enum
    scDynEnum
  };
  mutable int		m_updating; // used to prevent recursions
  int 			vt; //ProgVar::VarType
  
  taiField*		fldName;
  taiComboBox*		cmbVarType;
  QStackedWidget*	stack; // holds the subfields for different types
  
  taiIncrField*		incVal; // for: ints
  taiField*		fldVal; // for: char, string, most numbers
  taiToggle*		tglVal; // for: bool
  
  // for standard enums:
  taiTypeHier*		thEnumType;
  taiComboBox*		cboEnumValue;
  //for objects:
  taiTypeHier*		thValType;
  QLabel*		lblObjectValue;
  taiToken*		tkObjectValue;
  // for DynEnums:
  taiEditButton*	edDynEnum; // for invoking editor for values
  taiComboBox*		cboDynEnumValue;
  
  void			SetVarType(int value); // ProgVar::VarType
  virtual void		Constr_impl(QWidget* gui_parent_, bool read_only_); //override
  void			UpdateDynEnumCombo(const ProgVar* var); 
  void			DataChanged_impl(taiData* chld); // override -- used for Enum and Object
  override void		GetImage_impl(const void* base) {GetImage((const ProgVar*)base);}
  override void		GetValue_impl(void* base) const {GetValue((ProgVar*)base);} 
  taiProgVar(TypeDef* typ_, IDataHost* host, taiData* par, 
    QWidget* gui_parent_, int flags = 0);
    
protected slots:
  void			cmbVarType_itemChanged(int itm);
};


class PDP_API iPdpDataBrowser: public iDataBrowser { // viewer window used for class browsing of taBase objects
friend class PdpDataBrowser;
    Q_OBJECT
INHERITED(iDataBrowser)
public:
  ProjectBase*		curProject(); // the Project at root of what user has selected; Null if n/a
  bool		showFileObjectOps() {return true;} // override
  
  ~iPdpDataBrowser(); //

public: // slot overrrides
  void 		fileNew(); //override
  void 		fileOpen(); //override
  void 		fileSave(); //override
  void 		fileSaveAs(); //override
  void 		fileSaveAll(); //override
  void 		fileClose(); //override
  
protected:
  iPdpDataBrowser(taBase* root_, MemberDef* md_, TypeDef* typ_, PdpDataBrowser* browser_,
    QWidget* parent = 0);
  void 			Constr_Menu_impl(); // override
};


class TA_API PdpDataBrowser : public DataBrowser {
  // #NO_TOKENS the base type for objects with a top-level window or panel and a menu
INHERITED(DataBrowser)
friend class iPdpDataBrowser;
public:

  iPdpDataBrowser*		browser_win() {return (iPdpDataBrowser*)m_window;}
  
  PdpDataBrowser(taBase* root_, MemberDef* md_, bool is_root = false); 
    // use this one
  TA_BASEFUNS(PdpDataBrowser)
protected:
  override void		Constr_Window_impl(); // #IGNORE
private:
  void			Initialize() {}
  void			Destroy() {}
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

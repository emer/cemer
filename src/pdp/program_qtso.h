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

// program_qtso.h -- executables, gui files

#ifndef PROGRAM_QTSO_H
#define PROGRAM_QTSO_H

#include "ta_group.h"
#include "ta_script.h"

#include "program.h"

#include "ta_qtdata.h"
#include "ta_qtdialog.h"
#include "ta_qttype.h"
#include "ta_qtviewer.h"

#include "pdp_TA_type.h"

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
  enum StackControls { // #IGNORE current control
    scNone, // used at the beginning
    scInt,
    scField, // string and most numbers
    scToggle, // bool
    scBase, // taBase
    scEnum, // regular Enum
    scDynEnum
  };
  mutable int		m_updating; // used to prevent recursions
  int			sc; // current stack control 
  int 			vt; //ProgVar::VarType
  
  taiField*		fldName;
  taiComboBox*		cmbVarType;
  QWidget*		stack; // holds subfields for different types, gets deleted/recreated
  
  taiIncrField*		incVal; // for: ints
  taiField*		fldVal; // for: char, string, most numbers
  taiToggle*		tglVal; // for: bool
  
  // for standard enums:
  taiEnumTypeDefButton*	thEnumType;
  taiComboBox*		cboEnumValue;
  //for objects:
  taiTypeDefButton*	thValType;
  taiTokenPtrButton*	tkObjectValue;
  // for DynEnums:
  taiEditButton*	edDynEnum; // for invoking editor for values
  taiComboBox*		cboDynEnumValue;
  
  void			AssertControls(int value); // uncreates existing, creates new
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
private:
  void			init();
};


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


class TA_API tabProgramViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabProgramViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API iProgramEditor: public QWidget, public IDataHost {
  // widget for editing entire programs
INHERITED(QWidget)
  Q_OBJECT
public:
  
  IDataHost* 		outer_host; // YOU MUST SET THIS OR THE UNIVERSE WILL COLLAPSE
  
  QVBoxLayout*		layOuter;
  QHBoxLayout*		  layEdit;
  QWidget*		    widEdit; // container for the actual taiEdit
  HiLightButton*	    btnSave;
  HiLightButton*	    btnRevert;
  QTreeWidget*		  tw;
  
  bool			read_only; // set true if we are
  
  void			setEditNode(TAPtr value); // sets the object to show editor for

  iProgramEditor(QWidget* parent = NULL); //

public: // ITypedObject i/f
  void*			This() {return this;} 
  TypeDef*		GetTypeDef() const {return &TA_iProgramEditor;}
public: // IDataHost i/f -- some delegate up to mommy
  const iColor* 	colorOfCurRow() const {return &bg_color;} // only need one
  bool  		HasChanged() {return modified;}	
  bool			isConstructed() {return true;}
  bool			isModal() {return false;} // never for us
  bool			isReadOnly() {return read_only;}
  void*			Base(); // base of the object
  TypeDef*		GetBaseTypeDef(); // TypeDef on the base, for casting
  void			GetValue();
  void			GetImage();
  void			Changed(); // called by embedded item to indicate contents have changed
  void			SetItemAsHandler(taiData* item, bool set_it = true) {} 


protected:
  iColor		bg_color; // for edit area; only need one, because doesn't change
  taSmartRef		m_edit_node; // the current node being edited; its it is in widEdit
  bool			modified;
  
  void			InternalSetModified(bool value); // does all the gui config
  
protected slots:
  void			btnSave_clicked();
  void			btnRevert_clicked();
  
private:
  void			init();
};


class TA_API iProgramPanel: public iDataPanelFrame {
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  iProgramEditor*	pe;
  
  Program*		prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String	panel_type() const; // this string is on the subpanel button for this panel

  override int 		EditAction(int ea);
  void			FillList();
  override int		GetEditActions(); // after a change in selection, update the available edit actions (cut, copy, etc.)
  void			GetSelectedItems(ISelectable_PtrList& lst); // list of the selected cells

  void			AddedToPanelSet(); // override -- called when fully added to DataPanelSet
  
  iProgramPanel(taiDataLink* dl_);
  ~iProgramPanel();

public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iProgramPanel;}
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
//  override int 		EditAction_impl(taiMimeSource* ms, int ea, ISelectable* single_sel_node = NULL);

/*protected slots: */
};



#endif

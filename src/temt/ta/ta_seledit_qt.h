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

// ta_seledit_qt.h -- select edit, gui files

#ifndef TA_SELEDIT_QT_H
#define TA_SELEDIT_QT_H

#include "ta_seledit.h"

#include "ta_qtgroup.h"
#include "ta_qtviewer.h"
#include "ta_qtdialog.h"

#ifndef __MAKETA__
# include <QPointer>
#endif

#include "ta_TA_type.h"


class TA_API tabSelectEditViewType: public tabOViewType {
INHERITED(tabOViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabSelectEditViewType, tabOViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};


class TA_API iSelectEditDataHost : public taiEditDataHost {
  // ##NO_TOKENS ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditDataHost)
  Q_OBJECT
public:
  SelectEdit*	sele;

  override void		Constr_Body();
  
  iSelectEditDataHost(void* base, TypeDef* td, bool read_only_ = false,
  	QObject* parent = 0);
  iSelectEditDataHost()	{ Initialize();};
  ~iSelectEditDataHost();

protected:
  override void		ClearBody_impl();	// we also clear all the methods, and then rebuild them
  void 			Constr_Methods();

  override void		FillLabelContextMenu_SelEdit(iLabel* sender, QMenu* menu, int& last_id);
//obs  void			MakeMenuItem(QMenu* menu, const char* name, int index, int param, const char* slot);
//obs  QMenu*		FindMenuItem(QMenu* par_menu, const char* label);

  override void		Constr_Data_Labels(); 
  override void 	GetImage_Membs_def();
  override void 	GetValue_Membs_def();

protected slots:
  virtual void		DoRemoveSelEdit(); // #IGNORE removes the sel_item_index item
  virtual void		mnuRemoveMember_select(int idx); // #IGNORE removes the indicated member
  virtual void		mnuRemoveMethod_select(int idx); // #IGNORE removes the indicated method
private:
  void	Initialize();
};




class TA_API iSelectEditPanel: public iDataPanelFrame {
// ##NO_CSS note: SelectEdit main panel (for users)
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  iSelectEditDataHost*	se;
  SelectEdit*		sele() {return (m_link) ? (SelectEdit*)(link()->data()) : NULL;}
  
  override bool		HasChanged(); // 'true' if user has unsaved changes

  override String	panel_type() const {return "Select Edit";}

  iSelectEditPanel(taiDataLink* dl_);
  ~iSelectEditPanel();
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iSelectEditPanel;}
  override bool		ignoreDataChanged() const;
  
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};

#endif

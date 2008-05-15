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

// ta_program_qt.h -- executables, gui files

#ifndef TA_PROGRAM_QT_H
#define TA_PROGRAM_QT_H

#include "ta_group.h"
#include "ta_script.h"

#include "ta_program.h"

#include "ta_qtgroup.h"
#include "ta_qtviewer.h"
#include "ta_qtdialog.h"

#ifndef __MAKETA__
# include <QPointer>
#endif

#include "ta_TA_type.h"

class TA_API taiDynEnumMember : public taiMember {
  // an int member with #DYNENUM_ON_xxx flag indicating DynEnumType guy
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);

  TAQT_MEMBER_INSTANCE(taiDynEnumMember, taiMember);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);

  bool		isBit; // true if a bits type enum
private:
  void		Initialize();
  void 		Destroy() {};
};

class TA_API taiProgVarIntValMember : public taiMember {
  // the int_val member in a ProgVar -- switches from plain int to hard_enum
public:
  int		BidForMember(MemberDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);

  TAQT_MEMBER_INSTANCE(taiProgVarIntValMember, taiMember);
protected:
  override void GetImage_impl(taiData* dat, const void* base);
  override void	GetMbrValue_impl(taiData* dat, void* base);

private:
  void		Initialize();
  void 		Destroy() {};
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

class TA_API tabProgramGroupViewType: public tabGroupViewType {
INHERITED(tabGroupViewType)
public:
  override int		BidForView(TypeDef*);
  void			Initialize() {}
  void			Destroy() {}
  TA_VIEW_TYPE_FUNS(tabProgramGroupViewType, tabGroupViewType) //
protected:
//nn  override taiDataLink*	CreateDataLink_impl(taBase* data_);
  override void		CreateDataPanel_impl(taiDataLink* dl_);
};

class TA_API iProgramEditor: public QWidget, public virtual IDataHost, 
			     public virtual IDataLinkClient {
  // #NO_CSS widget for editing entire programs
INHERITED(QWidget)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum CustomEventType { // note: just copied from taiDataHost, not all used
    CET_RESHOW		= QEvent::User + 1,  // uses ReShowEvent
    CET_GET_IMAGE,
    CET_APPLY
  };
#endif

  QVBoxLayout*		layOuter;
  QScrollArea*		  scrBody;
  iStripeWidget*	  body; // container for the actual taiData items
  iMethodButtonMgr*	    meth_but_mgr; // note: not a widget
//QHBoxLayout*		  layButtons;
  HiLightButton*	    btnApply;
  HiLightButton*	    btnRevert;
  iTreeView*		  items;
  
  bool			read_only; // set true if we are
#ifndef __MAKETA__ 
  QPointer<iMainWindowViewer> m_window; // set this so cliphandler can be set for controls
#endif 
  int			editLines() const; // number of edit lines (typ 4) 
  void			setEditNode(TAPtr value, bool autosave = true); // sets the object to show editor for; autosaves previous if requested
  void 			setEditBgColor(const iColor& value); // set bg for edit, null for default
  void			defEditBgColor(); // set default color
  void			setShow(int value); // only used by expert toggle
  virtual void		Refresh(); // manual refresh

  iProgramEditor(QWidget* parent = NULL); //
  ~iProgramEditor();

public slots:
  void			Apply();
  void			Revert();
  void			ExpandAll(); // expands all, and resizes columns
  
public: // ITypedObject i/f
  void*			This() {return this;} 
  TypeDef*		GetTypeDef() const {return &TA_iProgramEditor;}
  
public: // IDataLinkClient i/f 
  void			DataLinkDestroying(taDataLink* dl); 
  void			DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // IDataHost i/f -- some delegate up to mommy
  const iColor	 	colorOfCurRow() const; // #IGNORE 
  bool  		HasChanged() {return m_modified;}	
  bool			isConstructed() {return true;}
  bool			isModal() {return false;} // never for us
  bool			isReadOnly() {return read_only;}
  taMisc::ShowMembs 	show() const {return (taMisc::ShowMembs)m_show;} 
    // used by polydata
  iMainWindowViewer* 	window() const;
  void*			Root() const {return (void*)base;} // base of the object
  taBase*		Base() const {return base;} // root of the object, if a taBase 
  TypeDef*		GetRootTypeDef() const; // TypeDef on the base, for casting
  void			GetValue();
  void			GetImage();
  void			Changed(); // called by embedded item to indicate contents have changed
  void			Apply_Async();


protected:
  int			ln_sz; // const, the line size, without margins
  int			ln_vmargin; // const, margin, typ 1 
  int 			line_ht; // const, size of each stripe
  int			m_changing; // for suppressing spurious notifies
  iColor		bg_color; // for edit area
  iColor		bg_color_dark; // for edit area
  bool			m_modified;
  bool			warn_clobber; // set if we get a notify and are already modified
  bool			apply_req;
  TAPtr			base; // no need for smartref, because we are a dlc
  MembSet_List		membs; // the member items, one set per line
  
  int 			row;
  int			m_show;
  taiData*		sel_item_dat; // used (and only valid!) for context menus
 
  override void 	customEvent(QEvent* ev_);
  virtual void		Base_Remove(); // removes base and deletes the current set of edit controls
  virtual void		Base_Add(); // adds controls etc for base
  bool			ShowMember(MemberDef* md);
  
  void			InternalSetModified(bool value); // does all the gui config
  void 			UpdateButtons();
  
protected slots:
  void			label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e); // note, it MUST have this name
  void			items_Notify(ISelectableHost* src, int op); // note: NULL if none
  void 			DoSelectForEdit(QAction* act);
  
private:
  void			Init();
};


class TA_API iProgramPanelBase: public iDataPanelFrame {
// ##NO_CSS note: ProgramPanel and ProgramGroupPanel are almost identical
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  iProgramEditor*	pe;
  
  override bool		HasChanged_impl(); // 'true' if user has unsaved changes
  void			FillList();

  iProgramPanelBase(taiDataLink* dl_);
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
//  override TypeDef*	GetTypeDef() const {return &TA_iProgramPanel;}
  
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2); //
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
  
protected slots:
  void			mb_Expert(bool checked); // expert button on minibar
};


class TA_API iProgramPanel: public iProgramPanelBase {
INHERITED(iProgramPanelBase)
  Q_OBJECT
public:
  Program*		prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String	panel_type() const {return "Edit Program";}

  void			FillList();

  iProgramPanel(taiDataLink* dl_);
  
public: // IDataLinkClient interface
//  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iProgramPanel;}
  
protected:
  override void		OnWindowBind_impl(iTabViewer* itv);
  
protected slots:
  void			items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};

class TA_API iProgramGroupPanel: public iProgramPanelBase {
INHERITED(iProgramPanelBase)
  Q_OBJECT
public:
  Program_Group*	progGroup() {return (m_link) ? 
    (Program_Group*)(link()->data()) : NULL;}
  override String	panel_type() const {return "Program Params";} 

  void			FillList();

  iProgramGroupPanel(taiDataLink* dl_);
  
public: // IDataLinkClient interface
//  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iProgramGroupPanel;}
  
protected slots:
  void			items_CustomExpandFilter(iTreeViewItem* item,
    int level, bool& expand);
};


TA_API void ProgramToolBoxProc(iToolBoxDockViewer* tb); // fills toolbox


class TA_API iProgramToolBar: public iToolBar {
INHERITED(iToolBar)
public:
  iProgramToolBar(ToolBar* viewer, QWidget* parent = NULL)
  :iToolBar(viewer, parent){}
protected:
  override void		Constr_post(); 
};

class TA_API iProgramCtrlDataHost : public taiEditDataHost, public virtual IRefListClient {
  // ##IGNORE ##NO_CSS ##NO_MEMBERS edit only selected items from a range of ta-base objects
INHERITED(taiEditDataHost)
  Q_OBJECT
public: //
// We use sets: 0:name/desc; 1: gp guys (ex step); set2: args; set3: vars
  enum CtrlMembSets {
    MS_PROG,
    MS_GP,
    MS_ARGS,
    MS_VARS, // note: must ARGS..VARS must be in numerical sequence
    
    MS_CNT	= 4, 
  };
  
  inline Program*	prog() const {return (Program*)root;}

  
  iProgramCtrlDataHost(Program* base, bool read_only_ = false,
		       bool modal_ = false, QObject* parent = 0);
  iProgramCtrlDataHost()		{ };
  ~iProgramCtrlDataHost();

  override bool ShowMember(MemberDef* md) const; //

protected: //
  // we maintain several lists to simply mgt and notify handling
  taBase_RefList	refs; // the data members from the Program
  taBase_RefList	refs_struct; // structural guys: arg and var lists themselves, gp
  override void		Enum_Members();
  override void		Constr_Data_Labels();
  override void 	Cancel_impl(); 
  override MemberDef*	GetMemberPropsForSelect(int sel_idx, taBase** base,
    String& lbl, String& desc); // (use sel_item_idx) enables things like ProgCtrl to play
  override void		GetValue_Membs_def();
  override void		GetImage_Membs();

  void	UpdateDynEnumCombo(taiComboBox* cb, const ProgVar* var); 

 
public: // IRefListClient i/f
  TYPED_OBJECT(iProgramCtrlDataHost);
  override void		DataDestroying_Ref(taBase_RefList* src, taBase* ta);
  override void		DataChanged_Ref(taBase_RefList* src, taBase* ta,
    int dcr, void* op1, void* op2);
};

class TA_API iProgramCtrlPanel: public iDataPanelFrame {
// ##NO_CSS panel for program control panel
INHERITED(iDataPanelFrame)
  Q_OBJECT
public:
  iProgramCtrlDataHost*	pc;
  
  Program*		prog() {return (m_link) ? (Program*)(link()->data()) : NULL;}
  override String	panel_type() const {return "Program Ctrl";}

  override bool		HasChanged_impl(); // 'true' if user has unsaved changes
  void			FillList();

  iProgramCtrlPanel(taiDataLink* dl_);
  ~iProgramCtrlPanel();
  
public: // IDataLinkClient interface
  override void*	This() {return (void*)this;}
  override TypeDef*	GetTypeDef() const {return &TA_iProgramCtrlPanel;}
  
protected:
  override void		DataChanged_impl(int dcr, void* op1, void* op2);
  override void		OnWindowBind_impl(iTabViewer* itv);
  override void		UpdatePanel_impl();
  override void		ResolveChanges_impl(CancelOp& cancel_op);
};

////////////////////////////////////////////////////////
//  Special ProgLib browser support

class TA_API taiProgLibElsButton : public taiListElsButton {
// for prog lib items
INHERITED(taiListElsButton)
public:
  int			columnCount(int view) const; // override
  const String		headerText(int index, int view) const; // override

  taiProgLibElsButton(TypeDef* typ, IDataHost* host, taiData* par,
		      QWidget* gui_parent_, int flags_ = 0); //note: typ is type of list
protected:
  override void		BuildCategories_impl();
  override int 		BuildChooser_0(taiItemChooser* ic, TABLPtr top_lst, 
				       QTreeWidgetItem* top_item);
};

class TA_API taiProgLibElArgType : public gpiFromGpArgType {
  // for ProgLibEl* ptr args
public:
  int 		BidForArgType(int aidx, TypeDef* argt, MethodDef* md, TypeDef* td);
  taiData*	GetDataRep_impl(IDataHost* host_, taiData* par,
    QWidget* gui_parent_, int flags_, MemberDef* mbr_);
  void		GetImage_impl(taiData* dat, const void* base);
  void		GetValue_impl(taiData* dat, void* base);

  TAQT_ARGTYPE_INSTANCE(taiProgLibElArgType, gpiFromGpArgType);
private:
  void		Initialize() {}
  void		Destroy() {}
};

#endif

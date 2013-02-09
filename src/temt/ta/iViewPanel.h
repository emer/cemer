// Copyright, 1995-2013, Regents of the University of Colorado,
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

#ifndef iViewPanel_h
#define iViewPanel_h 1

// parent includes:
#include <iPanelBase>
#include <IWidgetHost>

// member includes:
#include <taiWidget_List>

// declare all other types mentioned but not required to include:
class iHiLightButton; //
class iViewPanelSet; //
class taDataView; //
class QBoxLayout; //
class QKeyEvent; //


taTypeDef_Of(iViewPanel);

class TA_API iViewPanel: public iPanelBase, public virtual IWidgetHost {
  // frame for gui interface to a view element (view control panel) -- usually posted by the view, and locked -- provides optional IWidgetHost and Apply/Revert services, so you can use taiWidget ctrls
  Q_OBJECT
  INHERITED(iPanelBase)
  friend class iViewPanelSet;

public:
  enum ViewPanelFlags { // #BITS misc flags
    VP_0                = 0, // #IGNORE
    VP_USE_BTNS         = 0x0001, // use the Apply/Revert buttons on bottom
  };

#ifndef __MAKETA__
  enum CustomEventType { // note: just copied from taiEditorWidgetsMain, not all used
    CET_RESHOW          = QEvent::User + 1,  // uses ReShowEvent
    CET_GET_IMAGE,
    CET_APPLY
  };
#endif

//QHBoxLayout*          layButtons;
  iHiLightButton*          btnApply; //note: only if created
  iHiLightButton*          btnRevert;
  iHiLightButton*          btnCopyFrom;

  int                   vp_flags;
  bool                  read_only; // set true if we are

  taDataView*           dv() {return m_dv;} // can be statically replaced with subclass
  override bool         lockInPlace() const {return true;}
    // true if panel should not be replaced, ex. if dirty, or viewpanel
  override taiSigLink* par_link() const {return NULL;} // n/a
  override MemberDef*   par_md() const {return NULL;}
  override iPanelViewer*  tabViewerWin() const;
  override bool         isViewPanelFrame() const {return true;}


  override void         ClearDataPanelSet() {m_dps = NULL;}
  void                  MakeButtons(QBoxLayout* par_lay = NULL, QWidget* par_widg = NULL);
    // make the Apply/Revert btns, par=this if NULL, if no lay, use par_widg
  override void         InitPanel(); // we do a more elaborate check for m_dv and !updating
  override void         UpdatePanel(); // ditto
  override void         ClosePanel();
  override String       TabText() const; // text for the panel tab -- usually just the text of the sel_node

  iViewPanel(taDataView* dv_);
    // NOTE: dv will be nulled out if it destroys
  ~iViewPanel();

public slots:
  void                  Apply();
  void                  Revert();
  void                  CopyFrom();

public: // ISigLinkClient interface
  override void*        This() {return (void*)this;} //
  override void         SigLinkDestroying(taSigLink* dl); //note: dl is on the view, not underlying data
  override TypeDef*     GetTypeDef() const {return &TA_iViewPanel;}

public: // IWidgetHost i/f -- some delegate up to mommy
  const iColor          colorOfCurRow() const; // #IGNORE probably not used, we just return our own bg color
  bool                  HasChanged() {return m_modified;}
  bool                  isConstructed() {return true;}
  bool                  isModal() {return false;} // never for us
  bool                  isReadOnly() {return read_only;}
  TypeItem::ShowMembs   show() const;
    // used by polydata
  iMainWindowViewer*    window() const {return (tabView()) ? tabView()->viewerWindow() : NULL;}
  void*                 Root() const {return (void*)m_dv;} // (typical, could replace)
  taBase*               Base() const {return m_dv;} // (typical, could replace)
  TypeDef*              GetRootTypeDef() const {return (m_dv) ? m_dv->GetTypeDef() : NULL;} // (could replace)
  void                  GetValue(); // does setup, override the impl
  void                  GetImage() {UpdatePanel();}
public slots:
  void                  Changed(); // called by embedded item to indicate contents have changed
  void                  Apply_Async();

protected:
  taiWidget_List           dl; // add any taiWidget guys you make to this, so they get deleted
  iViewPanelSet*        m_dps; // set if we are in a viewpanelset
  taDataView*           m_dv;
  int                   updating; // #IGNORE >0 used to suppress update-related widget signals
  bool                  m_modified;
  bool                  apply_req;
  bool                  warn_clobber; // set if we get a notify and are already modified

  override void         customEvent(QEvent* ev_);
  override void         keyPressEvent(QKeyEvent* e);

  virtual void          CopyFrom_impl() {}
  virtual void          GetValue_impl() {}
  void                  InternalSetModified(bool value); // does all the gui config
  override void         ResolveChanges_impl(CancelOp& cancel_op); //generic behavior
  void                  UpdateButtons();
};

#endif // iViewPanel_h

// Copyright 2013-2017, Regents of the University of Colorado,
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

  int            vp_flags;
  bool           read_only; // set true if we are

  taDataView*    dv() {return m_dv;} // can be statically replaced with subclass
  bool           lockInPlace() const override {return true;} // true if panel should not be replaced, ex. if dirty, or viewpanel
  taiSigLink*    par_link() const override {return NULL;} // n/a
  MemberDef*     par_md() const override {return NULL;}
  iPanelViewer*  tabViewerWin() const override;
  bool           isViewPanelFrame() const override {return true;}


  void           ClearDataPanelSet() override {m_dps = NULL;}
  void           MakeButtons(QBoxLayout* par_lay = NULL, QWidget* par_widg = NULL);
    // make the Avert btns, par=this if NULL, if no lay, use par_widg
  void           InitPanel() override; // we do a more elaborate check for m_dv and !updating
  void           UpdatePanel() override; // ditto
  void           ClosePanel() override;
  String         TabText() const override; // text for the panel tab -- usually just the text of the sel_node

  iViewPanel(taDataView* dv_);
    // NOTE: dv will be nulled out if it destroys
  ~iViewPanel();

public slots:
  void           Apply();
  void           Revert();
  void           CopyFrom();

public: // ISigLinkClient interface
  void*         This() override {return (void*)this;} //
  void          SigLinkDestroying(taSigLink* dl) override; //note: dl is on the view, not underlying data
  TypeDef*      GetTypeDef() const override {return &TA_iViewPanel;}

public: // IWidgetHost i/f -- some delegate up to mommy
  const iColor  colorOfCurRow() const override; // #IGNORE probably not used, we just return our own bg color
  const iColor  backgroundColor() const override { return colorOfCurRow(); }
  const iColor  backgroundColorDark() const override { return colorOfCurRow(); }
  bool          HasChanged() override {return m_modified;}
  bool          isConstructed() override {return true;}
  bool          isModal() override {return false;} // never for us
  bool          isReadOnly() override {return read_only;}
  iMainWindowViewer*    window() const {return (tabView()) ? tabView()->viewerWindow() : NULL;}
  void*         Root() const override {return (void*)m_dv;} // (typical, could replace)
  taBase*       Base() const override {return m_dv;} // (typical, could replace)
  TypeDef*      GetRootTypeDef() const override {return (m_dv) ? m_dv->GetTypeDef() : NULL;} // (could replace)
  void          GetValue() override; // does setup, override the impl
  void          GetImage() override {UpdatePanel();}
public slots:
  void          Changed() override; // called by embedded item to indicate contents have changed
  void          Apply_Async() override;

protected:
  taiWidget_List dl; // add any taiWidget guys you make to this, so they get deleted
  iViewPanelSet* m_dps; // set if we are in a viewpanelset
  taDataView*   m_dv;
  int           updating; // #IGNORE >0 used to suppress update-related widget signals
  bool          m_modified;
  bool          apply_req;
  bool          warn_clobber; // set if we get a notify and are already modified

  void          customEvent(QEvent* ev_) override;
  void          keyPressEvent(QKeyEvent* e) override;

  virtual void  CopyFrom_impl() {}
  virtual void  GetValue_impl() {}
  void          InternalSetModified(bool value); // does all the gui config
  void          ResolveChanges_impl(CancelOp& cancel_op) override; //generic behavior
  void          UpdateButtons();
};

#endif // iViewPanel_h

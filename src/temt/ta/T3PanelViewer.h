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

#ifndef T3PanelViewer_h
#define T3PanelViewer_h 1

// parent includes:
#include <FrameViewer>

// member includes:
#include <T3Panel_List>

// declare all other types mentioned but not required to include:
class T3Panel; //
class iT3PanelViewer; //  #IGNORE
class iTabBarBase; //  #IGNORE
class T3DataView; //


taTypeDef_Of(T3PanelViewer);

class TA_API T3PanelViewer : public FrameViewer {
  // ##DEF_NAME_ROOT_T3Frames ##DEF_NAME_STYLE_2 #AKA_T3DataViewer top-level FrameViewer object that manges tabs of panels that contain 3D views
INHERITED(FrameViewer)
friend class T3DataView;
public:
  static T3Panel* GetBlankOrNewT3Panel(taBase* obj);
    // gets the first blank, or a new, frame, in the default proj browser for the obj -- used by all the view-creating routines for various viewable objs (tables, nets, etc.)

  T3Panel_List  panels; // #AKA_frames the panels associated with each tab

  bool         isRootLevelView() const CPP11_OVERRIDE {return true;}
  iT3PanelViewer*         widget(); // #IGNORE
  iTabBarBase*          tabBar(); // #IGNORE

  virtual T3DataView*   FindRootViewOfData(taBase* data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual T3Panel* FirstEmptyT3Panel(); // find the first panel with no contents (to avoid making a new one)
  virtual T3Panel* NewT3Panel(); // #MENU #MENU_ON_Object #MENU_CONTEXT create and map a new panel
  void                  PanelChanged(T3Panel* panel);
    // called by panel if changes, we update names

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const T3PanelViewer& cp);
  TA_DATAVIEWFUNS(T3PanelViewer, FrameViewer)

protected:
  // from taDataView
  void         DoActionChildren_impl(DataViewAction act) CPP11_OVERRIDE; // just one act

  // taViewer
  void         Constr_impl(QWidget* gui_parent) CPP11_OVERRIDE;
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) CPP11_OVERRIDE; // #IGNORE
//void         ResolveChanges(CancelOp& cancel_op) CPP11_OVERRIDE; // nn: no "dirty" for T3
  void         Reset_impl() CPP11_OVERRIDE; //  #IGNORE
  void         GetWinState_impl() CPP11_OVERRIDE; // set gui state; only called if mapped
  void         SetWinState_impl() CPP11_OVERRIDE; // fetch gui state; only called if mapped

  virtual void          ConstrPanels_impl();

private:
  void                  Initialize();
  void                  Destroy();
};

#endif // T3PanelViewer_h

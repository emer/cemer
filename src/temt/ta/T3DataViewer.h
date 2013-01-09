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

#ifndef T3DataViewer_h
#define T3DataViewer_h 1

// parent includes:
#include <FrameViewer>

// member includes:

// declare all other types mentioned but not required to include:

class TA_API T3DataViewer : public FrameViewer {
  // ##DEF_NAME_ROOT_T3Frames ##DEF_NAME_STYLE_2 top-level taDataViewer object that contains one 3D data view of multiple objects
INHERITED(FrameViewer)
friend class T3DataView;
public:
  static T3DataViewFrame* GetBlankOrNewT3DataViewFrame(taBase* obj);
    // gets the first blank, or a new, frame, in the default proj browser for the obj -- used by all the view-creating routines for various viewable objs (tables, nets, etc.)

  T3DataViewFrame_List  frames; //

  override bool         isRootLevelView() const {return true;}
  inline iT3DataViewer* widget() {return (iT3DataViewer*)inherited::widget();} // lex override

  iTabBarBase*          tabBar() { if(!widget()) return NULL; return widget()->tabBar(); }

  virtual T3DataView*   FindRootViewOfData(taBase* data); // looks for a root view of the data, returns it if found; useful to check for existing view before adding a new one

  virtual T3DataViewFrame* FirstEmptyT3DataViewFrame(); // find the first frame with no contents (to avoid making a new one)
  virtual T3DataViewFrame* NewT3DataViewFrame(); // #MENU #MENU_ON_Object #MENU_CONTEXT create and map a new frame
  void                  FrameChanged(T3DataViewFrame* frame);
    // called by frame if changes, we update names

  void  InitLinks();
  void  CutLinks();
  void  Copy_(const T3DataViewer& cp);
  TA_DATAVIEWFUNS(T3DataViewer, FrameViewer)

protected:
  // from taDataView
  override void         DataChanged_Child(taBase* child, int dcr, void* op1, void* op2);
  override void         DoActionChildren_impl(DataViewAction act); // just one act

  // DataViewer
  override void         Constr_impl(QWidget* gui_parent);
  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
//override void         ResolveChanges(CancelOp& cancel_op); // nn: no "dirty" for T3
  override void         Reset_impl(); //  #IGNORE
  override void         GetWinState_impl(); // set gui state; only called if mapped
  override void         SetWinState_impl(); // fetch gui state; only called if mapped

  virtual void          ConstrFrames_impl();

private:
  void                  Initialize();
  void                  Destroy();
};

#endif // T3DataViewer_h

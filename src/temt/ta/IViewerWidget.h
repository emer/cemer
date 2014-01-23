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

#ifndef IViewerWidget_h
#define IViewerWidget_h 1

// parent includes:

// member includes:
#include <taiMiscCore>

// declare all other types mentioned but not required to include:
class QWidget; // 
class taViewer; // 
class iMainWindowViewer; // 
class QCloseEvent; //

class TA_API IViewerWidget { // interface that all taViewer::widget() class must implement
friend class taViewer;
public:
  virtual bool          isDirty() const {return false;}
  virtual QWidget*      widget() = 0; // return the widget
  taViewer*           viewer() {return m_viewer;} // often lexically overridden to strongly type
  virtual iMainWindowViewer* viewerWindow() const; // the main window in which this widget lives

  void                  Constr() {Constr_impl();} // called virtually, after new, override impl
  void                  Close(); // deletes us, and disconects us from viewer -- YOU MUST NOT MAKE ANY CALLS TO OBJ AFTER THIS

//  inline operator QWidget()   {return &(widget());} // enables convenient implicit conversion
  void                  Refresh(); // manual refresh (brackets with Busy)
  virtual void          ResolveChanges(CancelOp& cancel_op) {ResolveChanges_impl(cancel_op);}
  // called from viewer prior to close; should normally autosave unsaved changes
  virtual void          SaveData() {}

  IViewerWidget(taViewer* viewer);
  virtual ~IViewerWidget(); // informs mummy of our destruction

protected:
  taViewer*           m_viewer; // our mummy
  mutable iMainWindowViewer* m_window; // can be set by creator, or dyn looked up/cached
  virtual void          closeEvent_Handler(QCloseEvent* e,
    CancelOp def_cancel_op = CO_PROCEED);
    // default says "proceed", delegates decision to viewer; call with CO_NOT_CANCELLABLE for unconditional
  virtual void          Constr_impl() {} // for virtual const CPP11_OVERRIDEruction (called after new)
  virtual void          Constr_post() {} // called virtually, in DV::Constr_post -- entire win struct is now available
  virtual void          OnClosing_impl(CancelOp& cancel_op); // invoked in dtor (uncancellable); you should also invoke in the closeEvent (maybe cancellable)
  virtual void          Refresh_impl() {}
  virtual void          ResolveChanges_impl(CancelOp& cancel_op) {}
};

#endif // IViewerWidget_h

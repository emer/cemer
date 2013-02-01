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

#ifndef iFrameViewer_h
#define iFrameViewer_h 1

// parent includes:
#ifndef __MAKETA__
#include <QWidget>
#endif

#include <IDataViewWidget>

// member includes:
#include <FrameViewer>

// declare all other types mentioned but not required to include:
class ISelectableHost; //


class TA_API iFrameViewer: public QWidget, public IDataViewWidget {
// ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for things in the center splitter of main
  Q_OBJECT
INHERITED(QWidget)
friend class FrameViewer;
friend class MainWindowViewer;
friend class iMainWindowViewer;
public:

  virtual int           stretchFactor() const {return 2;} // helps set sensible alloc of space in split
  inline FrameViewer*   viewer() {return (FrameViewer*)m_viewer;} // usually lex overridden in subclass
  MainWindowViewer*     mainWindowViewer();

//nn??  virtual void            UpdateTabNames(); // called by a siglink when a tab name might have changed
  virtual void          GetWinState() {GetWinState_impl();} // when saving view state
  virtual void          SetWinState() {SetWinState_impl();} // when showing, from view state

  iFrameViewer(FrameViewer* viewer_, QWidget* parent = NULL);
  ~iFrameViewer();

public slots:
  void          SelectableHostNotifySlot_Internal(ISelectableHost* src, int op);
    // connects guys nested below us; lets us trap, and forward
  void          SelectableHostNotifySlot_External(ISelectableHost* src, int op);
   // from external guys (forwarded from main window)

#ifndef __MAKETA__
signals:
  void          SelectableHostNotifySignal(ISelectableHost* src, int op);
    // forwarder, from all internal guys
#endif

public: // IDataViewerWidget i/f
  override QWidget*     widget() {return this;}
protected:
//  override void               Constr_impl(); // called virtually, after new
  virtual void          GetWinState_impl() {} // when saving view state
  virtual void          SetWinState_impl() {} // when showing, from view state

protected:
  short int             shn_changing; // for marking forwarding, so we don't reflect back

  void                  hideEvent(QHideEvent* e); // override
  void                  showEvent(QShowEvent* e); // override
  virtual void          Showing(bool showing); // #IGNORE called by the show/hide handlers
  virtual void          SelectionChanged_impl(ISelectableHost* src_host) {}
    // we call this when we receive a valid incoming change, or maybe new focus

private:
  void                  Init();
};

#endif // iFrameViewer_h

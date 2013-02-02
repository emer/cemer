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

#ifndef iT3Panel_h
#define iT3Panel_h 1

// parent includes:
#ifndef __MAKETA__
#include <QWidget>
#endif
#include <IDataViewWidget>

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif

// declare all other types mentioned but not required to include:
class iT3ViewspaceWidget; //
class T3ExaminerViewer; //
class iT3TabViewer; //
class T3Panel; //
class iViewPanelSet; //
class iViewPanelFrame; //
class T3DataViewRoot; //
class T3Node; //
class T3DataView; //
class SoNode; //


class TA_API iT3Panel : public QWidget, public IDataViewWidget {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS panel widget that contains 3D data views
  Q_OBJECT
INHERITED(QWidget)
friend class T3Panel;
friend class iT3TabViewer;
friend class T3TabViewer;
public:
#ifndef __MAKETA__
  QPointer<iViewPanelSet> panel_set; // contains ctrl panels for all our guys
#endif
//  taiWidgetMenu*            fileExportInventorMenu;

  iT3ViewspaceWidget*   t3vs;

  T3ExaminerViewer*     t3viewer() {return m_t3viewer;}

  T3DataViewRoot*       root();
  virtual void          setSceneTop(SoNode* node); // set top of scene -- usually called during Render_post
  override int          stretchFactor() const {return 4;} // 4/2 default
  inline T3Panel* viewer() const {return (T3Panel*)m_viewer;}
  iT3TabViewer*        viewerWidget() const;

  void                  NodeDeleting(T3Node* node);
    // called when a node is deleting; basically used to deselect

  virtual void          T3DataViewClosing(T3DataView* node); // used mostly to remove from selection list
  void                  RegisterPanel(iViewPanelFrame* pan);

  iT3Panel(T3Panel* viewer_, QWidget* parent = NULL);
  ~iT3Panel(); //

public: // menu and menu overrides
  void                  viewRefresh(); // rebuild/refresh the current view

public slots:
  virtual void          fileExportInventor();
  virtual void          viewSaved(int view_no); // connect to viewSaved on examiner viewer

public: // IDataViewWidget i/f
  override QWidget*     widget() {return this;}
protected:
  override void         Constr_impl();
  override void         Refresh_impl(); // note: we just do the lite Render_impl stuff

protected:
  T3ExaminerViewer*     m_t3viewer;
  virtual void          Render_pre(); // #IGNORE
  virtual void          Render_impl();  // #IGNORE
  virtual void          Render_post(); // #IGNORE
  virtual void          Reset_impl(); // note: delegated from DataViewer::Clear_impl

protected:
//  void                        hideEvent(QHideEvent* ev);
  void                  showEvent(QShowEvent* ev);
  void                  Showing(bool showing);

private:
  void                  Init();
}; //


#endif // iT3Panel_h

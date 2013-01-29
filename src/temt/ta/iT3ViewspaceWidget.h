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

#ifndef iT3ViewspaceWidget_h
#define iT3ViewspaceWidget_h 1

// parent includes:
#include "ta_def.h"
#ifndef __MAKETA__
#include <QWidget>
#endif
#include <ISelectableHost>

// member includes:
#include <SoPtr>
#include <taDataView>

// declare all other types mentioned but not required to include:
class SoSeparator; //
class T3ExaminerViewer; //
class iT3DataViewFrame; //
class SoNode; //
class iSoSelectionEvent; //
class QScrollBar; //
class SoPath; //


class TA_API iT3ViewspaceWidget: public QWidget, public ISelectableHost { // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS widget that encapsulates an Inventor viewer; adds context menu handling, and optional scroll bars
  Q_OBJECT
#ifndef __MAKETA__
typedef QWidget inherited;
#endif
friend class T3Viewspace;
public:
  enum SelectionMode {
    SM_NONE,
    SM_SINGLE,
    SM_MULTI
  };

  SoSelectionPtr        sel_so; //#IGNORE

  SoSeparator*          root_so() {return m_root_so.ptr();} // always valid -- you can add your own lights, camera, etc.
  QScrollBar*           horScrollBar(bool auto_create = false);
  void                  setHasHorScrollBar(bool value);
  QScrollBar*           verScrollBar(bool auto_create = false);
  void                  setHasVerScrollBar(bool value);

  SelectionMode         selMode() {return m_selMode;}
    // #IGNORE true adds a SoSelection node, and selection call back
  void                  setSelMode(SelectionMode); // #IGNORE true adds a SoSelection node, and selection call back

  T3ExaminerViewer*     t3viewer() {return m_t3viewer;}
  void                  setT3viewer(T3ExaminerViewer* value); // must be called once, after creation
  void                  setSceneGraph(SoNode* sg);
  void                  setTopView(taDataView* tv); // set topmost view; for show/hide -- calls SetVisible if visible on set

  void                  deleteScene(); // deletes the scene -- usually only called internally, not by clients of this component
  void                  ContextMenuRequested(const QPoint& pos); // #IGNORE called from render area

  inline iT3DataViewFrame* i_data_frame() const {return m_i_data_frame;}

  iT3ViewspaceWidget(iT3DataViewFrame* parent);
  iT3ViewspaceWidget(QWidget* parent = NULL);
  ~iT3ViewspaceWidget();

#ifndef __MAKETA__
signals:
  void                  initScrollBar(QScrollBar* sb); // orientation will be in sb
#endif

public: // ISelectableHost i/f
  override bool         hasMultiSelect() const {return true;} // always
  override QWidget*     widget() {return this;}
protected:
//  override void       EditAction_Delete(ISelectable::GuiContext gc_typ);
  override void         UpdateSelectedItems_impl();

protected:
  static void           SoSelectionCallback(void* inst, SoPath* path); // #IGNORE
  static void           SoDeselectionCallback(void* inst, SoPath* path); // #IGNORE

  QScrollBar*           m_horScrollBar;
  QScrollBar*           m_verScrollBar;
  T3ExaminerViewer*     m_t3viewer;
  SoSeparatorPtr        m_root_so; //
  SoNode*               m_scene; // actual top item set by user
  SelectionMode         m_selMode; // #IGNORE true adds a SoSelection node, and selection call back
  iT3DataViewFrame*     m_i_data_frame; // #IGNORE our parent object
  taDataViewRef         m_top_view; // #IGNORE topmost view obj, for show/hide ctrl
  signed char           m_last_vis; // #IGNORE keeps track of last hide/show state, to avoid spurious calls when several in a row: -1=hide, 0=init, 1=show

  override void         showEvent(QShowEvent* ev);
  override void         hideEvent(QHideEvent* ev);

  void                  SoSelectionEvent(iSoSelectionEvent* ev); // #IGNORE
  void                  LayoutComponents(); // called on resize or when comps change (ex scrollers)
  QScrollBar*           MakeScrollBar(bool ver);
  void                  resizeEvent(QResizeEvent* ev); // override

private:
  void                  init();
};

#endif // iT3ViewspaceWidget_h

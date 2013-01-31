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

#ifndef iProgramEditor_h
#define iProgramEditor_h 1

// parent includes:
#include <IDataHost>
#include <IDataLinkClient>

// member includes:
#include <MembSet>
#include <iColor>

#ifndef __MAKETA__
#include <QEvent>
#else
class QEvent; //
class QContextMenuEvent; //
class QAction; //
#endif

// declare all other types mentioned but not required to include:
class iBrowseHistory; //
class taiAction; //
class QVBoxLayout; //
class QScrollArea; //
class iStripeWidget; //
class iMethodButtonMgr; //
class QHBoxLayout; //
class QToolBar; //
class HiLightButton; //
class iTreeView; //
class iTreeSearch; //
class iMainWindowViewer; //
class taBase; // 
class TypeDef; // 
class taiDataLink; // 
class taDataLink; // 
class iTreeViewItem; //
class QWidget; //
class ISelectableHost; //


TypeDef_Of(iProgramEditor);

class TA_API iProgramEditor: public QWidget, public virtual IDataHost,
                             public virtual IDataLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS widget for editing entire programs
  INHERITED(QWidget)
  Q_OBJECT
public:
#ifndef __MAKETA__
  enum CustomEventType { // note: just copied from taiDataHost, not all used
    CET_RESHOW          = QEvent::User + 1,  // uses ReShowEvent
    CET_GET_IMAGE,
    CET_APPLY
  };
#endif

  iBrowseHistory*       brow_hist;
  taiAction*            historyBackAction;
  taiAction*            historyForwardAction;

  QVBoxLayout*          layOuter;
  QScrollArea*            scrBody;
  iStripeWidget*          body; // container for the actual taiData items
  iMethodButtonMgr*         meth_but_mgr; // note: not a widget
  QHBoxLayout*            layButtons;
  QToolBar*                 tb; // for the history buttons
  HiLightButton*            btnHelp;
  HiLightButton*            btnApply;
  HiLightButton*            btnRevert;
  iTreeView*              items;
  iTreeSearch*            search;
#ifndef __MAKETA__
  QPointer<QWidget>     first_tab_foc;  // first tab focus widget
#endif

  bool                  read_only; // set true if we are
#ifndef __MAKETA__
  QPointer<iMainWindowViewer> m_window; // set this so cliphandler can be set for controls
#endif
  int                   editLines() const {return m_editLines;} // number of edit lines (min 4)
  void                  setEditLines(int val);
  void                  setEditNode(taBase* value, bool autosave = true); // sets the object to show editor for; autosaves previous if requested
  void                  setEditBgColor(const iColor& value); // set bg for edit, null for default
  void                  defEditBgColor(); // set default color
  void                  setShow(int value); // only used by expert toggle
  iTreeViewItem*        AssertBrowserItem(taiDataLink* link);
  virtual void          Refresh(); // manual refresh
  virtual QWidget*      firstTabFocusWidget();

  iProgramEditor(QWidget* parent = NULL); //
  ~iProgramEditor();

public slots:
  void                  Apply();
  void                  Revert();
  void                  Help();
  void                  ExpandAll(); // expands all, and resizes columns
  void                  slot_AssertBrowserItem(taiDataLink* link)
    {AssertBrowserItem(link);}


public: // ITypedObject i/f
  void*                 This() {return this;}
  TypeDef*              GetTypeDef() const {return &TA_iProgramEditor;}

public: // IDataLinkClient i/f
  void                  DataLinkDestroying(taDataLink* dl);
  void                  DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2);

public: // IDataHost i/f -- some delegate up to mommy
  const iColor          colorOfCurRow() const; // #IGNORE
  bool                  HasChanged() {return m_modified;}
  bool                  isConstructed() {return true;}
  bool                  isModal() {return false;} // never for us
  bool                  isReadOnly() {return read_only;}
#ifndef __MAKETA__
  TypeItem::ShowMembs   show() const {return m_show;}
#endif
    // used by polydata
  iMainWindowViewer*    window() const;
  void*                 Root() const {return (void*)base;} // base of the object
  taBase*               Base() const {return base;} // root of the object, if a taBase
  TypeDef*              GetRootTypeDef() const; // TypeDef on the base, for casting
  void                  GetValue();
  void                  GetImage();
  void                  Changed(); // called by embedded item to indicate contents have changed
  void                  Apply_Async();

protected:
  int                   ln_sz; // const, the line size, without margins
  int                   ln_vmargin; // const, margin, typ 1
  int                   line_ht; // const, size of each stripe
  int                   m_editLines;
  int                   m_changing; // for suppressing spurious notifies
  iColor                bg_color; // for edit area
  iColor                bg_color_dark; // for edit area
  bool                  m_modified;
  bool                  warn_clobber; // set if we get a notify and are already modified
  bool                  apply_req;
  taBase*               base; // no need for smartref, because we are a dlc
  MembSet_List          membs; // the member items, one set per line

  int                   row;
  TypeItem::ShowMembs   m_show;
  MemberDef*            sel_item_mbr; // used (and only valid!) for context menus
  taBase*               sel_item_base; // used (and only valid!) for context menus

  override void         customEvent(QEvent* ev_);
  override bool         eventFilter(QObject *obj, QEvent *event);
  // event filter to trigger apply button on Ctrl+Return

  virtual void          Base_Remove(); // removes base and deletes the current set of edit controls
  virtual void          Base_Add(); // adds controls etc for base
  bool                  ShowMember(MemberDef* md);

  void                  InternalSetModified(bool value); // does all the gui config
  void                  UpdateButtons();
  void                  Controls_Remove(); // when Base_Remove or lines changes
  void                  Controls_Add(); // when Base_Add or lines changes

protected slots:
  void                  label_contextMenuInvoked(iLabel* sender, QContextMenuEvent* e); // note, it MUST have this name
  void                  items_Notify(ISelectableHost* src, int op); // note: NULL if none
  void                  DoSelectForEdit(QAction* act);

private:
  void                  Init();
};

#endif // iProgramEditor_h

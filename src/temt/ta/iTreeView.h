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

#ifndef iTreeView_h
#define iTreeView_h 1

// parent includes:
#include <ISelectableHost>
#ifndef __MAKETA__
#include <iTreeWidget>
#endif

// member includes:
#ifndef __MAKETA__
#include <QPointer>
#endif
#include <TypeItem>


// declare all other types mentioned but not required to include:
class iMainWindowViewer;
class String_PArray;
class iTreeViewItem;

#ifndef __MAKETA__
typedef QMap<QString, QVariant> QMap_qstr_qvar; // the QMap type that QVariant supports
// maps are always refcounted in Qt, similar to QString, so you pass them by value
#endif

class TA_API iTreeView: public iTreeWidget, public ISelectableHost {
  //  ##NO_INSTANCE ##NO_TOKENS ##NO_CSS ##NO_MEMBERS base class for all views of iTreeViewItems
INHERITED(iTreeWidget)
  Q_OBJECT
friend class iTreeViewItem;
public:
#ifndef __MAKETA__
  enum Roles { // extra roles, for additional data, etc.
    ObjDataRole = Qt::UserRole + 1, // for additional data
    ColKeyRole, // store a string in header to indicate the col key to use for data
    ColDataRole, // store a QString::QVariant map of colkey/Role numbers to add addtl data to nodes; ex, "desc":Qt::ToolTipRole for tooltip text for the col "desc"
    ColFormatRole, // stores values from the ColFormatFlags enum
    MaxColCharsRole // store an int of max col width (in chars), we elide text to that length
  };
#endif
  enum ColFormatFlags { // used to tweak formatting/display of cols
    CF_ELIDE_TO_FIRST_LINE      = 0x0001 // elide to first line, so row doesn't multi
  };

  enum TreeViewFlags { // #BITS
    TV_NONE                     = 0, // #NO_BIT
    TV_AUTO_EXPAND              = 0x0001, // invokes DefaultExpand automatically on open
    TV_AUTO_EXPANDED            = 0x0002, // #IGNORE flag marks when done first time
    TV_EXPAND_DISABLED  = 0x0004, // expands nested disabled items (usually they are not expanded unless you expand from the item itself)
    TV_NO_AUTO_RESIZE   = 0x0008 // when we autoexpand etc. doesn't resize cols
  };
  enum ContextMenuPosition {
    CM_START,           // called before filling of menu -- use to add items to start
    CM_END              // called after filling menu -- use to add items to end
  };

  static const String   opt_treefilt; // "TREEFILT_"

#ifndef __MAKETA__
  QPointer<iMainWindowViewer> main_window; // set this to the main window we belong in -- NOTE this is NOT always the same as mainWindow() and must be set specifically -- could be the program editor or another similar such thing..
  QPointer<QWidget>     focus_next_widget; // if set, this is the widget to focus next on when tab pressed
  QPointer<QWidget>     focus_prev_widget;  // if set, this is the widget to focus on when shift-tab pressed
#endif

#ifndef __MAKETA__
  static void           FillTypedList(const QList<QTreeWidgetItem*>& items,
    ISelectable_PtrList& list); // helper, for filling our own typed list

  QMap_qstr_qvar        colDataKeys(int col) const;
    // the map of role/key pairs, or an empty map if none
#endif

  bool                  useCustomExpand() const;
  bool                  doubleClickExpandsAll() const; // use at own risk...
  const KeyString       colKey(int col) const; // the key we set for data lookup
  void                  setColKey(int col, const KeyString& key);
    // sets in ColKeyRole -- you can do it yourself if you want
  int                   colFormat(int col);
  void                  setColFormat(int col, int format_flags);
  bool                  decorateEnabled() const {return m_decorate_enabled;}
  void                  setDecorateEnabled(bool value); //note: must be done at create time
  int                   defaultExpandLevels() const {return m_def_exp_levels;}
    // how many levels the DefaultExpand expands
  void                  setDefaultExpandLevels(int value)
    {m_def_exp_levels = (int)value;}
  iTreeViewItem*        item(int i) const; // item at i, NULL if out of range
  inline int            itemCount() const {return topLevelItemCount();}
  void                  setHeaderText(int col, const String& value); // convenience
  int                   maxColChars(int col); // value if set, -1 otherwise
  void                  setMaxColChars(int col, int value); // sets max number of chars for that text (when retrieved from the link); elided if greater
  TypeItem::ShowMembs   show() const {return m_show;}
  virtual void          setShow(TypeItem::ShowMembs value);
  const String          showContext() const {return m_show_context;}
  virtual void          setShowContext(const String& value)
    {m_show_context = value;}
   // ONLY SET DURING CREATE -- THIS IS NOT DYNAMIC

  inline TreeViewFlags  tvFlags() const {return (TreeViewFlags)tv_flags;}
  void                  setTvFlags(int value);

  void                  AddFilter(const String& value);
    // add a TREEFILT_xxx expression to exclude members and/or types; note: not dynamic, must be added before items created
  iTreeViewItem*        AssertItem(taiSigLink* link, bool super = true);
    // insures that the item for the link exists; returns NULL if it doesn't exist/couldn't be assertedtaMisc::
  bool                  HasFilter(TypeItem* ti) const;
    // true if the typeitem has a TREEFILT_xxx filter that was added to our list

  void                  AddColDataKey(int col, const KeyString& key, int role);
    // sets in ColDataRole, ex for a tooltip text or font for the col
  bool                  RemoveColDataKey(int col, const KeyString& key, int role);
    // removes ColDataRole; true if it was there
  void                  ClearColDataKeys(int col);
    // clears all the ColDataKeys in the col (provided for completeness)

  virtual void          Refresh() {Refresh_impl();} // manually refresh
  virtual bool          ShowNode(iTreeViewItem* item) const;
  // whether the node is visible in this show context
  virtual void		EmitTreeStructToUpdate();
  // emit signal that tree structure is about to be updated
  virtual void		EmitTreeStructUpdated();
  // emit signal that tree structure was updated

  /////////////////////////////////////////////////////////////////
  //            ScrollArea Management

  virtual void          SaveScrollPos();
  // save the current vertical scroll position for later restore
  virtual void          RestoreScrollPos();
  // restore the current vertical scroll position
  virtual void          ScrollTo(int scr_pos);
  // scroll vertically to given position -- directly controls vertical scroll bar
  virtual bool          PosInView(int scr_pos);
  // is given position within the main scroll area (in coordinates relative to central widget) within view?

  iTreeView(QWidget* parent = 0, int tv_flags = 0);
  ~iTreeView();

#ifndef __MAKETA__
signals:
  void                  CustomExpandFilter(iTreeViewItem* item, int level, bool& expand);
    // invoked when we want our mummy to do custom filtering, expand=true by default
  void                  FillContextMenuHookPre(ISelectable_PtrList& sel_items,
     taiWidgetActions* menu);
    // hook to allow client to add items to start of context menu before it shows
  void                  FillContextMenuHookPost(ISelectable_PtrList& sel_items,
     taiWidgetActions* menu);
    // hook to allow client to add items to end of context menu before it shows
  void                  ItemSelected(iTreeViewItem* item);
    // NULL if none -- NOTE: the preferred way is to use ISelectableHost::Notify signal
  void                  TreeStructToUpdate();
  // structure of tree is just about to be updated (items removed or added) -- connect to this to update anything that depends on this tree view
  void                  TreeStructUpdated();
  // structure of tree is just about to be updated (items removed or added) -- connect to this to update anything that depends on this tree view
#endif

public slots:
  virtual void          mnuFindFromHere(taiAction* mel); // called from context 'Find from here'; cast obj to iTreeViewItem*
  virtual void          mnuReplaceFromHere(taiAction* mel); // called from context 'Replace from here'; cast obj to iTreeViewItem*
  virtual void          ExpandDefault();
  // expand to the default level specified for this tree, or invokes CustomExpand if set
  virtual void          ExpandAll(int max_levels = 6);
  // expand all nodes, ml=-1 for "infinite" levels (there better not be any loops!!!)
  virtual void          CollapseAll(); // collapse all nodes
  virtual void          ExpandAllUnder(iTreeViewItem* item, int max_levels = 6);
  // expand all nodes under item, ml=-1 for "infinite" levels (there better not be any loops!!!)
  virtual void          ExpandDefaultUnder(iTreeViewItem* item);
  // expand to default level under given item
  virtual void          CollapseAllUnder(iTreeViewItem* item); // collapse all nodes under item

  void                  InsertEl(bool after=false); // insert new element at or after currently selected item
  void                  InsertDefaultEl(bool after=false); // insert default new element type object at or after currently selected item

public: // ISelectableHost i/f
  override bool         hasMultiSelect() const;
  override QWidget*     widget() {return this;}
protected:
  override void         FillContextMenu_pre(ISelectable_PtrList& sel_items,
    taiWidgetActions* menu);
  override void         FillContextMenu_post(ISelectable_PtrList& sel_items,
    taiWidgetActions* menu);

  override void         UpdateSelectedItems_impl();

protected:
  enum ExpandFlags {
    EF_CUSTOM_FILTER            = 0x01,
    EF_DEFAULT                  = 0x02, // we are in the DefaultExpand context
    EF_EXPAND_DISABLED          = 0x04 // either Expand on that guy, or set in flags
  };
  int                   tv_flags;
  String_PArray*        m_filters; // only created if any added
  short                 m_def_exp_levels; // level of default expand, typically 2
  TypeItem::ShowMembs   m_show;
  bool                  m_decorate_enabled;
  String                m_show_context;
  int                   in_mouse_press; // ugly hack
  int                   m_saved_scroll_pos;

  QFont&                italicFont() const; // so we don't create a new guy each node

  override void         focusInEvent(QFocusEvent* ev); // override
  override void         mousePressEvent(QMouseEvent* ev); //for exp/coll all
  override void         mouseDoubleClickEvent(QMouseEvent* ev); //for exp/coll all
  override void         showEvent(QShowEvent* ev); // override, for expand all

  virtual void          ExpandAll_impl(int max_levels, int exp_flags = 0);
  // inner code
  virtual void          ExpandItem_impl(iTreeViewItem* item, int level, int max_levels,
                                        int exp_flags = 0);
  // inner code; level=-1 when not known
  virtual void          GetSelectedItems(ISelectable_PtrList& lst);
  // list of the selected datanodes

  override void         keyPressEvent(QKeyEvent* e);
  override bool         focusNextPrevChild(bool next);

#ifndef __MAKETA__
  override QMimeData*   mimeData(const QList<QTreeWidgetItem*> items) const;
    // we replace this and provide the ta custom mime data (not the treewidget data)
  override QStringList  mimeTypes () const; // for dnd to work, we just permit almost anything via "text/plain", then decide on the drop whether to accept
#endif
  virtual void          ItemDestroyingCb(iTreeViewItem* item);
  virtual void          Refresh_impl();
  virtual void          Show_impl();

protected slots:
  void                  this_contextMenuRequested(QTreeWidgetItem* item,
    const QPoint & pos, int col ); //note: should probably rejig to use a virtual method
  void                  this_currentItemChanged(QTreeWidgetItem* curr, QTreeWidgetItem* prev);
  void                  this_itemSelectionChanged();
  virtual void          ExpandAllUnderInt(void* item);
  virtual void          CollapseAllUnderInt(void* item);
private:
  mutable QFont*        italic_font;
};

#endif // iTreeView_h

// Copyright, 1995-2007, Regents of the University of Colorado,
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


#include "ta_qtviewer.h"

// #include "ta_type.h"
// #include "ta_qt.h"
// #include "ta_qtdata.h"
// #include "ta_qtdialog.h"
// #include "ta_qttype.h"
// #include "ta_qtclipdata.h"
// #include "ta_script.h"
// #include "ta_project.h"
// #include "ta_qtclassbrowse.h"
// #include "ta_qtgroup.h"
// #include "ta_program_qt.h"
// #include "t3viewer.h"
// #include "ta_gui.h"

// #include "css_qt.h"
// #include "css_qtdialog.h"
// #include "css_machine.h"
// #include "css_ta.h"

// #include "icolor.h"
// #include "inetworkaccessmanager.h"

// #include <QAction>
// #include <QApplication>
// #include <qbuttongroup.h>
// #include <qclipboard.h>
// #include <QDesktopServices>
// #include <qdialog.h>
// #include <qevent.h>
// #include <QButtonGroup>
// #include <QFileInfo>
// #include <QHeaderView>
// #include <qimage.h>
// #include <QLayout>
// #include <qmenubar.h>
// #include <QList>
// #include <QMenu>
// #include <QNetworkReply>
// #include <QScrollArea>
// #include <QScrollBar>
// #include <QStackedWidget>
// #include <QStackedLayout>
// #include <QStatusBar>
// #include <QTabWidget>
// #include <QTextBrowser>
// #include <QTimer>
// #include <QToolBar>
// #include <QToolBox>
// #include <QToolTip>
// #include <QTreeWidget>
// #include <qvariant.h>
// #include <QVBoxLayout>
// #include <qwhatsthis.h>
// #include <QWebFrame>
// #include <QWebPage>
// #include <QWebView>
// #include <QProgressBar>
// #if (QT_VERSION >= 0x050000)
// #include <QUrlQuery>
// #endif

// #include "itextbrowser.h"
// #include "itextedit.h"
// #include "iscrollarea.h"

// using namespace Qt;

/* Viewing/Browsing hierarchies

  CLASS HIERARCHY

  (taDataViewer)
    DataViewer -- (abstract) main taBase persistable object for controlling viewing windows
      DataBrowser [ta_qtbrowse.h] -- controller for tree-based data browser
      T3Viewer [t3viewer.h] --  controller for 3d-based data navigator

  (QMainWindow)
    iMainWindowViewer -- (abstract) main window class that can be used by DataViewer classes; manages menus,
          docking toolbars
      iTabViewer -- (abstract) subclass that handles tabbed data panels
        iDataBrowser -- window for tree-based browsing
        iT3Viewer -- window for Inventor-based browsing

  (QFrame, IDataLinkClient)
    iDataPanel -- basic panel that can appear in tabbed viewers -- note that panels are not tied
          to any particular viewing subsystem, so can appear i.e., in browsing and 3d viewing
      iDataPanelSet -- contains any number of other types of panels, with a way to activate each
      iBrListDataPanel -- contains a list view of the item selected in the browse tree
      EditPanel -- property list -- based on the v3.2 EditDialog


  (QTreeWidgetItem, IDataLinkClient)
    BrListViewItem  -- (abstract) nodes in either the tree view (left side), or list views (right panel)
      taiTreeDataNode -- (abstract) nodes in the tree view (left side)
        tabTreeDataNode -- nodes for taBase data items
          tabListTreeDataNode -- nodes for taList data items
            tabGroupTreeDataNode -- nodes for taGroup data items
      taiListDataNode -- nodes in the list views (right panel)

  (QTreeWidget)
    iListView [implementation class, ta_qtbrowse.cc] -- light subclass of the Qt ListView

  (QWidget)
    iTabView -- pane with tabs, for showing panels; a iMainWindowViewer can split-nest/unsplit-unnest these
        ad infinitum

  (QTabBar)
    iTabBar -- tab bar in iTabView right hand side

  (QTab)
    iTab -- tabs in the iTabBar -- has a reference to its panel



  OWNERSHIP

  DataViewer (1)
    iMainWindowViewer (0:1)

  DataBrowser (1)
    iDataBrowser (0:1)
      iListView (1) -- on right hand side (tree view)
      iTabView (1:M) -- on left, recursively nested hor or ver, 2 per splitter
        iTabBar (1)
          iTab (1:M) -- always at least one tab (even if empty), user can add new tabs
            (iDataPanel) (0:1) -- contains ref to one of the TabView's panels
        iDataPanel (0:M) -- any number of data panels, in a widget stack


  CREATION SEQUENCES

  1. Interactively (by user)

    * create a DataViewer dv of the correct subclass
    * call dv->OpenWindow()
    * creates the correct iMainWindowViewer window class, and all its various subcomponents,
      such as list views, toolbars, etc.

  2. Loading from file (ex. when loading a project)

    * create the DataViewer dv of the correct subclass, during load
    * put dv on the winbMisc::unopened_windows list
    * when loading is complete, in the event loop, the u_w list is processed
    * for each u_w list item dv, call dv->OpenWindow()


  DESTRUCTION SEQUENCES

  1. Window Close -- the user closes the window  (but can be stopped by unsaved change prompts)

    for each iTabView tv
      for each iDataPanel dp
        call dp->Closing, with option to cancel (ex. if unsaved changes, and user responds "Cancel")

  2. Data Item deletion -- a data item being viewed is deleted

    TreeNodes: simply delete the node; this deletes all subnodes, and removes from display

    Panels: delete every iTab with which the panel is associated (except leave at least one tab)



*/

/* Clipboard Handling

  A subcontrol that wants to control clipboard handling must provide a signal slot interface,
  as follows; the slots/signals can have any names (* is optional):

  Slots:
    void EditAction(int); // called when user requests the indicated edit action,
      ex. via menu or accel key
    void GetEditActions(int&); // called to get current valid edit actions
    *void SetActionsEnabled(TBD); // enables/disables any actions
  Signals:
    *void UpdateUi(); // control can call if something changed, to update the ui -- calls back its
       GetEditAction and SetActionsEnabled slot functions


  When a clipboard-enabled control (ex. the data browser tree) gets the focus, it should call:

    SetClipboardHandler(QWidget* handler_obj, ...) [see object below for api details]

  If a control knows that it is no longer active,  it should call.

    SetClipboardHandler(NULL)

  NOTE: however you can't call the above just because you lose focus, because this happens
    normally. ex. an edit control has focus, then user clicks on a toolbar button -- the
    edit control loses focus. Instead, the mechanism used is that basically unless something
    actively grabs the handler, it is left with the previous value. The taiData objects
    have a base mechanism built in so that when an implementation control gets focus, it
    unregisters clipboard handling, by default. Therefore, only taiData controls that implement
    it will actually get the focus, but simply clicking away on a toolbar button won't
    dehandle.

*/

/* Selection Handling

  Selection handling is the system that handles user selection of items in the gui,
  and the corresponding changes that take place. For example, the user selects an
  item in the tree, which causes its corresponding panel to display. Or, the user
  selects a 3D item in the T3 viewer. Some areas permit multiple items to be selected;
  examples are T3 and ListViews.
  The system does not make a distinction between "current" and "selected" (ex. as is made
    by Qt in its ListView).

  A selection handler implies being a Clipboard Handler -- the ISelectableHost i/f
  provides an implementation ("ActionsEnabled" and "UpdateUi" are not needed.)

  Selections affect:
    gui -- things like panels change when selections change
    clip -- legal clipboard operations change, depending on selection
    actions -- selected items have actions, and multiple selections
        have dynamic actions, such as adding a projection between layers
    context menus -- these need access to selection

  Selection Sources/Sinks

  A gui item can be a selection source, sink, or both. A selection Source is something
  the user can work in to make selections.
  Sources:
    * tree nodes (browser, list views, specials, such as prog editor)
    * T3 viewer
  Sinks:
    * panel tabs (note: modal)
    * main window -- updates the Actions menu and actions
  How/When things become sinks is modal -- if the user is in the tree browser, then
  the panels should update; but if the user is in the prog editor, that containing
  panel should NOT update as the user clicks around on the items; however, the user
  still expects the clip menu items to be valid.


  Selection handling involves the following elements:

  Interfaces:
    ISelectableHost -- interface implemented by an object that supports selectable items
    ISelectable -- interface of an item that can be selected in the gui

  Main Menu:
    Edit/clip menu -- enabling of clip items is based on selection
    Actions menu -- contains the actions available, based on selection


  ISelectableHost i/f -- for sources of selection
    enum NotifyOp: GotFocus, SelectionChanged, Destroying
    abstract public virtuals you must implement:
      QWidget* widget() -- provides access to the implementers widget (for signals/slots etc.)
      bool hasMultiSelect() -- true if supports multi select
    abstract protected virtuals you must implement:
      ApplySelectedItems_impl -- called when force=true for changes, force gui to be selItems

    implemented virtuals, possible (but not usual) to extend:
      selItems() -- list of selected items
      dynActions() -- Action list, of current dynamic actions available
      dynMethods() -- list of current dynamic methods available
      (the list accessors are virtual, which supports possible JustInTime filling
       of the lists prior to returning them)
      SelectionChanging(bool) -- bracketing batch changes, so only one notify
      ClearSelectedItems() -- forced clearing
      AddSelectedItem(item) -- add the item
      RemoveSelectedItem(item) -- remove the item
      FillContextMenu(taiActions* menu) -- fill the context menu; host can extend
        to put items before or after as well
    convenience members (non virtual)
      handlerObj() -- this is provided so client can connect to us as ClipHandler
      selItem() -- convenience, first selected guy (or NULL if none)
      setSelItem(item, force) -- convenience, for prog setting of single select
      AddDynMenuItems(taiMenu) -- add the dynamic guys to the given menu
    ClipHandler methods:
      EditEnabled(int&) -- return enabled flags
      EditAction(int) -- perform the action
    connection methods:
      Connect_SelectableHostItemRemovingSlot(QObject* src_obj,
        const char* src_signal, bool disconnect = false) -- connects (or disconnects)
         an optional ItemRemoving notification
      Connect_SelectableHostNotifySignal(QObject* sink_obj,
        const char* sink_signal, bool disconnect = false) -- connects (or disconnects)
         a sink (ex iFrame) to the notify signal raised when sel changes (or
         gets focus, etc.)
    protected:
      handler -- the impl widget object for signals/slots
      (list members)
      Emit_NotifySignal(op) -- called internally, and also by imlementer when gets focus
      DoDynAction(int) -- called by the slot on helper
      ApplySelectedItems_impl -- called when force=true for changes, force gui to be selItems
      UpdateActions() -- implementation function that updates the dyn methods/actions
        (can be overridden/extended if necessary)

    SelectableHostHelper -- this is a helper QObject that handles signals/slots
      ISelectableHost* host -- the owning host
      Emit_NotifySignal(op) -- called to emit the signal
    signals:
      SelectableHostNotifySignal(ISelectableHost* src, op);
    slots:
      DynActionSlot(int) -- callback for dynamic actions
      ItemRemovingSlot(item)
      EditEnabled(int&) -- callback for when we are ClipHandler
      EditAction(int) --  callback for when we are ClipHandler

    Client of ISelectableHost:
      members:
        ISelectableHost* cur_host -- the client should keep track of the host
          with focus, so it can ignore other hosts; it should also check for the
          Delete op, and delete this if so

      slots:
        SelectableHostNotify(ISelectableHost* src, op) (name can be anything)
          this is the slot called by the ISelectableHost

  Notes:
    * whenever the Actions guys gets cleared for any reason, the actions are deleted,
      and any menu guys that have been created will also be deleted
  Source Changes:
    Source changes (the guy controlling the selectable clients) typically only happen
    explicitly, when that guy gets focus. In that case, it raises a Notify signal
    with GotFocus op, to enable clients to start tracking changes only from that
    guy -- until an explicit focus change happens again, they will typically
    ignore Notifies from other sources.


  What the implementing class must do:
    * provide the implementations for the trivial abstract virtuals, like widget()
    * provide an implementation for ApplySelectedItems_impl, which has to take
      the list and then force the gui to reflect the selection in the list
    * when you detect that selection changes in the gui, call the non-force
      member funcs, ex. setSelItem (for one guy),
      or: SelChanging(t) ... changes .. SelChanging(f) (for multiple guys)
      -- the non-force version won't callback your own ApplySel.. guy
    * detect when your control *receives* focus, and call Emit_Notify(ReceivedFocus)
      this will force the host/clients to configure themselves for our selections


  What the client class should do:
    * it should connect its Notify slot to the ISelectableHost via
      Connect_SelectableHostNotifySignal
    * if it connects to more than one source, it should have a member
      to keep track of the one with focus -- if so, it should ignore
      signals from any non-focus guys, update the focus guy, and delete
      it if the delete op is received
    * if it is the main window, when it gets the GotFocus op,
      it should set the src as the cliphandler by calling:
        SetClipboardHandler(src_host->handlerObj(),
          SLOT(EditEnabled(int&)), SLOT(EditAction(int)) )
      Note: the cliphandler can disengage independently of ISH
        still being the selection handler
    * when it acts on a valid SelectionChanged, it can update
      its action menus
    * clients must NEVER keep references to the sellists etc. outside
      the signal handler -- if they need to reference them, they must
      do so through the instance pointer they cache (which gets a signal
      on deletion, so client doesn't have to independently connect
      a delete notify signal)

  iFrame:

    slots:
      SelectableHostNotifySlot_Internal_ -- connects guys nested below us; lets us trap
      SelectableHostNotifySlot_External -- from external guys (forwarded from main window)

    signals:
      SelectableHostNotifySignal -- forwarder, from all internal guys

*/

//////////////////////////
//   IDataHost          //
//////////////////////////

//from ta_qtdata_def.h -- dependencies are here, so makes sense to impl here

//////////////////////////
//   taiDataLink        //
//////////////////////////

//////////////////////////////////
//      tabDataLink             //
//////////////////////////////////

//////////////////////////
//   tabODataLink       //
//////////////////////////

//////////////////////////////////
//   tabListDataLink            //
//////////////////////////////////

//////////////////////////////////
//      tabGroupDataLink        //
//////////////////////////////////

//////////////////////////////////
//  IDataViewWidget             //
//////////////////////////////////

//////////////////////////
//   ISelectable        //
//////////////////////////

//////////////////////////////////
//   IObjectSelectable          //
//////////////////////////////////

//////////////////////////////////
//   ISelectable_PtrList        //
//////////////////////////////////

//////////////////////////
//   DynMethod_PtrList  //
//////////////////////////

//////////////////////////////////
//   ISelectableHost            //
//////////////////////////////////

//////////////////////////////////
//  SelectableHostHelper        //
//////////////////////////////////

//////////////////////////////////
//  iFrameViewer                //
//////////////////////////////////

//////////////////////////
//   iBrowseViewer      //
//////////////////////////

//////////////////////////
//   iTabViewer         //
//////////////////////////

//////////////////////////
//  iDockViewer         //
//////////////////////////

//////////////////////////
//   iToolBoxDockViewer //
//////////////////////////

//////////////////////////
//   iToolBar           //
//////////////////////////

//////////////////////////////////
//  iToolBar_List               //
//////////////////////////////////

//////////////////////////
//   iApplicationToolBar//
//////////////////////////

//////////////////////////
//  iBaseClipWidgetAction //
//////////////////////////

//////////////////////////
//  iBrowseHistory      //
//////////////////////////

//////////////////////////
//  iMainWindowViewer   //
//////////////////////////

//////////////////////////
//      iTabBar         //
//////////////////////////

//////////////////////////
//   iDataPanel_PtrList //
//////////////////////////

//////////////////////////
//   iTabView           //
//////////////////////////

//////////////////////////
//   iTabView_PtrList   //
//////////////////////////

//////////////////////////
//      iDataPanel      //
//////////////////////////

//////////////////////////
//   iDataPanel_PtrList //
//////////////////////////

//////////////////////////
//   iDataPanelFrame    //
//////////////////////////

//////////////////////////
//    iViewPanelFrame   //
//////////////////////////

//////////////////////////
//    iDataPanelSet     //
//////////////////////////

//////////////////////////
//    iDataPanelSet     //
//////////////////////////

//////////////////////////
//    iViewPanelSet     //
//////////////////////////

//////////////////////////
//    iListDataPanel    //
//////////////////////////

//////////////////////////
//    iTextDataPanel    //
//////////////////////////

//////////////////////////
//    iDocDataPanel     //
//////////////////////////

//////////////////////////
//    iTreeView         //
//////////////////////////

//////////////////////////
//    iTreeViewItem     //
//////////////////////////

//////////////////////////////////
//      taiListDataNode         //
//////////////////////////////////

//////////////////////////////////
//      taiTreeDataNode         //
//////////////////////////////////

//////////////////////////////////
//   tabTreeDataNode            //
//////////////////////////////////

//////////////////////////////////
//   tabParTreeDataNode         //
//////////////////////////////////

//////////////////////////////////
//   tabDefChildTreeDataNode    //
//////////////////////////////////

//////////////////////////////////
//   tabListTreeDataNode        //
//////////////////////////////////

//////////////////////////////////
//   taiGroupTreeDataNode       //
//////////////////////////////////

//////////////////////////////////
//   iTreeSearch                //
//////////////////////////////////

//////////////////////////////////
//   iSearchDialog              //
//////////////////////////////////

//////////////////////////
//   iHelpBrowser       //
//////////////////////////


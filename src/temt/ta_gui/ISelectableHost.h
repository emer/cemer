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

#ifndef ISelectableHost_h
#define ISelectableHost_h 1

// parent includes:

// member includes:
#include <ISelectable>
#include <ISelectable_PtrList>
#include <DynMethod_PtrList>
#include <iAction_List>

// declare all other types mentioned but not required to include:
class QObject; // 
class iMainWindowViewer; // 
class taProject; // 
class ISelectable_PtrList; // 
class QWidget; // 
class ISelectableHost_QObj; // #IGNORE
class taiMimeSource; //

class TA_API ISelectableHost { // interface on the controlling widget hosting ISelectable items
friend class ISelectableHost_QObj;
friend class ISelectable;
public:
  enum NotifyOp { // notify ops for the NotifySignal -- note, passed as int in the sig/slot
    OP_GOT_FOCUS,
    OP_SELECTION_CHANGED,
    OP_DESTROYING
  };

  static const char* edit_enabled_slot; // for the SetClipboardHandler call, takes int& param
  static const char* edit_action_slot; // for the SetClipboardHandler call, takes int param
  static const char* actions_enabled_slot; // currently NULL
  static const char* update_ui_signal; // currently NULL
  static const char* edit_menu_action_slot;

  ISelectable*          curItem() {return selItems().SafeEl(0);} // convenience
  virtual void          setCurItem(ISelectable* item, bool forceUpdate = false);
//  virtual iAction_List& dynActions() {return dyn_actions;}
    // Action list, of current dynamic actions available
//  virtual DynMethod_PtrList& dynMethods() {return dyn_methods;}
   // -- list of current dynamic methods availableselectionChanged
  QObject*              clipHandlerObj() const;
    // provided so client can connect to us as a ClipHandler (EditEnabled, EditAction only)
  virtual bool          hasMultiSelect() const = 0; // true if supports multi select
  ISelectableHost_QObj* helperObj() const {return helper;} // for attaching slots
  iMainWindowViewer*    mainWindow() const; // returns main window we are embedded in
  taProject*            myProject() const; // project of which this viewer is a part
  taProject*            curProject() const; // only if we are a projviewer
  virtual bool          selectionChanging() {return (m_sel_chg_cnt != 0);}
    // you can use this to escape from sundry gui notifies to yourself (to avoid recursion)
  virtual ISelectable_PtrList&  selItems() {return sel_items;} // currently selected items
  virtual QWidget*      widget() = 0; // provides a gui parent for things like context menus

  virtual void          FillContextMenu(taiWidgetActions* menu);
    // s/b called by desc class, to put dynaction items onto menu
  virtual void          AddDynActions(taiWidgetActions* menu, int dyn_list,
    ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT);
   // add the dynamic guys to the given menu (note: FillContextMenu does this too)

  virtual void          SelectionChanging(bool begin, bool forced = true); // if used, must be called in pairs, with true then false
  virtual void          ClearSelectedItems(bool forced = true);
  virtual void          AddSelectedItem(ISelectable* item,  bool forced = true);
  virtual bool          RemoveSelectedItem(ISelectable* item,  bool forced = true);
    // 'true' if item was actually removed from (i.e. was in) list

  virtual void          EditActionsEnabled(int&); // return enabled flags
  virtual void          EditAction(int,
    ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT); // perform the action
  virtual void          DropEditAction(int ea); // perform the action (from drop handler)

  void                  Emit_GotFocusSignal() {Emit_NotifySignal(OP_GOT_FOCUS);}
    // only signal external guys should call (when we or a parent get focus)

  void                  Connect_SelectableHostNotifySignal(QObject* sink_obj,
    const char* sink_slot, bool discnct = false);  // connects (or disconnects) a sink (ex iFrame) to the notify signal raised when sel changes (or gets focus, etc.)
  void                  Connect_SelectableHostItemRemovingSlot(QObject* src_obj,
    const char* src_signal, bool discnct = false); // connects (or disconnects) an optional ItemRemoving notification
  void                  UpdateMethodsActionsForDrop();
    // uses ctxt_ms and ctxt_item
  void                  ctxtMenu_destroyed();

  ISelectableHost();
  virtual ~ISelectableHost();

protected:
  static void           ItemDeleting(ISelectable* item); // make sure it is off all sel lists

  int                   m_sel_chg_cnt;
   // counter to track batch selection changes; -ve means we are in Update (prog calls ignored)
  ISelectable_PtrList   sel_items;
  DynMethod_PtrList     dyn_methods[2]; // available dynamic methods; 0=single/view 1=obj
  iAction_List        dyn_actions[2]; // actions corresponding to methods (always 1:1)
  ISelectable::GuiContext dyn_context[2];
  int                   dyn_idx; // the current dyn index (of all [])
  taiMimeSource*        ctxt_ms; // during a drop and context holds the ms used for dyn and edit actions
  ISelectable*          ctxt_item; // during drop, holds the item dropped on; for context, holds the item

  virtual void          FillContextMenu_pre(ISelectable_PtrList& sel_items,
    taiWidgetActions* menu) {} // hook
  void                  FillContextMenu_int(ISelectable_PtrList& sel_items,
    taiWidgetActions* menu, int dyn_list, ISelectable::GuiContext sh_typ);
  virtual void          FillContextMenu_post(ISelectable_PtrList& sel_items,
    taiWidgetActions* menu) {} // hook

  virtual void          EditAction_Delete(ISelectable::GuiContext gc_typ); // actually does the Edit/Delete
  virtual void          UpdateSelectedItems_impl() = 0;
    // called when force=true for changes, force gui to be selItems
  void                  Emit_NotifySignal(NotifyOp op);
  virtual void          DoDynAction(int idx); // do the action in the list
  virtual void          SelectionChanged(bool forced);
    // invoked when selection of current clipboard/focus handler changes
  virtual void          UpdateMethodsActions(int dyn_list,
    ISelectable::GuiContext gc_typ = ISelectable::GC_DEFAULT);
    // updates the dyn methods/actions

private:
  ISelectableHost_QObj* helper;
};

#endif // ISelectableHost_h

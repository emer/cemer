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

#include "ISelectableHost.h"
#include <SelectableHostHelper>
#include <taiWidgetActions>
#include <taiMimeSource>
#include <taiClipData>
#include <taBase_RefList>
#include <taProject>
#include <taiSigLink>
#include <iMainWindowViewer>


#include <taMisc>
#include <taiMisc>

#include <css_machine.h>
#include <css_basic_types.h>
#include <css_qtdialog.h>
#include <css_ta.h>


void ISelectableHost::ItemDeleting(ISelectable* item) {
  taPtrList_impl* insts = ISelectable_PtrList::insts; // cache for convenience
  if (!insts) return; // note: prob shouldn't happen, if item exists!
  for (int i = insts->size - 1; i > 0; --i) {
    ISelectable_PtrList* list = (ISelectable_PtrList*)insts->FastEl_(i);
    list->RemoveEl_(item);
  }
}

const char* ISelectableHost::edit_enabled_slot = SLOT(EditActionsEnabled(int&));
const char* ISelectableHost::edit_action_slot = SLOT(EditAction(int));
const char* ISelectableHost::edit_menu_action_slot = SLOT(EditAction(taiAction*));
const char* ISelectableHost::actions_enabled_slot; // currently NULL
const char* ISelectableHost::update_ui_signal; // currently NULL

ISelectableHost::ISelectableHost() {
  m_sel_chg_cnt = 0;
  helper = new SelectableHostHelper(this);
  dyn_idx = 0;
  ctxt_ms = 0;
  ctxt_ms = NULL;
}

ISelectableHost::~ISelectableHost() {
  Emit_NotifySignal(OP_DESTROYING);
  //note: we delete it right now, to force disconnect of all signals/slots
  delete helper;
  helper = NULL;
}

void ISelectableHost::AddSelectedItem(ISelectable* item,  bool forced) {
  sel_items.AddUnique(item); //note: use raw list, because we are building it
  if (m_sel_chg_cnt == 0) // also ignored if in Update
    SelectionChanged(forced);
}

void ISelectableHost::AddDynActions(taiWidgetActions* menu, int dyn_list,
  ISelectable::GuiContext gc_typ)
{
  if (dyn_actions[dyn_list].count() == 0) return;
//nn,at top  menu->AddSep();
  for (int i = 0; i < (int)dyn_actions[dyn_list].count(); ++i) {
    taiAction* act = dyn_actions[dyn_list].FastEl(i);
    menu->AddAction(act);
  }
}

void ISelectableHost::ClearSelectedItems(bool forced) {
  SelectionChanging(true, forced);
  sel_items.Reset(); //note: use raw list, because we are building it
  SelectionChanging(false, forced);
}

QObject* ISelectableHost::clipHandlerObj() const {
  return helper;
}

void ISelectableHost::Connect_SelectableHostNotifySignal(QObject* sink_obj,
    const char* sink_slot, bool discnct)
{
  static const char* sig_nm = SIGNAL(NotifySignal(ISelectableHost*, int));
  if (discnct)
    QObject::disconnect(helper, sig_nm, sink_obj, sink_slot);
  else
    QObject::connect(helper, sig_nm, sink_obj, sink_slot);
}

void ISelectableHost::Connect_SelectableHostItemRemovingSlot(QObject* src_obj,
    const char* src_signal, bool discnct)
{
  static const char* slot_nm = SLOT(ItemRemoving(ISelectableItem*));
  if (discnct)
    QObject::disconnect(src_obj, src_signal, helper, slot_nm);
  else
    QObject::connect(src_obj, src_signal, helper, slot_nm);
}

void ISelectableHost::ctxtMenu_destroyed() {
  if (ctxt_ms) {
    delete ctxt_ms;
    ctxt_ms = NULL;
  }
  ctxt_item = NULL;
}

void ISelectableHost::DropEditAction(int ea) {
  ISelectable* ci = ctxt_item;
  if (!ci) return;
  ci->EditActionD_impl_(ctxt_ms, ea, ISelectable::GC_DEFAULT);
}

void ISelectableHost::EditAction(int ea,
    ISelectable::GuiContext gc_typ)
{
  ISelectable* ci = curItem();
  if (!ci) return;
  // delete is a special case
  if (ea & taiClipData::EA_DELETE) {
    if(taMisc::delete_prompts) {
      int chs = taMisc::Choice("Are you sure you want to delete the selected object(s)?",
                               "&No", "&Yes");
      if (chs != 1) return;
    }
    EditAction_Delete(gc_typ);
  } else {
    ISelectable_PtrList items(selItems());
    ci->EditAction_(items, ea, gc_typ);
  }
}

void ISelectableHost::EditAction_Delete(ISelectable::GuiContext gc_typ) {
  const ISelectable_PtrList& items = selItems();
  // first, compile a ref list of all taBase guys
  taBase_RefList ta_items;
  for (int i = 0; i < items.size; ++i) {
    ISelectable* ci = items.SafeEl(i);
    if (!ci) continue;
    taBase* tab =  ci->taData(gc_typ);// is the effLink data
    if (!tab) continue;
    ta_items.Add(tab);
  }
  // now, just request deletion -- items could go missing
  // if we've selected items that will get deleted by other items

  taProject* proj = myProject();
  if(proj) {
    taBase* tab = ta_items.Peek();
    if(ta_items.size > 1)
      proj->undo_mgr.SaveUndo(tab, "Delete " + String(ta_items.size) + " items", NULL, false,
                              tab->GetOwner());
    else
      proj->undo_mgr.SaveUndo(tab, "Delete", NULL, false, tab->GetOwner());
  }

  for (int i = ta_items.size - 1; i >= 0; --i) {
    taBase* tab = ta_items.SafeEl(i);
    if (!tab) continue;
    tab->Close(); // if happens immediately, will get removed from list
  }
}

void ISelectableHost::EditActionsEnabled(int& ea) {
  ISelectable* ci = curItem();
  if (!ci) return;
  ISelectable_PtrList items(selItems());
  int rval = ci->QueryEditActions_(items);
  // certain things disallowed if more than one item selected
  if (items.size > 1) {
    rval &= ~(taiClipData::EA_FORB_ON_MUL_SEL);
  }
  ea = rval;
}

void ISelectableHost::Emit_NotifySignal(NotifyOp op) {
  helper->Emit_NotifySignal(op);
}

void ISelectableHost::FillContextMenu(taiWidgetActions* menu) {
  if (sel_items.size == 0) return; // shouldn't happen
  QObject::connect(menu, SIGNAL(destroyed()), helper, SLOT(ctxtMenu_destroyed()) );
  ISelectable_PtrList& sel_items = selItems();
  ISelectable* item = sel_items.FastEl(0);
  if (sel_items.size == 1) {
    ctxt_ms = taiMimeSource::NewFromClipboard(); // deleted in the destroyed() handler
    ctxt_item = item;
  }

  FillContextMenu_pre(sel_items, menu);

  ISelectable::GuiContext sh_typ = item->shType();
  // init the dyn context
  dyn_idx = 0;

  if (sh_typ == ISelectable::GC_SINGLE_DATA) {
    FillContextMenu_int(sel_items, menu, 0, sh_typ);
  } else { // dual, make submenus
    String view_cap = "View";
    String obj_cap = "Object";
    item->GetContextCaptions(view_cap, obj_cap);
    taiMenu* sub = menu->AddSubMenu(view_cap);
    FillContextMenu_int(sel_items, sub, 0, ISelectable::GC_DUAL_DEF_VIEW);
    sub = menu->AddSubMenu(obj_cap);
    FillContextMenu_int(sel_items, sub, 1, ISelectable::GC_DUAL_DEF_DATA);
  }

  FillContextMenu_post(sel_items, menu);

  // have to delete ms now, because Qt deletes MimeSource from clipboard in event loop
  if (ctxt_ms) {
    delete ctxt_ms;
    ctxt_ms = NULL;
  }
}

void ISelectableHost::FillContextMenu_int(ISelectable_PtrList& sel_items,
  taiWidgetActions* menu, int dyn_list, ISelectable::GuiContext sh_typ)
{
  UpdateMethodsActions(dyn_list, sh_typ);

  // do the item-mediated portion
  ISelectable* ci = curItem();
  if (ci) {
    // start with dynamic actions
    if (dyn_actions[dyn_list].count() != 0) {
      AddDynActions(menu, dyn_list, sh_typ);
      menu->AddSep();
    }
    ci->FillContextMenu(sel_items, menu, sh_typ);
  }
}

void ISelectableHost::DoDynAction(int idx) {
  // find the right list, and adjust idx to refer to that list
  //const int max_lists = 1; // 2 lists
  int list = 0;
  while (idx > dyn_methods[list].size) {
    idx -= dyn_methods[list].size;
    list++;
  }
  DynMethod_PtrList& dyn_methods = this->dyn_methods[list];
//nn  taiAction_List&    dyn_actions = this->dyn_actions[list];
  ISelectable::GuiContext gui_ctxt = dyn_context[list];

  //note: we really won't have been called if any items don't have links,
  // but we have code in here to bail anyway if we do (maybe should put warning text?)
  if ((idx < 0) || (idx >= dyn_methods.size)) return; // shouldn't happen
  DynMethodDesc* dmd = dyn_methods.FastEl(idx);
  // NOTE: this function is based on the ta_qtdata:taiMethod::CallFun function
  // NOTE: we can't show the return values from any of the functions

  MethodDef* meth =  dmd->md;
  int use_argc = meth->fun_argc;
  int hide_args = (dmd->dmd_type == DynMethod_PtrList::Type_1N) ? 0 : 1;

  if (meth->stubp == NULL) return;
  // type of calling method's class
  // NOTE: we may look up exact type for each inst later
  TypeDef* typ = meth->owner->owner;
  // we use a smart ref in case anchor obj ever disappears in multi ops
  void* base; // the object calling the method

//TODO:  ApplyBefore();
  String argc_str = meth->OptionAfter("ARGC_");
  if (argc_str != "")
    use_argc = (int)argc_str;
  use_argc = MIN(use_argc, meth->arg_types.size);
  use_argc = MIN(use_argc, meth->arg_names.size);

  // use a copy of selected items list, to avoid issues if items change during these operations
  //NOTE: a bit hacky since adding dynamic drop actions...
  // if we are in the middle of a Drop, then ctxt_ms will have been set
  // otherwise, we have to recreate it here, since Qt deletes MimeSource guys
  // from clipboard automatically in the event loops, so we couldn't cache it
  ISelectable_PtrList sel_items_cp;
  if (dmd->dmd_type == DynMethod_PtrList::Type_MimeN_N) {
    if (!ctxt_item) return; // not supposed to happen
    sel_items_cp.Add(ctxt_item);
  } else {
    sel_items_cp = sel_items;
  }

  // if params to prompt for, do that now
  // NOTE: if it is also CONFIRM, we implicitly use param collection as confirm
  int prompt_argc = use_argc - hide_args;
  cssiArgDialog* arg_dlg = NULL; // if needed
  cssEl** prompt_params = NULL; // if needed -- points inside arg_dlg
  if( (prompt_argc != 0) || meth->HasOption("CONFIRM")) {
    base = curItem()->effLink(gui_ctxt)->data();
    arg_dlg = new cssiArgDialog(meth, typ, base, use_argc, hide_args);
    if (base && typ->IsActualTaBase()) {
      bool ok;
      iColor bgclr = ((taBase*)base)->GetEditColorInherit(ok);
      if (ok) arg_dlg->setBgColor(bgclr);
    }
    arg_dlg->Constr("", "");
    int ok_can = arg_dlg->Edit(true);   // true = wait for a response
    if (!(ok_can && !arg_dlg->err_flag))
      goto exit;
    // args in dlg are now: arg[0] arg[1] .. hide_args .. use_args
    // need to have [0] [1] be a dummy, since actual args are from [2]
    prompt_params = &(arg_dlg->obj->members->els[hide_args]);
    // make sure argc is now right
    prompt_argc = arg_dlg->obj->members->size-(1+hide_args);
  }
//TODO    GenerateScript();
#ifdef DMEM_COMPILE
    // don't actually run the command when using gui in dmem mode: everything happens via the script!
    if (taMisc::dmem_nprocs == 1)
#endif
    { // block for jumps
      cssEl* rval = NULL; // ignored
      int i;
      ISelectable* itN;
      taiSigLink* link = NULL;
      if (dmd->dmd_type == DynMethod_PtrList::Type_1N) { // same for all
        while (sel_items_cp.size > 0) {
          itN = sel_items_cp.TakeItem(0);
          typ = itN->GetEffDataTypeDef(gui_ctxt);
          link = itN->effLink(gui_ctxt);
          if (!link) continue;
          base = link->data();
          if (!base) continue;
          rval = (*(meth->stubp))(base, prompt_argc, prompt_params);
          if(rval) {
            cssEl::Ref(rval);
            cssEl::unRefDone(rval);
          }
        }
        goto exit;
      }

      // remaining types use more complicated params
      { // for jumps
      cssEl** param = (cssEl**)calloc(3 + prompt_argc, sizeof(cssEl*));
      param[0] = &cssMisc::Void; // method*
      param[1] = &cssMisc::Void; // this*
      param[2] = new cssTA_Base();
      for (i = 2; i <= prompt_argc; ++i)
        param[1+i] = prompt_params[i];

      const int si_presize = sel_items_cp.size; // size before ops
      switch(dmd->dmd_type) {
      case DynMethod_PtrList::Type_1_2N: { // call 1 with 2:N as a param
        //NOTE: we insist list size doesn't change, i.e. no deletions,
        // else we bail!
        ISelectable* it1 = sel_items_cp.FastEl(0); // the anchor item
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        link = it1->effLink(gui_ctxt);
        if (!link) goto free_mem;
        base = link->data();
        for (int i = 1;
          (i < sel_items_cp.size) && (sel_items_cp.size == si_presize);
          ++i)
        {
          itN = sel_items_cp.FastEl(i);
          link = itN->effLink(gui_ctxt); //note: prob can't be null, because we wouldn't get called
          if (!link) continue;
          *param[2] = (void*)link->data();
          rval = (*(meth->stubp))(base, 1 + prompt_argc, param);
          if(rval) {
            cssEl::Ref(rval);
            cssEl::unRefDone(rval);
          }
        }
      } break;
      case DynMethod_PtrList::Type_2N_1: { // call 2:N with 1 as param
        ISelectable* it1 = sel_items_cp.FastEl(0);
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        link = it1->effLink(gui_ctxt);
        if (!link) goto free_mem; //note: we prob wouldn't get called if any were null
        *param[2] = (void*)link->data();
        for (int i = 1;
          (i < sel_items_cp.size) && (sel_items_cp.size == si_presize);
          ++i)
        {
          itN = sel_items_cp.FastEl(i);
          link = itN->effLink(gui_ctxt);
          if (!link) continue; // prob won't happen
          base = link->data();
          rval = (*(meth->stubp))(base, 1 + prompt_argc, param); // note: "array" of 1 item
          if(rval) {
            cssEl::Ref(rval);
            cssEl::unRefDone(rval);
          }
        }
      } break;
      case DynMethod_PtrList::Type_MimeN_N: { // call 1:N with ms_objs[1..N] as params
        //NOTE: sel_items actually contains the drop target item
        if (!ctxt_ms) { // ctxt (not drop) mode -- need to make it
          ctxt_ms =  taiMimeSource::NewFromClipboard(); // deleted in the destroyed() handler
        }
        ISelectable* it1 = sel_items_cp.FastEl(0);
        typ = it1->GetEffDataTypeDef(gui_ctxt);
        while (sel_items_cp.size > 0) {
          itN = sel_items_cp.TakeItem(0);
          link = itN->effLink(gui_ctxt);
          if (!link) continue; // prob won't happen, because we wouldn't have been called
          base = link->data();
          for (int j = 0; j < ctxt_ms->count(); ++j) {
            ctxt_ms->setIndex(j);
            taBase* obj = ctxt_ms->tabObject();
            if (!obj) continue;
            *param[2] = obj;
            rval = (*(meth->stubp))(base, 1 + prompt_argc, param);
            if (link->isBase())
              ((taBase*)base)->UpdateAfterEdit();
            if(rval) {
              cssEl::Ref(rval);
              cssEl::unRefDone(rval);
            }
          }
        }
      } break;
      default: break; // compiler food, we handled all cases
      }
free_mem:
      delete param[2];
      free(param);
      } // for jumps
    }
exit:
  if (arg_dlg) {
    delete arg_dlg;
  }

}

iMainWindowViewer* ISelectableHost::mainWindow() const {
  QWidget* wid = const_cast<ISelectableHost*>(this)->widget();
  iMainWindowViewer* rval = NULL;
  while (wid && !(rval = qobject_cast<iMainWindowViewer*>(wid)))
    wid = wid->parentWidget();
  return rval;
}

taProject* ISelectableHost::myProject() const {
  iMainWindowViewer* imwv = mainWindow();
  if(!imwv) return NULL;
  return imwv->myProject();
}

taProject* ISelectableHost::curProject() const {
  iMainWindowViewer* imwv = mainWindow();
  if(!imwv) return NULL;
  return imwv->curProject();
}

bool ISelectableHost::RemoveSelectedItem(ISelectable* item,  bool forced) {
  bool rval = sel_items.RemoveEl(item); // use raw list, because we are building
  if ((rval || forced) && (m_sel_chg_cnt == 0))
    SelectionChanged(forced);
  return rval;
}

void ISelectableHost::SelectionChanged(bool forced) {
  if (forced) {
    m_sel_chg_cnt = -1;
    UpdateSelectedItems_impl();
    m_sel_chg_cnt = 0;
  }
  dyn_idx = 0;
  UpdateMethodsActions(0);
  Emit_NotifySignal(OP_SELECTION_CHANGED); // note: sent through event loop first
}

void ISelectableHost::SelectionChanging(bool begin, bool forced) {
  // if -ve, we are in the Update routine, so we basically ignore, by counting backwards
  if (begin)
    if (m_sel_chg_cnt < 0) --m_sel_chg_cnt; else ++m_sel_chg_cnt;
  else {
    if (m_sel_chg_cnt < 0) ++m_sel_chg_cnt; else --m_sel_chg_cnt;
    if (m_sel_chg_cnt == 0)
      SelectionChanged(forced);
  }
}

void ISelectableHost::UpdateMethodsActions(int dyn_list, ISelectable::GuiContext gc_typ) {
  dyn_context[dyn_list] = gc_typ;
  // enumerate dynamic methods
  dyn_methods[dyn_list].Reset();
  // if one dst, add the drop actions
  ISelectable_PtrList& sel_items = selItems();
  if (ctxt_ms && ctxt_item) {
    dyn_methods[dyn_list].FillForDrop(*ctxt_ms, ctxt_item);
  }
  dyn_methods[dyn_list].Fill(sel_items, gc_typ);

  // dynamically create actions
  dyn_actions[dyn_list].Reset(); // note: items ref deleted if needed
  for (int i = 0; i < dyn_methods[dyn_list].size; ++i) {
    DynMethodDesc* dmd = dyn_methods[dyn_list].FastEl(i);
    taiAction* act = new taiAction(dyn_idx, dmd->md->GetLabel(), QKeySequence(), dmd->md->name );
    QObject::connect(act, SIGNAL(IntParamAction(int)), helper, SLOT(DynAction(int)));
    dyn_actions[dyn_list].Add(act);
    dyn_idx++;
  }
}

void ISelectableHost::UpdateMethodsActionsForDrop() {
  dyn_context[0] = ISelectable::GC_DEFAULT;
  // enumerate dynamic methods
  dyn_methods[0].Reset();
  dyn_actions[0].Reset(); // note: items ref deleted if needed
  if (!ctxt_ms || !ctxt_item) return;
  dyn_methods[0].FillForDrop(*ctxt_ms, ctxt_item);

  // dynamically create actions
  for (int i = 0; i < dyn_methods[0].size; ++i) {
    DynMethodDesc* dmd = dyn_methods[0].FastEl(i);
    taiAction* act = new taiAction(i, dmd->md->GetLabel(), QKeySequence(), dmd->md->name );
    QObject::connect(act, SIGNAL(IntParamAction(int)), helper, SLOT(DynAction(int)));
    dyn_actions[0].Add(act);
    dyn_idx++;
  }
}

void ISelectableHost::setCurItem(ISelectable* item, bool forced) {
  ISelectable_PtrList& sel_items = selItems(); // in case overridden
  ISelectable* ci = sel_items.SafeEl(0);
  if ((ci == item) && (sel_items.size <= 1) && (!forced)) return;

  SelectionChanging(true, forced);
    ClearSelectedItems(forced);
    if (item)
      AddSelectedItem(item, forced);
  SelectionChanging(false, forced);
}


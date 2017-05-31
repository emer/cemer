// Copyright 2017, Regents of the University of Colorado,
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

#include "taiMimeSource.h"
#include <iClipData>
#include <taiObjectMimeFactory>
#include <taProject>
#include <iMainWindowViewer>
#include <MainWindowViewer>
#include <iTreeView>

#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taDataProc>
#include <MemberDef>

#include <QStringList>
#include <QApplication>
#include <QClipboard>

#include <Program>
#include <ProgramCall>

#include <sstream>

#include <taiWidgetTokenChooser>

using namespace std;

taiMimeSource* taiMimeSource::New(const QMimeData* ms_) {
  //TODO: for multi, check for multi source, and create a taiMultiMimeSource obj instead
  taiMimeSource* rval =  new taiMimeSource(ms_);
  rval->Decode();
  return rval;
}

taiMimeSource* taiMimeSource::NewFromClipboard() {
  taiMimeSource* rval =  new taiMimeSource(QApplication::clipboard()->mimeData());
  rval->Decode();
  return rval;
}

taiMimeSource::taiMimeSource(const QMimeData* ms_)
:inherited()
{
  m_md = ms_;
  m_src_action = 0; 
  m_this_proc = false;
  if (m_md)
    connect(m_md, SIGNAL(destroyed()), this, SLOT(ms_destroyed()) );
//compatibility 
  mi = NULL;
  iter_idx = 0;
}

taiMimeSource::~taiMimeSource() {
  if (m_md) {
    disconnect(m_md, 0, this, 0);
    m_md = NULL;
  }
}

QByteArray taiMimeSource::data(const QString& mimeType) const {
  if (m_md) return m_md->data(mimeType);
  else return QByteArray();
}

int taiMimeSource::data(const QString& mimeType, taString& result) const {
  QByteArray ba = data(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

int taiMimeSource::data(const QString& mimeType, istringstream& result) const {
  QByteArray ba = data(mimeType);
  result.str(string(ba.data(), ba.size()));
  return ba.size();
}

QStringList taiMimeSource::formats() const {
  if (m_md) return m_md->formats();
  else return QStringList();
}

bool taiMimeSource::hasFormat(const QString &mimeType) const {
  if (m_md) return m_md->hasFormat(mimeType);
  else return false;
}

taiMimeItem* taiMimeSource::GetMimeItem(TypeDef* td, const String& subkey) {
  if (!m_md) return NULL;
  taiMimeItem* rval = NULL;
  // check first if already fetched
  for (int i = 0; i < items.size; ++i) {
    rval = items.FastEl(i);
    //NOTE: we current require *exact type match* to avoid inheritance issues
    if ((rval->GetTypeDef() == td) && (rval->subkey() == subkey))
      return rval; 
  }
  // otherwise, try to make a guy: NULL if can't be made
  rval = taiMimeItem::ExtractByType(td, this, subkey);
  return rval;
}

void taiMimeSource::ms_destroyed() {
  m_md = NULL;
  taMisc::DebugInfo("taiMimeSource::m_md destroyed before clip operations completed!");
}

taiObjectsMimeItem* taiMimeSource::objects() const {
  if (!mi)
    mi = (taiObjectsMimeItem*)const_cast<taiMimeSource*>(this)->GetMimeItem(&TA_taiObjectsMimeItem);
  return mi; // could be NULL if no tacss
}

bool taiMimeSource::isObject() const {
  return (objects()); // asserts
}


int taiMimeSource::index() const {
  return iter_idx;
}


taBase* taiMimeSource::tabObject() const {
  if (!(isObject() && isThisProcess() && inRange())) return NULL;
  
  return ((taiObjectMimeItem*)item())->obj(); // looks up from path
}

void taiMimeSource::Decode() {
  String str;
  if (data(iClipData::tacss_common, str) > 0) {
    Decode_common(str);
  }
  objects(); // asserts
}

bool taiMimeSource::Decode_common(String arg) {
  // decode the common header info, leaving the remaining text after nl in arg
  // s/b in form: <obj_cnt>;<src edit action>;<procid>;\n
  bool ok;
  String str = arg.before(';');
  m_src_action = str.toInt(&ok);
  if (!ok) return false;
  arg = arg.after(';');

  str = arg.before(';');
  int process_id = str.toInt(&ok);
  if (!ok) return false;
  m_this_proc = (process_id == taMisc::ProcessId());
  //note: we could add more fields, and still be compatible
  return true;
}

int taiMimeSource::objectData(istringstream& result) {
  if (isObject() && inRange()) {
    QString fmt = taiObjectMimeFactory::tacss_objectdata + ";index=" + String(index());
    return data(fmt, result);
  } else {
    return 0;
  }
}

void taiMimeSource::ResolveObjects() {
  if (!isThisProcess()) return;
  taiObjectsMimeItem* omi = objects();
  if (!omi) return;
  for (int i = 0; i < omi->count() ; ++i) {
    taiObjectMimeItem* mii = omi->item(i);
    mii->obj(); // asserts
  }
}

/*  ClipBoard operations

For the EditActions and Queries, we follow the pattern below:

  Action result:
    1 - action was performed by the called function
    0 - action was not performed
    -1 - action was forbidden by the called function
    -2 - an error occurred executing the called function

  For Drag init operations, there will be no mime object (NULL).

  We handle Source and Dest operations differently:
  Source:
    controller handles making multiple calls to Query and Action
    routines, if multiple items are selected
  Dest:
    controller queries each possible handler, and if it will handle
    that operation, then it calls that handler once -- the handler
    must then decode multi source operands, and deal with them
    
  QUERIES:
  If Has Parents:
    Call ChildQuery_impl function(s) of parent
  Then:
    Call Object Query_impl function

  ACTIONS:
  If Has Parents:
    Call ChildAction_impl function(s) of parent
  Then:
    If parent action !=0: Call Object Action_impl function (see below)

  This lets the parent have first dibs on the action taken -- if it

  The above general calling pattern is established non-virtually in the taBase class.
  However, the virtual _impl functions provide the ability to do inheritance and/or overrides of behavior.

*/

/*
  We generally allow COPY, and on single or multi-, since that is not typically mediated by the owner

*/

//////////////////////////////////
//  taBase			//
//////////////////////////////////

void taBase::QueryEditActions(taiMimeSource* ms,
    int& allowed, int& forbidden)
{ 
  if (!ms) { // SRC
    QueryEditActionsS_impl(allowed, forbidden);
    return;
  }
  //DST
  // ASSIGN not allowed for multi srcs
  if (ms->isMulti())
    forbidden |= (iClipData::EA_PASTE_ASSIGN | iClipData::EA_DROP_ASSIGN |
                  iClipData::EA_DROP_COMPARE | iClipData::EA_PASTE_COMPARE);
  // not allowed on ro
  if (isGuiReadOnly()) {
    forbidden |= (iClipData::EA_PASTE_ASSIGN | 
                  iClipData::EA_DROP_ASSIGN);
  }  
  // note: single is just the degenerate case here
  // for multi, a bit more complicated, since we need to not allow something
  // not allowed by all, which is not the same as forbid
  int allowed_accum = 0; // add allowed to this guy
  int allowed_knockout = -1; // subtract allowed from this guy
  for (int i = 0; i < ms->count(); ++i) {
    ms->setIndex(i);
    // try it for every item -- we only ultimately allow what is allowed for all items
    int item_allowed = 0;
    QueryEditActionsD_impl(ms, item_allowed, forbidden); 
    allowed_accum |= item_allowed;
    allowed_knockout &= item_allowed;
  }
  allowed |= (allowed_accum & allowed_knockout);
}
  
void taBase::QueryEditActionsS_impl(int& allowed, int& forbidden)
{
  allowed |= iClipData::EA_COPY;
  if (isGuiReadOnly()) {
    forbidden |= (iClipData::EA_CLEAR);
  }
  // simple convention to forbid clip ops for things like Wizards: 
  if (HasUserData("NO_CLIP")) 
    forbidden |= iClipData::EA_SRC_OPS;  
}

void taBase::QueryEditActionsD_impl(taiMimeSource* ms, int& allowed, int& forbidden)
{
  // determine if we can assign to
  // note: only works inprocess, 
  // TODO: maybe we could just stream in the stuff for ext???
  if (!ms->isMulti() && ms->isThisProcess()) {
    // Assign (copy)
    // note: validation checks for the parent/child scenario
    taBase* obj = ms->tabObject();
    if(obj)
      allowed |= (iClipData::EA_PASTE_COMPARE | iClipData::EA_DROP_COMPARE);
    if (CanCopy(obj))
      allowed |= (iClipData::EA_PASTE_ASSIGN | iClipData::EA_DROP_ASSIGN);
    if (CanAppend(obj))
      allowed |= (iClipData::EA_PASTE_APPEND);
  }
}

int taBase::EditAction(taiMimeSource* ms, int ea)
{ 
  if (ea & iClipData::EA_SRC_OPS) {
    return EditActionS_impl(ea);
  }
  
  // DST
  int rval = iClipData::ER_IGNORED;
  if (!ms) return rval; // shouldn't happen
  // note: single is just the degenerate case here
  // we requery to be absolutely sure 
  int allowed = 0;
  int forbidden = 0;
  QueryEditActions(ms, allowed, forbidden);
  int eax = ea & (allowed & (~forbidden));
  if (!eax) {
    return iClipData::ER_ERROR; // prob an error!
  } 
  bool multi = false;
  bool multi_off = false;
  if(ms->count() > 1) {
    ++taMisc::in_gui_multi_action;
    multi = true;
  }
  for (int i = 0; i < ms->count(); ++i) {
    if(multi && i == ms->count()-1) {
      --taMisc::in_gui_multi_action;
      multi_off = true;
    }
    ms->setIndex(i);
    // do it for every item 
    rval = EditActionD_impl(ms, eax);
    // keep looping as long as stuff ok
    if (rval != iClipData::ER_OK)
      return rval;
  }
  if(multi && !multi_off) // didn't reach end
    --taMisc::in_gui_multi_action;
  return rval;
}

int taBase::EditActionS_impl(int ea) {
  //note: follows same logic as the Query
  if (ea & iClipData::EA_COPY) return 1; //note: Ui actually puts the data on the clipboard, when it sees the 1

  return 0;
}

int taBase::EditActionD_impl(taiMimeSource* ms, int ea) {
  if (ea & (iClipData::EA_PASTE_ASSIGN | iClipData::EA_DROP_ASSIGN)) {
    // don't show var choice dialogs during assign
    taProject* proj = (taProject*)GetThisOrOwner(&TA_taProject);
    if (proj) {
      proj->no_dialogs = true;  // don't forget to set back to false!!
    }
    String saved_name = this->GetName();  // save name to restore after the copy
    taBase* obj = ms->tabObject();
    bool ok; // dummy
    if (CheckError((!obj), false, ok,
      "Could not retrieve object from clipboard"))
      return iClipData::ER_ERROR;
    this->Copy(obj);
    this->Copy(obj);  // the second copy fixes up pointers for cases where the pointed to object was not yet available - see bug #2584
    this->SetName(saved_name);  // restore the name
    if (proj) {
      proj->undo_mgr.SaveUndo(obj, "ASSIGN " + obj->GetName(), NULL, false, this);
      proj->no_dialogs = false;
    }
    UpdateAfterEdit();
  }

  if (ea & (iClipData::EA_PASTE_APPEND)) {
    taBase* obj = ms->tabObject();
    bool ok; // dummy
    if (CheckError((!obj), false, ok,
        "Could not retrieve object from clipboard"))
      return iClipData::ER_ERROR;
    this->Append(obj);
    UpdateAfterEdit();
  }

  if (ea & (iClipData::EA_PASTE_COMPARE | iClipData::EA_DROP_COMPARE)) {
    taBase* obj = ms->tabObject();
    bool ok; // dummy
    if (CheckError((!obj), false, ok,
      "Could not retrieve object from clipboard"))
      return iClipData::ER_ERROR;
    this->DiffCompare(obj);
  }
  return 0;
}

// called twice: ms=NULL for SRC ops, ms=val for DST ops
void taBase::ChildQueryEditActions(const MemberDef* md, const taBase* child,
  taiMimeSource* ms, int& allowed, int& forbidden)
{
  if (ms && ms->isMulti()) {
    // for multi, a bit more complicated, since we need to not allow something
    // not allowed by all, which is not the same as forbid
    int allowed_accum = 0; // add allowed to this guy
    int allowed_knockout = -1; // subtract allowed from this guy
    for (int i = 0; i < ms->count(); ++i) {
      ms->setIndex(i);
      // try it for every item -- we only ultimately allow what is allowed for all items
      int item_allowed = 0;
      ChildQueryEditActions_impl(md, child, ms, item_allowed, forbidden);
      allowed_accum |= item_allowed;
      allowed_knockout &= item_allowed;
    }
    allowed |= (allowed_accum & allowed_knockout);
  } else {
    ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
  }
}

void taBase::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
  const taiMimeSource* ms, int& allowed, int& forbidden)
{
  // SRC
  // duplicate: note, don't test for parent-only query (child=NULL)
  if (child && ChildCanDuplicate(child))
    allowed |= iClipData::EA_DUPE;

  if (ms == NULL) return; // querying for src ops only

  // DST
  // if src action was Cut, that limits the Dst ops
  if (ms->srcAction() & (iClipData::EA_SRC_CUT))
   forbidden |= iClipData::EA_FORB_ON_SRC_CUT;

  if (isGuiReadOnly()) {
    forbidden |= (iClipData::EA_PASTE_INTO | iClipData::EA_DROP_ASSIGN |
      iClipData::EA_DROP_COPY_INTO | iClipData::EA_DROP_MOVE_INTO);
  }  
  // can't link etc. if not in this process
  if (!ms->isThisProcess())
    forbidden |= iClipData::EA_IN_PROC_OPS;
}

int taBase::ChildEditAction(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea)
{ 
  // determine the list-only operations allowed/forbidden, and apply to ea
  int rval = iClipData::ER_IGNORED;
  
  // SRC
  if (ea & iClipData::EA_SRC_OPS) { // dispatch on full ea, but only pass eax
    return ChildEditAction_impl(md, child, ms, ea);
  }
  
  
  // DST
  if (!ms) return rval; // shouldn't happen
  // note: single is just the degenerate case here
  
  // for in-process, we need to resolve all objects BEFORE
  // starting any operations, because the Path values in the ms
  // can easily become invalid as we do things like Move an object
  if (ms->isThisProcess()) {
    ms->ResolveObjects();
  }
  
  taProject* proj = (taProject*)GetThisOrOwner(&TA_taProject);
  if (proj) {
    proj->undo_mgr.Nest(true); 
  }
  bool multi = false;
  bool multi_off = false;
  if(ms->count() > 1) {
    ++taMisc::in_gui_multi_action;
    multi = true;
  }
  for (int i = 0; i < ms->count(); ++i) {
    if(multi && i == ms->count()-1) {
      --taMisc::in_gui_multi_action;
      multi_off = true;
    }
    ms->setIndex(i);
    rval = ChildEditAction_impl(md, child, ms, ea);
    // keep looping as long as stuff ok
    if (!(rval == iClipData::ER_OK || rval == iClipData::ER_CLEAR))
      break;
  }
  if(multi && !multi_off) // didn't reach end
    --taMisc::in_gui_multi_action;
  if (proj) {
    proj->undo_mgr.Nest(false); 
    iMainWindowViewer* imwv = iMainWindowViewer::GetViewerForObj(proj);
    if(imwv) {
      imwv->GetCurTreeView()->clearExtSelection(); // clear it
    }
  }

  return rval;
}

int taBase::ChildEditAction_impl(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea) 
{
    // op implementation (non-list/grp)
  if (ea & iClipData::EA_DUPE) {
    taProject* proj = (taProject*)GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(child, "Duplicate " + child->GetName(), NULL, false, this);
      // list is save_undo_owner
    }
    if (ChildDuplicate(child) != NULL)
      return iClipData::ER_OK;
    else return iClipData::ER_ERROR;
  }
  
  return 0;
}


//////////////////////////////////
//  taOBase (taList_impl)	//
//////////////////////////////////

void taOBase::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
  const taiMimeSource* ms, int& allowed, int& forbidden)
{
  // in general, we allow to CUT and DELETE -- inheriting class can forbid these if it wants
  // for the paste-like ops, we will generally allow insertions of compatible child
  // specific classes will need to replace this method to allow things like linking
  taList_impl* list = children_();
  if (list) {
    int item_idx = -1;
    if (child) item_idx = list->FindEl(child);
    // if it is a list item, or is null, then we can do list operations, so we call our L version
    if ((!child) || (item_idx >= 0))
      ChildQueryEditActionsL_impl(md, child, ms, allowed, forbidden);
  }
  inherited::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
  
/*no  // if child was a list item, then we don't pass the child to the base (since it doesn't boggle list items)
  if (item_idx >=0)
    inherited::ChildQueryEditActions_impl(md, NULL, ms, allowed, forbidden);
  else
    inherited::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden); */
}

void taOBase::ChildQueryEditActionsL_impl(const MemberDef* md, const taBase* lst_itm,
  const taiMimeSource* ms, int& allowed, int& forbidden)
{ //note: ONLY called if children_ valid/used
  taList_impl* list = children_();
  if (!list) return;
  // SRC ops
  if (lst_itm) {
    if (lst_itm->GetOwner() == list) {
      // only allow CUT in own list, otherwise can be confusing
      allowed |= iClipData::EA_CUT;
      // Delete only allowed if we are the owner
      allowed |= iClipData::EA_DELETE;
      // in general, allow duplicate
      //TODO: any forbidden cases? ex. read only?
//not yet implemented      allowed |= iClipData::EA_DUPE;
    } else { // otherwise, it is unlinking, not deleting
      allowed |= iClipData::EA_UNLINK;
    }
  }

  // DST ops

  taBase* obj = NULL;
  if (ms == NULL) return; // SRC only query
  // if not a taBase type of object, no more applicable
  if (!ms->isBase()) return;
  if (ms->isThisProcess()) {
    // cannot allow an object to be copied or moved into one of its own subobjects
    obj = ms->tabObject();
    if (this->IsChildOf(obj)) {
      // forbid both kinds of cases
      forbidden |= (iClipData::EA_DROP_COPY2 | iClipData::EA_DROP_MOVE2 |
                    iClipData::EA_PASTE2); //note: paste, regardless whether src cut or copy
    }
    // confusing to allow move of an obj on itself
    if (lst_itm && (lst_itm == obj)) {
      forbidden |= (iClipData::EA_DROP_COPY | iClipData::EA_DROP_MOVE);
      if ((ms->srcAction() & iClipData::EA_SRC_CUT))
        forbidden |= iClipData::EA_PASTE;
    }
  } else {
    forbidden |= iClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for L action calls
  }
  
  bool right_type = list->IsAcceptable(obj);
  
  // figure out what would be allowed if right type
  int op = 0;
  //note: we no longer allow linking generally, only into LINK_GROUPs
  if (md && md->HasOption("LINK_GROUP")) {
    if (lst_itm) {
      op |= (iClipData::EA_LINK | iClipData::EA_DROP_LINK);
    } else {
      op |= (iClipData::EA_LINK_INTO | iClipData::EA_DROP_LINK_INTO);
    }
  } else {
    if (lst_itm) {
      op |= (iClipData::EA_PASTE | iClipData::EA_DROP_COPY |
        iClipData::EA_DROP_MOVE);
    } else {
      op |= (iClipData::EA_PASTE_INTO | iClipData::EA_DROP_COPY_INTO |
        iClipData::EA_DROP_MOVE_INTO);
    }
  }

  if (right_type) {
    allowed |= op;
  }
  //TODO: some groups allow promiscuous linking -- they must enable such themselves
}

int taOBase::ChildEditAction_impl(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea)
{ 
  int rval = iClipData::ER_IGNORED;
  int allowed = 0;
  int forbidden = 0;
  // we will want list idx of child (if exists)
  taList_impl* list = children_();
  // give first priority to list ops
  if (list) {
    int item_idx = -1;
    if (child) item_idx = list->FindEl(child);
    
    // is it a list item, or op on the list?
    if ((!child) || (item_idx >= 0)) {
      ChildQueryEditActionsL_impl(md, child, ms, allowed, forbidden);
      int eax = (ea & (allowed & (~forbidden)));
      if (eax) { // some list op to do...
        // SRC -- not need to decode SRC on full ea, not the eax
        if (ea & iClipData::EA_SRC_OPS) {
          return ChildEditActionLS_impl(md, child, eax);
        } else { // DST
          if (!ms) return rval; // shouldn't happen
          if (ms->isThisProcess()) {
            rval = ChildEditActionLD_impl_inproc(md, child, ms, eax);
          } else {
            // DST OP, SRC OUT OF PROCESS
            rval = ChildEditActionLD_impl_ext(md, child, ms, eax);
          }
          return rval;
        }
      }
    }
  }
  
  return inherited::ChildEditAction_impl(md, child, ms, ea);
}


int taOBase::ChildEditActionLS_impl(const MemberDef* md, taBase* lst_itm, int ea) {
//only called if list valid and enabled
  taList_impl* list = children_();
  if (!list) return iClipData::ER_IGNORED;
  if (lst_itm == NULL) return iClipData::ER_IGNORED;
  if (list->HasOption("FIXED_SIZE")) return iClipData::ER_IGNORED;
  
  switch (ea & iClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself, or outer controller if multi
  case iClipData::EA_UNLINK: {
    taProject* proj = (taProject*)list->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(lst_itm, "Remove", NULL, false, this);
      // list is save_undo_owner
    }
    list->RemoveEl(lst_itm);
    return iClipData::ER_OK;
  }
  // for COPY, CUT, DELETE, and DRAG, nothing more for us to do; just ack
  // (either handled by outer controller, or dest)
  case iClipData::EA_COPY:
  case iClipData::EA_CUT:
  case iClipData::EA_DELETE: //note: handled by controller
  case iClipData::EA_DRAG:
    return iClipData::ER_OK;
  default: break; // compiler food
  }
  return iClipData::ER_IGNORED;
}

int taOBase::ChildEditActionLD_impl_inproc(const MemberDef* md,
                                           taBase* lst_itm, taiMimeSource* ms, int ea)
{
  taList_impl* list = children_();
  if (!list) return iClipData::ER_IGNORED;
  if (list->HasOption("FIXED_SIZE")) return iClipData::ER_IGNORED;
  int itm_idx = list->FindEl(lst_itm); // -1 if NULL ie at end
  
  taProject* proj = dynamic_cast<taProject*>(list->GetThisOrOwner(&TA_taProject));
  
  // itm_idx, -1 means parent itself
  int obj_idx = -1; // -1 means not in this list
  taBase* obj = NULL;
  
  //NOTE: OP/OP_INTO are implicitly encoded via existence of lst_itm so
  //  we always lump the two variants together (OP2)
  // only fetch obj for ops that require it
  if (ea & (iClipData::EA_PASTE2 | iClipData::EA_LINK2  | iClipData::EA_DROP_COPY2 |
            iClipData::EA_DROP_LINK2 | iClipData::EA_DROP_MOVE2))
  {
    obj = ms->tabObject();
    if(TestError(!obj, "ChildEditActionLD_impl_inproc",
                 "Could not retrieve object for operation")) return iClipData::ER_ERROR;
    // already in this list? (affects how we do drops/copies, etc.)
    obj_idx = list->FindEl(obj);
  }
  
  
  // All non-move paste ops (i.e., copy an object)
  if (
      (ea & (iClipData::EA_DROP_COPY2)) ||
      //  Cut/Paste svn is a move
      ((ea & iClipData::EA_PASTE2) && (ms->srcAction() & iClipData::EA_SRC_COPY))
      ) {
    if(proj) {
      proj->undo_mgr.SaveUndo(list, "Paste/Copy " + obj->GetName(), NULL, false, this);
      // list is save_undo_owner
    }
    taBase* new_obj = obj->MakeToken();
    // if dest is list itself, then targ item is the virtual new item (end+1)
    if (itm_idx < 0)
      itm_idx = list->size;
    list->Insert(new_obj, itm_idx); // at end if itm_idx=size
    new_obj->UnSafeCopy(obj);	// always copy after inserting so there is a full path & initlinks
    
    // retain the name if being copied from outside the list, otherwise give new name
    if (obj_idx < 0)
      new_obj->SetName(obj->GetName());
    else
      new_obj->SetDefaultName(); // should give it a new name, so not confused with existing obj
    // do a full UAE (not just DC) so associated update code gets retriggered
    new_obj->UpdateAfterEdit();
    tabMisc::DelayedFunCall_gui(new_obj, "BrowserSelectMe");
    if (obj->GetOwner()->GetName() == "templates") {  // only if dropped from toolbar
      tabMisc::DelayedFunCall_gui(new_obj, "ChooseMe");  // pop scoped chooser
      tabMisc::DelayedFunCall_gui(new_obj, "BrowserSelectMe");  // some cases require reselection because focus lost when changing type
    }
    return iClipData::ER_OK;
  }
  
  // All Move-like ops
  if (
      (ea & (iClipData::EA_DROP_MOVE2)) ||
      //  Cut/Paste is a move
      ((ea & iClipData::EA_PASTE2) && (ms->srcAction() & iClipData::EA_SRC_CUT))
      ) {
    if (obj == lst_itm)
      return 1; // nop
    if(proj) {
      proj->undo_mgr.SaveUndo(obj, "Move " + obj->GetName(), NULL, false, this);
      // list is save_undo_owner
    }
    if (obj_idx >= 0) { // in this list: just do a list move
      list->MoveBeforeIdx(obj_idx, itm_idx); // noop for self ops
      return iClipData::ER_CLEAR; // do nothing case of drop on self
    }
    else
    { // not in this list, need to do a transfer
      if (list->Transfer(obj)) { // should always succeed -- only fails if we already own item
        // was added at end, fix up location, if necessary
        if (itm_idx >= 0) { // if <0, then means "at end" already
          // for fm>to, to will just be the dst, because fm pushes to down
          list->MoveIdx(list->size - 1, itm_idx);
        }
        tabMisc::DelayedFunCall_gui(obj, "BrowserSelectMe");      }
      else {
        return iClipData::ER_ERROR;
      }
    }
    return iClipData::ER_CLEAR;
  }
  
  // Link ops
  if (ea &
      (iClipData::EA_LINK2 | iClipData::EA_DROP_LINK2))
  {
    if (obj_idx >= 0)
      return -1; // in this list: link forbidden
    if(proj) {
      proj->undo_mgr.SaveUndo(obj, "Insert Link", NULL, false, this);
      // list is save_undo_owner
    }
    if (itm_idx < 0)
      itm_idx = list->size;
    list->InsertLink(obj, itm_idx);
    return iClipData::ER_OK;
  }
  return iClipData::ER_IGNORED;
}

int taOBase::ChildEditActionLD_impl_ext(const MemberDef* md,
  taBase* lst_itm, taiMimeSource* ms, int ea) 
{
  taList_impl* list = children_();
  if (!list) return iClipData::ER_IGNORED;
  if (list->HasOption("FIXED_SIZE")) return iClipData::ER_IGNORED;
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return iClipData::ER_IGNORED;
//Note: the OP/OP_INTO is actually encoded in lst_itm NULL or not, so we lump together
  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  if (ea & (iClipData::EA_DROP_COPY2 |
    // iClipData::EA_DROP_MOVE | //can't do move across processes
     iClipData::EA_PASTE2))
  {
    istringstream istr;
    if (ms->objectData(istr) > 0) {
      taProject* proj = dynamic_cast<taProject*>(list->GetThisOrOwner(&TA_taProject));
      if(proj) {
	proj->undo_mgr.SaveUndo(list, "Paste/Copy", NULL, false, this);
	// list is save_undo_owner
      }
      TypeDef* td = list->GetTypeDef();
      void* new_el_ = NULL; // the dude added
      int dump_val = td->Dump_Load(istr, list, list, &new_el_);
      if (dump_val == 0) {
        //TODO: error output
        return iClipData::ER_ERROR; // load failed
      }
      // ok, now move the guy into the right place
      taBase* new_el = (taBase*)new_el_;
      list->MoveBefore(lst_itm, new_el);
      // call UAE to trigger associate update code, esp in parent lists etc.
      new_el->UpdateAfterEdit();
      return iClipData::ER_OK;
    } else { // no data
        //TODO: error output
      return iClipData::ER_ERROR;
    }
  }
  return iClipData::ER_IGNORED;
}


//////////////////////////////////
//  taGroup			//
//////////////////////////////////


void taGroup_impl::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child,
  const taiMimeSource* ms, int& allowed, int& forbidden)
{
  int subgrp_idx = -1;
  if (child) subgrp_idx = gp.FindEl(child);

  // if it is a group item, or is null, then we can do group operations, so we call our G version
  if ((child == NULL) || (subgrp_idx >= 0))
    ChildQueryEditActionsG_impl(md, (taGroup_impl*)child, ms, allowed, forbidden);

  taList_impl::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
}

void taGroup_impl::ChildQueryEditActionsG_impl(const MemberDef* md,
  taGroup_impl* subgrp, const taiMimeSource* ms, int& allowed, int& forbidden)
{
  // SRC ops
  if (subgrp) {
    // Delete only allowed if we are the owner
    if (subgrp->GetOwner() == &gp) {
      // only allow CUT in own context, to avoid confusion
      allowed |= iClipData::EA_CUT;
      allowed |= iClipData::EA_DELETE;
    } else { // otherwise, it is unlinking, not deleting
      allowed |= iClipData::EA_UNLINK;
    }
  }

  if (ms == NULL) return; // src op query
  
  // DST ops
  // if not a taBase type of object, no more applicable
  if (!ms->isBase()) return;
  if (!ms->isThisProcess())
    forbidden |= iClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls
  
  if (ms->isThisProcess()) {
    // cannot allow a group to be copied or moved into one of its own subgroups
    taGroup_impl* grp = static_cast<taGroup_impl*>(ms->tabObject());
    // the grp must not be this one, or parent to it
    if (this->IsChildOf(grp)) {
      // forbid both kinds of cases
      forbidden |= (iClipData::EA_DROP_COPY2 | iClipData::EA_DROP_MOVE2 |
        iClipData::EA_PASTE2); //note: paste, regardless whether src cut or copy
    }
    // confusing to allow move of an obj on itself
    if (subgrp && (subgrp == grp)) {
      forbidden |= (iClipData::EA_DROP_COPY | iClipData::EA_DROP_MOVE);
      if ((ms->srcAction() & iClipData::EA_SRC_CUT))
        forbidden |= iClipData::EA_PASTE;
    }
  } else {
    forbidden |= iClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls
  }
  TypeDef* td = ms->td();
  bool right_gptype = (td && (td->InheritsFrom(gp.el_base)) ); // only base type of subgps is what matters

  // find relevant ops 
  int op = 0;
  if (subgrp) {
    op |= (iClipData::EA_PASTE | iClipData::EA_DROP_COPY |
      iClipData::EA_DROP_MOVE);
  } else {
    op |= (iClipData::EA_PASTE_INTO | iClipData::EA_DROP_COPY_INTO |
      iClipData::EA_DROP_MOVE_INTO);
  }
  if (right_gptype) {
    allowed |= op;
  }
}

// called by a child -- lists etc. can then allow drops on the child, to indicate inserting into the list, etc.
int taGroup_impl::ChildEditAction_impl(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea) 
{
  int rval = iClipData::ER_IGNORED;
  // if child exists, but is not a group item, then just delegate down to base
  int subgrp_idx = -1;
  if (child)
    subgrp_idx = gp.FindEl(child);
    
  // group actions are eligible if group parent op, or child group
  if ((!child) || (subgrp_idx >= 0)) {
  
    // we will be calling our own G routines...
    // determine the group-only operations allowed/forbidden, and apply to ea
    int allowed = 0;
    int forbidden = 0;
    ChildQueryEditActionsG_impl(md, (taGroup_impl*)child, ms, allowed, forbidden);
    int eax = (ea & (allowed & (~forbidden)));
  
    if (eax) { // ok, it is a group operation
      if (ea & iClipData::EA_SRC_OPS) { // note: must use original ea to decode SRC
        rval = ChildEditActionGS_impl(md, (taGroup_impl*)child, eax);
      } else {
        if (ms == NULL) return iClipData::ER_IGNORED; // shouldn't happen
        // decode src location
        if (ms->isThisProcess())
          rval = ChildEditActionGD_impl_inproc(md, (taGroup_impl*)child, ms, eax);
        else
          // DST OP, SRC OUT OF PROCESS
          rval = ChildEditActionGD_impl_ext(md, (taGroup_impl*)child, ms, eax);
      }
      return rval;
    }
  }
  return inherited::ChildEditAction_impl(md, child, ms, ea);
}

int taGroup_impl::ChildEditActionGS_impl(const MemberDef* md, taGroup_impl* subgrp, int ea)
{
  int subgrp_idx = gp.FindEl(subgrp);
  // if the child is a group, we handle it, otherwise we let base class handle it
  switch (ea & iClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself
  case iClipData::EA_CUT: return 1; //nothing to do, just acknowledge -- deletion triggered by the dst, whether local or remote
  case iClipData::EA_DELETE: {
    if (subgrp) {
      taProject* proj = dynamic_cast<taProject*>(GetThisOrOwner(&TA_taProject));
      if(proj) {
	proj->undo_mgr.SaveUndo(this, "Remove", NULL, false, this);
	// list is save_undo_owner
      }
      RemoveGpIdx(subgrp_idx);
      return iClipData::ER_OK;
    } else return iClipData::ER_ERROR; // error TODO: error message
  }
  case iClipData::EA_DRAG: return iClipData::ER_OK; // nothing for us to do on the drag
  default: break; // compiler food
  }
  return iClipData::ER_IGNORED; // this function never calls down to List
}

int taGroup_impl::ChildEditActionGD_impl_inproc(const MemberDef* md, taGroup_impl* subgrp,
  taiMimeSource* ms, int ea)
{
  int subgrp_idx = gp.FindEl(subgrp);
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return iClipData::ER_IGNORED;
  int srcgrp_idx = -1; // -1 means not in this group
  taBase* srcobj = NULL;

  taProject* proj = dynamic_cast<taProject*>(GetThisOrOwner(&TA_taProject));

//NOTE: OP/OP_INTO are implicitly encoded via existence of lst_itm so 
//  we always lump the two variants together (OP2)
  // only fetch obj for ops that require it
  if (ea & (iClipData::EA_PASTE2 | iClipData::EA_LINK2  | iClipData::EA_DROP_COPY2 |
    iClipData::EA_DROP_LINK2 | iClipData::EA_DROP_MOVE2))
  {
    srcobj = ms->tabObject();
    if (srcobj == NULL) {
      taMisc::Error("Could not retrieve object for operation.");
      return iClipData::ER_ERROR;
    }
    // already in this list? (affects how we do drops/copies, etc.)
    srcgrp_idx = gp.FindEl(srcobj);
  }

  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (iClipData::EA_DROP_COPY2)) ||
    //  Cut/Paste is a move
    ((ea & iClipData::EA_PASTE2) && (ms->srcAction() & iClipData::EA_SRC_COPY))
  ) {
    if(proj) {
      proj->undo_mgr.SaveUndo(this, "Paste/Copy", NULL, false, this);
      // list is save_undo_owner
    }
    taBase* new_obj = srcobj->MakeToken();
    if (subgrp_idx < 0) 
      subgrp_idx = gp.size; // // at end if itm_idx=size
    gp.Insert(new_obj, subgrp_idx);
    new_obj->UnSafeCopy(srcobj);	// always copy after inserting so there is a full path & initlinks
    // retain the name if being copied from outside the gp, otherwise give new name
    if (srcgrp_idx < 0)
      new_obj->SetName(srcobj->GetName());
    else
      new_obj->SetDefaultName(); // should give it a new name, so not confused with existing obj 
    // do a full UAE (not just DC) so associated update code gets retriggered
//nn for gps    new_obj->UpdateAfterEdit();
    return iClipData::ER_OK;
  } 
  
  // All Move-like ops
  if (
    (ea & (iClipData::EA_DROP_MOVE2)) ||
    //  Cut/Paste is a move
    ((ea & iClipData::EA_PASTE2) && (ms->srcAction() & iClipData::EA_SRC_CUT))
  ) {
    if (srcobj == subgrp) return iClipData::ER_OK; // nop
    if(proj) {
      proj->undo_mgr.SaveUndo(srcobj, "Move", NULL, false, this);
      // list is save_undo_owner
    }
    if (srcgrp_idx >= 0) { // in this group: just do a group move
      gp.MoveBeforeIdx(srcgrp_idx, subgrp_idx); // ok if -1, means end
      return iClipData::ER_OK; // do nothing case of drop on self
    } else { // not directly in this group, need to do a transfer
      if (gp.Transfer(srcobj)) { // should always succeed -- only fails if we already own item
      // was added at end, fix up location, if necessary
        if (subgrp_idx >= 0) { // if <0, then means "at end" already
          gp.MoveIdx(gp.size - 1, subgrp_idx);
        }
      } else return iClipData::ER_ERROR; //TODO: error message
    }
    return iClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (iClipData::EA_LINK2 | iClipData::EA_DROP_LINK2))
  {
    if (srcgrp_idx >= 0) return iClipData::ER_FORBIDDEN; // in this list: link forbidden
    if(proj) {
      proj->undo_mgr.SaveUndo(srcobj, "Insert Link", NULL, false, this);
      // list is save_undo_owner
    }
    if (subgrp_idx < 0) 
      subgrp_idx = gp.size; 
    gp.InsertLink(srcobj, subgrp_idx);
    return iClipData::ER_OK;
  }
  return iClipData::ER_IGNORED;
}

int taGroup_impl::ChildEditActionGD_impl_ext(const MemberDef* md, taGroup_impl* subgrp, taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return iClipData::ER_IGNORED;

// Note on streaming:
// For Paste Here, we have to stream into ourself
//Note: the OP/OP_INTO is actually encoded in lst_itm NULL or not, so we lump together
  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  if (ea & (iClipData::EA_DROP_COPY2 |
    // iClipData::EA_DROP_MOVE | //can't do move across processes
     iClipData::EA_PASTE2))
  {
    istringstream istr;
    if (ms->objectData(istr) > 0) {
      TypeDef* td = GetTypeDef();
      // we need to add the new group BEFORE streaming!
      taGroup_impl* new_gp = NewGp_(1, td);
      bool ok; // dummy
      if (CheckError((!new_gp), false, ok,
        "Could not make new group for TypeDef:", td->name)) 
        return iClipData::ER_ERROR; // load failed
      // move it now, before the load
      taProject* proj = (taProject*)GetOwner(&TA_taProject);
      if(proj) {
	proj->undo_mgr.SaveUndo(this, "Move", NULL, false, this);
	// list is save_undo_owner
      }
      gp.MoveBefore(subgrp, new_gp);
      taBase* ld_par = this;
      int dump_val = td->Dump_Load(istr, new_gp, ld_par/*, &new_el_*/);
      
      if (CheckError((dump_val == 0), false, ok,
        "Load error for TypeDef:" + td->name)) 
        return iClipData::ER_ERROR; // load failed
      return iClipData::ER_OK;
    } else { // no data
      return iClipData::ER_ERROR; //TODO: error message
    }
  }
  return iClipData::ER_IGNORED;
}

//////////////////////////
//      taDataView

void taDataView::DV_QueryEditActionsS(int& allowed, int& forbidden)
{
  if (!isTopLevelView()) {
    forbidden |= (iClipData::EA_CUT | 
      iClipData::EA_DUPE | iClipData::EA_DELETE | 
      iClipData::EA_CLEAR | iClipData::EA_UNLINK);
  }
}
 
void taDataView::DV_QueryEditActionsD(taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  // note: don't need to forbid anything at item level, most controlled by owner
}

void taDataView::DV_ChildQueryEditActions(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  // nothing special in default
}

void taDataView::QueryEditActionsS_impl(int& allowed, int& forbidden)
{
  inherited::QueryEditActionsS_impl(allowed, forbidden);
  DV_QueryEditActionsS(allowed, forbidden);
}

void taDataView::QueryEditActionsD_impl(taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  inherited::QueryEditActionsD_impl(ms, allowed, forbidden);
  DV_QueryEditActionsD(ms, allowed, forbidden);
}

void taDataView::ChildQueryEditActions_impl(const MemberDef* md,
    const taBase* child, const taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  inherited::ChildQueryEditActions_impl(md,
    child, ms, allowed, forbidden);
  DV_ChildQueryEditActions(md, child, ms, allowed, forbidden);
}

void DataView_List::DV_ChildQueryEditActionsL_impl(const MemberDef* md,
  const taBase* lst_itm, const taiMimeSource* ms,
  int& allowed, int& forbidden) 
{
  bool is_root_level_view = (data_view && data_view->isRootLevelView());
  // unless we are ourself a root, then pretty much deny everything writish
  // into us
  // note that refusing to forbid will still require the normal type
  // checks, so that only compatible guys can get pasted
  if (!is_root_level_view) {
    forbidden |= (iClipData::EA_PASTE_INTO | iClipData::EA_PASTE_APPEND |
      iClipData::EA_LINK_INTO | iClipData::EA_DROP_COPY_INTO |
      iClipData::EA_DROP_LINK_INTO | iClipData::EA_DROP_MOVE_INTO);
    if (lst_itm) // forbid on top of guys too
      forbidden |= (iClipData::EA_PASTE | iClipData::EA_PASTE_APPEND |
        iClipData::EA_LINK | iClipData::EA_DROP_COPY |
        iClipData::EA_DROP_LINK | iClipData::EA_DROP_MOVE);
  }
}

void DataView_List::ChildQueryEditActionsL_impl(const MemberDef* md,
  const taBase* lst_itm, const taiMimeSource* ms,
  int& allowed, int& forbidden) 
{
  inherited::ChildQueryEditActionsL_impl(md,
    lst_itm, ms, allowed, forbidden);
  DV_ChildQueryEditActionsL_impl(md, lst_itm, ms,
    allowed,  forbidden);
}



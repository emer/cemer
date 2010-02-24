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


#include "ta_qtclipdata.h"

#include "ta_platform.h"

#include "ta_type.h"
#include "ta_matrix.h"
#include "ta_datatable.h"
#include "ta_project.h"
#include "ta_qtviewer.h"

#include <qapplication.h>
#include <qclipboard.h>
#include <QByteArray>

#include <sstream>

using namespace std;


/* Tabular clip operations

take as example:
col1: scalar alpha
col2: 2x3 mat
col3: 4x5 mat


so for the above, assume you copied all three cols and 2 rows, you would end up with a spreadsheet of

1 + 3 + 5 = 9 cols

2 x (4) = 8 rows

because 4 is the max row dimension

like this:

str1 c2:0,0 c2:0,1 c2:0,2 c3:0,0 ... c3:0,4
     c2:1,0 ...
                          c3:2,0 ...
                          c3:3,0 ...
str2 ...


*/

//////////////////////////////////
// 	taiClipData		//
//////////////////////////////////

const String taiClipData::tacss_common("tacss/common");

taiClipData::EditAction taiClipData::ClipOpToSrcCode(int ea) {
  switch (ea & EA_OP_MASK) {
  case EA_CUT:  return EA_SRC_CUT;
  case EA_COPY: return EA_SRC_COPY;
  case EA_DRAG: return EA_SRC_DRAG;
  default:      return (EditAction)0;
  }
}

taiClipData::taiClipData(int src_edit_action_)
:inherited()
{
  src_edit_action = src_edit_action_;
  setData(tacss_common, taiMimeFactory::StrToByteArray(GetCommonDescHeader()));
}


const QString taiClipData::GetCommonDescHeader() const {
  QString rval =  QString::number(src_edit_action) + ';' +
        QString::number(taPlatform::processId()) + ";";
  return rval;
}

void taiClipData::setTextData(const QString& mime_type, const QString& data_) {
  setData(mime_type, taiMimeFactory::StrToByteArray(data_));
}

void taiClipData::setTextFromStr(const String& str) {
  // sets text without doubly converting from String->QString->ByteArray
  setData(taiMimeFactory::text_plain, taiMimeFactory::StrToByteArray(str));
}


//////////////////////////////////
//  taiMimeFactory		//
//////////////////////////////////

const String taiMimeFactory::text_plain("text/plain");

QByteArray taiMimeFactory::StrToByteArray(const String& str) {
  return QByteArray(str.chars());
}

QByteArray taiMimeFactory::StrToByteArray(const QString& str) {
  return str.toLatin1();
}

//////////////////////////////////
//  taiMimeFactory_List		//
//////////////////////////////////

taiMimeFactory_List* taiMimeFactory_List::g_instance; 

void taiMimeFactory_List::setInstance(taiMimeFactory_List* value)
{
  g_instance = value;
}

taiMimeFactory* taiMimeFactory_List::StatGetInstanceByType(TypeDef* td) {
  taiMimeFactory_List* inst = instance();
  if (inst) return inst->GetInstanceByType(td);
  else return NULL;
}

void taiMimeFactory_List::Destroy() {
  if (g_instance == this) g_instance = NULL;
}

taiMimeFactory* taiMimeFactory_List::GetInstanceByType(TypeDef* td) {
  if (!td) return NULL;
  if (!td->DerivesFrom(&TA_taiMimeFactory)) return NULL;
  taiMimeFactory* rval = NULL;
  for (int i = 0; i < size; ++i) {
    rval = FastEl(i);
    //NOTE: if comparing of objects fails for some reason, compare names
    if (rval->GetTypeDef()->GetNonPtrType() == td)
      return rval;
  }
  // not here yet, so make one
  rval = (taiMimeFactory*)New(1, td);
  return rval;
}


//////////////////////////////////
//  taiObjectMimeFactory	//
//////////////////////////////////

const String taiObjectMimeFactory::tacss_objectdesc("tacss/objectdesc");
const String taiObjectMimeFactory::tacss_objectdata("tacss/objectdata");

void taiObjectMimeFactory::AddSingleObject(QMimeData* md, taBase* obj) {
  if (!obj) return;
  QString str;
  InitHeader(1, str);
  AddHeaderDesc(obj, str);
  md->setData(tacss_objectdesc, StrToByteArray(str));
  AddObjectData(md, obj, 0);
}

void taiObjectMimeFactory::AddMultiObjects(QMimeData* md,
   taPtrList_impl* obj_list)
{
  if (!obj_list) return;
  QString str;
  InitHeader(obj_list->size, str);
  // note: prob not necessary, but we iterate twice so header precedes data
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddHeaderDesc(obj, str);
  }
  md->setData(tacss_objectdesc, StrToByteArray(str));
  for (int i = 0; i < obj_list->size; ++i) {
    taBase* obj = (taBase*)obj_list->FastEl_(i); // better damn well be a taBase list!!!
    AddObjectData(md, obj, i);
  }
}

void taiObjectMimeFactory::AddObjectData(QMimeData* md, taBase* obj, int idx) {
    ostringstream ost;
    obj->Save_strm(ost);
    String str = tacss_objectdata + ";index=" + String(idx);
    md->setData(str, QByteArray(ost.str().c_str()));
}


void taiObjectMimeFactory::AddHeaderDesc(taBase* obj, QString& str) {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  str = str + obj->GetTypeDef()->name.toQString() + ";" +
        obj->GetPath().toQString() + ";\n"; // we put ; at end to facilitate parsing, and for }
}

void taiObjectMimeFactory::InitHeader(int cnt, QString& str) {
  str = QString::number(cnt) + ";\n";
}


//////////////////////////////////
//  taiMimeItem 		//
//////////////////////////////////

int taiMimeItem::data(const QMimeData* md, const QString& mimeType,
  taString& result) 
{
  QByteArray ba = md->data(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

taiMimeItem* taiMimeItem::ExtractByType(TypeDef* td, taiMimeSource* ms, 
    const String& mimetype)
{
  if (!td->DerivesFrom(&TA_taiMimeItem)) return false;
  taiMimeItem* inst = (taiMimeItem*)td->GetInstance();
  if (!inst) return false;
  return inst->Extract(ms, mimetype);
}

void taiMimeItem::Initialize() {
  m_index = -1;
  m_flags = 0;
  m_ms = NULL;
}

void taiMimeItem::AssertData() {
//TODO: not if zombie!
  if (!isDecoded()) {
    DecodeData_impl();
    m_flags |= MIF_DECODED;
  }
}

bool taiMimeItem::Constr(taiMimeSource* ms_, const String& mimetype) {
  m_ms = ms_;
  bool rval = Constr_impl(mimetype); // NOTE: mt is often nil (default)
  if (!rval)
    m_flags |= MIF_ZOMBIE;
  return rval;
}

QByteArray taiMimeItem::data(const QString& mimeType) const {
  if (m_ms) return m_ms->data(mimeType);
  else return QByteArray();
}
/*
int taiMimeItem::data(const QString& mimeType, taString& result) const {
  QByteArray ba = data(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

int taiMimeItem::data(const QString& mimeType, istringstream& result) const {
  QByteArray ba = data(mimeType);
  result.str(string(ba.data(), ba.size()));
  return ba.size();
}
*/
bool taiMimeItem::isThisProcess() const {
  if (m_ms) return m_ms->isThisProcess();
  else return false;
}

const QMimeData* taiMimeItem::mimeData() const {
 if (m_ms) return m_ms->mimeData();
  else return NULL;
}

//////////////////////////////////
// 	taiMimeItem_List	//
//////////////////////////////////

//////////////////////////////////
//  taiMultiMimeItem	//
//////////////////////////////////

void taiMultiMimeItem::Initialize() {
}

//////////////////////////////////
//  taiMultiMimeItem 		//
//////////////////////////////////

void taiMultiMimeItem::InitLinks() {
  inherited::InitLinks();
  taBase::Own(&items, this);
}

void taiMultiMimeItem::CutLinks() {
  items.CutLinks();
  inherited::CutLinks();
}


//////////////////////////////////
// 	taiObjectMimeItem 	//
//////////////////////////////////

void taiObjectMimeItem::Initialize() {
  m_td = NULL;
  m_obj = NULL;
}

bool taiObjectMimeItem::Constr_impl(const String&) {
  m_td = taMisc::types.FindName(m_type_name); // note: could be NULL if, ex. decoding leabra object in instance of bp
  return (m_td);
}

void taiObjectMimeItem::DecodeData_impl() {
  MemberDef* md;
  if (isThisProcess() && (!m_obj) && tabMisc::root) {
    m_obj = tabMisc::root->FindFromPath(path(), md);
  }
}

int taiObjectMimeItem::GetObjectData(istringstream& result) {
  AssertData();
  if (m_ms) {
    QString fmt = taiObjectMimeFactory::tacss_objectdata + ";index=" + String(GetIndex());
    return m_ms->data(fmt, result);
  } else {
    return 0;
  }
}


taBase* taiObjectMimeItem::obj() {  // note: only called when we are InProcess
  AssertData();
  return m_obj;
}


//////////////////////////////////
//  taiObjectsMimeItem		//
//////////////////////////////////

void taiObjectsMimeItem::Initialize() {
  m_all_base = 0;
  items.SetBaseType(&TA_taiObjectMimeItem);
}

taiMimeItem* taiObjectsMimeItem::Extract(taiMimeSource* ms, 
    const String& subkey)
{
  if (!ms->hasFormat(taiObjectMimeFactory::tacss_objectdesc))
    return NULL;
    
  taiObjectsMimeItem* rval = new taiObjectsMimeItem;
  rval->Constr(ms, subkey);
  return rval;
}

bool taiObjectsMimeItem::Constr_impl(const String&) {
  String arg;
  data(mimeData(), taiObjectMimeFactory::tacss_objectdesc, arg);
  
  // get header and class info
  String str;
  str = arg.before(';');
  bool ok;
  int itm_cnt = str.toInt(&ok);
  if (!ok) goto fail;
  arg = arg.after(";\n");
  
  for (int i = 0; i < itm_cnt; ++i) {
    // decode type and get the typedef obj
    String typeName = arg.before(';');
    if (typeName.length() == 0) goto fail;
    arg = arg.after(';');

    // decode path -- could be empty for non-taBase object
    String path = arg.before(';');
    arg = arg.after(';');
    //TODO: skip extension data
    taiObjectMimeItem* msd = (taiObjectMimeItem*) items.New(1);
    msd->m_type_name = typeName;
    msd->m_path = path;
    msd->Constr(ms());
    arg = arg.after('\n');
  }
  return true;

fail:
  items.Reset();
  return false;
}

bool taiObjectsMimeItem::allBase() const {
  if (m_all_base == 0) 
    CommonSubtype(); // asserts it
  return (m_all_base == 1);
}

TypeDef* taiObjectsMimeItem::CommonSubtype() const {
  m_all_base = -1; // assume not
  if (count() == 0) return NULL;
  TypeDef* rval = item(0)->td();
  for (int i = 1; (rval && (i < count())); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, 
      ((taiObjectMimeItem*)item(i))->td());
  }
  if (rval && rval->InheritsFrom(&TA_taBase))
    m_all_base = 1;
  return rval;
}


//////////////////////////////////
//  taiMimeSource		//
//////////////////////////////////

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
#ifdef DEBUG
  taMisc::Warning("taiMimeSource::m_md destroyed before clip operations completed!");
#endif
}

taiObjectsMimeItem* taiMimeSource::objects() const {
  if (!mi)
    mi = (taiObjectsMimeItem*)const_cast<taiMimeSource*>(this)->GetMimeItem(&TA_taiObjectsMimeItem);
  return mi; // could be NULL if no tacss
}



////////// Compatability methods follow:


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
  if (data(taiClipData::tacss_common, str) > 0) {
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
  m_this_proc = (process_id == taPlatform::processId());
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
    taiObjectMimeItem* mi = omi->item(i);
    mi->obj(); // asserts
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
    forbidden |= (taiClipData::EA_PASTE_ASSIGN | taiClipData::EA_DROP_ASSIGN);
  // not allowed on ro
  if (isGuiReadOnly()) {
    forbidden |= (taiClipData::EA_PASTE_ASSIGN | 
      taiClipData::EA_DROP_ASSIGN);
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
  allowed |= taiClipData::EA_COPY;
  if (isGuiReadOnly()) {
    forbidden |= (taiClipData::EA_CLEAR);
  }
  // simple convention to forbid clip ops for things like Wizards: 
  if (HasUserData("NO_CLIP")) 
    forbidden |= taiClipData::EA_SRC_OPS;  
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
    if (CanCopy(obj))
      allowed |= (taiClipData::EA_PASTE_ASSIGN | taiClipData::EA_DROP_ASSIGN);
  }
}

int taBase::EditAction(taiMimeSource* ms, int ea)
{ 
  if (ea & taiClipData::EA_SRC_OPS) {
    return EditActionS_impl(ea);
  }
  
  // DST
  int rval = taiClipData::ER_IGNORED;
  if (!ms) return rval; // shouldn't happen
  // note: single is just the degenerate case here
  // we requery to be absolutely sure 
  int allowed = 0;
  int forbidden = 0;
  QueryEditActions(ms, allowed, forbidden);
  int eax = ea & (allowed & (~forbidden));
  if (!eax) {
    return taiClipData::ER_ERROR; // prob an error!
  } 
  for (int i = 0; i < ms->count(); ++i) {
    ms->setIndex(i);
    // do it for every item 
    rval = EditActionD_impl(ms, eax);
    // keep looping as long as stuff ok
    if (rval != taiClipData::ER_OK)
      return rval;
  }
  return rval;
}

int taBase::EditActionS_impl(int ea) {
  //note: follows same logic as the Query
  if (ea & taiClipData::EA_COPY) return 1; //note: Ui actually puts the data on the clipboard, when it sees the 1

  return 0;
}

int taBase::EditActionD_impl(taiMimeSource* ms, int ea) {
  //TODO: decode AssignTo
  if (ea & (taiClipData::EA_PASTE_ASSIGN | taiClipData::EA_DROP_ASSIGN)) {
    taBase* obj = ms->tabObject();
    bool ok; // dummy
    if (CheckError((!obj), false, ok,
      "Could not retrieve object from clipboard"))
      return taiClipData::ER_ERROR;
    this->Copy(obj);
    UpdateAfterEdit();
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
    allowed |= taiClipData::EA_DUPE;

  if (ms == NULL) return; // querying for src ops only

  // DST
  // if src action was Cut, that limits the Dst ops
  if (ms->srcAction() & (taiClipData::EA_SRC_CUT))
   forbidden |= taiClipData::EA_FORB_ON_SRC_CUT;

  if (isGuiReadOnly()) {
    forbidden |= (taiClipData::EA_PASTE_INTO | taiClipData::EA_DROP_ASSIGN |
      taiClipData::EA_DROP_COPY_INTO | taiClipData::EA_DROP_MOVE_INTO);
  }  
  // can't link etc. if not in this process
  if (!ms->isThisProcess())
    forbidden |= taiClipData::EA_IN_PROC_OPS;
}

int taBase::ChildEditAction(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea)
{ 
  // determine the list-only operations allowed/forbidden, and apply to ea
  int rval = taiClipData::ER_IGNORED;
  
  // SRC
  if (ea & taiClipData::EA_SRC_OPS) { // dispatch on full ea, but only pass eax
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
  // these totaly mess with the gui updating for copy/paste/etc -- and with new
  // fixes, they are also unnecessary!
//   StructUpdate(true);
  for (int i = 0; i < ms->count(); ++i) {
    ms->setIndex(i);
    rval = ChildEditAction_impl(md, child, ms, ea);
    // keep looping as long as stuff ok
    if (rval != taiClipData::ER_OK)
      break;
  }
//   StructUpdate(false);
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
  if (ea & taiClipData::EA_DUPE) {
    if (ChildDuplicate(child) != NULL)
      return taiClipData::ER_OK;
    else return taiClipData::ER_ERROR;
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
      allowed |= taiClipData::EA_CUT;
      // Delete only allowed if we are the owner
      allowed |= taiClipData::EA_DELETE;
      // in general, allow duplicate
      //TODO: any forbidden cases? ex. read only?
//not yet implemented      allowed |= taiClipData::EA_DUPE;
    } else { // otherwise, it is unlinking, not deleting
      allowed |= taiClipData::EA_UNLINK;
    }
  }

  // DST ops

  if (ms == NULL) return; // SRC only query
  // if not a taBase type of object, no more applicable
  if (!ms->isBase()) return;
  if (ms->isThisProcess()) {
    // cannot allow an object to be copied or moved into one of its own subobjects
    taBase* obj = ms->tabObject();
    if (this->IsChildOf(obj)) {
      // forbid both kinds of cases
      forbidden |= (taiClipData::EA_DROP_COPY2 | taiClipData::EA_DROP_MOVE2 |
        taiClipData::EA_PASTE2); //note: paste, regardless whether src cut or copy
    }
    // confusing to allow move of an obj on itself
    if (lst_itm && (lst_itm == obj)) {
      forbidden |= (taiClipData::EA_DROP_COPY | taiClipData::EA_DROP_MOVE);
      if ((ms->srcAction() & taiClipData::EA_SRC_CUT))
        forbidden |= taiClipData::EA_PASTE;
    }
  } else {
    forbidden |= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for L action calls
  }
  
  // generic list paste allows any subtype of the base type
  bool right_type = ((list->el_base) && (ms->td() != NULL) && ms->td()->InheritsFrom(list->el_base));

  // figure out what would be allowed if right type
  int op = 0;
  //note: we no longer allow linking generally, only into LINK_GROUPs
  if (md && md->HasOption("LINK_GROUP")) {
    if (lst_itm) {
      op |= (taiClipData::EA_LINK | taiClipData::EA_DROP_LINK);
    } else {
      op |= (taiClipData::EA_LINK_INTO | taiClipData::EA_DROP_LINK_INTO);
    }
  } else {
    if (lst_itm) {
      op |= (taiClipData::EA_PASTE | taiClipData::EA_DROP_COPY |
        taiClipData::EA_DROP_MOVE);
    } else {
      op |= (taiClipData::EA_PASTE_INTO | taiClipData::EA_DROP_COPY_INTO |
        taiClipData::EA_DROP_MOVE_INTO);
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
  int rval = taiClipData::ER_IGNORED;
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
        if (ea & taiClipData::EA_SRC_OPS) {
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
  if (!list) return taiClipData::ER_IGNORED;
  if (lst_itm == NULL) return taiClipData::ER_IGNORED;
  
  switch (ea & taiClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself, or outer controller if multi
  case taiClipData::EA_UNLINK: {
    taProject* proj = (taProject*)list->GetOwner(&TA_taProject);
    if(proj) {
      proj->undo_mgr.SaveUndo(lst_itm, "Remove", NULL, false, list);
      // list is save_undo_owner
    }
    list->RemoveEl(lst_itm);
    return taiClipData::ER_OK;
  }
  // for COPY, CUT, DELETE, and DRAG, nothing more for us to do; just ack
  // (either handled by outer controller, or dest)
  case taiClipData::EA_COPY:
  case taiClipData::EA_CUT:
  case taiClipData::EA_DELETE: //note: handled by controller
  case taiClipData::EA_DRAG:
    return taiClipData::ER_OK;
  default: break; // compiler food
  }
  return taiClipData::ER_IGNORED;
}

int taOBase::ChildEditActionLD_impl_inproc(const MemberDef* md,
  taBase* lst_itm, taiMimeSource* ms, int ea)
{
  taList_impl* list = children_();
  if (!list) return taiClipData::ER_IGNORED;
  int itm_idx = list->FindEl(lst_itm); // -1 if NULL ie at end
  
  // itm_idx, -1 means parent itself
  int obj_idx = -1; // -1 means not in this list
  taBase* obj = NULL;

//NOTE: OP/OP_INTO are implicitly encoded via existence of lst_itm so 
//  we always lump the two variants together (OP2)
  // only fetch obj for ops that require it
  if (ea & (taiClipData::EA_PASTE2 | taiClipData::EA_LINK2  | taiClipData::EA_DROP_COPY2 |
    taiClipData::EA_DROP_LINK2 | taiClipData::EA_DROP_MOVE2))
  {
    obj = ms->tabObject();
    if(TestError(!obj, "ChildEditActionLD_impl_inproc",
		 "Could not retrieve object for operation")) return taiClipData::ER_ERROR;
    // already in this list? (affects how we do drops/copies, etc.)
    obj_idx = list->FindEl(obj);
  }
  
    
  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (taiClipData::EA_DROP_COPY2)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE2) && (ms->srcAction() & taiClipData::EA_SRC_COPY))
  ) {
    taProject* proj = dynamic_cast<taProject*>(list->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(list, "Paste/Copy", NULL, false, list);
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

    // special new delayed code to expand and select the new guy!
    if(!list->HasOption("NO_EXPAND_ALL") && !new_obj->HasOption("NO_EXPAND_ALL")) {
      tabMisc::DelayedFunCall_gui(new_obj, "BrowserExpandAll");
      tabMisc::DelayedFunCall_gui(new_obj, "BrowserSelectMe");
    }

    return taiClipData::ER_OK;
  }
  
  // All Move-like ops
  if (
    (ea & (taiClipData::EA_DROP_MOVE2)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE2) && (ms->srcAction() & taiClipData::EA_SRC_CUT))
  ) {
    if (obj == lst_itm) return 1; // nop
    taProject* proj = dynamic_cast<taProject*>(list->GetThisOrOwner(&TA_taProject));
    if(proj) {
      proj->undo_mgr.SaveUndo(obj, "Move", NULL, false, list);
      // list is save_undo_owner
    }
    if (obj_idx >= 0) { // in this list: just do a list move
      list->MoveBeforeIdx(obj_idx, itm_idx); // noop for self ops
      return taiClipData::ER_OK; // do nothing case of drop on self
    } else { // not in this list, need to do a transfer
      if (list->Transfer(obj)) { // should always succeed -- only fails if we already own item
        // was added at end, fix up location, if necessary
        if (itm_idx >= 0) { // if <0, then means "at end" already
          // for fm>to, to will just be the dst, because fm pushes to down
          list->MoveIdx(list->size - 1, itm_idx);
        }
	// special new delayed code to expand and select the new guy!
	if(!list->HasOption("NO_EXPAND_ALL") && !obj->HasOption("NO_EXPAND_ALL")) {
	  tabMisc::DelayedFunCall_gui(obj, "BrowserSelectMe"); // select new guy in case new owner was not expaned yet and needs expansion!
	}
      } else return taiClipData::ER_ERROR;
    }
    return taiClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (taiClipData::EA_LINK2 | taiClipData::EA_DROP_LINK2))
  {
    if (obj_idx >= 0) return -1; // in this list: link forbidden
    if (itm_idx < 0) 
      itm_idx = list->size; 
    list->InsertLink(obj, itm_idx);
    return taiClipData::ER_OK;
  }
  return taiClipData::ER_IGNORED;
}

int taOBase::ChildEditActionLD_impl_ext(const MemberDef* md,
  taBase* lst_itm, taiMimeSource* ms, int ea) 
{
  taList_impl* list = children_();
  if (!list) return taiClipData::ER_IGNORED;
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return taiClipData::ER_IGNORED;
//Note: the OP/OP_INTO is actually encoded in lst_itm NULL or not, so we lump together
  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  if (ea & (taiClipData::EA_DROP_COPY2 |
    // taiClipData::EA_DROP_MOVE | //can't do move across processes
     taiClipData::EA_PASTE2))
  {
    istringstream istr;
    if (ms->objectData(istr) > 0) {
      taProject* proj = dynamic_cast<taProject*>(list->GetThisOrOwner(&TA_taProject));
      if(proj) {
	proj->undo_mgr.SaveUndo(list, "Paste/Copy", NULL, false, list);
	// list is save_undo_owner
      }
      TypeDef* td = list->GetTypeDef();
      void* new_el_ = NULL; // the dude added
      int dump_val = td->Dump_Load(istr, list, list, &new_el_);
      if (dump_val == 0) {
        //TODO: error output
        return taiClipData::ER_ERROR; // load failed
      }
      // ok, now move the guy into the right place
      taBase* new_el = (taBase*)new_el_;
      list->MoveBefore(lst_itm, new_el);
      // call UAE to trigger associate update code, esp in parent lists etc.
      new_el->UpdateAfterEdit();
      return taiClipData::ER_OK;
    } else { // no data
        //TODO: error output
      return taiClipData::ER_ERROR;
    }
  }
  return taiClipData::ER_IGNORED;
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
      allowed |= taiClipData::EA_CUT;
      allowed |= taiClipData::EA_DELETE;
    } else { // otherwise, it is unlinking, not deleting
      allowed |= taiClipData::EA_UNLINK;
    }
  }

  if (ms == NULL) return; // src op query
  
  // DST ops
  // if not a taBase type of object, no more applicable
  if (!ms->isBase()) return;
  if (!ms->isThisProcess())
    forbidden |= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls
  
  if (ms->isThisProcess()) {
    // cannot allow a group to be copied or moved into one of its own subgroups
    taGroup_impl* grp = static_cast<taGroup_impl*>(ms->tabObject());
    // the grp must not be this one, or parent to it
    if (this->IsChildOf(grp)) {
      // forbid both kinds of cases
      forbidden |= (taiClipData::EA_DROP_COPY2 | taiClipData::EA_DROP_MOVE2 |
        taiClipData::EA_PASTE2); //note: paste, regardless whether src cut or copy
    }
    // confusing to allow move of an obj on itself
    if (subgrp && (subgrp == grp)) {
      forbidden |= (taiClipData::EA_DROP_COPY | taiClipData::EA_DROP_MOVE);
      if ((ms->srcAction() & taiClipData::EA_SRC_CUT))
        forbidden |= taiClipData::EA_PASTE;
    }
  } else {
    forbidden |= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for G action calls
  }
//NOTE: relaxed, 10/14/08 by BA
  // generic group paste only allows exact type, so we check for each inheriting from the other, which means same
  TypeDef* td = ms->td();
//old  bool right_gptype = (td && (td->InheritsFrom(GetTypeDef()) && GetTypeDef()->InheritsFrom(td)));
  bool right_gptype = (td && (td->InheritsFrom(GetTypeDef())) );

  // find relevant ops 
  int op = 0;
  if (subgrp) {
    op |= (taiClipData::EA_PASTE | taiClipData::EA_DROP_COPY |
      taiClipData::EA_DROP_MOVE);
  } else {
    op |= (taiClipData::EA_PASTE_INTO | taiClipData::EA_DROP_COPY_INTO |
      taiClipData::EA_DROP_MOVE_INTO);
  }
  if (right_gptype) {
    allowed |= op;
  }
}

// called by a child -- lists etc. can then allow drops on the child, to indicate inserting into the list, etc.
int taGroup_impl::ChildEditAction_impl(const MemberDef* md, taBase* child,
  taiMimeSource* ms, int ea) 
{
  int rval = taiClipData::ER_IGNORED;
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
      if (ea & taiClipData::EA_SRC_OPS) { // note: must use original ea to decode SRC
        rval = ChildEditActionGS_impl(md, (taGroup_impl*)child, eax);
      } else {
        if (ms == NULL) return taiClipData::ER_IGNORED; // shouldn't happen
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
  switch (ea & taiClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself
  case taiClipData::EA_CUT: return 1; //nothing to do, just acknowledge -- deletion triggered by the dst, whether local or remote
  case taiClipData::EA_DELETE: {
    if (subgrp) {
      RemoveGpIdx(subgrp_idx);
      return taiClipData::ER_OK;
    } else return taiClipData::ER_ERROR; // error TODO: error message
  }
  case taiClipData::EA_DRAG: return taiClipData::ER_OK; // nothing for us to do on the drag
  default: break; // compiler food
  }
  return taiClipData::ER_IGNORED; // this function never calls down to List
}

int taGroup_impl::ChildEditActionGD_impl_inproc(const MemberDef* md, taGroup_impl* subgrp,
  taiMimeSource* ms, int ea)
{
  int subgrp_idx = gp.FindEl(subgrp);
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return taiClipData::ER_IGNORED;
  int srcgrp_idx = -1; // -1 means not in this group
  taBase* srcobj = NULL;

//NOTE: OP/OP_INTO are implicitly encoded via existence of lst_itm so 
//  we always lump the two variants together (OP2)
  // only fetch obj for ops that require it
  if (ea & (taiClipData::EA_PASTE2 | taiClipData::EA_LINK2  | taiClipData::EA_DROP_COPY2 |
    taiClipData::EA_DROP_LINK2 | taiClipData::EA_DROP_MOVE2))
  {
    srcobj = ms->tabObject();
    if (srcobj == NULL) {
      taMisc::Error("Could not retrieve object for operation.");
      return taiClipData::ER_ERROR;
    }
    // already in this list? (affects how we do drops/copies, etc.)
    srcgrp_idx = gp.FindEl(srcobj);
  }

  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (taiClipData::EA_DROP_COPY2)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE2) && (ms->srcAction() & taiClipData::EA_SRC_COPY))
  ) {
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
    return taiClipData::ER_OK;
  } 
  
  // All Move-like ops
  if (
    (ea & (taiClipData::EA_DROP_MOVE2)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE2) && (ms->srcAction() & taiClipData::EA_SRC_CUT))
  ) {
    if (srcobj == subgrp) return taiClipData::ER_OK; // nop
    if (srcgrp_idx >= 0) { // in this group: just do a group move
      gp.MoveBeforeIdx(srcgrp_idx, subgrp_idx); // ok if -1, means end
      return taiClipData::ER_OK; // do nothing case of drop on self
    } else { // not directly in this group, need to do a transfer
      if (gp.Transfer(srcobj)) { // should always succeed -- only fails if we already own item
      // was added at end, fix up location, if necessary
        if (subgrp_idx >= 0) { // if <0, then means "at end" already
          gp.MoveIdx(gp.size - 1, subgrp_idx);
        }
      } else return taiClipData::ER_ERROR; //TODO: error message
    }
    return taiClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (taiClipData::EA_LINK2 | taiClipData::EA_DROP_LINK2))
  {
    if (srcgrp_idx >= 0) return taiClipData::ER_FORBIDDEN; // in this list: link forbidden
    if (subgrp_idx < 0) 
      subgrp_idx = gp.size; 
    gp.InsertLink(srcobj, subgrp_idx);
    return taiClipData::ER_OK;
  }
  return taiClipData::ER_IGNORED;
}

int taGroup_impl::ChildEditActionGD_impl_ext(const MemberDef* md, taGroup_impl* subgrp, taiMimeSource* ms, int ea)
{
  // if src is not even a taBase, we just stop
  if (!ms->isBase()) return taiClipData::ER_IGNORED;

// Note on streaming:
// For Paste Here, we have to stream into ourself
//Note: the OP/OP_INTO is actually encoded in lst_itm NULL or not, so we lump together
  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  if (ea & (taiClipData::EA_DROP_COPY2 |
    // taiClipData::EA_DROP_MOVE | //can't do move across processes
     taiClipData::EA_PASTE2))
  {
    istringstream istr;
    if (ms->objectData(istr) > 0) {
      TypeDef* td = GetTypeDef();
      // we need to add the new group BEFORE streaming!
      taGroup_impl* new_gp = NewGp_(1, td);
      bool ok; // dummy
      if (CheckError((!new_gp), false, ok,
        "Could not make new group for TypeDef:", td->name)) 
        return taiClipData::ER_ERROR; // load failed
      // move it now, before the load
      gp.MoveBefore(subgrp, new_gp);
      taBase* ld_par = this;
      int dump_val = td->Dump_Load(istr, new_gp, ld_par/*, &new_el_*/);
      
      if (CheckError((dump_val == 0), false, ok,
        "Load error for TypeDef:" + td->name)) 
        return taiClipData::ER_ERROR; // load failed
      return taiClipData::ER_OK;
    } else { // no data
      return taiClipData::ER_ERROR; //TODO: error message
    }
  }
  return taiClipData::ER_IGNORED;
}


void taDataView::DV_QueryEditActionsS(int& allowed, int& forbidden)
{
  if (!isTopLevelView()) {
    forbidden |= (taiClipData::EA_CUT | 
      taiClipData::EA_DUPE | taiClipData::EA_DELETE | 
      taiClipData::EA_CLEAR | taiClipData::EA_UNLINK);
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
    forbidden |= (taiClipData::EA_PASTE_INTO | taiClipData::EA_PASTE_APPEND |
      taiClipData::EA_LINK_INTO | taiClipData::EA_DROP_COPY_INTO |
      taiClipData::EA_DROP_LINK_INTO | taiClipData::EA_DROP_MOVE_INTO);
    if (lst_itm) // forbid on top of guys too
      forbidden |= (taiClipData::EA_PASTE | taiClipData::EA_PASTE_APPEND |
        taiClipData::EA_LINK | taiClipData::EA_DROP_COPY |
        taiClipData::EA_DROP_LINK | taiClipData::EA_DROP_MOVE);
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



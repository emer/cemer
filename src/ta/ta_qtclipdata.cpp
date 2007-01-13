// Copyright, 1995-2005, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of TA/CSS
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
/*
QByteArray taiMimeItem::data(const QString& mimeType) const {
  if (m_ms) return m_ms->data(mimeType);
  else return QByteArray();
}

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
  m_td = taMisc::types.FindName(m_type_name.chars()); // note: could be NULL if, ex. decoding leabra object in instance of bp
  return (m_td);
}

void taiObjectMimeItem::DecodeData_impl() {
  if (isThisProcess() && (!m_obj) && tabMisc::root) {
    m_obj = tabMisc::root->FindFromPath(path());
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

TypeDef* taiObjectsMimeItem::CommonSubtype() const {
  if (count() == 0) return NULL;
  TypeDef* rval = item(0)->td();
  for (int i = 1; (rval && (i < count())); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, 
      ((taiObjectMimeItem*)item(i))->td());
  }
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
  ms = ms_;
  m_src_action = 0; 
  m_this_proc = false;
  if (ms)
    connect(ms, SIGNAL(destroyed()), this, SLOT(ms_destroyed()) );
//TEMP
  mi = NULL;
  iter_idx = 0;
}

taiMimeSource::~taiMimeSource() {
  ms = NULL;
}

QByteArray taiMimeSource::data(const QString& mimeType) const {
  if (ms) return ms->data(mimeType);
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
  if (ms) return ms->formats();
  else return QStringList();
}

bool taiMimeSource::hasFormat(const QString &mimeType) const {
  if (ms) return ms->hasFormat(mimeType);
  else return false;
}

taiMimeItem* taiMimeSource::GetMimeItem(TypeDef* td, const String& subkey) {
  if (!ms) return NULL;
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
  ms = NULL;
#ifdef DEBUG
  taMisc::Warning("taiMimeSource::ms destroyed before clip operations completed!");
#endif
}

taiObjectsMimeItem* taiMimeSource::objects() const {
  if (!mi)
    mi = (taiObjectsMimeItem*)const_cast<taiMimeSource*>(this)->GetMimeItem(&TA_taiObjectsMimeItem);
  return mi;
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


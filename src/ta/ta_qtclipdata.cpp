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

void taiMimeItem::Initialize() {
  m_index = -1;
  m_flags = 0;
  m_ms = NULL;
}

void taiMimeItem::AssertData() {
  if (!isDecoded()) {
    DecodeData_impl();
    m_flags |= MIF_DECODED;
  }
}

void taiMimeItem::Constr(taiMimeSource* ms_, const String& mimetype) {
  m_ms = ms_;
  Constr_impl(mimetype); // NOTE: mt is often nil (default)
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

QMimeData* taiMimeItem::mimeData() const {
/*TODO  if (m_ms) return m_ms->mimeData();
  else */return NULL;
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
//  taiMatDataMimeItem 		//
//////////////////////////////////
/*
taiMatDataMimeItem::taiMatDataMimeItem(int data_type_)
{
  m_data_type = data_type_;
}

bool taiMatDataMimeItem::isMatrix() const {
  return (m_data_type == taiMimeSource::ST_MATRIX_DATA);
}
  
bool taiMatDataMimeItem::isTable() const {
  return (m_data_type == taiMimeSource::ST_TABLE_DATA);
}
  
void taiMatDataMimeItem::GetFormats_impl(QStringList& list, int) const {
  if (isMatrix())
    list.append(taiClipData::tacss_matrixdesc);
  else if (isTable())
    list.append(taiClipData::tacss_tabledesc);
}



//////////////////////////////////
//  taiRcvMatDataMimeItem 	//
//////////////////////////////////

taiRcvMatDataMimeItem::taiRcvMatDataMimeItem(int data_type_)
:inherited(data_type_)
{
  m_cols = 0;
  m_rows = 0;
  m_max_row = 1;
  m_geoms.SetBaseType(&TA_MatrixGeom);
}

void taiRcvMatDataMimeItem::DecodeMatrixDesc(String& arg) {
  // just do it all blind, because supposed to be in correct format
  String tmp = arg.before(';');
  m_cols = tmp.toInt();
  arg = arg.after(';');
  tmp = arg.before(';');
  m_rows = tmp.toInt();
  arg = arg.after(';'); 
}

void taiRcvMatDataMimeItem::DecodeTableDesc(String& arg) {
  DecodeMatrixDesc(arg);
  String tmp;
  for (int i = 0; i < m_cols; ++i) {
    tmp = arg.before(';');
    int col_cols = tmp.toInt();
    arg = arg.after(';');
    tmp = arg.before(';');
    int col_rows = tmp.toInt();
    m_max_row = MAX(m_max_row, col_rows);
    arg = arg.after(';'); 
    MatrixGeom* geom = new MatrixGeom(2, col_cols, col_rows);
    m_geoms.Add(geom);
  }
}

void taiRcvMatDataMimeItem::GetColGeom(int col, int& cols, int& rows) const {
  if (m_data_type == taiMimeSource::ST_MATRIX_DATA) {
    cols = 1;  rows = 1;
  } else {
    MatrixGeom* geom = (MatrixGeom*)m_geoms.SafeEl(col);
    if (geom) {
      cols = geom->SafeEl(0);
      rows = geom->SafeEl(1);
    } else { // bad call!
      cols = 0;  rows = 0;
    }
  }
}

*/
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

void taiObjectMimeItem::Constr_impl(const String& mimetype) {
  m_td = taMisc::types.FindName(m_type_name.chars()); // note: could be NULL if, ex. decoding leabra object in instance of bp
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
// 	taiMimeSource	//
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
  m_src_type = ST_UNDECODED;
  iter_idx = 0;
}

taiMimeSource::~taiMimeSource() {
  ms = NULL;
}

TypeDef* taiMimeSource::CommonSubtype() const {
  int size = count();
  if (!isObject() || (size == 0)) return NULL;
  TypeDef* rval = ((taiObjectMimeItem*)item(0))->td();
  for (int i = 1; (rval && (i < size)); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, 
      ((taiObjectMimeItem*)item(i))->td());
  }
  return rval;
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

int taiMimeSource::index() const {
  return iter_idx;
}

void taiMimeSource::ms_destroyed() {
  ms = NULL;
#ifdef DEBUG
  taMisc::Warning("taiMimeSource::ms destroyed before clip operations completed!");
#endif
}

taBase* taiMimeSource::tabObject() const {
  if (!(isObject() && isThisProcess() && inRange())) return NULL;
  
  return ((taiObjectMimeItem*)item())->obj(); // looks up from path
}

void taiMimeSource::Decode() {
  if (m_src_type != ST_UNDECODED) return;
  Decode_impl();
  // if nobody groked it, then it is something alien
  if (m_src_type == ST_UNDECODED) 
    m_src_type = ST_UNKNOWN;
}

void taiMimeSource::Decode_impl() {
  // inherited guys could call us first, and if still undecoded, try their own
  String str;
  if (data(taiClipData::tacss_common, str) > 0) {
    Decode_common(str);
  }
//TEMP
  if (data(taiObjectMimeFactory::tacss_objectdesc, str) > 0) {
    DecodeDesc_object(str);
/*TODO  } else if (data(taiClipData::tacss_matrixdesc, str) > 0) {
    DecodeDesc_matrix(str);
  } else if (data(taiClipData::tacss_tabledesc, str) > 0) {
    DecodeDesc_table(str);
  } else {
    if (TryDecode_matrix()) goto decoded;
    //NOTE: other guys could go here, just like the above */
  }
decoded:
  ;
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

bool taiMimeSource::DecodeDesc_object(String arg) {
  bool ok;
  String str = arg.before(';');
  m_itm_cnt = str.toInt(&ok);
  if (!ok) return false;
  arg = arg.after(";\n");
  
  mi = (taiObjectsMimeItem*)list.New(1, &TA_taiObjectsMimeItem);

  for (int i = 0; i < m_itm_cnt; ++i) {
    // decode type and get the typedef obj
    String typeName = arg.before(';');
    if (typeName.length() == 0) goto fail;
    arg = arg.after(';');

    // decode path -- could be empty for non-taBase object
    String path = arg.before(';');
    arg = arg.after(';');
    //TODO: skip extension data
    taiObjectMimeItem* msd = (taiObjectMimeItem*) mi->items.New(1, &TA_taiObjectMimeItem);
    msd->m_type_name = typeName;
    msd->m_path = path;
    msd->Constr(this);
    arg = arg.after('\n');
  }
  setIndex(0);
  m_src_type = ST_OBJECT;
  return true;

fail:
  list.Reset();
  return false;
}

bool taiMimeSource::DecodeDesc_matrix(String arg) {
  return false;
/*  
  taiRcvMatDataMimeItem* msd = new taiRcvMatDataMimeItem(ST_MATRIX_DATA);
  msd->DecodeMatrixDesc(arg);
  list.Add(msd);
  setIndex(0);
  m_src_type = ST_MATRIX_DATA;
  return true;

fail:
  list.Reset();
  return false; */
}

bool taiMimeSource::DecodeDesc_table(String arg) {
  return false;
/*  
  taiRcvMatDataMimeItem* msd = new taiRcvMatDataMimeItem(ST_TABLE_DATA);
  msd->DecodeTableDesc(arg);
  list.Add(msd);
  setIndex(0);
  m_src_type = ST_TABLE_DATA;
  return true;

fail:
  list.Reset();
  return false; */
}

int taiMimeSource::objectData(istringstream& result) {
  if (isObject() && inRange()) {
    QString fmt = taiObjectMimeFactory::tacss_objectdata + ";index=" + String(index());
    return data(fmt, result);
  } else {
    return 0;
  }
}


bool taiMimeSource::TryDecode_matrix() {
  // TODO: see if we can recognize data as a tabular format
  return false;
}

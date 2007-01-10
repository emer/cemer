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

const String taiClipData::text_plain_iso8859_1("text/plain;charset=ISO-8859-1"); 
const String taiClipData::text_plain_utf8("text/plain;charset=UTF-8");
const String taiClipData::text_plain("text/plain");
const String taiClipData::tacss_objectdesc("tacss/objectdesc");
const String taiClipData::tacss_matrixdesc("tacss/matrixdesc");
const String taiClipData::tacss_tabledesc("tacss/tabledesc"); 
const String taiClipData::tacss_objectdata("tacss/objectdata"); 

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
}

QByteArray taiClipData::encodedData_impl(const String& , int ) {
  return QByteArray();
}

QStringList taiClipData::formats() const {
  // NOTE: we only enumerate the non-hidden types, even though we accept them
  QStringList rval;
  GetFormats_impl(rval);
  return rval;
}

bool taiClipData::isObject() const {
  taiMimeItem* item = items(0);
  return (item) ? item->isObject() : false;
}

void taiClipData::GetFormats_impl(QStringList& list) const {
  list.append(text_plain);
  for (int i = 0; i < count(); ++i) {
    taiMimeItem* item = items(i);
    item->GetFormats(list, i);
  }
}

bool taiClipData::DecodeFormat(const String& mimeType, String& fmt, int& index) const {
  // variants on text/plain -- we accept these, because Qt (or Kde) supplies these externally, regardless
  index = 0; // default, unless we encounter a specific index
  if ((mimeType == text_plain_utf8) ||
    (mimeType == text_plain_iso8859_1))
  {
    fmt = text_plain;
    return true;
  }

  // for safety, we don't pretend to boggle other text/plain types, since the charset may be inappropriate
  if (mimeType.matches("text/plain;"))
      return false;

  // strip indexed guys
  fmt = mimeType.before(";index=");
  if (fmt.nonempty()) {
    index = mimeType.after(";index=").toInt();
    return true;
  }
  
  fmt = mimeType;
  return true;
}

QVariant taiClipData::retrieveData(const QString & mimeType, QVariant::Type type) const {
  // we only grok ByteArray (in this version)
  if (type != QVariant::ByteArray)
    return inherited::retrieveData(mimeType, type);

  String fmt;
  int index = 0;
//bool ok =
  DecodeFormat(mimeType, fmt, index);
  // hack: we want text/plain for objects to actually be objectdesc
  if (isObject() && (fmt == text_plain))
    fmt = tacss_objectdesc;
  return (const_cast<taiClipData*>(this))->encodedData_impl(fmt, index); 
}


//////////////////////////////////
// 	taiSingleClipData	//
//////////////////////////////////

taiSingleClipData::taiSingleClipData(taiMimeItem* item_, int src_edit_action_)
:taiClipData(src_edit_action_)
{
  item = item_;
}

taiSingleClipData::~taiSingleClipData() {
  if (item) {
    delete item;
    item = NULL;
  }
}

QByteArray taiSingleClipData::encodedData_impl(const String& fmt, int index) {
  //note: index should be 0 for singles
  if (item && (index == 0)) {
    // the objectdesc guys are special case
    if (fmt == tacss_objectdesc) {
      taiObjDataMimeItem* item = (taiObjDataMimeItem*)this->item; // ugh
      String rval = "1;" +
        String(src_edit_action) + ';' +
        String(taPlatform::processId()) + ";\n" +
        item->typeName() + ';' +
        item->path() + ';'; // we put ; at end to facilitate parsing, and for extensibility
      return QByteArray(rval.chars());
    }
    // all others we delegate to the item
    {
      ostringstream ost;
      item->SetData(ost, fmt);
      return QByteArray(ost.str().c_str());
    }
  }
  return inherited::encodedData_impl(fmt, index);
}


//////////////////////////////////
//  tabMultiClipData		//
//////////////////////////////////

taiMultiClipData::taiMultiClipData(taiMimeItem_List* list_, int src_edit_action_)
:inherited(src_edit_action_)
{
  list = list_;
}

taiMultiClipData::~taiMultiClipData() {
  if (list) {
    delete list;
    list = NULL;
  }
}

int taiMultiClipData::count() const {
  return list->size;
}

taiMimeItem* taiMultiClipData::items(int i) const {
  return list->SafeEl(i);
}

QByteArray taiMultiClipData::encodedData_impl(const String& fmt, int index) {
  if (list) {
  //  case IDX_MD_TEXT_PLAIN: use base
    // objdesc is a special case
    if (fmt == tacss_objectdesc) {
      String rval =
        String(list->size) + ";" +
        String(src_edit_action) + ';' +
        String(taPlatform::processId()) + ";";
      for (int i = 0; i < list->size; ++i) {
        taiObjDataMimeItem* mi = (taiObjDataMimeItem*)list->FastEl(i);
        rval = rval + '\n' + mi->typeName() + ';' +
          mi->path() + ';'; // we put ; at end to facilitate parsing, and for extensibility
      }
      return QByteArray(rval.chars());
    }
    // all others, we delegate to item of index
    {
      taiMimeItem* mi  = list->SafeEl(index);
      if (mi == NULL) return QByteArray();
      ostringstream ost;
      mi->SetData(ost, fmt);
      return QByteArray(ost.str().c_str());
    }
  }
  return inherited::encodedData_impl(fmt, index);
}


//////////////////////////////////
// 	taiMimeItem 		//
//////////////////////////////////

taiMimeItem* taiMimeItem::New(taBase* obj) {
  tabSndMimeItem* rval = new tabSndMimeItem(obj);
  return rval;
}

taiMimeItem::taiMimeItem(): QObject()   {
}


//////////////////////////////////
// 	taiMimeItem_List	//
//////////////////////////////////

void taiMimeItem_List::El_Done_(void* it) {
  if (it) delete ((taiMimeItem*)it);
}


//////////////////////////////////
//  taiMatDataMimeItem 		//
//////////////////////////////////

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


//////////////////////////////////
//  taiObjDataMimeItem 		//
//////////////////////////////////

taiObjDataMimeItem::taiObjDataMimeItem()
:inherited()
{
  m_obj = NULL;
}

void taiObjDataMimeItem::GetFormats_impl(QStringList& list, int idx) const {
  if (idx == 0) {
    list.append(taiClipData::tacss_objectdesc);
//    list.append(taiClipData::tacss_objectdata);
  }
  list.append(taiClipData::tacss_objectdata + ";index=" + String(idx));
}


//////////////////////////////////
// 	tabSndMimeItem 		//
//////////////////////////////////

tabSndMimeItem::tabSndMimeItem(taBase* obj_)
:inherited()
{
  m_obj = obj_;
}

String tabSndMimeItem::typeName() const {
  if (m_obj) {
    TypeDef* td_ = m_obj->GetTypeDef();
    if (td_) return td_->name;
  }
  return "";
}

String tabSndMimeItem::path() const {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  return (m_obj) ? m_obj->GetPath() : "";
}

void tabSndMimeItem::SetData(ostream& ost, const String& fmt) {
  if (m_obj) {
    m_obj->Save_strm(ost);
  }
}

TypeDef* tabSndMimeItem::td() const {
  return (m_obj) ? m_obj->GetTypeDef() : NULL;
}


//////////////////////////////////
// 	taiRcvExtMimeItem 	//
//////////////////////////////////

taiRcvMimeItem::taiRcvMimeItem(const String type_name_, const String path_)
:inherited()
{
  mtype_name = type_name_;
  mpath = path_;
  mtd = taMisc::types.FindName(type_name_.chars()); // note: could be NULL if, ex. decoding leabra object in instance of bp
  m_is_base = (mtd && (mtd->InheritsFrom(&TA_taBase)));
}

void* taiRcvMimeItem::obj() const {  // note: only called when we are InProcess
  if (m_obj == NULL) {
    if (tabMisc::root) {
      String str = String(path()); //unconstifying copy
      ((taiRcvMimeItem*)this)->m_obj = tabMisc::root->FindFromPath(str); // only a decaching op, so we cast away constness
    }
  }
  return m_obj;
}



//////////////////////////////////
// 	taiMimeSource	//
//////////////////////////////////

taiMimeSource* taiMimeSource::New(const QMimeData* ms_) {
  //TODO: for multi, check for multi source, and create a taiMultiMimeSource obj instead
  taiExtMimeSource* rval =  new taiExtMimeSource(ms_);
  rval->Decode();
  return rval;
}

taiMimeSource* taiMimeSource::New2(taiClipData* cd) {
  taiIntMimeSource* rval = new taiIntMimeSource(cd);
  return rval;
}

taiMimeSource* taiMimeSource::NewFromClipboard() {
  taiExtMimeSource* rval =  new taiExtMimeSource(QApplication::clipboard()->mimeData());
  rval->Decode();
  return rval;
}

taiMimeSource::taiMimeSource(const QMimeData* ms_)
:inherited()
{
  ms = ms_;
  iter_idx = -1;
  m_src_type = ST_UNDECODED;
  if (ms)
    connect(ms, SIGNAL(destroyed()), this, SLOT(ms_destroyed()) );
}

taiMimeSource::~taiMimeSource() {
  ms = NULL;
}

TypeDef* taiMimeSource::CommonSubtype() const {
  int size = count();
  if (!isTacss() || (size == 0)) return NULL;
  TypeDef* rval = ((taiObjDataMimeItem*)item(0))->td();
  for (int i = 1; (rval && (i < size)); ++i) {
    rval = TypeDef::GetCommonSubtype(rval, 
      ((taiObjDataMimeItem*)item(i))->td());
  }
  return rval;
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
  else return inherited::formats(); //TODO: maybe should just return an empty list!
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
  taMisc::Warning("taiMimeSource::ms destroyed");
#endif
}


void* taiMimeSource::object() const {  // gets the object, if possible. if local, tries to get from path, otherwise tries to make
  //note: we only use m_obj for caching, so we aren't really violating const be assigning to it (via unconstifying the m_obj)
  if (!(isTacss() && IsThisProcess() && inRange())) return NULL;

  return ((taiObjDataMimeItem*)item())->obj(); // looks up from path
}

int taiMimeSource::objectData(istringstream& result) const {
  if (isTacss() && inRange()) {
    QString fmt = taiClipData::tacss_objectdata + ";index=" + String(index());
    return data(fmt, result);
  } else {
    return 0;
  }
}

taBase* taiMimeSource::tabObject() const {
  taBase* rval = NULL;
  TypeDef* td_ = td();
  if (td_ && td_->InheritsFrom(&TA_taBase))
    rval = (taBase*)object();
  return rval;
}

QVariant taiMimeSource::retrieveData(const QString & mimetype, QVariant::Type type) const {
  //note: we can't access the ms method directly, but getting the data does the same thing
  if (ms) return ms->data(mimetype);
  else return QVariant();
}


//////////////////////////////////
// 	taiIntMimeSource	//
//////////////////////////////////

taiIntMimeSource::taiIntMimeSource(taiClipData* cd_)
:inherited(cd_)
{
  cd = cd_;
  iter_idx = 0;
}

taiIntMimeSource::~taiIntMimeSource() {
  // nothing
}


//////////////////////////////////
// 	taiExtMimeSource	//
//////////////////////////////////

taiExtMimeSource::taiExtMimeSource(const QMimeData* ms_)
:inherited(ms_)
{
  msrc_action = 0; 
  process_id = 0;
}

taiExtMimeSource::~taiExtMimeSource() {
  // nothing
}

void taiExtMimeSource::Decode() {
  if (m_src_type != ST_UNDECODED) return;
  Decode_impl();
  // if nobody groked it, then it is something alien
  if (m_src_type == ST_UNDECODED) 
    m_src_type = ST_UNKNOWN;
}

void taiExtMimeSource::Decode_impl() {
  // inherited guys could call us first, and if still undecoded, try their own
  String str;
  if (data(taiClipData::tacss_objectdesc, str) > 0) {
    DecodeDesc_object(str);
  } else if (data(taiClipData::tacss_matrixdesc, str) > 0) {
    DecodeDesc_matrix(str);
  } else if (data(taiClipData::tacss_tabledesc, str) > 0) {
    DecodeDesc_table(str);
  } else {
    if (TryDecode_matrix()) goto decoded;
    //NOTE: other guys could go here, just like the above
  }
decoded:
  ;
}

bool taiExtMimeSource::DecodeDesc_object(String arg) {
  // s/b in form: obj_cnt=N;src edit action;procid
  // [\n objtype;ta_path]xN
  String str = arg.before(';');
  if (str.length() == 0) return false;
  int itm_cnt = (int)str;
  arg = arg.after(';');

  str = arg.before(';');
  if (str.length() == 0) return false;
  msrc_action = (int)str;
  arg = arg.after(';');

  str = arg.before(';');
  if (str.length() == 0) return false;
  process_id = (int)str;

  for (int i = 0; i < itm_cnt; ++i) {
    arg = arg.after('\n');
    // decode type and get the typedef obj
    String typeName = arg.before(';');
    if (typeName.length() == 0) goto fail;
    arg = arg.after(';');

    // decode path -- could be empty for non-taBase object
    String path = arg.before(';');
    arg = arg.after(';');
    //TODO: skip extension data
    taiObjDataMimeItem* msd = new taiRcvMimeItem(typeName, path);
    list.Add(msd);
  }
  setIndex(0);
  m_src_type = ST_OBJECT;
  return true;

fail:
  list.Reset();
  return false;
}

bool taiExtMimeSource::DecodeDesc_matrix(String arg) {
  taiRcvMatDataMimeItem* msd = new taiRcvMatDataMimeItem(ST_MATRIX_DATA);
  msd->DecodeMatrixDesc(arg);
  list.Add(msd);
  setIndex(0);
  m_src_type = ST_MATRIX_DATA;
  return true;
/*
fail:
  list.Reset();
  return false; */
}

bool taiExtMimeSource::DecodeDesc_table(String arg) {
  taiRcvMatDataMimeItem* msd = new taiRcvMatDataMimeItem(ST_TABLE_DATA);
  msd->DecodeTableDesc(arg);
  list.Add(msd);
  setIndex(0);
  m_src_type = ST_TABLE_DATA;
  return true;
/*
fail:
  list.Reset();
  return false; */
}

bool taiExtMimeSource::IsThisProcess() const {
  return (process_id == taPlatform::processId());
}

bool taiExtMimeSource::TryDecode_matrix() {
  // TODO: see if we can recognize data as a tabular format
  return false;
}

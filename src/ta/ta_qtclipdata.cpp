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
#include "ta_project.h"

#include <qapplication.h>
#include <qclipboard.h>
#include <QByteArray>

#include <sstream>

using namespace std;



//////////////////////////////////
// 	taiClipData		//
//////////////////////////////////

// mime-type strings -- for all, XXX is optional if it is 0 (ex for single data case)
//NOTE: strings that are prefix subsets of longer strings MUST come later in search order, so we
// don't incorrectly decode them as XXX index suffix strings
const QString taiClipData::text_plain_iso8859_1("text/plain;charset=ISO-8859-1"); 
const QString taiClipData::text_plain_utf8("text/plain;charset=UTF-8");
const QString taiClipData::text_plain("text/plain");
const QString taiClipData::tacss_objectdesc("tacss/objectdesc");
const QString taiClipData::tacss_objectdata("tacss/objectdata"); 
const QString taiClipData::tacss_remdatataken("tacss/remdatataken");
const QString taiClipData::tacss_locdatataken("tacss/locdatataken"); 

const QString taiClipData::mime_types[IDX_MD_MAX + 1] = {
   text_plain,
   tacss_objectdesc,
   tacss_objectdata,
   tacss_remdatataken,
   tacss_locdatataken
};

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

QByteArray taiClipData::encodedData_impl(int , int ) {
  return QByteArray();
}

QStringList taiClipData::formats() const {
  // NOTE: we only enumerate the non-hidden types, even though we accept them
  QStringList rval;
  formats_impl(rval);
  return rval;
}

void taiClipData::formats_impl(QStringList& list) const {
  for (int i = 0; i <= IDX_MD_VISIBLE_MAX; ++i) {
    list.append(mime_types[i]);
  }
}

bool taiClipData::DecodeFormat(const QString& mimeType, int& fmt_num, int& index) const {
  // variants on text/plain -- we accept these, because Qt (or Kde) supplies these externally, regardless
  if ((mimeType == text_plain_utf8) ||
    (mimeType == text_plain_iso8859_1))
  {
    fmt_num = IDX_MD_TEXT_PLAIN;
    index = 0;
    return true;
  }

  // for safety, we don't pretend to boggle other text/plain types, since the charset may be inappropriate
  if (mimeType.startsWith("text/plain;"))
      return false;

  for (int i = 0; i <= IDX_MD_MAX; ++i ) {
    QString mti = mime_types[i];
    if (!mimeType.startsWith(mti)) continue;
    // we have the fmt_num, now check if there is an index suffix
    fmt_num = i;
    int mt_len = mimeType.length();
    int mti_len = mti.length();
    if (mt_len == mti_len)
      index = 0;
    else
       // index starts with first non-matching char
      index = mimeType.right(mt_len - mti_len).toInt();
    return true;
  }
  return false;
}

QVariant taiClipData::retrieveData(const QString & mimeType, QVariant::Type type) const {
  // we only grok ByteArray (in this version)
  if (type != QVariant::ByteArray)
    return inherited::retrieveData(mimeType, type);

  int fmt_num = -1;
  int index = 0;
//bool ok =
  DecodeFormat(mimeType, fmt_num, index);
  if (fmt_num == IDX_MD_TEXT_PLAIN)
    fmt_num = IDX_MD_OBJECTDESC;
  return ((taiClipData*) this)->encodedData_impl(fmt_num, index); // we do our non-const cheat here
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

QByteArray taiSingleClipData::encodedData_impl(int fmt_num, int index) {
  //note: index should be 0 for singles
  if (item && (index == 0)) switch (fmt_num) {
/*  case IDX_MD_TEXT_PLAIN: { //note: not currently used because base traps this and translates to IDX_MD_OBJECTDESC
    ostringstream ost;
    item->Output(ost);
    return QCString(ost.str().c_str());
  } */
  case IDX_MD_OBJECTDESC: {
    String rval =
      String(src_edit_action) + ';' +
      String(taPlatform::processId()) + ";1;\n" +
      item->type_name() + ';' +
      item->path() + ';'; // we put ; at end to facilitate parsing, and for extensibility
    return QByteArray(rval.chars());
  }
  case IDX_MD_OBJECTDATA: {
    ostringstream ost;
    item->SetData(ost);
    return QByteArray(ost.str().c_str());
  }
  case IDX_MD_REMDATATAKEN: { // dst should only call on Cut/Paste or Drag/Move
    if (!(src_edit_action & (EA_SRC_CUT | EA_SRC_DRAG))) break;
    item->RemDataTaken();
//CAN'T DO THIS HERE--CAUSNG CRASH    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QByteArray("ok"); //helps debugging
  }
  case IDX_MD_LOCDATATAKEN: { //simpler than REM, because internals took care of moving data itself
    item->LocDataTaken();
//    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QByteArray("ok"); //helps debugging
  }
  }
  return taiClipData::encodedData_impl(fmt_num, index);
}


//////////////////////////////////
// 	tabMultiClipData		//
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


QByteArray taiMultiClipData::encodedData_impl(int fmt_num, int index) {
  if (list) switch (fmt_num) {
//  case IDX_MD_TEXT_PLAIN: use base
  case IDX_MD_OBJECTDESC: {
    String rval =
      String(src_edit_action) + ';' +
      String(taPlatform::processId()) + ";" +
      String(list->size) + ";";
    for (int i = 0; i < list->size; ++i) {
      taiMimeItem* mi = list->FastEl(i);
      rval = rval + '\n' + mi->type_name() + ';' +
        mi->path() + ';'; // we put ; at end to facilitate parsing, and for extensibility
    }
    return QByteArray(rval.chars());
  }
  case IDX_MD_OBJECTDATA: {
    taiMimeItem* mi  = list->SafeEl(index);
    if (mi == NULL) return QByteArray();
    ostringstream ost;
    mi->SetData(ost);
    return QByteArray(ost.str().c_str());
  }
  case IDX_MD_REMDATATAKEN: { // dst should only call on Cut/Paste or Drag/Move
    if (!(src_edit_action & (EA_SRC_CUT | EA_SRC_DRAG))) break;
    taiMimeItem* mi  = list->SafeEl(index);
    if (mi) mi->RemDataTaken();
  //CAN'T DO THIS HERE--CAUSNG CRASH    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
  //      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QByteArray("ok"); //helps debugging
  }
  case IDX_MD_LOCDATATAKEN: { //simpler than REM, because internals took care of moving data itself
    taiMimeItem* mi = list->SafeEl(index);
    if (mi) mi->LocDataTaken();
//    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QByteArray("ok"); //helps debugging
  }
  }
  return taiClipData::encodedData_impl(fmt_num, index);
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
// 	tabSndMimeItem 		//
//////////////////////////////////

tabSndMimeItem::tabSndMimeItem(taBase* obj_)
:inherited()
{
  mobj = obj_;
}

String tabSndMimeItem::type_name() const {
  if (mobj) {
    TypeDef* td_ = mobj->GetTypeDef();
    if (td_) return td_->name;
  }
  return "";
}

void tabSndMimeItem::LocDataTaken() {
    mobj = NULL;
}

String tabSndMimeItem::path() const {
  //note: can't use Path_Long because path parsing routines don't handle names in paths
  return (mobj) ? mobj->GetPath() : "";
}

void tabSndMimeItem::RemDataTaken() {
  if (mobj) {
    mobj->Close();
    mobj = NULL;
  }
}

void tabSndMimeItem::SetData(ostream& ost) {
  if (mobj) {
    mobj->Save(ost);
  }
}

TypeDef* tabSndMimeItem::td() const {
  return (mobj) ? mobj->GetTypeDef() : NULL;
}


//////////////////////////////////
// 	taiRcvExtMimeItem 	//
//////////////////////////////////

taiRcvMimeItem::taiRcvMimeItem(const String type_name_, const String path_)
:inherited()
{
  mobj = NULL;
  mtype_name = type_name_;
  mpath = path_;
  mtd = taMisc::types.FindName(type_name_.chars()); // note: could be NULL if, ex. decoding leabra object in instance of bp
  mis_tab = (mtd && (mtd->InheritsFrom(&TA_taBase)));
}

void* taiRcvMimeItem::obj() const {  // note: only called when we are InProcess
  if (mobj == NULL) {
    if (tabMisc::root) {
      String str = String(path()); //unconstifying copy
      ((taiRcvMimeItem*)this)->mobj = tabMisc::root->FindFromPath(str); // only a decaching op, so we cast away constness
    }
  }
  return mobj;
}



//////////////////////////////////
// 	taiMimeSource	//
//////////////////////////////////

taiMimeSource* taiMimeSource::New(const QMimeData* ms_) {
  //TODO: for multi, check for multi source, and create a taiMultiMimeSource obj instead
  taiExtMimeSource* rval =  new taiExtMimeSource(ms_);
  String str;
  if (rval->data(taiClipData::tacss_objectdesc, str) > 0) {
    rval->DecodeDesc(str);
  }
  return rval;
}

taiMimeSource* taiMimeSource::New2(taiClipData* cd) {
  taiIntMimeSource* rval = new taiIntMimeSource(cd);
  return rval;
}

taiMimeSource::taiMimeSource(const QMimeData* ms_)
:inherited()
{
  ms = ms_;
  iter_idx = -1;
}

taiMimeSource::~taiMimeSource() {
  //nothing
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

void taiMimeSource::loc_data_taken() const {
  QString fmt = taiClipData::tacss_locdatataken + QString::number(index());
  data(fmt);
}

void* taiMimeSource::object() const {  // gets the object, if possible. if local, tries to get from path, otherwise tries to make
  //note: we only use mobj for caching, so we aren't really violating const be assigning to it (via unconstifying the mobj)
  if (!(IsThisProcess() && in_range())) return NULL;

  return item()->obj(); // looks up from path
}

int taiMimeSource::object_data(istringstream& result) const {
  if (in_range()) {
    QString fmt = taiClipData::tacss_objectdata + QString::number(index());
    return data(fmt, result);
  } else {
    return 0;
  }
}

taBase* taiMimeSource::tab_object() const {
  taBase* rval = NULL;
  TypeDef* td_ = td();
  if (td_ && td_->InheritsFrom(&TA_taBase))
    rval = (taBase*)object();
  return rval;
}

void taiMimeSource::rem_data_taken() const {
//TODO: this really doesn't work, because qt4 no longer sends things to the other process
  QString fmt = taiClipData::tacss_remdatataken + QString::number(index());
  data(fmt);
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

bool taiExtMimeSource::DecodeDesc(String arg) {

  // s/b in form: src edit action;procid;obj_cnt=N
  // [\n objtype;ta_path]xN
  String str = arg.before(';');
  if (str.length() == 0) return false;
  msrc_action = (int)str;
  arg = arg.after(';');

  str = arg.before(';');
  if (str.length() == 0) return false;
  process_id = (int)str;
  arg = arg.after(';');

  str = arg.before(';');
  if (str.length() == 0) return false;
  int itm_cnt = (int)str;

  for (int i = 0; i < itm_cnt; ++i) {
    arg = arg.after('\n');
    // decode type and get the typedef obj
    String type_name = arg.before(';');
    if (type_name.length() == 0) goto fail;
    arg = arg.after(';');

    // decode path -- could be empty for non-taBase object
    String path = arg.before(';');
    arg = arg.after(';');
    //TODO: skip extension data
    taiMimeItem* msd = new taiRcvMimeItem(type_name, path);
    list.Add(msd);
  }
  setIndex(0);
  return true;

fail:
  list.Reset();
  return false;
}

bool taiExtMimeSource::IsThisProcess() const {
  return (process_id == taPlatform::processId());
}


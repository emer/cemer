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

#include <qapplication.h>
#include <qclipboard.h>
#include <qcstring.h>

#include <sstream>

using namespace std;

// mime-type strings -- for all, XXX is optional if it is 0 (ex for single data case)
//NOTE: strings that are prefix subsets of longer strings MUST come later in search order, so we
// don't incorrectly decode them as XXX index suffix strings
const char* text_plain_iso8859_1 = "text/plain;charset=ISO-8859-1"; // fetch synonym for text_plain
const char* text_plain_utf8 = "text/plain;charset=UTF-8"; // fetch synonym for text_plain
const char* text_plain = "text/plain";
const char* tacss_objectdesc = "tacss/objectdesc";
const char* tacss_objectdata = "tacss/objectdata"; // "tacss/objectdataXXX" where XXX is the index number
const char* tacss_remdatataken = "tacss/remdatataken"; // "tacss/remdatatakenXXX" where XXX is the index number
const char* tacss_locdatataken = "tacss/locdatataken"; // "tacss/locdatatakenXXX" where XXX is the index number

const char* mime_types[] = {
   text_plain,
   tacss_objectdesc,
   tacss_objectdata,
   tacss_remdatataken,
   tacss_locdatataken
};


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
:taiMimeItem()
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


//////////////////////////////////
// 	taiRcvExtMimeItem 	//
//////////////////////////////////

taiRcvMimeItem::taiRcvMimeItem(const String type_name_, const String path_)
:taiMimeItem()
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

taiMimeSource* taiMimeSource::New(const QMimeSource* ms_) {
  //TODO: for multi, check for multi source, and create a taiMultiMimeSource obj instead
  taiExtMimeSource* rval =  new taiExtMimeSource(ms_);
  String str;
  if (rval->encodedData(tacss_objectdesc, str) > 0) {
    rval->DecodeDesc(str);
  }
  return rval;
}

taiMimeSource* taiMimeSource::New2(taiClipData* cd) {
  taiIntMimeSource* rval = new taiIntMimeSource(cd);
  return rval;
}

taiMimeSource::taiMimeSource(const QMimeSource* ms_)
: QMimeSource()
{
  ms = ms_;
  iter_idx = -1;
}

taiMimeSource::~taiMimeSource() {
  //nothing
}

QByteArray taiMimeSource::encodedData(const char * fmt) const {
  if (ms) return ms->encodedData(fmt);
  else return QByteArray();
}

int taiMimeSource::encodedData(const char* mimeType, taString& result) const {
  QByteArray ba = encodedData(mimeType);
  result.set(ba.data(), ba.size());
  return ba.size();
}

int taiMimeSource::encodedData(const char* mimeType, istringstream& result) const {
  QByteArray ba = encodedData(mimeType);
  result.str(string(ba.data(), ba.size()));
  return ba.size();
}

const char* taiMimeSource::format(int i) const {
  if (ms) return ms->format(i);
  else return NULL;
}

int taiMimeSource::index() const {
  return iter_idx;
}

void taiMimeSource::loc_data_taken() const {
  String fmt = tacss_locdatataken + String(index());
  encodedData(fmt.chars());
}

void* taiMimeSource::object() const {  // gets the object, if possible. if local, tries to get from path, otherwise tries to make
  //note: we only use mobj for caching, so we aren't really violating const be assigning to it (via unconstifying the mobj)
  if (!(IsThisProcess() && in_range())) return NULL;

  return item()->obj(); // looks up from path
}

int taiMimeSource::object_data(istringstream& result) const {
  if (in_range()) {
    String fmt = tacss_objectdata + String(index());
    return encodedData(fmt.chars(), result);
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

bool taiMimeSource::provides(const char* mimeType) const {
  if (ms) return ms->provides(mimeType);
  else return false;
}

void taiMimeSource::rem_data_taken() const {
  String fmt = tacss_remdatataken + String(index());
  encodedData(fmt.chars());
}


//////////////////////////////////
// 	taiIntMimeSource	//
//////////////////////////////////

taiIntMimeSource::taiIntMimeSource(taiClipData* cd_)
: taiMimeSource(cd_)
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

taiExtMimeSource::taiExtMimeSource(const QMimeSource* ms_)
: taiMimeSource(ms_)
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


//////////////////////////////////
// 	taiClipData		//
//////////////////////////////////

taiClipData::EditAction taiClipData::ClipOpToSrcCode(int ea) {
  switch (ea & EA_OP_MASK) {
  case EA_CUT:  return EA_SRC_CUT;
  case EA_COPY: return EA_SRC_COPY;
  case EA_DRAG: return EA_SRC_DRAG;
  default:      return (EditAction)0;
  }
}

taiClipData::taiClipData(int src_edit_action_, QWidget* dragSource, const char * name)
:Q3DragObject(dragSource, name)
{
  src_edit_action = src_edit_action_;
}

QByteArray taiClipData::encodedData(const char* mimeType) const {
  int fmt_num = -1;
  int index = 0;
//bool ok =
  DecodeFormat(mimeType, fmt_num, index);
  if (fmt_num == IDX_MD_TEXT_PLAIN)
    fmt_num = IDX_MD_OBJECTDESC;
  return ((taiClipData*) this)->encodedData_impl(fmt_num, index); // we do our non-const cheat here
}

QByteArray taiClipData::encodedData_impl(int , int ) {
  return QByteArray();
}


const char* taiClipData::format(int i) const {
  if (i < 0) return NULL;
  if (i <= IDX_MD_MAX)
    return mime_types[i];

  // must enumerate all index-decorated types as well
  i = i - IDX_MD_MAX - 1; // renormalize back to 0
  if (i >= (count() * 3)) return NULL;

  int fmt = i % 3;
  int idx = i / 3;
  switch (fmt) { //note: an internal cache is not considered part of constness, so we cast it away below
  case 0:
    ((taiMultiClipData*)this)->fmt_cache = tacss_objectdata + String(idx);
    break;
  case 1:
    ((taiMultiClipData*)this)->fmt_cache = tacss_remdatataken + String(idx);
    break;
  case 2:
    ((taiMultiClipData*)this)->fmt_cache = tacss_locdatataken + String(idx);
    break;
  }
  return fmt_cache.chars();
}

bool taiClipData::DecodeFormat(const char* mimeType, int& fmt_num, int& index) const {
  // variants on text/plain -- we accept these, because Qt (or Kde) supplies these externally, regardless
  if ((qstricmp(mimeType, text_plain_utf8) == 0) ||
    (qstricmp(mimeType, text_plain_iso8859_1) == 0))
  {
    fmt_num = IDX_MD_TEXT_PLAIN;
    index = 0;
    return true;
  }

  // for safety, we don't pretend to boggle other text/plain types, since the charset may be inappropriate
  if (qstrnicmp(mimeType, "text/plain;", 11) == 0)
      return false;

  for (int i = 0; i <= IDX_MD_MAX; ++i ) {
    uint len = qstrlen(mime_types[i]);
    if (qstrnicmp(mimeType, mime_types[i], len) != 0) continue;
    // we have the fmt_num, now check if there is an index suffix
    fmt_num = i;
    uint mtlen = qstrlen(mimeType);
    if (mtlen == len)
      index = 0;
    else
      index = (int)String(&(mimeType[len])); // start with first non-matching char -- still a valid /0 terminated string
    return true;
  }
  return false;
}


//////////////////////////////////
// 	taiSingleClipData	//
//////////////////////////////////

taiSingleClipData::taiSingleClipData(taiMimeItem* item_, int src_edit_action_, QWidget* dragSource, const char* name)
:taiClipData(src_edit_action_, dragSource, name)
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
    return QCString(rval.chars());
  }
  case IDX_MD_OBJECTDATA: {
    ostringstream ost;
    item->SetData(ost);
    return QCString(ost.str().c_str());
  }
  case IDX_MD_REMDATATAKEN: { // dst should only call on Cut/Paste or Drag/Move
    if (!(src_edit_action & (EA_SRC_CUT | EA_SRC_DRAG))) break;
    item->RemDataTaken();
//CAN'T DO THIS HERE--CAUSNG CRASH    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QCString("ok"); //helps debugging
  }
  case IDX_MD_LOCDATATAKEN: { //simpler than REM, because internals took care of moving data itself
    item->LocDataTaken();
//    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QCString("ok"); //helps debugging
  }
  }
  return taiClipData::encodedData_impl(fmt_num, index);
}


//////////////////////////////////
// 	tabMultiClipData		//
//////////////////////////////////

taiMultiClipData::taiMultiClipData(taiMimeItem_List* list_, int src_edit_action_, QWidget* dragSource, const char* name)
:taiClipData(src_edit_action_, dragSource, name)
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
    return QCString(rval.chars());
  }
  case IDX_MD_OBJECTDATA: {
    taiMimeItem* mi  = list->SafeEl(index);
    if (mi == NULL) return QByteArray();
    ostringstream ost;
    mi->SetData(ost);
    return QCString(ost.str().c_str());
  }
  case IDX_MD_REMDATATAKEN: { // dst should only call on Cut/Paste or Drag/Move
    if (!(src_edit_action & (EA_SRC_CUT | EA_SRC_DRAG))) break;
    taiMimeItem* mi  = list->SafeEl(index);
    if (mi) mi->RemDataTaken();
  //CAN'T DO THIS HERE--CAUSNG CRASH    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
  //      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QCString("ok"); //helps debugging
  }
  case IDX_MD_LOCDATATAKEN: { //simpler than REM, because internals took care of moving data itself
    taiMimeItem* mi = list->SafeEl(index);
    if (mi) mi->LocDataTaken();
//    if (src_edit_action & (EA_SRC_CUT)) // only clear clipboard for clip ops, not drag ops
//      QApplication::clipboard()->clear(QClipboard::Clipboard);
    return QCString("ok"); //helps debugging
  }
  }
  return taiClipData::encodedData_impl(fmt_num, index);
}



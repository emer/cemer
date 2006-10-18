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


// ta_base.cc

#include "ta_base.h"
#include "ta_defaults.h"
#include "ta_dump.h"
#include "ta_filer.h"
#include "ta_group.h"
#include "ta_project.h" // for taRootBase
#include "ta_TA_type.h"

#ifdef TA_GUI
# include "ta_seledit.h"
# include "ta_qt.h"
# include "ta_qtdata.h"
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "ta_qtgroup.h"
# include "ta_qtbrowse.h"
# include "ta_qtclipdata.h"

# include  <qcolor.h>
# include <QMessageBox>
# include  <qpixmap.h>
#endif

#include <QFileInfo>

#ifdef TA_GUI
static const char* folder_closed_xpm[]={
    "16 16 9 1",
    "g c #808080",
    "b c #c0c000",
    "e c #c0c0c0",
    "# c #000000",
    "c c #ffff00",
    ". c None",
    "a c #585858",
    "f c #a0a0a4",
    "d c #ffffff",
    "..###...........",
    ".#abc##.........",
    ".#daabc#####....",
    ".#ddeaabbccc#...",
    ".#dedeeabbbba...",
    ".#edeeeeaaaab#..",
    ".#deeeeeeefe#ba.",
    ".#eeeeeeefef#ba.",
    ".#eeeeeefeff#ba.",
    ".#eeeeefefff#ba.",
    ".##geefeffff#ba.",
    "...##gefffff#ba.",
    ".....##fffff#ba.",
    ".......##fff#b##",
    ".........##f#b##",
    "...........####."};

static const char* folder_open_xpm[]={
    "16 16 11 1",
    "# c #000000",
    "g c #c0c0c0",
    "e c #303030",
    "a c #ffa858",
    "b c #808080",
    "d c #a0a0a4",
    "f c #585858",
    "c c #ffdca8",
    "h c #dcdcdc",
    "i c #ffffff",
    ". c None",
    "....###.........",
    "....#ab##.......",
    "....#acab####...",
    "###.#acccccca#..",
    "#ddefaaaccccca#.",
    "#bdddbaaaacccab#",
    ".eddddbbaaaacab#",
    ".#bddggdbbaaaab#",
    "..edgdggggbbaab#",
    "..#bgggghghdaab#",
    "...ebhggghicfab#",
    "....#edhhiiidab#",
    "......#egiiicfb#",
    "........#egiibb#",
    "..........#egib#",
    "............#ee#"};

//NOTE: can't create these statically, prior to the QApplication creation, so we must do it this way...
const QPixmap* folder_closed_pixmap() {
  static const QPixmap* pm = NULL;
  if  (pm == NULL)
    pm = new QPixmap(folder_closed_xpm);
  return pm;
}

const QPixmap* folder_open_pixmap() {
  static const QPixmap* pm = NULL;
  if  (pm == NULL)
    pm = new QPixmap(folder_open_xpm);
  return pm;
}

#endif // def TA_GUI

//////////////////////////////////////////
// 		tabMisc			//
//////////////////////////////////////////

taRootBase* tabMisc::root = NULL;

taBase_PtrList 	tabMisc::delayed_remove;
taBase_PtrList 	tabMisc::delayed_updateafteredit;
taBase_PtrList	tabMisc::post_load_opr;

void tabMisc::Close_Obj(TAPtr obj) {
  delayed_remove.Link(obj);
/*obs -- the obj will send a delete notify
#ifdef TA_GUI
  if(taMisc::gui_active) {
    taiMisc::CloseEdits((void*)obj, obj->GetTypeDef());
    return;
  }
#endif */
  //  WaitProc();		// kill me now ?
}

void tabMisc::DeleteRoot() {
  if (tabMisc::root) {
    delete tabMisc::root;
    tabMisc::root = NULL;
  }
}

void tabMisc::WaitProc() {
#ifdef TA_GUI
  taiMisc::PurgeDialogs();
#endif
  for (int i = delayed_remove.size - 1; i >= 0; --i) {
    // if there is only 1 ref count left, we must be it, so just remove,
    // otherwise, try its owner, otherwise, prob an error (too many refs)
    TAPtr it = delayed_remove.FastEl(i);
    // we need to grab count, because we always remove, and it could delete
    int refn = taBase::GetRefn(it); 
    delayed_remove.Remove(i);
    //TODO: maybe should warn if refn>2, since that will mean refs remain
    if (refn > 1) {
      it->Close(); // tries owner if any, else just unrefs, which should delete
    } else if (refn < 1) { // VERY BAD! ref s/nb < 1!
      taMisc::Error("tabMisc::delayed_remove: item had refn<1!");
    }
  }
  if (delayed_updateafteredit.size > 0) {
    int i;
    for(i=0; i<delayed_updateafteredit.size; i++) {
      TAPtr it = delayed_updateafteredit.FastEl(i);
      it->UpdateAfterEdit();
    }
    delayed_updateafteredit.RemoveAll();
  }
}

/*obsbool tabMisc::NotifyEdits(TAPtr obj) {
#ifdef TA_GUI
  return taiMisc::NotifyEdits(obj, obj->GetTypeDef());
#else
  return false;
#endif
} */

void tabMisc::DelayedUpdateAfterEdit(TAPtr obj) {
  delayed_updateafteredit.Link(obj);
}



//////////////////////////
// 	taBase		//
//////////////////////////

/*!
    \class taBase ta_base.h
    \brief The root class for most other classes.

    \ingroup basic
    \mainclass

    \target notifications
    \section1 Change Notifications

    The taBase class provides a data update/change mechanism in conjunction
    with the taDataLink class. Note that the term "attributes" is used to
    refer to any member that controls the way the object behaves or looks;
    "dynamic state" on the other hand, is the set of members that describe
    the object's current parameters, ex. activation for a neuron. State
    changes usually don't invoke the following mechanisms.

    The update mechanism supports the following
    types of changes:

    UpdateAfterEdit -- a change that occurs after a user or script updates
    an attribute of an object. A UAE signaled during a batch update (see below)
    is generally ignored; batch updates therefore need to encompass the
    changes made by an UAE call.

    Batch data update -- this is a nestable update that enables any number
    of nested data changes; the update notification is deferred until all
    updates are complete. A data update implies the change to data members,
    including nested data members, but in general does not include any
    structural changes, such as adding or deleting child objects.

    Batch structure update -- this is a more wide-ranging kind of update
    that includes Batch Data update as a subset, but also comprises changes
    to data members, such as additions or deletions in lists.

    List/Group member insertion -- a container is notified when a new item
    is added.

    Deletion -- a link receives a notification when an object is about to
    be destroyed.

    The following situations are handled automatically, you don't need to
    do anything in your code: List/Group member changes; object deletion.
    The other cases require explicit handling in the code, as follows :

    UAE -- call this->UpdateAfterEdit() after attribute changes that could
    affect the UI or other objects. But *don't* call UAE when updating
    state variables as part of simulations, etc. -- those have their own
    display update mechanism.

    Batch or Struct Data Update -- call the update in pairs, around the
    code in which the changes are taking place (use StructUpdate if
    the change is structural):
    \code
	this->DataUpdate(true);
        \\ data changes go here
	this->DataUpdate(false);
    \endcode

    \section2 Lifetime Management
    
    taBase object lifetimes can be either Statically, or Dynamically 
    managed, and the visibility can either be Internal or External. 
    Static objects are created and destroyed by the compiler; Dynamic 
    objects are created/destroyed by the programmer (new/delete).
    Internal objects are used solely by the managing owner, whereas 
    External objects can be accessed by an external client.
    In practice, this gives rise to three lifetime/visibility scenarios:
    Static External, Dynamic External, and Internal.
     
    Internal objects need not be explicitly lifetime managed, and may
    be created statically or dynamically. Typically, refn remains 0
    for these objects, and no ref counting occurs for them.
    
    External objects can be uniformly treated as if they obeyed ref
    counting semantics. This enables external references to follow a 
    single paradigm. Of course the lifetime of static objects must be
    respected, but the ref counting mechanism will issue a warning if
    a "live" static object is deleted.
    
    Ref Counting Mechanism -- Ref() causes refn++, UnRef() causes refn--;
    if refn goes from 1 to 0, the object deletes. The transition from
    1 to 0 also causes refn to be set to a -ve sentinel value. The debug 
    version of the program can detect double destruction, or ref count
    operations after destruction, based on this sentinel.
    
    Statically Managed -- refn is set to 1 after creation (see below);
    Ref() causes refn++, UnRef() causes refn--; there
    should be no unref that causes refn to go from 1 to 0, so refn s/b
    1 on destruction.
    
    The basic rule is that statically managed objects should always
    be given an Own() operation, or a RefStatic() operation, after 
    creation, to insure the +1 refn is set. There is no corresponding
    unrefing operation.
    
    Most statically owned objects have an Own() operation performed on
    them in the InitLinks routine. Own(obj&, owner*) is basically used
    only when the obj is a static member of owner; therefore, this 
    version of Own automatically calls RefStatic(). It is ok for RefStatic
    to get called multiple times during initialization of an object --
    this enables new objects to get a RefStatic put into their owner's
    Initialize. Own() can detect a change in owner, for objects that
    provide the owner -- this lets it skip the RefStatic() operation.
    The above semantics should cover all normal cases.
    
    
    
*/

String 	taBase::no_name;
int	taBase::no_idx = -1;
MemberDef* taBase::no_mdef = NULL;

#ifdef DEBUG
void taBase::UnRef(TAPtr it) {
  if (--(it->refn) == 0) 
    delete it;
}

void taBase::Done(TAPtr it) {
  if (it->refn == 0)
    delete it;
}
#endif // DEBUG

#define REF_SENT 0x7fffff0

void taBase::DelPointer(TAPtr* ptr) {
  if(*ptr != NULL)
    UnRef(*ptr);
  *ptr = NULL;
}

String taBase::GetStringRep(TAPtr it) {
  if (it == NULL) 
    return String("NULL", 4);
  else
    return it->GetStringRep_impl();
}

void taBase::Own(TAPtr it, TAPtr onr) {
  if (it != NULL) Own(*it, onr);
}

void taBase::Own(taBase& it, TAPtr onr) {
//was causing list transfers to break
//  if (it.GetOwner() == onr) return; // same owner, redundant
  
  Ref(it);
  bool prv_own = (it.GetOwner() != NULL);
  it.SetOwner(onr);
  if (!prv_own)
    it.SetTypeDefaults();
  it.InitLinks();
  if(prv_own) {
//     if(it.InheritsFrom(TA_taNBase))
//       taMisc::Error("*** Warning: Object:",it.GetPath(),
// 		    "was transfered to a new owner, some parameters might have been reset");
  }
}

void taBase::Own(taSmartRef& it, TAPtr onr) {
  it.Init(onr);
}

void taBase::OwnPointer(TAPtr* ptr, TAPtr new_val, TAPtr onr) {
  if (*ptr == new_val) return;
  if (*ptr != NULL)
    UnRef(*ptr);
  *ptr = new_val;
  if(*ptr != NULL)
    Own(*ptr, onr);
}

void taBase::SetPointer(TAPtr* ptr, TAPtr new_val) {
  //note: we ref source first, to implicitly handle identity case (which in practice is probably
  // rare) without needing to do an explicit check
  if (new_val)
    Ref(new_val);
  if (*ptr)
    UnRef(*ptr);
  *ptr = new_val;
}

taFiler* taBase::StatGetFiler(TypeItem* td) {
  if (!taMisc::gui_active) //TODO: shouldn't we always get it???
    return NULL;

  bool cmprs = (td->HasOption("COMPRESS"));
  String filetype = td->OptionAfter("FILETYPE_");
  if (filetype.empty()) filetype = td->name;
  String ext = td->OptionAfter("EXT_");
  if (ext.nonempty()) ext = "." + ext;
  taFiler::FilerFlags ff = (cmprs) ? taFiler::DEF_FLAGS_COMPRESS :  taFiler::DEF_FLAGS;
  taFiler* result = taFiler::New(filetype, ext, ff);
  return result;
}

TypeDef* taBase::StatTypeDef(int) {
  return &TA_taBase;
}

taBase::ValType taBase::ValTypeForType(TypeDef* td) {
  if (td->ptr == 0) {
    if (td->DerivesFrom(TA_bool)) {
      return VT_INT;
    }
    // note: char is generic char, and typically we won't use signed char
    else if (td->DerivesFrom(TA_char)) {
      return VT_STRING; 
    }
    // note: explicit use of signed char is treated like a number
    else if (td->DerivesFrom(TA_unsigned_char))
      return VT_BYTE;
    else if (td->DerivesFrom(TA_signed_char) 
      || td->DerivesFrom(TA_short)
      || td->DerivesFrom(TA_unsigned_short)
      || td->DerivesFrom(TA_int)
      || td->DerivesFrom(TA_unsigned_int)
      ) 
    {
      return VT_INT; 
    }
    else if (td->DerivesFrom(TA_double)) 
    {
      return VT_DOUBLE; 
    }
    else if (td->DerivesFrom(TA_float)) 
    {
      return VT_FLOAT; 
    }
    else if(td->DerivesFormal(TA_enum)) { //maybe we should use String's for these???
      return VT_INT; 
    }
    else if(td->DerivesFrom(TA_taString))
      return VT_STRING;
    else if(td->DerivesFrom(TA_Variant)) {
      return VT_VARIANT;
    }
  }
  return VT_VARIANT;
}

const String taBase::ValTypeToStr(ValType vt) {
  static String str_String("String");
  static String str_double("double");
  static String str_float("float");
  static String str_int("int");
  static String str_byte("byte");
  static String str_Variant("Variant");
  switch (vt) {
  case VT_STRING: return str_String;
  case VT_DOUBLE: return str_double;
  case VT_FLOAT: return str_float;
  case VT_INT: return str_int;
  case VT_BYTE: return str_byte;
  case VT_VARIANT: return str_Variant;
  default: return _nilString; // compiler food
  }
}

void taBase::Destroy() {
#ifdef DEBUG
  SetFlag(DESTROYED);
#endif
}

#ifdef DEBUG
void taBase::CheckDestroyed() {
  assert(!HasFlag(DESTROYED) && "taBase object being multiply destroyed"); 
}
#endif

void taBase::UpdateAfterEdit() {
//obs  tabMisc::NotifyEdits(this);
  DataChanged(DCR_ITEM_UPDATED);
  taBase* _owner = GetOwner();
  if (_owner ) {
    bool handled = false;
    _owner->ChildUpdateAfterEdit(this, handled);
  }
}

void taBase::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  if (handled) return; // note: really shouldn't have been handled already if we are called...
  // call notify if it is an owned member object (but not list/group items)
  if (((char*)child >= ((char*)this)) && ((char*)child < ((char*)this + GetTypeDef()->size))) {
    handled = true;
    DataChanged(DCR_CHILD_ITEM_UPDATED);
    return;
  }
}

void taBase::CutLinks() {
}

void taBase::InitLinks_taAuto(TypeDef* td) {
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if((md->owner != &(td->members)) || !md->type->InheritsFrom(TA_taBase) || (md->type->ptr > 0)) continue;
    taBase* mb = (taBase*)md->GetOff(this);
    taBase::Own(*mb, this);
  }
}

void taBase::CutLinks_taAuto(TypeDef* td) {
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if((md->owner != &(td->members)) || !md->type->DerivesFrom(TA_taBase)) continue;
    if(md->type->ptr == 0) {
      taBase* mb = (taBase*)md->GetOff(this);
      mb->CutLinks();
    }
    else if(md->type->ptr == 1) {
      taBase** mb = (taBase**)md->GetOff(this);
      taBase::DelPointer(mb);
    }
  }
}

void taBase::AddDataView(taDataView* dv) {
#ifdef TA_GUI
  if (dv) dv->SetData(this);
#endif
}

void taBase::AddDataClient(IDataLinkClient* dlc) {
  taDataLink* dl = GetDataLink(); // autocreates if necessary
  if (dl != NULL) {
    dl->AddDataClient(dlc);
  }
}

void taBase::BatchUpdate(bool begin, bool struc) {
  taDataLink* dl = data_link();
  if (!dl) return;
  if (begin) {
    if (struc)
      DataChanged(DCR_STRUCT_UPDATE_BEGIN);
    else
      DataChanged(DCR_DATA_UPDATE_BEGIN);
  } else {
    if (struc)
      DataChanged(DCR_STRUCT_UPDATE_END);
    else
      DataChanged(DCR_DATA_UPDATE_END);
  }
}

void taBase::CallFun(const String& fun_name) {
#ifdef TA_GUI
  if(!taMisc::gui_active) return;
#endif
  MethodDef* md = GetTypeDef()->methods.FindName(fun_name);
  if(md != NULL)
    md->CallFun((void*)this);
  else
    taMisc::Error("*** CallFun Error: function:", fun_name, "not found on object:", this->GetPath());
}

bool taBase::CheckConfig_Gui(bool confirm_success, bool quiet) {
  taMisc::CheckConfigStart(confirm_success, quiet);
  bool ok = CheckConfig_impl(quiet);
  taMisc::CheckConfigEnd(ok);
  return ok;
}

bool taBase::CheckConfig_impl(bool quiet) {
  int cp_flags = m_flags; 
  bool this_rval = true;
  CheckThisConfig_impl(quiet, this_rval);
  if (this_rval) {
    ClearFlag(THIS_INVALID);
  } else {
    SetFlag(THIS_INVALID);
  }
  bool child_rval = true;
  CheckChildConfig_impl(quiet, child_rval);
  if (child_rval) {
    ClearFlag(CHILD_INVALID);
  } else {
    SetFlag(CHILD_INVALID);
  }
  if (cp_flags != m_flags)
    DataChanged(DCR_ITEM_UPDATED);
  return (this_rval && child_rval);
}

void taBase::ClearFlag(int flag) {
  m_flags &= ~flag;
}

void taBase::Close() {
  TAPtr own = GetOwner();
  if (own && own->Close_Child(this))
    return;
  taBase::UnRef(this);
}

void taBase::CloseLater() {
  tabMisc::Close_Obj(this);
}

bool taBase::Close_Child(TAPtr) {
  return false;
}

/*obsbool taBase::CloseEdit() {
#ifdef TA_GUI
  return taiMisc::CloseEdits((void*)this, GetTypeDef());
#endif
  return false;
} */

bool taBase::CopyFrom(TAPtr cpy_from) {
  if(cpy_from == NULL) return false;
  if(!cpy_from->InheritsFrom(GetTypeDef()) && !InheritsFrom(cpy_from->GetTypeDef())) {
    taMisc::Error("Cannot copy from given object of type:",cpy_from->GetTypeDef()->name,
		  "which does not inherit from:", GetTypeDef()->name,
		  "(or I don't inherit from it)", GetPath());
    return false;
  }
  UnSafeCopy(cpy_from);
  return true;
}

bool taBase::CopyTo(TAPtr cpy_to) {
  if(cpy_to == NULL) return false;
  if(!cpy_to->InheritsFrom(GetTypeDef()) && !InheritsFrom(cpy_to->GetTypeDef())) {
    taMisc::Error("Cannot copy to given object of type:",cpy_to->GetTypeDef()->name,
		  "which does not inherit from:", GetTypeDef()->name,
		  "(or I don't inherit from it)",GetPath());
    return false;
  }
  cpy_to->UnSafeCopy(this);
  return true;
}

void taBase::DataChanged(int dcr, void* op1, void* op2) {
  setDirty(true); // note, also then sets dirty for list ops, like Add etc.
  taDataLink* dl = data_link();
  if (dl) dl->DataDataChanged(dcr, op1, op2);
}

void taBase::DataViewAdding(taDataView* dv) {
#ifdef TA_GUI
  taDataLink* dl = GetDataLink(); // sets data_link
  if (dl) dl->AddDataClient(dv);
#endif
  dv->SetData_impl(this);
}

bool taBase::Dump_QuerySaveMember(MemberDef* md) { 
  // default is to save, unless explicit comment directive
  return !md->HasOption("NO_SAVE");
}

bool taBase::DuplicateMe() {
  TAPtr ownr = GetOwner();
  if(ownr == NULL) return false;
  if(!ownr->InheritsFrom(TA_taList_impl)) {
    taMisc::Error("Cannot duplicate me because owner is not a list/group!",GetPath());
    return false;
  }
  taList_impl* own = (taList_impl*)ownr;
  own->DuplicateEl(this);
  return true;
}

int taBase::Edit() {
#ifdef TA_GUI
  taiEdit* ie;
  if((ie = GetTypeDef()->ie) != NULL) {
    const iColor* bgclr = GetEditColorInherit();
    return ie->Edit((void*)this, false, bgclr);
  }
#endif
  return false;
}

TAPtr taBase::FindFromPath(const String& path, MemberDef*& ret_md, int start) const {
  if(((int)path.length() <= start) || (path == ".")) {
    ret_md = NULL;
    return (TAPtr)this;
  }
  if((path == "Null") || (path == "NULL")) {
    ret_md = NULL;
    return NULL;
  }

  TAPtr rval = NULL;
  bool ptrflag = false;
  int length = path.length();

  while(start < length) {
    if(path[start] == '*') {	// path is a pointer
      start += 2;
      ptrflag = true;
    }
    if(path[start] == '.') { 	// must be root, so search on next stuff
      start++;
      continue;
    }

    int delim_pos = taBase::GetNextPathDelimPos(path, start);
    String el_path = path(start,delim_pos-start); // element is between start and delim
    int next_pos = delim_pos+1;
    if((delim_pos < length) && (path[delim_pos] == '['))
      next_pos--;

    void* tmp_ptr;
    MemberDef* md;
    if(((md = FindMembeR(el_path, tmp_ptr)) != NULL) && md->type->InheritsFrom(TA_taBase)) {
      TAPtr mbr = (TAPtr)tmp_ptr;
      if(delim_pos < length) {	// there's more to be done..
	rval = mbr->FindFromPath(path, ret_md, next_pos); // start from after delim
      }
      else {
	rval = mbr;		// that's all folks..
	ret_md = md;
      }
    }
    else if((el_path == "root") && (delim_pos < length)) {
      start = next_pos;	// skip this element since it must be us
      continue;
    }
    if((ptrflag) && (rval != NULL))
      return *((TAPtr *)rval);
    return rval;
  }
  return NULL;
}

const KeyString taBase::key_name("name");
const KeyString taBase::key_type("type");
const KeyString taBase::key_type_desc("type_desc");
const KeyString taBase::key_desc("desc"); 
const KeyString taBase::key_disp_name("disp_name"); 

String taBase::GetColText(const KeyString& key, int /*itm_idx*/) const {
       if (key == key_name) return GetName();
  else if (key == key_type) return GetTypeDef()->name;
  else if (key == key_type_desc) return GetTypeDef()->desc;
// note: some classes override desc with dynamic desc's
  else if (key == key_desc) return GetDesc(); 
  else if (key == key_disp_name) return GetDisplayName(); 
  else return _nilString;
}

taDataLink* taBase::GetDataLink() {
#ifdef TA_GUI
  if (!data_link()) {
    taiViewType* iv;
    if ((iv = GetTypeDef()->iv) != NULL) {
      iv->GetDataLink(this, GetTypeDef()); // sets data_link
    }
  }
#endif
  return data_link();
}

const iColor* taBase::GetEditColor() {
  if (tabMisc::root) return tabMisc::root->GetObjColor(this);
  else return NULL;
}

const iColor* taBase::GetEditColorInherit() {
  const iColor* bgclr = GetEditColor();
  if (!bgclr) {
    TAPtr ownr = GetOwner();
    while ((ownr != NULL) && (bgclr == NULL)) {
      bgclr = ownr->GetEditColor();
      ownr = ownr->GetOwner();
    }
  }
  return bgclr;
}

taFiler* taBase::GetFiler(TypeItem* td) {
  if (!td) td = GetTypeDef();
  return StatGetFiler(td);
}

String taBase::GetDisplayName() const { 
  String rval = GetName(); 
  if (rval.nonempty()) return rval;
  // no name, so try the Owner.path name
  taBase* own = GetOwner();
  if (own) {
    rval.cat(own->GetName()).cat(GetPath(NULL, own));
    if (rval.nonempty()) return rval; // shouldn't be empty!
  }
  // last resort: (TypeName@HexAddr)
  rval = "(" + GetTypeDef()->name + "@" 
      + String(QString::number((intptr_t)this, 16)) + ")";
  return rval;
}

TypeDef* taBase::GetTypeDef() const {
  return &TA_taBase;
}

bool taBase::HasFlag(int flag) const {
  return (m_flags & flag);
}

int taBase::Load(istream& strm, TAPtr par, void** el_) { 
  int rval = GetTypeDef()->Dump_Load(strm, (void*)this, par, el_); 
  if (el_) {
    taBase* el = *((taBase**)el_); // better be!!!
    if (el) el->setDirty(false);
  }
  return rval;
}

int taBase::Load_File(TypeDef* td, void** el_) {
  int rval = false;
  taFiler* flr = GetFiler(td); 
  taRefN::Ref(flr);
  istream* strm = flr->Open();
  taBase* el = NULL;
  if (strm)
    rval = Load(*strm, NULL, (void**)&el);
  
  if (rval && el) {
    el->SetFileName(flr->fname);
  }
  
  taRefN::unRefDone(flr);
  if (el_)
    *el_ = (void*)el;
  return rval;
}

int taBase::LoadAs_File(const String& fname, TypeDef* td, void** el_) {
  if (fname.empty())
    return Load_File(td, el_);
    
  int rval = false;
  taFiler* flr = GetFiler(td); 
  taRefN::Ref(flr);
  flr->fname = fname;
  istream* strm = flr->open_read();
  if (strm) {
    taBase* el = NULL;
    rval = Load(*strm, NULL, (void**)&el);
  
    if (rval && el) {
      
      el->SetFileName(flr->fname);
    }
    if (el_)
      *el_ = (void*)el;
  }
  taRefN::unRefDone(flr);
  return rval;
}

int taBase::Save(ostream& strm, TAPtr par, int indent) { 
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this, par, indent); 
  setDirty(false);
  return rval;
}

int taBase::Save_File() { 
  String fname = GetFileName(); // empty if 1st or not supported
  return SaveAs_File(fname);
}

int taBase::SaveAs_File(const String& fname) {
  int rval = false;
  taFiler* flr = GetFiler(); 
  taRefN::Ref(flr);
  ostream* strm = NULL;
   
  if (fname.nonempty()) {
    // Save mode
    flr->fname = fname;
    strm = flr->Save();
    if (strm)
      rval = Save(*strm);
  } else { 
    // SaveAs mode
    flr->fname = GetName(); // filer etc. does auto extension
    strm = flr->SaveAs();
    if (strm)
      rval = SaveAs(*strm);
  }
  
  if (rval) {
    SetFileName(flr->fname);
  }
  
  taRefN::unRefDone(flr);
  return rval;
}

void taBase::SetFlag(int flag) {
  m_flags |= flag;
}

TAPtr taBase::GetOwner(TypeDef* td) const {
  TAPtr own = GetOwner();
  if(own == NULL)
    return NULL;
  if(own->InheritsFrom(td))
    return own;

  return own->GetOwner(td);
}

String taBase::GetPath_Long(TAPtr ta, TAPtr par_stop) const {
  if((this == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(this != par_stop)
    rval = par->GetPath_Long((TAPtr)this, par_stop);

  if((par != NULL) && (GetName() != ""))
    rval += "(" + GetName() + ")";

  if(ta != NULL) {
    MemberDef* md;
    if((md = FindMember(ta)) != NULL) {
      rval += "." + md->name;
    }
    else if((md = FindMemberPtr(ta)) != NULL) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      rval += ".?.";
    }
  }
  return rval;
}

String taBase::GetPath(TAPtr ta, TAPtr par_stop) const {
  if ((this == par_stop) && (ta == NULL))
    return ".";

  String rval;
  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) rval = "root";
  } else if (this != par_stop)
    rval = par->GetPath((TAPtr)this, par_stop);

  if (ta != NULL) {
    MemberDef* md;
    if ((md = FindMember(ta)) != NULL) {
      rval += "." + md->name;
    } else if ((md = FindMemberPtr(ta)) != NULL) {
      rval = String("*(") + rval + "." + md->name + ")";
    } else {
      rval += ".?.";
    }
  }
  return rval;
}

int taBase::GetNextPathDelimPos(const String& path, int start) {
  int point_idx = path.index('.', start+1); // skip any possible starting delim
  int brack_idx = path.index('[', start+1);

  // if there is a period but not a bracket, or the period is before the bracket
  if(((brack_idx < start) && (point_idx >= start)) ||
     ((point_idx < brack_idx ) && (point_idx >= start)))
  {
    return point_idx;
  }
  else if(brack_idx >= start) {		// else try the bracket
    return brack_idx;
  }
  return path.length();			// delimiter is end of string (its all element)
}

int taBase::GetLastPathDelimPos(const String& path) {
  int point_idx = path.index('.',-1);
  int brack_idx = path.index('[',-1);

  if(point_idx > brack_idx) {		// point comes after bracket
    return point_idx;
  }
  else if(brack_idx >= 0) {
    return brack_idx;
  }
  return 0;
}

TypeDef* taBase::GetScopeType() {
  TypeDef* scp_tp = NULL;
  String scp_nm  = GetTypeDef()->OptionAfter("SCOPE_");
  if (scp_nm.nonempty())
    scp_tp = taMisc::types.FindName(scp_nm);
  if (scp_tp) return scp_tp;
  else return taMisc::default_scope;
}

TAPtr taBase::GetScopeObj(TypeDef* scp_tp) {
  if (!scp_tp)
    scp_tp = GetScopeType();
  if (!scp_tp)
    return tabMisc::root;
  return GetOwner(scp_tp);
}

String taBase::GetStringRep_impl() const {
  String rval = GetTypeDef()->name + ":" + GetPath_Long();
  return rval;
}

void taBase::Help() {
  TypeDef* mytd = GetTypeDef();
  String full_file;
  while((mytd != NULL) && full_file.empty()) {
    String help_file = taMisc::help_file_tmplt;
    help_file.gsub("%t", mytd->name);
    full_file = taMisc::FindFileInclude(help_file);
    mytd = mytd->parents.SafeEl(0);	// go with the parent
  }
  if(full_file.empty()) {
    taMisc::Error("Sorry, no help available for type:", GetTypeDef()->name);
    return;
  }

  String help_cmd = taMisc::help_cmd;
  help_cmd.gsub("%s", full_file);
  system(help_cmd);
}

TAPtr taBase::MakeToken(TypeDef* td) {
  if(td->GetInstance() != NULL) {
    return ((TAPtr)td->GetInstance())->MakeToken();
  }
  else
    return NULL;
}

TAPtr taBase::MakeTokenAry(TypeDef* td, int no) {
  if(td->GetInstance() != NULL) {
    return ((TAPtr)td->GetInstance())->MakeTokenAry(no);
  }
  else
    return NULL;
}

TAPtr taBase::New(int, TypeDef*) {
  return NULL;
}

int taBase::NTokensInScope(TypeDef* td, TAPtr ref_obj, TypeDef* scp_tp) {
  if(ref_obj == NULL)
    return td->tokens.size;
  int cnt = 0;
  int i;
  for(i=0; i<td->tokens.size; i++) {
    TAPtr tmp = (TAPtr)td->tokens.FastEl(i);
    if(tmp->SameScope(ref_obj, scp_tp))
      cnt++;
  }
  return cnt;
}

bool taBase::RemoveDataClient(IDataLinkClient* dlc) {
  taDataLink* dl = data_link(); // doesn't autocreate
  if (dl != NULL) {
    return dl->RemoveDataClient(dlc);
  } else return false;
}

bool taBase::RemoveDataView(taDataView* dv) {
  DataViewRemoving(dv);
  bool rval = false;
#ifdef TA_GUI
  taDataLink* dl = data_link(); // doesn't autocreate
  //note: we should already have a datalink, because we must have added it earlier
  if (dl) rval = dl->RemoveDataClient(dv);
  // note: data_link may be deleted by this point
#endif
  dv->SetData_impl(NULL);
  return rval;
}

bool taBase::ReShowEdit(bool force) {
#ifdef TA_GUI
  return taiMisc::ReShowEdits((void*)this, GetTypeDef(), force);
#endif
  return false;
}

bool taBase::SameScope(TAPtr ref_obj, TypeDef* scp_tp) {
  if (!ref_obj)
    return true;
  if (!scp_tp)
    scp_tp = GetScopeType();
  if (!scp_tp)
    return true;

  TAPtr my_scp = GetOwner(scp_tp);
  if ((!my_scp) || (my_scp == ref_obj) || (my_scp == ref_obj->GetOwner(scp_tp)))
    return true;
  return false;
}

void taBase::SetDefaultName() {
  if(taMisc::not_constr || taMisc::in_init)
    return;
  TypeDef* td = GetTypeDef();
  int tok;
  if(td->tokens.keep && ((tok = td->tokens.Find((void *)this)) >= 0)) {
    String nm = td->name + "_" + String(tok);
    SetName(nm);
  }
}

void taBase::setDirty(bool value) {
  //note: base has no storage, and only forwards dirty (not !dirty)
  if (!value) return;
  taBase* owner;
  if ((owner = GetOwner())) {
    owner->setDirty(value);
  }
}

void taBase::SetTypeDefaults_impl(TypeDef* ttd, TAPtr scope) {
  if(ttd->defaults == NULL) return;
  int i;
  for(i=0; i<ttd->defaults->size; i++) {
    TypeDefault* td = (TypeDefault*)ttd->defaults->FastEl(i);
    TAPtr tdscope = td->GetScopeObj(taMisc::default_scope);
    if(tdscope == scope) {
      td->SetTypeDefaults(this);
      break;
    }
  }
}

void taBase::SetTypeDefaults_parents(TypeDef* ttd, TAPtr scope) {
  int i;
  for(i=0; i<ttd->parents.size; i++) {
    TypeDef* par = ttd->parents.FastEl(i);
    if(!par->InheritsFrom(TA_taBase)) continue;	// only ta-bases
    SetTypeDefaults_parents(par, scope); // first do parents of parent
    if(par->defaults != NULL)
      SetTypeDefaults_impl(par, scope);    // then actually do parent
  }
  if(ttd->defaults != NULL)
    SetTypeDefaults_impl(ttd, scope);    // then actually do it
}

void taBase::SetTypeDefaults() {
  TypeDef* ttd = GetTypeDef();	// this typedef = ttd
  TAPtr scope = GetScopeObj(taMisc::default_scope); // scope for default vals
  SetTypeDefaults_parents(ttd, scope);
}

void taBase::UpdateAllViews() {
  DataChanged(DCR_UPDATE_VIEWS);
}



#ifdef TA_GUI
static void tabase_base_closing_all_gp(TAPtr obj) {
  SelectEdit::BaseClosingAll(obj); // get it before it is moved around and stuff
  // also check for groups and objects in them that might die
  TypeDef* td = obj->GetTypeDef();
  int i;
  for (i=0;i<td->members.size;i++) {
    MemberDef* md = td->members.FastEl(i);
    if(!md->type->InheritsFrom(TA_taList_impl)) continue;
    if(md->type->InheritsFrom(TA_taGroup_impl)) {
      taGroup_impl* gp = (taGroup_impl*)md->GetOff((void*)obj);
      int lf;
      for (lf=0;lf<gp->leaves;lf++) {
	TAPtr chld = (TAPtr)gp->Leaf_(lf);
	if((chld != NULL) && (chld->GetOwner() == gp))
	  tabase_base_closing_all_gp(chld); // get it before it is moved around and stuff
      }
      continue;
    }
    taList_impl* gp = (taList_impl*)md->GetOff((void*)obj);
    int gi;
    for (gi=0;gi<gp->size;gi++) {
      TAPtr chld = (TAPtr)gp->FastEl_(gi);
      if((chld != NULL) && (chld->GetOwner() == gp))
	tabase_base_closing_all_gp(chld); // get it before it is moved around and stuff
    }
  }
}
#endif

bool taBase::ChangeMyType(TypeDef* new_type) {
  TAPtr ownr = GetOwner();
  if((new_type == NULL) || (ownr == NULL)) return false;
  if(!ownr->InheritsFrom(TA_taList_impl)) {
    taMisc::Error("ChangeMyType: Cannot change my type because owner is not a list/group!",GetPath());
    return false;
  }
  if(new_type == GetTypeDef()) {
    taMisc::Error("ChangeMyType: Object is already of selected type!",GetPath());
    return false;
  }
#ifdef TA_GUI
  tabase_base_closing_all_gp(this);
#endif
  taList_impl* own = (taList_impl*)ownr;
  return own->ChangeType(this, new_type);
  // owner will used delayed remove to make this safe!
}

bool taBase::UpdatePointers_NewPar_Ptr(taBase** ptr, taBase* old_par, taBase* new_par,
				 bool null_not_found) {
  if(!*ptr || !old_par || !new_par) return false;
  taBase* old_own = (*ptr)->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false;
  String old_path = (*ptr)->GetPath(NULL, old_par);
  taBase* new_guy = new_par->FindFromPath(old_path);
  if(new_guy)
    taBase::SetPointer(ptr, new_guy);
  else {
    if(null_not_found)
      taBase::SetPointer(ptr, NULL);
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

bool taBase::UpdatePointers_NewPar_SmPtr(taSmartPtr& ref, taBase* old_par, taBase* new_par,
					 bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) return false;
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false;
  String old_path = ref.ptr()->GetPath(NULL, old_par);
  taBase* new_guy = new_par->FindFromPath(old_path);
  if(new_guy)
    ref.set(new_guy);
  else {
    if(null_not_found)
      ref.set(NULL);		// reset to null if not found!
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

bool taBase::UpdatePointers_NewPar_Ref(taSmartRef& ref, taBase* old_par, taBase* new_par,
				 bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) return false;
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false;
  String old_path = ref.ptr()->GetPath(NULL, old_par);
  taBase* new_guy = new_par->FindFromPath(old_path);
  if(new_guy)
    ref.set(new_guy);
  else {
    if(null_not_found)
      ref.set(NULL);		// reset to null if not found!
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

int taBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->type->DerivesFrom(TA_taBase) && (md->type->ptr == 1)) {
      taBase** ptr = (taBase**)md->GetOff(this);
      nchg += taBase::UpdatePointers_NewPar_Ptr(ptr, old_par, new_par);
    }
    else if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taSmartRef)) {
	taSmartRef* ref = (taSmartRef*)md->GetOff(this);
	nchg += taBase::UpdatePointers_NewPar_Ref(*ref, old_par, new_par);
      }
      if(md->type->InheritsFrom(TA_taSmartPtr)) {
	taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
	nchg += taBase::UpdatePointers_NewPar_SmPtr(*ref, old_par, new_par);
      }
      else if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nchg += obj->UpdatePointers_NewPar(old_par, new_par);
      }
    }
  }
  return nchg;
}

bool taBase::UpdatePointers_NewObj_Ptr(taBase** ptr, taBase* ptr_owner, 
				       taBase* old_ptr, taBase* new_ptr) {
  if(!*ptr || (*ptr != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  taBase::SetPointer(ptr, new_ptr);
  ptr_owner->UpdateAfterEdit();	// update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_SmPtr(taSmartPtr& ref, taBase* ptr_owner, 
					 taBase* old_ptr, taBase* new_ptr) {
  if(!ref.ptr() || (ref.ptr() != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  ref.set(new_ptr);
  ptr_owner->UpdateAfterEdit();	// update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_Ref(taSmartRef& ref, taBase* ptr_owner, 
				       taBase* old_ptr, taBase* new_ptr) {
  if(!ref.ptr() || (ref.ptr() != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  ref.set(new_ptr);
  ptr_owner->UpdateAfterEdit();	// update this guy who owns the pointer
  return true;
}

int taBase::UpdatePointersToMe(taBase* new_ptr) {
  taBase* def_scope = GetScopeObj(taMisc::default_scope);
  if(def_scope)
    return UpdatePointersToMe_impl(def_scope, new_ptr);
  return 0;
}

int taBase::UpdatePointersToMe_impl(taBase* scope_obj, taBase* new_ptr) {
  int nchg = scope_obj->UpdatePointers_NewObj(this, new_ptr); // gets all my guys
  nchg += UpdatePointersToMyKids_impl(scope_obj, new_ptr);
  return nchg;
}

int taBase::UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr) {
  int nchg = 0;
  TypeDef* otd = GetTypeDef();
  TypeDef* ntd = new_ptr->GetTypeDef();
  for(int m=0;m<otd->members.size;m++) {
    MemberDef* omd = otd->members[m];
    MemberDef* nmd = NULL;
    if(ntd->members.size > m)
      nmd = ntd->members[m];
    if((omd->type->ptr == 0) && omd->type->InheritsFrom(TA_taBase)) {
      taBase* old_kid = (taBase*)omd->GetOff(this);
      taBase* new_kid = NULL;
      if(nmd && (nmd->type == omd->type)) new_kid = (taBase*)nmd->GetOff(this);
      nchg += old_kid->UpdatePointersToMyKids_impl(scope_obj, new_kid);
    }
  }
  return nchg;
}

int taBase::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if((md->type->ptr == 1) && md->type->DerivesFrom(TA_taBase)) {
      taBase** ptr = (taBase**)md->GetOff(this);
      nchg += taBase::UpdatePointers_NewObj_Ptr(ptr, this, old_ptr, new_ptr);
    }
    else if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taSmartRef)) {
	taSmartRef* ref = (taSmartRef*)md->GetOff(this);
	nchg += taBase::UpdatePointers_NewObj_Ref(*ref, this, old_ptr, new_ptr);
      }
      else if(md->type->InheritsFrom(TA_taSmartPtr)) {
	taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
	nchg += taBase::UpdatePointers_NewObj_SmPtr(*ref, this, old_ptr, new_ptr);
      }
      else if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nchg += obj->UpdatePointers_NewObj(old_ptr, new_ptr);
      }
    }
  }
  return nchg;
}

bool taBase::SelectForEdit(MemberDef* member, SelectEdit* editor, const String& extra_label) {
  if((editor == NULL) || (member == NULL)) return false;
#ifdef TA_GUI
  return editor->SelectMember(this, member, extra_label);
#else
  return false;
#endif
}

bool taBase::SelectForEditNm(const String& member, SelectEdit* editor, const String& extra_label) {
  if (!editor) return false;
#ifdef TA_GUI
  return editor->SelectMemberNm(this, member, extra_label);
#else
  return false;
#endif
}

bool taBase::SelectFunForEdit(MethodDef* function, SelectEdit* editor, const String& extra_label) {
  if (!editor) return false;
#ifdef TA_GUI
  return editor->SelectMethod(this, function, extra_label);
#else
  return false;
#endif
}

bool taBase::SelectFunForEditNm(const String& function, SelectEdit* editor, const String& extra_label) {
  if (!editor) return false;
#ifdef TA_GUI
  return editor->SelectMethodNm(this, function, extra_label);
#else
  return false;
#endif
}


//////////////////////////
//  taSmartPtr		//
//////////////////////////

TypeDef* taSmartPtr::GetBaseType(TypeDef* this_typ) {
  TypeDef* targ = this_typ;
  String act_name;
  while (targ) {
    act_name = targ->name.after("taSmartPtrT_");
    if (act_name.nonempty()) {
      TypeDef* rval = taMisc::types.FindName(act_name); 
      if (rval && rval->InheritsFrom(&TA_taBase))
        return rval;
    }
    targ = targ->GetParent();
  }
  return &TA_taBase; // default
}

//////////////////////////
//  taSmartRef		//
//////////////////////////

void taSmartRef::DataDataChanged(taDataLink*, int dcr, void* op1, void* op2) {
  if (m_own) {
    m_own->SmartRef_DataChanged(this, m_ptr, dcr, op1, op2);
  }
}
void taSmartRef::DataLinkDestroying(taDataLink* dl) {
  if (m_own) {
    m_own->SmartRef_DataDestroying(this, m_ptr); 
    m_ptr = NULL;
    m_own->UpdateAfterEdit();
  } else 
    m_ptr = NULL;
}

//////////////////////////
//	taOBase		//
//////////////////////////

void taOBase::Destroy() {
  CutLinks();
}

void taOBase::CutLinks() {
  if (m_data_link) {
    m_data_link->DataDestroying(); // link NULLs our pointer
  }
  owner = NULL;
// maybe don't do this for only obases, just for nbases..
// #ifdef TA_GUI
//   if(taMisc::gui_active)
//     taiMisc::CloseEdits((void*)this, GetTypeDef());
// #endif
  inherited::CutLinks();
}

/* all in taBase void taOBase::UpdateAfterEdit(){
  inherited::UpdateAfterEdit();
  tabMisc::NotifyEdits(this);
  if (owner && GetTypeDef()->HasOption("UAE_OWNER")) {
    bool handled = false;
    owner->ChildUpdateAfterEdit(this, handled);
  }
}

void taOBase::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  if (!handled && owner)
    owner->ChildUpdateAfterEdit(child, handled);
} */

//////////////////////////
//  taOABase		//
//////////////////////////

void taOABase::CutLinks() {
#ifdef TA_GUI
  SetAdapter(NULL);
#endif
  inherited::CutLinks();
}


#ifdef TA_USE_QT
taBaseAdapter::~taBaseAdapter() {
  if (owner && (owner->adapter == this)) owner->adapter = NULL;
  owner = NULL;
}

void taOABase::SetAdapter(taBaseAdapter* adapter_) {
  if (adapter == adapter_) return;
  if (adapter_) { // setting adapter
    if (adapter) { // chaining
      adapter->setParent(adapter_);
    }
    adapter = adapter_;
  } else { // deleting adapter
    if (adapter) {
      adapter->owner = NULL; // prevents callback delete
      delete adapter; // will delete chained adapters
      adapter = NULL;
    }
  }
}
#endif




//////////////////////////
//	taNBase		//
//////////////////////////

void taNBase::Copy_(const taNBase& cp) {
  if(!cp.name.empty())
    name = cp.name;
}

void taNBase::CutLinks() {
#ifdef TA_GUI
  SelectEdit::BaseClosingAll(this); // close any select edits affecting this guy (before cutting owner!)
#endif
  owner = NULL; //note: also done in taOBase, but we need to do it now, before the CloseEdits call
/*obs -- implicitly done in taOBase
#ifdef TA_GUI
  if (taMisc::gui_active)
    taiMisc::CloseEdits((void*)this, GetTypeDef());
#endif */
  taOBase::CutLinks();
}


//////////////////////////
//  taFBase		//
//////////////////////////

bool taFBase::SetFileName(const String& val) {
  // we get the canonical path, so we insure we can do exact filename compares later
  QFileInfo fi(val);
  file_name = fi.canonicalFilePath(); 
  return true;
}


//////////////////////////
// 	taList_impl	//
//////////////////////////

MemberDef* taList_impl::find_md = NULL;

void taList_impl::Initialize() {
  SetBaseType(&TA_taBase);
  el_def = 0;
}


void taList_impl::Destroy() {
  CutLinks();
}

void taList_impl::CutLinks() {
  if (m_data_link) m_data_link->DataDestroying(); // do early
  RemoveAll();
  inherited_taBase::CutLinks();
}

void taList_impl::Copy(const taList_impl& cp) {
  if(!cp.name.empty())
    name = cp.name;
  el_base = cp.el_base;
  el_typ = cp.el_typ;
  el_def = cp.el_def;
  taPtrList_impl::Copy_Duplicate(cp);
}

void taList_impl::UpdateAfterEdit(){
  inherited_taBase::UpdateAfterEdit();
}

void taList_impl::CheckChildConfig_impl(bool quiet, bool& rval) {
  //note: we have to process everyone, because this is the routine
  // that asserts or clears the state, even if an invalid found early
  for (int i = 0; i < size; ++i) {
    taBase* child = (taBase*)FastEl_(i);
//TODO: is a linked test necessary?? would it not be ok???
    // we only include owned items, not linked
    if (!child || (child->GetOwner() != this)) 
      continue;
    child->CheckConfig(quiet, rval);
  }
}


void taList_impl::ChildUpdateAfterEdit(TAPtr child, bool& handled) {
  // check for embedded member
  inherited_taBase::ChildUpdateAfterEdit(child, handled);
  // otherwise, we assume it is an owned list member
  if (!handled) {
    DataChanged(DCR_LIST_ITEM_UPDATE, child);
    handled = true;
  }
}

String taList_impl::ChildGetColText(void* child, TypeDef* typ, const KeyString& key, 
  int itm_idx) const
{
  if (child && typ && typ->InheritsFrom(&TA_taBase))
    return ChildGetColText_impl((taBase*)child, key, itm_idx);
  else return inherited_taPtrList::ChildGetColText(child, typ, key, itm_idx);
}

String taList_impl::ChildGetColText_impl(taBase* child, const KeyString& key, int itm_idx) const {
  return child->GetColText(key, itm_idx);
}

void taList_impl::DataChanged(int dcr, void* op1, void* op2) {
  inherited_taBase::DataChanged(dcr, op1, op2);
}

String taList_impl::GetColHeading(const KeyString& key) const {
  if (key == key_name) return String("Item");
  else if (key == key_type) return String("Type");
  else if (key == key_desc) return String("Description");
  else return _nilString;
}

const KeyString taList_impl::GetListColKey(int col) const {
  switch (col) {
  case 0: return key_name;
  case 1: return key_type;
  case 2: return key_desc; 
  default: return _nilKeyString;
  }
}


bool taList_impl::ChangeType(int idx, TypeDef* new_type) {
  if(new_type == NULL) return false;
  TAPtr itm = (TAPtr)el[idx];
  if(itm == NULL) return false;
  TypeDef* itd = itm->GetTypeDef();
  if(!new_type->InheritsFrom(itd) && !itm->InheritsFrom(new_type)) {
    // do they have a common parent? if so, convert to that first, then back to new_type
    if(itd->parents.size >= 1) {
      if(new_type->InheritsFrom(itd->GetParent())) {
	ChangeType(idx, itd->GetParent());
	itm = (TAPtr)el[idx];
	Remove(size-1);			// remove the last guy!
      }
      else if((itd->GetParent()->parents.size >= 1) &&
	      new_type->InheritsFrom(itd->GetParent()->GetParent())) {
	ChangeType(idx, itd->GetParent()->GetParent());
	itm = (TAPtr)el[idx];
	Remove(size-1);			// remove the last guy!
      }
      else {
	taMisc::Error("Cannot change to new type:",new_type->name,
		      "which does not inherit from:", itd->name,
		      "(or vice-versa)",itm->GetPath());
	return false;
      }
    }
    else {
      taMisc::Error("Cannot change to new type:",new_type->name,
		    "which does not inherit from:", itd->name,
		    "(or vice-versa)",itm->GetPath());
      return false;
    }
  }
  TAPtr rval = inherited_taBase::MakeToken(new_type);
  if(rval == NULL) return false;
  Add(rval);		// add to end of list
  String orgnm = rval->GetName();
  rval->UnSafeCopy(itm);	// do the copy!
  // if new name is based on type def, give it the appropriate new type def name
  String nwnm = rval->GetName();
  if(nwnm.contains(itm->GetTypeDef()->name))
    rval->SetName(orgnm);
  Swap(idx, size-1);		// switch positions, so old guy is now at end!
  itm->UpdatePointersToMe(rval); // allow us to update all things that might point to us
  tabMisc::Close_Obj(itm);
  // then do a delayed remove of this object (in case called by itself!)
  return true;
}

bool taList_impl::ChangeType(TAPtr itm, TypeDef* new_type) {
  int idx = Find(itm);
  if(idx >= 0)
    return ChangeType(idx, new_type);
  return false;
}

void taList_impl::Close() {
  if (size > 0) {
    RemoveAll();
  }
  inherited_taBase::Close();
}

bool taList_impl::Close_Child(TAPtr obj) {
  return Remove(obj);
}

int taList_impl::Dump_Save_PathR(ostream& strm, TAPtr par, int indent) {
   bool dump_my_path = !(this == par);
  // dump_my_path is a bit of a hack, to enable us to use this same
  // routine either for Dump_Save_PathR or when dumping list items
  // recursively -- when dump_my_path=false, we have already dumped the
  // item path one level above, so only need its members, and children
  
  // first save any sub-members (there usually aren't any)
  int rval = GetTypeDef()->Dump_Save_PathR(strm, (void*)this, (void*)par, indent);

  if (IsEmpty())  return rval;

  strm << "\n";			// actually saving a path: put a newline
  if (dump_my_path) {		
    taMisc::indent(strm, indent);
    Dump_Save_Path(strm, par, indent); // save my path!
    strm << " = [" << size << "] {\n";
    ++indent; // bump up 
  }
  // note: we bumped indent if it is truly nested...
  Dump_Save_PathR_impl(strm, this, indent);
  
  if (dump_my_path) {
    --indent;
    taMisc::indent(strm, indent);
    strm << "};\n";
  } 
  return true;
}

int taList_impl::Dump_Save_PathR_impl(ostream& strm, TAPtr par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  int cnt = 0;
  int i;
  for (i=0; i<size; i++) {
    TAPtr itm = (TAPtr)el[i];
    if(itm == NULL)
      continue;
    cnt++; // sure we are dumping something at this point

    if (El_GetOwner_(itm) != this) { // a link, create a dummy placeholder
      taMisc::indent(strm, indent);
      strm << itm->GetTypeDef()->name << " @[" << i << "] { };\n";
      continue;
    }
    taMisc::indent(strm, indent);
    itm->Dump_Save_Path(strm, par, indent); // must be relative to parent!  not this!
    // can't put this in dump_save_path cuz don't want it during non PathR times..
    if (itm->InheritsFrom(TA_taList_impl)) {
      taList_impl* litm = (taList_impl*)itm;
      if(!litm->IsEmpty()) {
        strm << " = [" << litm->size << "]";
      }
    }
    strm << " { ";
    // NOTE: incredibly sleazy trick, we pass par=this to flag the outer routine
    // not to redo our path etc. -- no easy workaround or redesign
    if (itm->Dump_Save_PathR(strm, itm, indent+1))
      taMisc::indent(strm, indent);
    strm << "};\n";
  }
  return cnt;
}

// actually save all the elements in the group
int taList_impl::Dump_SaveR(ostream& strm, TAPtr par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  String mypath = GetPath(NULL, par);
  int i;
  for(i=0; i<size; i++) {
    if(el[i] == NULL) continue;
    TAPtr itm = (TAPtr)el[i];
    if(El_GetOwner_((TAPtr)el[i]) == this) {
      itm->Dump_Save_impl(strm, par, indent);
    }
    else if(El_GetOwner_(itm) != NULL) {	// a link
      taMisc::indent(strm, indent) << GetTypeDef()->name << " ";
      if(par != NULL) strm << "@";
      strm << mypath << " = ";
      strm << "[" << i << "] = ";
      if(itm->Dump_Save_Path(strm, NULL, indent))
	strm << ";\n";
      if(itm->HasOption("LINK_SAVE"))
	itm->Dump_Save_impl(strm, NULL, indent); // save even though its a link!
    }
  }
  return true;
}

int taList_impl::Dump_Load_Value(istream& strm, TAPtr par) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)	return EOF;
  if(c == ';')	return 2;	// signal that its a path
  if(c == '}') {
    if(strm.peek() == ';') strm.get();
    return 2;
  }

  if(c == '{') {
    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
  }
  else if(c == '=') {		// a link or the number of items to create
    c = taMisc::skip_white(strm);
    if(c == '[') {
      c = taMisc::read_word(strm);
      if(c == ']') {
	int idx = atoi(taMisc::LexBuf);
	c = taMisc::skip_white(strm, true); // peek
	if(c == '=') {		// this means its a link
	  strm.get();
	  c = taMisc::read_word(strm); // get type
	  String typ_nm = taMisc::LexBuf;
	  TypeDef* eltd = taMisc::types.FindName(typ_nm);
	  if((eltd == NULL) || !(eltd->InheritsFrom(el_base))) {
	    taMisc::Error("*** Null or invalid type:",typ_nm,"to link into group:",
			    GetPath(),"of types:",el_base->name);
	    return false;
	  }
	  c = taMisc::read_till_rb_or_semi(strm);
	  String lnk_path = taMisc::LexBuf;
	  dumpMisc::path_subs.FixPath(eltd, tabMisc::root, lnk_path);
	  MemberDef* md;
	  TAPtr tp = tabMisc::root->FindFromPath(lnk_path, md);
	  if(idx < size)
	    ReplaceLink(idx, tp); // if already room, replace it..
	  else {
	    Link(tp);		// otherwise, add it..
	    idx = size-1;
	  }
	  if(tp == NULL) {
	    dumpMisc::vpus.Add((TAPtr*)&(el[idx]), (TAPtr)NULL, lnk_path);
	  }
	  return true;
	}
	else if(c == '{') {	// no type information -- just the count
	  strm.get();		// get the bracket
	  Alloc(idx);		// just make sure we have the ptrs allocated to this size
	  return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
	}
	else {			// type information -- create objects too!
	  taMisc::read_word(strm, true); // get type
	  String typ_nm = taMisc::LexBuf;
	  TypeDef* eltd = taMisc::types.FindName(typ_nm);
	  if((eltd == NULL) || !(eltd->InheritsFrom(el_base))) {
	    taMisc::Error("*** Null or invalid type:",typ_nm,"to create into group:",
			  GetPath(),"of types:",el_base->name);
	    return false;
	  }
	  el_typ = eltd;
	  // ensure that enough items are present (don't do full enforce size)
	  if(size < idx)
	    New(idx - size, el_typ);
	  c = taMisc::skip_white(strm);
	  if(c == '{') {
	    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
	  }
	}
      }
    }
    taMisc::Error("*** Bad formatting for link in group",GetPath(),
		    "of type:",GetTypeDef()->name);
    return false;
  }

  taMisc::Error("*** Missing '{', '=', or '[' in dump file for group:",
		GetPath(),"of type:",GetTypeDef()->name);
  return false;
}

void taList_impl::EnforceSameStru(const taList_impl& cp) {
  int i;
  for(i=0; i<cp.size; i++) {
    TAPtr citm = (TAPtr)cp.el[i];
    if(citm == NULL) continue;
    if(size <= i) {
      TAPtr itm = inherited_taBase::MakeToken(citm->GetTypeDef());
      if(itm != NULL)
	Add_(itm);
    }
    else {
      TAPtr itm = (TAPtr)el[i];
      if(citm->GetTypeDef() == itm->GetTypeDef())
	continue;
      TAPtr rval = inherited_taBase::MakeToken(citm->GetTypeDef());
      if(rval != NULL)
	Replace(i, rval);
    }
  }
  if(size > cp.size)
    for(i=size-1; i>=cp.size; i--)
      Remove(i);
}

void taList_impl::EnforceSize(int sz) {
  if(size < sz)
    New(sz - size);
  else {
    int i;
    for(i=size-1; i>=sz; i--)
      Remove(i);
  }
}

void taList_impl::EnforceType() {
  int i;
  for(i=0; i<size; i++) {
    TAPtr itm = (TAPtr)el[i];
    if((itm == NULL) || (itm->GetTypeDef() == el_typ))
      continue;

    TAPtr rval = inherited_taBase::MakeToken(el_typ);
    if(rval != NULL)
      Replace(i, rval);
  }
}

int taList_impl::Find(TypeDef* it) const {
  int i;
  for(i=0; i < size; i++) {
    if(((TAPtr)el[i])->InheritsFrom(it))
      return i;
  }
  return -1;
}

MemberDef* taList_impl::FindMembeR(const String& nm, void*& ptr) const {
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str != "") {
    idx_str = idx_str.after('[');
    int idx = atoi(idx_str);
    if((size == 0) || (idx >= size)) {
      ptr = NULL;
      return NULL;
    }
    ptr = el[idx];
    return ReturnFindMd();
  }

  int i;
  if((i = Find(nm)) >= 0) {
    ptr = el[i];
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindNameAddrR(nm, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    TAPtr first_el = (TAPtr)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == ((TAPtr) this)))) {
      first_el->FindMembeR(nm, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

MemberDef* taList_impl::FindMembeR(TypeDef* it, void*& ptr) const {
  int i;
  if((i = Find(it)) >= 0) {
    ptr = el[i];
    return ReturnFindMd();
  }

  MemberDef* rval;
  if((rval = GetTypeDef()->members.FindTypeAddrR(it, (void*)this, ptr)) != NULL)
    return rval;
  int max_srch = MIN(taMisc::search_depth, size);
  for(i=0; i<max_srch; i++) {
    TAPtr first_el = (TAPtr)FastEl_(i);
    if((first_el != NULL) && // only search owned objects
       ((first_el->GetOwner()==NULL) || (first_el->GetOwner() == ((TAPtr) this)))) {
      first_el->FindMembeR(it, ptr);
    }
  }
  ptr = NULL;
  return NULL;
}

TAPtr taList_impl::FindType_(TypeDef* it, int& idx) const {
  idx = Find(it);
  if(idx >= 0) return (TAPtr)el[idx];
  return NULL;
}

String taList_impl::GetPath(TAPtr ta, TAPtr par_stop) const {
  if((((TAPtr) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(((TAPtr) this) != par_stop)
    rval = par->GetPath((TAPtr)this, par_stop);

  if (ta != NULL) {
    MemberDef* md;
    if((md = FindMember(ta)) != NULL) {
      rval += "." + md->name;
    }
    else if((md = FindMemberPtr(ta)) != NULL) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      int gidx = Find_(ta);
      if(gidx >= 0)
	rval += "[" + String(gidx) + "]";
      else
	rval += "[?]";
    }
  }
  return rval;
}

String taList_impl::GetPath_Long(TAPtr ta, TAPtr par_stop) const {
  if((((TAPtr) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(((TAPtr) this) != par_stop)
    rval = par->GetPath_Long((TAPtr)this, par_stop);

  if(GetName() != "")
    rval += "(" + GetName() + ")";

  if (ta != NULL) {
    MemberDef* md;
    if((md = FindMember(ta)) != NULL) {
      rval += "." + md->name;
    }
    else if((md = FindMemberPtr(ta)) != NULL) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      int gidx = Find_(ta);
      if(gidx >= 0)
	rval += "[" + String(gidx) + "]";
      else
	rval += "[?]";
    }
  }
  return rval;
}

TAPtr taList_impl::New(int no, TypeDef* typ) {
  if(no == 0) {
#ifdef TA_GUI
    if (taMisc::gui_active)
      return gpiListNew::New(this, no, typ);
#endif
    return NULL;
  }
  if(typ == NULL)
    typ = el_typ;
  if(!typ->InheritsFrom(el_base)) {
    taMisc::Error("*** Attempt to create type:", typ->name,
		   "in list with base type:", el_base->name);
    return NULL;
  }
  TAPtr rval = NULL;
  Alloc(size + no);		// pre-allocate!
  if((size == 0) || (no > 1))
    el_typ = typ;	// first item or multiple items set el_typ
  int i;
  for(i=0; i < no; i++) {
    rval = inherited_taBase::MakeToken(typ);
    if(rval != NULL)
      Add_(rval);
  }
  return rval;
}

ostream& taList_impl::OutputR(ostream& strm, int indent) const {
  taMisc::indent(strm, indent) << name << "[" << size << "] = {\n";
  TypeDef* td = GetTypeDef();
  int i;
  for(i=0; i < td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->HasOption("EDIT_IN_GROUP"))
      md->Output(strm, (void*)this, indent+1);
  }

  for(i=0; i<size; i++) {
    if(el[i] == NULL) continue;
    ((TAPtr)el[i])->OutputR(strm, indent+1);
  }

  taMisc::indent(strm, indent) << "}\n";
  return strm;
}

bool taList_impl::Remove(int i) {
  // default could be out of range..
  if(el_def >= size-1)
    el_def = 0;
  return taPtrList_ta_base::Remove(i);
}

int taList_impl::ReplaceType(TypeDef* old_type, TypeDef* new_type) {
  int nchanged = 0;
  int sz = size;		// only go to current size
  int i;
  for(i=0;i<sz;i++) {
    if(((TAPtr)el[i])->GetTypeDef() != old_type) continue;
    if(ChangeType(i, new_type)) nchanged++;
  }
  return nchanged;
}

MemberDef* taList_impl::ReturnFindMd() const {
  if(find_md != NULL) return find_md;
  find_md = new MemberDef(&TA_taBase, "find_md", "return value", "", "", NULL);
  return find_md;
}

void taList_impl::SetBaseType(TypeDef* it) {
  el_base = it;
  el_typ = it;
}

int taList_impl::SetDefaultEl(TAPtr it) {
  int idx = Find(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultEl(const String& nm) {
  int idx = Find(nm);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultEl(TypeDef* it) {
  int idx = Find(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = taOBase::UpdatePointers_NewPar(old_par, new_par);
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewPar(old_par, new_par);
    }
    else {			// linked item: could be to something outside of scope!
      taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
      if(old_own != old_par) continue;
      String old_path = itm->GetPath(NULL, old_par);
      taBase* nitm = new_par->FindFromPath(old_path);
      if(nitm) {
	ReplaceLink_(i, nitm);
	nchg++;
      }
      else {
	Remove(i);
      }
    }
  }
  return nchg;
}

int taList_impl::UpdatePointers_NewObj(taBase* old_ptr, taBase* new_ptr) {
  int nchg = taOBase::UpdatePointers_NewObj(old_ptr, new_ptr);
  if(old_ptr && (old_ptr->GetOwner() == this)) return 0;
  // do not walk down the guy itself -- its a gonner
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    bool we_own = (itm->GetOwner() == this);
    if(itm == old_ptr) {	   // if it is the old guy, it is by defn a link because we're not the owner..
      if(!new_ptr)		   // if replacement is null, just remove it
	Remove(i);
      else
	ReplaceLink_(i, new_ptr);    // it is a link to old guy; replace it!
      nchg++;
    }
    else if(we_own) {		// only for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewObj(old_ptr, new_ptr);
    }
  }
  return nchg;
}

int taList_impl::UpdatePointersToMyKids_impl(taBase* scope_obj, taBase* new_ptr) {
  int nchg = taOBase::UpdatePointersToMyKids_impl(scope_obj, new_ptr);
  taList_impl* new_list = (taList_impl*)new_ptr;
  for(int i=size-1; i>=0; i--) {
    taBase* oitm = (taBase*)el[i];
    if(!oitm) continue;
    taBase* nitm = NULL;
    if(new_list && (new_list->size > i))
      nitm = (taBase*)new_list->el[i];
    if(nitm && (nitm->GetTypeDef() != oitm->GetTypeDef())) nitm = NULL; // not the same
    nchg += oitm->UpdatePointersToMe_impl(scope_obj, nitm);	// do it all over on this guy
  }
  return nchg;
}

#ifdef TA_GUI
const QPixmap* taList_impl::GetDataNodeBitmap(int bmf, int& flags_supported) const {
  flags_supported |= NBF_FOLDER_OPEN;
  if (bmf & NBF_FOLDER_OPEN)
    return folder_open_pixmap();
  else
    return folder_closed_pixmap();
}

/* TODO:
void taList_impl::QueryEditActions(const QMimeSource* ms, int& allowed, int& forbidden) const {
  // for lists, we generally will just treat a paste/drop as being before the first child item
  // if a list of lists were created, it could replace this method and check for child-type objects, or self-type objects
  QueryChildEditActions(NULL, ms, allowed, forbidden);
} */

/* src_obj will be NULL for out of process
*/
void taList_impl::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  // in general, we allow to CUT and DELETE -- inheriting class can forbid these if it wants
  // for the paste-like ops, we will generally allow insertions of compatible child
  // specific classes will need to replace this method to allow things like linking
  int item_idx = -1;
  if (child) item_idx = Find(child);

  // if it is a list item, or is null, then we can do list operations, so we call our L version
  if ((child == NULL) || (item_idx >= 0))
    ChildQueryEditActionsL_impl(md, child, ms, allowed, forbidden);

  // if child was a list item, then we don't pass the child to the base (since it doesn't boggle list items)
  if (item_idx >=0)
    inherited_taBase::ChildQueryEditActions_impl(md, NULL, ms, allowed, forbidden);
  else
    inherited_taBase::ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
}

void taList_impl::ChildQueryEditActionsL_impl(const MemberDef* md, const taBase* lst_itm, const taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  // SRC ops
  if (lst_itm) {
    // CUT generally always allowed (will either DELETE or UNLINK src item, depending on context)
    allowed |= taiClipData::EA_CUT;
    // Delete only allowed if we are the owner
    if (lst_itm->GetOwner() == this)
      allowed |= taiClipData::EA_DELETE;
    else // otherwise, it is unlinking, not deleting
      allowed |= taiClipData::EA_UNLINK;
  }

  // DST ops

  if (ms == NULL) return; // shouldn't happen -- might mean nothing on clipboard
  // if not a taBase type of object, no more applicable
  if (!ms->is_tab()) return;
  if (!ms->IsThisProcess())
    forbidden &= taiClipData::EA_IN_PROC_OPS; // note: redundant during queries, but needed for L action calls

  // generic list paste allows any subtype of the base type
  bool right_type = ((el_base != NULL) && (ms->td() != NULL) && ms->td()->InheritsFrom(el_base));

  if (right_type)
    allowed |= (taiClipData::EA_PASTE | taiClipData::EA_LINK |
        taiClipData::EA_DROP_COPY | taiClipData::EA_DROP_LINK | taiClipData::EA_DROP_MOVE);

  // LinkGroups only allow linking, not adding
  if (md && md->HasOption("LINK_GROUP"))
    forbidden |= (taiClipData::EA_PASTE | taiClipData::EA_DROP_COPY | taiClipData::EA_DROP_MOVE);

  //TODO: some groups allow promiscuous linking -- they must enable such themselves
  // TODO: maybe we could add a virtual method "CanLink" or somesuch, to let our descendant more closely decide
}

// called by a child -- lists etc. can then allow drops on the child, to indicate inserting into the list, etc.
// this routine just decodes the major category of operation: src, dst inproc, dst ext proc, and then dispatches
int taList_impl::ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea) {
  // if child exists, but is not a list item, then just delegate down to base
  int item_idx = -1;
  if (child) {
    item_idx = Find(child);
    if (item_idx < 0)
      return inherited_taBase::ChildEditAction_impl(md, child, ms, ea);
  }

  // we will be calling our own L routines...
  // however, if child is NULL, and our ops don't do anything, then we must call base ops
  // determine the list-only operations allowed/forbidden, and apply to ea
  int rval = taiClipData::ER_IGNORED;
  int allowed = 0;
  int forbidden = 0;
  ChildQueryEditActionsL_impl(md, child, ms, allowed, forbidden);
  if (ea & forbidden) return -1; // requested op was forbidden
  int eax = ea & (allowed & (~forbidden));

  if (ea & taiClipData::EA_SRC_OPS) {
    // for COPY, CUT, and DRAG, nothing more for us to do; just ack
    if (ea & (taiClipData::EA_COPY | taiClipData::EA_CUT | taiClipData::EA_DRAG))
      return taiClipData::ER_OK;

    rval = ChildEditActionLS_impl(md, child, eax);
  } else  if (ea & taiClipData::EA_DST_OPS) {
    if (ms == NULL) return taiClipData::ER_IGNORED;

    // decode src location
    if (ms->IsThisProcess())
      rval = ChildEditActionLD_impl_inproc(md, item_idx, child, ms, eax);
    else {
      // DST OP, SRC OUT OF PROCESS
      rval = ChildEditActionLD_impl_ext(md, item_idx, child, ms, eax);
    }
  }

  if ((rval == taiClipData::ER_IGNORED) && (child == NULL))
      rval = taBase::ChildEditAction_impl(md, NULL, ms, ea);
  return rval;
}

int taList_impl::ChildEditActionLS_impl(const MemberDef* md, taBase* lst_itm, int ea) {
  if (lst_itm == NULL) return taiClipData::ER_IGNORED;
  switch (ea & taiClipData::EA_OP_MASK) {
  //note: COPY is handled by the child object itself, or outer controller if multi
  case taiClipData::EA_DELETE: {
    Close_Child(lst_itm);
    return taiClipData::ER_OK;
  }
  case taiClipData::EA_UNLINK: {
    Remove(lst_itm);
    return taiClipData::ER_OK;
  }
  default: break; // compiler food
  }
  return taiClipData::ER_IGNORED;
}

int taList_impl::ChildEditActionLD_impl_inproc(const MemberDef* md, int itm_idx, taBase* lst_itm, taiMimeSource* ms, int ea)
{
  if (!ms->is_tab()) return taiClipData::ER_IGNORED; //not taBase
  // itm_idx, -1 means parent itself
  int obj_idx = -1; // -1 means not in this list
  taBase* obj = NULL;

  // only fetch obj for ops that require it
  if (ea & (taiClipData::EA_PASTE | taiClipData::EA_LINK  | taiClipData::EA_DROP_COPY |
    taiClipData::EA_DROP_LINK | taiClipData::EA_DROP_MOVE))
  {
    obj = ms->tab_object();
    if (obj == NULL) {
      taMisc::Error("Could not retrieve object for operation.");
      return taiClipData::ER_ERROR;
    }
    // already in this list? (affects how we do drops/copies, etc.)
    obj_idx = Find(obj);
  }
  
  // All non-move paste ops (i.e., copy an object)
  if (
    (ea & (taiClipData::EA_DROP_COPY)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->src_action() & taiClipData::EA_SRC_COPY))
  ) {
    // TODO: instead of cloning, we might be better off just streaming a new copy
    // since this will better guarantee that in-proc and outof-proc behavior is same
    taBase* new_obj = obj->MakeToken();
    //TODO: maybe the renaming should be delayed until put in list, or maybe better, done by list???
    new_obj->SetDefaultName(); // should give it a new name, so not confused with existing obj
    int new_idx;
    if (itm_idx < 0) 
      new_idx = 0; // if dest is list, then insert at beginning
    else if (itm_idx == (size - 1)) 
      new_idx = -1; // if clicked on last, then insert at end
    else new_idx = itm_idx + 1;
    Insert(new_obj, new_idx);
    new_obj->UnSafeCopy(obj);	// always copy after inserting so there is a full path & initlinks
    return taiClipData::ER_OK;
  }
  
  // All Move-like ops
  if (
    (ea & (taiClipData::EA_DROP_MOVE)) ||
    //  Cut/Paste is a move
    ((ea & taiClipData::EA_PASTE) && (ms->src_action() & taiClipData::EA_SRC_CUT))
  ) {
    if (obj == lst_itm) return 1; // nop
    if (obj_idx >= 0) { // in this list: just do a list move
      // to_idx will differ depending on whether dst is before or after the src object
      if (itm_idx < obj_idx) { // for before, to will be dst + 1
        Move(obj_idx, itm_idx + 1);
      } else if (itm_idx > obj_idx) { // for after, to will just be the dst
        Move(obj_idx, itm_idx);
      } else return taiClipData::ER_OK; // do nothing case of drop on self
    } else { // not in this list, need to do a transfer
      if (Transfer(obj)) { // should always succeed -- only fails if we already own item
      // was added at end, fix up location, if necessary
        Move(size - 1, itm_idx + 1);
      } else return taiClipData::ER_ERROR;
    }
    //NOTE: we never issue a data_taken() because we have actually moved the item ourself
    return taiClipData::ER_OK;
  }

  // Link ops
  if (ea &
    (taiClipData::EA_LINK | taiClipData::EA_DROP_LINK))
  {
    if (obj_idx >= 0) return -1; // in this list: link forbidden
    InsertLink(obj, itm_idx + 1);
    return taiClipData::ER_OK;
  }
  return taiClipData::ER_IGNORED;
}

int taList_impl::ChildEditActionLD_impl_ext(const MemberDef* md, int itm_idx, taBase* lst_itm, taiMimeSource* ms, int ea) {
  // if src is not even a taBase, we just stop
  if (!ms->is_tab()) return taiClipData::ER_IGNORED;

  // DST OPS WHEN SRC OBJECT IS OUT OF PROCESS
  switch (ea & taiClipData::EA_OP_MASK) {
  case taiClipData::EA_DROP_COPY:
  case taiClipData::EA_DROP_MOVE:
  case taiClipData::EA_PASTE:
  {
    istringstream istr;
    if (ms->object_data(istr) > 0) {
      TypeDef* td = GetTypeDef();
      int dump_val = td->Dump_Load(istr, this, this);
      if (dump_val == 0) {
        //TODO: error output
        return taiClipData::ER_ERROR; // load failed
      }
      // delete from source if it was a CUT or similar
      if ((ms->src_action() & (taiClipData::EA_SRC_CUT)) || (ea & (taiClipData::EA_DROP_MOVE)))
      {
        ms->rem_data_taken();
      }
      return taiClipData::ER_OK;
    } else { // no data
        //TODO: error output
      return taiClipData::ER_ERROR;
    }
  }
  }
  return taiClipData::ER_IGNORED;
}

#endif


//////////////////////////
// 	taDataView	//
//////////////////////////

void  taDataView::ChildRemoving(taDataView* child) {} //TEMP, just put in header

void taDataView::Initialize() {
  m_data = NULL;
  data_base = &TA_taBase;
  m_dbu_cnt = 0;
  m_parent = NULL;
  m_index = -1;
}

void taDataView::InitLinks() {
  inherited_taBase::InitLinks();
  //BA 2006-06-28 -- this may have been here for loading, but mdata not set yet at this point
  // so if no other purpose, it can be removed
  if (m_data) m_data->DataViewAdding(this); // note: is ok to make spurious calls to this
}

void taDataView::CutLinks() {
  if (m_data) {
    m_data->RemoveDataView(this); // nulls m_data
  }
  m_parent = NULL;
  inherited_taBase::CutLinks();
}

void taDataView::UpdateAfterEdit() {
  if (taMisc::is_loading) {
    if (m_data) m_data->DataViewAdding(this); // note: is ok to make spurious calls to this
  }
  inherited_taBase::UpdateAfterEdit();
}

void taDataView::DataDataChanged(taDataLink*, int dcr, void* op1_, void* op2_) {
  if (dcr == DCR_STRUCT_UPDATE_BEGIN) { // forces us to be in struct state
    if (m_dbu_cnt < 0) m_dbu_cnt *= -1; // switch state if necessary
    ++m_dbu_cnt;
    return;
  } else if (dcr == DCR_DATA_UPDATE_BEGIN) { // stay in struct state if struct state
    if (m_dbu_cnt > 0) ++m_dbu_cnt;
    else               --m_dbu_cnt;
    return;
  } else if ((dcr == DCR_STRUCT_UPDATE_END) || (dcr == DCR_DATA_UPDATE_END)) {
    bool stru = false;
    if (m_dbu_cnt < 0) ++m_dbu_cnt;
    else {stru = true; --m_dbu_cnt;}
    if (m_dbu_cnt == 0) {
      int pdbu = par_dbu_cnt();
      // we will only signal if no parent update, or if parent is data and we are structural
      if ((pdbu == 0)) {
        if (stru)
          DataStructUpdateEnd_impl();
        else
          DataUpdateView_impl();
      } else if ((pdbu < 0) && stru)
          DataStructUpdateEnd_impl();
    }
    return;
  }
  if ((m_dbu_cnt > 0) || (par_dbu_cnt() > 0))
    return;
  //TODO: need to confirm that supressing UAE's is not harmful...
  if (dcr == DCR_ITEM_UPDATED)
    DataUpdateAfterEdit_impl();
  else if (dcr == DCR_UPDATE_VIEWS)
    DataUpdateView_impl();
  else {
    DataDataChanged_impl(dcr, op1_, op2_);
  }
}

void taDataView::DataLinkDestroying(taDataLink* dl) {
  CutLinks();
  DataDestroying();
  //NOTE: we may be destroyed at this point -- do not put any more code
}

void taDataView::DoActions(DataViewAction acts) {
  // never do any rendering or resetting (incl. children) during load or copying, 
  if (taMisc::is_loading || taMisc::is_duplicating) return; 
  // only the structural reset done if in nogui mode
  
  if (acts & CONSTR_POST) {
    // note: only ever called manually
    Constr_post();
  }
  if (acts & CLEAR_IMPL) {
    // must be mapped to do clear
    if (isMapped())
      Clear_impl();
  }
  
  // no rendering should ever get done if not in gui mode, incl during late shutdown
  if (taMisc::gui_active) {
    if (acts & RENDER_PRE) {
      // must not already be constructed
  //TEMP    if (!isMapped())
        Render_pre();
    }
    // must be mapped for other render steps
    if (isMapped()) {
      if (acts & RENDER_IMPL) {
        Render_impl();
      }
      if (acts & RENDER_POST) {
        Render_post();
      }
    }
  }
  if (acts & CLOSE_WIN_IMPL) {
    if (isMapped())
      CloseWindow_impl();
  }
  if (acts & RESET_IMPL) {
    Reset_impl();
  }
}


String taDataView::GetLabel() const {
  return GetTypeDef()->GetLabel();
}

int taDataView::par_dbu_cnt() {
  taDataView* par = GET_MY_OWNER(taDataView);
  if (par) {
    int pdbu = par->dbu_cnt();
    if (pdbu > 0) return pdbu; //optimization -- don't need to go up the chain if parent is struct
    int ppdbu = par->par_dbu_cnt();
    int rval = abs(pdbu) + abs(ppdbu); // gives number of nestings... now is it structural or data?
    // both parent and grandparent have to not be structural, for result to be data
    if ((pdbu <= 0) && (ppdbu <= 0)) rval *= -1;
    return rval;
  } else
    return 0;
}

void taDataView::SetData(taBase* ta) {
  if (m_data == ta) return;
  if (m_data) {
    m_data->RemoveDataView(this); // nulls m_data
  }
  if (!ta) return;
  if (!ta->GetTypeDef()->InheritsFrom(data_base)) {
    taMisc::Error("*** taDataView::m_data must inherit from ", data_base->name);
  } else {
    ta->DataViewAdding(this); // sets m_data
  }
}

void taDataView::SetData_impl(taBase* ta) {
//TODO: prob don't ref count, since we are added to dude's data link
// maybe we should use a SmartRef instead, and use that instead of dl
//BA 9/18/06  
  taBase::SetPointer((taBase**)&m_data, ta);
}

TAPtr taDataView::SetOwner(TAPtr own) {
  TAPtr rval = inherited_taBase::SetOwner(own);
  m_parent = (taDataView*)GetOwner(parentType()); // NULL if no owner, or no compatible type
  return rval;
}

//////////////////////////////////
//   DataView_List	 	//
//////////////////////////////////


void DataView_List::DataChanged(int dcr, void* op1, void* op2) {
  inherited::DataChanged(dcr, op1, op2);
  taDataView* own = GET_MY_OWNER(taDataView);
  if (own)
    own->DataChanged_Child(this, dcr, op1, op2);
}

void DataView_List::El_disOwn_(void* it) {
  if (data_view) {
    data_view->ChildRemoving((taDataView*)it);
  }
  inherited::El_disOwn_(it);
}

void* DataView_List::El_Own_(void* it) {
  inherited::El_Own_(it);
  if (data_view)
    data_view->ChildAdding((taDataView*)it);
  return it;
}

TAPtr DataView_List::SetOwner(TAPtr own) {
  if (own && own->GetTypeDef()->InheritsFrom(&TA_taDataView))
    data_view = (taDataView*)own;
  else data_view = NULL;
  return inherited::SetOwner(own);
}

void DataView_List::DoAction(taDataView::DataViewAction acts) {
  if (acts && taDataView::CONSTR_MASK) {
    for (int i = 0; i < size; ++i) {
      taDataView* dv = FastEl(i);
      dv->DoActions(acts);
    }
  } else { // DESTR_MASK
    for (int i = size - 1; i >= 0 ; --i) {
      taDataView* dv = FastEl(i);
      dv->DoActions(acts);
    }
    if (acts & taDataView::RESET_IMPL)
      Reset();
  }
}


//////////////////////////
// 	taArray_base	//
//////////////////////////

void taArray_base::CutLinks() {
  Reset();
  owner = NULL;
  taOBase::CutLinks();
}

ostream& taArray_base::Output(ostream& strm, int indent) const {
  taMisc::indent(strm, indent);
  List(strm);
  strm << ";\n";
  return strm;
}

int taArray_base::Dump_Save_Value(ostream& strm, TAPtr, int) {
  strm << "{ ";
  int i;
  for(i=0;i<size;i++) {
    strm << El_GetStr_(FastEl_(i)) << ";";
  }
  return true;
}

int taArray_base::Dump_Load_Value(istream& strm, TAPtr) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(c != '{') {
    taMisc::Error("Missing '{' in dump file for type:",GetTypeDef()->name,"\n");
    return false;
  }
  c = taMisc::read_till_rb_or_semi(strm);
  int cnt = 0;
  while((c == ';') && (c != EOF)) {
    if(cnt > size-1)
      Add_(El_GetTmp_());
    El_SetFmStr_(FastEl_(cnt++), taMisc::LexBuf);
    c = taMisc::read_till_rb_or_semi(strm);
  }
  if (c==EOF)	return EOF;
  return true;
}


void int_Array::FillSeq(int start, int inc) {
  int i, v;
  for(i=0,v=start; i<size; i++, v += inc)
    FastEl(i) = v;
}


#ifdef TA_GUI

/*  ClipBoard operations

For the EditActions and Queries, we follow the pattern below:

  Action result:
    1 - action was performed by the called function
    0 - action was not performed
    -1 - action was forbidden by the called function
    -2 - an error occurred executing the called function

  For Drag init operations, there will be no mime object (NULL).

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
void taBase::QueryEditActions_impl(const taiMimeSource* ms, int& allowed, int& forbidden)
{
  allowed |= taiClipData::EA_COPY;
}

int taBase::EditAction_impl(taiMimeSource* ms, int ea) {
  //note: follows same logic as the Query
  if (ea & taiClipData::EA_COPY) return 1; //note: Ui actually puts the data on the clipboard, when it sees the 1

  return 0;
}

// gives ops allowed on child, with ms being clipboard or drop contents, md valid if we are a member, o/w NULL
void taBase::ChildQueryEditActions(const MemberDef* md, const taBase* child, taiMimeSource* ms,
    int& allowed, int& forbidden)
{
  if (ms && ms->is_multi()) {
    int item_allowed = 0;
    int item_allowed_accum = -1;
    for (int i = 0; i < ms->count(); ++i) {
      ms->setIndex(i);
      // try it for every item -- we only ultimately allow what is allowed for all items
      ChildQueryEditActions_impl(md, child, ms, item_allowed, forbidden);
      item_allowed_accum &= item_allowed;
    }
    allowed |= item_allowed_accum;
  } else {
    ChildQueryEditActions_impl(md, child, ms, allowed, forbidden);
  }
}

void taBase::ChildQueryEditActions_impl(const MemberDef* md, const taBase* child, const taiMimeSource* ms,
  int& allowed, int& forbidden)
{
  if (ms == NULL) return; // querying for src ops only

  // if src action was Cut, that limits the Dst ops
  if (ms->src_action() & (taiClipData::EA_SRC_CUT))
   forbidden |= taiClipData::EA_FORB_ON_SRC_CUT;

  // can't link etc. if not in this process
  if (!ms->IsThisProcess())
    forbidden |= taiClipData::EA_IN_PROC_OPS;
}

int taBase::ChildEditAction(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea)
{
  int rval = 0;
  if (ms && ms->is_multi()) {
    // note: we go backwards, since that seems to work best (ex. causes pasted items to end up in correct order)
    for (int i = ms->count() - 1; i >= 0 ; --i) {
      ms->setIndex(i);
      // if a src op, then child is the operand so look up each iteration, else it is dest, so leave it be
      if (ea & taiClipData::EA_SRC_OPS)
        child = ms->tab_object();
      rval = ChildEditAction_impl(md, child, ms, ea); //note: we just return the last value
    }
  } else {
    rval = ChildEditAction_impl(md, child, ms, ea);
  }
  return rval;
}

// this routine just decodes the major category of operation: src, dst and then dispatches
int taBase::ChildEditAction_impl(const MemberDef* md, taBase* child, taiMimeSource* ms, int ea) {
  if (ea & taiClipData::EA_SRC_OPS) {
    return ChildEditActionS_impl(md, child, ea);
  } else {
    return ChildEditActionD_impl(md, child, ms, ea);
  }
}
#endif // TA_GUI


//////////////////////////
// 	SArg_Array	//
//////////////////////////

void SArg_Array::Initialize() {
}

void SArg_Array::InitLinks() {
  String_Array::InitLinks();
  taBase::Own(labels, this);
}

void SArg_Array::UpdateAfterEdit() {
  labels.EnforceSize(size);
  inherited::UpdateAfterEdit();
}

void SArg_Array::Copy_(const SArg_Array& cp) {
  labels = cp.labels;
}

int SArg_Array::Dump_Save_Value(ostream& strm, TAPtr par, int indent) {
  int rval = String_Array::Dump_Save_Value(strm, par, indent);
  strm << "};" << endl;
  int rv2 = labels.Dump_Save_Value(strm, this, indent+1);
  return (rval && rv2);
}

int SArg_Array::Dump_Load_Value(istream& strm, TAPtr par) {
  int rval = String_Array::Dump_Load_Value(strm, par);
  int c = taMisc::skip_white(strm, true); // peek
  if(c == '{') {
    labels.Dump_Load_Value(strm, this);
  }
  return rval;
}




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


// ta_base.cc

#include <sstream>

#include "ta_base.h"
#include "ta_defaults.h"
#include "ta_dump.h"
#include "ta_filer.h"
#include "ta_group.h"
#include "ta_project.h" // for taRootBase, Doclinks
#include "ta_TA_type.h"
#include "ta_seledit.h"

#ifdef TA_USE_QT
# include <QStringList>
# include <QDir>
#endif
#ifdef TA_GUI
# include "ta_qt.h"
# include "ta_qtdata.h"
# include "ta_qtdialog.h"
# include "ta_qttype_def.h"
# include "ta_qtgroup.h"
# include "ta_qtclipdata.h"
# include "ta_qtviewer.h" // for Help Browser
# include "ta_seledit_qt.h" // for obj diff browser
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
taBase*	    tabMisc::cur_undo_save_top = NULL;
taBase*	    tabMisc::cur_undo_mod_obj = NULL;
taBase*	    tabMisc::cur_undo_save_owner = NULL;

taBase_RefList 	tabMisc::delayed_close;
taBase_RefList 	tabMisc::delayed_updateafteredit;
taBase_FunCallList  tabMisc::delayed_funcalls;
ContextFlag  tabMisc::in_wait_proc;

void tabMisc::DelayedClose(taBase* obj) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_close.AddUnique(obj); // only add once!!!
}

void tabMisc::DelayedUpdateAfterEdit(taBase* obj) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_updateafteredit.Add(obj);
}

void tabMisc::DelayedFunCall_gui(taBase* obj, const String& fun_name) {
  if(taMisc::quitting) return;
  if(!taMisc::gui_active) return;
  taMisc::do_wait_proc = true;
  delayed_funcalls.AddBaseFun(obj, fun_name);
}

void tabMisc::DelayedFunCall_nogui(taBase* obj, const String& fun_name) {
  if(taMisc::quitting) return;
  taMisc::do_wait_proc = true;
  delayed_funcalls.AddBaseFun(obj, fun_name);
}

void tabMisc::WaitProc() {
  if (in_wait_proc) return; // already doing it!
  ++in_wait_proc;
  // prevent reentrant waitprocs!
#ifdef TA_GUI
  taiDataHostBase::AsyncWaitProc();
  taiMisc::PurgeDialogs();
#endif
  bool did = false;
  bool rval = DoDelayedCloses();
  did |= rval;
  rval = DoDelayedUpdateAfterEdits();
  did |= rval;
  rval = DoDelayedFunCalls();
  did |= rval;

  if(!did) {
    DoAutoSave();		// only once it is quiet
  }
  --in_wait_proc;
}


/*
  Object on this list will only have refs if they are owned, ex. in a list.
  If the owner or mgr nukes the obj before we get to it, it will have been
  automatically removed from the delayed_close list. Otherwise, we will
  "nuke with extreme prejudice".
  Note that it might be possible for objects to get added to the list
  while deleting something -- we won't do those this cycle.
*/
bool tabMisc::DoDelayedCloses() {
  if (delayed_close.size == 0) return false;
  // note: this is sleazy, but very efficient -- we just completely 
  // hijack the memory of the current list in this working copy
  taBase_RefList items;
  items.Hijack(delayed_close);
  // we work fifo
  while (items.size > 0) {
    // note: active items can often have many refs, and CutLinks will typically
    // resolve those (ex. Projection, which can have many cons)
    // so if item is owned, we just defer to it, otherwise
    taBase* it = items.FastEl(0);
    items.RemoveIdx(0);
#ifdef DEBUG
    int refn = taBase::GetRefn(it); 
    if ((it->GetOwner() == NULL) && (refn != 1)) {
      taMisc::Warning("tabMisc::delayed_close: item had refn != 1, was=",
        String(refn), "type=", it->GetTypeDef()->name, "name=",
        it->GetName());
    }
#endif
    it->Close();
  }
  return true;
}

bool tabMisc::DoDelayedUpdateAfterEdits() {
  bool did_some = false;
  if (delayed_updateafteredit.size > 0) {
    while(delayed_updateafteredit.size > 0) {
      taBase* it = delayed_updateafteredit.Pop();
      it->UpdateAfterEdit();
    }
    did_some = true;
  }
  return did_some;
}

bool tabMisc::DoDelayedFunCalls() {
  bool did_some = false;
  while (delayed_funcalls.size > 0) {// note this must be fifo!
    // note: get all details before call, then remove
    FunCallItem* fci = delayed_funcalls.FastEl(0);
    taBase* it = fci->it;
    String fun_name = fci->fun_name;
    delayed_funcalls.RemoveIdx(0); // deletes fci
    if (it) {
      it->CallFun(fun_name);
    }
    did_some = true;
  }
  return did_some;
}

bool tabMisc::DoAutoSave() {
  if(!tabMisc::root) return false;
  bool did = false;
  taProject* proj;
  taLeafItr i;
  FOR_ITR_EL(taProject, proj, tabMisc::root->projects., i) {
    bool rval = proj->AutoSave();
    did |= rval;
  }
  return did;
}

void tabMisc::WaitProc_Cleanup() {
  taMisc::do_wait_proc = false;	// just to be sure
  delayed_updateafteredit.Reset();
  delayed_funcalls.Reset();
  delayed_close.Reset();
}

void tabMisc::DeleteRoot() {
  tabMisc::WaitProc_Cleanup();
  if (tabMisc::root) {
    delete tabMisc::root;
    tabMisc::root = NULL;
  }
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
ContextFlag	taMisc::is_loading;

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
        \\ data changes go hereContextFlag	taMisc::is_loading;

	this->DataUpdate(false);
    \endcode

    \section2 Lifetime Management
    
    taBase object lifetimes can be either Statically, or Dynamically 
    managed, and the visibility can either be Internal or External. 
    Static objects are created and destroyed by the compiler; Dynamic 
    objects are created/destroyed by the programmer ContextFlag	taMisc::is_loading;
(new/delete).
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
    1 to 0 also causes refn to be set to a -ve sentiContextFlag	taMisc::is_loading;
nel value. The debug 
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

///////////////////////////////////////////////////////////////////////////
// 	Reference counting mechanisms, all static just for consistency..

#ifdef DEBUG
void taBase::Ref(taBase& it) { 
  Ref(&it);
}
void taBase::Ref(taBase* it) {
  it->refn.ref();
}

void taBase::unRef(taBase* it) {
  if (it->refn <= 0) {
    cerr << "WARNING: taBase::unRef: taBase refn < 0 for item\n";
  } else
    it->refn.deref();
}

void taBase::Done(taBase* it) {
  if (it->refn == 0) {
    if (it->HasBaseFlag(DESTROYED))
      cerr << "WARNING: taBase::Done: taBase refn == 0 but item already destroyed\n";
    else
      delete it;
  }
}
#endif // DEBUG

#define REF_SENT 0x7fffff0

void taBase::Own(taBase* it, taBase* onr) {
  if (it != NULL) Own(*it, onr);
}

void taBase::Own(taBase& it, taBase* onr) {
//was causing list transfers to break
//  if (it.GetOwner() == onr) return; // same owner, redundant
  
  Ref(it);
//   bool prv_own = (it.GetOwner() != NULL);
  it.SetOwner(onr);
//   if (!prv_own)
//     it.SetTypeDefaults();
  it.InitLinks();
//   if(prv_own) {
//     if(it.InheritsFrom(TA_taNBase))
//       taMisc::Warning("*** Warning: Object:",it.GetPath(),
// 		    "was transfered to a new owner, some parameters might have been reset");
//   }
}

void taBase::Own(taSmartRef& it, taBase* onr) {
  it.Init(onr);
}

///////////////////////////////////////////////////////////////////////////
// 	Pointer management routines (all pointers should be ref'd!!)

void taBase::SetPointer(taBase** ptr, taBase* new_val) {
  //note: we ref source first, to implicitly handle identity case (which in practice is probably
  // rare) without needing to do an explicit check
  if (new_val)
    Ref(new_val);
  if (*ptr)
    UnRef(*ptr);
  *ptr = new_val;
}

void taBase::OwnPointer(taBase** ptr, taBase* new_val, taBase* onr) {
  if (*ptr == new_val) return;
  if (*ptr != NULL)
    UnRef(*ptr);
  *ptr = new_val;
  if(*ptr != NULL)
    Own(*ptr, onr);
}

void taBase::DelPointer(taBase** ptr) {
  if (*ptr != NULL) {
    UnRef(*ptr);
    *ptr = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////
//	Basic constructor/destructor ownership/initlink etc interface

void taBase::CutLinks() {
}

void taBase::InitLinks_taAuto(TypeDef* td) {
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if((md->owner != &(td->members)) || (md->type->ptr > 0)) continue;
    if(md->type->InheritsFrom(TA_taBase)) {
      taBase* mb = (taBase*)md->GetOff(this);
      taBase::Own(*mb, this);
    }
    else if(md->type->InheritsFrom(TA_taSmartRef)) {
      taSmartRef* sr = (taSmartRef*)md->GetOff(this);
      taBase::Own(*sr, this);
    }
  }
}

void taBase::CutLinks_taAuto(TypeDef* td) {
  // go in reverse order because sometimes later members refer to earlier member items
  for(int i=td->members.size-1; i>=0; i--) {
    MemberDef* md = td->members.FastEl(i);
    if((md->owner != &(td->members)) || !md->type->DerivesFrom(TA_taBase)) continue;
    if(md->type->ptr == 0) {
      taBase* mb = (taBase*)md->GetOff(this);
      mb->CutLinks();
    }
    else if(md->type->ptr == 1 && !md->HasOption("NO_SET_POINTER")) {
      taBase** mb = (taBase**)md->GetOff(this);
      taBase::DelPointer(mb);
    }
  }
}

void taBase::Destroy() {
#ifdef DEBUG
  SetBaseFlag(DESTROYED);
//TEMP -- try to find evil Mac issue
  if (tabMisc::delayed_close.FindEl(this) >= 0) {
    int i = 0;
    ++i;
    taMisc::Warning("taBase object being destroyed on DelayedClose list");
  }
#endif
}

#ifdef DEBUG
void taBase::CheckDestroyed() {
  if(HasBaseFlag(DESTROYED)) {
    taMisc::Error("taBase object being multiply destroyed");
  }
}
#endif

void taBase::Destroying() {
  if (HasBaseFlag(DESTROYING)) return; // already done by parent
  SetBaseFlag(DESTROYING);
  //note: following gets called in destructor of higher class, so 
  // its vtable accessor for datalinks will be valid in that context
  taDataLink* dl = data_link();
  if (dl) {
    dl->DataDestroying();
    delete dl; // NULLs our ref
  }
}

void taBase::SetTypeDefaults_impl(TypeDef* ttd, taBase* scope) {
  if(ttd->defaults == NULL) return;
  int i;
  for(i=0; i<ttd->defaults->size; i++) {
    TypeDefault* td = (TypeDefault*)ttd->defaults->FastEl(i);
    taBase* tdscope = td->GetScopeObj(taMisc::default_scope);
    if(tdscope == scope) {
      td->SetTypeDefaults(this);
      break;
    }
  }
}

void taBase::SetTypeDefaults_parents(TypeDef* ttd, taBase* scope) {
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
  taBase* scope = GetScopeObj(taMisc::default_scope); // scope for default vals
  SetTypeDefaults_parents(ttd, scope);
}

///////////////////////////////////////////////////////////////////////////
// actual constructors/destructors and related: defined in TA_BASEFUNS for derived classes

taBase* taBase::MakeToken(TypeDef* td) {
  if(td->GetInstance() != NULL) {
    return ((taBase*)td->GetInstance())->MakeToken();
  }
  else
    return NULL;
}

taBase* taBase::MakeTokenAry(TypeDef* td, int no) {
  if(td->GetInstance() != NULL) {
    return ((taBase*)td->GetInstance())->MakeTokenAry(no);
  }
  else
    return NULL;
}

TypeDef* taBase::GetTypeDef() const {
  return &TA_taBase;
}

TypeDef* taBase::StatTypeDef(int) {
  return &TA_taBase;
}


///////////////////////////////////////////////////////////////////////////
//	Object managment flags (taBase supports up to 8 flags for basic object mgmt purposes)

bool taBase::HasBaseFlag(int flag) const {
  return (base_flags & flag);
}

void taBase::SetBaseFlag(int flag) {
  base_flags = (BaseFlags)(base_flags | flag);
}

void taBase::ClearBaseFlag(int flag) {
  base_flags = (BaseFlags)(base_flags & ~flag);
}

bool taBase::isDestroying() const {
  if(HasBaseFlag(DESTROYING)) return true;
  taBase* own = GetOwner();
  if(own) return own->isDestroying();
  return false;
}

int taBase::GetEditableState(int mask) const {
// note: this routine is not virtual, nor should it be
  int rval = GetThisEditableState_impl(mask);
  if (rval) return rval;
  return GetOwnerEditableState_impl(mask);
}

int taBase::GetThisEditableState_impl(int mask) const {
  return base_flags & mask;
// to extend, call the inherited, and then augment the flags
// with your own -- you shouldn't make it "less" readonly than
// than your inherited call returns, but you can make it more
}

int taBase::GetOwnerEditableState_impl(int mask) const {
// to stop an object from querying higher, just stub this function out
// (ie replace with one that returns 0)
  taBase* own = GetOwner();
  if (own) return own->GetEditableState(mask); // note: must be full, not Own..._impl
  return 0;
}

///////////////////////////////////////////////////////////////////////////
//	Basic object properties: index in list, owner, name, description, etc

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

String taBase::GetUniqueName() const { 
  return GetPath_Long();
}

taBase* taBase::GetOwner(TypeDef* td) const {
  taBase* own = GetOwner();
  if(own == NULL)
    return NULL;
  if(own->InheritsFrom(td))
    return own;

  return own->GetOwner(td);
}

taBase* taBase::GetThisOrOwner(TypeDef* td) {
  if (InheritsFrom(td)) return this;
  return GetOwner(td);
}

taBase* taBase::GetParent() const	{ 
  taBase* rval = GetOwner();
  while (rval && (rval->InheritsFrom(TA_taList_impl)))
    rval = rval->GetOwner();
  return rval;
} 

bool taBase::IsChildOf(const taBase* obj) const {
  // note: we define ourself as a child of ourself
  const taBase* tobj = this;
  do {
    if (tobj == obj) return true;
  } while ((tobj = tobj->GetOwner()));
  return false;
}

bool taBase::IsParentOf(const taBase* obj) const {
  // note: we define ourself as a parent of ourself
  do {
    if (this == obj) return true;
  } while ((obj = obj->GetOwner()));
  return false;
}


void taBase::SetDefaultName_impl(int idx) {
  // name root -- use an explicit name root if any, else just the class name
  TypeDef* td = GetTypeDef();
  String nm = td->OptionAfter("DEF_NAME_ROOT_");
  if (nm.empty()) nm = td->name;
  // name style -- 0 is the legacy one
  int nm_style = td->OptionAfter("DEF_NAME_STYLE_").toInt(); // 0 if not present
  if(idx < 0) idx = 0;	// avoid -1 not found: can't just have typedef name for programs etc
  if (nm_style == 2) { // no number (for things that are unique in a container)
    ;
  } 
  else if (nm_style == 1) {
    nm += String(++idx); // use 1-based, no _
  } else { // 0, or unknown style -- use legacy
    nm += ("_" + String(idx)); 
  }
  SetName(nm);
}

//note: normally we dont' call SetDefaultName unless keeping tokens...
void taBase::SetDefaultName_() {
  TypeDef* td = GetTypeDef();
  if (td->HasOption("DEF_NAME_LIST")) {
    SetName(_nilString); // must clear, since desc class may already have set
    return; // not actually done until added to list
  }
//nn  if (!td->tokens.keep) return;
  int idx = td->tokens.FindEl((void *)this);
//nn  if (idx < 0) return;
  SetDefaultName_impl(idx);
}

String taBase::GetStateDecoKey() const {
  if(GetEnabled() == 0)
    return "NotEnabled";
  if(HasBaseFlag(taBase::THIS_INVALID))
    return "ThisInvalid";
  if(HasBaseFlag(taBase::CHILD_INVALID))
    return "ChildInvalid";
  int spec_st = GetSpecialState();
  if(spec_st < 1)
    return _nilString;
  return "SpecialState" + String(spec_st);
}


///////////////////////////////////////////////////////////////////////////
//	Paths in the structural hierarchy

String taBase::GetPath_Long(taBase* ta, taBase* par_stop) const {
  if((this == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(this != par_stop)
    rval = par->GetPath_Long((taBase*)this, par_stop);

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

String taBase::GetPath(taBase* ta, taBase* par_stop) const {
  if ((this == par_stop) && (ta == NULL))
    return ".";

  String rval;
  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) rval = "root";
  } else if (this != par_stop)
    rval = par->GetPath((taBase*)this, par_stop);

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

taBase* taBase::FindFromPath(const String& path, MemberDef*& ret_md, int start) const {
  if(((int)path.length() <= start) || (path == ".")) {
    ret_md = NULL;
    return (taBase*)this;
  }
  if((path == "Null") || (path == "NULL")) {
    ret_md = NULL;
    return NULL;
  }

  taBase* rval = NULL;
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

    MemberDef* md;
    void* tmp_ptr = FindMembeR(el_path, md);
    if(tmp_ptr && (!md || md->type->InheritsFrom(TA_taBase))) { // null md = taBase
      taBase* mbr = (taBase*)tmp_ptr;
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
      return *((taBase* *)rval);
    return rval;
  }
  return NULL;
}

Variant taBase::GetValFromPath(const String& path, MemberDef*& ret_md, bool warn_not_found) const {
  String eff_path = path;
  void* eff_base = (void*)this;
  TypeDef* eff_typ = GetTypeDef();
  while(eff_path.contains('.')) {
    String membname = eff_path.before('.');
    eff_path = eff_path.after('.');
    MemberDef* omd = eff_typ->members.FindName(membname);
    if(!omd) {
      TestError(warn_not_found, "GetValFromPath", "member:", membname, "not found in object of type:",
		eff_typ->name); 
      return _nilVariant;
    }
    eff_base = omd->GetOff(eff_base);
    if(!eff_base) {
      TestError(warn_not_found, "GetValFromPath", "could not get base value for member:", membname,
		"in object of type:", eff_typ->name); 
      return _nilVariant;
    }
    eff_typ = omd->type;
    if(eff_typ->InheritsFrom(&TA_taBase)) {
      return ((taBase*)eff_base)->GetValFromPath(eff_path, ret_md, warn_not_found);
    }
  }
  MemberDef* md = eff_typ->members.FindName(eff_path);
  if(!md) {
    TestError(warn_not_found, "GetValFromPath", "member:", eff_path, "not found in object of type:",
	      eff_typ->name); 
    return _nilVariant;
  }
  eff_base = md->GetOff(eff_base);
  if(!eff_base) {
    TestError(warn_not_found, "GetValFromPath", "could not get base value for member:", eff_path,
	      "in object of type:", eff_typ->name); 
    return _nilVariant;
  }
  eff_typ = md->type;
  ret_md = md;
  return eff_typ->GetValVar(eff_base, md);
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

taBase* taBase::GetScopeObj(TypeDef* scp_tp) {
  if (!scp_tp)
    scp_tp = GetScopeType();
  if (!scp_tp)
    return tabMisc::root;
  return GetOwner(scp_tp);
}

bool taBase::SameScope(taBase* ref_obj, TypeDef* scp_tp) {
  if (!ref_obj)
    return true;
  if (!scp_tp)
    scp_tp = GetScopeType();
  if (!scp_tp)
    return true;

  taBase* my_scp = GetOwner(scp_tp);
  if ((!my_scp) || (my_scp == ref_obj) || (my_scp == ref_obj->GetOwner(scp_tp)))
    return true;
  return false;
}

int taBase::NTokensInScope(TypeDef* td, taBase* ref_obj, TypeDef* scp_tp) {
  if(ref_obj == NULL)
    return td->tokens.size;
  int cnt = 0;
  int i;
  for(i=0; i<td->tokens.size; i++) {
    taBase* tmp = (taBase*)td->tokens.FastEl(i);
    if(tmp->SameScope(ref_obj, scp_tp))
      cnt++;
  }
  return cnt;
}

taBase* taBase::GetUndoBarrier() {
  taBase* own = GetOwner();
  if(!own) return NULL;
  if(own->HasOption("UNDO_BARRIER")) return own;
  return own->GetUndoBarrier();
}

////////////////////////////////////////////////////////////////////// 
// 	Saving and Loading to/from files

String taBase::GetFileNameFmProject(const String& ext, const String& tag, const String& subdir, bool dmem_proc_no) {
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return _nilString;
  String proj_base_nm = proj->file_name;
  if(proj_base_nm.contains(".proj"))
    proj_base_nm = proj_base_nm.before(".proj");
  String sd = subdir;
  if(!sd.empty()) {
    if(sd.lastchar() != '/')
      sd += '/';
    if(proj_base_nm.contains('/')) {
      String base_dir = proj_base_nm.before('/',-1);
      String fnm = proj_base_nm.after('/',-1);
      proj_base_nm = base_dir + "/" + sd + fnm;
    }
    else {
      proj_base_nm = sd + proj_base_nm;
    }
  }
  String dms;
  if(dmem_proc_no && (taMisc::dmem_nprocs > 1)) {
    dms = ".p" + taMisc::LeadingZeros(taMisc::dmem_proc, 2);
  }
  String rval = proj_base_nm + tag + dms + ext;
  return rval;
}

taFiler* taBase::StatGetFiler(TypeItem* td, String exts, int compress, String filetypes)
{
  bool cmprs = (compress <= 0); // either default or none
  if (td) {
    if (compress >= 0) cmprs = compress;
    else cmprs = (td->HasOption("COMPRESS"));
    if (filetypes.empty()) {
      filetypes = td->OptionAfter("FILETYPE_");
      if (filetypes.empty()) filetypes = td->name;
    }
    if (exts.empty()) {
      exts = td->OptionAfter("EXT_");
      if (exts.nonempty() && (exts[0] != '.')) {
        exts = "." + exts;
      }
    }
  }
  taFiler::FilerFlags ff = (cmprs) ? taFiler::DEF_FLAGS_COMPRESS : taFiler::DEF_FLAGS;
  taFiler* result = taFiler::New(filetypes, exts, ff);
  return result;
}

taFiler* taBase::GetFiler(TypeItem* td, const String& exts,
  int compress, const String& filetypes)
{
  if (!td) td = GetTypeDef();
  return StatGetFiler(td, exts, compress, filetypes);
}

int taBase::Load_strm(istream& strm, taBase* par, taBase** loaded_obj_ptr) { 
  StructUpdate(true);
  Dump_Load_pre();
  int rval = GetTypeDef()->Dump_Load(strm, (void*)this, par, (void**)loaded_obj_ptr); 
// #ifdef DEBUG
//   taMisc::Info("load struct update:",GetName());
// #endif
  StructUpdate(false);
  if(loaded_obj_ptr) {
    if(*loaded_obj_ptr) (*loaded_obj_ptr)->setDirty(false);
  }
  return rval;
}

void AppendFilerInfo(TypeDef* typ, String& exts, int& compress, String& filetypes) {
  if (typ->HasOption("COMPRESS"))
    compress = 1;
  String str = typ->OptionAfter("EXT_");
  if (str.nonempty()) {
    if (exts.nonempty()) exts.cat(",");
    if (str[0] != '.')  exts.cat(".");
    exts.cat(str);
  }
  str = typ->OptionAfter("FILETYPE_");
  if (str.nonempty()) {
    if (filetypes.nonempty()) filetypes.cat(",");
    filetypes.cat(str);
  }
}    


taFiler* taBase::GetLoadFiler(const String& fname, String exts,
  int compress, String filetypes, bool getset_file_name) 
{
  // get names/types here, because save/load are different
  TypeDef* typ = GetTypeDef(); // will be replaced with item type if we are a list
  if (exts.empty()) {
    // if we are a list, get for the default typ
    if (InheritsFrom(&TA_taList_impl)) {
      taList_impl* ths = (taList_impl*) this;
      typ = ths->el_base;
      if (typ)
        AppendFilerInfo(typ, exts, compress, filetypes);
      // and additionally, if a group, we can also load subgroups
      if (InheritsFrom(&TA_taGroup_impl)) {
        // keep prev typ for use in calcing context
        TypeDef* typ2 = GetTypeDef(); // we are the group
        AppendFilerInfo(typ2, exts, compress, filetypes);
      }
    } else { // otherwise, loading over an item, so just get from us
      AppendFilerInfo(typ, exts, compress, filetypes);
    }
  }
  taFiler* flr = StatGetFiler(NULL, exts, compress, filetypes); 
  taRefN::Ref(flr);
   
  if(fname.nonempty()) {
    flr->SetFileName(fname);
    flr->open_read();
  } else { 
    String tfname;
    if (getset_file_name)
      tfname = GetFileName();
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(tfname); // filer etc. does auto extension
    flr->Open();
  }
  if(flr->istrm && getset_file_name) {
    SetFileName(flr->FileName());
  }
  return flr;
}

int taBase::Load_cvt(taFiler*& flr) {
  int c = taMisc::read_till_eol(*flr->istrm);
  if(c == EOF) return false;	// bail
  if(!taMisc::LexBuf.contains("// ta_Dump File v1.0")) {
    flr->Close();
    flr->open_read();
    return true;		// continue
  }
  // first determine file type
  String_PArray key_strs;
  for(int i=0;i<taMisc::file_converters.size;i++) {
    DumpFileCvt* cvt = taMisc::file_converters[i];
    key_strs.Add(cvt->key_srch_str);
  }
  int typ_id = taMisc::find_strings(*flr->istrm, key_strs);
  flr->Close();			// file is done with anyway
  flr->open_read();		// read again in any case
  if(TestWarning((typ_id < 0), "Load_cvt",
		 "Old format file could not be identified; not converting!")) return false;
  DumpFileCvt* cvt = taMisc::file_converters[typ_id];
  taFiler* cvt_flr = taFiler::New(flr->filetype, flr->defExt());
  taRefN::Ref(cvt_flr);
  String cvt_fname = flr->FileName();
  QDir::setCurrent(taMisc::GetDirFmPath(cvt_fname));	
  String cvt_tag = "_v4precvt";
  if(!flr->defExt().empty()) {
    if(cvt_fname.contains(flr->defExt())) cvt_fname = cvt_fname.before(flr->defExt());
    cvt_fname += cvt_tag + flr->defExt();
  }
  else {
    String ex;
    if(cvt_fname.contains('.')) {
      ex = cvt_fname.after('.',-1);
      cvt_fname = cvt_fname.before('.',-1);
    }
    cvt_fname += cvt_tag + "." + ex;
  }
  cvt_flr->SetFileName(cvt_fname);
  cvt_flr->open_write();
  taMisc::replace_strings(*flr->istrm, *cvt_flr->ostrm, cvt->repl_strs);
  flr->Close();
  cvt_flr->Close();
  taRefN::unRefDone(flr);	// get rid of orig filer
  flr = cvt_flr;		// use new one
  flr->open_read();		// read the converted file
  int chs = taMisc::Choice("Note: converting old file of type: " + cvt->proj_type_base + 
		".\n Created intermediate cvt file as: " + cvt_fname + 
		".\n Many error messages are likely (and should be ignored)," +
		 "\n and you will probably need to convert object using convert button",
        "Continue", "Cancel");
  if (chs == 0) 
    return true;
  else return false;
}

int taBase::Load(const String& fname, taBase** loaded_obj_ptr) {
  int rval = false;
  taFiler* flr = GetLoadFiler(fname, _nilString, -1, _nilString);
  if(flr->istrm) {
    if(Load_cvt(flr)) {		// do conversion if needed
      taBase* lobj = NULL;
      rval = Load_strm(*flr->istrm, NULL, &lobj);
      if (loaded_obj_ptr)
	*loaded_obj_ptr = lobj;
      if(rval && lobj) {
	lobj->SetFileName(flr->FileName());
	if(taMisc::gui_active) {
	  if (lobj == this) { 
	    tabMisc::DelayedFunCall_gui(lobj, "RebuildAllViews");
	  } else { // loaded a new guy
	    tabMisc::DelayedFunCall_gui(lobj, "BrowserSelectMe");
          }
	}
      }
    }
  }
  flr->Close();
  taRefN::unRefDone(flr);
  return rval;
}

int taBase::Save_strm(ostream& strm, taBase* par, int indent) { 
  int rval = GetTypeDef()->Dump_Save(strm, (void*)this, par, indent); 
  setDirty(false);
  return rval;
}

taFiler* taBase::GetSaveFiler(const String& fname, String exts,
  int compress, String filetypes, bool getset_file_name)
{
  // get names/types here, because save/load are different
  TypeDef* typ = NULL; // we are the group
  if (exts.empty()) {
    // if we are a list, get for the default typ
    typ = GetTypeDef(); // we are the group
    AppendFilerInfo(typ, exts, compress, filetypes);
  }
  taFiler* flr = StatGetFiler(typ, exts, compress, filetypes); 
  taRefN::Ref(flr);
   
  if (fname.nonempty()) {
    flr->SetFileName(fname);
    flr->FixFileName();
    flr->Save();
  } else { 
    String tfname;
    if (getset_file_name)
      tfname = GetFileName();
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(tfname); // filer etc. does auto extension
    flr->SaveAs();
  }
  
  if (flr->ostrm && getset_file_name) {
    SetFileName(flr->FileName());
    // don't notify! very dangerous in middle of save, and also marks Dirty
   // DataChanged(DCR_ITEM_UPDATED);
  }
  return flr;
}

taFiler* taBase::GetAppendFiler(const String& fname, const String& ext, int compress,
  String filetypes, bool getset_file_name) 
{
  taFiler* flr = GetFiler(NULL, ext, compress, filetypes); 
  taRefN::Ref(flr);
   
  if (fname.nonempty()) {
    flr->SetFileName(fname);
    flr->open_append();
  } else { 
    String tfname;
    if (getset_file_name)
      tfname = GetFileName();
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(tfname); // filer etc. does auto extension
    flr->Append();
  }
  
  if(flr->ostrm && getset_file_name) {
    SetFileName(flr->FileName());
  }
  return flr;
}

int taBase::Save() { 
  String fname = GetFileName(); // empty if 1st or not supported
  return SaveAs(fname);
}

int taBase::SaveAs(const String& fname) {
  int rval = false;
  taFiler* flr = GetSaveFiler(fname, _nilString, -1, _nilString);
  if (flr->ostrm) {
    Save_strm(*(flr->ostrm));
    flr->Close();
    rval = true;
  }
  taRefN::unRefDone(flr);
  DataChanged(DCR_ITEM_UPDATED_ND);
  return rval;
}


int taBase::Save_String(String& save_str, taBase* par, int indent) {
  ostringstream oss;
  int rval = Save_strm(oss, par,indent);
  save_str = oss.str().c_str();
  return rval;
}


int taBase::Load_String(const String& load_str, taBase* par, taBase** loaded_obj_ptr) {
  istringstream iss(load_str.chars());
  int rval = Load_strm(iss, par, loaded_obj_ptr);
  return rval;
}

taBase::DumpQueryResult taBase::Dump_QuerySaveMember(MemberDef* md) { 
  return DQR_DEFAULT;
}

taBase* taBase::Dump_Load_Path_ptr(const String& el_path, TypeDef* ld_el_typ) {
  MemberDef* el_md = NULL;
  taBase** nw_el_ptr = (taBase**)FindMembeR(el_path, el_md);
  if(!nw_el_ptr) {
    taMisc::Warning("*** Dump_Load_Path_ptr: Could not find pointer member at path:",
		    el_path,"in parent object:",GetPath());
    return NULL;
  }
  if(!el_md) {
    taMisc::Warning("*** Dump_Load_Path_ptr: no el_md for item at path:",
		    el_path,"in parent object:",GetPath(),
		    "may not set pointers correctly!");
  }
  taBase* nw_el = *nw_el_ptr;

  if(nw_el && (nw_el->GetTypeDef() != ld_el_typ) &&
     !((nw_el->GetTypeDef() == &TA_taBase_List) && (ld_el_typ == &TA_taBase_Group)))
  {
    // object not the right type, try to create new one..
    if(taMisc::verbose_load >= taMisc::MESSAGES) {
      taMisc::Warning("*** Object in parent:",GetPath(),"at path", el_path,
		    "of type:",nw_el->GetTypeDef()->name,"is not the right type:",
		      ld_el_typ->name,", attempting to create new one");
    }
    if(el_md && (el_md->HasOption("OWN_POINTER") || !el_md->HasOption("NO_SET_POINTER"))) {
      taBase::DelPointer(nw_el_ptr);
    }
    else {
      nw_el_ptr = NULL;
    }
    nw_el = NULL;		// nuked it
  }

  if(!nw_el) {
    *nw_el_ptr = taBase::MakeToken(ld_el_typ);
    nw_el = *nw_el_ptr;
    if(!nw_el) {
      taMisc::Warning("*** Dump_Load_Path_ptr: Could not make new token of type:",
		      ld_el_typ->name,"for pointer member at path:",
		      el_path,"in parent object:",GetPath());
      return NULL;
    }
    if(el_md && el_md->HasOption("OWN_POINTER")) { // note: this was not in original!
      taBase::Own(nw_el,this);
    }
    else {
      taMisc::Warning("*** Dump_Load_Path_ptr: NOT owning new element of type:",
		      ld_el_typ->name,"for pointer member at path:",
		      el_path,"in parent object:",GetPath());
    }
  }
  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Success: Leaving TypeDef::Dump_Load_Path_ptr, type: " << ld_el_typ->name
	 << ", parent path = " << GetPath()
	 << ", el_path = " << el_path
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return nw_el;
}

taBase* taBase::Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ) {
  MemberDef* el_md = NULL;
  taBase* nw_el = (taBase*)FindMembeR(el_path, el_md);
  if(nw_el) return nw_el;
  taMisc::Warning("*** Object at path:",GetPath(),
		  "is not capable of creating a new element with the path:",el_path,
		  "of type:",ld_el_typ->name,
		  "something is askew in the loading paths");
  return NULL;
}

String taBase::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			 bool force_inline) const {
  if (sc == TypeDef::SC_DEFAULT) 
    sc = (taMisc::is_saving) ? TypeDef::SC_STREAMING : TypeDef::SC_VALUE;

  TypeDef* td = GetTypeDef();
  if(force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP")) {
    return td->GetValStr_class_inline(this, par, memb_def, sc, force_inline);
  }
  else {
    if(GetOwner() || (this == tabMisc::root))
      return GetPath();
    return td->name;
  }
}

String taBase::GetValStr_ptr(const TypeDef* td, const void* base, void* par, MemberDef* memb_def,
			     TypeDef::StrContext sc, bool force_inline) {
  taBase* rbase = *((taBase**)base);
  if(rbase && (rbase->GetOwner() || (rbase == tabMisc::root))) {
    if (sc == TypeDef::SC_STREAMING) {
      return dumpMisc::path_tokens.GetPath(rbase);	// use path tokens when saving..
    }
    else {
      return rbase->GetPath();
    }
  }
  else {
    if(rbase != NULL)
      return String((intptr_t)rbase);
  }
  return String::con_NULL;
}

bool taBase::SetValStr(const String& val, void* par, MemberDef* memb_def, 
		       TypeDef::StrContext sc, bool force_inline) {
  if (sc == TypeDef::SC_DEFAULT) 
    sc = (taMisc::is_saving) ? TypeDef::SC_STREAMING : TypeDef::SC_VALUE;

  if(force_inline || HasOption("INLINE") || HasOption("INLINE_DUMP")) {
    TypeDef* td = GetTypeDef();
    td->SetValStr_class_inline(val, this, par, memb_def, sc, force_inline);
    if (sc != TypeDef::SC_STREAMING)
      UpdateAfterEdit(); 	// only when not loading (else will happen after)
    return true;
  }
  return false;			// not processed otherwise..
}

bool taBase::SetValStr_ptr(const String& val, TypeDef* td, void* base, void* par,
			   MemberDef* memb_def, TypeDef::StrContext sc, bool force_inline) {
  taBase* bs = NULL;
  if((val != String::con_NULL) && (val != "Null")) {
    String tmp_val(val); // FindFromPath can change it
    if (sc == TypeDef::SC_STREAMING) {
      bs = dumpMisc::path_tokens.FindFromPath(tmp_val, td, base, par, memb_def);
      if(!bs)	// indicates error condition
	return false;
    }
    else {
      MemberDef* md = NULL;
      bs = tabMisc::root->FindFromPath(tmp_val, md);
      if(!bs) {
	taMisc::Warning("*** Invalid Path in SetValStr:",val);
	return false;
      }
      if(md) {			// otherwise it is a taBase* 
	if(md->type->ptr == 1) {
	  bs = *((taBase**)bs);
	  if(bs == NULL) {
	    taMisc::Warning("*** Null object at end of path in SetValStr:",val);
	    return false;
	  }
	}
	else if(md->type->ptr != 0) {
	  taMisc::Warning("*** ptr count != 1 in path:", val);
	  return false;
	}
      }
    }
  }
  if (memb_def && memb_def->HasOption("OWN_POINTER")) {
    if(!par)
      taMisc::Warning("*** NULL parent for owned pointer:",val);
    else
      taBase::OwnPointer((taBase**)base, bs, (taBase*)par);
  }
  else {
    if (memb_def && memb_def->HasOption("NO_SET_POINTER"))
      (*(taBase**)base) = bs;
    else
      taBase::SetPointer((taBase**)base, bs);
  }
  return true;
}

taBaseObjDiffRecExtra::taBaseObjDiffRecExtra(taBase* tab) {
  tabref = tab;
}

taObjDiffRec* taBase::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def, 
			   const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // always just add a record for this guy
  taObjDiffRec* odr = new taObjDiffRec(odl, nest_lev, GetTypeDef(), memb_def, (void*)this,
				       (void*)par, par_typ, par_od);
  odl.Add(odr);
  if(GetOwner())
    odr->extra = new taBaseObjDiffRecExtra((taBase*)this);

  GetTypeDef()->GetObjDiffVal_class(odl, nest_lev, this, memb_def, par, par_typ, odr);
  return odr;
}

///////////////////////////////////////////////////////////////////////////
// 	Updating of object properties

void taBase::UpdateAfterEdit() {
  if (isDestroying()) return;
  UpdateAfterEdit_impl();
  DataChanged(DCR_ITEM_UPDATED);
  /*TEST */
  taBase* _owner = GetOwner();
  if (_owner ) {
    bool handled = false;
    _owner->ChildUpdateAfterEdit(this, handled);
  } /* */
}

void taBase::UpdateAfterEdit_NoGui() {
  if (isDestroying()) return;
  UpdateAfterEdit_impl();
}

void taBase::ChildUpdateAfterEdit(taBase* child, bool& handled) {
  if (handled) return; // note: really shouldn't have been handled already if we are called...
  // call notify if it is an owned member object (but not list/group items)
  if (((char*)child >= ((char*)this)) && ((char*)child < ((char*)this + GetTypeDef()->size))) {
    handled = true;
    DataChanged(DCR_CHILD_ITEM_UPDATED);
    return;
  }
}

void taBase::UpdateAfterMove(taBase* old_owner) {
  UpdateAfterMove_impl(old_owner);
  //  DataChanged(DCR_ITEM_UPDATED);  no extra notify -- list takes care of it.  should
  // just _impl doing updating of pointers etc -- just have _impl stuff because it always
  // ends up being needed eventually..
}

void taBase::UpdateAllViews() {
  if(taMisc::gui_active)
    DataChanged(DCR_UPDATE_VIEWS);
}

void taBase::RebuildAllViews() {
  if(taMisc::gui_active)
    DataChanged(DCR_REBUILD_VIEWS);
}

void taBase::DataChanged(int dcr, void* op1, void* op2) {
  if(taMisc::is_loading)  return; // no notifies while loading!!

  if (dcr != DCR_ITEM_UPDATED_ND)
    setDirty(true); // note, also then sets dirty for list ops, like Add etc.
  // only assume stale for strict condition:
  if ((useStale() && (dcr == DCR_ITEM_UPDATED)))
    setStale();
  taDataLink* dl = data_link();
  if (dl) dl->DataDataChanged(dcr, op1, op2);
}

void taBase::setDirty(bool value) {
  //note: base has no storage, and only forwards dirty (not !dirty)
  if (!value) return;
  taBase* owner;
  if ((owner = GetOwner())) {
    owner->setDirty(value);
  }
}

void taBase::setStale() {
  taBase* owner;
  if ((owner = GetOwner())) {
    owner->setStale();
  }
}

///////////////////////////////////////////////////////////////////////////
//	Data Links -- notify other guys when you change

taDataLink* taBase::GetDataLink() {
  if (!data_link()) {
    if (isDestroying()) {
#ifdef DEBUG
      taMisc::Warning("Attempt to GetDataLink on a destructing object");
#endif
      return NULL;
    }
    taiViewType* iv;
    if ((iv = GetTypeDef()->iv) != NULL) {
      iv->GetDataLink(this, GetTypeDef()); // sets data_link
    }
  }
  return data_link();
}

bool taBase::AddDataClient(IDataLinkClient* dlc) {
  // refuse new links while destroying!
  if (isDestroying()) {
#ifdef DEBUG
    TestWarning(true,"AddDataClient","Attempt to add a DataLinkClient to a destructing object");
#endif
    return false;
  }
  taDataLink* dl = GetDataLink(); // autocreates if necessary
  if (dl != NULL) {
    dl->AddDataClient(dlc);
    return true;
  }
#ifdef DEBUG
  else {
    TestError(true, "AddDataClient","Attempt to add a DataLinkClient to an obj with no DataLink!");
  }
#endif
  return false;
}

bool taBase::RemoveDataClient(IDataLinkClient* dlc) {
  taDataLink* dl = data_link(); // doesn't autocreate
  if (dl != NULL) {
    return dl->RemoveDataClient(dlc);
  } else return false;
}

void taBase::BatchUpdate(bool begin, bool struc) {
//  taDataLink* dl = data_link(); // doesn't autocreate -- IMPORTANT!
//  if (!dl) return;
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

void taBase::SmartRef_DataDestroying(taSmartRef* ref, taBase* obj) {
  UpdateAfterEdit();
}


///////////////////////////////////////////////////////////////////////////
//	Checking the configuration of objects prior to using them


void taBase::CheckError_msg(const char* a, const char* b, const char* c,
			    const char* d, const char* e, const char* f,
			    const char* g, const char* h) const {
  String objinfo = "Config Error in: " + GetTypeDef()->name + " "
    + GetDisplayName() + " (path: " + GetPath_Long() + ")\n";
  taMisc::CheckError(objinfo, a, b, c, d, e, f, g, h);
}

bool taBase::CheckConfig_Gui(bool confirm_success, bool quiet) {
  taMisc::CheckConfigStart(confirm_success, quiet);
  bool ok = CheckConfig_impl(quiet);
  taMisc::CheckConfigEnd(ok);
  return ok;
}

bool taBase::CheckConfig_impl(bool quiet) {
  int cp_flags = base_flags; 
  bool this_rval = true;
  CheckThisConfig_impl(quiet, this_rval);
  if (this_rval) {
    ClearBaseFlag(THIS_INVALID);
  } else {
    SetBaseFlag(THIS_INVALID);
  }
  bool child_rval = true;
  CheckChildConfig_impl(quiet, child_rval);
  if (child_rval) {
    ClearBaseFlag(CHILD_INVALID);
  } else {
    SetBaseFlag(CHILD_INVALID);
  }
  if (cp_flags != base_flags)
    DataChanged(DCR_ITEM_UPDATED);
  return (this_rval && child_rval);
}

void taBase::ClearCheckConfig() {
  if (base_flags & INVALID_MASK) {
    ClearBaseFlag(INVALID_MASK);
    DataChanged(DCR_ITEM_UPDATED);
  }
}

///////////////////////////////////////////////////////////////////////////
//	Copying and changing type 

bool taBase::CanDoCopy_impl(const taBase* cp, bool quiet, bool copy) {
  bool ok = true; // ref var needed for Check
  // do the generic guys, that apply in every case
  if (CheckError((!cp), quiet, ok,
    "Copy: source is null")) return false;
  if (cp->InheritsFrom(GetTypeDef())) {
    // we will be doing the copy
    CanCopy_impl(cp, quiet, ok, true);
    if (ok && copy) UnSafeCopy(cp);
  } else if (InheritsFrom(cp->GetTypeDef())) {
    // other guy will be doing it
    cp->CanCopy_impl(this, quiet, ok, true);
    if (ok && copy) UnSafeCopy(cp);
  } else { // custom
    bool to_cp = false; // rare case where src will copy
    bool allowed = false;
    bool forbidden = false;
    // check rare case: cp will copy into us -- it can assert forbidden to stop everything
    cp->CanCopyCustom_impl(true, this, quiet, allowed, forbidden);
    to_cp = (allowed && !forbidden);
    // check most common case: cp to us
    CanCopyCustom_impl(false, cp, quiet, allowed, forbidden);
    if (!(allowed && !forbidden)) ok = false;
    // only give our own error, if forbidden guy didn't
    if (!forbidden) CheckError((!allowed), quiet, ok,
       "Cannot copy from given object of type:", cp->GetTypeDef()->name,
       "which does not inherit from:", GetTypeDef()->name,
      "(or I don't inherit from it)");
    if (ok && copy) {
      if (to_cp) cp->CopyToCustom(this);
      else       CopyFromCustom(cp);
    }
  }
  return ok;
}

bool taBase::CanCopy(const taBase* cp, bool quiet) const {
   // harmless, because CanDoCopy is actually const on check
  taBase* ths = const_cast<taBase*>(this);
  return ths->CanDoCopy_impl(cp, quiet, false);
}

// this guy is always called with cp_fm !null, and our class or a subclass
void taBase::CanCopy_impl(const taBase* cpy_from, bool quiet,
  bool& ok, bool /*virt*/) const
{
/* if this were a subclass, we would add this code:
  if (virt) {
    inherited::CanCopy_(cpy_from, quiet, ok, virt);
    if (!ok) return; // no reason to continue, and could be bad to do so
  }
*/
  if (CheckError((cpy_from->IsParentOf(this)), quiet, ok,
    "Copy: cannot Copy from a parent")) return;
}


bool taBase::Copy(const taBase* cp) {
  return CanDoCopy_impl(cp, false, true);
}

bool taBase::CopyFrom(taBase* cpy_from) {
// this one is easy, since it is really just the same as Copy, but with warnings
  if (!CanCopy(cpy_from, false)) return false;
  UnSafeCopy(cpy_from);
  return true;
}

bool taBase::CopyTo(taBase* cpy_to) {
  if(TestError((!cpy_to), "CopyTo", "targetis null")) return false;
  return cpy_to->CopyFrom(this);
}

void taBase::CopyToCustom(taBase* src) const {
  src->StructUpdate(true);
  src->SetBaseFlag(COPYING); // note: this is always outer guy, so do it here
    CopyToCustom_impl(src);
  src->ClearBaseFlag(COPYING); // note: this is always outer guy, so do it here
  src->StructUpdate(false);
}

void taBase::CopyFromCustom(const taBase* cp_fm) {
  StructUpdate(true);
  SetBaseFlag(COPYING); // note: this is always outer guy, so do it here
    CopyFromCustom_impl(cp_fm);
  ClearBaseFlag(COPYING); // note: this is always outer guy, so do it here
  StructUpdate(false);
}

void taBase::Copy_impl(const taBase& cp) { // note: not a virtual method
  // just the flags
  base_flags = (BaseFlags)((base_flags & ~COPY_MASK) | (cp.base_flags & COPY_MASK));
}

bool taBase::ChildCanDuplicate(const taBase* chld, bool quiet) const {
  bool ok = true;
  if (CheckError((chld == NULL), quiet, ok,
    "Duplicate:", "no object provided!"))
     return false;
  const taList_impl* lst = children_();
  if (CheckError((!lst), quiet, ok,
    "Duplicate: cannot duplicate obj because owner is not a list/group!"))
     return false;
  return ok;
}

taBase* taBase::ChildDuplicate(const taBase* chld) {
  taList_impl* lst = children_();
  if (lst) {
    taBase* rval = lst->DuplicateEl(chld);
    if(rval && taMisc::gui_active) {
      if(!lst->HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL")) {
	tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
	tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
    }
    return rval;
  }
  return NULL;
}

bool taBase::DuplicateMe() {
  taBase* own = GetOwner();
  if (TestError((own == NULL), "DuplicateMe", "owner is null")) return false;
  if (!own->ChildCanDuplicate(this, false)) return false;
  return (own->ChildDuplicate(this));
}


#ifdef TA_GUI
static void tabase_base_closing_all_gp(taBase* obj) {
//  SelectEdit::BaseClosingAll(obj); // get it before it is moved around and stuff
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
	taBase* chld = (taBase*)gp->Leaf_(lf);
	if((chld != NULL) && (chld->GetOwner() == gp))
	  tabase_base_closing_all_gp(chld); // get it before it is moved around and stuff
      }
      continue;
    }
    taList_impl* gp = (taList_impl*)md->GetOff((void*)obj);
    int gi;
    for (gi=0;gi<gp->size;gi++) {
      taBase* chld = (taBase*)gp->FastEl_(gi);
      if((chld != NULL) && (chld->GetOwner() == gp))
	tabase_base_closing_all_gp(chld); // get it before it is moved around and stuff
    }
  }
}
#endif

bool taBase::ChangeMyType(TypeDef* new_type) {
  taBase* ownr = GetOwner();
  if(TestError(((new_type == NULL) || (ownr == NULL)), "ChangeMyType",
	       "new type or owner is NULL")) return false;
  if(TestError(!ownr->InheritsFrom(TA_taList_impl),
	       "ChangeMyType", "Cannot change my type because owner is not a list/group!"))
    return false;
  if(TestError((new_type == GetTypeDef()), "ChangeMyType",
	       "Object is already of selected type!")) return false;
#ifdef TA_GUI
  tabase_base_closing_all_gp(this);
#endif
  taList_impl* own = (taList_impl*)ownr;
  return own->ChangeType(this, new_type);
  // owner will used delayed remove to make this safe!
}

///////////////////////////////////////////////////////////////////////////
//	Type information

TypeDef* taBase::GetStemBase() const {
  TypeDef* rval = GetTypeDef()->GetStemBase();
  if(!rval) rval = &TA_taBase;
  return rval;
}


void* taBase::FindMembeR(const String& nm, MemberDef*& ret_md) const {
  ret_md = NULL;
  TypeDef* td = GetTypeDef();

  // first check directly in our members for member names
  MemberDef* md = td->members.FindName(nm);
  if(md) {
    ret_md = md;
    return md->GetOff((void*)this);
  }

  // then check for taBase items, checking object name and type (breadth first)
  for(int i=0; i < td->members.size; i++) {
    md = td->members[i];
    if(!md->type->InheritsFrom(TA_taBase) || md->HasOption("NO_FIND")) continue;
    taBase* mobj = (taBase*)md->GetOff((void*)this);
    if(mobj->FindCheck(nm) || md->type->InheritsFromName(nm)) {
      ret_md = md;
      return mobj;
    }
  }

  // then do a depth-recursive search
  for(int i=0; i < td->members.size; i++) {
    md = td->members[i];
    if(!md->type->InheritsFrom(TA_taBase) || md->HasOption("NO_FIND")) continue;
    taBase* mobj = (taBase*)md->GetOff((void*)this);
    void* rval = mobj->FindMembeR(nm, ret_md);
    if(rval)
      return rval;
  }
  return NULL;
}

///////////// Searching

void taBase::Search(const String& srch, taBase_PtrList& items,
		    taBase_PtrList* owners,
		    bool contains, bool case_sensitive,
		    bool obj_name, bool obj_type,
		    bool obj_desc, bool obj_val,
		    bool mbr_name, bool type_desc) {
  String srch_act = srch;
  if(!case_sensitive)
    srch_act.downcase();
  Search_impl(srch_act, items, owners, contains, case_sensitive, obj_name, obj_type,
	      obj_desc, obj_val, mbr_name, type_desc);
}


bool taBase::SearchTestStr_impl(const String& srch, String tst,
				bool contains, bool case_sensitive) {
  if(!case_sensitive) tst.downcase();
  if(contains) {
    if(tst.contains(srch)) return true;
  }
  else {
    if(tst == srch) return true;
  }
  return false;
}

bool taBase::SearchTestItem_impl(taBase* obj, const String& srch,
				 bool contains, bool case_sensitive,
				 bool obj_name, bool obj_type,
				 bool obj_desc, bool obj_val,
				 bool mbr_name, bool type_desc) {
  if(!obj) return false;
  if(obj_name) {
    if(SearchTestStr_impl(srch, obj->GetName(), contains, case_sensitive)) return true;
  }
  if(obj_type) {
    if(SearchTestStr_impl(srch, obj->GetTypeDef()->name, contains, case_sensitive)) return true;
    if(type_desc) {
      if(SearchTestStr_impl(srch, obj->GetTypeDef()->desc, contains, case_sensitive)) return true;
    }
  }
  if(obj_desc) {
    if(SearchTestStr_impl(srch, obj->GetDesc(), contains, case_sensitive)) return true;
  }
  if(obj_val) {
    if(SearchTestStr_impl(srch, obj->GetDisplayName(), contains, case_sensitive)) return true;
    String strval = GetTypeDef()->GetValStr(obj, NULL, NULL, TypeDef::SC_DEFAULT, true);
    // true = force_inline
    if(SearchTestStr_impl(srch, strval, contains, case_sensitive)) return true;
  }

  if(mbr_name) {
    TypeDef* td = obj->GetTypeDef();
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if(SearchTestStr_impl(srch, md->name, contains, case_sensitive)) return true;
      if(type_desc) {
	if(SearchTestStr_impl(srch, md->desc, contains, case_sensitive)) return true;
      }
    }
  }
  return false;
}

void taBase::Search_impl(const String& srch, taBase_PtrList& items,
			 taBase_PtrList* owners,
			 bool contains, bool case_sensitive,
			 bool obj_name, bool obj_type,
			 bool obj_desc, bool obj_val,
			 bool mbr_name, bool type_desc) {
  TypeDef* td = GetTypeDef();
  int st_sz = items.size;
  // first pass: just look at our guys
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	if(mbr_name) {
	  if(SearchTestStr_impl(srch, md->name, contains, case_sensitive)) {
	    items.Link(obj);
	    continue;
	  }
	  else if(type_desc) {
	    if(SearchTestStr_impl(srch, md->desc, contains, case_sensitive)) {
	      items.Link(obj);
	      continue;
	    }
	  }
	}
	if(SearchTestItem_impl(obj, srch, contains, case_sensitive, obj_name, obj_type,
			       obj_desc, obj_val, mbr_name, type_desc)) {
	  items.Link(obj);
	}
      }
    }
  }
  // second pass: recurse
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	obj->Search_impl(srch, items, owners,contains, case_sensitive, obj_name, obj_type,
			 obj_desc, obj_val, mbr_name, type_desc);
      }
    }
  }
  if(owners && (items.size > st_sz)) { // we added somebody somewhere..
    owners->Link(this);
  }
}

void taBase::CompareSameTypeR(Member_List& mds, TypeSpace& base_types, 
			      voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
			      taBase* cp_base, int show_forbidden,
			      int show_allowed, bool no_ptrs) {
  if(!cp_base) return;
  TypeDef* td = GetTypeDef();
  if(td != cp_base->GetTypeDef()) return; // must be same type..
  // search our guy:
  td->CompareSameType(mds, base_types, trg_bases, src_bases, (void*)this, (void*)cp_base,
		      show_forbidden, show_allowed, no_ptrs);
  // then recurse..
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->type->ptr > 0 || !md->type->InheritsFrom(TA_taBase)) continue;
    if(md->type->HasOption("EDIT_INLINE") || md->type->HasOption("INLINE")) continue;
    if(md->HasOption("HIDDEN")) continue; // categorically don't look at hidden objects for diffs
    taBase* obj = (taBase*)md->GetOff(this);
    taBase* cp_obj = (taBase*)md->GetOff(cp_base);
    obj->CompareSameTypeR(mds, base_types, trg_bases, src_bases,
			  cp_obj, show_forbidden, show_allowed, no_ptrs);
  }
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
    else if(td->DerivesFormal(TA_enum)) {
      return VT_STRING; 
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

///////////////////////////////////////////////////////////////////////////
//	Printing out object state values

String taBase::GetStringRep(taBase* it) {
  if (it == NULL) 
    return String("NULL", 4);
  else
    return it->GetStringRep_impl();
}

String taBase::GetStringRep_impl() const {
  String rval = GetTypeDef()->name + ":" + GetPath_Long();
  return rval;
}

///////////////////////////////////////////////////////////////////////////
//	User Data: optional configuration settings for objects

bool taBase::HasUserData(const String& key) const {
  UserDataItem_List* ud = GetUserDataList();
  if (ud)
    return (ud->FindLeafName(key));
  
  return false;
}

const Variant taBase::GetUserData(const String& key) const {
  UserDataItem_List* ud = GetUserDataList();
  if (ud) {
    UserDataItemBase* udi = ud->FindLeafName(key);
    if (udi) return udi->valueAsVariant();
  }
  return _nilVariant;
}

UserDataItemBase* taBase::GetUserDataItem(const String& key) const {
  UserDataItemBase* rval = NULL;
  UserDataItem_List* ud = GetUserDataList();
  if (ud) {
    rval = ud->FindLeafName(key);
  }
  return rval;
}


UserDataItemBase* taBase::GetUserDataOfType(TypeDef* typ,
  const String& key, bool force_create) 
{
  if (!typ) return NULL;
  typ = typ->GetNonPtrType();
  // need to verify, in case we have to create
  if (!typ->InheritsFrom(&TA_UserDataItemBase)) return NULL;
  UserDataItem_List* ud = GetUserDataList(force_create);
  if (!ud) return NULL;
  UserDataItemBase* rval = NULL; 
  for (int i = 0; i < ud->leaves; ++i) {
    UserDataItemBase* udi = ud->Leaf(i);
    if (udi->name != key) continue;
    if (udi->InheritsFrom(typ)) {
      rval = udi;
      break;
    }
  }
  if (!rval && force_create) {
    rval = (UserDataItemBase*)ud->New(1, typ);
    rval->name = key;
    DataChanged(DCR_USER_DATA_UPDATED);
  }
  return rval;
}

UserDataItemBase* taBase::GetUserDataOfTypeC(TypeDef* typ,
  const String& key) const
{
  return const_cast<taBase*>(this)->GetUserDataOfType(typ, key, false);
}

taDoc* taBase::GetDocLink() const {
  UserData_DocLink* uddl = (UserData_DocLink*)GetUserDataOfTypeC(
    &TA_UserData_DocLink, "DocLink");
  if (!uddl) return NULL;
  return uddl->doc;
}

bool taBase::RemoveUserData(const String& key) {
  UserDataItem_List* ud = GetUserDataList();
  if (ud) {
    UserDataItemBase* udi = ud->FindLeafName(key);
    if (udi) {
      udi->Close();
      DataChanged(DCR_USER_DATA_UPDATED);
      return true;
    }
  }
  return false;
}

void taBase::SetDocLink(taDoc* doc) {
  // only force if setting a doc
  UserData_DocLink* uddl = (UserData_DocLink*)GetUserDataOfType(
    &TA_UserData_DocLink, "DocLink", (doc != NULL));
  if (TestError((doc && !uddl), "SetDocLink", "Could not set DocLink -- the object may not support UserData")) return;
  if (!doc && !uddl) return; // not setting, doesn't already have
  if (doc) {
    if(TestError(!doc->GetOwner(), "SetDocLink", "doc does not have an owner -- invalid:",
		 doc->name))
      return;
    if(TestError(!SameScope(doc, &TA_taProject), "SetDocLink",
		 "doc is not in the same project as this object -- invalid:",
		 doc->name))
      return;
    uddl->doc = doc;
  } else {
    uddl->Close(); // do now, so update is good
  }
  DataChanged(DCR_USER_DATA_UPDATED);
}

void taBase::PrintMyPath() {
  taMisc::Info(GetPath());
}

UserDataItem* taBase::SetUserData(const String& name, const Variant& value)
{
  if (TestError((name.empty()),
    "SetUserData",
    "name must be a valid name")) {
    return NULL;
  }
  
  UserDataItem_List* ud = GetUserDataList(true);
  if (!ud) return NULL; // not supported, shouldn't be calling
  
  bool notify = true; // always!
  UserDataItemBase* udi = ud->FindLeafName(name);
  if (udi) {
    if (TestError(!udi->isSimple(),
      "SetUserData",
      "non-simple UserData item with that name already exists!")) {
      return NULL;
    }
  } else {
    udi = new UserDataItem;
    udi->SetName(name);
    ud->Add(udi);
  }
  TestWarning(!udi->setValueAsVariant(value),"SetUserData",
	      "Attempt to set existing UserData value as Variant, was not supported for", name);
  if (notify) DataChanged(DCR_USER_DATA_UPDATED);
  return dynamic_cast<UserDataItem*>(udi); // should succeed!
}

void taBase::SetUserData_Gui(const String& key, const Variant& value,
    const String& desc)
{
  UserDataItem* udi = SetUserData(key, value);
  if (udi && desc.nonempty()) 
    udi->SetDesc(desc);
}

///////////////////////////////////////////////////////////////////////////
// 	Browser gui

const KeyString taBase::key_name("name");
const KeyString taBase::key_type("type");
const KeyString taBase::key_type_desc("type_desc");
const KeyString taBase::key_desc("desc"); 
const KeyString taBase::key_disp_name("disp_name"); 
const KeyString taBase::key_unique_name("unique_name"); 

const String taBase::statusTip(const KeyString&) const {
  TypeDef* typ = GetTypeDef();
  String rval = GetName() + " (" + typ->name + "): " + typ->desc;
  return rval;
}

const String taBase::GetToolTip(const KeyString& key) const {
  // the default just returns the same text as for the col
  return GetColText(key);
}

String taBase::GetColText(const KeyString& key, int /*itm_idx*/) const {
       if (key == key_name) return GetName();
  else if (key == key_type) return GetTypeName();
  else if (key == key_type_desc) return GetTypeDef()->desc;
// note: some classes override desc with dynamic desc's
  else if (key == key_desc) return GetDesc(); 
  else if (key == key_disp_name) return GetDisplayName(); 
  else return _nilString;
}

const QVariant taBase::GetColData(const KeyString& key, int role) const {
// these are just the defaults -- later guys can override to trap anything
       if (role == Qt::StatusTipRole) return statusTip(key);
  else if (role == Qt::ToolTipRole) return GetToolTip(key);
  else return QVariant();
}

void taBase::BrowseMe() {
  // try to determine whether this is member obj, or not
  taBase* own = GetOwner();
  MemberDef* md = NULL;
  if (own) {
    md = own->FindMember((void*)this);
  }
  MainWindowViewer* wv = MainWindowViewer::NewBrowser(this, md);
  if (wv) wv->ViewWindow();
}

///////////////////////////////////////////////////////////////////////////
//	Edit Dialog gui

bool taBase::Edit() {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  taiEdit* ie;
  if((ie = GetTypeDef()->ie) != NULL) {
    //note: taiEdit looks up color, if hinting enabled
    return ie->Edit((void*)this, false);
  }
#endif
  return false;
}

bool taBase::EditDialog(bool modal) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (modal) {
    taiEdit* ie;
    if ((ie = GetTypeDef()->ie)) {
      //note: taiEdit looks up color, if hinting enabled
      return ie->EditDialog((void*)this, false, true); // r/w, modal
    }
  } else {

    // first, check for an edit dialog and use that if found
    MainWindowViewer* edlg = MainWindowViewer::FindEditDialog(this);
    if(edlg) {
      edlg->Show();		// focus on it
      return true;
    }

    edlg = MainWindowViewer::NewEditDialog(this);
    if (edlg) {
      edlg->ViewWindow();
      iMainWindowViewer* iwv = edlg->widget();
      iwv->resize( taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgVer) );
      return iwv->AssertPanel((taiDataLink*)GetDataLink());
        //bool new_tab, bool new_tab_lock)
    }
  }
#endif
  return false;
}

#ifndef TA_GUI
// see ta_qtviewer.cpp
bool taBase::EditPanel(bool new_tab, bool pin_tab) {
  return false;
}
bool taBase::BrowserSelectMe() {
  return false;
}
bool taBase::BrowserExpandAll() {
  return false;
}
bool taBase::BrowserCollapseAll() {
  return false;
}
bool taBase::GuiFindFromMe(const String& find_str) {
  return false;
}
#endif

/*obs bool taBase::ReShowEdit(bool force) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  return taiMisc::ReShowEdits((void*)this, GetTypeDef(), force);
#endif
  return false;
}*/

const iColor taBase::GetEditColor(bool& ok) {
  String dec_key = GetTypeDecoKey(); // nil if none
  if (dec_key.nonempty()) {
    ViewColor* vc = taMisc::view_colors->FindName(dec_key);
    if(vc) {
      ok = true;
      if(vc->use_bg)
	return vc->bg_color.color(); // prefer bg color if specified; else use fg
      else if(vc->use_fg)
	return vc->fg_color.color();
    }
  }
  ok = false; 
  return iColor();
}

const iColor taBase::GetEditColorInherit(bool& ok) {
  iColor bgclr = GetEditColor(ok);
  if (!ok) {
    taBase* ownr = GetOwner();
    while ((ownr != NULL) && (!ok)) {
      bgclr = ownr->GetEditColor(ok);
      ownr = ownr->GetOwner();
    }
  }
  return bgclr;
}

void taBase::CallFun(const String& fun_name) {
#ifdef TA_GUI
  if(!taMisc::gui_active) return;
#endif
  MethodDef* md = GetTypeDef()->methods.FindName(fun_name);
  if(md != NULL)
    md->CallFun((void*)this);
  else
    TestWarning(true, "CallFun", "function:", fun_name, "not found on object");
}

Variant taBase::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "ChangeMyType") return _nilVariant;
  return Variant(GetStemBase()->name); // taiTypePtrArgType will convert from String
}

String taBase::DiffCompareString(taBase* cmp_obj, taDoc*& doc) {
  if(TestError(!cmp_obj, "DiffCompareString", "cmp_obj is null")) return _nilString;
  if(!doc) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "DiffCompare", "cannot find project")) return _nilString;
    doc = (taDoc*)proj->docs.New(1);
    doc->name = "DiffCompare_" + GetDisplayName() + "_" + cmp_obj->GetDisplayName();
  }
  String str_a, str_b;
  taStringDiff diff;
  
  Save_String(str_a);
  cmp_obj->Save_String(str_b);
  diff.DiffStrings(str_a, str_b);
  String rval = diff.GetDiffStr(str_a, str_b);
  String html_safe = rval;
  html_safe.xml_esc();
  doc->text = "<html>\n<head></head>\n<body>\n== DiffCompare of: "
    + GetDisplayName() + " and: " + cmp_obj->GetDisplayName() + " ==\n<pre>\n"
    + html_safe + "\n</pre>\n</body>\n</html>\n";
  doc->UpdateText();
  tabMisc::DelayedFunCall_gui(doc, "BrowserSelectMe");
  return rval;
}

bool taBase::DiffCompare(taBase* cmp_obj) {
  if(TestError(!cmp_obj, "DiffCompareString", "cmp_obj is null")) return false;
  taObjDiff_List odl_me;
  taObjDiff_List odl_cmp;
  
  odl_me.tab_obj_a = this;
  odl_cmp.tab_obj_a = cmp_obj;

  GetObjDiffVal(odl_me, 0);
  cmp_obj->GetObjDiffVal(odl_cmp, 0);

  taObjDiff_List diffs;
  diffs.tab_obj_a = this;
  diffs.tab_obj_b = cmp_obj;
  odl_me.Diff(diffs, odl_cmp);

  taiObjDiffBrowser* odb = taiObjDiffBrowser::New(diffs, taiMisc::defFontSize);
  bool rval = odb->Browse();

  if(rval) {
    DoDiffEdits(diffs);
  }

  return true;
}

static void DoDiffEdits_SetRelPath(taBase* par_obj, taObjDiffRec* srec, taObjDiffRec* drec) {
  MemberDef* md;
  taBase* new_guy = par_obj->FindFromPath(srec->value, md);
  if((drec->type->ptr == 1) && drec->type->DerivesFrom(&TA_taBase)) {
    if(drec->mdef && drec->mdef->HasOption("OWN_POINTER")) {
      if(!drec->par_addr)
	taMisc::Warning("*** NULL parent for owned pointer:",drec->GetDisplayName());
      else
	taBase::OwnPointer((taBase**)drec->addr, new_guy, (taBase*)drec->par_addr);
    }
    else {
      if(drec->mdef && drec->mdef->HasOption("NO_SET_POINTER"))
	(*(taBase**)drec->addr) = new_guy;
      else
	taBase::SetPointer((taBase**)drec->addr, new_guy);
    }
  }
  else if(drec->type->InheritsFrom(TA_taSmartRef)) {
    ((taSmartRef*)drec->addr)->set(new_guy);
  }
  else if(drec->type->InheritsFrom(TA_taSmartPtr)) {
    ((taSmartPtr*)drec->addr)->set(new_guy);
  }
}

bool taBase::DoDiffEdits(taObjDiff_List& diffs) {
  StructUpdate(true);

  taProject* proj_a = (taProject*)diffs.tab_obj_a->GetOwner(&TA_taProject);
  taProject* proj_b = (taProject*)diffs.tab_obj_b->GetOwner(&TA_taProject);

  for(int i=0; i<diffs.size; i++) {
    taObjDiffRec* rec = diffs[i];
    if(!rec->HasDiffFlag(taObjDiffRec::ACT_MASK)) continue;
    if(!rec->addr || !rec->type) continue; // sanity checks..

    bool ta_bases = false;
    bool tab_diff_typ = false;
    bool taptr = false;
    taBase* tab_a = NULL;
    taBase* tab_b = NULL;;
    // make sure pointers are still current
    if(rec->type->InheritsFrom(&TA_taBase)) {
      if(rec->extra) {
	if(!((taBaseObjDiffRecExtra*)rec->extra)->tabref.ptr())
	  continue;
	tab_a = (taBase*)rec->addr;
      }
      if(rec->diff_odr) {
	if(rec->diff_odr->extra) {
	  if(!((taBaseObjDiffRecExtra*)rec->diff_odr->extra)->tabref.ptr())
	    continue;
	  tab_b = (taBase*)rec->diff_odr->addr;
	}
      }
      if(tab_a && tab_b)
	ta_bases = true;
      if((!rec->mdef && (rec->type != rec->diff_odr->type))) {
	if(!ta_bases) continue; // no can do
	tab_diff_typ = true;
      }
    }
    else if(((rec->type->ptr == 1) && rec->type->DerivesFrom(&TA_taBase)) ||
	    rec->type->InheritsFrom(TA_taSmartRef) ||
	    rec->type->InheritsFrom(TA_taSmartPtr)) {
      taptr = true;
    }

    taBase* tab_par_a = NULL;
    taBase* tab_par_b = NULL;
    if(rec->par_type && rec->par_type->InheritsFrom(&TA_taBase)) {
      // make sure *parent* pointer is still current
      if(rec->par_odr && rec->par_odr->extra) {
	if(!((taBaseObjDiffRecExtra*)rec->par_odr->extra)->tabref.ptr())
	  continue;
	tab_par_a = (taBase*)rec->par_addr;
      }
    }
    if(rec->diff_odr && rec->diff_odr->par_type && 
       rec->diff_odr->par_type->InheritsFrom(&TA_taBase)) {
      // make sure *parent* pointer is still current
      if(rec->diff_odr->par_odr && rec->diff_odr->par_odr->extra) {
	if(!((taBaseObjDiffRecExtra*)rec->diff_odr->par_odr->extra)->tabref.ptr())
	  continue;
	tab_par_b = (taBase*)rec->diff_odr->par_addr;
      }
    }

    //////////////////////////////////
    //		Copy -- both ways -- uses value string instead of live obj

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB) &&
       rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      // this is unfortunate but possible 
      taMisc::Info("Copying A -> B:", rec->diff_odr->GetDisplayName(), "=", rec->value);
      taMisc::Info("Copying B -> A:", rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
 	// need to replace old guy with new one
	taBase* down = tab_b->GetOwner();
	if(down) {		// should always be true
	  down->CopyChildBefore(tab_a, tab_b);
	}

	taBase* sown = tab_a->GetOwner();
	if(sown) {		// should always be true
	  sown->CopyChildBefore(tab_b, tab_a);
	}

	tab_b->Close();	// nuke old guys
	tab_a->Close();	// nuke old guys
      }
      else {
	if(rec->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
	  DoDiffEdits_SetRelPath(diffs.tab_obj_b, rec, rec->diff_odr);
	}
	else if(taptr) {
	  DoDiffEdits_SetRelPath(proj_b, rec, rec->diff_odr); // always project relative
	}
	else {
	  rec->diff_odr->type->SetValStr(rec->value, rec->diff_odr->addr,
					 rec->diff_odr->par_addr, rec->diff_odr->mdef);
	}
	if(tab_par_b) tab_par_b->UpdateAfterEdit();

	if(rec->diff_odr->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
	  DoDiffEdits_SetRelPath(diffs.tab_obj_a, rec->diff_odr, rec);
	}
	else if(taptr) {
	  DoDiffEdits_SetRelPath(proj_a, rec->diff_odr, rec); // always project relative
	}
	else {
	  rec->type->SetValStr(rec->diff_odr->value, rec->addr, rec->par_addr, rec->mdef);
	}
	if(tab_par_a) tab_par_a->UpdateAfterEdit();
	
      }
      continue;
    }

    //////////////////////////////////
    //		Copy -- only one way

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB)) {
      taMisc::Info("Copying A -> B:", rec->diff_odr->GetDisplayName(), "=", rec->value);
      if(tab_diff_typ) {
 	// need to replace old guy with new one
	taBase* down = tab_b->GetOwner();
	if(down) {
	  down->CopyChildBefore(tab_a, tab_b);
	  tab_b->Close();	// nuke old guy
	}
      }
      else {
	if(rec->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
	  DoDiffEdits_SetRelPath(diffs.tab_obj_b, rec, rec->diff_odr);
	}
	else if(taptr) {
	  DoDiffEdits_SetRelPath(proj_b, rec, rec->diff_odr); // always project relative
	}
	else {
	  rec->diff_odr->type->CopyFromSameType(rec->diff_odr->addr, rec->addr);
	  if(ta_bases) {
	    tab_b->SetName(tab_a->GetName()); // need to copy names too!
	  }
	}
	if(tab_par_b) tab_par_b->UpdateAfterEdit();
      }
      continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      taMisc::Info("Copying B -> A:", rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
 	// need to replace old guy with new one
	taBase* down = tab_a->GetOwner();
	if(down) {
	  down->CopyChildBefore(tab_b, tab_a);
	  tab_a->Close();	// nuke old guy
	}
      }
      else {
	if(rec->diff_odr->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
	  DoDiffEdits_SetRelPath(diffs.tab_obj_a, rec->diff_odr, rec);
	}
	else if(taptr) {
	  DoDiffEdits_SetRelPath(proj_a, rec->diff_odr, rec); // always project relative
	}
	else {
	  rec->type->CopyFromSameType(rec->addr, rec->diff_odr->addr);
	  if(ta_bases) {
	    tab_a->SetName(tab_b->GetName()); // need to copy names too!
	  }
	}
	if(tab_par_a) tab_par_a->UpdateAfterEdit();
      }
      continue;
    }
    
    if(!ta_bases) continue;	// only ta bases from this point on!

    //////////////////////////////////
    //		Add
    bool add = false;
    bool added = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_A)) { // do add before del..
      taMisc::Info("Adding A to B:", rec->GetDisplayName());
      add = true;
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_B)) { // do add before del..
      taMisc::Info("Adding B to A:", rec->GetDisplayName());
      add = true;
    }
    if(add) {
      if(rec->diff_odr->nest_level < rec->nest_level) {
	// for last obj in list, dest is now another member in parent obj..
// 	taMisc::Info("diff nest -- rec:", String(rec->nest_level), "diff:",
// 		     String(rec->diff_odr->nest_level),"rec path:", tab_a->GetPath(),
// 		     "diff path:", tab_b->GetPath());
	if(tab_par_b) {
	  if(rec->par_odr->mdef) {
	    // find member in dest par (parents always ta base..)
	    if(rec->diff_odr->par_odr->mdef &&
	       rec->diff_odr->par_odr->mdef->name == rec->par_odr->mdef->name) {
	      // parent is the guy!
	      tab_par_b->CopyChildBefore(tab_a, NULL); // NULL goes to end..
	      added = true;
	    }
	    else {
	      MemberDef* dmd;
	      void* mbase = tab_par_b->FindMembeR(rec->par_odr->mdef->name, dmd);
	      if(dmd && dmd->type->InheritsFrom(&TA_taBase)) { // it should!
		taBase* down = (taBase*)mbase;
		down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
		added = true;
	      }
	    }
	  }
	  else { // go one level higher
	    taObjDiffRec* parpar_a = rec->par_odr->par_odr;
	    if(parpar_a->mdef) {
	      taBase* tabparpar_b = tab_par_b->GetOwner();
	      // find member in dest par (parents always ta base..)
	      MemberDef* dmd;
	      void* mbase = tabparpar_b->FindMembeR(parpar_a->mdef->name, dmd);
	      if(dmd && dmd->type->InheritsFrom(&TA_taBase)) { // it should!
		taBase* down = (taBase*)mbase;
		down->CopyChildBefore(tab_a, NULL); // NULL goes to end..
		added = true;
	      }
	    }
	  }
	}
      }
      else if(rec->diff_odr->nest_level > rec->nest_level) {
	if(tab_par_b) {
	  taBase* down = tab_par_b;
	  for(int k=0;k < rec->diff_odr->nest_level - rec->nest_level;k++) {
	    down = down->GetOwner();
	    if(!down) break;
	  }
	  if(down) {
	    down->CopyChildBefore(tab_a, NULL); // no ref info avail -- probably end
	    added = true;
	  }
	}
      }
      else {			// equal nesting!
	taBase* down = tab_b->GetOwner();
	if(down) {
	  down->CopyChildBefore(tab_a, tab_b);
	  added = true;
	}
      }
    }
    if(!added) {
      taMisc::Warning("NOTE: Last Add was NOT successful!", rec->GetDisplayName());
    }

    //////////////////////////////////
    //		Del

    if(rec->extra && !((taBaseObjDiffRecExtra*)rec->extra)->tabref.ptr()) continue;
    // double-check

    bool del = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_A)) {
      taMisc::Info("Deleting A:", rec->GetDisplayName());
      del = true;
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_B)) {
      taMisc::Info("Deleting B:", rec->GetDisplayName());
      del = true;
    }

    if(del) {
      tab_a->Close();
    }

  }
  StructUpdate(false);
  return true;
}


bool taBase::SelectForEdit(MemberDef* member, SelectEdit* editor, const String& extra_label,
			   const String& sub_gp_nm) {
  if(TestError(!member,"SelectForEdit", "member is null")) return false;
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectForEdit", "cannot find project")) return false;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  return editor->SelectMember(this, member, extra_label, "", sub_gp_nm);
}

bool taBase::SelectForEditNm(const String& member, SelectEdit* editor,
			     const String& extra_label, const String& sub_gp_nm,
			     const String& desc) {
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);  StructUpdate(true);

    if(TestError(!proj, "SelectForEditNm", "cannot find project")) return false;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  return editor->SelectMemberNm(this, member, extra_label, desc, sub_gp_nm);
}

int taBase::SelectForEditSearch(const String& memb_contains, SelectEdit*& editor) {
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectForEditSearch", "cannot find project")) return -1;
    editor = (SelectEdit*)proj->edits.New(1);
    editor->name = "Srch_" + memb_contains;
    editor->desc = "Search of members containing: " + memb_contains 
      + " in object: " + GetDisplayName();
    editor->DataChanged(DCR_ITEM_UPDATED); // so name updates in treee
  }
  TypeDef* td = GetTypeDef();
  int nfound = 0;
  // look for guys on me
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->name.contains(memb_contains)) {
      nfound++;
      editor->SelectMember(this, md, GetName());
    }
  }
  // then look in my sub-guys
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nfound += obj->SelectForEditSearch(memb_contains, editor);
      }
    }
  }
  return nfound;
}

int taBase::SelectForEditCompare(taBase*cmp_obj, SelectEdit*& editor, bool no_ptrs) {
  if(TestError(!cmp_obj, "SelectForEditCompare", "cmp_obj is null")) return -1;
  if(TestError(GetTypeDef() != cmp_obj->GetTypeDef(), "SelectForEditCompare", 
	       "objects must have the exact same type to be able to be compared")) return -1;
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectForEditCompare", "cannot find project")) return -1;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  int rval = editor->CompareObjs(this, cmp_obj, no_ptrs);
  tabMisc::DelayedFunCall_gui(editor, "BrowserSelectMe");
  return rval;
}

bool taBase::SelectFunForEdit(MethodDef* function, SelectEdit* editor,
			      const String& extra_label, const String& sub_gp_nm) {
  if(TestError(!function, "SelectFunForEdit", "function is null")) return false;
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectFunForEdit", "cannot find project")) return false;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  return editor->SelectMethod(this, function, extra_label, "", sub_gp_nm);
}

bool taBase::SelectFunForEditNm(const String& function, SelectEdit* editor,
				const String& extra_label, const String& sub_gp_nm,
				const String& desc) {
  if(!editor) {
    taProject* proj = GET_MY_OWNER(taProject);
    if(TestError(!proj, "SelectFunForEditNm", "cannot find project")) return false;
    editor = (SelectEdit*)proj->edits.New(1);
  }
  return editor->SelectMethodNm(this, function, extra_label, desc, sub_gp_nm);
}

///////////////////////////////////////////////////////////////////////////
//	Closing 

void taBase::Close() {
  if(isDestroying()) return;
  taBase* own = GetOwner();
  if (own && own->Close_Child(this))
    return;
  taBase::UnRef(this);
}

void taBase::CloseLater() {
  if(isDestroying()) return;
  taBase* own = GetOwner();
  if (own && own->CloseLater_Child(this))
    return;
  tabMisc::DelayedClose(this);
}

bool taBase::Close_Child(taBase*) {
  return false;
}

bool taBase::CloseLater_Child(taBase*) {
  return false;
}

void taBase::Help() {
  TypeDef* mytd = GetTypeDef();
#ifdef TA_GUI
  iHelpBrowser::StatLoadType(mytd);
#endif
/*legacy... 
  taProject* proj = GET_MY_OWNER(taProject);
  if(!proj) return;
  taDoc* help_doc = proj->FindMakeDoc("HelpDoc", taMisc::web_help_wiki, mytd->name);
  help_doc->EditDialog();*/
}

// old help 
//   String full_file;
//   while((mytd != NULL) && full_file.empty()) {
//     String help_file = taMisc::help_file_tmplt;
//     help_file.gsub("%t", mytd->name);
//     full_file = taMisc::FindFileOnLoadPath(help_file);
//     mytd = mytd->parents.SafeEl(0);	// go with the parent
//   }
//   if(TestError(full_file.empty(), "Help",
// 	       "Sorry, no help available")) return;
//   String help_cmd = taMisc::help_cmd;
//   help_cmd.gsub("%s", full_file);
//   int rval = system(help_cmd);	// rval is compiler food
//}

///////////////////////////////////////////////////////////////////////////
//	Updating pointers (when objects change type or are copied)

taBase* taBase::UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par, taBase* new_par) {
  String old_path = old_guy->GetPath(NULL, old_par);
  MemberDef* md;
  taBase* new_guy = new_par->FindFromPath(old_path, md);
  String old_nm = old_guy->GetName();
  if(old_guy->GetOwner() && (!new_guy ||
			     (old_nm != new_guy->GetName()))) {
    // path-based guy is NULL or not right -- try to lookup by name
    String old_own_path = old_guy->GetOwner()->GetPath(NULL, old_par);
    taBase* new_own = new_par->FindFromPath(old_own_path, md);
    if(new_own && new_own->InheritsFrom(&TA_taList_impl)) {
      taList_impl* lst = (taList_impl*)new_own;
      new_guy = (taBase*)lst->FindName_(old_nm);
    }
  }
  return new_guy;
}

bool taBase::UpdatePointers_NewPar_Ptr(taBase** ptr, taBase* old_par, taBase* new_par,
				 bool null_not_found) {
  if(!*ptr || !old_par || !new_par) return false;
  if(*ptr == old_par) {
    taBase::SetPointer(ptr, new_par);
    return true;
  }
  taBase* old_own = (*ptr)->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false; // if not scoped in our guy, bail
  taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_par, new_par);
  if(new_guy) {
    taBase::SetPointer(ptr, new_guy);
  }
  else {
    if(null_not_found)
      taBase::SetPointer(ptr, NULL);
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

bool taBase::UpdatePointers_NewPar_PtrNoSet(taBase** ptr, taBase* old_par, taBase* new_par,
					    bool null_not_found) {
  if(!*ptr || !old_par || !new_par) return false;
  if(*ptr == old_par) {
    *ptr = new_par;
    return true;
  }
  taBase* old_own = (*ptr)->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false; // if not scoped in our guy, bail
  taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_par, new_par);
  if(new_guy)
    *ptr = new_guy;
  else {
    if(null_not_found)
      *ptr = NULL;
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

bool taBase::UpdatePointers_NewPar_SmPtr(taSmartPtr& ref, taBase* old_par, taBase* new_par,
					 bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) return false;
  if(ref.ptr() == old_par) {
    ref.set(new_par);
    return true;
  }
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false; // if not scoped in our guy, bail
  taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
  if(new_guy)
    ref.set(new_guy);
  else {
    if(null_not_found)
      ref.set(NULL);		// reset to null if not found!
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

bool taBase::UpdatePointers_NewPar_Ref(taSmartRef& ref, taBase* old_par, taBase* new_par,
				 bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) return false;
  if(ref.ptr() == old_par) {
    ref.set(new_par);
    return true;
  }
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) return false; // if not scoped in our guy, bail
  taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
  if(new_guy)
    ref.set(new_guy);
  else {
    if(null_not_found)
      ref.set(NULL);		// reset to null if not found!
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

int taBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;			// total number changed
  int mychg = 0;		// my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->is_static) continue;
    if((md->type->ptr == 1) && md->type->DerivesFrom(TA_taBase) &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
// #ifdef DEBUG
//       cerr << "Non-RO raw pointer member: " << md->name << " in type: " << td->name
// 	   << " updating pointer -- should be a ref!" << endl;
//       taMisc::FlushConsole();
// #endif
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasOption("NO_SET_POINTER")) {
	int chg = UpdatePointers_NewPar_PtrNoSet(ptr, old_par, new_par);
	nchg += chg; mychg += chg;
      }
      else {
	int chg = UpdatePointers_NewPar_Ptr(ptr, old_par, new_par);
	nchg += chg; mychg += chg;
      }
    }
    else if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taSmartRef)) {
	taSmartRef* ref = (taSmartRef*)md->GetOff(this);
	int chg = UpdatePointers_NewPar_Ref(*ref, old_par, new_par);
	nchg += chg; mychg += chg;
      }
      if(md->type->InheritsFrom(TA_taSmartPtr)) {
	taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
	int chg = UpdatePointers_NewPar_SmPtr(*ref, old_par, new_par);
	nchg += chg; mychg += chg;
      }
      else if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nchg += obj->UpdatePointers_NewPar(old_par, new_par); // doesn't count for me
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();		// uae me: i changed
  return nchg;
}

int taBase::UpdatePointers_NewPar_IfParNotCp(const taBase* cp, TypeDef* par_type) {
  taBase* mypar = GetOwner(par_type);
  if(mypar && !mypar->HasBaseFlag(COPYING)) { // only if my guy is not already doing it
    taBase* cppar = cp->GetOwner(par_type);
    if(mypar != cppar) {
      return UpdatePointers_NewPar(cppar, mypar); // update any pointers within this guy
    }
  }
  return -1;
}

///////////////////

bool taBase::UpdatePointers_NewParType_Ptr(taBase** ptr, TypeDef* par_typ, taBase* new_par,
					   bool null_not_found) {
  if(!*ptr || !new_par) return false;
  if((*ptr)->InheritsFrom(par_typ)) {
    taBase::SetPointer(ptr, new_par);
    return true;
  }
  taBase* old_par = (*ptr)->GetOwner(par_typ);
  if(!old_par) return false;
  taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_par, new_par);
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

bool taBase::UpdatePointers_NewParType_PtrNoSet(taBase** ptr, TypeDef* par_typ, taBase* new_par,
					   bool null_not_found) {
  if(!*ptr || !new_par) return false;
  if((*ptr)->InheritsFrom(par_typ)) {
    *ptr = new_par;
    return true;
  }
  taBase* old_par = (*ptr)->GetOwner(par_typ);
  if(!old_par) return false;
  taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_par, new_par);
  if(new_guy)
    *ptr = new_guy;
  else {
    if(null_not_found)
      *ptr = NULL;
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

bool taBase::UpdatePointers_NewParType_SmPtr(taSmartPtr& ref, TypeDef* par_typ,
					     taBase* new_par, bool null_not_found) {
  if(!ref.ptr() || !new_par) return false;
  if(ref.ptr()->InheritsFrom(par_typ)) {
    ref.set(new_par);
    return true;
  }
  taBase* old_par = ref.ptr()->GetOwner(par_typ);
  if(!old_par) return false;
  taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
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

bool taBase::UpdatePointers_NewParType_Ref(taSmartRef& ref, TypeDef* par_typ, taBase* new_par,
				       bool null_not_found) {
  if(!ref.ptr() || !new_par) return false;
  if(ref.ptr()->InheritsFrom(par_typ)) {
    ref.set(new_par);
    return true;
  }
  taBase* old_par = ref.ptr()->GetOwner(par_typ);
  if(!old_par) return false;
  taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
  if(new_guy)
    ref.set(new_guy);
  else {
    //    cerr << "update ptrs: old_path " << old_path << " not found " << endl;
    if(null_not_found)
      ref.set(NULL);		// reset to null if not found!
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

int taBase::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;			// total number changed
  int mychg = 0;		// my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->is_static) continue;
    if((md->type->ptr == 1) && md->type->DerivesFrom(TA_taBase) &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
// #ifdef DEBUG
//       cerr << "Non-RO raw pointer member: " << md->name << " in type: " << td->name
// 	   << " updating pointer -- should be a ref!" << endl;
//       taMisc::FlushConsole();
// #endif
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasOption("NO_SET_POINTER")) {
	int chg = taBase::UpdatePointers_NewParType_PtrNoSet(ptr, par_typ, new_par);
	nchg += chg; mychg += chg;
      }
      else {
	int chg = taBase::UpdatePointers_NewParType_Ptr(ptr, par_typ, new_par);
	nchg += chg; mychg += chg;
      }
    }
    else if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taSmartRef)) {
	taSmartRef* ref = (taSmartRef*)md->GetOff(this);
	int chg = taBase::UpdatePointers_NewParType_Ref(*ref, par_typ, new_par);
	nchg += chg; mychg += chg;
      }
      if(md->type->InheritsFrom(TA_taSmartPtr)) {
	taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
	int chg = taBase::UpdatePointers_NewParType_SmPtr(*ref, par_typ, new_par);
	nchg += chg; mychg += chg;
      }
      else if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nchg += obj->UpdatePointers_NewParType(par_typ, new_par);
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();		// uae me: i changed
  return nchg;
}

/////////////

bool taBase::UpdatePointers_NewObj_Ptr(taBase** ptr, taBase* ptr_owner, 
				       taBase* old_ptr, taBase* new_ptr) {
  if(!*ptr || (*ptr != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  taBase::SetPointer(ptr, new_ptr);
  ptr_owner->UpdateAfterEdit();	// update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_PtrNoSet(taBase** ptr, taBase* ptr_owner, 
					    taBase* old_ptr, taBase* new_ptr) {
  if(!*ptr || (*ptr != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  *ptr = new_ptr;
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
  TypeDef* ntd = NULL;
  if(new_ptr)
    ntd = new_ptr->GetTypeDef();
  for(int m=0;m<otd->members.size;m++) {
    MemberDef* omd = otd->members[m];
    MemberDef* nmd = NULL;
    if(ntd && ntd->members.size > m)
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
  int nchg = 0;			// total number changed
  int mychg = 0;		// my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if((md->type->ptr == 1) && md->type->DerivesFrom(TA_taBase) &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
// #ifdef DEBUG
//       cerr << "Non-RO raw pointer member: " << md->name << " in type: " << td->name
// 	   << " updating pointer -- should be a ref!" << endl;
//       taMisc::FlushConsole();
// #endif
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasOption("NO_SET_POINTER")) {
	int chg = taBase::UpdatePointers_NewObj_PtrNoSet(ptr, this, old_ptr, new_ptr);
	nchg += chg; mychg += chg;
      }
      else {
	int chg = taBase::UpdatePointers_NewObj_Ptr(ptr, this, old_ptr, new_ptr);
	nchg += chg; mychg += chg;
      }
    }
    else if(md->type->ptr == 0) {
      if(md->type->InheritsFrom(TA_taSmartRef)) {
	taSmartRef* ref = (taSmartRef*)md->GetOff(this);
	int chg = taBase::UpdatePointers_NewObj_Ref(*ref, this, old_ptr, new_ptr);
	nchg += chg; mychg += chg;
      }
      else if(md->type->InheritsFrom(TA_taSmartPtr)) {
	taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
	int chg = taBase::UpdatePointers_NewObj_SmPtr(*ref, this, old_ptr, new_ptr);
	nchg += chg; mychg += chg;
      }
      else if(md->type->InheritsFrom(TA_taBase)) {
	taBase* obj = (taBase*)md->GetOff(this);
	nchg += obj->UpdatePointers_NewObj(old_ptr, new_ptr);
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();		// uae me: i changed
  return nchg;
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
    taBase* tmp_ptr = m_ptr;
    m_ptr = NULL;
    //send a changing ref, in case it only monitors for setting/clearing (not destroying)
    m_own->SmartRef_DataRefChanging(this, NULL, false); 
    m_own->SmartRef_DataDestroying(this, tmp_ptr); 
    //NO MORE CODE HERE -- object may have destroyed itself
  } else 
    m_ptr = NULL;
}

void taSmartRef::DataRefChanging(taBase* obj, bool setting) {
  if (m_own) {
    m_own->SmartRef_DataRefChanging(this, obj, setting); 
  }
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
    delete m_data_link; // NULLS the ref
  }
  owner = NULL;
  if (user_data_) {
    delete user_data_;
    user_data_ = NULL;
  }
  inherited::CutLinks();
}

void taOBase::Copy_(const taOBase& cp) {
  if (user_data_) {
    user_data_->Reset(); // note: we just leave an empty list if no cp.userdata
    if (cp.user_data_)
      user_data_->Copy(*cp.user_data_);
  } else if (cp.user_data_ && (cp.user_data_->size > 0)) {
    GetUserDataList(true)->Copy(*cp.user_data_);
  }
}

UserDataItem_List* taOBase::GetUserDataList(bool force) const { 
  if (!user_data_ && force) {
    user_data_ = new UserDataItem_List; 
    taOBase* ths =  const_cast<taOBase*>(this); // note: harmless const casts
    taBase::Own(user_data_, ths);
    user_data_->el_typ = &TA_UserDataItem; // set default type to create
    ths->DataChanged(DCR_USER_DATA_UPDATED);
  } 
  return user_data_;
}


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

void taNBase::SetDefaultName() {
  if(taMisc::not_constr || taMisc::in_init || GetTypeDef() == &TA_taNBase)
    return;
  SetDefaultName_();
}

void taNBase::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  name = taMisc::StringCVar(name); // make names C legal names -- just much safer
}

//////////////////////////
//  taFBase		//
//////////////////////////

bool taFBase::SetFileName(const String& val) {
  QFileInfo fi(val);
  //note: canonical paths only available if file actually exists...
  if (fi.exists()) {
    // we get the canonical path, so we insure we can do exact filename compares later
    file_name = fi.canonicalFilePath(); 
  } else
    file_name = val; 
  return true;
}


//////////////////////////
// 	taList_impl	//
//////////////////////////

void taBase_RefList::Initialize() {
  m_own = NULL;
}

taBase_RefList::~taBase_RefList() {
  m_own = NULL; // conservative, even though removing items shouldn't trigger anything
  Reset();
}

void taBase_RefList::DataLinkDestroying(taDataLink* dl) {
  // note: dl has already done a RemoveDataLink on us
  taBase* tab = dl->taData();
  if (tab) { // should exist!
    // note: we need to remove all instances, in case multiply-added
    while (RemoveEl(tab)) {;} 
    if (m_own) {
      m_own->DataDestroying_Ref(this, tab);
    }
  }
#ifdef DEBUG //**CAUTION: warnings can cause gui eventloop events!
  else {
    taMisc::Warning("Unexpected taData() NULL in taBase_RefList::DataLinkDestroying()");
  }
#endif
}

void taBase_RefList::DataDataChanged(taDataLink* dl, int dcr, void* op1, void* op2) {
  if (!m_own) return;
  taBase* tab = dl->taData();
  m_own->DataChanged_Ref(this, tab, dcr, op1, op2);
}

void* taBase_RefList::El_Ref_(void* it_) {
  taBase* it = (taBase*)it_;
  it->AddDataClient(this); 
  return it_;
}

void* taBase_RefList::El_unRef_(void* it_) {
  taBase* it = (taBase*)it_;
  it->RemoveDataClient(this);
  return it_;
}

void taBase_RefList::setOwner(IRefListClient* own_) {
  m_own = own_;
}

int taBase_RefList::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  int nchg = 0;
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    taBase* old_own = itm->GetOwner(old_par->GetTypeDef());
    if(old_own != old_par) continue;
    String old_path = itm->GetPath(NULL, old_par);
    MemberDef* md;
    taBase* nitm = new_par->FindFromPath(old_path, md);
    if(nitm) {
      ReplaceIdx(i, nitm);
      nchg++;
    }
    else {
      RemoveIdx(i);
    }
  }
  return nchg;
}



//////////////////////////
// 	taList_impl	//
//////////////////////////

void taList_impl::Initialize() {
  SetBaseType(&TA_taBase);
  el_def = 0;
  m_trg_load_size = -1;
}


void taList_impl::Destroy() {
  CutLinks();
}

void taList_impl::CutLinks() {
  if (m_data_link) m_data_link->DataDestroying(); // do early
  RemoveAll();
  inherited_taBase::CutLinks();
}

void taList_impl::CanCopy_impl(const taBase* cp_fm_, bool quiet,
  bool& ok, bool virt) const 
{
  if (virt) {
    inherited::CanCopy_impl(cp_fm_, quiet, ok, virt);
    if (!ok) return; // no reason to continue, and could be bad to do so
  }
  const taList_impl* cp_fm = (const taList_impl*)cp_fm_; // is safe
  // we only allow list copies when the base types are the same,
  // since otherwise, we must be dealing with lists for different
  // purposes, and they cannot be considered assignable
  if (CheckError((el_base != cp_fm->el_base), quiet, ok,
    "Copy: Lists must have same el_base to be copyable")) return;
}

void taList_impl::Copy_(const taList_impl& cp) {
//old  if(!cp.name.empty()) name = cp.name;
//no    name = cp.name;
  el_base = cp.el_base;
  el_typ = cp.el_typ;
  el_def = cp.el_def;
//old  taPtrList_impl::Copy_Duplicate(cp);
  taPtrList_impl::Copy_Exact(cp);
}

void* taList_impl::El_CopyN_(void* to_, void* fm) {
  taBase* to = (taBase*)to_;
  to->StructUpdate(true);
  void* rval = El_Copy_(to_, fm);
  El_SetName_(to_, El_GetName_(fm));
  to->StructUpdate(false);
  return rval;
}

void taList_impl::UpdateAfterEdit(){
  inherited_taBase::UpdateAfterEdit();
  if(!el_typ->InheritsFrom(el_base)) el_typ = el_base;
}

void taList_impl::CheckChildConfig_impl(bool quiet, bool& rval) {
  //note: we have to process everyone, because this is the routine
  // that asserts or clears the state, even if an invalid found early
  for (int i = 0; i < size; ++i) {
    taBase* child = (taBase*)FastEl_(i);
    // we only include owned items, not linked
    if (!child || (child->GetOwner() != this)) 
      continue;
    child->CheckConfig(quiet, rval);
  }
}


void taList_impl::ChildUpdateAfterEdit(taBase* child, bool& handled) {
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
  //note: set stale before notifies
  if (useStale() && ((dcr >= DCR_LIST_ORDER_MIN) && (dcr <= DCR_LIST_ORDER_MAX)))
    setStale();
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

String taList_impl::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_disp_name) return GetName(); // get name here instead of display name
  return inherited_taBase::GetColText(key, itm_idx);
}

bool taList_impl::ChangeType(int idx, TypeDef* new_type) {
  if(TestError(!new_type, "ChangeType", "new type is null")) return false;
  if(TestError(!InRange(idx), "ChangeType", "index is out of range")) return false;
  taBase* itm = (taBase*)el[idx];
  if(TestError(!itm, "ChangeType", "item is null")) return false;
  TypeDef* itd = itm->GetTypeDef();
  if(!new_type->InheritsFrom(itd) && !itm->InheritsFrom(new_type)) {
    // do they have a common parent? if so, convert to that first, then back to new_type
    if(itd->parents.size >= 1) {
      if(new_type->InheritsFrom(itd->GetParent())) {
	ChangeType(idx, itd->GetParent());
	itm = (taBase*)el[idx];
	RemoveIdx(size-1);			// remove the last guy!
      }
      else if((itd->GetParent()->parents.size >= 1) &&
	      new_type->InheritsFrom(itd->GetParent()->GetParent())) {
	ChangeType(idx, itd->GetParent()->GetParent());
	itm = (taBase*)el[idx];
	RemoveIdx(size-1);			// remove the last guy!
      }
      else {
	TestError(true, "Changetype", "Cannot change to new type:",new_type->name,
		  "which does not inherit from:", itd->name,
		  "(or vice-versa)",itm->GetPath(NULL,this));
	return false;
      }
    }
    else {
      TestError(true, "ChangeType", "Cannot change to new type:",new_type->name,
		"which does not inherit from:", itd->name,
		"(or vice-versa)",itm->GetPath(NULL,this));
      return false;
    }
  }
  taBase* rval = taBase::MakeToken(new_type);
  if(TestError(!rval, "ChangeType", "maketoken is null")) return false;
  Add(rval);		// add to end of list
  String orgnm = itm->GetName();
  rval->UnSafeCopy(itm);	// do the copy!
  rval->SetName(orgnm);		// also copy name -- otherwise not copied
  SwapIdx(idx, size-1);		// switch positions, so old guy is now at end!
  itm->UpdatePointersToMe(rval); // allow us to update all things that might point to us
  // then do a delayed remove of this object (in case called by itself!)
  itm->CloseLater();
  return true;
}

bool taList_impl::ChangeType(taBase* itm, TypeDef* new_type) {
  int idx = FindEl(itm);
  if(idx >= 0)
    return ChangeType(idx, new_type);
  TestWarning(true,"ChangeType","item not found");
  return false;
}

void taList_impl::Close() {
  if (size > 0) {
    RemoveAll();
  }
  inherited_taBase::Close();
}

bool taList_impl::Close_Child(taBase* obj) {
  return RemoveEl(obj);
}

bool taList_impl::CloseLater_Child(taBase* obj) {
#ifdef DEBUG
  if (obj->refn <= 0) {
    cerr << "WARNING: taList_impl::CloseLater_Child: taBase refn <= 0 for item type="
      << obj->GetTypeDef()->name.chars() << "name='" << obj->GetName().chars() << "'\n";
  }
#endif
  // just add to list -- it will then close us
  tabMisc::DelayedClose(obj);
  return true;
}

taBase*	taList_impl::CopyChildBefore(taBase* src, taBase* child_pos) {
  int idx = FindEl(child_pos);
  if(idx < 0) idx = size;
  taBase* new_obj = src->MakeToken();
  Insert(new_obj, idx);
  new_obj->UnSafeCopy(src);
  new_obj->SetName(src->GetName());
  new_obj->UpdateAfterEdit();
  return new_obj;
}

String taList_impl::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			      bool force_inline) const {
  String nm = " Size: ";
  nm += String(size);
  nm += String(" (") + el_typ->name + ")";
  return nm;
}

bool taList_impl::SetValStr(const String& val, void* par, MemberDef* memb_def, 
			    TypeDef::StrContext sc, bool force_inline) {
  if(val != String::con_NULL) {
    String tmp = val;
    if(tmp.contains('(')) {
      tmp = tmp.after('(');
      tmp = tmp.before(')');
    }
    tmp.gsub(" ", "");
    TypeDef* td = taMisc::types.FindName(tmp);
    if(td != NULL) {
      el_typ = td;
      return true;
    }
  }
  return false;
}

taObjDiffRec* taList_impl::GetObjDiffVal(taObjDiff_List& odl, int nest_lev,  MemberDef* memb_def,
	  const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  taObjDiffRec* odr = inherited::GetObjDiffVal(odl, nest_lev, memb_def, par, par_typ, par_od);

  // need a special sub-parent to distinguish list elements from members -- keep member def info tho
  taObjDiffRec* lsodr = new taObjDiffRec(odl, nest_lev+1, GetTypeDef(), memb_def, (void*)this,
				       (void*)this, GetTypeDef(), odr);
  if(GetOwner())
    lsodr->extra = new taBaseObjDiffRecExtra((taBase*)this);

  lsodr->name = odr->name + "_el";
  lsodr->value = odr->value + "_el";
  lsodr->ComputeHashCode();
  odl.Add(lsodr);

  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(itm && itm->GetOwner() == this) {
      itm->GetObjDiffVal(odl, nest_lev+2, NULL, this, GetTypeDef(), lsodr);
    }
  }
  return odr;
}

int taList_impl::Dump_Save_PathR(ostream& strm, taBase* par, int indent) {
   bool dump_my_path = !(this == par);
  // dump_my_path is a bit of a hack, to enable us to use this same
  // routine either for Dump_Save_PathR or when dumping list items
  // recursively -- when dump_my_path=false, we have already dumped the
  // item path one level above, so only need its members, and children
  
  // first save any sub-members (there usually aren't any)
  int rval = GetTypeDef()->Dump_Save_PathR(strm, (void*)this, (void*)par, indent);

//   if (IsEmpty())  return rval;
  // actually need to save this to be able to undo back to an empty group

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

int taList_impl::Dump_Save_PathR_impl(ostream& strm, taBase* par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  int cnt = 0;
  int i;
  for (i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(itm == NULL)
      continue;
    cnt++; // sure we are dumping something at this point

    if (El_Kind_(itm) == EK_LINK) { // a link, create a dummy placeholder
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
int taList_impl::Dump_SaveR(ostream& strm, taBase* par, int indent) {
  if(!Dump_QuerySaveChildren()) return true;
  String mypath = GetPath(NULL, par);
  int i;
  for(i=0; i<size; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    if (!itm) continue;
    ElKind ek = El_Kind_(itm);
    if (ek == EK_OWN) {
      itm->Dump_Save_impl(strm, par, indent);
    }
    else if (ek == EK_LINK) {	// a link
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

void taList_impl::Dump_Load_pre() {
  inherited::Dump_Load_pre();	// called only when directly loading this group!
  //  Reset(); // actually can't do this because same fun is used for loading
  // entire list/group as for one new element in the group.. hmm.
}

taBase* taList_impl::Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ) {
  if(el_path.firstchar() != '[' || el_path.lastchar() != ']') {
    taMisc::Warning("*** Dump_Load_Path_parent: path is incorrectly formatted:",el_path,
		    "for parent obj:",GetPath());
    return NULL;
  }
  int idx = (int)el_path.between('[',']');
  taBase* nw_el = NULL;
  if(InRange(idx)) {
    nw_el = (taBase*)FastEl_(idx);
    if(!nw_el)	RemoveIdx(idx);	// somehow has a null guy on list.. nuke it
  }
  if(nw_el) {
    // check for correct type, but allow a list to be created in a group
    // for backwards compatibility with changes from groups to lists
    if((nw_el->GetTypeDef() != ld_el_typ) &&
       !((nw_el->GetTypeDef() == &TA_taBase_List) && (ld_el_typ == &TA_taBase_Group))) {
      // object not the right type, try to create new one..
      if(taMisc::verbose_load >= taMisc::MESSAGES) {
	taMisc::Warning("*** Object at path:",GetPath(),
			"of type:",nw_el->GetTypeDef()->name,"is not the right type:",
			ld_el_typ->name,", attempting to create new one");
      }
      nw_el = taBase::MakeToken(ld_el_typ);
      if(!nw_el) {
	taMisc::Warning("*** Dump_Load_Path_parent: Could not make new token of type:",
			ld_el_typ->name,"for child item at path:",
			el_path,"in parent list:",GetPath());
	return NULL;
      }
      ReplaceIdx(idx, nw_el);
      // this should no longer be necc as any changes will be in the parent!
      // todo: nuke me..
//       if(nw_el->GetOwner() != NULL) {
// 	String new_path = nw_el->GetPath(NULL, find_base);
// 	if(new_path != orig_path) {
// 	  dumpMisc::path_subs.Add(ld_el_typ, find_base, orig_path, new_path);
// 	}
//       }
    }
  }
  else {
    nw_el = New(1,ld_el_typ);
    // assuming that these are saved in order so we just add sequentially w/out checking
    // no clear action to take if this is not the case b/c we don't know how to
    // create intervening types..
    if(!nw_el) {
      taMisc::Warning("*** New: Could not make a token of:",ld_el_typ->name,"in:",GetPath());
      return NULL;
    }
    // this should no longer be necc as any changes will be in the parent!
    // todo: nuke me..
//     if(nw_el->GetOwner()) {
//       String new_path = nw_el->GetPath(NULL, find_base);
//       if(new_path != orig_path) {
// 	dumpMisc::path_subs.Add(ld_el_typ, find_base, orig_path, new_path);
//       }
//     }
  }
  if(m_trg_load_size >= 0 && idx+1 == m_trg_load_size) {
    // we are the last guy who is supposed to be loaded -- set the list size to be the 
    // target size -- nukes any extra guys that might have been lingering!!
    SetSize(m_trg_load_size);
    m_trg_load_size = -1;	// reset for next time around
  }

  if(taMisc::verbose_load >= taMisc::TRACE) {
    cerr << "Success: Leaving TypeDef::Dump_Load_Path_parent, type: " << ld_el_typ->name
	 << ", parent path = " << GetPath()
	 << ", el_path = " << el_path
	 << "\n"; // endl;
    taMisc::FlushConsole();
  }
  return nw_el;
}

int taList_impl::Dump_Load_Value(istream& strm, taBase* par) {
  m_trg_load_size = -1;

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
	  if(TestWarning((!eltd || !eltd->InheritsFrom(el_base)),
			 "Dump_Load_Value",
			 "Null or invalid type:",typ_nm,"to link into list of types:",
			 el_base->name)) return false;
	  c = taMisc::read_till_rb_or_semi(strm);
	  String lnk_path = taMisc::LexBuf;
	  dumpMisc::path_subs.FixPath(eltd, tabMisc::root, lnk_path);
	  MemberDef* md;
	  taBase* tp = tabMisc::root->FindFromPath(lnk_path, md);
	  if(idx < size)
	    ReplaceLinkIdx(idx, tp); // if already room, replace it..
	  else {
	    Link(tp);		// otherwise, add it..
	    idx = size-1;
	  }
	  if(tp == NULL) {
	    dumpMisc::vpus.AddVPU((taBase**)&(el[idx]), (taBase*)NULL, lnk_path);
	  }
	  return true;
	}
	else if(c == '{') {	// no type information -- just the count
	  strm.get();		// get the bracket
	  Alloc(idx);		// just make sure we have the ptrs allocated to this size
	  m_trg_load_size = idx; // target loading size
	  if(m_trg_load_size == 0) {
	    if(size > 0)
	      cerr << "load reset size: " << size << endl;
	    taList_impl::RemoveAll();
	    // normal load size enforcement occurs on last item loaded --
	    // if no items to load, it never happens!  This is the enforcer!
	    // also: for undo optimized loading (i.e., not saving networks), this count
	    // is actually set accurately pre-filtering so it is fine
	    // also, using list version b/c groups may have sub guys and don't want to nuke 
	    // those guys!!
	  }
	  return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
	}
	else {			// type information -- create objects too!
	  taMisc::read_word(strm, true); // get type
	  String typ_nm = taMisc::LexBuf;
	  TypeDef* eltd = taMisc::types.FindName(typ_nm);
	  if(TestWarning((!eltd || !eltd->InheritsFrom(el_base)),
			 "Dump_Load_Value",
			 "Null or invalid type:",typ_nm,"to link into list of types:",
			 el_base->name)) return false;
	  el_typ = eltd;
	  // ensure that enough items are present (don't do full enforce size)
//  	  if(size < idx)
// 	    New(idx - size, el_typ);
	  // actually, probably much better to do full enforce size!  but this case is weird
	  // not sure if it actually still gets processed -- why would it??
	  // actually very important to set the exact size
	  SetSize(idx);
	  c = taMisc::skip_white(strm);
	  if(c == '{') {
	    return GetTypeDef()->members.Dump_Load(strm, (void*)this, (void*)par);
	  }
	}
      }
    }
    TestWarning(true, "Dump_Load_Value", "Bad formatting for link");
    return false;
  }

  TestWarning(true, "Dump_Load_Value", "Missing '{', '=', or '[' in dump file");
  return false;
}

void taList_impl::El_SetDefaultName_(void* item_, int idx) {
  taBase* item = (taBase*)item_;
  if (item->HasOption("DEF_NAME_LIST")) {
    item->SetDefaultName_impl(idx);
  }
}

void taList_impl::EnforceSameStru(const taList_impl& cp) {
  int i;
  for(i=0; i<cp.size; i++) {
    taBase* citm = (taBase*)cp.el[i];
    if(citm == NULL) continue;
    if(size <= i) {
      taBase* itm = taBase::MakeToken(citm->GetTypeDef());
      if(itm != NULL)
	Add_(itm);
    }
    else {
      taBase* itm = (taBase*)el[i];
      if(citm->GetTypeDef() == itm->GetTypeDef())
	continue;
      taBase* rval = taBase::MakeToken(citm->GetTypeDef());
      if(rval != NULL)
	ReplaceIdx(i, rval);
    }
  }
  if(size > cp.size)
    for(i=size-1; i>=cp.size; i--)
      RemoveIdx(i);
}

void taList_impl::SetSize(int sz) {
  if(size < sz)
    New(sz - size);
  else {
    Trim(sz);
  }
}

void taList_impl::EnforceType() {
  int i;
  for(i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if((itm == NULL) || (itm->GetTypeDef() == el_typ))
      continue;

    taBase* rval = taBase::MakeToken(el_typ);
    if(rval != NULL)
      ReplaceIdx(i, rval);
  }
}

void* taList_impl::FindMembeR(const String& nm, MemberDef*& ret_md) const {
  ret_md = NULL;
  
  // first look for special list index syntax
  String idx_str = nm;
  idx_str = idx_str.before(']');
  if(idx_str.nonempty()) {
    idx_str = idx_str.after('[');
    int idx = atoi(idx_str);
    if((idx >= size) || (idx < 0)) {
      return NULL;
    }
    return el[idx];		// don't have an md for this guy
  }

  // then look for items in the list itself, by name or type
  taBase* fnd = FindNameType_(nm);
  if(fnd)
    return fnd;

  // then look on members of list obj itself, recursively 
  void* rval = inherited::FindMembeR(nm, ret_md);
  if(rval)
    return rval;

  // finally, look recursively on owned objs on list
  //  int max_srch = MIN(taMisc::search_depth, size);
  // these days, it just doesn't make sense to restrict!
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    if(itm && itm->GetOwner() == this) {
      rval = itm->FindMembeR(nm, ret_md);
      if(rval)
	return rval;
    }
  }
  return NULL;
}

int taList_impl::FindTypeIdx(TypeDef* it) const {
  for(int i=0; i < size; i++) {
    taBase* tmp = (taBase*)el[i];
    if(tmp && tmp->InheritsFrom(it)) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindType_(TypeDef* it) const {
  int idx = FindTypeIdx(it);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

int taList_impl::FindNameContainsIdx(const String& item_nm) const {
  for(int i=0; i < size; i++) {
    taBase* it = (taBase*)el[i];
    if(it && it->GetName().contains(item_nm)) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindNameContains_(const String& item_nm) const {
  int idx = FindNameContainsIdx(item_nm);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

int taList_impl::FindNameTypeIdx(const String& item_nm) const {
  for(int i=0; i < size; i++) {
    taBase* it = (taBase*)el[i];
    if(it && (it->FindCheck(item_nm) || it->InheritsFromName(item_nm))) {
      return i;
    }
  }
  return -1;
}

taBase* taList_impl::FindNameType_(const String& item_nm) const {
  int idx = FindNameTypeIdx(item_nm);
  if(idx >= 0) return (taBase*)el[idx];
  return NULL;
}

String taList_impl::GetPath(taBase* ta, taBase* par_stop) const {
  if((((taBase*) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(((taBase*) this) != par_stop)
    rval = par->GetPath((taBase*)this, par_stop);

  if (ta != NULL) {
    MemberDef* md;
    if((md = FindMember(ta)) != NULL) {
      rval += "." + md->name;
    }
    else if((md = FindMemberPtr(ta)) != NULL) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      int gidx = FindEl_(ta);
      if(gidx >= 0)
	rval += "[" + String(gidx) + "]";
      else
	rval += "[?]";
    }
  }
  return rval;
}

String taList_impl::GetPath_Long(taBase* ta, taBase* par_stop) const {
  if((((taBase*) this) == par_stop) && (ta == NULL))
    return ".";
  String rval;

  taBase* par = GetOwner();
  if(par == NULL) {
    if(ta == NULL) rval = "root";
  }
  else if(((taBase*) this) != par_stop)
    rval = par->GetPath_Long((taBase*)this, par_stop);

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
      int gidx = FindEl_(ta);
      if(gidx >= 0)
	rval += "[" + String(gidx) + "]";
      else
	rval += "[?]";
    }
  }
  return rval;
}

taBase* taList_impl::New_impl(int no, TypeDef* typ, const String& name_) {
  if(typ == NULL)
    typ = el_typ;
  if(TestWarning(!typ->InheritsFrom(el_base), "New",
		 "Attempt to create type:", typ->name,
		 "in list with base type:", el_base->name)) return NULL;
  if(TestError(typ->HasOption("VIRT_BASE"),
		 "You cannot create a token of type:", typ->name,
		 "because it is a 'virtual' base type -- you must create a more specific subtype of it instead")) return NULL;
  if(no > 10)			// adding large numbers is slow!
    StructUpdate(true);
  taBase* rval = NULL;
  Alloc(size + no);		// pre-allocate!
  if((size == 0) || (no > 1))
    el_typ = typ;	// first item or multiple items set el_typ
  int i;
  for(i=0; i < no; i++) {
    rval = taBase::MakeToken(typ);
    if(TestError(!rval, "New", "Could not make a token of type",typ->name,"(probably has #NO_INSTANCE in object header comment directive)"))
      goto exit;
    if (name_.nonempty() && (name_ != "(default name)")) {
      // if only 1 inst, then name is literal, else it is a base
      if (no == 1) {
        rval->SetName(name_);
      } else {
        rval->SetName(name_ + "_" + String(i));
      }
    }
    Add_(rval);
  }
 exit:
  if(no > 10)
    StructUpdate(false);
  return rval;
}

taBase* taList_impl::New_gui(int no, TypeDef* typ, const String& name_) {
  if(!el_typ->InheritsFrom(el_base)) el_typ = el_base;
  taBase* rval = New(no, typ, name_);
  if (rval) {
    if (taMisc::gui_active && !taMisc::no_auto_expand) {
      if(!HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL")) 
      {
        tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
        tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
      }
    }
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
    ((taBase*)el[i])->OutputR(strm, indent+1);
    taMisc::FlushConsole();
  }

  taMisc::indent(strm, indent) << "}\n";
  return strm;
}

bool taList_impl::RemoveIdx(int i) {
  // default could be out of range..
  if(el_def >= size-1)
    el_def = 0;
  return taPtrList_ta_base::RemoveIdx(i);
}

bool taList_impl::Transfer(taBase* item) {
  taBase* oldo = item->GetOwner();
  bool rval = Transfer_((void*)item);
  item->UpdateAfterMove(oldo);
  return rval;
}

int taList_impl::ReplaceType(TypeDef* old_type, TypeDef* new_type) {
  int nchanged = 0;
  int sz = size;		// only go to current size
  int i;
  for(i=0;i<sz;i++) {
    if(((taBase*)el[i])->GetTypeDef() != old_type) continue;
    if(ChangeType(i, new_type)) nchanged++;
  }
  return nchanged;
}

void taList_impl::SetBaseType(TypeDef* it) {
  el_base = it;
  el_typ = it;
}

int taList_impl::SetDefaultEl(taBase* it) {
  int idx = FindEl(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultElName(const String& nm) {
  int idx = FindNameIdx(nm);
  if(idx >= 0)    el_def = idx;
  return idx;
}

int taList_impl::SetDefaultElType(TypeDef* it) {
  int idx = FindTypeIdx(it);
  if(idx >= 0)    el_def = idx;
  return idx;
}

void taList_impl::SetDefaultName() {
  // nop; still 
}

int taList_impl::SelectForEditSearch(const String& memb_contains, SelectEdit*& editor) {
  int nfound = taOBase::SelectForEditSearch(memb_contains, editor);
  for(int i=0;i<size;i++) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nfound += itm->SelectForEditSearch(memb_contains, editor);
    }
  }
  return nfound;
}

void taList_impl::Search_impl(const String& srch, taBase_PtrList& items,
			 taBase_PtrList* owners,
			 bool contains, bool case_sensitive,
			 bool obj_name, bool obj_type,
			 bool obj_desc, bool obj_val,
			 bool mbr_name, bool type_desc) {
  int st_sz = items.size;
  taOBase::Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
		       obj_desc, obj_val, mbr_name, type_desc);
  bool already_added_me = false;
  if(items.size > st_sz)
    already_added_me = true;
  for(int i=0; i<size; i++) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      if(SearchTestItem_impl(itm, srch, contains, case_sensitive, obj_name, obj_type,
			     obj_desc, obj_val, mbr_name, type_desc)) {
	items.Link(itm);
      }
      itm->Search_impl(srch, items, owners, contains, case_sensitive, obj_name, obj_type,
		       obj_desc, obj_val, mbr_name, type_desc);
    }
  }
  if(owners && (items.size > st_sz) && !already_added_me) { // we added somebody somewhere..
    owners->Link(this);
  }
}

void taList_impl::CompareSameTypeR(Member_List& mds, TypeSpace& base_types,
				   voidptr_PArray& trg_bases, voidptr_PArray& src_bases,
				   taBase* cp_base, int show_forbidden,
				   int show_allowed, bool no_ptrs) {
  if(!cp_base) return;
  if(GetTypeDef() != cp_base->GetTypeDef()) return; // must be same type..

  taOBase::CompareSameTypeR(mds, base_types, trg_bases, src_bases, cp_base,
			    show_forbidden, show_allowed, no_ptrs);
  // then recurse..
  taList_impl* cp_lst = (taList_impl*)cp_base;
  int mxsz = MIN(size, cp_lst->size);
  for(int i=0; i<mxsz; i++) {
    taBase* itm = (taBase*)FastEl_(i);
    taBase* cp_itm = (taBase*)cp_lst->FastEl_(i);
    if(!itm || !cp_itm) continue;
    if((itm->GetOwner() == this) && (cp_itm->GetOwner() == cp_lst)) {
       // for guys we own (not links; prevents loops)
      itm->CompareSameTypeR(mds, base_types, trg_bases, src_bases, cp_itm,
			    show_forbidden, show_allowed, no_ptrs);
    }
  }
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
      MemberDef* md;
      taBase* nitm = new_par->FindFromPath(old_path, md);
      if(nitm) {
	ReplaceLinkIdx_(i, nitm);
	nchg++;
      }
      else {
	RemoveIdx(i);
      }
    }
  }
  return nchg;
}

int taList_impl::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  int nchg = taOBase::UpdatePointers_NewParType(par_typ, new_par);
  for(int i=size-1; i>=0; i--) {
    taBase* itm = (taBase*)el[i];
    if(!itm) continue;
    if(itm->GetOwner() == this) { // for guys we own (not links; prevents loops)
      nchg += itm->UpdatePointers_NewParType(par_typ, new_par);
    }
    else {			// linked item: could be to something outside of scope!
      taBase* old_own = itm->GetOwner(par_typ);
      String old_path = itm->GetPath(NULL, old_own);
      MemberDef* md;
      taBase* nitm = new_par->FindFromPath(old_path, md);
      if(nitm) {
	ReplaceLinkIdx_(i, nitm);
	nchg++;
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
	RemoveIdx(i);
      else
	ReplaceLinkIdx_(i, new_ptr);    // it is a link to old guy; replace it!
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


#endif


//////////////////////////
// 	taDataView	//
//////////////////////////

void taDataView::Initialize() {
  m_data = NULL;
  data_base = &TA_taBase;
  m_dbu_cnt = 0;
  m_parent = NULL;
  m_index = -1;
  m_vis_cnt = 0;
  m_defer_refresh = 0;
}

void taDataView::CutLinks() {
  m_parent = NULL;
  SetData(NULL);
  inherited::CutLinks();
}
 
void taDataView::Copy_(const taDataView& cp) {
  SetData(cp.m_data);
}

void taDataView::UpdateAfterEdit() {
  inherited::UpdateAfterEdit();
  if (isMapped())
    Render_impl();
}

void taDataView::UpdateAfterEdit_impl() {
  inherited::UpdateAfterEdit_impl();
  if (taMisc::is_loading) {
    if (m_data) 
      m_data->AddDataClient(this);
  }
}

void taDataView::ChildAdding(taDataView* child) {
  child->m_vis_cnt = m_vis_cnt;
}

// arbitrary number, not likely to be higher than this
#define MAX_VIS_CNT 6

void taDataView::SetVisible(bool showing) {
  DataViewAction act = (showing) ? SHOWING_IMPL : HIDING_IMPL;
  DoActions(act);
}

void taDataView::SetVisible_impl(DataViewAction act) {
  int do_defer_refresh = 0;
  if (act & SHOWING_IMPL) {
    if (++m_vis_cnt == 1) {
      // first hide->show: do a defered struct guy if appl
      do_defer_refresh = m_defer_refresh;
      if (do_defer_refresh > 0)
        DataDataChanged(NULL, DCR_STRUCT_UPDATE_BEGIN, NULL, NULL);
      else if (do_defer_refresh < 0)
        DataDataChanged(NULL, DCR_DATA_UPDATE_BEGIN, NULL, NULL);
    }
#ifdef DEBUG
    TestError((m_vis_cnt > MAX_VIS_CNT), "taDataView::SetVisible_impl",
      "m_vis_cnt > likely max, may indicate show/hide issues (is:",
      String(m_vis_cnt), ")");
#endif
  } else { // act & HIDING_IMPL
    if (TestError((--m_vis_cnt < 0), "taDataView::SetVisible_impl",
      "m_vis_cnt went -ve, indicates show/hide issues"))
      m_vis_cnt = 0;
  }
  
  DoActionChildren_impl(act);
  
  if (do_defer_refresh == 0) return;
  
  m_defer_refresh = 0;
  if (do_defer_refresh > 0)
    DataDataChanged(NULL, DCR_STRUCT_UPDATE_END, NULL, NULL);
  else // (do_defer_rebuild < 0)
    DataDataChanged(NULL, DCR_DATA_UPDATE_END, NULL, NULL);
}


void taDataView::IgnoredDataChanged(taDataLink*, int dcr, void* op1_, void* op2_) {
  // note: should not need to track anything during loading
  if (taMisc::is_loading) return;
  
  // keep track if we need to update -- struct has priority, and overrides data
  if ((dcr == DCR_STRUCT_UPDATE_BEGIN) ||
    (dcr == DCR_REBUILD_VIEWS))
    m_defer_refresh = 1;
  else if ((m_defer_refresh == 0) && ((dcr == DCR_DATA_UPDATE_BEGIN) ||
    (dcr <= DCR_ITEM_UPDATED_ND) || (dcr == DCR_UPDATE_VIEWS)))
    m_defer_refresh = -1;
}

// set this to emit debug messages for the following code..
// #define DATA_DATA_DEBUG 1

void taDataView::DataDataChanged(taDataLink*, int dcr, void* op1_, void* op2_) {
  // detect the implicit DATA_UPDATE_END
#ifdef DATA_DATA_DEBUG    
  if(dcr <= DCR_ITEM_UPDATED_ND) {
    cerr << GetName() << " iu: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
  }
#endif
  if ((m_dbu_cnt == -1) && (dcr <= DCR_ITEM_UPDATED_ND))
    dcr = DCR_DATA_UPDATE_END;
  // we need to reinterpret a ITEM_UPDATED if we are in datamode with count=1
  // that is sent instead of the terminal DATA_UPDATE_END
  if (dcr == DCR_STRUCT_UPDATE_BEGIN) { // forces us to be in struct state
    if (m_dbu_cnt < 0) m_dbu_cnt *= -1; // switch state if necessary
    ++m_dbu_cnt;
#ifdef DATA_DATA_DEBUG    
    cerr << GetName() << " stru start: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
    return;
  } else if (dcr == DCR_DATA_UPDATE_BEGIN) { // stay in struct state if struct state
    if (m_dbu_cnt > 0) ++m_dbu_cnt;
    else               --m_dbu_cnt;
#ifdef DATA_DATA_DEBUG    
    cerr << GetName() << " data start: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
    return;
  } else if ((dcr == DCR_STRUCT_UPDATE_END) || (dcr == DCR_DATA_UPDATE_END))
  {
    bool stru = false;
    if (m_dbu_cnt < 0) ++m_dbu_cnt;
    else {stru = true; --m_dbu_cnt;}
#ifdef DATA_DATA_DEBUG    
    if(dcr == DCR_DATA_UPDATE_END)
      cerr << GetName() << " data end: " << m_dbu_cnt << endl;
    else
      cerr << GetName() << " stru end: " << m_dbu_cnt << endl;
    taMisc::FlushConsole();
#endif
    if (m_dbu_cnt == 0) {
      int pdbu = parDbuCnt();
      // we will only signal if no parent update, or if parent is data and we are structural
      if ((pdbu == 0)) {
        if (stru) {
          DataStructUpdateEnd_impl();
          DataDataChanged_impl(DCR_STRUCT_UPDATE_END, NULL, NULL);
        } else {
          DataUpdateView_impl();
          DataDataChanged_impl(DCR_DATA_UPDATE_END, NULL, NULL);
        }
      } else if ((pdbu < 0) && stru) {
          DataStructUpdateEnd_impl();
          DataDataChanged_impl(DCR_STRUCT_UPDATE_END, NULL, NULL);
      }
    }
    return;
  }
  if ((m_dbu_cnt > 0) || (parDbuCnt() > 0))
    return;
  if (dcr <= DCR_ITEM_UPDATED_ND)
    DataUpdateAfterEdit();
  else if (dcr == DCR_UPDATE_VIEWS) {
    DataUpdateView_impl();
    DataDataChanged_impl(DCR_UPDATE_VIEWS, NULL, NULL);
  } else if (dcr == DCR_REBUILD_VIEWS) {
    DataRebuildView_impl();
    DataDataChanged_impl(DCR_REBUILD_VIEWS, NULL, NULL);
  } else {
    DataDataChanged_impl(dcr, op1_, op2_);
  }
}

void taDataView::DataLinkDestroying(taDataLink*) {
  m_data = NULL;
  DataDestroying();
}

void taDataView::DataUpdateAfterEdit() {
  DataUpdateAfterEdit_impl();
  taDataView* par = parent();
  if (par)
    par->DataUpdateAfterEdit_Child(this);
}

void taDataView::DoActions(DataViewAction acts) {
  if (acts & SHOWING_HIDING_MASK) {
    SetVisible_impl(acts); // note: only 1 will be called
    return; // NEVER combined with any other action
  }
  // never do any rendering during load or copying, 
  if (!(taMisc::is_loading || taMisc::is_duplicating)) { 
  
    if (acts & CONSTR_POST) {
      // note: only ever called manually
      Constr_post();
    }
    if (acts & UNBIND_IMPL) {
      Unbind_impl();
    }
  }
  if (acts & CLEAR_IMPL) {
    // must be mapped to do clear
    if (isMapped())
      Clear_impl();
  }
  if (acts & RESET_IMPL) {
    Reset_impl();
  }
  
  if (taMisc::is_loading || taMisc::is_duplicating) return; 
  // no rendering should ever get done if not in gui mode, incl during late shutdown
  if (taMisc::gui_active || taMisc::gui_no_win) {
    if (acts & RENDER_PRE) {
/*obs      // must not already be constructed
      if (!isMapped())*/
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
}


String taDataView::GetLabel() const {
  return GetTypeDef()->GetLabel();
}

int taDataView::parDbuCnt() {
  taDataView* par = parent();
  if (par) {
    int pdbu = par->dbuCnt();
    if (pdbu > 0) return pdbu; //optimization -- don't need to go up the chain if parent is struct
    int ppdbu = par->parDbuCnt();
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
    m_data->RemoveDataClient(this);
    m_data = NULL;
  }
  if (!ta) return;
  if (!ta->GetTypeDef()->InheritsFrom(data_base)) {
    taMisc::Warning("taDataView::m_data must inherit from ", data_base->name);
  } else {
    ta->AddDataClient(this);
    m_data = ta;
  }
}

taBase* taDataView::SetOwner(taBase* own) {
  taBase* rval = inherited::SetOwner(own);
  // note: we have to do this here and in InitLinks
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

taBase* DataView_List::SetOwner(taBase* own) {
  if (own && own->GetTypeDef()->InheritsFrom(&TA_taDataView))
    data_view = (taDataView*)own;
  else data_view = NULL;
  return inherited::SetOwner(own);
}

void DataView_List::DoAction(taDataView::DataViewAction acts) {
  if (acts & taDataView::CONSTR_MASK) {
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

void taArray_base::DataChanged(int dcr, void* op1, void* op2) {
  taOBase::DataChanged(dcr, op1, op2);
}

String taArray_base::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
			      bool force_inline) const {
  if(force_inline) {
    return GetValStr();		// this is the taArray_impl version that actually lists items
  }
  else {
    String nm = " Size: ";
    nm += String(size);
    nm += String(" (") + GetTypeDef()->name + ")";
    return nm;
  }
}

bool taArray_base::SetValStr(const String& val, void* par, MemberDef* memb_def, 
			    TypeDef::StrContext sc, bool force_inline) {
  InitFromString(val);
  return true;
}

int taArray_base::Dump_Save_Value(ostream& strm, taBase*, int) {
  strm << "{ ";
  int i;
  for(i=0;i<size;i++) {
    strm << El_GetStr_(FastEl_(i)) << ";";
  }
  return true;
}

int taArray_base::Dump_Load_Value(istream& strm, taBase*) {
  int c = taMisc::skip_white(strm);
  if(c == EOF)    return EOF;
  if(c == ';') // just a path
    return 2;  // signal that just a path was loaded..

  if(TestError((c != '{'), "Dump_Load_Value",
	       "Missing '{' in dump file")) return false;
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

const int int_FixedArray::blank = 0;
const int int_Array::blank = 0;
const float float_Array::blank = 0.0f;
const double double_Array::blank = 0.0;
const char char_Array::blank = '\0';
const Variant Variant_Array::blank;
const voidptr voidptr_Array::blank = NULL;

//////////////////////////
//  String_Array	//
//////////////////////////

const String String_Array::blank = "";

#ifdef TA_USE_QT
void String_Array::ToQStringList(QStringList& sl) {
  sl.clear();
  for (int i = 0; i < size; ++i) {
    sl.append(FastEl(i).toQString());
  }
}
#endif // TA_USE_QT

String String_Array::ToDelimString(const String& delim) {
  String rval;
  for (int i = 0; i < size; ++i) {
    rval += FastEl(i);
    if(i < size-1) rval += delim;
  }
  return rval;
}

void String_Array::FmDelimString(const String& str, const String& delim, bool reset_first) {
  if(reset_first) Reset();
  String remainder = str;
  while(remainder.nonempty()) {
    if(remainder.contains(delim)) {
      Add(remainder.before(delim));
      remainder = remainder.after(delim);
    }
    else {
      Add(remainder);
      remainder = _nilString;
    }
  }
}


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
  labels.SetSize(size);
  inherited::UpdateAfterEdit();
}

void SArg_Array::Copy_(const SArg_Array& cp) {
  labels = cp.labels;
}

int SArg_Array::Dump_Save_Value(ostream& strm, taBase* par, int indent) {
  int rval = String_Array::Dump_Save_Value(strm, par, indent);
  strm << "};" << endl;
  int rv2 = labels.Dump_Save_Value(strm, this, indent+1);
  return (rval && rv2);
}

int SArg_Array::Dump_Load_Value(istream& strm, taBase* par) {
  int rval = String_Array::Dump_Load_Value(strm, par);
  int c = taMisc::skip_white(strm, true); // peek
  if(c == '{') {
    labels.Dump_Load_Value(strm, this);
  }
  return rval;
}

bool SArg_Array::HasValue(const String& key) const {
  return (labels.FindEl(key) >= 0);
}

String SArg_Array::GetValue(const String& key) const {
  int idx = labels.FindEl(key);
  if (idx >= 0) return SafeEl(idx);
  else return _nilString;
}

void SArg_Array::SetValue(const String& key, const String& value) {
  int idx = labels.FindEl(key);
  if (idx >= 0) {
    if(TestWarning((idx >= size), "SetValue", 
		   "Consistency error: SArg_Array used as key/values has missing entry for key:", key)) {//nop
    }
    else {
      FastEl(idx) = value;
    }
  } else {
    labels.Add(key);
    Add(value);
  }
#ifdef DEBUG // helpful when viewing stuff in gui 
  DataChanged(DCR_ITEM_UPDATED);
#endif
}

///////////////////////////////////////
// NameVar_Array

const String NameVar_Array::def_sep(", ");
const NameVar NameVar_Array::blank;

int NameVar_Array::FindName(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).name == op)
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name == op)
	return i;
    }
  }
  return -1;
}

int NameVar_Array::FindNameContains(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).name.contains(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).name.contains(op))
	return i;
    }
  }
  return -1;
}

int NameVar_Array::FindValue(const Variant& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value == op)
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value == op)
	return i;
    }
  }
  return -1;
}

int NameVar_Array::FindValueContains(const String& op, int start) const {
  int i;
  if(start < 0) {		// search backwards if start < 0
    for(i=size-1; i>=0; i--) {
      if(FastEl(i).value.toString().contains(op))
	return i;
    }
  }
  else {
    for(i=start; i<size; i++) {
      if(FastEl(i).value.toString().contains(op))
	return i;
    }
  }
  return -1;
}

Variant NameVar_Array::GetVal(const String& nm) {
  int idx = FindName(nm);
  if(idx < 0) return _nilVariant;
  return FastEl(idx).value;
}

bool NameVar_Array::GetAllVals(const String& nm, String_Array& vals) {
  for(int i=0;i<size; i++) {
    NameVar& nv = FastEl(i);
    if(nv.name != nm) continue;
    vals.Add(nv.value.toString());
  }
  return (vals.size == 0);
}

bool NameVar_Array::SetVal(const String& nm, const Variant& vl) {
  int idx = FindName(nm);
  if(idx < 0) {
    Add(NameVar(nm, vl));
    return true;
  }
  else {
    FastEl(idx).value = vl;
    return false;
  }
}

//////////////////////////
// taBase_FunCallList	//
//////////////////////////


void taBase_FunCallList::El_Done_(void* it_) {
  FunCallItem* it = (FunCallItem*)it_;
  it->it->RemoveDataClient(this);
  delete it;
}

bool taBase_FunCallList::AddBaseFun(taBase* obj, const String& fun_name) {
  FunCallItem* fci = new FunCallItem(obj, fun_name);
  Add(fci);
  obj->AddDataClient(this); 
  return true;
}

void taBase_FunCallList::DataLinkDestroying(taDataLink* dl) {
  taBase* obj = dl->taData();
  if (!obj) return; // shouldn't happen;
  bool got_one = false;
  for(int i = size-1; i>=0; i--) {
    FunCallItem* fci = FastEl(i);
    if(fci->it == obj) {
      RemoveIdx(i);
      got_one = true;
    }
  }
  
  if(!got_one) {
    taMisc::Error("Internal error -- taBase_FunCallList DataDestroying_Ref didn't find base in base_funs!");
  }
}

//////////////////////////
// UserDataItemBase	//
//////////////////////////

UserDataItemBase::UserDataItemBase(const String& type_name, const String& key_) 
:inherited()
{
  Initialize();
  name = key_;
  taMisc::AddUserDataSchema(type_name, this);
}

//////////////////////////
// UserDataItem	//
//////////////////////////

UserDataItem::UserDataItem(const String& type_name, const String& key_,
  const Variant& value_,  const String& desc_)
:inherited(type_name, key_)
{
  value = value_;
  desc = desc_;
}

//////////////////////////
// taWikiURL
//////////////////////////

void taWikiURL::Initialize() {
  sync = false;
}

String taWikiURL::GetURL() {
  if(!sync) return _nilString;
  if(wiki.nonempty()) {
    String wiki_url = taMisc::GetWikiURL(wiki, true); // true = add index.php/
    if(TestError(wiki_url.empty(), "GetURL", "wiki named:", wiki,
		 "not found in global preferences/options under wiki_url settings"))
      return _nilString;
    return wiki_url + url;
  }
  return url;
}

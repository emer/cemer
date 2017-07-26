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

#include "taBase.h"
#include <taMatrix>
#include <taBaseItr>
#include <MemberDef>
#include <taBase_PtrList>
#include <FlatTreeEl>
#include <FlatTreeEl_List>
#include <ObjDiff>
#include <taProject>
#include <Program>
#include <taDoc>
#include <ControlPanel>
#include <ParamSet>
#include <MethodDef>
#include <UserDataItem_List>
#include <UserDataItemBase>
#include <UserDataItem>
#include <taSmartRef>
#include <taSmartPtr>
#include <taList_impl>
#include <taGroup_impl>
#include <taFiler>
#include <taiMimeSource>
#include <ISigLinkClient>
#include <iColor>
#include <taTaskThread>
#include <Completions>
#include <SigLinkSignal>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taSigLink>
#include <taSigLinkItr>
#include <TypeDefault>
#include <int_Matrix>
#include <byte_Matrix>
#include <slice_Matrix>
#include <String_Matrix>
#include <MainWindowViewer>
#include <taiEdit>
#include <ViewColor_List>
#include <dumpMisc>
#include <taiViewType>
#include <UserData_DocLink>
#include <iMainWindowViewer>
#include <taiMisc>
#include <iHelpBrowser>
#include <MatrixGeom>

#include <QVariant>
#include <QDir>

#include <sstream>

using namespace std;


/*!
    \class taBase ta_base.h
    \brief The root class for most other classes.

    \ingroup basic
    \mainclass

    \target notifications
    \section1 Change Notifications

    The taBase class provides a data update/change mechanism in conjunction
    with the taSigLink class. Note that the term "attributes" is used to
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

///////////////////////////////////////////////////////////////////////////
//      Reference counting mechanisms, all static just for consistency..

#ifdef DEBUG
void taBase::Ref(taBase& it) {
  Ref(&it);
}
void taBase::Ref(taBase* it) {
  // convenient for tracking specific leaks:
  // if(it->InheritsFrom(&TA_DataTable)) {
  //   taMisc::Info("dt ref:", String((int64_t)(void*)it), it->GetName());
  // }
  it->refn.ref();
}

void taBase::unRef(taBase* it) {
  if (it->refn <= 0) {
    taMisc::Warning("taBase::unRef: taBase refn < 0 for item");
  }
  else {
    // convenient for tracking specific leaks:
    // if(it->InheritsFrom(&TA_DataTable)) {
    //   taMisc::Info("dt unref:", String((int64_t)(void*)it), it->GetName());
    // }
    it->refn.deref();
  }
}

void taBase::Done(taBase* it) {
  if (it->refn == 0) {
    if (it->HasBaseFlag(DESTROYED))
      taMisc::Warning("taBase::Done: taBase refn == 0 but item already destroyed");
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
  Ref(it);
  it.SetOwner(onr);
  it.InitLinks();
}

void taBase::Own(taSmartRef& it, taBase* onr) {
  it.Init(onr);
}

///////////////////////////////////////////////////////////////////////////
//      Pointer management routines (all pointers should be ref'd!!)

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
//      Basic constructor/destructor ownership/initlink etc interface

void taBase::CutLinks() {
}

void taBase::InitLinks_taAuto(TypeDef* td) {
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if((md->owner != &(td->members)) || md->type->IsAnyPtr()) continue;
    if(md->is_static) continue; // no init links on statics!
    if(md->type->IsActualTaBase()) {
      taBase* mb = (taBase*)md->GetOff(this);
      taBase::Own(*mb, this);
      mb->SetName(md->name);    // always set the names of members to their member name!
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
    if(md->is_static) continue;
    if((md->owner != &(td->members)) || !md->type->IsTaBase()) continue;
    if(md->type->IsNotPtr()) {
      taBase* mb = (taBase*)md->GetOff(this);
      mb->CutLinks();
    }
    else if(md->type->IsPointer() && !md->HasNoSetPointer()) {
      taBase** mb = (taBase**)md->GetOff(this);
      taBase::DelPointer(mb);
    }
  }
}

void taBase::AutoNameMyMembers() {
  TypeDef* td = GetTypeDef();
  for(int i=0; i<td->members.size; i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->type->IsAnyPtr()) continue;
    if(md->type->IsActualTaBase()) {
      if(!md->HasOption("NO_AUTO_NAME")) {
        taBase* mb = (taBase*)md->GetOff(this);
        mb->SetName(md->name);
      }
    }
  }
}

void taBase::Register() {
  if(!taMisc::not_constr) {
    // if(GetTypeDef()->name == "taMatrixT") {
    //   taMisc::Info("MatrixT register");
    // }
    GetTypeDef()->RegisterFinal((void*)this);
  }
  SetBaseFlag(REGISTERED);
}

void taBase::unRegister() {
  // if(GetTypeDef()->name == "taMatrixT") {
  //   taMisc::Info("MatrixT attempt unregister");
  // }
  CheckDestroyed();
  if(!taMisc::not_constr && HasBaseFlag(REGISTERED)) {
    GetTypeDef()->unRegisterFinal((void*)this); ClearBaseFlag(REGISTERED);
  }
}

void taBase::InitSetDefaultName() {
  if(taMisc::is_loading || taMisc::is_duplicating) return;
  SetDefaultName();
}

void taBase::Destroy() {
#ifdef DEBUG
  SetBaseFlag(DESTROYED);
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
  // its vtable accessor for siglinks will be valid in that context
  taSigLink* dl = sig_link();
  if (dl) {
    dl->SigDestroying();
    delete dl; // NULLs our ref
  }
  if(taMisc::is_loading) {
    dumpMisc::update_after.RemoveEl(this); // justin case we're on the list!
  }
  tabMisc::RemoveFromAllDelayedLists(this); // double sure..
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
    if(!par->IsActualTaBase()) continue; // only ta-bases
    SetTypeDefaults_parents(par, scope); // first do parents of parent
    if(par->defaults != NULL)
      SetTypeDefaults_impl(par, scope);    // then actually do parent
  }
  if(ttd->defaults != NULL)
    SetTypeDefaults_impl(ttd, scope);    // then actually do it
}

void taBase::SetTypeDefaults() {
  TypeDef* ttd = GetTypeDef();  // this typedef = ttd
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
//      Object managment flags (taBase supports up to 8 flags for basic object mgmt purposes)

bool taBase::isDestroying() const {
  if(HasBaseFlag(DESTROYING)) return true;
  taBase* own = GetOwner();
  if(own && own != this) {
    if(this->IsParentOf(own)) {
      taMisc::Error("parent-child loop in:", GetName());
    }
    else {
      return own->isDestroying();
    }
  }
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
//      Basic object properties: index in list, owner, name, description, etc

String taBase::GetDisplayName() const {
  String rval = GetName();
  if (rval.nonempty()) return rval;
  // no name, so try the Owner.path name
  taBase* own = GetOwner();
  if (own) {
    rval.cat(own->GetName()).cat(GetPath(own));
    if (rval.nonempty()) return rval; // shouldn't be empty!
  }
  // last resort: (TypeName@HexAddr)
  rval = "(" + GetTypeDef()->name + "@"
    + String(QString::number((intptr_t)this, 16)) + ")";
  return rval;
}

void taBase::MakeNameUnique() {
  // nop for base -- only defined for taNBase
}

taBase* taBase::GetOwner(TypeDef* td) const {
  taBase* own = GetOwner();
  if(own == NULL)
    return NULL;
  if(own->InheritsFrom(td))
    return own;
  
  return own->GetOwner(td);
}

taProject* taBase::GetMyProj() const {
  return (taProject*)GetOwner(&TA_taProject);
}

taBase* taBase::GetThisOrOwner(TypeDef* td) const {
  if (InheritsFrom(td)) return const_cast<taBase*>(this);
  return GetOwner(td);
}

taBase* taBase::GetMemberOwner(bool highest) const {
  taBase* rval = GetOwner();
  if(!rval) return NULL;
  char* st_addr = (char*)rval;
  char* e_addr = st_addr + rval->GetTypeDef()->size;
  char* my_addr = (char*)this;
  if(my_addr < st_addr || my_addr >= e_addr) 
    return NULL;                // I don't live within my owner
  if(highest) {
    taBase* high = rval->GetMemberOwner(true);
    if(high) return high;
  }
  return rval;
}

taBase* taBase::GetParent() const {
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

void taBase::OwnTempObj() {
  taBase::Own(this, tabMisc::root); // own us!
}


void taBase::SetDefaultName_impl(int idx) {
  // name root -- use an explicit name root if any, else just the class name
  TypeDef* td = GetTypeDef();
  String nm = td->name;
  nm << "_"  << String(idx);
  SetName(nm);
}

//note: normally we dont' call SetDefaultName unless keeping tokens...
void taBase::SetDefaultName_() {
  TypeDef* td = GetTypeDef();
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

taBase::IndexMode taBase::IndexModeDecode(const Variant& idx, int cont_dims) const {
  IndexMode mode = IDX_UNK;
  if(idx.isStringType()) {
    mode = IDX_NAME;
  }
  else if(idx.isNumericStrict()) {
    mode = IDX_IDX;
  }
  else if(idx.isMatrixType()) {
    taMatrix* mat = idx.toMatrix();
    if(TestError(!mat, "IndexModeDecode", "matrix index is NULL"))
      return IDX_UNK;
    if(mat->InheritsFrom(&TA_String_Matrix)) {
      mode = IDX_NAMES;
    }
    else if(mat->InheritsFrom(&TA_byte_Matrix)) {
      mode = IDX_MASK;
    }
    else if(mat->InheritsFrom(&TA_slice_Matrix)) {
      mode = IDX_SLICE;
    }
    else if(mat->InheritsFrom(&TA_int_Matrix)) {
      if(cont_dims == 1) {
        if(mat->dims() == 1 && mat->dim(0) == 1) {
          mode = IDX_COORD;
        }
        else if(mat->dims() == 1 || (mat->dims() == 2 && mat->dim(0) == 1)) {
          mode = IDX_COORDS;
        }
        else if(mat->dims() == 2 && mat->dim(0) == 3 && mat->dim(1) == 1) {
          mode = IDX_SLICE;
        }
      }
      else {
        if(mat->dims() == 1 && mat->dim(0) == cont_dims) { // dims must be exact match
          mode = IDX_COORD;
        }
        else if(mat->dims() == 2 && mat->dim(0) == 3 &&
                (mat->dim(1) >= 1 && mat->dim(1) <= cont_dims)) {
          mode = IDX_SLICE;
        }
        else if(mat->dims() == 2 && mat->dim(0) == cont_dims) {
          mode = IDX_COORDS;
        }
        // else if(mat->dims() == 1) { // cannot infer frames -- prevents dim mismatch detection!
        //   mode = IDX_FRAMES;
        // }
        if(mode == IDX_UNK) {
          String matstr;
          mat->Print(matstr);
          TestError(true, "Index Decode",
                    "index access failed, most likely because the number of dimensions provided did not match the dimensionality of the container:", String(cont_dims), " index provided was:", matstr);
        }
      }
    }
    else {
      mode = IDX_MISC;          // typically a Variant_Matrix
    }
  }
  return mode;
}

bool taBase::IndexModeValidate(const Variant& idx, IndexMode mode, int cont_dims) const {
  switch(mode) {
  case IDX_IDX: {
    if(TestError(!idx.isNumeric(), "IndexModeValidate::IDX_IDX",
                 "index is not numeric type:",
                 idx.getTypeAsString()))
      return false;
    return true;
    break;
  }
  case IDX_NAME: {
    if(TestError(!idx.isStringType(), "IndexModeValidate::IDX_NAME/CONTAINS",
                 "index is not String type:",
                 idx.getTypeAsString()))
      return false;
    const String& nm = idx.toString();
    if(TestError(nm.empty(), "IndexModeValidate::IDX_NAME/CONTAINS",
                 "index string is empty"))
      return false;
    return true;
    break;
  }
  case IDX_NAMES: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_NAMES",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    String_Matrix* cmat = dynamic_cast<String_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_NAMES",
                 "index matrix is NULL or not a String_Matrix"))
      return false;
    return true;
    break;
  }
  case IDX_COORD: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_COORD",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_COORD",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(TestError(cmat->dims() != 1, "IndexModeValidate::IDX_COORD",
                 "index matrix does not have dims = 1"))
      return false;
    if(TestError(cmat->dim(0) != cont_dims, "IndexModeValidate::IDX_COORD",
                 "index matrix dim(0):", String(cmat->dim(0)),
                 "is not size of container coordinates:", String(cont_dims)))
      return false;
    return true;
    break;
  }
  case IDX_COORDS: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_COORDS",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_COORDS",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(cont_dims == 1 && cmat->dims() == 1) {
      // empty is ok..
      // if(TestError(cmat->size == 0, "IndexModeValidate::IDX_COORDS",
      //                   "index matrix is empty -- no coordinates"))
      //        return false;
      if(TestError(cmat->dim(0) != cont_dims, "IndexModeValidate::IDX_COORDS",
                   "index matrix dim(0):", String(cmat->dim(0)),
                   "is not size of container coordinates:", String(cont_dims)))
        return false;

    }
    else {
      if(TestError(cmat->dims() != 2, "IndexModeValidate::IDX_COORDS",
                   "index matrix does not have dims = 2"))
        return false;
      if(TestError(cmat->dim(0) != cont_dims, "IndexModeValidate::IDX_COORDS",
                   "index matrix dim(0):", String(cmat->dim(0)),
                   "is not size of container coordinates:", String(cont_dims)))
        return false;
    }
    return true;
    break;
  }
  case IDX_FRAMES: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_FRAMES",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_FRAMES",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(TestError(cmat->dims() != 1, "IndexModeValidate::IDX_FRAMES",
                 "index matrix is not 1 dimensional"))
      return false;
    return true;
    break;
  }
  case IDX_SLICE: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_SLICE",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_SLICE",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(TestError(cmat->dims() != 2, "IndexModeValidate::IDX_SLICE",
                 "index matrix does not have dims = 2"))
        return false;
    if(TestError(cmat->dim(0) != 3, "IndexModeValidate::IDX_SLICE",
                 "index matrix dim(0):", String(cmat->dim(0)), "is not = 3"))
      return false;
    if(TestError(cmat->dim(1) != cont_dims, "IndexModeValidate::IDX_SLICE",
                 "index matrix dim(1):", String(cmat->dim(1)),
                 "is not size of container coordinates:", String(cont_dims)))
      return false;
    return true;
    break;
  }
  case IDX_MASK: {
    if(TestError(!idx.isMatrixType(), "IndexModeValidate::IDX_MASK",
                 "index is not taMatrix type:",
                 idx.getTypeAsString()))
      return false;
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(idx.toMatrix());
    if(TestError(!cmat, "IndexModeValidate::IDX_MASK",
                 "index matrix is NULL or not a byte_Matrix"))
      return false;
    if(TestError(cmat->dims() != cont_dims, "IndexModeValidate::IDX_MASK",
                 "index matrix dims:", String(cmat->dims()),
                 "is not size of container coordinates:", String(cont_dims)))
      return false;
    // todo: needs further checks on cont dims
    return true;
    break;
  }
  case IDX_MISC: {
    return true;                // nothing to do here
    break;
  }
  case IDX_UNK:
    break;
  }
  return false;
}

int taBase::IterCount() const {
  if(!ElView()) return ElemCount();
  IndexMode vmd = ElViewMode();
  if(vmd == IDX_COORDS) {
    return ElView()->Frames();  // outer dim value
  }
  else if(vmd == IDX_FRAMES) {
    return ElView()->size;
  }
  else if(vmd == IDX_MASK) {
    byte_Matrix* bmat = (byte_Matrix*)ElView();
    int mx = MIN(bmat->size, ElemCount());
    int sum = 0;
    for(int i=0; i<mx; i++) {
      if(bmat->FastEl_Flat(i) > 0) sum++;
    }
    return sum;
  }
  return 0;                     // nothing else supported by this base guy
}

bool taBase::IterValidate(taMatrix* vmat, IndexMode mode, int cont_dims) const {
  if(!vmat) return true;
  if(mode == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(vmat);
    if(TestError(!cmat, "IterValidate::IDX_COORDS",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(cont_dims == 1 && cmat->dims() == 1) {
    }
    else {
      if(TestError(cmat->dims() != 2, "IterValidate::IDX_COORDS",
                   "index matrix does not have dims = 2"))
        return false;
      if(TestError(cmat->dim(0) < cont_dims, "IterValidate::IDX_COORDS",
                   "index matrix dim(0):", String(cmat->dim(0)),
                   "is not size of container coordinates:", String(cont_dims)))
        return false;
    }
  }
  else if(mode == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(vmat);
    if(TestError(!cmat, "IterValidate::IDX_COORDS",
                 "index matrix is NULL or not an int_Matrix"))
      return false;
    if(TestError(cmat->dims() != 1, "IndexModeValidate::IDX_FRAMES",
                 "index matrix is not 1 dimensional"))
      return false;
  }
  else if(mode == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(vmat);
    if(TestError(!cmat, "IterValidate::IDX_MASK",
                 "index matrix is NULL or not a byte_Matrix"))
      return false;
    if(TestError(cmat->dims() != cont_dims, "IterValidate::IDX_MASK",
                 "index matrix dims:", String(cmat->dims()),
                 "is not size of container coordinates:", String(cont_dims)))
      return false;
    if(ElViewMode() == IDX_MASK) {
      if(TestError(cmat->size != ElemCount(), "IterValidate::IDX_MASK",
                   "el_view size:", String(cmat->size), "not equal to size of list:",
                   String(ElemCount())))
        return false;
    }
  }
  else {
    TestError(true, "IterValidate", "view mode must be either IDX_COORDS, IDX_MASK, or IDX_FRAMES");
    return false;
  }
  return true;
}

Variant taBase::IterFirst(taBaseItr& itr) const {
  if(IterFirst_impl(itr)) return IterElem(itr);
  return _nilVariant;
}

Variant taBase::IterNext(taBaseItr& itr) const {
  if(IterNext_impl(itr)) return IterElem(itr);
  return _nilVariant;
}

int taBase::IterNextIndex(taBaseItr& itr) const {
  if(IterNext_impl(itr))
    return itr.el_idx;
  return -1;
}

int taBase::IterFirstIndex(taBaseItr& itr) const {
  if(IterFirst_impl(itr))
    return itr.el_idx;
  return -1;
}

bool taBase::IterFirst_impl(taBaseItr& itr) const {
  itr.count = 0;
  itr.el_idx = 0;              // just to be sure
  if(!ElView()) {
    if(ElemCount() > 0) return true;
    itr.SetDone();
    return false;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(0); // first guy
    if(itr.el_idx < 0) itr.el_idx += ElemCount();
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(0); // first guy
    if(itr.el_idx < 0) itr.el_idx += ElemCount();
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=0; i<ElemCount(); i++) {
      if(cmat->FastEl_Flat(i)) {
        itr.el_idx = i;
        return true;            // byte_matrix guaranteed to be same size as list
      }
    }
  }
  itr.SetDone();
  return false;
}

bool taBase::IterNext_impl(taBaseItr& itr) const {
  itr.count++;
  if(!ElView()) {
    itr.el_idx++;
    if(itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size <= itr.count) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(itr.count); // next guy
    if(itr.el_idx < 0) itr.el_idx += ElemCount();
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size <= itr.count) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(itr.count); // next guy
    if(itr.el_idx < 0) itr.el_idx += ElemCount();
    if(itr.el_idx < 0 || itr.el_idx >= ElemCount()) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=itr.el_idx+1; i<ElemCount(); i++) { // search for next
      if(!cmat || cmat->FastEl_Flat(i)) { // true
        itr.el_idx = i;     // byte_matrix guaranteed to be same size as list
        return true;
      }
    }
  }
  itr.SetDone();
  return false;
}

//////////////////////////////////////
//      Reverse iterator functions

Variant taBase::IterLast(taBaseItr& itr) const {
  if(IterLast_impl(itr)) return IterElem(itr);
  return _nilVariant;
}

Variant taBase::IterPrev(taBaseItr& itr) const {
  if(IterPrev_impl(itr)) return IterElem(itr);
  return _nilVariant;
}

int taBase::IterPrevIndex(taBaseItr& itr) const {
  if(IterPrev_impl(itr))
    return itr.el_idx;
  return -1;
}

int taBase::IterLastIndex(taBaseItr& itr) const {
  if(IterLast_impl(itr))
    return itr.el_idx;
  return -1;
}

bool taBase::IterLast_impl(taBaseItr& itr) const {
  itr.count = 0;
  itr.el_idx = 0;              // just to be sure
  const int ec = ElemCount();
  if(!ElView()) {
    if(ec > 0) {
      itr.el_idx = ec-1;
      return true;
    }
    itr.SetDone();
    return false;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(cmat->size-1); // last guy
    if(itr.el_idx < 0) itr.el_idx += ec;
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(cmat->size-1); // last guy
    if(itr.el_idx < 0) itr.el_idx += ec;
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=ec-1; i>=0; i--) {
      if(cmat->FastEl_Flat(i)) {
        itr.el_idx = i;
        return true;            // byte_matrix guaranteed to be same size as list
      }
    }
  }
  itr.SetDone();
  return false;
}

bool taBase::IterPrev_impl(taBaseItr& itr) const {
  itr.count++;
  const int ec = ElemCount();
  if(!ElView()) {
    itr.el_idx--;
    if(itr.el_idx < 0) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size <= itr.count) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(cmat->size - 1 - itr.count); // prev guy
    if(itr.el_idx < 0) itr.el_idx += ec;
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_FRAMES) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(!cmat || cmat->size <= itr.count) {
      itr.SetDone();
      return false;
    }
    itr.el_idx = cmat->FastEl_Flat(cmat->size - 1 - itr.count); // prev guy
    if(itr.el_idx < 0) itr.el_idx += ec;
    if(itr.el_idx < 0 || itr.el_idx >= ec) {
      itr.SetDone();
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=ec - 2 - itr.el_idx; i>=0; i--) { // search for prev
      if(!cmat || cmat->FastEl_Flat(i)) { // true
        itr.el_idx = i;     // byte_matrix guaranteed to be same size as list
        itr.SetDone();
        return true;
      }
    }
  }
  return false;
}

bool taBase::FixSliceValsFromSize(int& start, int& end, int sz) const {
  if(start < 0)start += sz;
  if(end <= 0) end += sz; // for 8.0: 0 is end for negative range! only logical case.
  if(start > sz-1) start = sz-1; // keep in bounds
  if(end > sz) end = sz;
  if(TestError(end < start, "Slice Vals",
               "slice end is before start.  start:", String(start),
               "end:", String(end)))
    return false;
  return true;
}

///////////////////////////////////////////////////////////////////////////
//      Paths in the structural hierarchy

String taBase::GetPath_impl(taBase* ta, taBase* par_stop) const {
  if ((this == par_stop) && (ta == NULL))
    return ".";

  String rval;
  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) {
      rval = "root";
    }
  }
  else if (this != par_stop) {
    rval = par->GetPath_impl((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    MemberDef* md = NULL;
    if ((md = FindMemberBase(ta))) {
      rval += "." + md->name;
    }
    else if ((md = FindMemberPtr(ta))) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      rval += ".?.";
    }
  }
  return rval;
}

String taBase::GetPathNames_impl(taBase* ta, taBase* par_stop) const {
  if (taMisc::is_undo_saving) return GetPath_impl(ta, par_stop); // use indexes for undo

  if ((this == par_stop) && (ta == NULL))
    return ".";

  String rval;
  taBase* par = GetOwner();
  if (par == NULL) {
    if (ta == NULL) {
      rval = "root";
    }
  }
  else if (this != par_stop) {
    rval = par->GetPathNames_impl((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    MemberDef* md = NULL;
    if ((md = FindMemberBase(ta))) {
      rval += "." + md->name;
    }
    else if ((md = FindMemberPtr(ta))) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      rval += ".?.";
    }
  }
  return rval;
}

String taBase::GetPathFromProj() const {
  taProject* proj = (taProject*)GetThisOrOwner(&TA_taProject);
  return GetPathNames(proj);
}

String taBase::DisplayPath() const {
  // todo: could generalize with a list of special owner types..
  Program* prog_own = (Program*)GetOwner(&TA_Program);
  if(prog_own) {
    return String(".") + prog_own->name + GetPathNames(prog_own);
  }
  return GetPathFromProj();
}

taBase* taBase::ElemPath(const String& path, TypeDef* expect_type, bool err_msg) const {
  MemberDef* md;
  taBase* rval = FindFromPath(path, md);
  TestError(err_msg && !rval, "ElemPath",
            "path not found under this object:", path,
            "the program may crash after this point -- if so, please report as a bug to developers using Report Bug button on toolbar");
  if(rval && expect_type && !rval->InheritsFrom(expect_type)) {
    TestError(err_msg, "ElemPath",
              "path returned an object of a different type than expected. path:", path,
              "expected type:", expect_type->name, "actual type:", rval->GetTypeDef()->name,
              "the program may crash after this point -- if so, please report as a bug to developers using Report Bug button on toolbar");
    rval = NULL;
  }
  return rval;
}

taBase* taBase::FindFromPath(const String& path, MemberDef*& ret_md, int start) const {
  String effective_path = path;
  if(((int)effective_path.length() <= start) || (effective_path == ".")) {
    ret_md = NULL;
    return (taBase*)this;
  }
  if((effective_path == "Null") || (effective_path == "NULL")) {
    ret_md = NULL;
    return NULL;
  }

  // taMisc::DebugInfo("ffp:", effective_path.from(start));
  
  taBase* rval = NULL;
  bool ptrflag = false;
  int length = effective_path.length();

  while(start < length) {
    if(effective_path[start] == '*') {    // path is a pointer
      start += 2;               // assumes "*("
      ptrflag = true;
      if (effective_path[length-1] == ')') {
        effective_path = effective_path.before(length-1);  // remove the trailing parens
      }
    }
    if(effective_path[start] == '.') {    // must be root, so search on next stuff
      start++;
      continue;
    }

    int delim_pos = taBase::GetNextPathDelimPos(effective_path, start);
    String el_path = effective_path(start,delim_pos-start); // element is between start and delim
    int next_pos = delim_pos+1;
    if((delim_pos < length) && (effective_path[delim_pos] == '['))
      next_pos--;

    MemberDef* md;
    void* tmp_ptr = FindMembeR(el_path, md);
    ret_md = md;                // default
    // if(md) {
    //   taMisc::DebugInfo("elp:", el_path, String(tmp_ptr), md->name);
    // }
    // else {
    //   taMisc::DebugInfo("elp:", el_path, String(tmp_ptr));
    // }      
    if (tmp_ptr && md && md->type->IsPointer()) {
      if (!md->type->IsTaBase() && md->type->IsClass()) {
        return NULL;            // must be tabase
      }
      taBase** mbr = (taBase**)tmp_ptr;
      rval = *mbr;
    }
    if (tmp_ptr && md && md->type->InheritsFrom(TA_taSmartRef)) {
      taSmartRef* ref = (taSmartRef*)tmp_ptr;
      rval = ref->ptr();
      if(rval && delim_pos < length) {  // there's more to be done..
        rval = rval->FindFromPath(effective_path, ret_md, next_pos); // start from after delim
      }
    }
    else if (tmp_ptr && md && md->type->InheritsFrom(TA_taSmartPtr)) {
      taSmartPtr* ref = (taSmartPtr*)tmp_ptr;
      rval = ref->ptr();
      if(rval && delim_pos < length) {  // there's more to be done..
        rval = rval->FindFromPath(effective_path, ret_md, next_pos); // start from after delim
      }
    }
    else if(tmp_ptr && (!md || md->type->IsActualTaBase())) { // null md = taBase
      taBase* mbr = (taBase*)tmp_ptr;
      if(delim_pos < length) {  // there's more to be done..
        rval = mbr->FindFromPath(effective_path, ret_md, next_pos); // start from after delim
      }
      else {
        rval = mbr;             // that's all folks..
        ret_md = md;
        // if(ret_md) {
        //   taMisc::DebugInfo("done:", String(rval), ret_md->name);
        // }
        // else {
        //   taMisc::DebugInfo("done:", String(rval));
        // }
      }
    }
    else if((el_path == "root") && (delim_pos < length)) {
      start = next_pos; // skip this element since it must be us
      continue;
    }
    else if(this->InheritsFrom(&TA_taGroup_impl)) {
      taGroup_impl* group = (taGroup_impl*)this;
      for (int i=0; i<group->gp.size; i++) {
        taGroup_impl* sub_group = group->gp.FastEl(i);
        if (sub_group->GetName() == el_path) {
          rval = sub_group;
          if(delim_pos < length) {  // there's more to be done..
            rval = rval->FindFromPath(effective_path, ret_md, next_pos); // start from after delim
          }
        }
      }
    }
    if((ptrflag) && (rval != NULL)) {
      taBase* prval = *((taBase* *)rval);
      // taMisc::DebugInfo("ptr non-null:", String(rval), String(prval));
      return prval;
    }
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
    if(eff_typ->IsActualTaBase()) {
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
  int point_idx = taMisc::find_not_in_quotes(path, '.', start+1); // skip any possible starting delim
  int brack_idx = taMisc::find_not_in_quotes(path, '[', start+1);

  // if there is a period but not a bracket, or the period is before the bracket
  if(((brack_idx < start) && (point_idx >= start)) ||
     ((point_idx < brack_idx ) && (point_idx >= start)))
  {
    return point_idx;
  }
  else if(brack_idx >= start) {         // else try the bracket
    return brack_idx;
  }
  return path.length();                 // delimiter is end of string (its all element)
}

int taBase::GetLastPathDelimPos(const String& path) {
  int point_idx = taMisc::find_not_in_quotes(path, '.',-1);
  int brack_idx = taMisc::find_not_in_quotes(path, '[',-1);

  if(point_idx > brack_idx) {           // point comes after bracket
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
    scp_tp = taMisc::FindTypeName(scp_nm);
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
  if(GetOwner() == NULL) return false; // un-owned never in scope

  if(GetOwner() == &tabMisc::root->templates) // specifically exclude templates!
    return false;
  
  if (!ref_obj)
    return true;
  if (!scp_tp)
    scp_tp = GetScopeType();
  if (!scp_tp)
    return true;

  taBase* my_scp = GetOwner(scp_tp);
  if(!my_scp) {
    if(scp_tp == taMisc::default_scope) {
      return true;              // fully outside of scope -- could be a global of some sort -- include
    }
    else { // try again with default scope if outside of narrower scope
      return SameScope(ref_obj, taMisc::default_scope);
    }
  }

  if ((my_scp == ref_obj) || (my_scp == ref_obj->GetOwner(scp_tp)))
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
//      Saving and Loading to/from files

String taBase::GetFileNameFmProject(const String& ext, const String& tag, const String& subdir, bool dmem_proc_no) {
  taProject* proj = GetMyProj();
  if(!proj) return _nilString;
  String proj_base_nm = proj->name; // use the actual name which should be what was saved
  // and thus doesn't change when we use cluster run or something else that might use
  // a different filename
  if(proj_base_nm.contains(tag)) // this should be unnecessary but just in case..
    proj_base_nm = proj_base_nm.before(tag,-1);
  String base_dir = taMisc::NoFinalPathSep(proj->proj_dir);
  String fnm = taMisc::GetFileFmPath(proj_base_nm);
  // this is special support for the ClusterRun system -- checks for 
  // a ../results directory if we're in a ../models directory, and uses that
  // if this is true, we ignore the subdir thing because it will get in the way
  bool clust_run = false;
  if(base_dir.endsWith("/models")) {
    String oneup = taMisc::GetDirFmPath(base_dir);
    String resultspath = oneup + taMisc::path_sep + "results";
    if(taMisc::DirExists(resultspath)) {
      base_dir = resultspath;
      clust_run = true;
    }
  }
  if(!clust_run) {
    if(subdir.nonempty()) {
      if(base_dir.nonempty())
        base_dir = base_dir + taMisc::path_sep + subdir;
      else
        base_dir = subdir;
    }
  }
  taMisc::MakePath(base_dir);   // make sure path exists!
  String dms;
  if(dmem_proc_no && (taMisc::dmem_nprocs > 1)) {
    dms = ".p" + taMisc::LeadingZeros(taMisc::dmem_proc, 2);
  }
  String rval = base_dir + taMisc::path_sep + fnm + tag + dms + ext;
  return rval;
}

String taBase::GetProjDir() const {
  taProject* proj = GetMyProj();
  if(!proj) return _nilString;
  return proj->proj_dir;
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
  StructUpdate(false);
  SigEmit(SLS_STRUCT_UPDATE_ALL); // during loading, updates are blocked, so now we redo everything
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
    }
    else { // otherwise, loading over an item, so just get from us
      AppendFilerInfo(typ, exts, compress, filetypes);
    }
  }
  taFiler* flr = StatGetFiler(NULL, exts, compress, filetypes);
  taRefN::Ref(flr);

  taProject* proj = GetMyProj();

  if(fname.nonempty()) {
    String filename(fname);
    filename.trim();
    flr->SetFileName(taMisc::ExpandFilePath(filename, proj));
    flr->open_read();
  }
  else {
    String tfname;
    if (getset_file_name)
      tfname = GetFileName();
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(taMisc::ExpandFilePath(tfname, proj)); // filer etc. does auto extension
    flr->Open();
  }
  if(flr->istrm && getset_file_name) {
    //    SetFileName(taMisc::CompressFilePath(flr->FileName(), proj));
    SetFileName(flr->FileName()); // now that compress working, not a good idea..
  }
  return flr;
}

int taBase::Load_cvt(taFiler*& flr) {
  int c = taMisc::read_till_eol(*flr->istrm);
  if(c == EOF) return false;    // bail
  if(!taMisc::LexBuf.contains("// ta_Dump File v1.0")) {
    flr->Close();
    flr->open_read();
    return true;                // continue
  }
  // first determine file type
  String_PArray key_strs;
  for(int i=0;i<taMisc::file_converters.size;i++) {
    DumpFileCvt* cvt = taMisc::file_converters[i];
    key_strs.Add(cvt->key_srch_str);
  }
  int typ_id = taMisc::find_strings(*flr->istrm, key_strs);
  flr->Close();                 // file is done with anyway
  flr->open_read();             // read again in any case
  if(TestWarning((typ_id < 0), "Load_cvt",
                 "Old format file could not be identified; not converting!")) return false;
  DumpFileCvt* cvt = taMisc::file_converters[typ_id];
  taFiler* cvt_flr = taFiler::New(flr->filetype, flr->defExt());
  taRefN::Ref(cvt_flr);
  String cvt_fname = flr->FileName();
  QDir::setCurrent(taMisc::GetDirFmPath(cvt_fname));
  String cvt_tag = "_v4precvt";
  if(!flr->defExt().empty()) {
    if (cvt_fname.contains(flr->defExt())) {
      cvt_fname = cvt_fname.before(flr->defExt());
    }
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
  taRefN::unRefDone(flr);       // get rid of orig filer
  flr = cvt_flr;                // use new one
  flr->open_read();             // read the converted file
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
    if(Load_cvt(flr)) {         // do conversion if needed
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
  int compress, String filetypes, bool getset_file_name, bool make_copy)
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

  taProject* proj = GetMyProj();

  if (fname.nonempty()) {
    flr->SetFileName(taMisc::ExpandFilePath(fname, proj));
    flr->FixFileName();
    flr->Save();
  }
  else {
    String tfname;
    // if (getset_file_name)
      tfname = GetFileName();   // always get, just don't set
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(taMisc::ExpandFilePath(tfname, proj)); // filer etc. does auto extension
    flr->SaveAs(true, make_copy);
  }

  if (flr->ostrm && getset_file_name) {
    //    SetFileName(taMisc::CompressFilePath(flr->FileName(), proj));
    SetFileName(flr->FileName()); // now that compress working, not a good idea..
    // don't notify! very dangerous in middle of save, and also marks Dirty
   // SigEmitUpdated();
  }
  return flr;
}

taFiler* taBase::GetAppendFiler(const String& fname, const String& ext, int compress,
  String filetypes, bool getset_file_name)
{
  taFiler* flr = GetFiler(NULL, ext, compress, filetypes);
  taRefN::Ref(flr);

  taProject* proj = GetMyProj();

  if (fname.nonempty()) {
    flr->SetFileName(taMisc::ExpandFilePath(fname, proj));
    flr->open_append();
  }
  else {
    String tfname;
    if (getset_file_name)
      tfname = GetFileName();
    if (tfname.empty())
      tfname = GetName();
    flr->SetFileName(taMisc::ExpandFilePath(tfname, proj)); // filer etc. does auto extension
    flr->Append();
  }

  if(flr->ostrm && getset_file_name) {
    //    SetFileName(taMisc::CompressFilePath(flr->FileName()));
    SetFileName(flr->FileName()); // now that compress working, not a good idea..
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
  SigEmit(SLS_ITEM_UPDATED_ND);
  return rval;
}


int taBase::Save_String(String& save_str, taBase* par, int indent) {
  ostringstream oss;
  int rval = Save_strm(oss, par,indent);
  string str = oss.str();
  save_str.set(str.c_str(), str.length());
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
                    el_path,"in parent object:",DisplayPath());
    return NULL;
  }
  if(!el_md) {
    taMisc::Warning("*** Dump_Load_Path_ptr: no el_md for item at path:",
                    el_path,"in parent object:",DisplayPath(),
                    "may not set pointers correctly!");
  }
  taBase* nw_el = *nw_el_ptr;

  if(nw_el && (nw_el->GetTypeDef() != ld_el_typ) &&
     !((nw_el->GetTypeDef() == &TA_taBase_List) && (ld_el_typ == &TA_taBase_Group)))
  {
    // object not the right type, try to create new one..
    if(taMisc::verbose_load >= taMisc::MESSAGES) {
      taMisc::Warning("*** Object in parent:",DisplayPath(),"at path", el_path,
                    "of type:",nw_el->GetTypeDef()->name,"is not the right type:",
                      ld_el_typ->name,", attempting to create new one");
    }
    if(el_md && (el_md->HasOwnPointer() || !el_md->HasNoSetPointer())) {
      taBase::DelPointer(nw_el_ptr);
    }
    else {
      nw_el_ptr = NULL;
    }
    nw_el = NULL;               // nuked it
  }

  if(!nw_el) {
    *nw_el_ptr = taBase::MakeToken(ld_el_typ);
    nw_el = *nw_el_ptr;
    if(!nw_el) {
      taMisc::Warning("*** Dump_Load_Path_ptr: Could not make new token of type:",
                      ld_el_typ->name,"for pointer member at path:",
                      el_path,"in parent object:",DisplayPath());
      return NULL;
    }
    if(el_md && el_md->HasOwnPointer()) { // note: this was not in original!
      taBase::Own(nw_el,this);
    }
    else {
      taMisc::Warning("*** Dump_Load_Path_ptr: NOT owning new element of type:",
                      ld_el_typ->name,"for pointer member at path:",
                      el_path,"in parent object:",DisplayPath());
    }
  }
  if(nw_el && el_path.contains('\"')) {
    String elnm = el_path.before('\"',-1);
    elnm = elnm.after('\"',-1);
    if(elnm.nonempty()) {
      nw_el->SetName(elnm);
    }
  }

  if(taMisc::verbose_load >= taMisc::TRACE) {
    String msg;
    msg << "Success: Leaving TypeDef::Dump_Load_Path_ptr, type: " << ld_el_typ->name
        << ", parent path = " << DisplayPath()
        << ", el_path = " << el_path;
    taMisc::Info(msg);
  }
  return nw_el;
}

taBase* taBase::Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ) {
  MemberDef* el_md = NULL;
  taBase* nw_el = (taBase*)FindMembeR(el_path, el_md);
  if(nw_el) return nw_el;
  if(taMisc::verbose_load != taMisc::QUIET) {
    taMisc::Warning("*** Dump_Load_Path_parent: Object at path:",DisplayPath(),
                    "is not capable of creating a new element with the path:",el_path,
                    "of type:",ld_el_typ->name,
                    "something is askew in the loading paths");
  }
  return NULL;
}

String taBase::GetValStr(void* par, MemberDef* memb_def, TypeDef::StrContext sc,
                         bool force_inline) const {
  if (sc == TypeDef::SC_DEFAULT)
    sc = (taMisc::is_saving) ? TypeDef::SC_STREAMING : TypeDef::SC_VALUE;

  TypeDef* td = GetTypeDef();
  if(force_inline || td->IsSaveInline()) {
    return td->GetValStr_class_inline(this, par, memb_def, sc, force_inline);
  }
  else {
    if(GetOwner() || this == tabMisc::root)
      return GetPathFromProj();
    return td->name;
  }
}

String taBase::GetValStr_ptr(const TypeDef* td, const void* base, void* par, MemberDef* memb_def,
                             TypeDef::StrContext sc, bool force_inline) {
  taBase* rbase = *((taBase**)base);
  if(rbase && (rbase->GetOwner() || (rbase == tabMisc::root))) {
    switch(sc) {
    case TypeDef::SC_STREAMING:
      return dumpMisc::path_tokens.GetPath(rbase);      // use path tokens when saving..
    case TypeDef::SC_DISPLAY:
    case TypeDef::SC_SEARCH:
      return rbase->GetName();
    default:
      return rbase->GetPathFromProj();
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

  TypeDef* td = GetTypeDef();
  if(force_inline || td->IsSaveInline()) {
    td->SetValStr_class_inline(val, this, par, memb_def, sc, force_inline);
    if (sc != TypeDef::SC_STREAMING)
      UpdateAfterEdit();        // only when not loading (else will happen after)
    return true;
  }
  return false;                 // not processed otherwise..
}

bool taBase::SetValStr_ptr(const String& val, TypeDef* td, void* base, void* par,
                           MemberDef* memb_def, TypeDef::StrContext sc, bool force_inline) {
  taBase* bs = NULL;
  if((val != String::con_NULL) && (val != "Null")) {
    String tmp_val(val); // FindFromPath can change it
    if (sc == TypeDef::SC_STREAMING) {
      bs = dumpMisc::path_tokens.FindFromPath(tmp_val, td, base, par, memb_def);
      if(!bs)   // indicates error condition
        return false;
    }
    else {
      MemberDef* md = NULL;
      bs = tabMisc::RootFindFromPath(tmp_val, md); // not streaming, should be safe..
      if(!bs) {
        taMisc::Warning("*** Invalid Path in SetValStr:",val);
        return false;
      }
      if(md) {                  // otherwise it is a taBase*
        if(md->type->IsPointer()) {
          bs = *((taBase**)bs);
          if(bs == NULL) {
            taMisc::Warning("*** Null object at end of path in SetValStr:",val);
            return false;
          }
        }
        else if(md->type->IsPtrPtr()) {
          taMisc::Warning("*** ptr count != 1 in path:", val);
          return false;
        }
      }
    }
  }
  if (memb_def && memb_def->HasOwnPointer()) {
    if(!par)
      taMisc::Warning("*** NULL parent for owned pointer:",val);
    else
      taBase::OwnPointer((taBase**)base, bs, (taBase*)par);
  }
  else {
    if (memb_def && memb_def->HasNoSetPointer())
      (*(taBase**)base) = bs;
    else
      taBase::SetPointer((taBase**)base, bs);
  }
  return true;
}

int taBase::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                          void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc, bool replace_deep) {
  TypeDef* td = GetTypeDef();
  int rval = td->ReplaceValStr_class(srch, repl, mbr_filt, this, par, par_typ, memb_def, sc, replace_deep);
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

FlatTreeEl* taBase::GetFlatTree(FlatTreeEl_List& ftl, int nest_lev, FlatTreeEl* par_el,
                                const taBase* par_obj, MemberDef* md) const {
  FlatTreeEl* fel = NULL;
  if(md) {
    fel = ftl.NewMember(nest_lev, md, par_obj, par_el);
  }
  else {
    fel = ftl.NewObject(nest_lev, this, par_el);
  }
  GetFlatTreeValue(ftl, fel);   // get our value
  ftl.GetFlatTreeMembers(fel, this); // default gets reps of members based on this obj
  return fel;
}

void taBase::GetFlatTreeValue(FlatTreeEl_List& ftl, FlatTreeEl* ft, bool ptr) const {
  // NOTE: overrides typically refer back to here for ptr case, and provide a full-value
  // rep in the non-ptr case -- those caes should skip the GetFlatTreeMembers call
  // in GetFlatTree!
  // note: can also override default sizeof size here..
  if(ptr) {
    if(GetOwner() || (this == tabMisc::root)) {
      if(IsChildOf(ftl.top_obj.ptr())) {
        ft->value = GetPathNames(ftl.top_obj.ptr()); // scope by top obj
        ft->SetBaseFlag(BF_MISC2);                   // indicates relative path
      }
      else {
        ft->value = DisplayPath();
      }
    }
  }
  else {
    if(ft->mdef) {    // for inline member objects, just use type name -- pointer useless
      ft->value = ft->type->name;
    }
    else if(ft->nest_level == 0) { // don't use name for top level guy
      ft->value = ft->type->name;
    }
    else {
      ft->value = ft->type->name + ": " + GetDisplayName(); // this is us..
    }
  }
}

///////////////////////////////////////////////////////////////////////////
//      Updating of object properties

void taBase::UpdateAfterEdit() {
  if (isDestroying()) return;
  UpdateAfterEdit_impl();
  if(isDestroying()) return;    // could have decided to destroy during UAE
  SigEmitUpdated();
  taBase* _owner = GetOwner();
  if (_owner ) {
    bool handled = false;
    _owner->ChildUpdateAfterEdit(this, handled);
  }
}

void taBase::UpdateAfterEdit_NoGui() {
  if (isDestroying()) return;
  UpdateAfterEdit_impl();
}

void taBase::UpdateAll() {
  UpdateAfterEdit();
  TypeDef* td = GetTypeDef();
  for (int i=0;i<td->members.size;i++) {
    MemberDef* md = td->members.FastEl(i);
    if(md->HasNoSave() || !md->type->IsActualTaBase()) continue;
    if(md->type->HasOption("NO_UPDATE_AFTER")) continue;
    taBase* md_tab = (taBase*)md->GetOff(this);
    md_tab->UpdateAll();
  }
}

void taBase::ChildUpdateAfterEdit(taBase* child, bool& handled) {
  if (handled)
    return; // note: really shouldn't have been handled already if we are called...
  
  // only notify and UAE if it is an owned member object (but not list/group items)
  taBase* mo = child->GetMemberOwner(false);
  if(mo == this) {
    handled = true;
    SigEmit(SLS_CHILD_ITEM_UPDATED); // this is trapped by some..
    UpdateAfterEdit();          // if a child has been updated, we need to get parent notified.. -- if this has a parent, it will kick up to it too..
  }
}

void taBase::UpdatePointersAfterCopy_(const taBase& cp) {
  if (taMisc::is_loading) {
    return;
  }
  taBase* owner = GetOwner();
  if (owner && owner != tabMisc::root) {
    if (!owner->HasBaseFlag(COPYING)) {
      UpdatePointers_NewPar((taBase*)&cp, this); // update any pointers within this guy
    }
  }
  // for work we can't handle generically
  UpdatePointersAfterCopy_impl(cp);
}

void taBase::UpdateAfterMove(taBase* old_owner) {
  taBase* owner = GetOwner();
  if (owner && old_owner) {
    UpdatePointers_NewPar(old_owner, owner); // update any pointers within this guy
  }
  UpdateAfterMove_impl(old_owner);
}

void taBase::UpdateAllViews() {
  if(taMisc::gui_active)
    SigEmit(SLS_UPDATE_VIEWS);
}

void taBase::RebuildAllViews() {
  if(taMisc::gui_active)
    SigEmit(SLS_REBUILD_VIEWS);
}

void taBase::SigEmit(int sls, void* op1, void* op2) {
  if(taMisc::is_loading)  return; // no notifies while loading!!
  if(!tabMisc::root || tabMisc::root->in_init) return;
  if(!taTaskThread::inMainThread()) return;
  
  if (sls != SLS_ITEM_UPDATED_ND)
    setDirty(true); // note, also then sets dirty for list ops, like Add etc.
  // only assume stale for strict condition:
  if ((useStale() && (sls == SLS_ITEM_UPDATED)))
    setStale();
  taSigLink* dl = sig_link();
  if (dl) dl->SigLinkEmit(sls, op1, op2);
}

void taBase::SigEmitUpdated() {
  SigEmit(SLS_ITEM_UPDATED);
}

bool taBase::InStructUpdate() {
  taSigLink* dl = sig_link(); // doesn't autocreate
  return (dl ? (dl->dbuCnt() > 0) : false);
}

void taBase::setDirty(bool value) {
  //note: base has no storage, and only forwards dirty (not !dirty)
  if (!value) return;
  if (taBase* owner = GetOwner()) {
    owner->setDirty(value);
  }
}

void taBase::setStale() {
  if (taBase* owner = GetOwner()) {
    owner->setStale();
  }
}

void taBase::SetMember(const String& member, const String& value) {
  TypeDef* td = GetTypeDef();

  ta_memb_ptr mbr_off = 0;
  int base_off = 0;
  MemberDef* mbr_def = TypeDef::FindMemberPathStatic(td, base_off, mbr_off,
                                                 member, false); // no warn
  if(mbr_def && !mbr_def->IsGuiReadOnly()) {
    void* address = MemberDef::GetOff_static(this, base_off, mbr_off);
    mbr_def->type->SetValStr(value, address, NULL, mbr_def);
    UpdateAfterEdit();
  }
}

String taBase::GetMemberStrVal(const String& member) const {
  TypeDef* td = GetTypeDef();

  ta_memb_ptr mbr_off = 0;
  int base_off = 0;
  MemberDef* mbr_def = TypeDef::FindMemberPathStatic(td, base_off, mbr_off,
                                                 member, true); // warn!
  if(mbr_def) {
    void* address = MemberDef::GetOff_static(this, base_off, mbr_off);
    return mbr_def->type->GetValStr(address, NULL, mbr_def);
  }
  return _nilString;
}

///////////////////////////////////////////////////////////////////////////
//      Data Links -- notify other guys when you change

taSigLink* taBase::GetSigLink() {
  if (!sig_link()) {
    if (isDestroying()) {
      taMisc::DebugInfo("Attempt to GetSigLink on a destructing object");
      return NULL;
    }
    if (taiViewType* iv = GetTypeDef()->iv) {
      iv->GetSigLink(this, GetTypeDef()); // sets sig_link
    }
  }
  return sig_link();
}

bool taBase::AddSigClient(ISigLinkClient* dlc) {
  // refuse new links while destroying!
  if (isDestroying()) {
    DebugInfo("AddSigClient","Attempt to add a SigLinkClient to a destructing object");
    return false;
  }
  taSigLink* dl = GetSigLink(); // autocreates if necessary
  if (dl != NULL) {
    dl->AddSigClient(dlc);
    return true;
  }
  else {
    DebugInfo("AddSigClient","Attempt to add a SigLinkClient to an obj with no SigLink!");
  }
  return false;
}

bool taBase::RemoveSigClient(ISigLinkClient* dlc) {
  taSigLink* dl = sig_link(); // doesn't autocreate
  if (dl != NULL) {
    return dl->RemoveSigClient(dlc);
  } else return false;
}

void taBase::BatchUpdate(bool begin, bool struc) {
//  taSigLink* dl = sig_link(); // doesn't autocreate -- IMPORTANT!
//  if (!dl) return;
  if (begin) {
    if (struc)
      SigEmit(SLS_STRUCT_UPDATE_BEGIN);
    else
      SigEmit(SLS_DATA_UPDATE_BEGIN);
  } else {
    if (struc)
      SigEmit(SLS_STRUCT_UPDATE_END);
    else
      SigEmit(SLS_DATA_UPDATE_END);
  }
}

void taBase::SmartRef_SigDestroying(taSmartRef* ref, taBase* obj) {
  if(!isDestroying())
    UpdateAfterEdit();
}

String& taBase::ListSigClients(String& strm, int indent) {
  taSigLink* dl = GetSigLink();
  if(!dl) return strm;
  dl->ListClients(strm, indent);
  return strm;
}


///////////////////////////////////////////////////////////////////////////
//      Checking the configuration of objects prior to using them


bool taBase::TestError_impl(bool test, const String& fun_name,
                       const String& a, const String& b, const String& c,
                       const String& d, const String& e, const String& f,
                       const String& g, const String& h) const {
  if(!test) return false;
  return taMisc::TestError_impl(this, test, fun_name, a, b, c, d, e, f, g, h);
}

bool taBase::TestWarning_impl(bool test, const String& fun_name,
                         const String& a, const String& b, const String& c,
                         const String& d, const String& e, const String& f,
                         const String& g, const String& h) const {
  if(!test) return false;
  return taMisc::TestWarning_impl(this, test, fun_name, a, b, c, d, e, f, g, h);
}

void taBase::DebugInfo(const String& fun_name,
                       const String& a, const String& b, const String& c,
                       const String& d, const String& e, const String& f,
                       const String& g, const String& h) const {
  String objinfo = "obj: " + GetTypeDef()->name + " "
    + GetDisplayName() + "::" + fun_name + "() (path: " + DisplayPath() + " )\n";
  taMisc::DebugInfo(objinfo, a, b, c, d, e, f, g, h);
}

void taBase::CheckError_msg(const String& a, const String& b, const String& c,
                            const String& d, const String& e, const String& f,
                            const String& g, const String& h) const {
  String objinfo = "Config Error in: " + GetTypeDef()->name + " "
    + GetDisplayName() + "\npath: " + DisplayPath() + "\n";
  taMisc::CheckError(objinfo, a, b, c, d, e, f, g, h);
  taMisc::check_error_objects.Add(const_cast<taBase*>(this));
  taMisc::check_error_messages.Add(taMisc::SuperCat(a, b, c, d, e, f, g, h, ""));

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
    SigEmitUpdated();
  return (this_rval && child_rval);
}

void taBase::ClearCheckConfig() {
  if (base_flags & INVALID_MASK) {
    ClearBaseFlag(INVALID_MASK);
    SigEmitUpdated();
  }
}

///////////////////////////////////////////////////////////////////////////
//      Copying and changing type

bool taBase::CanDoCopy_impl(const taBase* cp, bool quiet, bool copy) {
  bool ok = true; // ref var needed for Check
  // do the generic guys, that apply in every case
  if (CheckError((!cp), quiet, ok,
    "Copy: source is null")) return false;
  if (cp->InheritsFrom(GetTypeDef())) {
    // we will be doing the copy
    CanCopy_impl(cp, quiet, ok, true);
    if (ok && copy) UnSafeCopy(cp);
  }
  else if (InheritsFrom(cp->GetTypeDef())) {
    // other guy will be doing it
    cp->CanCopy_impl(this, quiet, ok, true);
    if (ok && copy) UnSafeCopy(cp);
  }
  else { // custom
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

bool taBase::CopyFrom(const taBase* cp) {
  // copyfrom is used widely but really Copy has the best semantics and is the one and only
  bool rval = false;
  String saved_name = this->GetName();
  StructUpdate(true); // wrap in struct update so name gets set -- CopyCustom otherwise does it..
  rval = CanDoCopy_impl(cp, false, true);
  this->SetName(saved_name);  
  StructUpdate(false);
  return rval;
  // // this one is easy, since it is really just the same as Copy, but with warnings
  // if (!CanCopy(cpy_from, false)) return false;
  // UnSafeCopy(cpy_from);
  // return true;
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
  src->UpdatePointersAfterCopy_(*this);
  src->StructUpdate(false);
}

void taBase::CopyFromCustom(const taBase* cp_fm) {
  StructUpdate(true);
  SetBaseFlag(COPYING); // note: this is always outer guy, so do it here
    CopyFromCustom_impl(cp_fm);
  ClearBaseFlag(COPYING); // note: this is always outer guy, so do it here
  UpdatePointersAfterCopy_(*cp_fm);
  StructUpdate(false);
}

void taBase::Copy_impl(const taBase& cp) { // note: not a virtual method
  // just the flags
  base_flags = (BaseFlags)((base_flags & ~COPY_MASK) | (cp.base_flags & COPY_MASK));
}

void taBase::Copy_assign(const taBase& cp) { // note: not a virtual method
  // no updating etc at this level -- nothing copied!
}

bool taBase::CanAppend(const taBase* appendee) const {
  return false;
}

// base class does no appending
bool taBase::Append(taBase* appendee) {
  return false;
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
    if(chld->InheritsFrom(&TA_taProject)) {
      String str;
      taProject* rval = NULL;
      ((taBase*)chld)->Save_String(str);
      lst->Load_String(str, NULL, (taBase**)&rval);
      if(rval) {
        rval->SetName(chld->GetName() + "_copy");
      }
      taMisc::ProcessEvents();
      return rval;
    }
    else {
      taBase* rval = lst->DuplicateEl(chld);
      if(rval && taMisc::gui_active) {
        if(!taMisc::in_gui_multi_action &&
           !lst->HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL")) {
          // Bug 2231 - no auto expand on duplicate
          //        tabMisc::DelayedFunCall_gui(rval, "BrowserExpandAll");
          tabMisc::DelayedFunCall_gui(rval, "BrowserSelectMe");
        }
      }
      return rval;
    }
  }
  return NULL;
}

taBase* taBase::DuplicateMe() {
  taBase* own = GetOwner();
  if (TestError((own == NULL), "DuplicateMe", "owner is null")) return NULL;
  if (!own->ChildCanDuplicate(this, false)) return NULL;
  return (own->ChildDuplicate(this));
}


#ifdef TA_GUI
static void tabase_base_closing_all_gp(taBase* obj) {
//  ControlPanel::BaseClosingAll(obj); // get it before it is moved around and stuff
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
//      Type information

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
    if(!md->type->IsActualTaBase() || md->HasNoFind())
      continue;
    taBase* mobj = (taBase*)md->GetOff((void*)this);
    if(mobj->FindCheck(nm) || md->type->InheritsFromName(nm)) {
      ret_md = md;
      return mobj;
    }
  }

  // then do a depth-recursive search
  for(int i=0; i < td->members.size; i++) {
    md = td->members[i];
    if(!md->type->IsActualTaBase() || md->HasNoFind())
      continue;
    taBase* mobj = (taBase*)md->GetOff((void*)this);
    void* rval = mobj->FindMembeR(nm, ret_md);
    if(rval)
      return rval;
  }
  return NULL;
}

///////////// Searching

void taBase::Search(const String& srch, taBase_PtrList& items,
                    taBase_PtrList* owners, bool text_only,
                    bool contains, bool case_sensitive,
                    bool obj_name, bool obj_type,
                    bool obj_desc, bool obj_val,
                    bool mbr_name, bool type_desc) {
  if(TestWarning(srch.empty(), "Search",
                 "search string is empty!")) {
    return;
  }
  String srch_act = srch;
  if(!case_sensitive)
    srch_act.downcase();
  String_Array str_ary;
  str_ary.Split(srch, " ");

  if(SearchTestItem_impl(str_ary, text_only, contains, case_sensitive, obj_name, obj_type,
                         obj_desc, obj_val, mbr_name, type_desc)) {
    items.Link(this);
  }
  SearchIn_impl(str_ary, items, owners, text_only, contains, case_sensitive, obj_name, obj_type,
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

static String SearchGetObjVal_impl(taBase* obj) {
  String strval;
  if(obj->InheritsFrom(&TA_taMatrix)) {
    taMatrix* mat = (taMatrix*)obj;
    if(mat->size < 1000) {  // prevent long delays from ginormous tables
      strval = obj->GetTypeDef()->GetValStr(obj, NULL, NULL,
                                            TypeDef::SC_VALUE, true);
    }
  }
  else {
    // important to use display to avoid path names and just use names of objs
    strval = obj->GetTypeDef()->GetValStr(obj, NULL, NULL,
                                          TypeDef::SC_VALUE, true);
  }
  return strval;
}


// todo: turn this on to get useful debug info about what is matching in a search function
// #define SEARCH_DEBUG 1

bool taBase::SearchTestItem_impl(const String_Array& srch, bool text_only,
                                 bool contains, bool case_sensitive,
                                 bool obj_name, bool obj_type,
                                 bool obj_desc, bool obj_val,
                                 bool mbr_name, bool type_desc,
                                 MemberDef* md) {
  if(srch.size == 0) return false;

#ifdef SEARCH_DEBUG
  String srch_match;
#endif

  bool all_matched = true;    // reset on fail
  for(int s=0; s<srch.size; s++) {
    String sstr = srch[s];
    bool cur_matched = false;
    
    if (text_only) {
      String text = GetDisplayName();
      if (case_sensitive) {
        cur_matched = text.contains(sstr);
      }
      else {
        cur_matched = text.contains_ci(sstr);
      }
    }
    else {  // deep search
      if(sstr.contains('=')) {
        String nm = sstr.before('=');
        String vl = sstr.after('=');
        if(md) {
          cur_matched = SearchTestStr_impl(nm, md->name, contains, case_sensitive);
        }
        else {
          cur_matched = SearchTestStr_impl(nm, GetName(), contains, case_sensitive);
        }
        if(cur_matched) {
          String strval = SearchGetObjVal_impl(this);
#ifdef SEARCH_DEBUG
          taMisc::DebugInfo("nm:", nm, "vl:", vl, "strval:", strval);
#endif
          cur_matched = SearchTestStr_impl(vl, strval, contains, case_sensitive);
#ifdef SEARCH_DEBUG
          if(cur_matched) srch_match = "nm=val: " + nm + "=" + strval;
#endif
        }
      }
      if(!cur_matched && md && mbr_name) {
        cur_matched = SearchTestStr_impl(sstr, md->name, contains, case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "md: " + md->name;
#endif
        if(!cur_matched && type_desc) {
          cur_matched = SearchTestStr_impl(sstr, md->desc, contains, case_sensitive);
#ifdef SEARCH_DEBUG
          if(cur_matched) srch_match = "md desc: " + md->desc;
#endif
        }
      }
      if(!cur_matched && obj_name) {
        cur_matched = SearchTestStr_impl(sstr, GetName(), contains, case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "name: " + GetName();
#endif
      }
      if(!cur_matched && obj_type) {
        cur_matched = SearchTestStr_impl(sstr, GetTypeDef()->name, contains,
                                         case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "type name: " + GetTypeDef()->name;
#endif
      }
      if(!cur_matched && type_desc) {
        cur_matched = SearchTestStr_impl(sstr, GetTypeDef()->desc, contains,
                                         case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "type desc: " + GetTypeDef()->desc;
#endif
      }
      if(!cur_matched && obj_desc) {
        cur_matched = SearchTestStr_impl(sstr, GetDesc(), contains, case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "desc: " + GetDesc();
#endif
      }
      if(!cur_matched && obj_val) {
        String strval = SearchGetObjVal_impl(this);
        cur_matched = SearchTestStr_impl(sstr, strval, contains, case_sensitive);
#ifdef SEARCH_DEBUG
        if(cur_matched) srch_match = "val str: " + strval;
#endif
      }
      if(!cur_matched && mbr_name) {
        TypeDef* td = GetTypeDef();
        for(int m=0;m<td->members.size;m++) {
          MemberDef* tmd = td->members[m];
          if(SearchTestStr_impl(sstr, tmd->name, contains, case_sensitive)) {
            cur_matched = true;
#ifdef SEARCH_DEBUG
            if(cur_matched) srch_match = "type md name: " + tmd->name;
#endif
            break;
          }
          if(type_desc) {
            if(SearchTestStr_impl(sstr, tmd->desc, contains, case_sensitive)) {
              cur_matched = true;
#ifdef SEARCH_DEBUG
              if(cur_matched) srch_match = "type md desc: " + tmd->desc;
#endif
              break;
            }
          }
        }
      }
    }

    if(!cur_matched) {
      all_matched = false;      // conjunctive across search terms
      break;
    }
  }

#ifdef SEARCH_DEBUG
  if(all_matched) {
    taMisc::DebugInfo("search hit in:", DisplayPath(), "md:",
                      (md ? md->name : "NULL"),
                      "from:", srch_match);
  }
#endif

  return all_matched;
}

void taBase::SearchIn_impl(const String_Array& srch, taBase_PtrList& items,
                           taBase_PtrList* owners, bool text_only,
                           bool contains, bool case_sensitive,
                           bool obj_name, bool obj_type,
                           bool obj_desc, bool obj_val,
                           bool mbr_name, bool type_desc) {
  if(srch.size == 0) return;
  TypeDef* td = GetTypeDef();
  int st_sz = items.size;
  
  if (!text_only) {  // if text only we just search the visible text not the members
    // first pass: just look at our guys
    for(int m=0;m<td->members.size;m++) {
      MemberDef* md = td->members[m];
      if(!md->type->IsActualTaBase()) continue;
      if(md->IsInvisible() || md->HasNoFind()) // || md->HasExpert())
        continue;
      taBase* obj = (taBase*)md->GetOff(this);
      if(!obj) continue;
      if(obj->SearchTestItem_impl(srch, text_only, contains, case_sensitive, obj_name, obj_type,
                                  obj_desc, obj_val, mbr_name, type_desc, md)) {
        //      taMisc::DebugInfo("memb search hit in:", DisplayPath(), "md:", md->name);
        items.Link(obj);
      }
    }
  }
  // second pass: recurse
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(!md->type->IsActualTaBase()) continue;
    if(md->IsInvisible() || md->HasNoFind()) // || md->HasExpert())
      continue;
    taBase* obj = (taBase*)md->GetOff(this);
    obj->SearchIn_impl(srch, items, owners, text_only, contains, case_sensitive, obj_name, obj_type,
                       obj_desc, obj_val, mbr_name, type_desc);
  }
  
  if(owners && (items.size > st_sz)) { // we added somebody somewhere..
    owners->Link(this);
  }
}

taBase::ValType taBase::ValTypeForType(TypeDef* td) {
  if (td->IsNotPtr()) {
    if (td->IsBool()) {
      return VT_INT;
    }
    else if(td->IsInt()) {
      // note: char is generic char, and typically we won't use signed char
      if (td->DerivesFrom(TA_char)) {
        return VT_STRING;
      }
      // note: explicit use of signed char is treated like a number
      else if (td->DerivesFrom(TA_unsigned_char))
        return VT_BYTE;
      else {
        return VT_INT;
      }
    }
    else if(td->IsFloat()) {
      if (td->DerivesFrom(TA_double)) {
        return VT_DOUBLE;
      }
      else if (td->DerivesFrom(TA_float)) {
        return VT_FLOAT;
      }
    }
    else if(td->IsEnum()) {
      return VT_STRING;
    }
    else if(td->IsString())
      return VT_STRING;
    else if(td->IsVariant()) {
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
  static String str_bool("bool");
  switch (vt) {
    case VT_STRING: return str_String;
    case VT_DOUBLE: return str_double;
    case VT_FLOAT: return str_float;
    case VT_INT: return str_int;
    case VT_BYTE: return str_byte;
    case VT_VARIANT: return str_Variant;
    case VT_BOOL: return str_bool;
    default: return _nilString; // compiler food
  }
}

///////////////////////////////////////////////////////////////////////////
//      User Data: optional configuration settings for objects

void taBase::CopyUserData(const taBase& cp) {
  UserDataItem_List* our_udl = GetUserDataList(false);
  if(our_udl) {
    our_udl->Reset();
  }
  UserDataItem_List* cp_udl = cp.GetUserDataList(false);
  if(!cp_udl || cp_udl->size == 0) {
    RemoveAllUserData();
    return;
  }
  if(!our_udl) {
    our_udl = GetUserDataList(true);
  }
  if(our_udl) {
    our_udl->Copy(*cp_udl);
  }
}

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
    SigEmit(SLS_USER_DATA_UPDATED);
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
      SigEmit(SLS_USER_DATA_UPDATED);
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
  SigEmit(SLS_USER_DATA_UPDATED);
}

void taBase::PrintMyPath() {
  taMisc::Info(GetPathNames());
}

UserDataItem* taBase::SetUserData(const String& name, const Variant& value,
                                  bool gui_notify_signal)
{
  if (TestError((name.empty()),
    "SetUserData",
    "name must be a valid name")) {
    return NULL;
  }

  UserDataItem_List* ud = GetUserDataList(true);
  if (!ud) return NULL; // not supported, shouldn't be calling

  if(gui_notify_signal && !taTaskThread::inMainThread())
    gui_notify_signal = false;
  
  UserDataItemBase* udi = ud->FindLeafName(name);
  if (udi) {
    if (TestError(!udi->isSimple(),
      "SetUserData",
      "non-simple UserData item with that name already exists!")) {
      return NULL;
    }
  }
  else {
    udi = new UserDataItem;
    udi->SetName(name);
    ud->Add(udi);
  }
  TestWarning(!udi->setValueAsVariant(value),"SetUserData",
              "Attempt to set existing UserData value as Variant, was not supported for",
              name);
  if (gui_notify_signal) {
    SigEmit(SLS_USER_DATA_UPDATED);
  }
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
//      Browser gui

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

String taBase::GetColText(const KeyString& key, int itm_idx) const {
  if (key == key_name) return GetName();
  else if (key == key_type) return GetTypeName();
  else if (key == key_type_desc) return GetTypeDef()->desc;
// note: some classes override desc with dynamic desc's
  else if (key == key_desc) return GetDesc();
  else if (key == key_disp_name) {
    if (GetOwner() && GetOwner()->DisplayDescription()) {
      String rval = GetDisplayName();
      if(GetDesc().nonempty())
        rval +=  " // " + GetDesc();
      return rval;
    }
    else {
      return GetDisplayName();
    }
  }
  else return _nilString;
}

const QVariant taBase::GetColData(const KeyString& key, int role) const {
// these are just the defaults -- later guys can override to trap anything
       if (role == Qt::StatusTipRole) return static_cast<const char *>(statusTip(key));
  else if (role == Qt::ToolTipRole) return static_cast<const char *>(GetToolTip(key));
  else return QVariant();
}

void taBase::BrowseMe() {
  // try to determine whether this is member obj, or not
  taBase* own = GetOwner();
  MemberDef* md = NULL;
  if (own) {
    md = own->FindMemberBase((void*)this);
  }
  MainWindowViewer* wv = MainWindowViewer::NewBrowser(this, md);
  if (wv) wv->ViewWindow();
}

///////////////////////////////////////////////////////////////////////////
//      Edit Dialog gui

bool taBase::Edit() {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (taiEdit *ie = GetTypeDef()->ie) {
    //note: taiEdit looks up color, if hinting enabled
    return ie->Edit((void*)this, false);
  }
#endif
  return false;
}

bool taBase::OpenInWindow(bool modal) {
  if(!taMisc::gui_active) return false;
#ifdef TA_GUI
  if (modal) {
    if (taiEdit *ie = GetTypeDef()->ie) {
      //note: taiEdit looks up color, if hinting enabled
      return ie->EditDialog((void*)this, false, true); // r/w, modal
    }
  }
  else {
    // first, check for an edit dialog and use that if found
    MainWindowViewer* edlg = MainWindowViewer::FindEditDialog(this);
    if(!edlg) {
      edlg = MainWindowViewer::NewEditDialog(this);
    }
    edlg->ViewWindow();
    iMainWindowViewer* iwv = edlg->widget();
    iwv->resize( taiM->dialogSize(taiMisc::dlgBig | taiMisc::dlgVer) );
    return iwv->AssertPanel((taiSigLink*)GetSigLink());
    //bool new_tab, bool new_tab_lock)
  }
#endif
  return false;
}

MainWindowViewer* taBase::GetMyBrowser() {
  taProject* proj = GetMyProj();
  if(!proj) return NULL;
  return proj->GetDefaultProjectBrowser();
}

#ifndef TA_GUI

// **** SEE iMainWindowViewer.cpp for the ifdef TA_GUI implementations ****
bool taBase::EditPanel(bool new_tab, bool pin_tab) {
  return false;
}
bool taBase::BrowserSelectMe() {
  return false;
}
bool taBase::BrowserEditMe() {
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

bool taBase::ChooseMe() {
  return false;
}

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

void taBase::CallFun(const String& fun_name, const String& args_str) {
  MethodDef* md = GetTypeDef()->methods.FindName(fun_name);
  if(md != NULL)
    md->CallFun((void*)this, args_str);
  else
    TestError(true, "CallFun", "function:", fun_name, "not found on object");
}

void taBase::CallObjFun(taBase* obj, const String& fun_name) {
  if(!obj) return;
  obj->CallFun(fun_name);
  obj->UpdateAfterEdit();       // this is a program-level function so make it safe
}

void taBase::SetMemberStr(taBase* obj, const String& memb_name, const String& str) {
  if(!obj) return;
  MemberDef* md = NULL;
  void* mbase = NULL;
  if(memb_name.contains('.')) {
    String parp = memb_name.before('.',-1);
    taBase* mdown = obj->FindFromPath(parp, md);
    if(mdown) {
      String mn = memb_name.after('.',-1);
      md = mdown->GetTypeDef()->members.FindName(mn);
      if(md) mbase = md->GetOff(mdown);
    }
  }
  else {
    md = obj->GetTypeDef()->members.FindName(memb_name);
    if(md) mbase = md->GetOff(obj);
  }
  if(!md) {
    taMisc::Error("SetMemberStr", "member:", memb_name,
                  "not found in object of type:",
                  obj->GetTypeDef()->name);
    return;
  }
  md->type->SetValStr(str, mbase, NULL, md);
}

void taBase::SetMemberVar(taBase* obj, const String& memb_name, const Variant& val) {
  if(!obj) return;
  MemberDef* md = NULL;
  void* mbase = NULL;
  if(memb_name.contains('.')) {
    String parp = memb_name.before('.',-1);
    taBase* mdown = obj->FindFromPath(parp, md);
    if(mdown) {
      String mn = memb_name.after('.',-1);
      md = mdown->GetTypeDef()->members.FindName(mn);
      if(md) mbase = md->GetOff(mdown);
    }
  }
  else {
    md = obj->GetTypeDef()->members.FindName(memb_name);
    if(md) mbase = md->GetOff(obj);
  }
  if(!md) {
    taMisc::Error("SetMemberVar", "member:", memb_name,
                  "not found in object of type:",
                  obj->GetTypeDef()->name);
    return;
  }
  md->type->SetValVar(val, mbase, NULL, md);
}

bool taBase::IsMemberEditable(const String& memb_name) const {
  MemberDef* md = GetTypeDef()->members.FindName(memb_name);
  if(!md) return false;
  return !md->IsGuiReadOnly();
}


Variant taBase::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if (fun_name == "NewColMatrixN_gui" && arg_idx == 2) {
    MatrixGeom* mg = new MatrixGeom(2, 2, 2);
    return mg;
  }
  if(fun_name != "ChangeMyType") return _nilVariant;
  return Variant(GetStemBase()->name); // taiTypeOfTypePtrArgType will convert from String
}

bool taBase::ControlPanelStdItemFilter(void* base_, void* ctrl_panel_) {
  if(!ctrl_panel_) return false;
  ControlPanel* pan = (ControlPanel*)ctrl_panel_;
  if(pan->IsClone()) return false;
  taBase* agp = pan->GetOwner(&TA_ArchivedParams_Group);
  if(agp) return false;         // skip all archived
  return true;
}

bool taBase::ControlPanelNoParamSetItemFilter(void* base_, void* ctrl_panel_) {
  if(!ctrl_panel_) return false;
  ControlPanel* pan = (ControlPanel*)ctrl_panel_;
  if(pan->InheritsFrom(&TA_ParamSet)) return false;
  if(pan->IsClone()) return false;
  taBase* agp = pan->GetOwner(&TA_ArchivedParams_Group);
  if(agp) return false;         // skip all archived
  return true;
}

String taBase::DiffCompareString(taBase* cmp_obj, taDoc*& doc) {
  if(TestError(!cmp_obj, "DiffCompareString", "cmp_obj is null")) return _nilString;
  if(!doc) {
    taProject* proj = GetMyProj();
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

String taBase::GetDiffString() const {
  return BrowserEditString();
}

bool taBase::DiffCompare(taBase* cmp_obj) {
  if(TestError(!cmp_obj, "DiffCompare", "cmp_obj is null")) return false;
  
  ObjDiff* diff = new ObjDiff;
  diff->Diff(this, cmp_obj);      // compute the diffs
  diff->DisplayDialog(false);     // non-modal -- dialog disposes of diff
  return true;
}

bool taBase::AddToControlPanel(MemberDef* member, ControlPanel* ctrl_panel) {
  if(TestError(!member,"AddToControlPanel", "member is null")) return false;
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();
    if(TestError(!proj, "AddToControlPanel", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  return ctrl_panel->AddMemberPrompt(this, member);
}

bool taBase::AddToControlPanelNm
(const String& member, ControlPanel* ctrl_panel, const String& extra_label, const String& sub_gp_nm, const String& desc, bool short_label) {
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();  StructUpdate(true);

    if(TestError(!proj, "AddToControlPanelNm", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  return ctrl_panel->AddMemberNm(this, member, extra_label, desc, sub_gp_nm, short_label);
}

bool taBase::AddFunToControlPanel(MethodDef* function, ControlPanel* ctrl_panel, const String& extra_label) {
  if(TestError(!function, "AddControlFunForEdit", "function is null")) return false;
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();
    if(TestError(!proj, "AddControlFunForEdit", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  return ctrl_panel->AddMethod(this, function, extra_label);
}

bool taBase::AddFunToControlPanelNm
(const String& function, ControlPanel* ctrl_panel, const String& extra_label, const String& sub_gp_nm,  const String& desc) {
  if(!ctrl_panel) {
    taProject* proj = GetMyProj();
    if(TestError(!proj, "AddControlFunForEditNm", "cannot find project")) return false;
    ctrl_panel = (ControlPanel*)proj->ctrl_panels.New(1);
  }
  return ctrl_panel->AddMethodNm(this, function, extra_label, desc, sub_gp_nm);
}

void taBase::GetControlPanelLabel(MemberDef* mbr, String& label, const String& xtra_lbl, bool short_label) const {
  if(xtra_lbl.nonempty()) {
    label = xtra_lbl + "_";
  }
  else if(short_label) {
    label = "";                 // nothing
  }
  else {
    taBase* mo = GetMemberOwner(true); // highest
    if(mo) {
      label = mo->GetName() + GetPath(mo);
    }
    else {
      label = GetDisplayName();
    }
    if (label.nonempty()) {
      label = label.CamelToSnake().elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_LONG); //16
      label += "_";
    }
  }
  label += mbr->GetLabel().CamelToSnake();
  // always do the full member name itself! NOT .elidedTo(taiMisc::CP_ITEM_ELIDE_LENGTH_SHORT);
  label = taMisc::StringCVar(label);
}

void taBase::GetControlPanelDesc(MemberDef* mbr, String& desc) const {
  MemberDef::GetMembDesc(mbr, desc, "");
}

ControlPanel* taBase::MemberControlPanel(const String& member_name, TypeDef* panel_type) const {
  TypeDef* td = GetTypeDef();
  MemberDef* md = td->members.FindName(member_name);
  if(!md) {                     // used in low-level routines, no err msg
    return NULL;
  }
  taSigLink* dl = sig_link();
  if(!dl) {
    return NULL;
  }

  for(int i=0; i<dl->clients_list().size; i++) {
    ISigLinkClient* dlc = dl->clients_list().FastEl(i);
    TypeDef* dlc_typ = dlc->GetTypeDef();
    if(!dlc_typ) continue;
    if(!dlc_typ->InheritsFrom(&TA_taBase_RefList)) continue;
    taBase_RefList* rlist = (taBase_RefList*)dlc->This();
    if(!rlist) continue;
    IRefListClient* lown = rlist->getOwner();
    if(!lown) continue;
    if(!lown->GetTypeDef()->InheritsFrom(&TA_ControlPanel)) continue;
    ControlPanel* pan = (ControlPanel*)lown->This();
    if(!pan || pan->IsArchived()) continue;
    if(panel_type != NULL) {
      if(pan->GetTypeDef() != panel_type) continue;
    }
    int idx = pan->FindMbrBase(this, md);
    if(idx >= 0) {
      return pan;
    }
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////
//      Closing

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
  if (own && own->CloseLater_Child(this)) {
    SetBaseFlag(DESTROYING);      // protect us from further actions
    return;
  }
  tabMisc::DelayedClose(this);  // fallback action - closelater_child is actually the same usu
  SetBaseFlag(DESTROYING);      // protect us from further actions
}

bool taBase::Close_Child(taBase*) {
  return false;
}

bool taBase::CloseLater_Child(taBase*) {
  return false;
}

void taBase::Help() {
#ifdef TA_GUI
  TypeDef* mytd = GetTypeDef();
  iHelpBrowser::StatLoadType(mytd);
#endif
}


///////////////////////////////////////////////////////////////////////////
//      Updating pointers (when objects change type or are copied)

taBase* taBase::UpdatePointers_NewPar_FindNew(taBase* old_guy, taBase* old_par, taBase* new_par) {
  String old_path = old_guy->GetPathNames(old_par);
  MemberDef* md;
  if (!new_par) {
    return NULL;
  }
  taBase* new_guy = new_par->FindFromPath(old_path, md);
  String old_nm = old_guy->GetName();
  bool not_found = !new_guy || (old_nm != new_guy->GetName()) ||
    (old_guy->GetTypeDef() != new_guy->GetTypeDef());
  if(not_found) {
    // try without names, just using indexes, to see if a name change threw us off
    old_path = old_guy->GetPath(old_par);
    new_guy = new_par->FindFromPath(old_path, md);
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
    if(null_not_found) {
      taBase::SetPointer(ptr, NULL);
      if (this->GetName().nonempty()) {
        WarnSettingToNull(old_par, new_par);
      }
    }
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
  
  if (!old_own) { // find the owner
    old_own = (*ptr)->GetOwner();
    if (old_own->GetTypeDef() != new_par->GetTypeDef()) {
      taProject* old_proj = old_own->GetMyProj();
      taProject* new_proj = new_par->GetMyProj();
      // if different project we need to locate new par from path
      if (old_proj != new_proj) {
        taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_proj, new_proj);
        if(new_guy) {
          *ptr = new_guy;
        }
        else {
          if(null_not_found) {
            *ptr = NULL;
            if (this->GetName().nonempty()) {
              WarnSettingToNull(old_proj, new_proj);
            }
          }
          return false;
        }
      }
    }
  }
  
  if(old_own != old_par)
    return false;
  
  taBase* new_guy = UpdatePointers_NewPar_FindNew(*ptr, old_par, new_par);
  if(new_guy) {
    *ptr = new_guy;
  }
  else {
    if(null_not_found) {
      *ptr = NULL;
      if (this->GetName().nonempty()) {
        WarnSettingToNull(old_par, new_par);
      }
    }
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

bool taBase::UpdatePointers_NewPar_SmPtr(taSmartPtr& ref, taBase* old_par, taBase* new_par,
                                         bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) {
    return false;
  }
  if(ref.ptr() == old_par) {
    ref.set(new_par);
    return true;
  }
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if(old_own != old_par) {
    if (old_own && this->GetName().nonempty()) {
      WarnSettingToNull(old_par, new_par);
    }
    return false; // if not scoped in our guy, bail
  }
  
  taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
  if(new_guy) {
    ref.set(new_guy);
  }
  else {
    if(null_not_found) {
      ref.set(NULL);            // reset to null if not found!
      if (this->GetName().nonempty()) {
        WarnSettingToNull(old_par, new_par);
      }
    }
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

bool taBase::UpdatePointers_NewPar_Ref(taSmartRef& ref, taBase* old_par, taBase* new_par,
                                       bool null_not_found) {
  if(!ref.ptr() || !old_par || !new_par) {
    if (ref.ptr() && null_not_found) {
      ref.set(NULL);
      if (this->GetName().nonempty()) {
        WarnSettingToNull(old_par, new_par);
      }
    }
    return false;
  }
  if(ref.ptr() == old_par) {
    ref.set(new_par);
    return true;
  }
  
  taBase* old_own = ref.ptr()->GetOwner(old_par->GetTypeDef());
  if (old_own == old_par || old_own == new_par) {
    taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_par, new_par);
    if(new_guy) {
      ref.set(new_guy);
      // note: this does not call UAE: done later on owner
      return true;
    }
    else {
      if(null_not_found) {
        ref.set(NULL);            // reset to null if not found!
        if (this->GetName().nonempty()) {
          WarnSettingToNull(old_par, new_par);
        }
      }
      return false;
    }
  }
  // keep looking
  bool rval = false;
  bool keep_looking = true;
  while(keep_looking) {
    taBase* old_grand_par = old_par->GetOwner();
    if (!old_grand_par) {
      break;
    }
    // types need to be the same - old could be Program and new ProgEl_list
    taBase* new_grand_par = new_par->GetOwner(old_grand_par->GetTypeDef());
    if (!new_grand_par) {
      break;
    }
    old_own = ref.ptr()->GetOwner(old_grand_par->GetTypeDef());
    if (old_own == old_grand_par) {
      keep_looking = false;
      taBase* new_guy = UpdatePointers_NewPar_FindNew(ref.ptr(), old_grand_par, new_grand_par);
      if(new_guy) {
        ref.set(new_guy);
        rval = true;
      }
      else {
        if(null_not_found) {
          ref.set(NULL);            // reset to null if not found!
          if (this->GetName().nonempty()) {
            WarnSettingToNull(old_par, new_par);
          }
        }
      }
      break;
    }
    old_par = old_grand_par;
    new_par = new_grand_par;
  }
  if (rval == false && null_not_found) {
    ref.set(NULL);
    if (this->GetName().nonempty()) {
      WarnSettingToNull(old_par, new_par);
    }
  }
  return rval;
}

int taBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  // taMisc::Info("uptr_npar:", DisplayPath());
  TypeDef* td = GetTypeDef();
  int nchg = 0;                 // total number changed
  int mychg = 0;                // my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->HasOption("NO_UPDATE_POINTER")) continue;
    if((md->type->IsPointer()) && md->type->IsTaBase() && !md->HasOwnPointer() && 
       (!md->HasReadOnly() || md->HasOption("UPDATE_POINTER"))) {
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasNoSetPointer()) {
        int chg = UpdatePointers_NewPar_PtrNoSet(ptr, old_par, new_par);
        nchg += chg; mychg += chg;
      }
      else {
        int chg = UpdatePointers_NewPar_Ptr(ptr, old_par, new_par);
        nchg += chg; mychg += chg;
      }
    }
    else if(md->type->IsNotPtr()) {
      // uncomment for debug when trying to catch pointer updates on ProgramCall updates -
//      if (md->name == "target") {
//        ;
//      }
      if(md->type->InheritsFrom(TA_taSmartRef)) {
        taSmartRef* ref = (taSmartRef*)md->GetOff(this);
        // Fix for bug 2594 - but changing the default to false might be too much;
        // might need to default to true and figure the conditions for setting to false
        bool null_not_found = false;

        // get list owners
        taList_impl* ref_ptr_list = NULL;
        if (ref && ref->ptr()) {
          ref_ptr_list = (taList_impl*)ref->ptr()->GetOwner(&TA_taList_impl);
        }
        taList_impl* new_par_list = NULL;
        if (new_par) {
          new_par_list = (taList_impl*)new_par->GetOwner(&TA_taList_impl);
        }
        
        // is the object referenced in the same group hierarchy as the new parent group
        if (ref_ptr_list && new_par_list) {
          // now get the root groups
          taGroup_impl* ref_ptr_grp_root = NULL;
          if (ref_ptr_list->InheritsFrom(&TA_taGroup_impl)) {
            ref_ptr_grp_root = (taGroup_impl*)ref_ptr_list;
            if(ref_ptr_grp_root->root_gp)
              ref_ptr_grp_root = ref_ptr_grp_root->root_gp; // go up to root of initial find owner
          }
          taGroup_impl* new_par_grp_root = NULL;
          if (new_par_list->InheritsFrom(&TA_taGroup_impl)) {
            new_par_grp_root = (taGroup_impl*)new_par_list;
            if(new_par_grp_root->root_gp)
              new_par_grp_root = new_par_grp_root->root_gp; // go up to root of initial find owner
          }
          
          // if not the same root group don't try to update pointers
//          if (ref_ptr_grp_root == NULL || (ref_ptr_grp_root != new_par_grp_root)) {
//            null_not_found = false;
//          }
        }
        
        int chg = UpdatePointers_NewPar_Ref(*ref, old_par, new_par, null_not_found);
        nchg += chg; mychg += chg;
      }
      if(md->type->InheritsFrom(TA_taSmartPtr)) {
        taSmartPtr* ref = (taSmartPtr*)md->GetOff(this);
        int chg = UpdatePointers_NewPar_SmPtr(*ref, old_par, new_par);
        nchg += chg; mychg += chg;
      }
      else if(md->type->IsActualTaBase()) {
        taBase* obj = (taBase*)md->GetOff(this);
        nchg += obj->UpdatePointers_NewPar(old_par, new_par); // doesn't count for me
      }
    }
  }
  // if(mychg > 0) {
  //   tabMisc::DelayedUpdateAfterEdit(this); // uae me: i changed
  // }
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
      ref.set(NULL);            // reset to null if not found!
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
    if(null_not_found)
      ref.set(NULL);            // reset to null if not found!
    return false;
  }
  // note: this does not call UAE: assumption is that it is a like-for-like switch..
  return true;
}

int taBase::UpdatePointers_NewParType(TypeDef* par_typ, taBase* new_par) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;                 // total number changed
  int mychg = 0;                // my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->HasOption("NO_UPDATE_POINTER")) continue;
    if((md->type->IsPointer()) && md->type->IsTaBase() && !md->HasOwnPointer() && 
       (!md->HasReadOnly() || md->HasOption("UPDATE_POINTER"))) {
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasNoSetPointer()) {
        int chg = taBase::UpdatePointers_NewParType_PtrNoSet(ptr, par_typ, new_par);
        nchg += chg; mychg += chg;
      }
      else {
        int chg = taBase::UpdatePointers_NewParType_Ptr(ptr, par_typ, new_par);
        nchg += chg; mychg += chg;
      }
    }
    else if(md->type->IsNotPtr()) {
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
      else if(md->type->IsActualTaBase()) {
        taBase* obj = (taBase*)md->GetOff(this);
        nchg += obj->UpdatePointers_NewParType(par_typ, new_par);
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();          // uae me: i changed
  return nchg;
}

/////////////

bool taBase::UpdatePointers_NewObj_Ptr(taBase** ptr, taBase* ptr_owner,
                                       taBase* old_ptr, taBase* new_ptr) {
  if(!*ptr || (*ptr != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  taBase::SetPointer(ptr, new_ptr);
  ptr_owner->UpdateAfterEdit(); // update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_PtrNoSet(taBase** ptr, taBase* ptr_owner,
                                            taBase* old_ptr, taBase* new_ptr) {
  if(!*ptr || (*ptr != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  *ptr = new_ptr;
  ptr_owner->UpdateAfterEdit(); // update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_SmPtr(taSmartPtr& ref, taBase* ptr_owner,
                                         taBase* old_ptr, taBase* new_ptr) {
  if(!ref.ptr() || (ref.ptr() != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  ref.set(new_ptr);
  ptr_owner->UpdateAfterEdit(); // update this guy who owns the pointer
  return true;
}

bool taBase::UpdatePointers_NewObj_Ref(taSmartRef& ref, taBase* ptr_owner,
                                       taBase* old_ptr, taBase* new_ptr) {
  if(!ref.ptr() || (ref.ptr() != old_ptr)) return false;
  if(ptr_owner->GetOwner(old_ptr->GetTypeDef()) == old_ptr) return false;
  // don't replace on children of the old object
  ref.set(new_ptr);
  ptr_owner->UpdateAfterEdit(); // update this guy who owns the pointer
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
    if(omd->type->IsActualTaBase()) {
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
  int nchg = 0;                 // total number changed
  int mychg = 0;                // my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->HasOption("NO_UPDATE_POINTER")) continue;
    if((md->type->IsPointer()) && md->type->IsTaBase() && !md->HasOwnPointer() &&
       (!md->HasReadOnly() || md->HasOption("UPDATE_POINTER"))) {
      taBase** ptr = (taBase**)md->GetOff(this);
      if(md->HasNoSetPointer()) {
        int chg = taBase::UpdatePointers_NewObj_PtrNoSet(ptr, this, old_ptr, new_ptr);
        nchg += chg; mychg += chg;
      }
      else {
        int chg = taBase::UpdatePointers_NewObj_Ptr(ptr, this, old_ptr, new_ptr);
        nchg += chg; mychg += chg;
      }
    }
    else if(md->type->IsNotPtr()) {
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
      else if(md->type->IsActualTaBase()) {
        taBase* obj = (taBase*)md->GetOff(this);
        nchg += obj->UpdatePointers_NewObj(old_ptr, new_ptr);
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();          // uae me: i changed
  return nchg;
}

void taBase::WarnSettingToNull(taBase* old_par, taBase* new_par) {
  taProject* old_proj = old_par->GetMyProj();
  if(!old_proj) return;
  taProject* new_proj = new_par->GetMyProj();
  if (old_proj != new_proj) {
    taMisc::Warning(this->GetName(), " not found in new location - setting to NULL");
  }
}

void taBase::GetArgCompletionList(const String& method, const String& arg, taBase* base_obj, Completions& completions) {
  if (method == "SetMember" && arg == "member") {
    TypeDef* td = GetTypeDef();
    if (td) {
      for (int i=0; i<td->members.size; i++) {
        MemberDef* md = td->members.FastEl(i);
        if (!md->IsGuiReadOnly() && !md->IsEditorHidden()) {
          completions.member_completions.Link(md);
        }
      }
    }
  }
}

// Copyright, 1995-2013, Regents of the University of Colorado,
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
#include <taObjDiff_List>
#include <taDoc>
#include <SelectEdit>
#include <MethodDef>
#include <UserDataItem_List>
#include <UserDataItemBase>
#include <UserDataItem>
#include <taSmartRef>
#include <taSmartPtr>
#include <taList_impl>
#include <taFiler>
#include <taiMimeSource>
#include <ISigLinkClient>
#include <iColor>

#include <DataChangedReason>
#include <taMisc>
#include <tabMisc>
#include <taRootBase>
#include <taSigLink>
#include <TypeDefault>
#include <int_Matrix>
#include <byte_Matrix>
#include <slice_Matrix>
#include <MainWindowViewer>
#include <taiEdit>
#include <ViewColor_List>
#include <dumpMisc>
#include <taObjDiffRec>
#include <taiViewType>
#include <UserData_DocLink>
#include <iMainWindowViewer>
#include <taiMisc>
#include <taiObjDiffBrowser>
#include <iHelpBrowser>

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
    if(md->type->IsActualTaBase()) {
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
    if((md->owner != &(td->members)) || !md->type->IsTaBase()) continue;
    if(md->type->IsNotPtr()) {
      taBase* mb = (taBase*)md->GetOff(this);
      mb->CutLinks();
    }
    else if(md->type->IsPointer() && !md->HasOption("NO_SET_POINTER")) {
      taBase** mb = (taBase**)md->GetOff(this);
      taBase::DelPointer(mb);
    }
  }
}

void taBase::Register() {
  if(!taMisc::not_constr)
    GetTypeDef()->RegisterFinal((void*)this);
  SetBaseFlag(REGISTERED);
}

void taBase::unRegister() {
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
  // its vtable accessor for datalinks will be valid in that context
  taSigLink* dl = data_link();
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
//      Basic object properties: index in list, owner, name, description, etc

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

taBase* taBase::GetThisOrOwner(TypeDef* td) {
  if (InheritsFrom(td)) return this;
  return GetOwner(td);
}

taBase* taBase::GetParent() const       {
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
  if(idx < 0) idx = 0;  // avoid -1 not found: can't just have typedef name for programs etc
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
    if(TestError(cmat->dim(0) < cont_dims, "IndexModeValidate::IDX_COORD",
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
    }
    else {
      if(TestError(cmat->dims() != 2, "IndexModeValidate::IDX_COORDS",
                   "index matrix does not have dims = 2"))
        return false;
      if(TestError(cmat->dim(0) < cont_dims, "IndexModeValidate::IDX_COORDS",
                   "index matrix dim(0):", String(cmat->dim(0)),
                   "is not size of container coordinates:", String(cont_dims)))
        return false;
    }
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
    return ElView()->frames();  // outer dim value
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
    TestError(true, "IterValidate", "view mode must be either IDX_COORDS or IDX_MASK");
    return false;
  }
  return true;
}

void taBase::DelIter(taBaseItr*& itr) const {
  if(!itr) return;
  taBase::unRefDone(itr);
  itr = NULL;
}

int taBase::IterNextIndex(taBaseItr*& itr) const {
  if(IterNext_impl(itr))
    return itr->el_idx;
  DelIter(itr);
  return -1;
}

int taBase::IterFirstIndex(taBaseItr*& itr) const {
  if(IterFirst_impl(itr))
    return itr->el_idx;
  DelIter(itr);
  return -1;
}

bool taBase::IterFirst_impl(taBaseItr*& itr) const {
  if(!itr) return false;
  itr->count = 0;
  itr->el_idx = 0;              // just to be sure
  if(!ElView()) {
    if(ElemCount() > 0) return true;
    return false;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size == 0) {
      return false;
    }
    itr->el_idx = cmat->FastEl_Flat(0); // first guy
    if(itr->el_idx < 0) itr->el_idx += ElemCount();
    if(itr->el_idx < 0 || itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=0; i<ElemCount(); i++) {
      if(cmat->FastEl_Flat(i)) {
        itr->el_idx = i;
        return true;            // byte_matrix guaranteed to be same size as list
      }
    }
  }
  return false;
}

bool taBase::IterNext_impl(taBaseItr*& itr) const {
  if(!itr) return false;
  itr->count++;
  if(!ElView()) {
    itr->el_idx++;
    if(itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  if(ElViewMode() == IDX_COORDS) {
    int_Matrix* cmat = dynamic_cast<int_Matrix*>(ElView());
    if(cmat->size <= itr->count) {
      return false;
    }
    itr->el_idx = cmat->FastEl_Flat(itr->count); // next guy
    if(itr->el_idx < 0) itr->el_idx += ElemCount();
    if(itr->el_idx < 0 || itr->el_idx >= ElemCount()) {
      return false;
    }
    return true;
  }
  else if(ElViewMode() == IDX_MASK) {
    byte_Matrix* cmat = dynamic_cast<byte_Matrix*>(ElView());
    for(int i=itr->el_idx+1; i<ElemCount(); i++) { // search for next
      if(cmat->FastEl_Flat(i)) { // true
        itr->el_idx = i;     // byte_matrix guaranteed to be same size as list
        return true;
      }
    }
  }
  return false;
}

bool taBase::FixSliceValsFromSize(int& start, int& end, int sz) const {
  if(start < 0) start += sz;
  if(end < 0) end += (sz+1); // needs the +1 to allow -1 to be the end and do <
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

  if ((par != NULL) && (GetName() != ""))
    rval += "(" + GetName() + ")";

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
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
    if (ta == NULL) {
      rval = "root";
    }
  }
  else if (this != par_stop) {
    rval = par->GetPath((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
      rval += ".?.";
    }
  }
  return rval;
}

String taBase::GetPathNames(taBase* ta, taBase* par_stop) const {
  if (taMisc::is_undo_saving) return GetPath(ta, par_stop); // use indexes for undo

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
    rval = par->GetPathNames((taBase*)this, par_stop);
  }

  if (ta != NULL) {
    if (MemberDef *md = FindMember(ta)) {
      rval += "." + md->name;
    }
    else if (MemberDef *md = FindMemberPtr(ta)) {
      rval = String("*(") + rval + "." + md->name + ")";
    }
    else {
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
    if(path[start] == '*') {    // path is a pointer
      start += 2;
      ptrflag = true;
    }
    if(path[start] == '.') {    // must be root, so search on next stuff
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
    if(tmp_ptr && (!md || md->type->IsActualTaBase())) { // null md = taBase
      taBase* mbr = (taBase*)tmp_ptr;
      if(delim_pos < length) {  // there's more to be done..
        rval = mbr->FindFromPath(path, ret_md, next_pos); // start from after delim
      }
      else {
        rval = mbr;             // that's all folks..
        ret_md = md;
      }
    }
    else if((el_path == "root") && (delim_pos < length)) {
      start = next_pos; // skip this element since it must be us
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
//      Saving and Loading to/from files

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

String taBase::GetProjDir() const {
  taProject* proj = GET_MY_OWNER(taProject);
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
  DataChanged(DCR_STRUCT_UPDATE_ALL); // during loading, updates are blocked, so now we redo everything
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
  taMisc::save_use_name_paths = false; // default is to NOT use name paths
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
   // DataItemUpdated();
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
                    el_path,"in parent object:",GetPathNames());
    return NULL;
  }
  if(!el_md) {
    taMisc::Warning("*** Dump_Load_Path_ptr: no el_md for item at path:",
                    el_path,"in parent object:",GetPathNames(),
                    "may not set pointers correctly!");
  }
  taBase* nw_el = *nw_el_ptr;

  if(nw_el && (nw_el->GetTypeDef() != ld_el_typ) &&
     !((nw_el->GetTypeDef() == &TA_taBase_List) && (ld_el_typ == &TA_taBase_Group)))
  {
    // object not the right type, try to create new one..
    if(taMisc::verbose_load >= taMisc::MESSAGES) {
      taMisc::Warning("*** Object in parent:",GetPathNames(),"at path", el_path,
                    "of type:",nw_el->GetTypeDef()->name,"is not the right type:",
                      ld_el_typ->name,", attempting to create new one");
    }
    if(el_md && (el_md->HasOption("OWN_POINTER") || !el_md->HasOption("NO_SET_POINTER"))) {
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
                      el_path,"in parent object:",GetPathNames());
      return NULL;
    }
    if(el_md && el_md->HasOption("OWN_POINTER")) { // note: this was not in original!
      taBase::Own(nw_el,this);
    }
    else {
      taMisc::Warning("*** Dump_Load_Path_ptr: NOT owning new element of type:",
                      ld_el_typ->name,"for pointer member at path:",
                      el_path,"in parent object:",GetPathNames());
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
        << ", parent path = " << GetPathNames()
        << ", el_path = " << el_path;
    taMisc::Info(msg);
  }
  return nw_el;
}

taBase* taBase::Dump_Load_Path_parent(const String& el_path, TypeDef* ld_el_typ) {
  MemberDef* el_md = NULL;
  taBase* nw_el = (taBase*)FindMembeR(el_path, el_md);
  if(nw_el) return nw_el;
  taMisc::Warning("*** Dump_Load_Path_parent: Object at path:",GetPathNames(),
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
      return GetPathNames();
    return td->name;
  }
}

String taBase::GetValStr_ptr(const TypeDef* td, const void* base, void* par, MemberDef* memb_def,
                             TypeDef::StrContext sc, bool force_inline) {
  taBase* rbase = *((taBase**)base);
  if(rbase && (rbase->GetOwner() || (rbase == tabMisc::root))) {
    if (sc == TypeDef::SC_STREAMING) {
      return dumpMisc::path_tokens.GetPath(rbase);      // use path tokens when saving..
    }
    else {
      return rbase->GetPathNames();
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
      bs = tabMisc::root->FindFromPath(tmp_val, md);
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

int taBase::ReplaceValStr(const String& srch, const String& repl, const String& mbr_filt,
                          void* par, TypeDef* par_typ, MemberDef* memb_def, TypeDef::StrContext sc) {
  TypeDef* td = GetTypeDef();
  int rval = td->ReplaceValStr_class(srch, repl, mbr_filt, this, par, par_typ, memb_def, sc);
  if(rval > 0)
    UpdateAfterEdit();
  return rval;
}

taObjDiffRec* taBase::GetObjDiffVal(taObjDiff_List& odl, int nest_lev, MemberDef* memb_def,
                           const void* par, TypeDef* par_typ, taObjDiffRec* par_od) const {
  // always just add a record for this guy
  taObjDiffRec* odr = new taObjDiffRec(odl, nest_lev, GetTypeDef(), memb_def, (void*)this,
                                       (void*)par, par_typ, par_od);
  odl.Add(odr);
  if(GetOwner()) {
    odr->tabref = new taBaseRef;
    ((taBaseRef*)odr->tabref)->set((taBase*)this);
  }

  GetTypeDef()->GetObjDiffVal_class(odl, nest_lev, this, memb_def, par, par_typ, odr);
  return odr;
}

///////////////////////////////////////////////////////////////////////////
//      Updating of object properties

void taBase::UpdateAfterEdit() {
  if (isDestroying()) return;
  UpdateAfterEdit_impl();
  DataItemUpdated();
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
  //  DataItemUpdated();  no extra notify -- list takes care of it.  should
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
  taSigLink* dl = data_link();
  if (dl) dl->DataDataChanged(dcr, op1, op2);
}

void taBase::DataItemUpdated() {
  DataChanged(DCR_ITEM_UPDATED);
}

bool taBase::InStructUpdate() {
  taSigLink* dl = data_link(); // doesn't autocreate
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

///////////////////////////////////////////////////////////////////////////
//      Data Links -- notify other guys when you change

taSigLink* taBase::GetDataLink() {
  if (!data_link()) {
    if (isDestroying()) {
      taMisc::DebugInfo("Attempt to GetDataLink on a destructing object");
      return NULL;
    }
    if (taiViewType* iv = GetTypeDef()->iv) {
      iv->GetDataLink(this, GetTypeDef()); // sets data_link
    }
  }
  return data_link();
}

bool taBase::AddDataClient(ISigLinkClient* dlc) {
  // refuse new links while destroying!
  if (isDestroying()) {
    DebugInfo("AddDataClient","Attempt to add a DataLinkClient to a destructing object");
    return false;
  }
  taSigLink* dl = GetDataLink(); // autocreates if necessary
  if (dl != NULL) {
    dl->AddDataClient(dlc);
    return true;
  }
  else {
    DebugInfo("AddDataClient","Attempt to add a DataLinkClient to an obj with no DataLink!");
  }
  return false;
}

bool taBase::RemoveDataClient(ISigLinkClient* dlc) {
  taSigLink* dl = data_link(); // doesn't autocreate
  if (dl != NULL) {
    return dl->RemoveDataClient(dlc);
  } else return false;
}

void taBase::BatchUpdate(bool begin, bool struc) {
//  taSigLink* dl = data_link(); // doesn't autocreate -- IMPORTANT!
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

String& taBase::ListDataClients(String& strm, int indent) {
  taSigLink* dl = GetDataLink();
  if(!dl) return strm;
  dl->ListClients(strm, indent);
  return strm;
}


///////////////////////////////////////////////////////////////////////////
//      Checking the configuration of objects prior to using them


bool taBase::TestError(bool test, const char* fun_name,
                       const char* a, const char* b, const char* c,
                       const char* d, const char* e, const char* f,
                       const char* g, const char* h) const {
  if(!test) return false;
  return taMisc::TestError(this, test, fun_name, a, b, c, d, e, f, g, h);
}

bool taBase::TestWarning(bool test, const char* fun_name,
                         const char* a, const char* b, const char* c,
                         const char* d, const char* e, const char* f,
                         const char* g, const char* h) const {
  if(!test) return false;
  return taMisc::TestWarning(this, test, fun_name, a, b, c, d, e, f, g, h);
}

void taBase::DebugInfo(const char* fun_name,
                       const char* a, const char* b, const char* c,
                       const char* d, const char* e, const char* f,
                       const char* g, const char* h) const {
  String objinfo = "obj: " + GetTypeDef()->name + " "
    + GetDisplayName() + "::" + fun_name + "() (path: " + GetPathNames() + ")\n";
  taMisc::DebugInfo(objinfo, a, b, c, d, e, f, g, h);
}

void taBase::CheckError_msg(const char* a, const char* b, const char* c,
                            const char* d, const char* e, const char* f,
                            const char* g, const char* h) const {
  String objinfo = "Config Error in: " + GetTypeDef()->name + " "
    + GetDisplayName() + " (path: " + GetPathNames() + ")\n";
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
    DataItemUpdated();
  return (this_rval && child_rval);
}

void taBase::ClearCheckConfig() {
  if (base_flags & INVALID_MASK) {
    ClearBaseFlag(INVALID_MASK);
    DataItemUpdated();
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

bool taBase::CopyFrom(taBase* cp) {
  // copyfrom is used widely but really Copy has the best semantics and is the one and only
  return CanDoCopy_impl(cp, false, true);
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
      if(!taMisc::in_gui_multi_action &&
         !lst->HasOption("NO_EXPAND_ALL") && !rval->HasOption("NO_EXPAND_ALL")) {
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
    if(!md->type->IsActualTaBase() || md->HasOption("NO_FIND"))
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
    if(!md->type->IsActualTaBase() || md->HasOption("NO_FIND"))
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
    if(md->type->IsNotPtr()) {
      if(md->type->IsActualTaBase()) {
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
    if(md->type->IsActualTaBase()) {
      taBase* obj = (taBase*)md->GetOff(this);
      obj->Search_impl(srch, items, owners,contains, case_sensitive, obj_name, obj_type,
                       obj_desc, obj_val, mbr_name, type_desc);
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
    if(md->type->IsAnyPtr() || !md->type->IsActualTaBase()) continue;
    if(md->type->HasOption("EDIT_INLINE") || md->type->HasOption("INLINE")) continue;
    if(md->HasOption("HIDDEN")) continue; // categorically don't look at hidden objects for diffs
    taBase* obj = (taBase*)md->GetOff(this);
    taBase* cp_obj = (taBase*)md->GetOff(cp_base);
    obj->CompareSameTypeR(mds, base_types, trg_bases, src_bases,
                          cp_obj, show_forbidden, show_allowed, no_ptrs);
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
//      User Data: optional configuration settings for objects

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
  taMisc::Info(GetPathNames());
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

bool taBase::EditDialog(bool modal) {
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
    return iwv->AssertPanel((taiSigLink*)GetDataLink());
    //bool new_tab, bool new_tab_lock)
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

Variant taBase::GetGuiArgVal(const String& fun_name, int arg_idx) {
  if(fun_name != "ChangeMyType") return _nilVariant;
  return Variant(GetStemBase()->name); // taiTypeOfTypePtrArgType will convert from String
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
  if(TestError(!cmp_obj, "DiffCompare", "cmp_obj is null")) return false;

  taObjDiff_List* diffs = new taObjDiff_List;
  diffs->CreateSrcs();
  diffs->src_a->tab_obj_a = this; // root of paths!
  diffs->src_b->tab_obj_a = cmp_obj; // root of paths!
  diffs->tab_obj_a = this;
  diffs->tab_obj_b = cmp_obj;

  GetObjDiffVal(*(diffs->src_a), 0);
  cmp_obj->GetObjDiffVal(*(diffs->src_b), 0);

  diffs->Diff();

  taiObjDiffBrowser* odb = taiObjDiffBrowser::New(diffs, taiMisc::defFontSize);
  bool rval = odb->Browse();

  // browser is not modal and will return immediately
  // browser now owns the diffs list and will delete it when it dies -- and it will
  // be responsible for performing any actions that get done..

  // if(rval) {
  //   DoDiffEdits(diffs);
  // }

  return true;
}

static void DoDiffEdits_SetRelPath(taBase* par_obj, taObjDiffRec* srec, taObjDiffRec* drec) {
  MemberDef* md;
  taBase* new_guy = par_obj->FindFromPath(srec->value, md);
  if(drec->type->IsPointer() && drec->type->IsTaBase()) {
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
    if(rec->type->IsActualTaBase()) {
      if(rec->tabref) {
        if(!((taBaseRef*)rec->tabref)->ptr())
          continue;
        tab_a = (taBase*)rec->addr;
      }
      if(rec->diff_odr) {
        if(rec->diff_odr->tabref) {
          if(!((taBaseRef*)rec->diff_odr->tabref)->ptr())
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
    else if(((rec->type->IsPointer()) && rec->type->IsTaBase()) ||
            rec->type->InheritsFrom(TA_taSmartRef) ||
            rec->type->InheritsFrom(TA_taSmartPtr)) {
      taptr = true;
    }

    taBase* tab_par_a = NULL;
    taBase* tab_par_b = NULL;
    if(rec->par_type && rec->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->par_odr && rec->par_odr->tabref) {
        if(!((taBaseRef*)rec->par_odr->tabref)->ptr())
          continue;
        tab_par_a = (taBase*)rec->par_addr;
      }
    }
    if(rec->diff_odr && rec->diff_odr->par_type &&
       rec->diff_odr->par_type->IsActualTaBase()) {
      // make sure *parent* pointer is still current
      if(rec->diff_odr->par_odr && rec->diff_odr->par_odr->tabref) {
        if(!((taBaseRef*)rec->diff_odr->par_odr->tabref)->ptr())
          continue;
        tab_par_b = (taBase*)rec->diff_odr->par_addr;
      }
    }

    String tab_a_path;
    String tab_b_path;
    if(tab_par_a) tab_a_path = tab_par_a->GetPathNames();
    if(tab_par_b) tab_b_path = tab_par_b->GetPathNames();

    //////////////////////////////////
    //          Copy -- both ways -- uses value string instead of live obj

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB) &&
       rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      // this is unfortunate but possible
      taMisc::Info("Copying A -> B:", tab_a_path, "->", tab_b_path, "\n",
                   rec->diff_odr->GetDisplayName(), "=", rec->value);
      taMisc::Info("Copying B -> A:", tab_b_path, "->", tab_a_path, "\n",
                   rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        taBase* down = tab_b->GetOwner();
        if(down) {              // should always be true
          down->CopyChildBefore(tab_a, tab_b);
        }

        taBase* sown = tab_a->GetOwner();
        if(sown) {              // should always be true
          sown->CopyChildBefore(tab_b, tab_a);
        }

        tab_b->Close(); // nuke old guys
        tab_a->Close(); // nuke old guys
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
        if(tab_par_b) {
          tab_par_b->MemberUpdateAfterEdit(rec->diff_odr->mdef);
          tab_par_b->UpdateAfterEdit();
        }

        if(rec->diff_odr->HasDiffFlag(taObjDiffRec::VAL_PATH_REL)) {
          DoDiffEdits_SetRelPath(diffs.tab_obj_a, rec->diff_odr, rec);
        }
        else if(taptr) {
          DoDiffEdits_SetRelPath(proj_a, rec->diff_odr, rec); // always project relative
        }
        else {
          rec->type->SetValStr(rec->diff_odr->value, rec->addr, rec->par_addr, rec->mdef);
        }
        if(tab_par_a) {
          tab_par_a->MemberUpdateAfterEdit(rec->mdef);
          tab_par_a->UpdateAfterEdit();
        }
      }
      continue;
    }

    //////////////////////////////////
    //          Copy -- only one way

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_AB)) {
      taMisc::Info("Copying A -> B:", tab_a_path, "->", tab_b_path, "\n",
                   rec->diff_odr->GetDisplayName(), "=", rec->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        taBase* down = tab_b->GetOwner();
        if(down) {
          down->CopyChildBefore(tab_a, tab_b);
          tab_b->Close();       // nuke old guy
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
        if(tab_par_b) {
          tab_par_b->MemberUpdateAfterEdit(rec->diff_odr->mdef);
          tab_par_b->UpdateAfterEdit();
        }
      }
      continue;
    }

    if(rec->HasDiffFlag(taObjDiffRec::ACT_COPY_BA)) {
      taMisc::Info("Copying B -> A:", tab_b_path, "->", tab_a_path, "\n",
                   rec->GetDisplayName(), "=", rec->diff_odr->value);
      if(tab_diff_typ) {
        // need to replace old guy with new one
        taBase* down = tab_a->GetOwner();
        if(down) {
          down->CopyChildBefore(tab_b, tab_a);
          tab_a->Close();       // nuke old guy
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
        if(tab_par_a) {
          tab_par_a->MemberUpdateAfterEdit(rec->mdef);
          tab_par_a->UpdateAfterEdit();
        }
      }
      continue;
    }

    if(!ta_bases) continue;     // only ta bases from this point on!

    //////////////////////////////////
    //          Add
    bool add = false;
    bool added = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_A)) { // do add before del..
      taMisc::Info("Adding A to B:", tab_a_path, "\n", rec->GetDisplayName());
      add = true;
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_ADD_B)) { // do add before del..
      taMisc::Info("Adding B to A:", tab_b_path, "\n", rec->GetDisplayName());
      add = true;
    }
    if(add) {
      if(rec->diff_odr->nest_level < rec->nest_level) {
        // for last obj in list, dest is now another member in parent obj..
//      taMisc::Info("diff nest -- rec:", String(rec->nest_level), "diff:",
//                   String(rec->diff_odr->nest_level),"rec path:", tab_a->GetPathNames(),
//                   "diff path:", tab_b->GetPathNames());
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
              if(dmd && dmd->type->IsActualTaBase()) { // it should!
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
              if(dmd && dmd->type->IsActualTaBase()) { // it should!
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
      else {                    // equal nesting!
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
    //          Del

    if(rec->tabref && !((taBaseRef*)rec->tabref)->ptr()) continue;
    // double-check

    bool del = false;
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_A)) {
      taMisc::Info("Deleting A:", tab_a_path, "\n", rec->GetDisplayName());
      del = true;
    }
    if(!rec->mdef && rec->HasDiffFlag(taObjDiffRec::ACT_DEL_B)) {
      taMisc::Info("Deleting B:", tab_b_path, "\n", rec->GetDisplayName());
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
    editor->DataItemUpdated(); // so name updates in treee
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
    if(md->type->IsNotPtr()) {
      if(md->type->IsActualTaBase()) {
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

void taBase::GetSelectText(MemberDef* mbr, String xtra_lbl,
                           String& full_lbl, String& desc) const {
  if (xtra_lbl.empty())
    xtra_lbl = GetName().elidedTo(16);
  String lbl = xtra_lbl;
  if (lbl.nonempty()) lbl += "_";
  lbl += mbr->GetLabel();
  full_lbl = taMisc::StringCVar(lbl);
  // desc is the member description
//   if (desc.empty())
//     MemberDef::GetMembDesc(mbr, desc, "");
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
}


///////////////////////////////////////////////////////////////////////////
//      Updating pointers (when objects change type or are copied)

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
      ref.set(NULL);            // reset to null if not found!
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
      ref.set(NULL);            // reset to null if not found!
    return false;
  }
  // note: this does not call UAE: done later on owner
  return true;
}

int taBase::UpdatePointers_NewPar(taBase* old_par, taBase* new_par) {
  TypeDef* td = GetTypeDef();
  int nchg = 0;                 // total number changed
  int mychg = 0;                // my actual guys changed
  for(int m=0;m<td->members.size;m++) {
    MemberDef* md = td->members[m];
    if(md->is_static) continue;
    if((md->type->IsPointer()) && md->type->IsTaBase() &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
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
    else if(md->type->IsNotPtr()) {
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
      else if(md->type->IsActualTaBase()) {
        taBase* obj = (taBase*)md->GetOff(this);
        nchg += obj->UpdatePointers_NewPar(old_par, new_par); // doesn't count for me
      }
    }
  }
  if(mychg > 0)
    UpdateAfterEdit();          // uae me: i changed
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
    if(md->is_static) continue;
    if((md->type->IsPointer()) && md->type->IsTaBase() &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
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
    if((md->type->IsPointer()) && md->type->IsTaBase() &&
       !md->HasOption("OWN_POINTER") && !md->HasOption("NO_UPDATE_POINTER") &&
       (!md->HasOption("READ_ONLY") || md->HasOption("UPDATE_POINTER"))) {
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

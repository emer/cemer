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

#ifndef taSmartRef_h
#define taSmartRef_h 1

// parent includes:
#include <IDataLinkClient>

// member includes:
#include <taBase>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taDataLink; // 


class TA_API taSmartRef: protected IDataLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##SMART_POINTER safe reference for taBase objects -- does not ref count, but is a data link client so it tracks changes and automatically sets ptr to NULL when object dies
friend class taBase;
friend class TypeDef; // for various
friend class MemberDef; // for streaming
friend class taDataView; // for access to link
public:
  inline taBase*        ptr() const {return m_ptr;}
  void                  set(taBase* src) {if (src == m_ptr) return;
    if (m_ptr) {m_ptr->RemoveDataClient(this);
      //note: important to wait to get mptr in case RDC indirectly deleted it
      taBase* t = m_ptr; m_ptr = NULL; DataRefChanging(t, false);}
    if (src && src->AddDataClient(this))
      {m_ptr = src; DataRefChanging(m_ptr, true);} }

  virtual TypeDef*      GetBaseType() const {return &TA_taBase;}
  taBase*               GetOwner() const { return m_own; }

  inline                operator bool() const {return (m_ptr);}
    // needed to avoid ambiguities when we have derived T* operators
  inline bool           operator !() const {return !(m_ptr);}
  inline                operator taBase*() const {return m_ptr;}
  inline taBase*        operator->() const {return m_ptr;}
  taBase*               operator=(const taSmartRef& src)
    {set(src.m_ptr); return m_ptr;}
    //NOTE: copy only implies ptr, NOT the owner!
  taBase*               operator=(taBase* src) {set(src); return m_ptr;}

  inline void           Init(taBase* own_) {m_own = own_;} // call in owner's Initialize or InitLinks
  inline void           CutLinks() {set(NULL); m_own = NULL;}
  taSmartRef() {m_own = NULL; m_ptr = NULL;}
  ~taSmartRef() {CutLinks();}

protected:
  taBase*               m_own;
  mutable taBase*       m_ptr;

  void                  DataRefChanging(taBase* obj, bool setting);

private:
  taSmartRef(const taSmartRef& src); // not defined

public: // ITypedObject interface
  override void*        This() {return (void*)this;} //
  override TypeDef*     GetTypeDef() const {return &TA_taSmartRef;} //note: only one typedef for all

public: // IDataLinkClient interface
  override TypeDef*     GetDataTypeDef() const
    {return (m_ptr) ? m_ptr->GetTypeDef() : &TA_taBase;} // TypeDef of the data
  override void         DataDataChanged(taDataLink*, int dcr, void* op1, void* op2);
  override void         DataLinkDestroying(taDataLink* dl);
};

#endif // taSmartRef_h

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
#include <ISigLinkClient>

// member includes:
#include <taBase>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taSigLink; // 


taTypeDef_Of(taSmartRef);

class TA_API taSmartRef: protected ISigLinkClient {
  // ##NO_INSTANCE ##NO_TOKENS ##SMART_POINTER safe reference for taBase objects -- does not ref count, but is a data link client so it tracks changes and automatically sets ptr to NULL when object dies
friend class taBase;
friend class TypeDef; // for various
friend class MemberDef; // for streaming
friend class taDataView; // for access to link
public:
  inline taBase*        ptr() const {return m_ptr;}
  void                  set(taBase* src) {if (src == m_ptr) return;
    if (m_ptr) {m_ptr->RemoveSigClient(this);
      //note: important to wait to get mptr in case RDC indirectly deleted it
      taBase* t = m_ptr; m_ptr = NULL; SigChanging(t, false);}
    if (src && src->AddSigClient(this))
      {m_ptr = src; SigChanging(m_ptr, true);} }

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

  void                  SigChanging(taBase* obj, bool setting);

private:
  taSmartRef(const taSmartRef& src); // not defined

public: // ITypedObject interface
  void*        This() CPP11_OVERRIDE {return (void*)this;} //
  TypeDef*     GetTypeDef() const CPP11_OVERRIDE {return &TA_taSmartRef;} //note: only one typedef for all

public: // ISigLinkClient interface
  TypeDef*     GetDataTypeDef() const CPP11_OVERRIDE
    {return (m_ptr) ? m_ptr->GetTypeDef() : &TA_taBase;} // TypeDef of the data
  void         SigLinkRecv(taSigLink*, int sls, void* op1, void* op2) CPP11_OVERRIDE;
  void         SigLinkDestroying(taSigLink* dl) CPP11_OVERRIDE;
};

#endif // taSmartRef_h

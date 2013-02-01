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

#ifndef tabDefChildRef_h
#define tabDefChildRef_h 1

// parent includes:
#include <ISigLinkClient>

// member includes:
#include <taList_impl>

// declare all other types mentioned but not required to include:
class TypeDef; // 
class taSigLink; // 
class tabDefChildTreeDataNode; // 


TypeDef_Of(tabDefChildRef);

class TA_API tabDefChildRef: protected ISigLinkClient { // ##NO_INSTANCE ##NO_TOKENS "safe" reference for taList objects -- does not ref count, but is a dlc so it tracks changes etc.
public:
  inline taList_impl*   ptr() const {return m_ptr;}
  void                  set(taList_impl* src) {if (src == m_ptr) return;
    if (m_ptr) m_ptr->RemoveSigClient(this); m_ptr = NULL;
    if (src && src->AddSigClient(this)) m_ptr = src;}

  void                  Init(tabDefChildTreeDataNode* own_, taList_impl* ptr_)
    {m_own = own_; set(ptr_);}
  tabDefChildRef() {m_own = NULL; m_ptr = NULL;}
  ~tabDefChildRef() {set(NULL);}

protected:
  tabDefChildTreeDataNode*      m_own;
  mutable taList_impl*  m_ptr;

public: // ITypedObject interface
  override void*        This() {return (void*)this;} //
  override TypeDef*     GetTypeDef() const {return &TA_tabDefChildRef;}

public: // ISigLinkClient interface
  override TypeDef*     GetDataTypeDef() const
    {return (m_ptr) ? m_ptr->GetTypeDef() : &TA_taList_impl;}
  override void         SigLinkRecv(taSigLink*, int sls, void* op1, void* op2);
  override void         SigLinkDestroying(taSigLink* dl);
};

#endif // tabDefChildRef_h

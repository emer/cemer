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

#ifndef TypeInfoBrowseViewer_h
#define TypeInfoBrowseViewer_h 1

// parent includes:
#include <BrowseViewer>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MemberDef; // 
class taiSigLink; // 


TypeDef_Of(TypeInfoBrowseViewer);

class TA_API TypeInfoBrowseViewer : public BrowseViewer {
  // #NO_TOKENS represents a class browser instance
INHERITED(BrowseViewer)
public:
  static TypeInfoBrowseViewer*     New(void* root, TypeDef* root_typ, MemberDef* root_md = NULL);
    // accepts any TypeItem or XxxSpace guy

  String                root_str; // #READ_ONLY #NO_SHOW a string version, so we can stream the guy

  override void*        root() {return m_root;}
  void                  setRoot(void* root, TypeDef* root_typ, MemberDef* root_md = NULL); // use this to set the root
  override taiSigLink* rootLink()
    {return (m_root) ? (taiSigLink*)((TypeItem*)m_root)->sig_link : NULL;}

  void  UpdateAfterEdit();
  TA_DATAVIEWFUNS(TypeInfoBrowseViewer, BrowseViewer) //
protected:
  void*                 m_root; // #IGNORE

  override IDataViewWidget* ConstrWidget_impl(QWidget* gui_parent); // #IGNORE
  void                  StrToRoot();
  void                  RootToStr();
private:
  void  Copy_(const TypeInfoBrowseViewer& cp);
  void                  Initialize();
  void                  Destroy() {}
};

#endif // TypeInfoBrowseViewer_h

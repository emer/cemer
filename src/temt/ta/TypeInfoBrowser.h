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

#ifndef TypeInfoBrowser_h
#define TypeInfoBrowser_h 1

// parent includes:
#include <BrowseViewer>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MemberDef; // 
class taiSigLink; // 


taTypeDef_Of(TypeInfoBrowser);

class TA_API TypeInfoBrowser : public BrowseViewer {
  // #NO_TOKENS represents a class browser instance
INHERITED(BrowseViewer)
public:
  static TypeInfoBrowser*     New(void* root, TypeDef* root_typ, MemberDef* root_md = NULL);
    // accepts any TypeItem or XxxSpace guy

  String                root_str; // #READ_ONLY #NO_SHOW a string version, so we can stream the guy

  void*        root() CPP11_OVERRIDE {return m_root;}
  void                  setRoot(void* root, TypeDef* root_typ, MemberDef* root_md = NULL); // use this to set the root
  taiSigLink* rootLink() CPP11_OVERRIDE
    {return (m_root) ? (taiSigLink*)((TypeItem*)m_root)->sig_link : NULL;}

  void  UpdateAfterEdit();
  TA_DATAVIEWFUNS(TypeInfoBrowser, BrowseViewer) //
protected:
  void*                 m_root; // #IGNORE

  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) CPP11_OVERRIDE; // #IGNORE
  void                  StrToRoot();
  void                  RootToStr();
private:
  void  Copy_(const TypeInfoBrowser& cp);
  void                  Initialize();
  void                  Destroy() {}
};

#endif // TypeInfoBrowser_h

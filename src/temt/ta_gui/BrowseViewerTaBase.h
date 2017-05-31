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

#ifndef BrowseViewerTaBase_h
#define BrowseViewerTaBase_h 1

// parent includes:
#include <BrowseViewer>

// member includes:
#include <taSmartRef>

// declare all other types mentioned but not required to include:
class taBase; // 
class MemberDef; // 
class taiSigLink; // 


taTypeDef_Of(BrowseViewerTaBase);

class TA_API BrowseViewerTaBase : public BrowseViewer {
  // #NO_TOKENS #AKA_tabBrowseViewer for browsing when root is a taBase
INHERITED(BrowseViewer)
friend class iDataBrowser;
public:
  static BrowseViewerTaBase*       New(taBase* root, MemberDef* md = NULL);
    // convenience function

  taSmartRef            m_root;

  void*        root() override {return (void*)m_root.ptr();}
  taiSigLink* rootLink()  override {return (m_root) ?
    (taiSigLink*)m_root->GetSigLink() : NULL;}

  void  UpdateAfterEdit() override; // if root deletes, our window must die
  void  InitLinks() override;
  void  CutLinks() override;
  TA_DATAVIEWFUNS(BrowseViewerTaBase, BrowseViewer) //

protected:
  IViewerWidget* ConstrWidget_impl(QWidget* gui_parent) override; // #IGNORE


private:
  void  Copy_(const BrowseViewerTaBase& cp);
  void                  Initialize() {}
  void                  Destroy() {CutLinks();}
};

#endif // BrowseViewerTaBase_h

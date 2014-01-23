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

#ifndef BrowseViewer_h
#define BrowseViewer_h 1

// parent includes:
#include <FrameViewer>

// member includes:

// declare all other types mentioned but not required to include:
class TypeDef; // 
class MemberDef; // 
class iBrowseViewer; // #IGNORE
class taiSigLink; // 


taTypeDef_Of(BrowseViewer);

class TA_API BrowseViewer : public FrameViewer {
  // #NO_TOKENS ##DEF_NAME_ROOT_Tree ##DEF_NAME_STYLE_2 the base type for browser frames (tree of objects or classes)
INHERITED(FrameViewer)
friend class iDataBrowser;
public:

  TypeDef*              root_typ; // type of the root item
  MemberDef*            root_md; // if the root item is a member, NULL otherwise

  virtual void*         root() {return NULL;} // subtype must supply
  inline TypeDef*       rootType() {return root_typ;}
  inline MemberDef*     rootMemb() {return root_md;}
  virtual taiSigLink*  rootLink() {return NULL;}

  inline iBrowseViewer* widget() {return (iBrowseViewer*)inherited::widget();}
  // #IGNORE

  TA_DATAVIEWFUNS(BrowseViewer, FrameViewer) //
protected:
  void         Render_pre() CPP11_OVERRIDE; //
private:
  void  Copy_(const BrowseViewer& cp);
  void                  Initialize();
  void                  Destroy() {CutLinks();}
};

#endif // BrowseViewer_h

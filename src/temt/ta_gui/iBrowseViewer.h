// Copyright 2013-2018, Regents of the University of Colorado,
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

#ifndef iBrowseViewer_h
#define iBrowseViewer_h 1

// parent includes:
#include <iFrameViewer>

// member includes:
#include <BrowseViewer>
#include <BuiltinTypeDefs>

// declare all other types mentioned but not required to include:
class iTreeView; //
class iTreeViewItem; //
class iTreeSearch; //
class ISelectable_PtrList; //
class taiWidgetActions; //


class TA_API iBrowseViewer : public iFrameViewer { // base of viewer window used for object and class browsing
Q_OBJECT
INHERITED(iFrameViewer)
public:

  iTreeView*            lvwDataTree;
  iTreeSearch*          lvwTreeSearch;

  inline BrowseViewer*  browser() {return (BrowseViewer*)m_viewer;}
  void*                 root() {return (browser()) ? browser()->root() : NULL;}
  TypeDef*              root_typ() {return (browser()) ? browser()->root_typ : &TA_void;}
  MemberDef*            root_md() {return (browser()) ? browser()->root_md : NULL;} //
  taiSigLink*           rootLink() {return (browser()) ? browser()->rootLink() : NULL;}
  virtual int           stretchFactor() const override {return 1;} //  1/2 default

  void                  Reset();
  virtual void          ApplyRoot(); // #IGNORE actually applies the new root value

  iBrowseViewer(BrowseViewer* browser_, QWidget* parent = 0);
  ~iBrowseViewer();

public slots:
  virtual void          mnuBrowseNodeDrop(int param) {mnuBrowseNodeDrop_param = param;}
    // called from within the node->dropped event

protected slots:
  virtual void          lvwDataTree_FillContextMenuHookPost(ISelectable_PtrList& sel_items, taiWidgetActions* menu);

protected: // IViewerWidget i/f
  void         Refresh_impl() override;

protected:
  int                   mnuBrowseNodeDrop_param;
    // param from the mnuBrowseDrop slot -- called by a node, only valid for its call
  int                   cur_expand_depth; // holds expand level for each group as we iterate through sub items of objs, types, etc

private:
  void                  Init();
};

#endif // iBrowseViewer_h
